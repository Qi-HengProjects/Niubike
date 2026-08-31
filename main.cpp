#include "DatabaseEngine.h"
#include "MainMenu.h"
#include "Admin.h"
#include <iostream>

using namespace std;

int main() {
    // Initialize the central DataManager (lives in RAM for the session)
    DataManager dm;

    // Load all customers, bicycles, and rentals from .txt files into RAM
    loadAllDatabases(dm);

    // Variable to hold the currently logged-in customer's profile
    Customer currentCustomer;

    // Main application loop
    // login() returns 0 when authentication or sign-up succeeds
    
    while (true) {
        int result = login(dm, currentCustomer);
        if (result == 0) {
            menu(dm, currentCustomer);
        } else if (result == 1) {
            admin(dm);
        } else {
            // Option to exit program entirely from login screen
            break; 
        }
    }
    
    

    // Save all data back to text files before exiting the program
    saveAllDatabases(dm);

    cout << "\nThank you for using the Bicycle Rental System! Goodbye." << endl;
    return 0;
}