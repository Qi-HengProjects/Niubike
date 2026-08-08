#ifndef MAINMENU_H
#define MAINMENU_H

#include "DatabaseEngine.h"

int login(DataManager &dm, Customer &currentCustomer);
void menu(DataManager &dm, const Customer &currentCustomer);

#endif // MAINMENU_H