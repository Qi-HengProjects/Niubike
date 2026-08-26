#include <iostream>
#include <iomanip>
#include <limits>
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
    cout << getCenteredString(ssAmount.str(), 80) << endl << endl;

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

void paymentLogic(DataManager &dm, int methodChoice, double amountDue, const Customer &currentCustomer) {
    for (auto &r : dm.rentals) {
        // 1. Changed while to if to prevent infinite looping
        if (r.custId == currentCustomer.customerId && r.paymentStatus == "Pending") {
            double amountPaid = 0.0, change = 0.0;
            bool success = false;

            switch (methodChoice) {
                case 1: {
                    cout << "\nInput the amount paid: $";
                    while (true) {
                        if (!(cin >> amountPaid)) {
                            cin.clear();
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                            cout << "Invalid amount! Please enter a number: $";
                            continue;
                        }
                        if (amountPaid < amountDue) {
                            cout << "Insufficient amount! Minimum due is $" << fixed << setprecision(2) << amountDue << ". Try again: $";
                            continue;
                        }
                        break;
                    }

                    change = amountPaid - amountDue;
                    cout << "Change: $" << fixed << setprecision(2) << change << endl;
                    success = true;
                    break;
                }
                case 2:
                    cout << "\nScan the QR code below to make the payment." << endl;
                    success = true;
                    break;
                case 3:
                    cout << "\nPlease wave or insert your card in the POS machine." << endl;
                    success = true;
                    break;
                default:
                    cout << "\nInvalid payment choice." << endl;
                    break;
            }

            if (success) {
                // 3. Update database status in RAM and save to file
                r.paymentStatus = "Paid";
                saveRentals(dm.rentals);
                string transactionId = "TXN-" + r.rentalId;
                showPaymentSuccess(transactionId, amountDue);
                return;
            }
            
            cout << "\nPress Enter to continue...";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.get();
            return;
        }
    }
}

void processPayment(DataManager &dm, const Customer &currentCustomer) {
    double totalDue = 0.0;
    bool hasPending = false;

    // Find total amount due for pending rental
    for (const auto &r : dm.rentals) {
        if (r.custId == currentCustomer.customerId && r.paymentStatus == "Pending") {
            totalDue = r.rentingPrice + r.deposit;
            hasPending = true;
            break;
        }
    }

    if (!hasPending) {
        clearScreen();
        cout << "No pending payments found for your account.\n\n";
        cout << "Press Enter to return to main menu...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
        return;
    }

    // 1. Show UI and get user choice
    int choice = showPaymentGateway(totalDue);

    // 2. Execute Payment Logic
    if (choice != 0) {
        paymentLogic(dm, choice, totalDue, currentCustomer);
    }
}