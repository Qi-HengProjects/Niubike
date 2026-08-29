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

// Cancels an Active, not-yet-overdue rental: releases its bike(s) back to
// "Available" stock and marks the rental Cancelled/Refunded as appropriate.
bool cancelRental(DataManager &dm, const string &rentalId);
void handleCancelMenu(DataManager &dm, const Customer &currentCustomer);

#endif