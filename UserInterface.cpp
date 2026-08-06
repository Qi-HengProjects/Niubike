#include "UserInterface.h"
#include "DatabaseEngine.h"
#include "Helpers.h"
#include "rental.h"
#include <iostream>
#include <sstream>
#include <limits>
#include <algorithm>

using namespace std;

static bool containsComma(const string &s) {
    return s.find(',') != string::npos;
}

int login(DataManager &dm) {
    while (true) {
        clearScreen();

        const string asciiArt = R"(
 _____  ___    __     ____  ____  _______    __     __   ___  _______ 
(\"   \|"  \  |" \   ("  _||_ " ||   _  "\  |" \   |/"| /  ")/"     "|
|.\\   \    | ||  |  |   (  ) : |(. |_)  :) ||  |  (: |/   /(: ______)
|: \.   \\  | |:  |  (:  |  | . )|:     \/  |:  |  |    __/  \/      |
|.  \    \. | |.  |   \\ \__/ // (|  _  \\  |.  |  (// _  \  // ___)_ 
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

                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                cout << getCenteredString("Username: ", 160);
                getline(cin, loginUIn);
                cout << getCenteredString("Password: ", 160);
                getline(cin, loginPwdIn);

                if (loginUIn.empty() || loginPwdIn.empty()) {
                    cout << "\nUsername and Password cannot be empty." << endl;
                    cout << "Press Enter to continue...";
                    cin.get();
                    continue;
                }

                bool found = false;
                for (const auto &c : dm.customers) {
                    string dbUser = c.username;
                    string dbPwd = c.password;
                    if (!dbUser.empty() && dbUser.back() == '\r') dbUser.pop_back();
                    if (!dbPwd.empty() && dbPwd.back() == '\r') dbPwd.pop_back();

                    if (dbUser == loginUIn) {
                        found = true;
                        if (dbPwd == loginPwdIn) {
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

                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                while (true) {
                    cout << getCenteredString("Name (as per IC): ", 170);
                    getline(cin, customerName);
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

                    if (signupUIn.empty()) {
                        cout << "Error: Username cannot be empty.\n";
                    } else if (signupUIn.find(' ') != string::npos) {
                        cout << "Error: Username cannot contain spaces.\n";
                    } else if (containsComma(signupUIn)) {
                        cout << "Error: Username cannot contain commas.\n";
                    } else {
                        bool duplicate = false;
                        for (const auto &c : dm.customers) {
                            string dbUser = c.username;
                            if (!dbUser.empty() && dbUser.back() == '\r') dbUser.pop_back();

                            if (dbUser == signupUIn) {
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

                dm.customers.push_back(newC);
                saveCustomers(dm.customers);

                cout << "\nSign up successful! Your account has been registered." << endl;
                cout << "Press Enter to continue...";
                cin.get();
                return 0;
            }

            default: {
                cout << "\nInvalid option. Please choose 1 or 2." << endl;
                cout << "Press Enter to continue...";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cin.get();
                continue;
            }
        }
    }
}

// Updated menu to accept DataManager reference
void menu(DataManager &dm)
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
                // Pass DataManager instance to rentalMenu
                vector<int> userRentals = rentalMenu(dm);
                
                if (!userRentals.empty()) {
                    printInvoice(dm, userRentals);
                }
                break;
            }   
            case 2:
            case 3:
            case 4:
            case 5:
            {
                cout << "\nFeature coming soon! Press Enter to continue...";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cin.get();
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