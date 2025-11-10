// include/calc.h 
#pragma once

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <unordered_map>

namespace calc 
{

// Basic error/result
struct Error 
{ 
    std::string message; size_t pos = 0; 
};

template<typename T>
struct Result
{ 
    T value;
    std::optional<Error> error;

    // Constructors
    Result() : value(), error(std::nullopt) {}
    Result(T v) : value(std::move(v)), error(std::nullopt) {}
    Result(Error e) : value(), error(e) {}
    Result(T v, Error e) : value(std::move(v)), error(e) {}

    // Move constructor
    Result(Result&& other) noexcept
        : value(std::move(other.value)), error(std::move(other.error)) {}

    bool ok() const { return !error.has_value(); }

    // Factory helpers
    static Result<T> Ok(T v) { return Result<T>(std::move(v)); }
    static Result<T> Err(const std::string &m, size_t p=0) {
        return Result<T>(T(), Error{m, p});
    }
};

// Token kinds
enum class TokenKind { Number, Ident, Plus, Minus, Star, Slash, Caret, LParen, RParen, Comma, Assign, End };
struct Token { TokenKind kind; std::string text; size_t pos = 0; };

// AST
struct ASTNode;
using ASTPtr = std::unique_ptr<ASTNode>;
struct ASTNode 
{
    enum class Type { Number, Variable, Binary, Unary, Function, Assign } type;
    // payload (only the fields that are used for the variant)
    double number{};
    std::string name;
    char op{};
    ASTPtr left, right;
    std::vector<ASTPtr> args;

    static ASTPtr make_number(double v);
    static ASTPtr make_number_text(const std::string &txt); // store textual representation (for base-aware parsing)
    static ASTPtr make_variable(const std::string &n);
    static ASTPtr make_binary(char op, ASTPtr l, ASTPtr r);
    static ASTPtr make_unary(char op, ASTPtr operand);
    static ASTPtr make_function(const std::string &fname, std::vector<ASTPtr> arguments);
    static ASTPtr make_assign(const std::string &varname, ASTPtr expr);

    // optional: store original text for number
    std::string number_text;
};

// Lexer
class Lexer 
{
public:
    explicit Lexer(const std::string &input);
    Token peek();
    Token next();
private:
    std::string input_;
    size_t pos_ = 0;
    Token lex_number_or_ident();
};

// Parser
class Parser 
{
public:
    explicit Parser(const std::vector<Token> &tokens);
    Result<ASTPtr> parse_statement(); // assignment or expression
private:
    const std::vector<Token> &tokens_;
    size_t idx_ = 0;
    Token peek() const;
    Token consume();
    Result<ASTPtr> parse_expression();
    Result<ASTPtr> parse_term();
    Result<ASTPtr> parse_factor();
    Result<ASTPtr> parse_primary();
};

// Context & evaluator
struct Context 
{
    int base = 10; // 2,10,16 supported
    std::unordered_map<std::string,double> vars;
    void set_var(const std::string &name, double value);
    std::optional<double> get_var(const std::string &name) const;
};

class Evaluator 
{
public:
    explicit Evaluator(Context &ctx);
    Result<double> eval(const ASTPtr &node);
private:
    Context &ctx_;
    Result<double> eval_number(const ASTPtr &node);
    Result<double> eval_variable(const ASTPtr &node);
    Result<double> eval_binary(const ASTPtr &node);
    Result<double> eval_unary(const ASTPtr &node);
    Result<double> eval_function(const ASTPtr &node);
    Result<double> eval_assign(const ASTPtr &node);
    // numeric parsing helper
    Result<double> parse_number_text(const std::string &text);
};

// IO & question processing
struct Question 
{ 
    std::string text; int base = 10; 
};

class IOHandler 
{
public:
    static std::string read_file(const std::string &path);
    static std::vector<Question> split_questions(const std::string &file_text); //input
    static void write_output(const std::string &path, const std::string &content); // output
};

class QuestionProcessor 
{
public:
    // process one question (multiple statements separated by newline or semicolon)
    Result<double> process(const Question &q);
};

} 
// namespace calc

