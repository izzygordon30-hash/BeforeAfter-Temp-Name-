# Chython Toolkit

require 'rdoc/rdoc'

# Toolkit 
# Every piece of .ch text becomes one of these
module CH 
    module TokenType
        KEYWORD = :keyword
        IDENTIFIER = :identifier

        NUMBER = :number
        PERCENT = :percent
        STRING = :string
        CHANCE = :chance

        COLON = :colon
        LBRACE = :lbrace
        RBRACE = :rbrace
        NEWLINE = :newline
        EOF = :eof
        UNKNOWN = :unknown
    end

# Token 
# One unit of .ch text with type, value and source location

Token = Struct.new(:type, :value, :line, :col) do
    def to_s
        "#{type}(#{value.inspect}) @ #{line}:#{col}"
    end

    def is?(type_sym)
        type == type_sym
    end

    def keyword?(word)
        type == TokenType::KEYWORD && value.downcase == word.to_s.downcase
    end

    def chance_word?
        type == TokenType::CHANCE
    end

    def number?
        type == TokenType::NUMBER
    end

    def percent?
        type == TokenType::PERCENT
    end
end

# Chance
# Normalizes all probabilities expressions 0.0 to 1.0 float

module Chance
    WORDS = {
        "always"  => 1.0,
        "never"    => 0.0,
        "sometimes" => 0.4,
        "often"   => 0.7,
        "rarely"   => 0.2,
}.freeze

def self.parse(value)
    key = value.to_s.downcase
    return WORDS[key] if WORDS.key?(key)
    return value.to_f / 100.0    if value.to_s.end_with?("%")
    v = value.to_f
    return v if v >= 0.0 && v <= 1.0
    raise CHError.new("Invalid chance value: #{value.inspect}")
end
    def self.word?(value)
        WORDS.key?(value.to_s.downcase)  
    end

    def self.all_words
        WORDS.keys  
    end
end

# CH ERROR 
# Friendly error with file + line info.
# Hard to silently miss. 
class CHError < StandardError
    attr_reader :line, :col, :filename
    
    def initialize(msg, line: nil, col: nil, filename: nil)
        @line = line
        @col = col
        @filename = filename
        location = [filename, line && "line #{line}", col && "col #{col}"].compact.join(", ")  
        super(location.empty? ? msg : "#{msg} (#{location})")
    end
end

class SourceReader
    attr_reader :line, :col, :filename

    def initialize(source, filename: "<input>")
        @source     = source
        @pos        = 0
        @line       = 1
        @col        = 1
        @filename   = filename  
    end

    def peeker(offset = 0)
        @source[@pos + offset]  
    end

    def current
        @source[@pos]  
    end

    def advance
        ch = @source[@pos]  
        return nil if ch.nil?
        if ch == "\n"
            @line += 1
            @col = 1
        else
            @col += 1
        end
        @pos += 1
        ch
    end

    def eof?
        @pos >= @source.length  
    end
    
    def skip_spaces
        advance while !eof? && (current == " " || current == "\t")  
    end

    def skip_comment
        advance while !eof? && current != "\n"
        advance if current == "\n"
    end

    def location 
        {line: @line, col: @col, filename: @filename}  
    end
end

## Keyword List
# Every reserved word in ba
# Add new keyword here only - nowhere else got that?!
KEYWORDS = %w[
    character ability when rule it its is are an a the
    stats cost costs effort first then also chalways
    hp strength
    defense dodge
    speed attack
    tenacity 
    mark reduce increase enter health buff debuff 
    by level mode chance below above at and or not in of for
].uniq.freeze

CHANCE_WORDS = Chance.all_words.freeze

def self.keyword?(word)
    KEYWORDS.include?(word.downcase)  
end

def self.chance_word?(word)
    CHANCE_WORDS.include?(word.downcase)
end

## Stat Names
# Known stat indentifiers - map .ch names to C stat IDs
# Add 
#
#
STAT_IDS = {
    "hp" => 1,
    "health" => 1, # alias for hp
    "max_hp" => 2,

    "attack" => 3, 
    "atk" => 3, 
    "strength" => 3, # alias for attack
    "dmg" => 3,

    "defense" => 4,
    "dodge" => 5,
    "tenacity" => 6,
    "effort" => 7,
}.freeze

def self.stat_id(name)
    STAT_IDS[name.downcase]  
end

def self.stat?(name)
    STAT_IDS.key?(name.downcase)
end

# Primitive Names
# Maps .ch action words to CORE_PRIMATIVE_* IDs

PRIMITIVE_IDS = {
    "mark"     => 26,  # CORE_PRIMITIVE_MARK
    "reduce"   => 1,   # CORE_PRIMITIVE_MODIFY (negative)
    "increase" => 1,   # CORE_PRIMITIVE_MODIFY (positive)
    "lock"     => 3,   # CORE_PRIMITIVE_LOCK
    "restore"  => 17,  # CORE_PRIMITIVE_RESTORE
    "spawn"    => 6,   # CORE_PRIMITIVE_SPAWN
    "pierce"   => 27,  # CORE_PRIMITIVE_PIERCE
    "steal"    => 2,   # CORE_PRIMITIVE_MOVE
    "convert"  => 16,  # CORE_PRIMITIVE_CONVERT
    "surge"    => 15,  # CORE_PRIMITIVE_SURGE
    "echo"     => 22,  # CORE_PRIMITIVE_ECHO
    "sense"    => 23,  # CORE_PRIMITIVE_SENSE
    "mirror"   => 18,  # CORE_PRIMITIVE_MIRROR
    "link"     => 4,   # CORE_PRIMITIVE_LINK
    "gate"     => 5,   # CORE_PRIMITIVE_GATE
    "shield"   => 25,  # CORE_PRIMITIVE_SHIELD
    "diverge"  => 21,  # CORE_PRIMITIVE_DIVERGE
    "unwrite"  => 20,  # CORE_PRIMITIVE_UNWRITE
    "forge"    => 12,  # CORE_PRIMITIVE_FORGE
    "recall"   => 14,  # CORE_PRIMITIVE_RECALL
    "reveal"   => 11,  # CORE_PRIMITIVE_REVEAL
    "weight"   => 24,  # CORE_PRIMITIVE_WEIGHT
    "order"    => 9,   # CORE_PRIMITIVE_ORDER
    "chance"   => 10,  # CORE_PRIMITIVE_CHANCE
    "scope"    => 8,   # CORE_PRIMITIVE_SCOPE
    "time"     => 7,   # CORE_PRIMITIVE_TIME
    "bond"     => 13,  # CORE_PRIMITIVE_BOND
    "inherit"  => 19,  # CORE_PRIMITIVE_INHERIT
  }.freeze
 
def self.primitive_id(name)
    PRIMITIVE_IDS[name.downcase]  
end

def self.primitive?(name)
    PRIMITIVE_IDS.key?(name.downcase)
end

## AST Node Types
# Every parsed .ch construct becomes one of these

module NodeType
    CHARACTER   = :character  
    ABILITY     = :ability
    RULE        = :rule
    STAT        = :stat
    STAT_BLOCK  = :stat_block
    STEP        = :step
    CONDITION   = :condition
    CHANCE      = :chance_node
end

## AST Node 
# A named node with children and metadata
# Rust reads this to compile bytecode

Node = Struct.new(:type, :name, :attrs, :children, :line) do
    def initialize(type, name: nil, attrs: {}, children: [], line: nil)
        super(type, name, attrs, children, line)
    end  

    def add_child(node)
        children << node
        self  
    end

    def attr(key) 
        attrs[key]
    end
    
    def to_h 
    {
        type: type,
        name: name,
        attrs: attrs,
        children: children.map(&:to_h),
        line: line
    }
    end
end

## Validator
# Checks AST nodes for common mistakes before Rust compiles it/sees it.
# Gives friendly error msgs not crypic compilier ones

module Validator

    def self.validate_character(node)
        errors = []
        
        errors << "character is missing a name buddy" if node.name.nil? || node.name.empty?
        stat_block = node.children.find { |c| c.type == NodeType::STAT_BLOCK }
        errors << "character '#{node.name}' missing hp stat" unless stat_block

        if stat_block
            hp = stat_block.children.find { |c| c.type == NodeType::STAT && c.name == "hp"}
            errors << "character '#{node.name}' missing hp stat" unless hp
        end

        errors
    end

    def self.validate_ability(node)
        errors = []

        errors << "ability is missing a name buddy" if node.name.nil? || node.name.empty?
        steps = node.children.select { |c| c.type == NodeType::STEP }
        errors << "ability '#{node.name}' has no steps" if steps.empty? 

        steps.each_with_index do |step, i|
            prim = step.attr(:primitive_id)  
            errors << "ability '#{node.name}' step #{i + 1} has unknown action '#{step.name}'" unless prim
        end

        errors
    end

    def self.validate(node)
        case node.type
        when NodeType::CHARACTER then validate_character(node)
        when NodeType::ABILITY then validate_ability(node)
        else []
        end  
    end

    def self.validate_all(nodes)
        nodes.flat_map {|n| validate(n)}  
    end
end

## JSON Emitter
#  Turns AST into JSON Rust can read
#  
module Emitter
    require "json"
    
    def self.emit(nodes)
        JSON.pretty_generate(nodes.map(&:to_h))  
    end

    def self.emit_to_file(nodes, path)
        File.write(path, emit(nodes))
        puts "[CH] Emitted #{nodes.size} node -> #{path}"  
    end
end

# C HEADER EMITTER
# Generates #define constants from the character stat nodes
# No Magic numbers in C code.

module CHeaderEmitter
    def self.emit(nodes, guard_name: "CH_GENERATED_H")
        lines = []
        lines << "/* auto-generated by chay - do not edit here */"
        lines << "#ifndef #{guard_name}"
        lines << "#define #{guard_name}"
        lines << ""

        nodes.each do |node|
            next unless node.type == NodeType::CHARACTER
            prefix = node.name.upcase.gsub(/\s+/, "_")

            stat_block = node.children.find { |c| c.type == NodeType::STAT_BLOCK }
            next unless stat_block

            stat_block.children.each do |stat|
                next unless stat.type == NodeType::STAT
                name = stat.name.upcase
                value = stat.attr(:value)
                lines << "#define #{prefix}_#{name} #{value}"
            end
            lines << ""
        end

        lines << "#endif /* #{guard_name} */"
        lines.join("\n")
    end

    def self.emit_to_file(nodes, path, guard_name = "CH_GENERATED_H")
        File.write(path, emit(nodes, guard_name: guard_name))
        puts "[CH] Generated C header -> #{path}"
    end
end

## Pipeline 
#  Ties everyting together
#  Call CH::Pipeline.run("file.ch") to process a .ch file.

module Pipeline
    def self.run(filepath, output_dir: ".") 
        puts "[CH] Processing and Reading #{filepath}"
        source = File.read(filepath)
        name = File.basename(filepath, ".ch")
            puts "[CH] Pipeline ready - tokenizer/parser slots stubbed"
    end
end
end

puts "=== CH Toolkit Test Start ==="

# -------------------------
# Test 1: Chance parsing
# -------------------------
puts "\n[Test 1] Chance.parse"

puts CH::Chance.parse("always")      # expect 1.0
puts CH::Chance.parse("never")       # expect 0.0
puts CH::Chance.parse("sometimes")   # expect 0.4
puts CH::Chance.parse("75%")         # expect 0.75
puts CH::Chance.parse("0.25")        # expect 0.25

# -------------------------
# Test 2: SourceReader basics
# -------------------------
puts "\n[Test 5] Token pattern detection"

src = "14hp 5speed 0.3 crit"
r = CH::SourceReader.new(src)

buffer = ""

until r.eof?
  ch = r.current

  if ch =~ /[0-9.]/
    buffer += ch
  elsif ch =~ /[a-zA-Z]/
    buffer += ch
  else
    puts "token=#{buffer}" unless buffer.empty?
    buffer = ""
  end

  r.advance
end

puts "token=#{buffer}" unless buffer.empty?