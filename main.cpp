#include <iostream>
#include <string>
#include <sstream>
#include "DatabaseEngine.h"
#include "MainMenu.h"

using namespace std;

int main()
{
    DataManager dm;
    loadAllDatabases(dm);   // Load customers, bicycles, rentals

    Customer currentCustomer;

    if (login(dm, currentCustomer) == 0) {
        menu(dm, currentCustomer);
    }

    saveAllDatabases(dm);   // Save before exit
    return 0;
}