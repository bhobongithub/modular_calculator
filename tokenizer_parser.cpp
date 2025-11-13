//tokenizer_parser.cpp 
#include "calc.h"
#include <cctype>
#include <stdexcept>
using namespace calc;
using namespace std;

//AST factory helpers 
ASTPtr ASTNode::make_number(double v) { auto p = make_unique<ASTNode>(); p->type = Type::Number; p->number = v; return p; }
ASTPtr ASTNode::make_number_text(const string &txt) { auto p = make_unique<ASTNode>(); p->type = Type::Number; p->number_text = txt; return p; }
ASTPtr ASTNode::make_variable(const string &n) { auto p = make_unique<ASTNode>(); p->type = Type::Variable; p->name = n; return p; }
ASTPtr ASTNode::make_binary(char op, ASTPtr l, ASTPtr r) { auto p = make_unique<ASTNode>(); p->type = Type::Binary; p->op = op; p->left = std::move(l); p->right = std::move(r); return p; }
ASTPtr ASTNode::make_unary(char op, ASTPtr operand) { auto p = make_unique<ASTNode>(); p->type = Type::Unary; p->op = op; p->left = std::move(operand); return p; }
ASTPtr ASTNode::make_function(const string &fname, vector<ASTPtr> arguments) { auto p = make_unique<ASTNode>(); p->type = Type::Function; p->name = fname; p->args = std::move(arguments); return p; }
ASTPtr ASTNode::make_assign(const string &varname, ASTPtr expr) { auto p = make_unique<ASTNode>(); p->type = Type::Assign; p->name = varname; p->right = std::move(expr); return p; }

//Lexer 
Lexer::Lexer(const string &input) : input_(input), pos_(0) {}

static bool is_ident_start(char c) { return isalpha((unsigned char)c) || c == '_'; }
static bool is_ident_char(char c) { return isalnum((unsigned char)c) || c == '_'; }

Token Lexer::peek() { size_t save = pos_; Token t = next(); pos_ = save; return t; }




Token Lexer::next() {
    // Skip whitespace
    while (pos_ < input_.size() && isspace((unsigned char)input_[pos_])) ++pos_;
    if (pos_ >= input_.size()) return Token{TokenKind::End, "", pos_};

    char c = input_[pos_];
    size_t start = pos_;

    //Detect number literals 
    if (isdigit((unsigned char)c)) {
        string s;

        // handle binary or hex prefixes: 0b..., 0x...
        if (c == '0' && pos_ + 1 < input_.size()) {
            char n1 = tolower(input_[pos_ + 1]);
            if (n1 == 'b') {
                s += "0b";
                pos_ += 2;
                while (pos_ < input_.size() && (input_[pos_] == '0' || input_[pos_] == '1' || input_[pos_] == '_')) {
                    if (input_[pos_] != '_') s.push_back(input_[pos_]);
                    ++pos_;
                }
                return Token{TokenKind::Number, s, start};
            }
            if (n1 == 'x') {
                s += "0x";
                pos_ += 2;
                while (pos_ < input_.size() && (isxdigit((unsigned char)input_[pos_]) || input_[pos_] == '_')) {
                    if (input_[pos_] != '_') s.push_back(input_[pos_]);
                    ++pos_;
                }
                return Token{TokenKind::Number, s, start};
            }
        }

        // Decimal or floating point
        bool hasDot = false;
        while (pos_ < input_.size() &&
               (isdigit((unsigned char)input_[pos_]) || input_[pos_] == '.' || input_[pos_] == '_')) {
            if (input_[pos_] == '.') {
                if (hasDot) break; // second dot stops the number
                hasDot = true;
            }
            if (input_[pos_] != '_') s.push_back(input_[pos_]);
            ++pos_;
        }

        return Token{TokenKind::Number, s, start};
    }

    //Detect bare hex literals like FF0045 or FACE (me)
    if (isalpha((unsigned char)c)) {
        string s;
        while (pos_ < input_.size() && (isalnum((unsigned char)input_[pos_]) || input_[pos_] == '_')) {
            s.push_back(input_[pos_]);
            ++pos_;
        }

        // Check if token is ALL hex digits (0-9, A-F) and contains at least one A–F
        bool all_hex = !s.empty() &&
                       all_of(s.begin(), s.end(), [](unsigned char ch) {
                           return isxdigit(ch);
                       });
        bool has_hex_letter = any_of(s.begin(), s.end(), [](unsigned char ch) {
            char lower = tolower(ch);
            return lower >= 'a' && lower <= 'f';
        });

        if (all_hex && has_hex_letter) {
            // Implicit bare hex literal
            return Token{TokenKind::Number, "0x" + s, start};  // add 0x prefix for evaluator
        }
        //end of code I added

    //Identifiers (variables and functions like sin, cos, etc.)
    if (is_ident_start(c)) {
        string s;
        while (pos_ < input_.size() && is_ident_char(input_[pos_])) s.push_back(input_[pos_++]);
        return Token{TokenKind::Ident, s, start};
    }

    //Operators
    ++pos_;
    switch (c) {
        case '+': return Token{TokenKind::Plus, "+", start};
        case '-': return Token{TokenKind::Minus, "-", start};
        case '*': return Token{TokenKind::Star, "*", start};
        case '/': return Token{TokenKind::Slash, "/", start};
        case '^': return Token{TokenKind::Caret, "^", start};
        case '%': return Token{TokenKind::Percent, "%", start}; //new changes
        case '(': return Token{TokenKind::LParen, "(", start};
        case ')': return Token{TokenKind::RParen, ")", start};
        case ',': return Token{TokenKind::Comma, ",", start};
        case '=': return Token{TokenKind::Assign, "=", start};
        default:  return Token{TokenKind::End, string(1, c), start};
    }
}


// --- Parser ---
Parser::Parser(const vector<Token> &tokens) : tokens_(tokens), idx_(0) {}
Token Parser::peek() const { if (idx_ < tokens_.size()) return tokens_[idx_]; return Token{TokenKind::End, "", 0}; }
Token Parser::consume() { return (idx_ < tokens_.size())? tokens_[idx_++] : Token{TokenKind::End, "", 0}; }

Result<ASTPtr> Parser::parse_statement() {
    // If starts with identifier and next token is '=', treat as assignment
    if (peek().kind == TokenKind::Ident) {
        Token id = consume();
        if (peek().kind == TokenKind::Assign) {
            consume();
            auto rhs = parse_expression();
            if (!rhs.ok()) return Result<ASTPtr>::Err(rhs.error->message);
            return Result<ASTPtr>::Ok(ASTNode::make_assign(id.text, std::move(rhs.value)));
        }
        idx_--; // put back ident
    }
    return parse_expression();
}

// Recursive descent with precedence (parse_expression -> parse_term -> parse_factor -> primary)
Result<ASTPtr> Parser::parse_expression() {
    auto left_r = parse_term(); if (!left_r.ok()) return left_r;
    ASTPtr left = std::move(left_r.value);
    while (true) {
        Token t = peek();
        if (t.kind == TokenKind::Plus || t.kind == TokenKind::Minus) {
            consume();
            auto right_r = parse_term(); if (!right_r.ok()) return right_r;
            left = ASTNode::make_binary(t.text[0], std::move(left), std::move(right_r.value));
        } else break;
    }
    return Result<ASTPtr>::Ok(std::move(left));
}

Result<ASTPtr> Parser::parse_term() {
    auto left_r = parse_factor(); if (!left_r.ok()) return left_r;
    ASTPtr left = std::move(left_r.value);
    while (true) {
        Token t = peek();
        if (t.kind == TokenKind::Star || t.kind == TokenKind::Slash) {
            consume();
            auto right_r = parse_factor(); if (!right_r.ok()) return right_r;
            left = ASTNode::make_binary(t.text[0], std::move(left), std::move(right_r.value));
        } else break;
    }
    return Result<ASTPtr>::Ok(std::move(left));
}

Result<ASTPtr> Parser::parse_factor() {
    // handle exponentiation right-associative
    auto left_r = parse_primary(); if (!left_r.ok()) return left_r;
    ASTPtr left = std::move(left_r.value);
    if (peek().kind == TokenKind::Caret) {
        consume();
        auto right_r = parse_factor(); if (!right_r.ok()) return right_r;
        left = ASTNode::make_binary('^', std::move(left), std::move(right_r.value));
    }
    return Result<ASTPtr>::Ok(std::move(left));
}

Result<ASTPtr> Parser::parse_primary() {
    Token t = peek();
    if (t.kind == TokenKind::Number) {
        consume();
        return Result<ASTPtr>::Ok(ASTNode::make_number_text(t.text));
    }
    if (t.kind == TokenKind::Ident) {
        consume();
        // function call
        if (peek().kind == TokenKind::LParen) {
            // parse single-arg function or comma-separated args
            string fname = t.text;
            consume(); // '('
            vector<ASTPtr> args;
            if (peek().kind != TokenKind::RParen) {
                while (true) {
                    auto a = parse_expression(); if (!a.ok()) return a;
                    args.push_back(std::move(a.value));
                    if (peek().kind == TokenKind::Comma) { consume(); continue; }
                    break;
                }
            }
            if (peek().kind != TokenKind::RParen) return Result<ASTPtr>::Err("Expected ')'");
            consume();
            return Result<ASTPtr>::Ok(ASTNode::make_function(fname, std::move(args)));
        }
        // variable
        return Result<ASTPtr>::Ok(ASTNode::make_variable(t.text));
    }
    if (t.kind == TokenKind::LParen) { consume(); auto e = parse_expression(); if (!e.ok()) return e; if (peek().kind!=TokenKind::RParen) return Result<ASTPtr>::Err("Expected )"); consume(); return e; }
    if (t.kind == TokenKind::Minus) { consume(); auto p = parse_primary(); if (!p.ok()) return p; return Result<ASTPtr>::Ok(ASTNode::make_unary('-', std::move(p.value))); }
    return Result<ASTPtr>::Err("Unexpected token in primary");
}
