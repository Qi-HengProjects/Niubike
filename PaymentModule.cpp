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

void paymentLogic(DataManager &dm, int methodChoice, double amountDue, const Customer &currentCustomer) {
    for (auto &r : dm.rentals) {
        // 1. Changed while to if to prevent infinite looping
        if (r.custId == currentCustomer.customerId && r.paymentStatus == "Pending") {
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
                r.amountPaid = r.rentingPrice + r.deposit;   // NEW: fully settle the current balance
                saveRentals(dm.rentals);
                string transactionId = "TXN-" + r.rentalId;
                showPaymentSuccess(transactionId, amountDue);
                return;
            }
            
            cout << "\n" << getCenteredString("Press Enter to continue...", 165);
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
            totalDue = (r.rentingPrice + r.deposit) - r.amountPaid;
            hasPending = true;
            break;
        }
    }

    if (!hasPending) {
        clearScreen();
        cout << getCenteredString("No pending payments found for your account.", 165) << "\n\n";
        cout << getCenteredString("Press Enter to return to main menu...", 165);
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
        return;
    }

    int choice = showPaymentGateway(totalDue);

    if (choice != 0) {
        paymentLogic(dm, choice, totalDue, currentCustomer);
    }
}