#ifndef RENTAL_H
#define RENTAL_H

#include "DatabaseEngine.h"
#include "Structures.h"
#include <vector>
#include <string>

using namespace std;

// Displays bicycle menu & collects user choices
vector<int> rentalMenu(DataManager &dm);

// Processes duration, updates bicycle & rental database, prints invoice
void checkOut(DataManager &dm, const Customer &currentCustomer, const vector<int> &selectedRent);

// Top-up functions
bool topUpRental(DataManager &dm, const string &rentalId, int extraHours);
void handleTopUpMenu(DataManager &dm, const Customer &currentCustomer);

// Return bicycle function
void returnBicycles(DataManager &dm, const Customer &currentCustomer);

#endif