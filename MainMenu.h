#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include "DatabaseEngine.h"

int login(DataManager &dm, Customer &currentCustomer);
void menu(DataManager &dm, const Customer &currentCustomer);

#endif