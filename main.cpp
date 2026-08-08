#include <iostream>
#include <string>
#include <sstream>
#include "DatabaseEngine.h"
#include "MainMenu.h"

using namespace std;


int main()
{
    DataManager dm;
    loadAllDatabases(dm);   // load customers.txt, bicycles.txt, rentals.txt into dm

    Customer currentCustomer;

    if(login(dm, currentCustomer) == 0) {
        menu(dm, currentCustomer);
    }
    menu(dm, currentCustomer);

    saveAllDatabases(dm);   // persist any changes before exiting
    return 0;
}