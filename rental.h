#ifndef RENTAL_H
#define RENTAL_H

#include "Structures.h"
#include <vector>

// Displays bicycle menu & collects user choices
std::vector<int> rentalMenu(DataManager &dm);

// Processes duration, updates bicycle & rental database, prints invoice
void printInvoice(DataManager &dm, const std::vector<int> &selectedRent);

#endif