#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#include "Core.h"
#include "Combat.h"

#ifdef _WIN32
#include <windows.h>
#endif

/* ======================================================
   COLORS
   ====================================================== */
#define RED   "\033[91m"
#define GRN   "\033[92m"
#define YEL   "\033[93m"
#define BLU   "\033[94m"
#define PRP   "\033[95m"
#define CYN   "\033[96m"
#define WHT   "\033[97m"
#define BOLD  "\033[1m"
#define RST   "\033[0m"

/* ======================================================
   SIM EFFECT IDS (NO NEW SYSTEMS, JUST CONSTANTS)
   ====================================================== */
#define EFX_STUN    1001
#define EFX_MARK    1002
#define EFX_WITHER  1003
#define EFX_SURGE   1004

/* ======================================================
   WITHER — tracked separately since entity has no
   embedded combat field. core_combat lives alongside
   core_entity, not inside it.
   ====================================================== */
typedef struct {
    core_bool  active;
    core_i32   stacks;
    core_f32   rate;    /* hp lost per tick as ratio of max_hp */
} sim_wither;

/* ======================================================
   HP BAR
   ====================================================== */
static void hp_bar(core_i32 hp, core_i32 max_hp) {
    int filled = (max_hp > 0) ? (hp * 24 / max_hp) : 0;
    const char* col = hp > max_hp * 0.5f ? GRN :
                      hp > max_hp * 0.2f ? YEL : RED;
    printf("%s[", col);
    for (int i = 0; i < 24; i++)
        printf(i < filled ? "\xe2\x96\x88" : "\xe2\x96\x91");
    printf("]" RST " %d/%d", hp, max_hp);
}

/* ======================================================
   POOL HELPERS (SAME STYLE, JUST FUNCTIONS)
   ====================================================== */
static void add_effect(core_entity* e, core_u32 effect_id, core_u32 source, core_i32 stacks, core_u32 duration) {
    if (e->effect_pool.count >= CORE_EFFECT_POOL_MAX)
        return;

    core_u32 i = e->effect_pool.count++;
    e->effect_pool.effects[i].id       = effect_id;
    e->effect_pool.effects[i].source   = source;
    e->effect_pool.effects[i].stacks   = stacks;
    e->effect_pool.effects[i].duration = duration;
}

static void remove_effect_by_id(core_entity* e, core_u32 effect_id) {
    core_u32 write = 0;
    for (core_u32 i = 0; i < e->effect_pool.count; i++) {
        if (e->effect_pool.effects[i].id == effect_id)
            continue;
        e->effect_pool.effects[write++] = e->effect_pool.effects[i];
    }
    e->effect_pool.count = write;
}

/* ======================================================
   ENTITY CARD
   ====================================================== */
static void print_card(const char* name,
                        core_entity* e,
                        sim_wither*  w,
                        const char*  col)
{
    printf("\n%s%s\xe2\x95\x94\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90"
           "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90"
           "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90"
           "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90"
           "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90"
           "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90"
           "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90"
           "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90"
           "\xe2\x95\x97%s\n", BOLD, col, RST);
    printf("%s%s| %-30s|%s\n", BOLD, col, name, RST);
    printf("%s%s|------------------------------|%s\n", BOLD, col, RST);

    /* HP */
    printf("%s|%s HP:    ", col, RST);
    hp_bar(e->stats.hp, e->stats.max_hp);
    printf("\n");

    /* Stats */
    printf("%s|%s STR:%-4d  DEF:%-4d  SPD:%-4d\n",
           col, RST,
           e->stats.strength, e->stats.defense, e->stats.speed);

    printf("%s|%s ATK:%-4d  DOD:%-4d  TEN:%-4d\n",
           col, RST,
           e->stats.attack, e->stats.dodge, e->stats.tenacity);

    /* Effort bar */
    printf("%s|%s EFF:  ", col, RST);
    hp_bar(e->stats.effort, 100);
    printf("\n");

    /* State flags */
    printf("%s|%s ", col, RST);
    if (e->state & CORE_CAN_MOVE)     printf(GRN  "MOVE "    RST);
    if (e->state & CORE_CAN_ATTACK)   printf(GRN  "ATK "     RST);
    if (e->state & CORE_CAN_DEFEND)   printf(GRN  "DEF "     RST);
    if (e->state & CORE_IS_STUNNED)   printf(RED  "STUN "    RST);
    if (e->state & CORE_IS_FROZEN)    printf(BLU  "FREEZE "  RST);
    if (e->state & CORE_INVULNERABLE) printf(PRP  "INVULN "  RST);
    if (e->state & CORE_SILENCED)     printf(YEL  "SILENCE " RST);
    if (e->state & CORE_IS_ON_FIRE)   printf(RED  "FIRE "    RST);
    if (e->state & CORE_LOCKED)       printf(YEL  "MARKED "  RST);
    printf("\n");

    /* Zeals */
    if (e->zeal_pool.count > 0) {
        printf("%s|%s ZEAL: ", col, RST);
        for (core_u32 i = 0; i < e->zeal_pool.count; i++)
            printf(PRP "[zeal:%u x%d] " RST,
                   e->zeal_pool.slots[i].id,
                   e->zeal_pool.slots[i].stacks);
        printf("\n");
    }

    /* Effects */
    if (e->effect_pool.count > 0) {
        printf("%s|%s EFX:  ", col, RST);
        for (core_u32 i = 0; i < e->effect_pool.count; i++)
            printf(YEL "[eff:%u x%d dur:%u] " RST,
                   e->effect_pool.effects[i].id,
                   e->effect_pool.effects[i].stacks,
                   e->effect_pool.effects[i].duration);
        printf("\n");
    }

    /* Wither */
    if (w && w->active)
        printf("%s|%s " RED "WITHER: %d stacks (%.0f%% drain/tick)" RST "\n",
               col, RST, w->stacks, w->rate * 100.0f);

    printf("%s%s\xe2\x95\x9a\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90"
           "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90"
           "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90"
           "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90"
           "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90"
           "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90"
           "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90"
           "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90"
           "\xe2\x95\x9d%s\n", BOLD, col, RST);
}

/* ======================================================
   WAIT FOR INPUT
   ====================================================== */
static void wait(void) {
    printf(CYN "\n  [ Press ENTER to continue... ]" RST);
    while (getchar() != '\n');
}

/* ======================================================
   APPLY STEP — only uses real primitives from combat.h
   ====================================================== */
static void apply_step(core_ability_step* step,
                        core_entity* source,
                        core_entity* target,
                        sim_wither*  target_wither,
                        const char*  target_name)
{
    switch (step->type) {

        case CORE_PRIMITIVE_MODIFY:
            printf("  " CYN "[MODIFY]" RST
                   " stat:%u  delta:%d  -> %s\n",
                   step->params.modify.stat_id,
                   step->params.modify.value,
                   target_name);

            /* stat_id 1 = hp */
            if (step->params.modify.stat_id == 1) {
                target->stats.hp += step->params.modify.value;
                if (target->stats.hp < 0)
                    target->stats.hp = 0;
                if (target->stats.hp > target->stats.max_hp)
                    target->stats.hp = target->stats.max_hp;
            }

            /* stat_id 4 = defense */
            if (step->params.modify.stat_id == 4)
                target->stats.defense += step->params.modify.value;

            /* stat_id 9 = effort */
            if (step->params.modify.stat_id == 9) {
                target->stats.effort += step->params.modify.value;
                if (target->stats.effort < 0)   target->stats.effort = 0;
                if (target->stats.effort > 100) target->stats.effort = 100;
            }
            break;

        case CORE_PRIMITIVE_LOCK:
            printf("  " BLU "[LOCK]" RST " stun -> %s\n", target_name);

            target->state |=  CORE_IS_STUNNED;
            target->state &= ~CORE_CAN_MOVE;
            target->state &= ~CORE_CAN_ATTACK;

            add_effect(target, EFX_STUN, source->id.index, 1, 2);
            break;

        case CORE_PRIMITIVE_PIERCE:
            printf("  " RED "[PIERCE]" RST
                   " bypassing defense -> %s\n", target_name);

            target->stats.defense = 0;

            if (target->state & CORE_INVULNERABLE) {
                target->state &= ~CORE_INVULNERABLE;
                printf("       " RED "INVULNERABLE stripped!\n" RST);
            }
            break;

        case CORE_PRIMITIVE_MARK:
            printf("  " PRP "[MARK]" RST
                   " mark:%u tagged on %s\n",
                   step->params.mark.mark_id,
                   target_name);

            target->state |= CORE_LOCKED;
            add_effect(target, EFX_MARK, source->id.index, 1, step->params.mark.duration.current);
            break;

        case CORE_PRIMITIVE_RESTORE:
            printf("  " GRN "[RESTORE]" RST
                   " restoring %s +%.0f%% bonus\n",
                   target_name,
                   step->params.restore.bonus_ratio * 100.0f);

            {
                core_i32 base = target->stats.max_hp;
                core_i32 bonus = (core_i32)(base * step->params.restore.bonus_ratio);
                target->stats.hp = base + bonus;

                if (target->stats.hp > target->stats.max_hp)
                    target->stats.hp = target->stats.max_hp;
            }

            /* clear stun on self-restore */
            target->state &= ~CORE_IS_STUNNED;
            target->state |= CORE_CAN_MOVE | CORE_CAN_ATTACK;

            remove_effect_by_id(target, EFX_STUN);
            break;

        case CORE_PRIMITIVE_GATE:
            printf("  " YEL "[GATE]" RST
                   " condition:%u  threshold:%u checked\n",
                   step->params.gate.condition_id,
                   step->params.gate.threshold);
            break;

        case CORE_PRIMITIVE_CONVERT:
            printf("  " PRP "[CONVERT]" RST
                   " type %u -> %u on %s\n",
                   step->params.convert.from_type,
                   step->params.convert.to_type,
                   target_name);
            break;

        case CORE_PRIMITIVE_SURGE:
            printf("  " YEL "[SURGE]" RST
                   " stat:%u x%.1f on %s\n",
                   step->params.surge.stat_id,
                   step->params.surge.multiplier,
                   target_name);

            if (step->params.surge.stat_id == 3) {
                target->stats.strength = (core_i32)(
                    target->stats.strength * step->params.surge.multiplier);
            }

            add_effect(target, EFX_SURGE, source->id.index, 1, step->params.surge.duration.current);
            break;

        /* MODIFY + LOCK = wither in this sim */
        case CORE_PRIMITIVE_LINK:
            printf("  " RED "[LINK->WITHER]" RST
                   " corruption link applied to %s\n", target_name);

            if (target_wither) {
                target_wither->active = true;
                target_wither->stacks++;
                target_wither->rate = 0.04f * target_wither->stacks;
            }

            add_effect(target, EFX_WITHER, source->id.index, 1, 5);
            break;

        default:
            printf("  " YEL "[PRIMITIVE %u]" RST " fired on %s\n",
                   step->type, target_name);
            break;
    }
}

/* ======================================================
   EXECUTE ABILITY
   ====================================================== */
static void execute_ability(const char*         ability_name,
                             core_ability_entry* ab,
                             core_entity*        source,
                             core_entity*        target,
                             sim_wither*         target_wither,
                             const char*         target_name)
{
    printf("\n" BOLD WHT
           "  >> %s\n" RST, ability_name);

    for (core_u32 i = 0; i < ab->step_count; i++)
        apply_step(&ab->steps[i], source, target, target_wither, target_name);
}

/* ======================================================
   TICK — wither drains, effects tick down
   ====================================================== */
static void tick(const char* name, core_entity* e, sim_wither* w) {

    /* wither drain */
    if (w && w->active) {
        core_i32 loss = (core_i32)(e->stats.max_hp * w->rate);
        if (loss < 1) loss = 1;
        e->stats.hp -= loss;
        if (e->stats.hp < 0) e->stats.hp = 0;

        printf(RED "  [WITHER]" RST " %s loses %d hp (stacks:%d)\n",
               name, loss, w->stacks);
    }

    /* effect pool tick down + expire handling */
    core_u32 write = 0;
    for (core_u32 i = 0; i < e->effect_pool.count; i++) {

        core_effect_instance fx = e->effect_pool.effects[i];

        if (fx.duration > 0) {
            fx.duration--;

            if (fx.duration == 0) {

                printf(YEL "  [EFFECT EXPIRED]" RST
                       " eff:%u on %s\n",
                       fx.id, name);

                /* cleanup based on known sim ids */
                if (fx.id == EFX_STUN) {
                    e->state &= ~CORE_IS_STUNNED;
                    e->state |= CORE_CAN_MOVE | CORE_CAN_ATTACK;
                }

                if (fx.id == EFX_MARK) {
                    e->state &= ~CORE_LOCKED;
                }

                if (fx.id == EFX_WITHER) {
                    if (w) {
                        w->stacks--;
                        if (w->stacks <= 0) {
                            w->stacks = 0;
                            w->active = false;
                            w->rate = 0.0f;
                        } else {
                            w->rate = 0.04f * w->stacks;
                        }
                    }
                }

                continue;
            }
        }

        e->effect_pool.effects[write++] = fx;
    }

    e->effect_pool.count = write;

    /* effort refund */
    if (e->stats.effort > 0)
        e->stats.effort = (core_i32)(e->stats.effort * 0.85f);
}

/* ======================================================
   PLAYER ACTION MENU
   ====================================================== */
static int player_menu(void) {
    printf("\n" BOLD GRN "  YOUR TURN — Choose an action:\n" RST);
    printf(GRN "  [1]" RST " Basic Strike       (MODIFY hp -30)\n");
    printf(GRN "  [2]" RST " Rally              (RESTORE self +10%%)\n");
    printf(GRN "  [3]" RST " Chain with Sea Bud (GATE + MODIFY x2)\n");
    printf(GRN "  [4]" RST " Surge Strike       (SURGE str + MODIFY)\n");
    printf(GRN "  [5]" RST " Guard              (LOCK self immune 1 tick)\n");
    printf(GRN "  [6]" RST " Skip turn\n");
    printf(CYN "  > " RST);

    int choice = 0;
    scanf("%d", &choice);
    while (getchar() != '\n');
    return choice;
}

/* ======================================================
   SEA BUD AI
   ====================================================== */
static void sea_bud_turn(core_entity* sea_bud, core_entity* boss, sim_wither* boss_wither) {

    core_ability_entry act = {0};

    int r = rand() % 3;

    if (r == 0) {
        printf(BOLD BLU "\n  SEA BUD uses TIDAL MARK!\n" RST);

        act.step_count = 2;

        act.steps[0].type                      = CORE_PRIMITIVE_MARK;
        act.steps[0].params.mark.target        = boss->id.index;
        act.steps[0].params.mark.mark_id       = 7;
        act.steps[0].params.mark.duration.current = 3;

        act.steps[1].type                      = CORE_PRIMITIVE_MODIFY;
        act.steps[1].params.modify.target      = boss->id.index;
        act.steps[1].params.modify.stat_id     = 1;
        act.steps[1].params.modify.value       = -25;

        execute_ability("TIDAL MARK", &act, sea_bud, boss, boss_wither, "BOSS");
    }

    if (r == 1) {
        printf(BOLD BLU "\n  SEA BUD uses QUICK SLASH!\n" RST);

        act.step_count = 1;

        act.steps[0].type                  = CORE_PRIMITIVE_MODIFY;
        act.steps[0].params.modify.target  = boss->id.index;
        act.steps[0].params.modify.stat_id = 1;
        act.steps[0].params.modify.value   = -20;

        execute_ability("QUICK SLASH", &act, sea_bud, boss, boss_wither, "BOSS");
    }

    if (r == 2) {
        printf(BOLD BLU "\n  SEA BUD uses DEFENSE SONG!\n" RST);

        act.step_count = 1;

        act.steps[0].type                  = CORE_PRIMITIVE_MODIFY;
        act.steps[0].params.modify.target  = sea_bud->id.index;
        act.steps[0].params.modify.stat_id = 4;
        act.steps[0].params.modify.value   = 10;

        execute_ability("DEFENSE SONG", &act, sea_bud, sea_bud, NULL, "SEA BUD");
    }
}

/* ======================================================
   BOSS AI
   ====================================================== */
static void boss_turn(core_entity* boss, core_entity* player, sim_wither* player_wither) {

    core_ability_entry act = {0};

    int r = rand() % 4;

    if (r == 0) {
        printf(BOLD RED "\n  BOSS uses WITHER SLAM!\n" RST);

        act.step_count = 3;

        act.steps[0].type                         = CORE_PRIMITIVE_PIERCE;
        act.steps[0].params.pierce.target         = player->id.index;
        act.steps[0].params.pierce.what_to_pierce = 4;

        act.steps[1].type                    = CORE_PRIMITIVE_MODIFY;
        act.steps[1].params.modify.target    = player->id.index;
        act.steps[1].params.modify.stat_id   = 1;
        act.steps[1].params.modify.value     = -55;

        act.steps[2].type                    = CORE_PRIMITIVE_LINK;
        act.steps[2].params.link.entity_a    = boss->id.index;
        act.steps[2].params.link.entity_b    = player->id.index;

        execute_ability("WITHER SLAM", &act, boss, player, player_wither, "PLAYER");
    }

    if (r == 1) {
        printf(BOLD RED "\n  BOSS uses CRUSHING LOCK!\n" RST);

        act.step_count = 2;

        act.steps[0].type                    = CORE_PRIMITIVE_LOCK;
        act.steps[0].params.lock.stat_id      = 0;
        act.steps[0].params.lock.duration.current = 2;

        act.steps[1].type                    = CORE_PRIMITIVE_MODIFY;
        act.steps[1].params.modify.target    = player->id.index;
        act.steps[1].params.modify.stat_id   = 1;
        act.steps[1].params.modify.value     = -35;

        execute_ability("CRUSHING LOCK", &act, boss, player, player_wither, "PLAYER");
    }

    if (r == 2) {
        printf(BOLD RED "\n  BOSS uses BRUTAL STRIKE!\n" RST);

        act.step_count = 1;

        act.steps[0].type                    = CORE_PRIMITIVE_MODIFY;
        act.steps[0].params.modify.target    = player->id.index;
        act.steps[0].params.modify.stat_id   = 1;
        act.steps[0].params.modify.value     = -45;

        execute_ability("BRUTAL STRIKE", &act, boss, player, player_wither, "PLAYER");
    }

    if (r == 3) {
        printf(BOLD RED "\n  BOSS uses MARKED EXECUTION!\n" RST);

        act.step_count = 2;

        act.steps[0].type                      = CORE_PRIMITIVE_MARK;
        act.steps[0].params.mark.target        = player->id.index;
        act.steps[0].params.mark.mark_id       = 9;
        act.steps[0].params.mark.duration.current = 2;

        act.steps[1].type                      = CORE_PRIMITIVE_MODIFY;
        act.steps[1].params.modify.target      = player->id.index;
        act.steps[1].params.modify.stat_id     = 1;
        act.steps[1].params.modify.value       = -40;

        execute_ability("MARKED EXECUTION", &act, boss, player, player_wither, "PLAYER");
    }
}

/* ======================================================
   MAIN
   ====================================================== */
int main(void) {

    #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    #endif
    
    srand((unsigned)time(NULL));

    printf(BOLD CYN
           "\n  ================================\n"
           "     BEFORE / AFTER ENGINE\n"
           "       BATTLE SIMULATION\n"
           "  ================================\n" RST);

    wait();

    /* ---- SPAWN ---- */
    core_entity player = {0};
    player.id              = (core_entity_id){1, 1};
    player.stats.hp        = 120;
    player.stats.max_hp    = 120;
    player.stats.strength  = 35;
    player.stats.attack    = 30;
    player.stats.defense   = 40;
    player.stats.speed     = 15;
    player.stats.dodge     = 20;
    player.stats.tenacity  = 25;
    player.stats.effort    = 80;
    player.state           = CORE_CAN_MOVE | CORE_CAN_ATTACK | CORE_CAN_DEFEND;

    core_entity sea_bud = {0};
    sea_bud.id             = (core_entity_id){2, 1};
    sea_bud.stats.hp       = 85;
    sea_bud.stats.max_hp   = 85;
    sea_bud.stats.strength = 22;
    sea_bud.stats.attack   = 28;
    sea_bud.stats.defense  = 15;
    sea_bud.stats.speed    = 24;
    sea_bud.stats.dodge    = 35;
    sea_bud.stats.effort   = 90;
    sea_bud.state          = CORE_CAN_MOVE | CORE_CAN_ATTACK | CORE_CAN_DEFEND;

    core_entity boss = {0};
    boss.id                = (core_entity_id){99, 1};
    boss.stats.hp          = 500;
    boss.stats.max_hp      = 500;
    boss.stats.strength    = 80;
    boss.stats.attack      = 75;
    boss.stats.defense     = 60;
    boss.stats.speed       = 8;
    boss.stats.effort      = 100;
    boss.state             = CORE_CAN_ACT | CORE_INVULNERABLE;

    sim_wither player_wither  = {false, 0, 0.0f};
    sim_wither sea_bud_wither = {false, 0, 0.0f};
    sim_wither boss_wither    = {false, 0, 0.0f};

    /* ---- INITIAL STATE ---- */
    printf(BOLD "\n  [ INITIAL STATE ]\n" RST);
    print_card("PLAYER",  &player,  &player_wither,  GRN);
    print_card("SEA BUD", &sea_bud, &sea_bud_wither, BLU);
    print_card("BOSS",    &boss,    &boss_wither,     RED);
    wait();

    /* ======================================================
       BATTLE LOOP
       ====================================================== */
    int round = 1;
    while (round <= 10) {

        printf(BOLD CYN "\n  ================================\n" RST);
        printf(BOLD CYN "           ROUND %d\n" RST, round);
        printf(BOLD CYN "  ================================\n" RST);

        /* -------------------------------
           BOSS TURN
           ------------------------------- */
        printf(BOLD RED "\n  TURN — BOSS\n" RST);

        if (boss.stats.hp > 0) {
            boss_turn(&boss, &player, &player_wither);
        }

        if (player.stats.hp <= 0) {
            printf(RED "\n  PLAYER HAS FALLEN.\n" RST);
            break;
        }

        wait();

        /* -------------------------------
           SEA BUD TURN
           ------------------------------- */
        printf(BOLD BLU "\n  TURN — SEA BUD\n" RST);

        if (sea_bud.stats.hp > 0) {
            sea_bud_turn(&sea_bud, &boss, &boss_wither);
        }

        if (boss.stats.hp <= 0) {
            printf(GRN "\n  BOSS HAS FALLEN.\n" RST);
            break;
        }

        wait();

        /* -------------------------------
           PLAYER TURN
           ------------------------------- */
        printf(BOLD GRN "\n  TURN — PLAYER\n" RST);

        print_card("PLAYER",  &player,  &player_wither,  GRN);
        print_card("BOSS",    &boss,    &boss_wither,     RED);

        if (player.state & CORE_IS_STUNNED) {
            printf(RED "\n  PLAYER IS STUNNED AND LOSES TURN!\n" RST);
        } else {

            int choice = player_menu();

            core_ability_entry player_action = {0};
            player_action.id = 3;

            if (choice == 1) {
                printf(GRN "\n  Basic Strike!\n" RST);
                player_action.step_count = 1;
                player_action.steps[0].type                  = CORE_PRIMITIVE_MODIFY;
                player_action.steps[0].params.modify.target  = boss.id.index;
                player_action.steps[0].params.modify.stat_id = 1;
                player_action.steps[0].params.modify.value   = -30;

                execute_ability("BASIC STRIKE", &player_action,
                                &player, &boss, &boss_wither, "BOSS");
            }

            if (choice == 2) {
                printf(GRN "\n  Rally!\n" RST);
                player_action.step_count = 1;
                player_action.steps[0].type                         = CORE_PRIMITIVE_RESTORE;
                player_action.steps[0].params.restore.target        = player.id.index;
                player_action.steps[0].params.restore.bonus_ratio   = 0.10f;

                execute_ability("RALLY", &player_action,
                                &player, &player, &player_wither, "PLAYER");
            }

            if (choice == 3) {
                printf(GRN "\n  Chain Strike!\n" RST);
                player_action.step_count = 3;

                player_action.steps[0].type                         = CORE_PRIMITIVE_GATE;
                player_action.steps[0].params.gate.condition_id     = 1;
                player_action.steps[0].params.gate.threshold        = 1;

                player_action.steps[1].type                         = CORE_PRIMITIVE_MODIFY;
                player_action.steps[1].params.modify.target         = boss.id.index;
                player_action.steps[1].params.modify.stat_id        = 1;
                player_action.steps[1].params.modify.value          = -45;

                player_action.steps[2].type                         = CORE_PRIMITIVE_MODIFY;
                player_action.steps[2].params.modify.target         = boss.id.index;
                player_action.steps[2].params.modify.stat_id        = 1;
                player_action.steps[2].params.modify.value          = -35;

                printf(PRP "  [BOND: Sea Bud + Player chain fires]\n" RST);

                execute_ability("CHAIN STRIKE", &player_action,
                                &player, &boss, &boss_wither, "BOSS");
            }

            if (choice == 4) {
                printf(GRN "\n  Surge Strike!\n" RST);
                player_action.step_count = 2;

                player_action.steps[0].type                        = CORE_PRIMITIVE_SURGE;
                player_action.steps[0].params.surge.stat_id        = 3;
                player_action.steps[0].params.surge.multiplier     = 1.5f;
                player_action.steps[0].params.surge.duration.current = 2;

                player_action.steps[1].type                        = CORE_PRIMITIVE_MODIFY;
                player_action.steps[1].params.modify.target        = boss.id.index;
                player_action.steps[1].params.modify.stat_id       = 1;
                player_action.steps[1].params.modify.value         = (core_i32)(-player.stats.strength * 1.2f);

                execute_ability("SURGE STRIKE", &player_action,
                                &player, &boss, &boss_wither, "BOSS");
            }

            if (choice == 5) {
                printf(GRN "\n  Guard!\n" RST);
                player.state |= CORE_INVULNERABLE;
                add_effect(&player, 9999, player.id.index, 1, 1);
                printf(PRP "  [GUARD]" RST " PLAYER becomes INVULNERABLE for 1 tick\n");
            }

            if (choice == 6) {
                printf(YEL "\n  Turn skipped.\n" RST);
            }
        }

        if (boss.stats.hp <= 0) {
            printf(GRN "\n  BOSS HAS FALLEN.\n" RST);
            break;
        }

        wait();

        /* ======================================================
           END OF ROUND TICK
           ====================================================== */
        printf(BOLD RED "\n  [ END OF ROUND — TICK ]\n" RST);

        tick("PLAYER",  &player,  &player_wither);
        tick("SEA BUD", &sea_bud, &sea_bud_wither);
        tick("BOSS",    &boss,    &boss_wither);

        /* guard effect removal (id 9999) */
        remove_effect_by_id(&player, 9999);
        player.state &= ~CORE_INVULNERABLE;

        wait();

        /* ======================================================
           ROUND STATE
           ====================================================== */
        printf(BOLD CYN "\n  [ ROUND STATE ]\n" RST);
        print_card("PLAYER",  &player,  &player_wither,  GRN);
        print_card("SEA BUD", &sea_bud, &sea_bud_wither, BLU);
        print_card("BOSS",    &boss,    &boss_wither,     RED);

        if (player.stats.hp <= 0 || boss.stats.hp <= 0)
            break;

        round++;
        wait();
    }

    /* ======================================================
       FINAL VERDICT
       ====================================================== */
    printf(BOLD CYN "\n  [ FINAL VERDICT ]\n" RST);

    if (boss.stats.hp <= 0)
        printf(GRN "  BOSS DEFEATED.\n" RST);
    else
        printf(YEL "  Boss at %d hp — still standing.\n" RST, boss.stats.hp);

    if (player.stats.hp <= 0)
        printf(RED "  PLAYER DOWN.\n" RST);
    else if (player_wither.active)
        printf(RED "  Player alive but WITHERING — cleanse needed.\n" RST);
    else
        printf(GRN "  Player standing clean.\n" RST);

    printf(BOLD CYN
           "\n  ================================\n"
           "       SIMULATION COMPLETE\n"
           "  ================================\n" RST);

    return 0;
}