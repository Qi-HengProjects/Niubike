#include "MainMenu.h"
#include "DatabaseEngine.h"
#include "Helpers.h"
#include "History.h"
#include "RentalModule.h"
#include <iostream>
#include <sstream>
#include <limits>
#include <algorithm>

using namespace std;

// Helper function to trim all whitespace, \r, and \n
static string trim(const string &s) {
    size_t first = s.find_first_not_of(" \t\r\n");
    if (first == string::npos) return "";
    size_t last = s.find_last_not_of(" \t\r\n");
    return s.substr(first, (last - first + 1));
}

static bool containsComma(const string &s) {
    return s.find(',') != string::npos;
}

int login(DataManager &dm, Customer &currentCustomer) {
    while (true) {
        clearScreen();

        const string asciiArt = R"(
 _____  ___    __     ____  ____  _______    __     __   ___  _______ 
(\"   \|"  \  |" \   ("  _||_ " ||   _  "\  |" \   |/"| /  ")/"     "|
|.\\   \    | ||  |  |   (  ) : |(. |_)  :) ||  |  (: |/   /(: ______)
|: \.   \\  | |:  |  (:  |  | . )|:     \/  |:  |  |    __/  \/      |
|.  \    \. | |.  |   \\ \__/ // (|  _  \\  |.  |  (// _  \  // ____)_ 
|    \    \ | /\  |\  /\\ __ //\ |: |_)  :) /\  |\ |: | \  \(:      "|
 \___|\____\)(__\_|_)(__________)(_______/ (__\_|_)(__|  \__)\_______)
)";

        printCenteredBlock(asciiArt, 165);
        cout << getCenteredString("Bicycle Rental Services", 165) << endl << endl;

        cout << getCenteredString("1. Log In ", 165) << endl;
        cout << getCenteredString("2. Sign Up", 165) << endl;
        cout << getCenteredString("Option:   ", 165);

        int loginOpt;
        if (!(cin >> loginOpt)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "\nInvalid choice! Please enter 1 or 2." << endl;
            cout << "Press Enter to continue...";
            cin.get();
            continue;
        }

        // Clean newline buffer immediately after integer input
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (loginOpt) {
            case 1: {
                string loginUIn, loginPwdIn;
                clearScreen();
                const string loginInBox = R"(
+---------------------------------------------+
|                    Login                    |
+---------------------------------------------+
)";
                printCenteredBlock(loginInBox, 165);

                cout << getCenteredString("Username: ", 160);
                getline(cin, loginUIn);
                cout << getCenteredString("Password: ", 160);
                getline(cin, loginPwdIn);

                // Trim user input to prevent trailing space mismatches
                string cleanUIn = trim(loginUIn);
                string cleanPwdIn = trim(loginPwdIn);

                if (cleanUIn.empty() || cleanPwdIn.empty()) {
                    cout << "\nUsername and Password cannot be empty." << endl;
                    cout << "Press Enter to continue...";
                    cin.get();
                    continue;
                }

                bool found = false;
                for (const auto &c : dm.customers) {
                    // Fully trim database values
                    string dbUser = trim(c.username);
                    string dbPwd  = trim(c.password);

                    if (dbUser == cleanUIn) {
                        found = true;
                        if (dbPwd == cleanPwdIn) {
                            currentCustomer = c; // Save logged-in customer
                            cout << "\nLogin successful. Welcome " << c.customerName << "!" << endl;
                            cout << "Press Enter to continue...";
                            cin.get();
                            return 0;
                        } else {
                            cout << "\nIncorrect password." << endl;
                        }
                        break;
                    }
                }

                if (!found) {
                    cout << "\nUsername not found. Please sign up first." << endl;
                }

                cout << "Press Enter to continue...";
                cin.get();
                continue;
            }

            case 2: {
                string signupUIn, signupPwdIn, customerName, customerIc;
                clearScreen();
                const string signupInBox = R"(
+---------------------------------------------+
|                   Sign Up                   |
+---------------------------------------------+
)";
                printCenteredBlock(signupInBox, 165);

                while (true) {
                    cout << getCenteredString("Name (as per IC): ", 170);
                    getline(cin, customerName);
                    customerName = trim(customerName);
                    if (customerName.empty()) {
                        cout << "Error: Name cannot be empty.\n";
                    } else if (containsComma(customerName)) {
                        cout << "Error: Name cannot contain commas.\n";
                    } else {
                        break;
                    }
                }

                while (true) {
                    cout << getCenteredString("IC number: ", 170);
                    getline(cin, customerIc);
                    customerIc = trim(customerIc);
                    if (customerIc.empty()) {
                        cout << "Error: IC cannot be empty.\n";
                    } else if (containsComma(customerIc)) {
                        cout << "Error: IC cannot contain commas.\n";
                    } else {
                        break;
                    }
                }

                while (true) {
                    cout << getCenteredString("Username: ", 170);
                    getline(cin, signupUIn);
                    signupUIn = trim(signupUIn);

                    if (signupUIn.empty()) {
                        cout << "Error: Username cannot be empty.\n";
                    } else if (signupUIn.find(' ') != string::npos) {
                        cout << "Error: Username cannot contain spaces.\n";
                    } else if (containsComma(signupUIn)) {
                        cout << "Error: Username cannot contain commas.\n";
                    } else {
                        bool duplicate = false;
                        for (const auto &c : dm.customers) {
                            if (trim(c.username) == signupUIn) {
                                duplicate = true;
                                break;
                            }
                        }
                        if (duplicate) {
                            cout << "Error: Username is already taken. Please pick another.\n";
                        } else {
                            break;
                        }
                    }
                }

                while (true) {
                    cout << getCenteredString("Password: ", 170);
                    getline(cin, signupPwdIn);
                    signupPwdIn = trim(signupPwdIn);
                    if (signupPwdIn.length() < 4) {
                        cout << "Error: Password must be at least 4 characters long.\n";
                    } else if (containsComma(signupPwdIn)) {
                        cout << "Error: Password cannot contain commas.\n";
                    } else {
                        break;
                    }
                }

                Customer newC;
                newC.customerId = "C" + to_string(dm.customers.size() + 1);
                newC.customerName = customerName;
                newC.customerIc = customerIc;
                newC.payStatus = "None";
                newC.history = "New account created";
                newC.username = signupUIn;
                newC.password = signupPwdIn;
                newC.isMember = false; //placeholder for now

                dm.customers.push_back(newC);
                saveCustomers(dm.customers);

                currentCustomer = newC; // Save new customer data!

                cout << "\nSign up successful! Your account has been registered." << endl;
                cout << "Press Enter to continue...";
                cin.get();
                return 0;
            }

            default: {
                cout << "\nInvalid option. Please choose 1 or 2." << endl;
                cout << "Press Enter to continue...";
                cin.get();
                continue;
            }
        }
    }
}

void menu(DataManager &dm, const Customer &currentCustomer)
{
    while (true) {
        clearScreen();
        const string menuText = R"(
    1. Rent
    2. Top Up Time
    3. Return Bike
    4. Payment
    5. History
    6. Exit
    )";

        printCenteredBlock(menuText, 165);
        int menuOpt;
        cout << getCenteredString("Option: ", 165);

        if (!(cin >> menuOpt)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid menu choice. Press Enter to continue...";
            cin.get();
            continue;
        }

        switch (menuOpt)
        {
            case 1:
            {
                vector<int> userRentals = rentalMenu(dm);

                if (!userRentals.empty()) {
                    checkOut(dm, currentCustomer, userRentals);
                }
                break;
            }
            case 2:
            case 3:
            case 4:
            {
                cout << "\nFeature coming soon! Press Enter to continue...";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cin.get();
                break;
            }
            case 5:
            {
                History history;
                history.displayUserHistory(dm, currentCustomer.customerId);
                break;
            }
            case 6:
            {
                cout << "\nLogging out...\n";
                return;
            }
            default:
            {
                cout << "\nInvalid option. Press Enter to continue...";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cin.get();
                break;
            }
        }
    }
}