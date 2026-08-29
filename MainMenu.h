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
// Non-const so the Loyalty Program screen can flip isMember on for the
// current session as soon as the customer joins, without requiring a re-login.
void menu(DataManager &dm, Customer &currentCustomer);

#endif // MAINMENU_H