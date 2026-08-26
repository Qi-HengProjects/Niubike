#ifndef RETURNMODULE_H
#define RETURNMODULE_H
#include "RentalModule.h"
#include "DatabaseEngine.h"
#include "Helpers.h"
#include "MainMenu.h"
#include "DatabaseEngine.h"
void returnBikeLogic();

void returnBikeLogic(DataManager &dm, const Customer &currentCustomer);

bool topUpRental(DataManager &dm, const string &rentalId, int extraHours);
void handleTopUpMenu(DataManager &dm, const Customer &currentCustomer);

#endif // RETURNMODULE_H