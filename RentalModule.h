#ifndef RENTAL_H
#define RENTAL_H

#include "DatabaseEngine.h"
#include "Structures.h"
#include <vector>
using namespace std;

// Displays bicycle menu & collects user choices
vector<int> rentalMenu(DataManager &dm);

// Processes duration, updates bicycle & rental database, prints invoice
void checkOut(DataManager &dm, const Customer &currentCustomer, const vector<int> &selectedRent);

#endif