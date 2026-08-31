#include "RentalModule.h"
#include "DatabaseEngine.h"
#include "Helpers.h"
#include "LoyaltyModule.h"
#include "PenaltyModule.h"
#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <algorithm>
#include <iomanip>
#include <ctime>
#include <sstream>

using namespace std;

static string trim(const string &s) {
    size_t first = s.find_first_not_of(" \t\r\n");
    if (first == string::npos) return "";
    size_t last = s.find_last_not_of(" \t\r\n");
    return s.substr(first, (last - first + 1));
}

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

        // Retrieve real-time available stock counts for each bicycle category
        int availRegular   = getAvailableCount(dm, "Regular");
        int availTwoSeater = getAvailableCount(dm, "Two Seater");
        int availEBike     = getAvailableCount(dm, "E-Bike");
        int availKidsBike  = getAvailableCount(dm, "Kids Bike");
        int availCityBike  = getAvailableCount(dm, "City Bike");

        // Deduct bikes already selected in the current session cart to prevent over-booking
        for (int sel : selectedRent) {
            if (sel == 1) availRegular--;
            if (sel == 2) availTwoSeater--;
            if (sel == 3) availEBike--;
            if (sel == 4) availKidsBike--;
            if (sel == 5) availCityBike--;
        }

        string border  = "=========================================================";
        string divider = "-----------+--------------+----------+-------------------";

        // Render rental menu table header
        cout << getCenteredString(border, 165) << endl;
        cout << getCenteredString("BICYCLE RENTAL MENU", 165) << endl;
        cout << getCenteredString(border, 165) << endl;
        cout << getCenteredString(" Option |   Category   | Price/hr | Available Stock    ", 170) << endl;
        cout << getCenteredString(divider, 165) << endl;

        // Construct row display strings with dynamic stock status messages
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

        // Render current cart summary
        string selectedSummary = " Selected so far: " + to_string(selectedRent.size()) + " bike(s)";
        cout << getCenteredString(selectedSummary, 165) << endl;
        cout << getCenteredString(border, 165) << endl << endl;

        cout << getCenteredString("Enter option (1-5, or 0 to finish selection): ", 165);

        // Input handling and validation
        if (!(cin >> bikeOpt)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            checkEofOrExit();
            cout << "\nInvalid input! Please enter a number.\n";
            cout << "Press Enter to continue...";
            cin.get();
            continue;
        }

        if (bikeOpt == 0) break;

        // Validate selection against available stock
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

    // Prompt and validate rental duration in hours
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
        checkEofOrExit();
        cout << "\n[!] Invalid duration! Please enter a positive whole number.\n\n";
    }

    const string categoryNames[6] = { "", "Regular", "Two Seater", "E-Bike", "Kids Bike", "City Bike" };

    vector<string> assignedBikeIds;
    vector<string> assignedCategories;
    vector<double> assignedRates;

    // Match selected categories to specific available bicycle instances
    for (int opt : selectedRent) {
        string targetCat = categoryNames[opt];
        
        for (auto &b : dm.bicycles) {
            if (trim(b.bikeType) == targetCat && trim(b.status) == "Available") {
                b.status = "Rented"; // Update bike status to Rented
                assignedBikeIds.push_back(b.bikeId);
                assignedCategories.push_back(b.bikeType);
                assignedRates.push_back(b.price);
                break; // Assign one bicycle per cart selection entry
            }
        }
    }

    // Persist updated bicycle availability status to database
    saveBicycles(dm.bicycles);

    double subtotal = 0.0;
    double totalDiscount = 0.0;
    const double DEPOSIT_PER_BIKE = 15.00;
    int totalBikes = static_cast<int>(assignedBikeIds.size());

    // Retrieve customer loyalty program membership details and discount tier
    MembershipInfo memberInfo = getMembershipInfo(dm, currentCustomer);
    time_t nowTs = time(nullptr);

    clearScreen();

    string border  = "===============================================================================";
    string divider = "-----------+---------+-------------+-------------------+----------+----------+-----------+-----------";

    cout << getCenteredString(border, 165) << endl;
    cout << getCenteredString("CART", 165) << endl;
    cout << getCenteredString(border, 165) << endl;

    // Display loyalty discount info header if applicable
    if (memberInfo.isMember && memberInfo.discountPercent > 0.0) {
        ostringstream ssMember;
        ssMember << "[Loyalty] " << memberInfo.tierName << " member -- "
                  << fixed << setprecision(0) << (memberInfo.discountPercent * 100)
                  << "% off rental rates applied below.";
        cout << getCenteredString(ssMember.str(), 165) << endl << endl;
    }

    // Render detailed cart breakdown table header
    ostringstream headerSS;
    headerSS << " " << left << setw(10) << "Rental ID"
             << "| " << setw(7) << "Bike ID"
             << "| " << setw(11) << "Category"
             << "| " << setw(17) << "Rate ($/hr)"
             << "| " << setw(8) << "Hours"
             << "| " << setw(8) << "Deposit"
             << "| " << setw(9) << "Discount"
             << "| " << "Line Total ($)";
    cout << getCenteredString(headerSS.str(), 165) << endl;
    cout << getCenteredString(divider, 165) << endl;

    // Create distinct Rental record per assigned bicycle and format row display
    vector<string> newRentalIds;

    for (size_t i = 0; i < assignedBikeIds.size(); i++) {
        double rawLineTotal = assignedRates[i] * hours;
        double discountAmount = rawLineTotal * memberInfo.discountPercent;
        double lineTotal = rawLineTotal - discountAmount;

        subtotal += lineTotal;
        totalDiscount += discountAmount;

        // Construct new Rental database record
        Rental newR;
        newR.rentalId = generateNextRentalId(dm);
        newR.rentalDuration = to_string(hours) + " hours";
        newR.paymentStatus = "Pending";
        newR.rentingStatus = "Active";
        newR.rentingPrice = lineTotal;
        newR.deposit = DEPOSIT_PER_BIKE;
        newR.amountPaid = 0.0;
        newR.custId = currentCustomer.customerId;
        newR.bikeIdsStr = { assignedBikeIds[i] };
        newR.checkoutTime = nowTs;
        newR.originalBikeIdsStr = { assignedBikeIds[i] };

        dm.rentals.push_back(newR);
        newRentalIds.push_back(newR.rentalId);

        // Format row item for display table
        ostringstream rowSS;
        rowSS << " " << left << setw(10) << newR.rentalId
              << "| " << setw(7) << assignedBikeIds[i]
              << "| " << setw(11) << assignedCategories[i]
              << "| $" << setw(16) << fixed << setprecision(2) << assignedRates[i]
              << "| " << setw(2) << hours << " hrs "
              << "| $" << setw(7) << fixed << setprecision(2) << DEPOSIT_PER_BIKE
              << "| $" << setw(8) << fixed << setprecision(2) << discountAmount
              << "| $" << fixed << setprecision(2) << lineTotal;

        cout << getCenteredString(rowSS.str(), 165) << endl;
    }

    // Calculate overall total financial summary
    double totalDeposit = DEPOSIT_PER_BIKE * totalBikes;
    double grandTotal = subtotal + totalDeposit;

    cout << getCenteredString(divider, 165) << endl;

    ostringstream ssSub, ssDep, ssGrand;
    ssSub << "Rental Subtotal (" << totalBikes << " bike/s x " << hours << " hrs):     $" << fixed << setprecision(2) << subtotal;
    ssDep << "Refundable Deposit ($" << fixed << setprecision(2) << DEPOSIT_PER_BIKE << " x " << totalBikes << " bike/s): $" << fixed << setprecision(2) << totalDeposit;
    ssGrand << "TOTAL AMOUNT DUE: $" << fixed << setprecision(2) << grandTotal;

    cout << getCenteredString(ssSub.str(), 135) << endl;
    if (totalDiscount > 0.005) {
        ostringstream ssSaved;
        ssSaved << "Loyalty Savings (" << memberInfo.tierName << "): -$" << fixed << setprecision(2) << totalDiscount;
        cout << getCenteredString(ssSaved.str(), 135) << endl;
    }
    cout << getCenteredString(ssDep.str(), 135) << endl;
    cout << getCenteredString(border, 165) << endl;
    cout << getCenteredString(ssGrand.str(), 165) << endl;
    cout << getCenteredString(border, 165) << endl << endl;

    // Persist newly created rental records to file storage
    saveRentals(dm.rentals);

    // Format confirmation message with list of newly assigned rental IDs
    string idsJoined;
    for (size_t i = 0; i < newRentalIds.size(); i++) {
        idsJoined += newRentalIds[i];
        if (i + 1 < newRentalIds.size()) idsJoined += ", ";
    }

    cout << getCenteredString("[+] Rental confirmed! Assigned Rental ID(s): " + idsJoined, 165) << endl;
    cout << getCenteredString("Each bike has its own Rental ID -- use it for top-ups, payment, and returns.", 165) << endl;
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

    // Compute total hourly rate for assigned bicycle(s)
    double totalHourlyRate = 0.0;
    for (const auto &bikeId : record->bikeIdsStr) {
        Bicycle* bike = findBicycleById(dm, bikeId);
        if (bike) {
            totalHourlyRate += bike->price;
        }
    }

    double addedCost = extraHours * totalHourlyRate;

    // Extract current duration, add extra hours, and format new duration string
    int currentHours = 0;
    stringstream durSS(record->rentalDuration);
    durSS >> currentHours;
    int newHours = currentHours + extraHours;

    // Update rental record fields
    record->rentalDuration = to_string(newHours) + " hours";
    record->rentingPrice += addedCost;
    record->paymentStatus = "Pending";
    record->toppedUp = true; // Flag prevents future cancellation

    saveRentals(dm.rentals); // Persist updated rental record

    stringstream feeSS, costSS;
    feeSS << fixed << setprecision(2) << addedCost;
    costSS << fixed << setprecision(2) << record->rentingPrice;

    // Display top-up confirmation box
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

bool cancelRental(DataManager &dm, const string &rentalId) {
    Rental* record = findRentalById(dm, rentalId);
    if (!record) {
        cout << getCenteredString("[Error] Rental ID " + rentalId + " not found.", 165) << "\n";
        return false;
    }

    if (trim(record->rentingStatus) != "Active") {
        cout << getCenteredString("[Error] Only an active rental (not yet returned or already cancelled) can be cancelled.", 165) << "\n";
        return false;
    }

    // Restrict cancellation if payment has already been completed
    if (trim(record->paymentStatus) == "Paid") {
        cout << getCenteredString("[Error] This rental has already been paid for and can no longer be cancelled.", 165) << "\n";
        cout << getCenteredString("Please use Return Bike instead.", 165) << "\n";
        return false;
    }

    // Restrict cancellation if rental duration was extended via top-up
    if (record->toppedUp) {
        cout << getCenteredString("[Error] This rental has already been topped up and can no longer be cancelled.", 165) << "\n";
        return false;
    }

    if (record->bikeIdsStr.empty()) {
        cout << getCenteredString("[Error] No bicycles associated with this rental.", 165) << "\n";
        return false;
    }

    // Check if rental is overdue; overdue rentals must go through Return Bike flow
    int plannedHours = 0;
    {
        stringstream durSS(record->rentalDuration);
        durSS >> plannedHours;
    }
    Bicycle* firstBike = findBicycleById(dm, record->bikeIdsStr[0]);
    double hourlyRate = firstBike ? firstBike->price : 0.0;
    LateFeeResult lateCheck = calculateLateFee(plannedHours, record->checkoutTime, hourlyRate);
    if (lateCheck.isLate) {
        cout << getCenteredString("[Error] This rental is already overdue and can no longer be cancelled.", 165) << "\n";
        cout << getCenteredString("Please use Return Bike instead to settle the late fee and close it out.", 165) << "\n";
        return false;
    }

    // Return all associated bicycles back to "Available" inventory pool
    for (const string &bikeId : record->bikeIdsStr) {
        Bicycle* bike = findBicycleById(dm, bikeId);
        if (bike) {
            bike->status = "Available";
        }
    }
    saveBicycles(dm.bicycles);

    // Reset rental record metrics to zero / cancelled
    record->bikeIdsStr.clear();
    record->rentingStatus = "Cancelled";
    record->paymentStatus = "Cancelled";
    record->rentingPrice = 0.0;
    record->deposit = 0.0;
    record->amountPaid = 0.0; // nothing was ever paid -- nothing to refund

    saveRentals(dm.rentals); // Persist updated rental state

    cout << "\n";
    cout << getCenteredString("========================================", 165) << "\n";
    cout << getCenteredString("        RENTAL CANCELLED                ", 165) << "\n";
    cout << getCenteredString("========================================", 165) << "\n";
    cout << getCenteredString("Rental ID       : " + record->rentalId, 165) << "\n";
    cout << getCenteredString("No payment had been made for this rental -- nothing to refund.", 165) << "\n";
    cout << getCenteredString("The bike(s) from this booking are back in stock immediately.", 165) << "\n";
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

    // Filter active rentals matching current customer
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

    // Display formatted table of active rentals available for duration extension
    ostringstream headerSS;
    headerSS << " " << left << setw(10) << "Rental ID"
             << "| " << setw(25) << "Assigned Bike"
             << "| " << setw(19) << "Current Duration"
             << "| " << setw(15) << "Payment Status";
    cout << getCenteredString(headerSS.str(), 165) << "\n";
    cout << getCenteredString(divider, 165) << "\n";

    for (const auto *r : activeRentals) {
        if (r->bikeIdsStr.empty()) {
            ostringstream rowSS;
            rowSS << " " << left << setw(10) << r->rentalId
                  << "| " << setw(25) << "-"
                  << "| " << setw(19) << r->rentalDuration
                  << "| " << setw(15) << r->paymentStatus;
            cout << getCenteredString(rowSS.str(), 165) << "\n";
            continue;
        }

        for (const string &bikeId : r->bikeIdsStr) {
            ostringstream rowSS;
            rowSS << " " << left << setw(10) << r->rentalId
                  << "| " << setw(25) << bikeId
                  << "| " << setw(19) << r->rentalDuration
                  << "| " << setw(15) << r->paymentStatus;
            cout << getCenteredString(rowSS.str(), 165) << "\n";
        }
    }
    cout << getCenteredString(border, 165) << "\n\n";

    // Prompt user for Rental ID target
    string rentalId;
    cout << getCenteredString("Enter Rental ID to Top-Up (or 0 to cancel): ", 165);
    cin >> rentalId;
    if (!cin) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        checkEofOrExit();
        return;
    }

    if (trim(rentalId) == "0") return;

    // Validate selected rental ownership and active state
    Rental* record = findRentalById(dm, trim(rentalId));
    if (!record || trim(record->custId) != trim(currentCustomer.customerId) || trim(record->rentingStatus) != "Active") {
        cout << "\n" << getCenteredString("[Error] Invalid or inactive Rental ID selection.", 165) << "\n";
        cout << getCenteredString("Press Enter to continue...", 165);
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
        return;
    }

    // Prompt and validate input for extra hours
    int extraHours;
    cout << getCenteredString("Enter additional hours to add: ", 165);
    while (!(cin >> extraHours)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        checkEofOrExit();
        cout << getCenteredString("Invalid input! Enter additional hours to add: ", 165);
    }

    // Process top-up logic
    topUpRental(dm, trim(rentalId), extraHours);

    cout << "\n" << getCenteredString("Press Enter to continue...", 165);
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

void handleCancelMenu(DataManager &dm, const Customer &currentCustomer) {
    clearScreen();
    string border  = "===============================================================================";
    string divider = "------------+--------------------------+--------------------+------------------";

    cout << getCenteredString(border, 165) << "\n";
    cout << getCenteredString("CANCEL RENTAL", 165) << "\n";
    cout << getCenteredString(border, 165) << "\n\n";

    // Gather rentals eligible for cancellation
    vector<Rental*> cancellable;
    for (auto &r : dm.rentals) {
        if (trim(r.custId) == trim(currentCustomer.customerId)
            && trim(r.rentingStatus) == "Active"
            && trim(r.paymentStatus) == "Pending"
            && !r.toppedUp) {
            cancellable.push_back(&r);
        }
    }

    if (cancellable.empty()) {
        cout << getCenteredString("No rentals available for cancellation.", 165) << "\n";
        cout << getCenteredString("(Only unpaid rentals that haven't been topped up can be cancelled.)", 165) << "\n\n";
        cout << getCenteredString(border, 165) << "\n";
        cout << getCenteredString("Press Enter to return...", 165);
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
        return;
    }

    // Display table of cancellable rentals
    ostringstream headerSS;
    headerSS << " " << left << setw(10) << "Rental ID"
             << "| " << setw(25) << "Assigned Bike"
             << "| " << setw(19) << "Duration"
             << "| " << "Price ($)";
    cout << getCenteredString(headerSS.str(), 165) << "\n";
    cout << getCenteredString(divider, 165) << "\n";

    for (const auto *r : cancellable) {
        string bikeList;
        for (size_t j = 0; j < r->bikeIdsStr.size(); j++) {
            bikeList += r->bikeIdsStr[j];
            if (j + 1 < r->bikeIdsStr.size()) bikeList += ", ";
        }
        if (bikeList.empty()) bikeList = "-";

        ostringstream rowSS;
        rowSS << " " << left << setw(10) << r->rentalId
              << "| " << setw(25) << bikeList
              << "| " << setw(19) << r->rentalDuration
              << "| $" << fixed << setprecision(2) << r->rentingPrice;
        cout << getCenteredString(rowSS.str(), 165) << "\n";
    }
    cout << getCenteredString(border, 165) << "\n\n";

    // Prompt user for target Rental ID
    string rentalId;
    cout << getCenteredString("Enter Rental ID to Cancel (or 0 to go back): ", 165);
    cin >> rentalId;
    if (!cin) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        checkEofOrExit();
        return;
    }

    if (trim(rentalId) == "0") return;

    // Verify if selected rental is in the cancellable list
    bool eligible = false;
    for (const auto *r : cancellable) {
        if (trim(r->rentalId) == trim(rentalId)) {
            eligible = true;
            break;
        }
    }

    if (!eligible) {
        cout << "\n" << getCenteredString("[Error] Invalid selection, or that rental isn't eligible for cancellation.", 165) << "\n";
        cout << getCenteredString("Press Enter to continue...", 165);
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
        return;
    }

    // Request explicit user confirmation before cancelling
    int confirm = 0;
    cout << "\n" << getCenteredString("Are you sure you want to cancel Rental " + trim(rentalId) + "?", 165) << "\n";
    cout << getCenteredString("1. Yes, cancel it", 165) << "\n";
    cout << getCenteredString("2. No, keep it", 165) << "\n";
    cout << getCenteredString("Option: ", 165);
    while (true) {
        if (!(cin >> confirm)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            checkEofOrExit();
            cout << getCenteredString("Invalid input! Enter 1 or 2: ", 165);
            continue;
        }
        if (confirm == 1 || confirm == 2) break;
        cout << getCenteredString("Invalid choice! Enter 1 or 2: ", 165);
    }

    // Execute or abort cancellation based on prompt selection
    if (confirm == 1) {
        cancelRental(dm, trim(rentalId));
    } else {
        cout << "\n" << getCenteredString("Cancellation aborted. Your rental remains active.", 165) << "\n";
    }

    cout << "\n" << getCenteredString("Press Enter to continue...", 165);
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}