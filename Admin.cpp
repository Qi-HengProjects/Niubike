#include "Admin.h"
#include "History.h"
#include "Helpers.h"
#include "DatabaseEngine.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <limits>

using namespace std;

void  admin(DataManager &dm)
{
    while (true) {
        clearScreen();

        const string asciiArt = R"(
      __      _________   ___      ___   __    _____  ___   
    /""\     |"       "\|"  \    /"  | |"  \ (\"    \|"  \  
    /    \    (.  ___  :) \   \  //   | ||  | |.\\   \    | 
   /' /\  \   |: \   ) || /\\  \/.    | |:  | |: \.   \\  | 
  //  __'  \  (| (___\ |||: \.        | |.  | |.  \    \. | 
 /   /  \\  \ |:       :)|.  \    /:  | /\  |\|    \    \ | 
(___/    \___)(________/ |___|\__/|___|(__\_|_)\___|\____\) 
                                                            
)";

        printCenteredBlock(asciiArt, 165);
        const string adminBox = R"(
+---------------------------------------------+
|                    Admin                    |
+---------------------------------------------+
)";
        printCenteredBlock(adminBox, 165);
        cout << endl;

        cout << getCenteredString("1. View Sales      ", 165) << endl;
        cout << getCenteredString("2. Rental Status   ", 165) << endl;
        cout << getCenteredString("3. Bike Maintenance", 165) << endl;
        cout << getCenteredString("4. Rental Log      ", 165) << endl;
        cout << getCenteredString("5. Inventory       ", 165) << endl;
        cout << getCenteredString("Option:   ", 165);

        int adminOpt;
        if (!(cin >> adminOpt)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "\nInvalid choice! Please enter 1 or 2." << endl;
            cout << "Press Enter to continue...";
            cin.get();
            continue;
        }

        // Clean newline buffer immediately after integer input
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}