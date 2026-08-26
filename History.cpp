#include "History.h"
#include "Helpers.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <limits>

using namespace std;

// Helper to strip whitespace, tabs, and hidden Windows carriage returns (\r)
static string trimString(const string &s) {
    size_t first = s.find_first_not_of(" \t\r\n");
    if (first == string::npos) return "";
    size_t last = s.find_last_not_of(" \t\r\n");
    return s.substr(first, (last - first + 1));
}

// Pure procedural function definition (no History:: prefix)
void displayUserHistory(const DataManager &dm, const string &customerId) {
    clearScreen();

    string border  = "====================================================================================================";
    string divider = "----------+--------------+------------------+---------------+--------------+-------------------------";

    cout << getCenteredString(border, 165) << endl;
    cout << getCenteredString("RENTAL TRANSACTION HISTORY", 165) << endl;
    cout << getCenteredString(border, 165) << endl;

    bool hasHistory = false;

    // Header row
    ostringstream headerSS;
    headerSS << " " << left << setw(9) << "Rental ID"
             << "| " << setw(13) << "Duration"
             << "| " << setw(17) << "Payment Status"
             << "| " << setw(14) << "Renting Status"
             << "| " << setw(13) << "Total Amount"
             << "| " << "Bikes Rented";
    cout << getCenteredString(headerSS.str(), 165) << endl;
    cout << getCenteredString(divider, 165) << endl;

    // Loop through all rentals in RAM and filter by customer ID
    for (const auto &r : dm.rentals) {
        if (trimString(r.custId) == trimString(customerId)) {
            hasHistory = true;

            // Join vector<string> bikeIds into a readable comma-separated string
            string bikesStr = "";
            for (size_t i = 0; i < r.bikeIdsStr.size(); ++i) {
                bikesStr += trimString(r.bikeIdsStr[i]);
                if (i + 1 < r.bikeIdsStr.size()) {
                    bikesStr += ", ";
                }
            }

            double totalAmount = r.rentingPrice + r.deposit;

            ostringstream rowSS;
            rowSS << " " << left << setw(9) << trimString(r.rentalId)
                  << "| " << setw(13) << trimString(r.rentalDuration)
                  << "| " << setw(17) << trimString(r.paymentStatus)
                  << "| " << setw(14) << trimString(r.rentingStatus)
                  << "| $" << setw(12) << fixed << setprecision(2) << totalAmount
                  << "| " << bikesStr;

            cout << getCenteredString(rowSS.str(), 165) << endl;
        }
    }

    if (!hasHistory) {
        cout << getCenteredString("No rental history found for your account.", 165) << endl;
    }

    cout << getCenteredString(border, 165) << endl << endl;
    cout << getCenteredString("Press Enter to return to main menu...", 165);

    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}