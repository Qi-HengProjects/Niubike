#include "UserInterface.h"
#include "DatabaseEngine.h"
#include <iostream>
#include <sstream>
#include "Helpers.h"

using namespace std;

int login(DataManager &dm) {
    while (true) {
        clearScreen();

        const string asciiArt = R"(
 _____  ___    __     ____  ____  _______   __     __   ___  _______
(\"   \|"  \  |" \   ("  _||_ " ||   _  "\ |" \   |/"| /  ")/"     "|
|.\\   \    | ||  |  |   (  ) : |(. |_)  :)||  |  (: |/   /(: ______)
|: \.   \\  | |:  |  (:  |  | . )|:     \/ |:  |  |    __/  \/      |
|.  \    \. | |.  |   \\ \__/ // (|  _  \\ |.  |  (// _  \  // ___)_
|    \    \ | /\  |\  /\\ __ //\ |: |_)  :)/\  |\ |: | \  \(:      "|
 \___|\____\)(__\_|_)(__________)(_______/(__\_|_)(__|  \__)\_______)
)";

        printCenteredBlock(asciiArt, 165);
        cout << getCenteredString("Bicycle Rental Services", 165) << endl;

        int loginOpt;
        cout << getCenteredString("1. Log In ", 165) << endl;
        cout << getCenteredString("2. Sign Up", 165) << endl;
        cout << getCenteredString("Option:   ", 165);
        cin >> loginOpt;

        switch (loginOpt) {
            case 1: {
                string loginUIn, loginPwdIn, signupUIn, signupPwdIn, customerId, customerName, customerIc;
                clearScreen();
                const string loginInBox = R"(
                +---------------------------------------------+
                |                    Login                    |
                +---------------------------------------------+
                )";
                printCenteredBlock(loginInBox, 165);
                cout << getCenteredString("Username: ", 170);
                cin >> loginUIn;
                cout << getCenteredString("Password: ", 170);
                cin >> loginPwdIn;

                bool found = false;
                for (const auto &c : dm.customers) {
                    if (c.username == loginUIn) {
                        found = true;
                        if (c.password == loginPwdIn) {
                            cout << "Login successful. Welcome " << c.customerName << endl;
                            return 0;
                        } else {
                            cout << "Incorrect password." << endl;
                        }
                        break;
                    }
                }
                if (!found) {
                    cout << "Username not found. Please sign up first." << endl;
                }

                cout << "Press Enter to continue...";
                cin.ignore();
                cin.get();
                continue;
            }
            case 2: {
                string signupUIn, signupPwdIn, customerId, customerName, customerIc, payStatus, history;
                clearScreen();
                const string signupInBox = R"(
                +---------------------------------------------+
                |                  Sign Up                    |
                +---------------------------------------------+
                )";
                printCenteredBlock(signupInBox, 165);

                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                cout << getCenteredString("Name (as per IC) ", 170);
                getline(cin, customerName);
                cout << getCenteredString("IC number: ", 170);
                getline(cin, customerIc);
                cout << getCenteredString("Username: ", 170);
                getline(cin, signupUIn);
                cout << getCenteredString("Password: ", 170);
                getline(cin, signupPwdIn);

                bool taken = false;
                for (const auto &c : dm.customers) {
                    if (c.username == signupUIn) {
                        taken = true;
                        break;
                    }
                }
                if (taken) {
                    cout << "Username already exists. Please choose another." << endl;
                    cout << "Press Enter to continue...";
                    cin.ignore();
                    cin.get();
                    continue;
                }

                Customer newC;
                newC.customerId = "C" + to_string(dm.customers.size() + 1);
                newC.customerName = customerName;
                newC.customerIc = customerIc;
                newC.payStatus = "None";
                newC.history = "New account created";
                newC.username = signupUIn;
                newC.password = signupPwdIn;

                dm.customers.push_back(newC);
                saveCustomers(dm.customers);

                cout << "Sign up successful." << endl;
                cout << "Press Enter to continue...";
                cin.get();
                cout << endl;
                return 0;
            }
            default: {
                cout << "Invalid option." << endl;
                cout << "Press Enter to continue...";
                cin.ignore();
                cin.get();
                continue;
            }
        }
    }
}

void menu() {
    const string menuText = R"(
    1. Rent
    2. Top Up Time
    3. Return Bike
    4. Payment
    5. History
    )";

    printCenteredBlock(menuText, 165);
    int menuOpt;
    cout << getCenteredString("Option: ", 165);
    cin >> menuOpt;
}