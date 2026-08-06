#include "rental.h"
#include "DatabaseEngine.h"
#include "Helpers.h"
#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <algorithm>

using namespace std;

// Helper to strip whitespace, tabs, and hidden Windows carriage returns (\r)
static string trim(const string &s) {
    size_t first = s.find_first_not_of(" \t\r\n");
    if (first == string::npos) return "";
    size_t last = s.find_last_not_of(" \t\r\n");
    return s.substr(first, (last - first + 1));
}

// Count stock from dm.bicycles directly
static int getAvailableCount(const DataManager &dm, const string &bikeType) {
    int count = 0;
    for (const auto &b : dm.bicycles) {
        string status = trim(b.status);
        string type = trim(b.bikeType);

        if (type == bikeType && status == "Available") {
            count++;
        }
    }
    return count;
}

vector<int> rentalMenu(DataManager &dm) {
    vector<int> selectedRent;

    const string categories[5] = {
        "Regular",
        "Two Seater",
        "E-Bike",
        "Kids Bike",
        "City Bike"
    };

    int bikeOpt = -1;

    while (bikeOpt != 0) {
        clearScreen();

        // Rely directly on dm.bicycles loaded by loadAllDatabases(dm) in main()
        int availRegular   = getAvailableCount(dm, "Regular");
        int availTwoSeater = getAvailableCount(dm, "Two Seater");
        int availEBike     = getAvailableCount(dm, "E-Bike");
        int availKidsBike  = getAvailableCount(dm, "Kids Bike");
        int availCityBike  = getAvailableCount(dm, "City Bike");

        // Deduct bikes selected in current session cart
        for (int sel : selectedRent) {
            if (sel == 1) availRegular--;
            if (sel == 2) availTwoSeater--;
            if (sel == 3) availEBike--;
            if (sel == 4) availKidsBike--;
            if (sel == 5) availCityBike--;
        }

        string border  = "=========================================================";
        string divider = "-----------+--------------+----------+-------------------";

        cout << getCenteredString(border, 165) << endl;
        cout << getCenteredString("BICYCLE RENTAL MENU", 165) << endl;
        cout << getCenteredString(border, 165) << endl;
        cout << getCenteredString(" Option |   Category   | Price/hr | Available Stock    ", 170) << endl;
        cout << getCenteredString(divider, 165) << endl;

        string row1 = "   1.   | Regular      | $5.00    | " + to_string(availRegular)   + (availRegular   > 0 ? " Available   " : " OUT OF STOCK");
        string row2 = "   2.   | Two Seater   | $10.00   | " + to_string(availTwoSeater) + (availTwoSeater > 0 ? " Available   " : " OUT OF STOCK");
        string row3 = "   3.   | E-Bike       | $12.00   | " + to_string(availEBike)     + (availEBike     > 0 ? " Available   " : " OUT OF STOCK");
        string row4 = "   4.   | Kids Bike    | $4.00    | " + to_string(availKidsBike)  + (availKidsBike  > 0 ? " Available   " : " OUT OF STOCK");
        string row5 = "   5.   | City Bike    | $7.00    | " + to_string(availCityBike)  + (availCityBike  > 0 ? " Available   " : " OUT OF STOCK");

        cout << getCenteredString(row1, 165) << endl;
        cout << getCenteredString(row2, 165) << endl;
        cout << getCenteredString(row3, 165) << endl;
        cout << getCenteredString(row4, 165) << endl;
        cout << getCenteredString(row5, 165) << endl;

        cout << getCenteredString(border, 165) << endl;

        string selectedSummary = " Selected so far: " + to_string(selectedRent.size()) + " bike(s)";
        cout << getCenteredString(selectedSummary, 165) << endl;
        cout << getCenteredString(border, 165) << endl << endl;

        cout << getCenteredString("Enter option (1-5, or 0 to finish selection): ", 165);

        if (!(cin >> bikeOpt)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "\nInvalid input! Please enter a number.\n";
            cout << "Press Enter to continue...";
            cin.get();
            continue;
        }

        if (bikeOpt == 0) break;

        if (bikeOpt >= 1 && bikeOpt <= 5) {
            string chosenCategory = categories[bikeOpt - 1];
            int currentAvail = 0;
            if (bikeOpt == 1) currentAvail = availRegular;
            else if (bikeOpt == 2) currentAvail = availTwoSeater;
            else if (bikeOpt == 3) currentAvail = availEBike;
            else if (bikeOpt == 4) currentAvail = availKidsBike;
            else if (bikeOpt == 5) currentAvail = availCityBike;

            if (currentAvail <= 0) {
                cout << "\n[!] Sorry, " << chosenCategory << " is currently OUT OF STOCK!\n";
                cout << "Press Enter to continue...";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cin.get();
            } else {
                selectedRent.push_back(bikeOpt);
                cout << "\n[+] Successfully added " << chosenCategory << " to your rental cart.\n";
                cout << "Press Enter to continue...";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cin.get();
            }
        } else {
            cout << "\nInvalid choice! Please select 1-5 or 0 to exit.\n";
            cout << "Press Enter to continue...";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.get();
        }
    }

    return selectedRent;
    
}



void printInvoice(DataManager &dm, const vector<int> &selectedRent) {
    if (selectedRent.empty()) {
        cout << "\nNo bicycles were selected. Returning to menu...\n";
        return;
    }

    clearScreen();

    // 1. Prompt for rental duration in hours
    int hours = 0;
    while (true) {
        cout << getCenteredString("=========================================================", 165) << endl;
        cout << getCenteredString("RENTAL DURATION", 165) << endl;
        cout << getCenteredString("=========================================================", 165) << endl << endl;
        cout << getCenteredString("Enter rental duration (in hours): ", 165);
        
        if (cin >> hours && hours > 0) {
            break;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "\n[!] Invalid duration! Please enter a positive whole number.\n\n";
    }

    // 2. Map option numbers to Category Names and Prices
    const string categoryNames[6] = { "", "Regular", "Two Seater", "E-Bike", "Kids Bike", "City Bike" };
    const double categoryPrices[6] = { 0.0, 5.00, 10.00, 12.00, 4.00, 7.00 };

    // 3. Match selections to actual available bicycles in database and assign IDs
    vector<string> assignedBikeIds;
    vector<string> assignedCategories;
    vector<double> assignedRates;

    for (int opt : selectedRent) {
        string targetCat = categoryNames[opt];
        
        // Find the first matching available bike in dm.bicycles
        for (auto &b : dm.bicycles) {
            if (trim(b.bikeType) == targetCat && trim(b.status) == "Available") {
                b.status = "Rented"; // Mark as rented
                assignedBikeIds.push_back(b.bikeId);
                assignedCategories.push_back(b.bikeType);
                assignedRates.push_back(b.price);
                break;
            }
        }
    }

    // Persist changes to bicycles.txt
    saveBicycles(dm.bicycles);

    // 4. Calculate Financials
    double subtotal = 0.0;
    const double DEPOSIT_PER_BIKE = 15.00; // Refundable security deposit
    int totalBikes = static_cast<int>(assignedBikeIds.size());

    clearScreen();

    string border  = "===============================================================================";
    string divider = "-----------+--------------+------------------+----------+----------+-----------";

    cout << getCenteredString(border, 165) << endl;
    cout << getCenteredString("OFFICIAL RENTAL INVOICE & RECEIPT", 165) << endl;
    cout << getCenteredString(border, 165) << endl;

    // Header row
    ostringstream headerSS;
    headerSS << " " << left << setw(7) << "Bike ID"
             << "| " << setw(13) << "Category"
             << "| " << setw(17) << "Rate ($/hr)"
             << "| " << setw(9) << "Hours"
             << "| " << setw(9) << "Deposit"
             << "| " << "Line Total ($)";
    cout << getCenteredString(headerSS.str(), 165) << endl;
    cout << getCenteredString(divider, 165) << endl;

    // Print itemized rows
    for (size_t i = 0; i < assignedBikeIds.size(); i++) {
        double lineTotal = assignedRates[i] * hours;
        subtotal += lineTotal;

        ostringstream rowSS;
        rowSS << " " << left << setw(7) << assignedBikeIds[i]
              << "| " << setw(13) << assignedCategories[i]
              << "| $" << setw(16) << fixed << setprecision(2) << assignedRates[i]
              << "| " << setw(2) << hours << " hrs   "
              << "| $" << setw(8) << fixed << setprecision(2) << DEPOSIT_PER_BIKE
              << "| $" << fixed << setprecision(2) << lineTotal;

        cout << getCenteredString(rowSS.str(), 165) << endl;
    }

    double totalDeposit = DEPOSIT_PER_BIKE * totalBikes;
    double grandTotal = subtotal + totalDeposit;

    cout << getCenteredString(divider, 165) << endl;

    // Summary calculations display
    ostringstream ssSub, ssDep, ssGrand;
    ssSub << "Rental Subtotal (" << totalBikes << " bike/s x " << hours << " hrs):     $" << fixed << setprecision(2) << subtotal;
    ssDep << "Refundable Deposit ($" << fixed << setprecision(2) << DEPOSIT_PER_BIKE << " x " << totalBikes << " bike/s): $" << fixed << setprecision(2) << totalDeposit;
    ssGrand << "TOTAL AMOUNT DUE: $" << fixed << setprecision(2) << grandTotal;

    cout << getCenteredString(ssSub.str(), 135) << endl;
    cout << getCenteredString(ssDep.str(), 135) << endl;
    cout << getCenteredString(border, 165) << endl;
    cout << getCenteredString(ssGrand.str(), 165) << endl;
    cout << getCenteredString(border, 165) << endl << endl;

    cout << getCenteredString("[+] Rental confirmed! Status updated in database.", 165) << endl;
    cout << getCenteredString("Press Enter to return to main menu...", 165);
    
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}