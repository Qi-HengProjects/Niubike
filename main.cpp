#include <iostream>
#include <string>
#include <sstream>
#include "DatabaseEngine.h"
#include "UserInterface.h"

using namespace std;


int main()
{
    DataManager dm;
    loadAllDatabases(dm);   // load customers.txt, bicycles.txt, rentals.txt into dm

    login(dm);
    menu();

    saveAllDatabases(dm);   // persist any changes before exiting
    return 0;
}