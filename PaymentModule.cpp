#include <iostream>
#include "Helpers.h"
#include "PaymentModule.h"
using namespace std;

int showPaymentGatewayUI(double grandTotal) {
    clearScreen();

    string border = "============================================";

    cout << getCenteredString(border, 80) << endl;
    cout << getCenteredString("PAYMENT GATEWAY", 80) << endl;
    cout << getCenteredString(border, 80) << endl << endl;

    ostringstream ssAmount;
    ssAmount << "Amount Payable: $" << fixed << setprecision(2) << grandTotal;
    cout << getCenteredString(ssAmount.str(), 80) << endl << endl;

    cout << getCenteredString("Select Payment Method:", 80) << endl;
    cout << getCenteredString("1. Cash", 80) << endl;
    cout << getCenteredString("2. Touch 'n Go eWallet", 80) << endl;
    cout << getCenteredString("3. Credit / Debit Card", 80) << endl;
    cout << getCenteredString("0. Cancel Payment", 80) << endl << endl;

    cout << getCenteredString("Option: ", 80);

    int methodChoice;
    cin >> methodChoice;
    return methodChoice;
}

void showPaymentSuccessUI(const string &transactionId, double amountPaid) {
    clearScreen();

    string border = "============================================";

    cout << getCenteredString(border, 80) << endl;
    cout << getCenteredString("PAYMENT SUCCESSFUL", 80) << endl;
    cout << getCenteredString(border, 80) << endl << endl;

    ostringstream ssTx, ssPaid;
    ssTx << "Transaction ID : " << transactionId;
    ssPaid << "Total Paid     : $" << fixed << setprecision(2) << amountPaid;

    cout << getCenteredString(ssTx.str(), 80) << endl;
    cout << getCenteredString(ssPaid.str(), 80) << endl;
    cout << getCenteredString("Status         : CONFIRMED", 80) << endl << endl;

    cout << getCenteredString(border, 80) << endl;
    cout << getCenteredString("Press Enter to return to main menu...", 80);

    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

void paymentLogic(int methodChoice, double amountDue) {
    double amountPaid, change;
    switch (methodChoice) {
        default:
            cout << "Please enter a valid choice."<< endl;
        case 1:
            cout << "Input the amount paid: ";
            cin >> amountPaid;
            change = amountPaid - amountDue;
        case 2:
            cout << "Scan the qr below to make the payment." << endl;
        case 3:
            cout << "Please wave or insert your card in the POS machine. ";
        case 4:
            break;
    }
    cout << "Press enter to continue..." << endl;
    cin.get();
}
