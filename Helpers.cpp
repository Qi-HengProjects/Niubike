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