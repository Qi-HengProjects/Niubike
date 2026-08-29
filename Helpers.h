#ifndef NIUBIKE_HELPERS_H
#define NIUBIKE_HELPERS_H
#include <string>
#include "Structures.h"

using namespace std;

void clearScreen();
string getCenteredString(const string& text, int width = 80);
void printCenteredBlock(const string& text, int width = 80);
void checkEofOrExit();
string sanitizeForCsv(const string& field);

#endif //NIUBIKE_HELPERS_H