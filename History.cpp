#include "History.h"
#include "Helpers.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <limits>

using namespace std;

string History::trimStr(const string &s) const {
    size_t first = s.find_first_not_of(" \t\r\n");
    if (first == string::npos) return "";
    size_t last = s.find_last_not_of(" \t\r\n");
    return s.substr(first, (last - first + 1));
}

void History::displayUserHistory(const DataManager &dm, const string &currentCustId) const {
    clearScreen();

    // Table width: 85 characters
    string border  = "=====================================================================================";
    string divider = "--------+---------------+-------------------+-----------------+------------+-------------";

    cout << getCenteredString(border, 165) << endl;
    cout << getCenteredString("YOUR RENTAL HISTORY", 165) << endl;
    cout << getCenteredString(border, 165) << endl;

    bool foundAny = false;

    // Table Header
    ostringstream headerSS;
    headerSS << " " << left << setw(7) << "ID"
             << "| " << setw(13) << "Duration"
             << "| " << setw(17) << "Payment Status"
             << "| " << setw(15) << "Rental Status"
             << "| " << setw(10) << "Price"
             << "| " << "Bikes Rented";

    cout << getCenteredString(headerSS.str(), 165) << endl;
    cout << getCenteredString(divider, 165) << endl;

    // Loop through database and display matching customer rentals
    for (const auto &r : dm.rentals) {
        if (trimStr(r.custId) == trimStr(currentCustId)) {
            foundAny = true;

            // Format bike IDs list (e.g., "B001, B002")
            string bikesFormatted = "";
            for (size_t i = 0; i < r.bikeIdsStr.size(); ++i) {
                bikesFormatted += trimStr(r.bikeIdsStr[i]);
                if (i + 1 < r.bikeIdsStr.size()) {
                    bikesFormatted += ", ";
                }
            }

            ostringstream rowSS;
            rowSS << " " << left << setw(7) << trimStr(r.rentalId)
                  << "| " << setw(13) << trimStr(r.rentalDuration)
                  << "| " << setw(17) << trimStr(r.paymentStatus)
                  << "| " << setw(15) << trimStr(r.rentingStatus)
                  << "| $" << setw(9) << fixed << setprecision(2) << r.rentingPrice
                  << "| " << bikesFormatted;

            cout << getCenteredString(rowSS.str(), 165) << endl;
        }
    }

    if (!foundAny) {
        cout << endl;
        cout << getCenteredString("[!] No rental history found. Please rent a bike first!", 165) << endl;
        cout << endl;
    }

    cout << getCenteredString(border, 165) << endl << endl;
    cout << getCenteredString("Press Enter to return to main menu...", 165);

    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}