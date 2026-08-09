#include "DatabaseEngine.h"
#include "MainMenu.h"
#include <iostream>

using namespace std;

int main() {
    // 1. Initialize the central DataManager (lives in RAM for the session)
    DataManager dm;

    // 2. Load all customers, bicycles, and rentals from .txt files into RAM
    loadAllDatabases(dm);

    // 3. Variable to hold the currently logged-in customer's profile
    Customer currentCustomer;

    // 4. Main application loop
    // login() returns 0 when authentication or sign-up succeeds
    while (login(dm, currentCustomer) == 0) {
        // Launch main menu for the active customer
        menu(dm, currentCustomer);

        // Loop returns here when user selects Option 6 (Logout) in menu()
    }

    // 5. Save all data back to text files before exiting the program
    saveAllDatabases(dm);

    cout << "\nThank you for using the Bicycle Rental System! Goodbye." << endl;
    return 0;
}