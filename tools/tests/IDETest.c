// ============================================================
//  BA Language IDE  —  Raylib C Implementation
//  Features: multi-tab, syntax highlight, smart summary,
//            diagnostics, hover tooltips, 4 themes, scrolling
//  Requires: raylib 4.0+  (raylib.h + libraylib)
//  Build:    gcc ba_ide.c -o ba_ide -lraylib -lm
// ============================================================

#include "raylib.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

// ── Window ───────────────────────────────────────────────────
#define WIN_W    1200
#define WIN_H    700

// ── Layout ───────────────────────────────────────────────────
#define TITLE_H  30
#define TAB_H    36
#define STATUS_H 22
#define LN_W     52
#define SIDE_W   272
#define FS       14
#define LH       22
#define PT       5
#define PL       10

// ── Editor dims ──────────────────────────────────────────────
#define ED_X   (LN_W)
#define ED_Y   (TITLE_H + TAB_H)
#define ED_W   (WIN_W - LN_W - SIDE_W)
#define ED_H   (WIN_H - TITLE_H - TAB_H - STATUS_H)
#define PNL_X  (WIN_W - SIDE_W)
#define PNL_Y  (TITLE_H + TAB_H)
#define PNL_H  (WIN_H - TITLE_H - TAB_H - STATUS_H)

// ── Limits ───────────────────────────────────────────────────
#define MAX_LINES  512
#define LINE_LEN   512
#define MAX_TABS   12
#define MAX_ERRS   32
#define MAX_TOKS   256
#define MAX_SUMM   14
#define MAX_PARTS  8
#define PART_LEN   128
#define ERR_LEN    128
#define TIP_LEN    128

// ─────────────────────────────────────────────────────────────
//  TOKEN SYSTEM
// ─────────────────────────────────────────────────────────────

typedef enum {
    TK_STRUCT, TK_STAT, TK_VERB, TK_MOD, TK_CMP,
    TK_KW, TK_NUM, TK_PCT, TK_CHC, TK_BRC,
    TK_COL, TK_CMT, TK_WS, TK_PLN
} TKType;

typedef struct { TKType t; char v[128]; } Token;
typedef struct { int n; Token tok[MAX_TOKS]; } LineToks;

static const char *L_STRUCT[] = {"character","ability","when","rule",0};
static const char *L_STAT[]   = {"hp","strength","defense","dodge","speed","attack","tenacity","health",0};
static const char *L_VERB[]   = {"mark","reduce","increase","enter","restore","pierce","steal","convert","lock","surge","echo","sense",0};
static const char *L_MOD[]    = {"first","then","also",0};
static const char *L_CMP[]    = {"below","above","at",0};
static const char *L_CHC[]    = {"always","sometimes","often","rarely","never",0};
static const char *L_KW[]     = {"it","its","is","are","an","a","the","stats","cost","costs","effort","by","with","level","mode","chance","and","or","not","in","of","for","their","into","rage","fire","buff","debuff","enemy","ally","name",0};

static bool InList(const char **list, const char *w) {
    for (; *list; list++) if (strcmp(*list, w) == 0) return true;
    return false;
}

static TKType ClassifyWord(const char *w) {
    char lw[128]; int i = 0;
    while (w[i] && i < 127) { lw[i] = tolower((unsigned char)w[i]); i++; } lw[i] = 0;
    if (InList(L_STRUCT, lw)) return TK_STRUCT;
    if (InList(L_STAT,   lw)) return TK_STAT;
    if (InList(L_VERB,   lw)) return TK_VERB;
    if (InList(L_MOD,    lw)) return TK_MOD;
    if (InList(L_CMP,    lw)) return TK_CMP;
    if (InList(L_CHC,    lw)) return TK_CHC;
    if (InList(L_KW,     lw)) return TK_KW;
    return TK_PLN;
}

static void TokenizeLine(const char *line, LineToks *lt) {
    lt->n = 0;
    int i = 0, len = (int)strlen(line);
    while (i < len && lt->n < MAX_TOKS - 1) {
        char c = line[i];
        Token *tk = &lt->tok[lt->n];

        if (c == ' ' || c == '\t') {
            tk->t = TK_WS; int s = 0;
            while (i < len && (line[i] == ' ' || line[i] == '\t') && s < 127) tk->v[s++] = line[i++];
            tk->v[s] = 0; lt->n++; continue;
        }
        if (c == '/' && i + 1 < len && line[i+1] == '/') {
            tk->t = TK_CMT; int s = 0;
            while (i < len && s < 127) tk->v[s++] = line[i++];
            tk->v[s] = 0; lt->n++; break;
        }
        // percentage: (NNN%) or (NN.N%)
        if (c == '(') {
            int j = i + 1, ns = 0; char nb[32];
            while (j < len && (isdigit((unsigned char)line[j]) || line[j] == '.') && ns < 31) nb[ns++] = line[j++];
            nb[ns] = 0;
            if (ns > 0 && j < len && line[j] == '%' && j+1 < len && line[j+1] == ')') {
                tk->t = TK_PCT; int s = 0;
                while (i <= j + 1 && s < 127) tk->v[s++] = line[i++];
                tk->v[s] = 0; lt->n++; continue;
            }
        }
        if (c == '{' || c == '}') {
            tk->t = TK_BRC; tk->v[0] = c; tk->v[1] = 0; i++; lt->n++; continue;
        }
        if (c == ':') {
            tk->t = TK_COL; tk->v[0] = c; tk->v[1] = 0; i++; lt->n++; continue;
        }
        if (isdigit((unsigned char)c)) {
            tk->t = TK_NUM; int s = 0;
            while (i < len && (isdigit((unsigned char)line[i]) || line[i] == '.') && s < 127) tk->v[s++] = line[i++];
            tk->v[s] = 0; lt->n++; continue;
        }
        if (isalpha((unsigned char)c) || c == '_') {
            int s = 0;
            while (i < len && (isalnum((unsigned char)line[i]) || line[i] == '_') && s < 127) tk->v[s++] = line[i++];
            tk->v[s] = 0; tk->t = ClassifyWord(tk->v); lt->n++; continue;
        }
        tk->t = TK_PLN; tk->v[0] = c; tk->v[1] = 0; i++; lt->n++;
    }
}

// ─────────────────────────────────────────────────────────────
//  THEME SYSTEM
// ─────────────────────────────────────────────────────────────

typedef struct {
    const char *name;
    Color bg, panel, bdr, lnum, cline, caret, tx;
    Color cSTR, cSTA, cVER, cMOD, cCMP, cKW, cNUM, cPCT, cCHC, cBRC, cCOL, cCMT;
    Color acc, cERR, cOK, muted;
} Theme;

#define C(r,g,b) {r,g,b,255}

static Theme THEMES[4] = {
    { "Dark+",
      C(13,17,23), C(22,27,34), C(48,54,61), C(72,79,88), C(28,33,41), C(230,237,243), C(230,237,243),
      C(255,123,114), C(121,192,255), C(210,168,255), C(255,166,87), C(255,166,87), C(126,224,135),
      C(121,192,255), C(240,136,62), C(227,179,65), C(230,237,243), C(139,148,158), C(139,148,158),
      C(31,111,235), C(255,123,114), C(63,185,80), C(100,110,120) },

    { "Monokai",
      C(39,40,34), C(30,31,28), C(73,72,62), C(117,113,94), C(62,61,50), C(248,248,242), C(248,248,242),
      C(249,38,114), C(102,217,232), C(166,226,46), C(230,219,116), C(253,151,31), C(102,217,232),
      C(174,129,255), C(253,151,31), C(230,219,116), C(248,248,242), C(248,248,242), C(117,113,94),
      C(249,38,114), C(249,38,114), C(166,226,46), C(117,113,94) },

    { "Dracula",
      C(40,42,54), C(33,34,44), C(68,71,90), C(98,114,164), C(55,57,68), C(248,248,242), C(248,248,242),
      C(255,85,85), C(139,233,253), C(189,147,249), C(255,184,108), C(255,184,108), C(80,250,123),
      C(189,147,249), C(255,184,108), C(241,250,140), C(248,248,242), C(98,114,164), C(98,114,164),
      C(189,147,249), C(255,85,85), C(80,250,123), C(98,114,164) },

    { "Abyss",
      C(0,12,24), C(0,6,15), C(31,58,95), C(45,95,138), C(0,20,40), C(220,228,237), C(220,228,237),
      C(232,67,147), C(34,180,211), C(163,122,204), C(240,170,58), C(240,170,58), C(34,180,211),
      C(163,122,204), C(240,170,58), C(232,232,74), C(220,228,237), C(91,141,184), C(45,95,138),
      C(232,67,147), C(232,67,147), C(34,180,211), C(45,95,138) }
};

static int themeIdx = 0;
#define TH (THEMES[themeIdx])

static Color TokColor(TKType t) {
    switch (t) {
        case TK_STRUCT: return TH.cSTR;
        case TK_STAT:   return TH.cSTA;
        case TK_VERB:   return TH.cVER;
        case TK_MOD:    return TH.cMOD;
        case TK_CMP:    return TH.cCMP;
        case TK_KW:     return TH.cKW;
        case TK_NUM:    return TH.cNUM;
        case TK_PCT:    return TH.cPCT;
        case TK_CHC:    return TH.cCHC;
        case TK_BRC:    return TH.cBRC;
        case TK_COL:    return TH.cCOL;
        case TK_CMT:    return TH.cCMT;
        default:        return TH.tx;
    }
}

// ─────────────────────────────────────────────────────────────
//  DATA STRUCTURES
// ─────────────────────────────────────────────────────────────

typedef struct {
    char name[64];
    char lines[MAX_LINES][LINE_LEN];
    int  lineCount;
    int  cx, cy;       // cursor col & line (0-based)
    int  scrollY;      // first visible line index
    int  scrollX;      // horizontal pixel offset
    bool dirty;
} EdTab;

typedef struct {
    int  type;         // 0=character  1=ability  2=trigger
    char name[64];
    char parts[MAX_PARTS][PART_LEN];
    int  partCount;
} SummItem;

typedef struct { int line; char msg[ERR_LEN]; } ErrItem;

// ─────────────────────────────────────────────────────────────
//  TOOLTIP DATA
// ─────────────────────────────────────────────────────────────

typedef struct { const char *kw; const char *tip; } TipEntry;

static TipEntry TIPS[] = {
    {"character", "Defines a game entity with stats and abilities"},
    {"ability",   "Active skill: cost, action sequence, and effects"},
    {"when",      "Conditional trigger — fires on a game state event"},
    {"rule",      "Reserved for future passive rule blocks"},
    {"hp",        "Hit Points — the entity health pool"},
    {"strength",  "Physical strike power"},
    {"defense",   "Flat incoming damage reduction"},
    {"dodge",     "Evasion chance (0.0 = 0%, 1.0 = 100%)"},
    {"speed",     "Turn order and movement speed"},
    {"attack",    "Base damage output stat"},
    {"tenacity",  "Resistance to crowd-control and debuffs"},
    {"mark",      "Tags an entity for delayed or bonus effects"},
    {"reduce",    "Decreases a stat or value — needs 'by <amount>'"},
    {"increase",  "Increases a stat or value — needs 'by <amount>'"},
    {"enter",     "Transitions entity to a named mode or state"},
    {"restore",   "Heals or recovers a stat value"},
    {"first",     "First action in the ability chain"},
    {"then",      "Fires after the previous chained action"},
    {"also",      "Concurrent side-effect, fires alongside others"},
    {"below",     "Condition: stat is strictly below a threshold"},
    {"above",     "Condition: stat is strictly above a threshold"},
    {"always",    "100% — always triggers, no chance check"},
    {"sometimes", "~40% — occasional trigger chance"},
    {"often",     "~70% — frequent trigger chance"},
    {"rarely",    "~15% — rare trigger chance"},
    {"never",     "0% — completely disabled"},
    {"effort",    "Action point cost to activate this ability"},
    {"level",     "Intensity tier of an effect (1 = lowest)"},
    {0, 0}
};

static const char *GetTip(const char *word) {
    char lw[64]; int i = 0;
    while (word[i] && i < 63) { lw[i] = tolower((unsigned char)word[i]); i++; } lw[i] = 0;
    for (int j = 0; TIPS[j].kw; j++) if (strcmp(TIPS[j].kw, lw) == 0) return TIPS[j].tip;
    return NULL;
}

// ─────────────────────────────────────────────────────────────
//  CONTENT HELPERS
// ─────────────────────────────────────────────────────────────

static void SetContent(EdTab *tab, const char *text) {
    tab->lineCount = 0;
    int i = 0, start = 0, len = (int)strlen(text);
    while (i <= len) {
        if (i == len || text[i] == '\n') {
            int ll = i - start;
            if (ll >= LINE_LEN) ll = LINE_LEN - 1;
            strncpy(tab->lines[tab->lineCount], text + start, ll);
            tab->lines[tab->lineCount][ll] = 0;
            tab->lineCount++;
            start = i + 1;
        }
        i++;
    }
    if (tab->lineCount == 0) { tab->lines[0][0] = 0; tab->lineCount = 1; }
}

static void InitSamples(EdTab tabs[], int *count) {
    *count = 3;

    strcpy(tabs[0].name, "FlameRush.ba");
    SetContent(&tabs[0],
        "ability FlameRush {\n"
        "    it costs effort level 5\n"
        "\n"
        "    first mark the enemy with fire\n"
        "    then reduce their hp by 25 (60%)\n"
        "    also enter rage (50%)\n"
        "}"
    );

    strcpy(tabs[1].name, "EmberWolf.ba");
    SetContent(&tabs[1],
        "character EmberWolf {\n"
        "    it is a fire beast\n"
        "\n"
        "    its stats are:\n"
        "        hp is 120\n"
        "        attack is 35\n"
        "        speed is 18\n"
        "        defense is 12\n"
        "        dodge is 0.20\n"
        "        tenacity is 8\n"
        "}"
    );

    strcpy(tabs[2].name, "SeaBud.ba");
    SetContent(&tabs[2],
        "character SeaBud {\n"
        "    it is an alien creature\n"
        "\n"
        "    its stats are:\n"
        "        hp is 85\n"
        "        strength is 22\n"
        "        defense is 15\n"
        "        dodge is 0.35\n"
        "}\n"
        "\n"
        "ability TidalMark {\n"
        "    it costs effort level 7\n"
        "\n"
        "    first mark the enemy with level 7\n"
        "    then reduce their health by 40 (60%)\n"
        "}\n"
        "\n"
        "when hp is below 20:\n"
        "    increase attack by 10\n"
        "    enter rage mode (80%)"
    );

    for (int i = 0; i < *count; i++) {
        tabs[i].cx = 0; tabs[i].cy = 0;
        tabs[i].scrollX = 0; tabs[i].scrollY = 0;
        tabs[i].dirty = false;
    }
}

// ─────────────────────────────────────────────────────────────
//  DIAGNOSTICS
// ─────────────────────────────────────────────────────────────

static int GetErrors(EdTab *tab, ErrItem *errs, int maxErrs) {
    int n = 0, braces = 0;
    for (int i = 0; i < tab->lineCount && n < maxErrs - 1; i++) {
        const char *ln = tab->lines[i];
        for (int j = 0; ln[j]; j++) {
            if (ln[j] == '{') braces++;
            else if (ln[j] == '}') braces--;
        }
        // Make lowercase copy for pattern check
        char lw[LINE_LEN]; int k = 0;
        while (ln[k] && k < LINE_LEN - 1) { lw[k] = tolower((unsigned char)ln[k]); k++; } lw[k] = 0;

        if (strstr(lw, "reduce") && !strstr(lw, "by")) {
            snprintf(errs[n].msg, ERR_LEN, "'reduce' requires 'by <amount>'");
            errs[n++].line = i + 1;
        }
        if (strstr(lw, "increase") && !strstr(lw, "by")) {
            snprintf(errs[n].msg, ERR_LEN, "'increase' requires 'by <amount>'");
            errs[n++].line = i + 1;
        }
    }
    if (braces > 0 && n < maxErrs) {
        snprintf(errs[n].msg, ERR_LEN, "%d unclosed block%s — missing '}'", braces, braces > 1 ? "s" : "");
        errs[n++].line = 0;
    } else if (braces < 0 && n < maxErrs) {
        snprintf(errs[n].msg, ERR_LEN, "%d extra '}' brace%s", -braces, -braces > 1 ? "s" : "");
        errs[n++].line = 0;
    }
    return n;
}

// ─────────────────────────────────────────────────────────────
//  SMART SUMMARY
// ─────────────────────────────────────────────────────────────

static void LowerCopy(const char *src, char *dst, int max) {
    int i = 0;
    while (src[i] && i < max - 1) { dst[i] = tolower((unsigned char)src[i]); i++; }
    dst[i] = 0;
}

static const char *SkipSpaces(const char *p) { while (*p == ' ') p++; return p; }

// Read word into buf (stops at space/punct), returns chars consumed
static int ReadWord(const char *p, char *buf, int max) {
    int i = 0;
    while (p[i] && !isspace((unsigned char)p[i]) && p[i] != '{' && p[i] != ':' && i < max - 1) buf[i++] = p[i];
    buf[i] = 0; return i;
}

// Read number into buf, returns chars consumed
static int ReadNum(const char *p, char *buf, int max) {
    int i = 0;
    while (p[i] && (isdigit((unsigned char)p[i]) || p[i] == '.') && i < max - 1) buf[i++] = p[i];
    buf[i] = 0; return i;
}

static int GetSummary(EdTab *tab, SummItem *items, int maxItems) {
    int n = 0;
    for (int i = 0; i < tab->lineCount && n < maxItems; i++) {
        char lw[LINE_LEN];
        LowerCopy(tab->lines[i], lw, LINE_LEN);
        const char *p;

        // ── Character ─────────────────────────────────
        if ((p = strstr(lw, "character")) && p == lw + strspn(lw, " \t")) {
            SummItem *it = &items[n++];
            it->type = 0; it->partCount = 0;
            const char *ns = tab->lines[i] + (p - lw) + 9;
            ns = SkipSpaces(ns);
            int ni = 0;
            while (*ns && *ns != '{' && !isspace((unsigned char)*ns) && ni < 63) it->name[ni++] = *ns++;
            it->name[ni] = 0;

            // Scan ahead for stats
            const char *stats[] = {"hp","strength","defense","dodge","speed","attack","tenacity",0};
            for (int j = i + 1; j < tab->lineCount && j < i + 25 && it->partCount < MAX_PARTS; j++) {
                char ll[LINE_LEN]; LowerCopy(tab->lines[j], ll, LINE_LEN);
                if (strstr(ll, "}")) break;
                for (int s = 0; stats[s]; s++) {
                    char *sp = strstr(ll, stats[s]);
                    if (!sp) continue;
                    char *ip = strstr(sp, " is ");
                    if (!ip) { ip = strstr(sp, "\tis "); if (!ip) continue; }
                    ip += 4;
                    char val[32]; ReadNum(ip, val, 32);
                    if (val[0]) {
                        char capStat[32];
                        snprintf(capStat, 32, "%c%s", toupper((unsigned char)stats[s][0]), stats[s] + 1);
                        snprintf(it->parts[it->partCount++], PART_LEN, "%s: %s", capStat, val);
                        break;
                    }
                }
            }
            if (it->partCount == 0) strncpy(it->parts[it->partCount++], "No stats defined yet", PART_LEN - 1);
            continue;
        }

        // ── Ability ───────────────────────────────────
        if ((p = strstr(lw, "ability")) && p == lw + strspn(lw, " \t")) {
            SummItem *it = &items[n++];
            it->type = 1; it->partCount = 0;
            const char *ns = tab->lines[i] + (p - lw) + 7;
            ns = SkipSpaces(ns);
            int ni = 0;
            while (*ns && *ns != '{' && !isspace((unsigned char)*ns) && ni < 63) it->name[ni++] = *ns++;
            it->name[ni] = 0;

            for (int j = i + 1; j < tab->lineCount && j < i + 25 && it->partCount < MAX_PARTS; j++) {
                char ll[LINE_LEN]; LowerCopy(tab->lines[j], ll, LINE_LEN);
                if (strstr(ll, "}")) break;

                // Cost
                char *cp = strstr(ll, "effort level");
                if (cp) {
                    cp += 12; cp = (char *)SkipSpaces(cp);
                    char val[16]; ReadNum(cp, val, 16);
                    if (val[0]) { snprintf(it->parts[it->partCount++], PART_LEN, "Costs %s effort", val); continue; }
                }
                // Reduce ... by N (PCT%)
                if (strstr(ll, "reduce") && strstr(ll, " by ")) {
                    char *bp = strstr(ll, " by ") + 4;
                    char val[16]; ReadNum(bp, val, 16);
                    char pct[16] = "";
                    char *pp = strchr(bp, '(');
                    if (pp) { pp++; int pi = 0; while (*pp && *pp != ')' && *pp != '%' && pi < 15) pct[pi++] = *pp++; pct[pi] = 0; }
                    if (pct[0]) snprintf(it->parts[it->partCount++], PART_LEN, "Deals %s damage (%s%% power)", val, pct);
                    else        snprintf(it->parts[it->partCount++], PART_LEN, "Deals %s damage", val);
                    continue;
                }
                // Increase ... by N
                if (strstr(ll, "increase") && strstr(ll, " by ")) {
                    char *bp = strstr(ll, " by ") + 4;
                    char val[16]; ReadNum(bp, val, 16);
                    if (val[0]) { snprintf(it->parts[it->partCount++], PART_LEN, "Boosts stat by +%s", val); continue; }
                }
                // Mark
                if (strstr(ll, "mark")) {
                    snprintf(it->parts[it->partCount++], PART_LEN, "Marks the target");
                    continue;
                }
                // Enter [mode] (PCT%)
                if (strstr(ll, "enter")) {
                    char *ep = strstr(ll, "enter") + 5;
                    ep = (char *)SkipSpaces(ep);
                    // strip "first"/"then"/"also" prefix already done by lowercase
                    char mode[32] = ""; int mi = 0;
                    while (*ep && *ep != '(' && mi < 31) mode[mi++] = *ep++;
                    while (mi > 0 && mode[mi-1] == ' ') mi--;
                    mode[mi] = 0;
                    char pct[16] = "";
                    if (*ep == '(') { ep++; int pi = 0; while (*ep && *ep != ')' && *ep != '%' && pi < 15) pct[pi++] = *ep++; pct[pi] = 0; }
                    if (pct[0]) snprintf(it->parts[it->partCount++], PART_LEN, "Enters %s state (%s%%)", mode, pct);
                    else        snprintf(it->parts[it->partCount++], PART_LEN, "Enters %s state", mode);
                    continue;
                }
            }
            if (it->partCount == 0) strncpy(it->parts[it->partCount++], "No effects defined yet", PART_LEN - 1);
            continue;
        }

        // ── When trigger ──────────────────────────────
        if ((p = strstr(lw, "when ")) && p == lw + strspn(lw, " \t")) {
            SummItem *it = &items[n++];
            it->type = 2; it->partCount = 0;
            const char *cs = tab->lines[i] + (p - lw) + 5;
            char cond[64]; int ci = 0;
            while (*cs && *cs != ':' && *cs != '{' && ci < 63) cond[ci++] = *cs++;
            while (ci > 0 && cond[ci-1] == ' ') ci--;
            cond[ci] = 0;
            snprintf(it->name, 64, "when %s", cond);

            for (int j = i + 1; j < tab->lineCount && j < i + 12 && it->partCount < MAX_PARTS; j++) {
                const char *el = tab->lines[j];
                int indent = 0;
                while (el[indent] == ' ' || el[indent] == '\t') indent++;
                if (indent == 0 && el[0] != 0) break;
                if (el[indent] != 0) strncpy(it->parts[it->partCount++], el + indent, PART_LEN - 1);
            }
            if (it->partCount == 0) strncpy(it->parts[it->partCount++], "No effects defined", PART_LEN - 1);
            continue;
        }
    }
    return n;
}

// ─────────────────────────────────────────────────────────────
//  EDITOR INPUT
// ─────────────────────────────────────────────────────────────

static void EditorInput(EdTab *tab, float dt) {
    int visLines = ED_H / LH;

    // Mouse wheel scroll
    float wheel = GetMouseWheelMove();
    if (wheel != 0.0f) {
        tab->scrollY -= (int)(wheel * 3);
        if (tab->scrollY < 0) tab->scrollY = 0;
        int maxScroll = tab->lineCount - visLines / 2;
        if (maxScroll < 0) maxScroll = 0;
        if (tab->scrollY > maxScroll) tab->scrollY = maxScroll;
    }

    char *line = tab->lines[tab->cy];
    int   llen  = (int)strlen(line);

    // Character input
    int key = GetCharPressed();
    while (key > 0) {
        if (key >= 32 && llen < LINE_LEN - 1) {
            for (int i = llen; i >= tab->cx; i--) line[i+1] = line[i];
            line[tab->cx++] = (char)key;
            tab->dirty = true;
        }
        key = GetCharPressed();
    }

    // Tab → 4 spaces
    if (IsKeyPressed(KEY_TAB)) {
        llen = (int)strlen(tab->lines[tab->cy]);
        for (int sp = 0; sp < 4 && llen < LINE_LEN - 2; sp++) {
            for (int i = llen; i >= tab->cx; i--) tab->lines[tab->cy][i+1] = tab->lines[tab->cy][i];
            tab->lines[tab->cy][tab->cx++] = ' ';
            llen++;
        }
        tab->dirty = true;
    }

    // Backspace
    if (IsKeyPressed(KEY_BACKSPACE)) {
        llen = (int)strlen(tab->lines[tab->cy]);
        if (tab->cx > 0) {
            for (int i = tab->cx; i <= llen; i++) tab->lines[tab->cy][i-1] = tab->lines[tab->cy][i];
            tab->cx--;
            tab->dirty = true;
        } else if (tab->cy > 0) {
            int prevLen = (int)strlen(tab->lines[tab->cy - 1]);
            int curLen  = (int)strlen(tab->lines[tab->cy]);
            if (prevLen + curLen < LINE_LEN - 1) {
                strcat(tab->lines[tab->cy - 1], tab->lines[tab->cy]);
                for (int i = tab->cy; i < tab->lineCount - 1; i++) strcpy(tab->lines[i], tab->lines[i+1]);
                tab->lines[--tab->lineCount][0] = 0;
                tab->cy--;
                tab->cx = prevLen;
                tab->dirty = true;
            }
        }
    }

    // Delete
    if (IsKeyPressed(KEY_DELETE)) {
        llen = (int)strlen(tab->lines[tab->cy]);
        if (tab->cx < llen) {
            for (int i = tab->cx; i < llen; i++) tab->lines[tab->cy][i] = tab->lines[tab->cy][i+1];
            tab->dirty = true;
        }
    }

    // Enter — split line, auto-indent
    if (IsKeyPressed(KEY_ENTER)) {
        if (tab->lineCount < MAX_LINES - 1) {
            int indent = 0;
            while (tab->lines[tab->cy][indent] == ' ') indent++;
            char rest[LINE_LEN];
            strcpy(rest, tab->lines[tab->cy] + tab->cx);
            tab->lines[tab->cy][tab->cx] = 0;

            for (int i = tab->lineCount; i > tab->cy + 1; i--) strcpy(tab->lines[i], tab->lines[i-1]);
            tab->lineCount++;

            char newln[LINE_LEN]; int ni = 0;
            for (int s = 0; s < indent && ni < LINE_LEN - 1; s++) newln[ni++] = ' ';
            int ri = 0;
            while (rest[ri] && ni < LINE_LEN - 1) newln[ni++] = rest[ri++];
            newln[ni] = 0;
            strcpy(tab->lines[tab->cy + 1], newln);
            tab->cy++;
            tab->cx = indent;
            tab->dirty = true;
        }
    }

    // Arrow navigation (+ key repeat via IsKeyDown)
    static float repTimer[4] = {0}; // L R U D
    static bool  repActive[4]= {0};
    int navKeys[4] = {KEY_LEFT, KEY_RIGHT, KEY_UP, KEY_DOWN};

    for (int d = 0; d < 4; d++) {
        bool fire = false;
        if (IsKeyPressed(navKeys[d])) { fire = true; repTimer[d] = 0.35f; repActive[d] = true; }
        else if (repActive[d] && IsKeyDown(navKeys[d])) {
            repTimer[d] -= dt;
            if (repTimer[d] <= 0.0f) { fire = true; repTimer[d] = 0.045f; }
        } else { repActive[d] = false; repTimer[d] = 0; }

        if (!fire) continue;
        llen = (int)strlen(tab->lines[tab->cy]);
        if (d == 0) { // LEFT
            if (tab->cx > 0) tab->cx--;
            else if (tab->cy > 0) { tab->cy--; tab->cx = (int)strlen(tab->lines[tab->cy]); }
        } else if (d == 1) { // RIGHT
            if (tab->cx < llen) tab->cx++;
            else if (tab->cy < tab->lineCount - 1) { tab->cy++; tab->cx = 0; }
        } else if (d == 2) { // UP
            if (tab->cy > 0) { tab->cy--; int ml = (int)strlen(tab->lines[tab->cy]); if (tab->cx > ml) tab->cx = ml; }
        } else { // DOWN
            if (tab->cy < tab->lineCount - 1) { tab->cy++; int ml = (int)strlen(tab->lines[tab->cy]); if (tab->cx > ml) tab->cx = ml; }
        }
    }

    if (IsKeyPressed(KEY_HOME)) tab->cx = 0;
    if (IsKeyPressed(KEY_END))  tab->cx = (int)strlen(tab->lines[tab->cy]);

    if (IsKeyPressed(KEY_PAGE_UP)) {
        tab->cy -= visLines; if (tab->cy < 0) tab->cy = 0;
        int ml = (int)strlen(tab->lines[tab->cy]); if (tab->cx > ml) tab->cx = ml;
    }
    if (IsKeyPressed(KEY_PAGE_DOWN)) {
        tab->cy += visLines; if (tab->cy >= tab->lineCount) tab->cy = tab->lineCount - 1;
        int ml = (int)strlen(tab->lines[tab->cy]); if (tab->cx > ml) tab->cx = ml;
    }

    // Clamp
    llen = (int)strlen(tab->lines[tab->cy]);
    if (tab->cx < 0) tab->cx = 0;
    if (tab->cx > llen) tab->cx = llen;

    // Scroll-follow cursor
    if (tab->cy < tab->scrollY) tab->scrollY = tab->cy;
    if (tab->cy >= tab->scrollY + visLines) tab->scrollY = tab->cy - visLines + 1;
    if (tab->scrollY < 0) tab->scrollY = 0;
}

// ─────────────────────────────────────────────────────────────
//  DRAW HELPERS
// ─────────────────────────────────────────────────────────────

static Color AlphaBlend(Color c, unsigned char a) { c.a = a; return c; }

// Draw text and return new X
static int DT(const char *t, int x, int y, int sz, Color c) {
    DrawText(t, x, y, sz, c); return x + MeasureText(t, sz);
}

// ─────────────────────────────────────────────────────────────
//  DRAW TITLE BAR
// ─────────────────────────────────────────────────────────────

static void DrawTitleBar(void) {
    DrawRectangle(0, 0, WIN_W, TITLE_H, TH.panel);
    DrawLine(0, TITLE_H - 1, WIN_W, TITLE_H - 1, TH.bdr);
    DrawText("BA", 14, 8, 14, TH.acc);
    DrawText("Language IDE  v0.1", 36, 9, 11, TH.muted);

    // Theme selector buttons (right-aligned)
    int bx = WIN_W - 8;
    for (int i = 3; i >= 0; i--) {
        const char *nm = THEMES[i].name;
        int bw = MeasureText(nm, 11) + 14;
        bx -= bw + 4;
        bool sel = (i == themeIdx);
        Color bc = sel ? TH.acc : TH.bdr;
        Color tc = sel ? TH.acc : TH.muted;
        DrawRectangleLines(bx, 6, bw, 19, bc);
        DrawText(nm, bx + 7, 9, 11, tc);
        Vector2 mp = GetMousePosition();
        if (CheckCollisionPointRec(mp, (Rectangle){bx, 6, bw, 19}) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            themeIdx = i;
    }
    bx -= 4;
    DrawText("F1", bx - MeasureText("F1", 10) - 4, 10, 10, TH.muted);
}

// ─────────────────────────────────────────────────────────────
//  DRAW TAB BAR
// ─────────────────────────────────────────────────────────────

static int DrawTabBar(EdTab tabs[], int tabCount, int activeTab) {
    DrawRectangle(0, TITLE_H, WIN_W, TAB_H, TH.panel);
    DrawLine(0, TITLE_H + TAB_H - 1, WIN_W, TITLE_H + TAB_H - 1, TH.bdr);

    int tx = 0, closeIdx = -1;
    Vector2 mp = GetMousePosition();

    for (int i = 0; i < tabCount; i++) {
        bool active = (i == activeTab);
        int nw  = MeasureText(tabs[i].name, 12);
        int tw  = nw + 36;
        int ty  = TITLE_H;

        // Background
        DrawRectangle(tx, ty, tw, TAB_H, active ? TH.bg : TH.panel);
        DrawLine(tx + tw, ty, tx + tw, ty + TAB_H, TH.bdr);
        // Top accent
        if (active) DrawRectangle(tx, ty, tw, 2, TH.acc);
        // Dirty dot
        Color dotC = tabs[i].dirty ? TH.cERR : AlphaBlend(TH.acc, active ? 200 : 80);
        DrawRectangle(tx + 8, ty + 15, 6, 6, dotC);
        // Name
        DrawText(tabs[i].name, tx + 18, ty + 12, 12, active ? TH.tx : TH.muted);
        // Close 'x'
        if (tabCount > 1) {
            int cx2 = tx + tw - 16, cy2 = ty + 11;
            Rectangle cr = {cx2, cy2, 12, 12};
            bool hov = CheckCollisionPointRec(mp, cr);
            DrawText("x", cx2 + 2, cy2, 12, hov ? TH.tx : TH.muted);
            if (hov && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) closeIdx = i;
        }
        // Tab click (only if not on close)
        Rectangle tr = {tx, ty, tw - 18, TAB_H};
        if (CheckCollisionPointRec(mp, tr) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            // signal switch handled in main via return + globals
        }
        tx += tw;
    }
    // '+' button
    if (tabCount < MAX_TABS) {
        Rectangle nr = {tx, TITLE_H, 28, TAB_H};
        bool hov = CheckCollisionPointRec(mp, nr);
        DrawText("+", tx + 8, TITLE_H + 9, 16, hov ? TH.tx : TH.muted);
    }
    return closeIdx;
}

// Handle tab bar click separately (returns new active, or -1 for +)
static int TabBarInput(EdTab tabs[], int tabCount, int activeTab) {
    int tx = 0;
    Vector2 mp = GetMousePosition();
    if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) return activeTab;
    if (mp.y < TITLE_H || mp.y > TITLE_H + TAB_H) return activeTab;

    for (int i = 0; i < tabCount; i++) {
        int tw = MeasureText(tabs[i].name, 12) + 36;
        Rectangle tr = {tx, TITLE_H, tw - 18, TAB_H};
        if (CheckCollisionPointRec(mp, tr)) return i;
        tx += tw;
    }
    // '+' button
    if (tabCount < MAX_TABS) {
        Rectangle nr = {tx, TITLE_H, 28, TAB_H};
        if (CheckCollisionPointRec(mp, nr)) return -1; // signal new tab
    }
    return activeTab;
}

// ─────────────────────────────────────────────────────────────
//  DRAW EDITOR (line numbers + syntax text + cursor)
// ─────────────────────────────────────────────────────────────

static void DrawEditor(EdTab *tab, bool showCursor, char *hoverWord, int maxHW) {
    int visLines = ED_H / LH;
    Vector2 mp = GetMousePosition();
    hoverWord[0] = 0;

    // ── Editor text area ────────────────────────────────────
    BeginScissorMode(ED_X, ED_Y, ED_W, ED_H);
    DrawRectangle(ED_X, ED_Y, ED_W, ED_H, TH.bg);

    // Current-line highlight
    int clY = ED_Y + PT + (tab->cy - tab->scrollY) * LH;
    if (clY >= ED_Y && clY <= ED_Y + ED_H)
        DrawRectangle(ED_X, clY, ED_W, LH, TH.cline);

    // Render lines with syntax coloring
    LineToks lt;
    for (int i = tab->scrollY; i < tab->lineCount && i <= tab->scrollY + visLines; i++) {
        int lineY = ED_Y + PT + (i - tab->scrollY) * LH;
        if (lineY + LH < ED_Y || lineY > ED_Y + ED_H) continue;

        TokenizeLine(tab->lines[i], &lt);
        int x = ED_X + PL - tab->scrollX;

        for (int j = 0; j < lt.n; j++) {
            Color c = TokColor(lt.tok[j].t);
            int tw = MeasureText(lt.tok[j].v, FS);
            // Hover detection (keywords only)
            if (lt.tok[j].t != TK_WS && lt.tok[j].t != TK_PLN) {
                Rectangle tr = {x, lineY, tw, LH};
                if (CheckCollisionPointRec(mp, tr)) strncpy(hoverWord, lt.tok[j].v, maxHW - 1);
            }
            DrawText(lt.tok[j].v, x, lineY, FS, c);
            x += tw;
        }
    }

    // Blinking cursor
    if (showCursor) {
        char prefix[LINE_LEN];
        int pl = tab->cx < (int)strlen(tab->lines[tab->cy]) ? tab->cx : (int)strlen(tab->lines[tab->cy]);
        strncpy(prefix, tab->lines[tab->cy], pl);
        prefix[pl] = 0;
        int cx_px = ED_X + PL - tab->scrollX + MeasureText(prefix, FS);
        int cy_px = ED_Y + PT + (tab->cy - tab->scrollY) * LH;
        DrawLine(cx_px, cy_px, cx_px, cy_px + FS + 2, TH.caret);
    }
    EndScissorMode();

    // ── Line numbers ────────────────────────────────────────
    BeginScissorMode(0, ED_Y, LN_W, ED_H);
    DrawRectangle(0, ED_Y, LN_W, ED_H, TH.bg);
    DrawLine(LN_W - 1, ED_Y, LN_W - 1, ED_Y + ED_H, TH.bdr);

    for (int i = tab->scrollY; i < tab->lineCount && i <= tab->scrollY + visLines; i++) {
        int lineY = ED_Y + PT + (i - tab->scrollY) * LH;
        if (lineY + LH < ED_Y || lineY > ED_Y + ED_H) continue;
        char num[10]; snprintf(num, 10, "%d", i + 1);
        int nw = MeasureText(num, 12);
        Color lc = (i == tab->cy) ? TH.tx : TH.lnum;
        DrawText(num, LN_W - nw - 8, lineY + 1, 12, lc);
    }
    EndScissorMode();
}

// ─────────────────────────────────────────────────────────────
//  DRAW SIDE PANEL
// ─────────────────────────────────────────────────────────────

static void DrawSidePanel(EdTab *tab, SummItem *summ, int summN, ErrItem *errs, int errN) {
    DrawRectangle(PNL_X, PNL_Y, SIDE_W, PNL_H, TH.panel);
    DrawLine(PNL_X, PNL_Y, PNL_X, PNL_Y + PNL_H, TH.bdr);

    int sx = PNL_X + 12, y = PNL_Y + 12;
    int maxW = SIDE_W - 24;

    // ── Smart Summary ────────────────────────────────────────
    DrawText("SMART SUMMARY", sx, y, 10, TH.muted); y += 18;

    if (summN == 0) {
        DrawText("Define a character, ability", sx, y, 11, TH.muted); y += 14;
        DrawText("or when block to see summary.", sx, y, 11, TH.muted); y += 16;
    }

    int summCutoff = PNL_Y + (int)(PNL_H * 0.55f);
    for (int i = 0; i < summN && y < summCutoff; i++) {
        SummItem *it = &summ[i];
        // Icon + colored name
        const char *icon = it->type == 1 ? "E " : it->type == 2 ? "W " : "C ";
        Color ic = it->type == 1 ? TH.cVER : it->type == 2 ? TH.cMOD : TH.cSTA;
        int ix = sx;
        DrawText(icon, ix, y, 12, ic); ix += MeasureText(icon, 12) + 2;
        // Truncate name if needed
        char nm[64]; strncpy(nm, it->name, 63); nm[63] = 0;
        while (MeasureText(nm, 12) > maxW - 20 && strlen(nm) > 4) nm[strlen(nm)-1] = 0;
        DrawText(nm, ix, y, 12, ic);
        y += 18;

        for (int j = 0; j < it->partCount && y < summCutoff; j++) {
            DrawLine(sx + 2, y + 1, sx + 2, y + 13, TH.bdr);
            char pt[PART_LEN]; strncpy(pt, it->parts[j], PART_LEN - 1);
            while (MeasureText(pt, 11) > maxW - 12 && strlen(pt) > 4) pt[strlen(pt)-1] = 0;
            DrawText(pt, sx + 10, y, 11, TH.tx);
            y += 15;
        }
        y += 5;
    }

    // ── Divider ──────────────────────────────────────────────
    int div1 = PNL_Y + (int)(PNL_H * 0.57f);
    DrawLine(PNL_X, div1, WIN_W, div1, TH.bdr);
    y = div1 + 10;

    // ── Diagnostics ──────────────────────────────────────────
    {
        char hdr[32] = "DIAGNOSTICS";
        if (errN > 0) { char ec[16]; snprintf(ec, 16, " (%d)", errN); strncat(hdr, ec, 31 - strlen(hdr)); }
        DrawText(hdr, sx, y, 10, errN > 0 ? TH.cERR : TH.muted);
    }
    y += 18;

    int errCutoff = PNL_Y + (int)(PNL_H * 0.82f);
    if (errN == 0) {
        DrawText("v  No errors found", sx, y, 11, TH.cOK); y += 16;
    } else {
        for (int i = 0; i < errN && y < errCutoff; i++) {
            DrawRectangle(sx - 2, y - 1, maxW + 4, 20, AlphaBlend(TH.cERR, 28));
            DrawRectangle(sx - 2, y - 1, 2, 20, TH.cERR);
            char buf[ERR_LEN];
            if (errs[i].line > 0) snprintf(buf, ERR_LEN, "L%d: %s", errs[i].line, errs[i].msg);
            else strncpy(buf, errs[i].msg, ERR_LEN - 1);
            while (MeasureText(buf, 11) > maxW - 8 && strlen(buf) > 4) buf[strlen(buf)-1] = 0;
            DrawText(buf, sx + 6, y + 2, 11, TH.cERR);
            y += 23;
        }
    }

    // ── Divider ──────────────────────────────────────────────
    int div2 = PNL_Y + (int)(PNL_H * 0.83f);
    DrawLine(PNL_X, div2, WIN_W, div2, TH.bdr);
    y = div2 + 10;

    // ── Legend ───────────────────────────────────────────────
    DrawText("LEGEND", sx, y, 10, TH.muted); y += 16;

    typedef struct { const char *label; Color c; } LegEntry;
    LegEntry leg[] = {
        {"Structure", TH.cSTR}, {"Stats",      TH.cSTA},
        {"Verbs",     TH.cVER}, {"Modifiers",  TH.cMOD},
        {"Numbers",   TH.cNUM}, {"Chance",     TH.cCHC},
        {"Percent",   TH.cPCT}, {"Comments",   TH.cCMT},
    };
    int col = 0;
    for (int i = 0; i < 8 && y < PNL_Y + PNL_H - 4; i++) {
        int lx = sx + col * (maxW / 2 + 4);
        DrawRectangle(lx, y + 3, 7, 7, leg[i].c);
        DrawText(leg[i].label, lx + 11, y, 11, TH.muted);
        col++;
        if (col >= 2) { col = 0; y += 16; }
    }
}

// ─────────────────────────────────────────────────────────────
//  DRAW STATUS BAR
// ─────────────────────────────────────────────────────────────

static void DrawStatusBar(EdTab *tab, int errN) {
    int sy = WIN_H - STATUS_H;
    DrawRectangle(0, sy, WIN_W, STATUS_H, TH.panel);
    DrawLine(0, sy, WIN_W, sy, TH.bdr);

    int x = 12;
    DrawText("BA", x, sy + 5, 11, TH.acc); x += 24;

    char pos[32]; snprintf(pos, 32, "Ln %d  Col %d", tab->cy + 1, tab->cx + 1);
    DrawText(pos, x, sy + 5, 10, TH.muted); x += MeasureText(pos, 10) + 14;

    DrawText(tab->name, x, sy + 5, 10, TH.muted); x += MeasureText(tab->name, 10) + 14;

    char lc[32]; snprintf(lc, 32, "%d lines", tab->lineCount);
    DrawText(lc, x, sy + 5, 10, TH.muted);

    // Right side: status
    char sbuf[48];
    Color sc;
    if (errN > 0) { snprintf(sbuf, 48, "! %d error%s", errN, errN > 1 ? "s" : ""); sc = TH.cERR; }
    else          { snprintf(sbuf, 48, "OK"); sc = TH.cOK; }
    int sw = MeasureText(sbuf, 10);
    DrawText(sbuf, WIN_W - sw - 52, sy + 5, 10, sc);
    DrawText("UTF-8", WIN_W - 40, sy + 5, 10, TH.muted);
}

// ─────────────────────────────────────────────────────────────
//  DRAW TOOLTIP
// ─────────────────────────────────────────────────────────────

static void DrawTooltip(const char *tip, Vector2 pos) {
    if (!tip || !tip[0]) return;
    int tw = MeasureText(tip, 11) + 16;
    int tx = (int)pos.x + 14, ty = (int)pos.y - 34;
    if (tx + tw > WIN_W - 4) tx = WIN_W - tw - 4;
    if (ty < 4) ty = (int)pos.y + 16;
    DrawRectangle(tx - 1, ty - 1, tw + 2, 24, TH.bdr);
    DrawRectangle(tx, ty, tw, 22, TH.panel);
    DrawText(tip, tx + 8, ty + 5, 11, TH.tx);
}

// ─────────────────────────────────────────────────────────────
//  MAIN
// ─────────────────────────────────────────────────────────────

int main(void) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(WIN_W, WIN_H, "BA Language IDE");
    SetTargetFPS(60);

    // Tabs — static so the ~3 MB array lives in BSS, not the stack
    static EdTab tabs[MAX_TABS];
    memset(tabs, 0, sizeof(tabs));
    int tabCount = 0, activeTab = 0;
    InitSamples(tabs, &tabCount);

    // State
    SummItem summ[MAX_SUMM];
    ErrItem  errs[MAX_ERRS];
    int      summN = 0, errN = 0;
    float    blinkTimer = 0.0f;
    bool     showCursor = true;
    char     hoverWord[64] = "";

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        // ── Global keys ───────────────────────────────────────
        if (IsKeyPressed(KEY_F1)) themeIdx = (themeIdx + 1) % 4;

        bool ctrl = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);

        // Ctrl+Tab: cycle tabs
        if (ctrl && IsKeyPressed(KEY_TAB))
            activeTab = (activeTab + 1) % tabCount;

        // Ctrl+W: close active tab
        if (ctrl && IsKeyPressed(KEY_W) && tabCount > 1) {
            for (int i = activeTab; i < tabCount - 1; i++) tabs[i] = tabs[i+1];
            tabCount--;
            if (activeTab >= tabCount) activeTab = tabCount - 1;
        }

        // Ctrl+N: new empty tab
        if (ctrl && IsKeyPressed(KEY_N) && tabCount < MAX_TABS) {
            int id = tabCount;
            snprintf(tabs[id].name, 64, "new-%d.ba", id + 1);
            strcpy(tabs[id].lines[0], "// New .ba file");
            strcpy(tabs[id].lines[1], "");
            tabs[id].lineCount = 2;
            tabs[id].cx = tabs[id].cy = 0;
            tabs[id].scrollX = tabs[id].scrollY = 0;
            tabs[id].dirty = false;
            activeTab = tabCount++;
        }

        // ── Tab bar mouse input ────────────────────────────────
        int newActive = TabBarInput(tabs, tabCount, activeTab);
        if (newActive == -1 && tabCount < MAX_TABS) {
            // '+' clicked
            int id = tabCount;
            snprintf(tabs[id].name, 64, "new-%d.ba", id + 1);
            strcpy(tabs[id].lines[0], "// New .ba file");
            strcpy(tabs[id].lines[1], "");
            tabs[id].lineCount = 2;
            tabs[id].cx = tabs[id].cy = 0;
            tabs[id].scrollX = tabs[id].scrollY = 0;
            tabs[id].dirty = false;
            activeTab = tabCount++;
        } else if (newActive >= 0 && newActive < tabCount) {
            activeTab = newActive;
        }

        EdTab *tab = &tabs[activeTab];

        // ── Editor input (only when mouse not over panel) ──────
        Vector2 mp = GetMousePosition();
        bool overPanel = (mp.x > PNL_X);
        bool overTabBar = (mp.y < ED_Y);
        if (!overPanel && !overTabBar) EditorInput(tab, dt);

        // ── Cursor blink ──────────────────────────────────────
        blinkTimer += dt;
        if (blinkTimer > 0.52f) { showCursor = !showCursor; blinkTimer = 0; }
        // Reset blink on keypress so cursor is always visible when typing
        if (GetKeyPressed()) { showCursor = true; blinkTimer = 0; }

        // ── Analysis (per frame — fast for small files) ────────
        summN = GetSummary(tab, summ, MAX_SUMM);
        errN  = GetErrors(tab, errs, MAX_ERRS);

        // ── Draw ──────────────────────────────────────────────
        BeginDrawing();
        ClearBackground(TH.bg);

        DrawEditor(tab, showCursor, hoverWord, 64);
        DrawSidePanel(tab, summ, summN, errs, errN);
        DrawTitleBar();
        DrawTabBar(tabs, tabCount, activeTab);
        DrawStatusBar(tab, errN);

        // Handle tab close clicks (returned from DrawTabBar)
        // Re-check close button clicks
        {
            int tx2 = 0;
            for (int i = 0; i < tabCount; i++) {
                int tw = MeasureText(tabs[i].name, 12) + 36;
                if (tabCount > 1) {
                    int cx2 = tx2 + tw - 16, cy2 = TITLE_H + 11;
                    Rectangle cr = {cx2, cy2, 12, 12};
                    if (CheckCollisionPointRec(mp, cr) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        for (int j = i; j < tabCount - 1; j++) tabs[j] = tabs[j+1];
                        tabCount--;
                        if (activeTab >= tabCount) activeTab = tabCount - 1;
                        break;
                    }
                }
                tx2 += tw;
            }
        }

        // Tooltip
        if (hoverWord[0]) {
            const char *tip = GetTip(hoverWord);
            DrawTooltip(tip, mp);
        }

        // ── Help overlay (bottom-left) ─────────────────────────
        DrawText("F1: theme  Ctrl+N: new tab  Ctrl+W: close  Ctrl+Tab: switch",
                 8, WIN_H - STATUS_H - 16, 10, AlphaBlend(TH.muted, 140));

        EndDrawing();
    }

    CloseWindow();
    return 0;
}