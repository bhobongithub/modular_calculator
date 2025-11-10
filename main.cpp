// main.cpp 
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>
#include "calc.h"
using namespace std;
using namespace calc;

int main(int argc, char** argv) 
{
    cout << "Modular Calculator - reading questions separated by '----'\n";

    string inputPath = (argc > 1) ? argv[1] : "input.txt";
    string outputPath = "output.txt"; 
    ofstream clearFile(outputPath, ios::trunc);
    clearFile.close();

    string file = IOHandler::read_file(inputPath);
    if (file.empty()) 
    {
        cerr << "ERROR: Empty or unreadable input file: " << inputPath << "\n";
        return 1;
    }

    auto questions = IOHandler::split_questions(file);
    size_t qn = 0;
    for (auto &q : questions) 
    {
        qn++;
        cout << "--- Question " << qn << " (base=" << q.base << ") ---\n";
        QuestionProcessor proc;
        auto res = proc.process(q);

        std::ostringstream outText;
        outText << "--- Question " << qn << " (base=" << q.base << ") ---\n";
        outText << q.text << "\n";

        if (res.ok()) {
            cout << fixed << setprecision(12) << res.value << "\n";
            outText << "Answer: " << fixed << setprecision(12) << res.value << "\n";
        } else {
            cerr << "Error: " << res.error->message << "\n";
            outText << "Error: " << res.error->message << "\n";
        }

        outText << "----------------------------------------\n";
        IOHandler::write_output(outputPath, outText.str());
    }

    cout << "\nResults saved to " << outputPath << "\n";
    return 0;
}

