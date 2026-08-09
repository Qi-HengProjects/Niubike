#include "ReturnModule.h"
#include "DatabaseEngine.h"
#include "Helpers.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <string>
#include <vector>

using namespace std;

// Helper to trim whitespace
static string trimString(const string &s) {
    size_t first = s.find_first_not_of(" \t\r\n");
    if (first == string::npos) return "";
    size_t last = s.find_last_not_of(" \t\r\n");
    return s.substr(first, (last - first + 1));
}

void returnBikeLogic(DataManager &dm, const Customer &currentCustomer) {
    clearScreen();
    bool foundActive = false;

    for (auto &r : dm.rentals) {
        // Find active rental belonging to current customer
        if (trimString(r.rentingStatus) == "Active" && trimString(r.custId) == currentCustomer.customerId) {
            foundActive = true;

            cout << "============================================" << endl;
            cout << "                RETURN BIKES                " << endl;
            cout << "============================================" << endl;
            cout << "Rental ID : " << r.rentalId << endl;
            cout << "Bicycles to Return:" << endl;

            // 1. Clean single loop to display bike IDs
            for (size_t i = 0; i < r.bikeIdsStr.size(); i++) {
                cout << "  " << (i + 1) << ". " << r.bikeIdsStr[i] << endl;
            }

            int returnChoice = 0;
            // 2. Fixed validation condition using &&
            do {
                cout << "\nAre you returning these bike(s)?\n1. Yes\n2. No\nOption: ";
                if (!(cin >> returnChoice)) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
            } while (returnChoice != 1 && returnChoice != 2);

            if (returnChoice == 1) {
                // 3. Mark each bike as "Available" in dm.bicycles
                for (const string &id : r.bikeIdsStr) {
                    for (auto &b : dm.bicycles) {
                        if (trimString(b.bikeId) == trimString(id)) {
                            b.status = "Available";
                            break;
                        }
                    }
                }

                // 4. Update rental status to "Returned" (keep r.bikeIds intact for History!)
                r.rentingStatus = "Returned";

                // 5. Persist changes to disk
                saveBicycles(dm.bicycles);
                saveRentals(dm.rentals);

                cout << "\n[+] All bikes returned successfully!" << endl;
                cout << "[+] Please claim your refundable deposit: $" << fixed << setprecision(2) << r.deposit << endl;
            } else {
                cout << "\nReturn cancelled. Your rental remains active." << endl;
            }

            cout << "\nPress Enter to return to main menu...";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.get();
            return; // Exit after processing active rental
        }
    }

    if (!foundActive) {
        cout << "No active rentals found for your account.\n\n";
        cout << "Press Enter to return to main menu...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
    }
}