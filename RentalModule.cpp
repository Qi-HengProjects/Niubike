#include "RentalModule.h"
#include "DatabaseEngine.h"
#include "PaymentModule.h"
#include "Helpers.h"
#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <algorithm>
#include <iomanip>
#include <sstream>

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

void checkOut(DataManager &dm, const Customer &currentCustomer, const vector<int> &selectedRent) {
    if (selectedRent.empty()) {
        cout << "\nNo bicycles were selected. Returning to menu...\n";
        return;
    }

    clearScreen();

    int hours = 0;
    while (true) {
        cout << getCenteredString("=========================================================", 165) << endl;
        cout << getCenteredString("RENTAL DURATION", 165) << endl;
        cout << getCenteredString("=========================================================", 165) << endl << endl;
        cout << getCenteredString("Enter rental duration (in hours): ", 165);
        
        if (cin >> hours && hours > 0) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            break;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "\n[!] Invalid duration! Please enter a positive whole number.\n\n";
    }

    const string categoryNames[6] = { "", "Regular", "Two Seater", "E-Bike", "Kids Bike", "City Bike" };

    vector<string> assignedBikeIds;
    vector<string> assignedCategories;
    vector<double> assignedRates;

    for (int opt : selectedRent) {
        string targetCat = categoryNames[opt];
        
        for (auto &b : dm.bicycles) {
            if (trim(b.bikeType) == targetCat && trim(b.status) == "Available") {
                b.status = "Rented";
                assignedBikeIds.push_back(b.bikeId);
                assignedCategories.push_back(b.bikeType);
                assignedRates.push_back(b.price);
                break;
            }
        }
    }

    saveBicycles(dm.bicycles);

    double subtotal = 0.0;
    const double DEPOSIT_PER_BIKE = 15.00;
    int totalBikes = static_cast<int>(assignedBikeIds.size());

    clearScreen();

    string border  = "===============================================================================";
    string divider = "-----------+--------------+------------------+----------+----------+-----------";

    cout << getCenteredString(border, 165) << endl;
    cout << getCenteredString("CART", 165) << endl;
    cout << getCenteredString(border, 165) << endl;

    ostringstream headerSS;
    headerSS << " " << left << setw(7) << "Bike ID"
             << "| " << setw(13) << "Category"
             << "| " << setw(17) << "Rate ($/hr)"
             << "| " << setw(9) << "Hours"
             << "| " << setw(9) << "Deposit"
             << "| " << "Line Total ($)";
    cout << getCenteredString(headerSS.str(), 165) << endl;
    cout << getCenteredString(divider, 165) << endl;

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

    ostringstream ssSub, ssDep, ssGrand;
    ssSub << "Rental Subtotal (" << totalBikes << " bike/s x " << hours << " hrs):     $" << fixed << setprecision(2) << subtotal;
    ssDep << "Refundable Deposit ($" << fixed << setprecision(2) << DEPOSIT_PER_BIKE << " x " << totalBikes << " bike/s): $" << fixed << setprecision(2) << totalDeposit;
    ssGrand << "TOTAL AMOUNT DUE: $" << fixed << setprecision(2) << grandTotal;

    cout << getCenteredString(ssSub.str(), 135) << endl;
    cout << getCenteredString(ssDep.str(), 135) << endl;
    cout << getCenteredString(border, 165) << endl;
    cout << getCenteredString(ssGrand.str(), 165) << endl;
    cout << getCenteredString(border, 165) << endl << endl;

    Rental newR;
    newR.rentalId = "R" + to_string(dm.rentals.size() + 1);
    newR.rentalDuration = to_string(hours) +  " hours";
    newR.paymentStatus = "Pending";
    newR.rentingStatus = "Active";
    newR.rentingPrice  = subtotal;
    newR.deposit = totalDeposit;
    newR.amountPaid = 0.0;   
    newR.custId = currentCustomer.customerId;
    newR.bikeIdsStr = assignedBikeIds;

    dm.rentals.push_back(newR);
    saveRentals(dm.rentals);

    cout << getCenteredString("[+] Rental confirmed! Status updated in database. Please make your payment at the payment page.", 165) << endl;
    cout << getCenteredString("Press Enter to return to main menu...", 165);
    cin.get(); 
}

bool topUpRental(DataManager &dm, const string &rentalId, int extraHours) {
    if (extraHours <= 0) {
        cout << getCenteredString("[Error] Extra hours must be greater than 0.", 165) << "\n";
        return false;
    }

    Rental* record = findRentalById(dm, rentalId);
    if (!record) {
        cout << getCenteredString("[Error] Rental ID " + rentalId + " not found.", 165) << "\n";
        return false;
    }

    if (trim(record->rentingStatus) != "Active") {
        cout << getCenteredString("[Error] Cannot top up hours for an already returned/completed rental.", 165) << "\n";
        return false;
    }

    if (record->bikeIdsStr.empty()) {
        cout << getCenteredString("[Error] No bicycles associated with this rental.", 165) << "\n";
        return false;
    }

    double totalHourlyRate = 0.0;
    for (const auto &bikeId : record->bikeIdsStr) {
        Bicycle* bike = findBicycleById(dm, bikeId);
        if (bike) {
            totalHourlyRate += bike->price;
        }
    }

    double addedCost = extraHours * totalHourlyRate;

    int currentHours = 0;
    stringstream durSS(record->rentalDuration);
    durSS >> currentHours;
    int newHours = currentHours + extraHours;

    record->rentalDuration = to_string(newHours) + " hours";
    record->rentingPrice += addedCost;
    record->paymentStatus = "Pending";

    saveRentals(dm.rentals);

    stringstream feeSS, costSS;
    feeSS << fixed << setprecision(2) << addedCost;
    costSS << fixed << setprecision(2) << record->rentingPrice;

    cout << "\n";
    cout << getCenteredString("========================================", 165) << "\n";
    cout << getCenteredString("        TOP-UP SUCCESSFUL!              ", 165) << "\n";
    cout << getCenteredString("========================================", 165) << "\n";
    cout << getCenteredString("Rental ID       : " + record->rentalId, 165) << "\n";
    cout << getCenteredString("Hours Added     : +" + to_string(extraHours) + " hour(s)", 165) << "\n";
    cout << getCenteredString("New Duration    : " + to_string(newHours) + " total hour(s)", 165) << "\n";
    cout << getCenteredString("Additional Fee  : $" + feeSS.str(), 165) << "\n";
    cout << getCenteredString("New Rental Cost : $" + costSS.str(), 165) << "\n";
    cout << getCenteredString("Payment Status  : Pending (please settle the difference at the Payment page)", 165) << "\n";
    cout << getCenteredString("========================================", 165) << "\n";
    return true;
}

void handleTopUpMenu(DataManager &dm, const Customer &currentCustomer) {
    clearScreen();
    string border  = "===============================================================================";
    string divider = "------------+--------------------------+--------------------+------------------";

    cout << getCenteredString(border, 165) << "\n";
    cout << getCenteredString("ACTIVE RENTALS AVAILABLE FOR TOP-UP", 165) << "\n";
    cout << getCenteredString(border, 165) << "\n\n";

    // Gather active rentals for this customer
    vector<Rental*> activeRentals;
    for (auto &r : dm.rentals) {
        if (trim(r.custId) == trim(currentCustomer.customerId) && trim(r.rentingStatus) == "Active") {
            activeRentals.push_back(&r);
        }
    }

    if (activeRentals.empty()) {
        cout << getCenteredString("No active rentals found to top up.", 165) << "\n\n";
        cout << getCenteredString(border, 165) << "\n";
        cout << getCenteredString("Press Enter to return...", 165);
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
        return;
    }

    // Display formatted table of active rentals
    ostringstream headerSS;
    headerSS << " " << left << setw(10) << "Rental ID"
             << "| " << setw(25) << "Assigned Bike(s)"
             << "| " << setw(19) << "Current Duration"
             << "| " << setw(15) << "Payment Status";
    cout << getCenteredString(headerSS.str(), 165) << "\n";
    cout << getCenteredString(divider, 165) << "\n";

    for (const auto *r : activeRentals) {
        string bikeList;
        for (size_t i = 0; i < r->bikeIdsStr.size(); ++i) {
            bikeList += r->bikeIdsStr[i];
            if (i + 1 < r->bikeIdsStr.size()) bikeList += ", ";
        }
        if (bikeList.empty()) bikeList = "-";

        ostringstream rowSS;
        rowSS << " " << left << setw(10) << r->rentalId
              << "| " << setw(25) << bikeList
              << "| " << setw(19) << r->rentalDuration
              << "| " << setw(15) << r->paymentStatus;
        cout << getCenteredString(rowSS.str(), 165) << "\n";
    }
    cout << getCenteredString(border, 165) << "\n\n";

    string rentalId;
    cout << getCenteredString("Enter Rental ID to Top-Up (or 0 to cancel): ", 165);
    cin >> rentalId;

    if (trim(rentalId) == "0") return;

    Rental* record = findRentalById(dm, trim(rentalId));
    if (!record || trim(record->custId) != trim(currentCustomer.customerId) || trim(record->rentingStatus) != "Active") {
        cout << "\n" << getCenteredString("[Error] Invalid or inactive Rental ID selection.", 165) << "\n";
        cout << getCenteredString("Press Enter to continue...", 165);
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
        return;
    }

    int extraHours;
    cout << getCenteredString("Enter additional hours to add: ", 165);
    while (!(cin >> extraHours)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << getCenteredString("Invalid input! Enter additional hours to add: ", 165);
    }

    topUpRental(dm, trim(rentalId), extraHours);

    cout << "\n" << getCenteredString("Press Enter to continue...", 165);
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

void returnBicycles(DataManager &dm, const Customer &currentCustomer) {
    clearScreen();
    string border = "===============================================================================";

    cout << getCenteredString(border, 165) << "\n";
    cout << getCenteredString("RETURN BICYCLES", 165) << "\n";
    cout << getCenteredString(border, 165) << "\n\n";

    // Find active rentals for this customer
    vector<Rental*> activeRentals;
    for (auto &r : dm.rentals) {
        if (trim(r.custId) == trim(currentCustomer.customerId) && trim(r.rentingStatus) == "Active") {
            activeRentals.push_back(&r);
        }
    }

    if (activeRentals.empty()) {
        cout << getCenteredString("You have no active rentals to return.", 165) << "\n\n";
        cout << getCenteredString("Press Enter to return...", 165);
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
        return;
    }

    for (size_t i = 0; i < activeRentals.size(); ++i) {
        Rental *r = activeRentals[i];
        string bikeList;
        for (size_t j = 0; j < r->bikeIdsStr.size(); ++j) {
            bikeList += r->bikeIdsStr[j];
            if (j + 1 < r->bikeIdsStr.size()) bikeList += ", ";
        }
        
        ostringstream ss;
        ss << (i + 1) << ". Rental ID: " << r->rentalId << " | Bikes: " << bikeList << " | Status: " << r->paymentStatus;
        cout << getCenteredString(ss.str(), 165) << "\n";
    }

    cout << "\n" << getCenteredString("Enter choice number to return (0 to cancel): ", 165);
    int choice;
    if (!(cin >> choice) || choice <= 0 || choice > static_cast<int>(activeRentals.size())) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }

    Rental *selected = activeRentals[choice - 1];

    // 1. Mark renting status as Returned / Completed
    selected->rentingStatus = "Returned";

    // 2. Return all associated bicycles back to "Available"
    for (const string &bId : selected->bikeIdsStr) {
        Bicycle *b = findBicycleById(dm, bId);
        if (b) {
            b->status = "Available";
        }
    }

    saveBicycles(dm.bicycles);
    saveRentals(dm.rentals);

    cout << "\n" << getCenteredString("[+] Bicycles returned successfully!", 165) << "\n";

    // 3. Check if rental has a pending balance due
    double balanceDue = (selected->rentingPrice + selected->deposit) - selected->amountPaid;
    if (balanceDue > 0 || trim(selected->paymentStatus) == "Pending") {
        cout << getCenteredString("[!] You have a pending balance of $" + to_string(balanceDue) + " for this rental.", 165) << "\n";
        cout << getCenteredString("Redirecting directly to Payment Page...", 165) << "\n\n";
        cout << getCenteredString("Press Enter to proceed to payment...", 165);
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();

        // Direct redirect to payment module
        processPayment(dm, currentCustomer);
    } else {
        cout << getCenteredString("Press Enter to continue...", 165);
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
    }
}