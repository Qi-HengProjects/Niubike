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

string getCenteredString(const string& text, int width) {
    int padding = (width - static_cast<int>(text.length())) / 2;
    if (padding > 0) {
        return string(padding, ' ') + text;
    }
    return text;
}

void printCenteredBlock(const string& text, int width) {
    stringstream ss(text);
    string line;
    while (getline(ss, line)) {
        cout << getCenteredString(line, width) << '\n';
    }
}

void checkEofOrExit() {
    if (cin.eof()) {
        cout << "\n[!] Input stream closed unexpectedly. Exiting to avoid hanging...\n";
        exit(0);
    }
}

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