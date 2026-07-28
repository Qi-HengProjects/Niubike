#include <iostream>
#include <string>
#include <sstream>
#include "DatabaseEngine.h"

using namespace std;

void clearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

string getCenteredString(const string& text, int width = 80)
{
    int padding = (width - static_cast<int>(text.length())) / 2;
    if (padding > 0)
    {
        return string(padding, ' ') + text;
    }
    return text;
}


void printCenteredBlock(const string& text, int width = 80)
{
    stringstream ss(text);
    string line;

    while (getline(ss, line))
    {
        cout << getCenteredString(line, width) << '\n';
    }
}

int login(DataManager &dm)
{
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
                clearScreen();
                const string loginInBox = R"(
                +---------------------------------------------+
                |                    Login                    |
                +---------------------------------------------+
                )";
                printCenteredBlock(loginInBox, 165);
                string loginUIn, loginPwdIn;
                cout << getCenteredString("Username: ", 170);
                cin >> loginUIn;
                cout << getCenteredString("Password: ", 170);
                cin >> loginPwdIn;

                bool found = false;
                for (const auto &c : dm.customers) {
                    if (c.username == loginUIn) {
                        found = true;
                        if (c.password == loginPwdIn) {
                            cout << "Login successful. Welcome " << c.customer_name << endl;
                            return 0;   // success -> exit the loop/function
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
                continue;   // back to option screen
            }
            case 2: {
                clearScreen();
                const string signupInBox = R"(
                +---------------------------------------------+
                |                  Sign Up                    |
                +---------------------------------------------+
                )";
                printCenteredBlock(signupInBox, 165);
                string signupUIn, signupPwdIn;
                cout << getCenteredString("Username: ", 170);
                cin >> signupUIn;
                cout << getCenteredString("Password: ", 170);
                cin >> signupPwdIn;

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
                    continue;   // back to option screen
                }

                Customer newC;
                newC.customer_id = to_string(dm.customers.size() + 1);
                newC.username = signupUIn;
                newC.password = signupPwdIn;
                dm.customers.push_back(newC);
                saveCustomers(dm.customers);

                cout << "Sign up successful." << endl;
                return 0;   // success -> exit the loop/function
            }
            default: {
                cout << "Invalid option." << endl;
                cout << "Press Enter to continue...";
                cin.ignore();
                cin.get();
                continue;   // back to option screen
            }
        }
    }
}

void menu()
{
    const string menu = R"(
    1. Rent
    2. Top Up Time
    3. Return Bike
    4. Payment
    5. History
    )";

    printCenteredBlock(menu, 165);
    int menuOPt;
    cout << getCenteredString("Option: ", 165);
    cin >> menuOPt;
    

}

int main()
{
    DataManager dm;
    loadAllDatabases(dm);   // load customers.txt, bicycles.txt, rentals.txt into dm

    login(dm);

    saveAllDatabases(dm);   // persist any changes before exiting
    return 0;
}