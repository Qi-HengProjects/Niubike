#include <iostream>
#include <iomanip>
#include <limits>
#include <sstream>
#include "DatabaseEngine.h"
#include "Helpers.h"
#include "PaymentModule.h"

using namespace std;

int showPaymentGateway(double grandTotal) {
    clearScreen();

    string border = "============================================";

    // Display gateway title banner
    cout << getCenteredString(border, 165) << endl;
    cout << getCenteredString("PAYMENT GATEWAY", 165) << endl;
    cout << getCenteredString(border, 165) << endl << endl;

    // Display total amount due
    ostringstream ssAmount;
    ssAmount << "Amount Payable: $" << fixed << setprecision(2) << grandTotal;
    cout << getCenteredString(ssAmount.str(), 165) << endl << endl;

    // Display supported payment methods
    cout << getCenteredString("Select Payment Method:", 165) << endl;
    cout << getCenteredString("1. Cash               ", 165) << endl;
    cout << getCenteredString("2. Touch 'n Go eWallet", 165) << endl;
    cout << getCenteredString("3. Credit / Debit Card", 165) << endl;
    cout << getCenteredString("0. Cancel Payment     ", 165) << endl << endl;

    cout << getCenteredString("Option: ", 165);

    int methodChoice = -1;
    // Input loop to ensure valid numerical choice selection
    while (true) {
        if (!(cin >> methodChoice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            checkEofOrExit();
            cout << getCenteredString("Invalid input! Enter a number (0-3): ", 165);
            continue;
        }
        if (methodChoice >= 0 && methodChoice <= 3) {
            break;
        }
        cout << getCenteredString("Invalid choice! Enter a number (0-3): ", 165);
    }
    return methodChoice;
}

void showPaymentSuccess(const string &transactionId, double amountPaid, double change) {
    clearScreen();

    string border = "============================================";

    // Render success header banner
    cout << getCenteredString(border, 165) << endl;
    cout << getCenteredString("PAYMENT SUCCESSFUL", 165) << endl;
    cout << getCenteredString(border, 165) << endl << endl;

    // Format transaction metrics
    ostringstream ssTx, ssPaid, ssChange;
    ssTx << "Transaction ID : " << transactionId;
    ssPaid << "Total Paid     : $" << fixed << setprecision(2) << amountPaid;
    ssChange << "Change Given   : $" << fixed << setprecision(2) << change;

    cout << getCenteredString(ssTx.str(), 165) << endl;
    cout << getCenteredString(ssPaid.str(), 165) << endl;
    // Display change details if change was calculated
    if (change > 0.005) {
        cout << getCenteredString(ssChange.str(), 165) << endl;
    }
    cout << getCenteredString("Status         : CONFIRMED", 165) << endl << endl;

    // Navigation pause before returning to main menu
    cout << getCenteredString(border, 165) << endl;
    cout << getCenteredString("Press Enter to return to main menu...", 165);

    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

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

    // Iterate through pending rentals to display detailed row items
    for (size_t i = 0; i < pending.size(); i++) {
        Rental *r = pending[i];
        double due = (r->rentingPrice + r->deposit) - r->amountPaid;

        // Construct comma-separated list of assigned bicycle IDs
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

    // Prompt and validate customer selection choice
    int choice;
    while (true) {
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << getCenteredString("Invalid input! Enter a number: ", 165);
            continue;
        }
        if (choice == 0) {
            return nullptr; // Return null on cancellation
        }
        if (choice >= 1 && choice <= static_cast<int>(pending.size())) {
            return pending[choice - 1]; // Return chosen pointer
        }
        cout << getCenteredString("Invalid choice! Try again: ", 165);
    }
}

void paymentLogic(DataManager &dm, int methodChoice, double amountDue, const Customer &currentCustomer, const string &rentalId) {
    Rental *rPtr = findRentalById(dm, rentalId);

    // Validate rental existence, ownership, and pending status
    if (!rPtr || rPtr->custId != currentCustomer.customerId || rPtr->paymentStatus != "Pending") {
        cout << "\n" << getCenteredString("[Error] Rental not found or already paid.", 165) << endl;
        cout << "\n" << getCenteredString("Press Enter to continue...", 165);
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
        return;
    }

    Rental &r = *rPtr;
    double amountPaid = 0.0, change = 0.0;
    PaymentOutcome outcome = PaymentOutcome::FAILED;

    // Process payment method mechanics
    switch (methodChoice) {
        // Option 1: Cash Payment Method
        case 1: {
            cout << "\n" << getCenteredString("Input the amount paid (0 to cancel): $", 165);
            while (true) {
                if (!(cin >> amountPaid)) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << getCenteredString("Invalid amount! Please enter a number: $", 165);
                    continue;
                }
                if (amountPaid == 0) {
                    outcome = PaymentOutcome::CANCELLED;
                    break;
                }
                if (amountPaid < amountDue) {
                    ostringstream ssErr;
                    ssErr << "Insufficient amount! Minimum due is $" << fixed << setprecision(2) << amountDue << ". Try again (0 to cancel): $";
                    cout << getCenteredString(ssErr.str(), 165);
                    continue;
                }

                // Simple change-back math: whatever was handed over beyond the
                // amount due goes back to the customer as change. (Shown on the
                // final success screen -- printing it here would just get wiped
                // out by the clearScreen() in showPaymentSuccess().)
                change = amountPaid - amountDue;
                outcome = PaymentOutcome::SUCCESS;
                break;
            }
            break;
        }
        // Option 2 & 3: Electronic / Card POS Payment Method
        case 2:
        case 3: {
            string methodName = (methodChoice == 2) ? "eWallet" : "Card";
            if (methodChoice == 2) {
                cout << "\n" << getCenteredString("Scan the QR code below to make the payment.", 165) << endl;
            } else {
                cout << "\n" << getCenteredString("Please wave or insert your card in the POS machine.", 165) << endl;
            }

            // Real terminals can decline a transaction -- ask for the actual
            // outcome instead of assuming every attempt succeeds.
            cout << getCenteredString("Was the " + methodName + " transaction approved?", 165) << endl;
            cout << getCenteredString("1. Yes, approved", 165) << endl;
            cout << getCenteredString("2. No, declined/failed", 165) << endl;
            cout << getCenteredString("Option: ", 165);

            int confirm;
            while (true) {
                if (!(cin >> confirm)) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << getCenteredString("Invalid input! Enter 1 or 2: ", 165);
                    continue;
                }
                if (confirm == 1 || confirm == 2) break;
                cout << getCenteredString("Invalid input! Enter 1 or 2: ", 165);
            }

            outcome = (confirm == 1) ? PaymentOutcome::SUCCESS : PaymentOutcome::FAILED;
            break;
        }
        default:
            cout << "\n" << getCenteredString("Invalid payment choice.", 165) << endl;
            outcome = PaymentOutcome::FAILED;
            break;
    }

    // The DB is only ever touched here, on the SUCCESS branch. FAILED and
    // CANCELLED fall through untouched -- paymentStatus stays "Pending" and
    // the rental keeps showing up in the customer's pending-payment list.
    switch (outcome) {
        case PaymentOutcome::SUCCESS: {
            r.paymentStatus = "Paid";
            r.amountPaid = r.rentingPrice + r.deposit;   // fully settle the current balance
            saveRentals(dm.rentals); // Persist updated payment state to file
            string transactionId = "TXN-" + r.rentalId;
            showPaymentSuccess(transactionId, amountDue, change);
            return;
        }
        case PaymentOutcome::FAILED:
            cout << "\n" << getCenteredString("[!] Payment failed. Rental " + r.rentalId + " remains Pending -- try again from the Payment page.", 165) << endl;
            break;
        case PaymentOutcome::CANCELLED:
            cout << "\n" << getCenteredString("Payment cancelled. Rental " + r.rentalId + " remains Pending.", 165) << endl;
            break;
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

    // Handle case where customer has no outstanding payments
    if (pending.empty()) {
        clearScreen();
        cout << getCenteredString("No pending payments found for your account.", 165) << "\n\n";
        cout << getCenteredString("Press Enter to return to main menu...", 165);
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
        return;
    }

    // Allow user to select target pending rental
    Rental *selected = selectPendingRental(dm, pending);
    if (!selected) {
        return; // user cancelled selection
    }

    // Calculate total balance due for selected rental
    double totalDue = (selected->rentingPrice + selected->deposit) - selected->amountPaid;
    string selectedRentalId = selected->rentalId;

    // Display gateway and obtain user method choice
    int choice = showPaymentGateway(totalDue);

    // Trigger payment processing logic if not cancelled
    if (choice != 0) {
        paymentLogic(dm, choice, totalDue, currentCustomer, selectedRentalId);
    }
}