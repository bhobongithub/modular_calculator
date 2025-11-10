// processor.cpp 
#include "calc.h"
#include <sstream>

using namespace calc;
using namespace std;

Result<double> QuestionProcessor::process(const Question &q) 
{
    Context ctx; ctx.base = q.base;
    Evaluator eval(ctx);
    istringstream ss(q.text);
    string line;
    double last = 0;
    bool any = false;
    while (std::getline(ss, line)) 
    {
        // trim
        auto trim = [](string &s){ while(!s.empty() && isspace((unsigned char)s.front())) s.erase(s.begin()); while(!s.empty() && isspace((unsigned char)s.back())) s.pop_back(); };
        trim(line);
        if (line.empty()) continue;
        // lex
        Lexer lx(line);
        vector<Token> tokens;
        while (true) {
            Token t = lx.next();
            tokens.push_back(t);
            if (t.kind == TokenKind::End) break;
        }
        Parser p(tokens);
        auto ast_r = p.parse_statement();
        if (!ast_r.ok()) return Result<double>::Err(ast_r.error->message);
        auto res = eval.eval(ast_r.value);
        if (!res.ok()) return res;
        last = res.value; any = true;
    }
    if (!any) return Result<double>::Err("Empty question");
    return Result<double>::Ok(last);
}