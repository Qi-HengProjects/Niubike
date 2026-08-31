#include "MainMenu.h"
#include "DatabaseEngine.h"
#include "Helpers.h"
#include "History.h"
#include "RentalModule.h"
#include "PaymentModule.h"
#include "ReturnModule.h"
#include "LoyaltyModule.h"
#include <iostream>
#include <limits>
#include <algorithm>

using namespace std;

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

        // System logo banner
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

        // Main access options
        cout << getCenteredString("1. Log In ", 165) << endl;
        cout << getCenteredString("2. Sign Up", 165) << endl;
        cout << getCenteredString("3. Exit   ", 165) << endl;
        cout << getCenteredString("Option:   ", 165);

        // Input validation for authentication menu
        int loginOpt;
        if (!(cin >> loginOpt)) {
            cin.clear(); // Clear stream fail state
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear stream buffer
            cout << "\nInvalid choice! Please enter 1 or 2." << endl;
            cout << "Press Enter to continue...";
            cin.get();
            continue;
        }

        // Clean newline buffer immediately after integer input
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (loginOpt) {
            // Case 1: User Login Verification
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

                // Hardcoded administrative credentials
                string adminU = "NiuB";
                string adminPwd = "yrugay";

                // 1. CHECK ADMIN FIRST
                if (cleanUIn == adminU) {
                    if (cleanPwdIn == adminPwd) {
                        cout << "\nAdmin login successful. Welcome NiuB!" << endl;
                        cout << "Press Enter to continue...";
                        cin.get();
            
                        // Return signal for main controller to launch admin module
                        return 1; // Exit login after admin session ends
                    } else {
                        cout << "\nIncorrect password." << endl;
                        cout << "Press Enter to continue...";
                        cin.get();
                        continue;
                    }
                }

                // 2. CHECK CUSTOMERS LOOP (Only runs if NOT Admin)
                bool found = false;
                for (const auto &c : dm.customers) {
                    // Fully trim database values to compare safely
                    string dbUser = trim(c.username);
                    string dbPwd  = trim(c.password);

                    if (dbUser == cleanUIn) {
                        found = true;
                        if (dbPwd == cleanPwdIn) {
                            currentCustomer = c; // Save logged-in customer session data
                            cout << "\nLogin successful. Welcome " << c.customerName << "!" << endl;
                            cout << "Press Enter to continue...";
                            cin.get();
                            return 0; // Return signal for standard customer session
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

            // Case 2: New Customer Account Registration
            case 2: {
                string signupUIn, signupPwdIn, customerName, customerIc;
                clearScreen();
                const string signupInBox = R"(
+---------------------------------------------+
|                   Sign Up                   |
+---------------------------------------------+
)";
                printCenteredBlock(signupInBox, 165);

                // Validate customer full name
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

                // Validate Identification Card (IC) format rules
                while (true) {
                    cout << getCenteredString("IC number (12 digits, no dashes): ", 170);
                    getline(cin, customerIc);
                    customerIc = trim(customerIc);
                    if (customerIc.empty()) {
                        cout << "Error: IC cannot be empty.\n";
                    } else if (customerIc.find('-') != string::npos) {
                        cout << "Error: IC should not contain dashes -- digits only, e.g. 030115071234.\n";
                    } else if (!std::all_of(customerIc.begin(), customerIc.end(), ::isdigit)) {
                        cout << "Error: IC should only contain digits.\n";
                    } else if (customerIc.length() != 12) {
                        cout << "Error: IC must be exactly 12 digits long (yours has " << customerIc.length() << ").\n";
                    } else {
                        break;
                    }
                }

                // Validate username availability and format
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
                        // Check for existing duplicate username in database
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

                // Validate password rules
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

                // Construct new customer structure with auto-generated ID
                Customer newC;
                newC.customerId = "C" + to_string(dm.customers.size() + 1);
                newC.customerName = customerName;
                newC.customerIc = customerIc;
                newC.payStatus = "None";
                newC.username = signupUIn;
                newC.password = signupPwdIn;
                newC.isMember = "False"; // Default non-member status

                // Persist new user to runtime vector and file storage
                dm.customers.push_back(newC);
                saveCustomers(dm.customers);

                currentCustomer = newC; // Save new customer data into active session!

                cout << "\nSign up successful! Your account has been registered." << endl;
                cout << "Press Enter to continue...";
                cin.get();
                return 0; // Return success signal
            }

            // Case 3: Exit System Request
            case 3: {
                cout << "\nExiting program..." << endl;
                cout << "Press Enter to continue...";
                cin.get();
                return 2; // Signal main system controller loop to terminate
            }

            default: {
                cout << "\nInvalid option. Please choose 1, 2, or 3." << endl;
                cout << "Press Enter to continue...";
                cin.get();
                continue;
            }
        }
    }
}

void menu(DataManager &dm, Customer &currentCustomer)
{
    while (true) {
        clearScreen();
        // Menu visual block layout
        const string menuText = R"(
    1. Rent            
    2. Cancel Rental   
    3. Top Up Time     
    4. Return Bike     
    5. Payment         
    6. History         
    7. Loyalty Program 
    8. Exit            
    )";

        printCenteredBlock(menuText, 165);
        int menuOpt;
        cout << getCenteredString("Option: ", 165);

        // Validate menu choice input
        if (!(cin >> menuOpt)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid menu choice. Press Enter to continue...";
            cin.get();
            continue;
        }

        // Route choice to corresponding module handlers
        switch (menuOpt)
        {
            case 1:
            {
                // Launch rental selection process
                vector<int> userRentals = rentalMenu(dm);

                // Proceed to checkout if bicycles were selected
                if (!userRentals.empty()) {
                    checkOut(dm, currentCustomer, userRentals);
                }
                break;
            }
            case 2:
                // Handle rental cancellation requests
                handleCancelMenu(dm, currentCustomer);
                break;
            case 3:
                // Handle extending active rental durations
                handleTopUpMenu(dm, currentCustomer);
                break;
            case 4:
                // Process bicycle return workflows
                returnBikeLogic(dm, currentCustomer);
                break;
            case 5:
                // Process pending rental payments
                processPayment(dm, currentCustomer);
                break;
            case 6:
                // View customer transaction logs
                displayUserHistory(dm, currentCustomer.customerId);
                break;
            case 7:
                // Access member benefits and tier management
                handleLoyaltyMenu(dm, currentCustomer);
                break;
            case 8:
                // Logout and return to primary authentication screen
                cout << "\nLogging out...\n";
                cout << "Press Enter to continue...";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cin.get();
                return; // Exit customer menu loop back to caller
            default:
                cout << "\nInvalid option. Press Enter to continue...";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cin.get();
                break;
        }
    }
}