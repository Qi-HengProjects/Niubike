#ifndef MAINMENU_H
#define MAINMENU_H
#include "MainMenu.h"
#include "DatabaseEngine.h"
#include "Helpers.h"
#include "ReturnModule.h"
#include "History.h"
#include "RentalModule.h"
#include "PaymentModule.h"


int login(DataManager &dm, Customer &currentCustomer);
void menu(DataManager &dm, const Customer &currentCustomer);

#endif // MAINMENU_H