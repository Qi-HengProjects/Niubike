#include "DatabaseEngine.h"
#include <iostream>
#include <sstream>
using namespace std;

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

//print stuff with padding
string getCenteredString(const string& text, int width) {
    int padding = (width - static_cast<int>(text.length())) / 2;
    if (padding > 0) {
        return string(padding, ' ') + text;
    }
    return text;
}

//print a block of stuff with padding
void printCenteredBlock(const string& text, int width) {
    stringstream ss(text);
    string line;
    while (getline(ss, line)) {
        cout << getCenteredString(line, width) << '\n';
    }
}

// Prevents infinite loop execution when input streams close or scripts hit end-of-file.
void checkEofOrExit() {
    if (cin.eof()) {
        cout << "\n[!] Input stream closed unexpectedly. Exiting to avoid hanging...\n";
        exit(0);
    }
}

//Replaces commas (',') with semicolons (';') and strips line breaks ('\n', '\r') to avoid breaking row or column structure.

string sanitizeForCsv(const string& field) {
    string clean;
    clean.reserve(field.size());
    for (char c : field) {
        if (c == ',') {
            clean += ';';       // commas would shift CSV columns -> swap for a safe separator
        } else if (c == '\n' || c == '\r') {
            clean += ' ';        // embedded newlines would break the one-row-per-line format
        } else {
            clean += c;
        }
    }
    return clean;
}