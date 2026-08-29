#include <iostream>
#include <iomanip>
#include <limits>
#include <sstream>
#include "MainMenu.h"
#include "DatabaseEngine.h"
#include "Helpers.h"
#include "History.h"
#include "RentalModule.h"
#include "PaymentModule.h"

using namespace std;

int showPaymentGateway(double grandTotal) {
    clearScreen();

    string border = "============================================";

    cout << getCenteredString(border, 165) << endl;
    cout << getCenteredString("PAYMENT GATEWAY", 165) << endl;
    cout << getCenteredString(border, 165) << endl << endl;

    ostringstream ssAmount;
    ssAmount << "Amount Payable: $" << fixed << setprecision(2) << grandTotal;
    cout << getCenteredString(ssAmount.str(), 165) << endl << endl;

    cout << getCenteredString("Select Payment Method:", 165) << endl;
    cout << getCenteredString("1. Cash               ", 165) << endl;
    cout << getCenteredString("2. Touch 'n Go eWallet", 165) << endl;
    cout << getCenteredString("3. Credit / Debit Card", 165) << endl;
    cout << getCenteredString("0. Cancel Payment     ", 165) << endl << endl;

    cout << getCenteredString("Option: ", 165);

    int methodChoice;
    cin >> methodChoice;
    return methodChoice;
}

void showPaymentSuccess(const string &transactionId, double amountPaid) {
    clearScreen();

    string border = "============================================";

    cout << getCenteredString(border, 165) << endl;
    cout << getCenteredString("PAYMENT SUCCESSFUL", 165) << endl;
    cout << getCenteredString(border, 165) << endl << endl;

    ostringstream ssTx, ssPaid;
    ssTx << "Transaction ID : " << transactionId;
    ssPaid << "Total Paid     : $" << fixed << setprecision(2) << amountPaid;

    cout << getCenteredString(ssTx.str(), 165) << endl;
    cout << getCenteredString(ssPaid.str(), 165) << endl;
    cout << getCenteredString("Status         : CONFIRMED", 165) << endl << endl;

    cout << getCenteredString(border, 165) << endl;
    cout << getCenteredString("Press Enter to return to main menu...", 165);

    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

// Lets the customer choose which pending rental they want to pay for,
// instead of always settling whichever one happens to be first in the list.
Rental* selectPendingRental(DataManager &dm, vector<Rental*> &pending) {
    // Only one pending rental -- nothing to choose, pay it directly.
    if (pending.size() == 1) {
        return pending[0];
    }

    clearScreen();

    string border  = "===============================================================================";
    string divider = "-------+-----------+--------------------------+--------------------+-----------";

    cout << getCenteredString(border, 165) << endl;
    cout << getCenteredString("SELECT RENTAL TO PAY FOR", 165) << endl;
    cout << getCenteredString(border, 165) << endl << endl;

    // Header - Column widths: 6 | 10 | 25 | 19 | 10
    ostringstream headerSS;
    headerSS << " " << left << setw(6) << "No."
             << "| " << setw(10) << "Rental ID"
             << "| " << setw(25) << "Assigned Bike(s)"
             << "| " << setw(19) << "Duration"
             << "| " << setw(10) << "Due ($)";

    cout << getCenteredString(headerSS.str(), 165) << endl;
    cout << getCenteredString(divider, 165) << endl;

    for (size_t i = 0; i < pending.size(); i++) {
        Rental *r = pending[i];
        double due = (r->rentingPrice + r->deposit) - r->amountPaid;

        string bikeList;
        for (size_t j = 0; j < r->bikeIdsStr.size(); j++) {
            bikeList += r->bikeIdsStr[j];
            if (j + 1 < r->bikeIdsStr.size()) bikeList += ", ";
        }
        if (bikeList.empty()) bikeList = "-";

        ostringstream dueSS;
        dueSS << "$" << fixed << setprecision(2) << due;

        // Row - Matching Column widths: 6 | 10 | 25 | 19 | 10
        ostringstream rowSS;
        rowSS << " " << left << setw(6) << to_string(i + 1) + "."
              << "| " << setw(10) << r->rentalId
              << "| " << setw(25) << bikeList
              << "| " << setw(19) << r->rentalDuration
              << "| " << setw(10) << dueSS.str();

        cout << getCenteredString(rowSS.str(), 165) << endl;
    }

    cout << getCenteredString(divider, 165) << endl << endl;

    cout << endl;
    cout << getCenteredString("0. Cancel", 165) << endl << endl;
    cout << getCenteredString("Select rental to pay (enter number): ", 165);

    int choice;
    while (true) {
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << getCenteredString("Invalid input! Enter a number: ", 165);
            continue;
        }
        if (choice == 0) {
            return nullptr;
        }
        if (choice >= 1 && choice <= static_cast<int>(pending.size())) {
            return pending[choice - 1];
        }
        cout << getCenteredString("Invalid choice! Try again: ", 165);
    }
}

void paymentLogic(DataManager &dm, int methodChoice, double amountDue, const Customer &currentCustomer, const string &rentalId) {
    Rental *rPtr = findRentalById(dm, rentalId);

    if (!rPtr || rPtr->custId != currentCustomer.customerId || rPtr->paymentStatus != "Pending") {
        cout << "\n" << getCenteredString("[Error] Rental not found or already paid.", 165) << endl;
        cout << "\n" << getCenteredString("Press Enter to continue...", 165);
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
        return;
    }

    Rental &r = *rPtr;
    double amountPaid = 0.0, change = 0.0;
    bool success = false;

    switch (methodChoice) {
        case 1: {
            cout << "\n" << getCenteredString("Input the amount paid: $", 165);
            while (true) {
                if (!(cin >> amountPaid)) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << getCenteredString("Invalid amount! Please enter a number: $", 165);
                    continue;
                }
                if (amountPaid < amountDue) {
                    ostringstream ssErr;
                    ssErr << "Insufficient amount! Minimum due is $" << fixed << setprecision(2) << amountDue << ". Try again: $";
                    cout << getCenteredString(ssErr.str(), 165);
                    continue;
                }
                break;
            }

            change = amountPaid - amountDue;
            ostringstream ssChange;
            ssChange << "Change: $" << fixed << setprecision(2) << change;
            cout << getCenteredString(ssChange.str(), 165) << endl;
            success = true;
            break;
        }
        case 2:
            cout << "\n" << getCenteredString("Scan the QR code below to make the payment.", 165) << endl;
            success = true;
            break;
        case 3:
            cout << "\n" << getCenteredString("Please wave or insert your card in the POS machine.", 165) << endl;
            success = true;
            break;
        default:
            cout << "\n" << getCenteredString("Invalid payment choice.", 165) << endl;
            break;
    }

    if (success) {
        r.paymentStatus = "Paid";
        r.amountPaid = r.rentingPrice + r.deposit;   // fully settle the current balance
        saveRentals(dm.rentals);
        string transactionId = "TXN-" + r.rentalId;
        showPaymentSuccess(transactionId, amountDue);
        return;
    }

    cout << "\n" << getCenteredString("Press Enter to continue...", 165);
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

void processPayment(DataManager &dm, const Customer &currentCustomer) {
    // Gather ALL pending rentals for this customer (not just the first one found)
    vector<Rental*> pending;
    for (auto &r : dm.rentals) {
        if (r.custId == currentCustomer.customerId && r.paymentStatus == "Pending") {
            pending.push_back(&r);
        }
    }

    if (pending.empty()) {
        clearScreen();
        cout << getCenteredString("No pending payments found for your account.", 165) << "\n\n";
        cout << getCenteredString("Press Enter to return to main menu...", 165);
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
        return;
    }

    Rental *selected = selectPendingRental(dm, pending);
    if (!selected) {
        return; // user cancelled selection
    }

    double totalDue = (selected->rentingPrice + selected->deposit) - selected->amountPaid;
    string selectedRentalId = selected->rentalId;

    int choice = showPaymentGateway(totalDue);

    if (choice != 0) {
        paymentLogic(dm, choice, totalDue, currentCustomer, selectedRentalId);
    }
}