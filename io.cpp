//io.cpp 
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm> 
#include "calc.h"
using namespace calc;
using namespace std;

string IOHandler::read_file(const string &path) {
    ifstream ifs(path);
    if (!ifs) return string();
    std::ostringstream ss; ss << ifs.rdbuf();
    return ss.str();
}

vector<Question> IOHandler::split_questions(const string &file_text) {
    vector<Question> out;
    istringstream ss(file_text);
    string line;
    string cur;
    int base = 10;
    while (std::getline(ss, line)) {
        // trim
        auto trim = [](string &s){
            while(!s.empty() && isspace((unsigned char)s.front())) s.erase(s.begin());
            while(!s.empty() && isspace((unsigned char)s.back())) s.pop_back();
        };
        string tline = line;
        trim(tline);
        if (tline == "----") {
            if (!cur.empty()) {
                Question q; q.text = cur; q.base = base; out.push_back(q);
                cur.clear(); base = 10; // reset default base
            }
            continue;
        }
        // detect base header like "base=16" on a line alone
        string lower = tline;
        transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char c){ return std::tolower(c); });
        if (lower.rfind("base=", 0) == 0) {
            try { base = stoi(lower.substr(5)); } catch(...) { base = 10; }
            continue; // do not include header in question body
        }
        cur += line + "\n";
    }
    if (!cur.empty()) { Question q; q.text = cur; q.base = base; out.push_back(q); }
    return out;
}
void IOHandler::write_output(const std::string &path, const std::string &content) {
    ofstream ofs(path, ios::app); // append mode so multiple questions add up
    if (!ofs) {
        cerr << "ERROR: Could not open output file: " << path << "\n";
        return;
    }
    ofs << content << "\n";
}
