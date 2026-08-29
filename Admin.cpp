#include "Admin.h"
#include "History.h"
#include "Helpers.h"
#include "DatabaseEngine.h"
#include "AnalyticsModule.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <limits>
#include <map>
#include <algorithm>

using namespace std;

// Helper function to update bicycle status within Bike Maintenance
static void updateBikeMaintenanceStatus(DataManager &dm) {
    const string lineSingle = "----------------------------------------------------------------------";
    
    cout << "\n" << getCenteredString("==================================================", 165) << endl;
    cout << getCenteredString("UPDATE BICYCLE MAINTENANCE & STATUS", 165) << endl;
    cout << getCenteredString("==================================================", 165) << endl << endl;

    string targetId;
    cout << getCenteredString("Enter Bike ID to update (or 0 to cancel): ", 165);
    cin >> targetId;

    if (targetId == "0") return;

    auto it = find_if(dm.bicycles.begin(), dm.bicycles.end(), [&](const Bicycle &b) {
        return b.bikeId == targetId;
    });

    if (it == dm.bicycles.end()) {
        cout << "\n" << getCenteredString("[Error] Bike ID not found!", 165) << endl;
        cout << getCenteredString("Press Enter to continue...", 165);
        string dummy;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(cin, dummy);
        return;
    }

    cout << "\n" << getCenteredString("Current Status : " + it->status, 165) << endl;
    cout << getCenteredString("Current Condition: " + it->maintenance, 165) << endl << endl;

    cout << getCenteredString("Select Action:", 165) << endl;
    cout << getCenteredString("1. Set Status to Active / Available", 165) << endl;
    cout << getCenteredString("2. Set Status to Under Maintenance ", 165) << endl;
    cout << getCenteredString("3. Set Status to Repair Needed     ", 165) << endl;
    cout << getCenteredString("4. Set Status to Retired           ", 165) << endl;
    cout << getCenteredString("5. Update Maintenance Notes Only   ", 165) << endl;
    cout << getCenteredString("0. Cancel                          ", 165) << endl << endl;
    cout << getCenteredString("Option: ", 165);

    int choice;
    if (!(cin >> choice) || choice == 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }

    switch (choice) {
        case 1:
            it->status = "Available";
            it->maintenance = "none";
            break;
        case 2:
            it->status = "Maintenance";
            it->maintenance = "Under Inspection";
            break;
        case 3:
            it->status = "Repair";
            it->maintenance = "Needs Repair";
            break;
        case 4:
            it->status = "Retired";
            it->maintenance = "none";
            break;
        case 5: {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "\n" << getCenteredString("Enter New Maintenance Note: ", 165);
            string note;
            getline(cin, note);
            if (!note.empty()) {
                it->maintenance = note;
            }
            break;
        }
        default:
            cout << "\n" << getCenteredString("Invalid choice. Status unchanged.", 165) << endl;
            break;
    }

    saveBicycles(dm.bicycles);
    cout << "\n" << getCenteredString("[+] Bicycle " + targetId + " maintenance status updated successfully!", 165) << endl;
    cout << getCenteredString("Press Enter to continue...", 165);
    string dummy;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, dummy);
}

void admin(DataManager &dm)
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

        cout << getCenteredString("1. View Sales        ", 165) << endl;
        cout << getCenteredString("2. Rental Status     ", 165) << endl;
        cout << getCenteredString("3. Bike Maintenance  ", 165) << endl;
        cout << getCenteredString("4. Rental Log        ", 165) << endl;
        cout << getCenteredString("5. Inventory         ", 165) << endl;
        cout << getCenteredString("6. Fleet Analytics   ", 165) << endl;
        cout << getCenteredString("7. Exit              ", 165) << endl;
        cout << getCenteredString("Option:   ", 165);

        int adminOpt;
        if (!(cin >> adminOpt)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "\nInvalid choice! Please enter a valid number." << endl;
            cout << "Press Enter to continue...";
            string dummy;
            getline(cin, dummy);
            continue;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        const string lineDouble = "======================================================================";
        const string lineSingle = "----------------------------------------------------------------------";

        switch (adminOpt) {
            case 1: {
                clearScreen();
                dm.rentals.clear();
                loadRentals(dm.rentals);

                double totalSales = 0.0;
                int paidCount = 0;

                for (const auto &r : dm.rentals) {
                    if (r.paymentStatus == "Paid" || r.paymentStatus == "PAID") {
                        totalSales += r.rentingPrice;
                        paidCount++;
                    }
                }

                stringstream ssSales;
                ssSales << left << setw(28) << "Total Revenue:" << "RM" << fixed << setprecision(2) << totalSales;

                stringstream ssOrders;
                ssOrders << left << setw(28) << "Total Orders in Record:" << dm.rentals.size();

                stringstream ssPaid;
                ssPaid << left << setw(28) << "Paid Orders:" << paidCount;

                cout << getCenteredString(lineDouble, 165) << endl;
                cout << getCenteredString("VIEW SALES", 165) << endl;
                cout << getCenteredString(lineDouble, 165) << endl;
                cout << getCenteredString(ssOrders.str(), 165) << endl;
                cout << getCenteredString(ssPaid.str(), 165) << endl;
                cout << getCenteredString(ssSales.str(), 165) << endl;
                cout << getCenteredString(lineDouble, 165) << endl;
                cout << endl;
                cout << getCenteredString("Press Enter to return to admin menu...", 165);

                string dummy;
                getline(cin, dummy);
                break;
            }
            case 2: {
                clearScreen();
                dm.rentals.clear();
                loadRentals(dm.rentals);

                int activeCount = 0;
                int returnedCount = 0;

                for (const auto &r : dm.rentals) {
                    if (r.rentingStatus == "Active" || r.rentingStatus == "Ongoing") {
                        activeCount++;
                    } else if (r.rentingStatus == "Returned" || r.rentingStatus == "Completed") {
                        returnedCount++;
                    }
                }

                cout << getCenteredString(lineDouble, 165) << endl;
                cout << getCenteredString("RENTAL STATUS", 165) << endl;
                cout << getCenteredString(lineDouble, 165) << endl;
                cout << getCenteredString("Active: " + to_string(activeCount) + "   |   Returned: " + to_string(returnedCount) + "   |   Total: " + to_string(dm.rentals.size()), 165) << endl;
                cout << getCenteredString(lineSingle, 165) << endl;

                stringstream headerSS;
                headerSS << left
                         << setw(11) << "Rental ID"
                         << setw(13) << "Cust ID"
                         << setw(11) << "Duration"
                         << setw(12) << "Status"
                         << setw(11) << "Payment"
                         << setw(12) << "Bikes";
                cout << getCenteredString(headerSS.str(), 165) << endl;
                cout << getCenteredString(lineSingle, 165) << endl;

                if (dm.rentals.empty()) {
                    cout << getCenteredString("No rental records found.", 165) << endl;
                } else {
                    for (const auto &r : dm.rentals) {
                        string bikesJoined = "";
                        for (size_t i = 0; i < r.bikeIdsStr.size(); ++i) {
                            bikesJoined += r.bikeIdsStr[i];
                            if (i + 1 < r.bikeIdsStr.size()) {
                                bikesJoined += ", ";
                            }
                        }
                        if (bikesJoined.empty()) bikesJoined = "N/A";

                        stringstream rowSS;
                        rowSS << left
                              << setw(11) << r.rentalId
                              << setw(13) << r.custId
                              << setw(11) << r.rentalDuration
                              << setw(12) << r.rentingStatus
                              << setw(11) << r.paymentStatus
                              << setw(12) << bikesJoined;

                        cout << getCenteredString(rowSS.str(), 165) << endl;
                    }
                }

                cout << getCenteredString(lineDouble, 165) << endl;
                cout << endl;
                cout << getCenteredString("Press Enter to return to admin menu...", 165);

                string dummy;
                getline(cin, dummy);
                break;
            }
            case 3: { // BIKE MAINTENANCE & STATUS
                bool stayInMaint = true;
                while (stayInMaint) {
                    clearScreen();
                    dm.bicycles.clear();
                    loadBicycles(dm.bicycles);

                    int maintCount = 0;
                    int availCount = 0;
                    int rentedCount = 0;

                    for (const auto &b : dm.bicycles) {
                        if (b.status == "Maintenance" || b.status == "Repair") {
                            maintCount++;
                        } else if (b.status == "Available") {
                            availCount++;
                        } else if (b.status == "Rented") {
                            rentedCount++;
                        }
                    }

                    cout << getCenteredString(lineDouble, 165) << endl;
                    cout << getCenteredString("BIKE MAINTENANCE & STATUS", 165) << endl;
                    cout << getCenteredString(lineDouble, 165) << endl;
                    cout << getCenteredString("In Maintenance: " + to_string(maintCount) + "   |   Available: " + to_string(availCount) + "   |   Rented: " + to_string(rentedCount), 165) << endl;
                    cout << getCenteredString(lineSingle, 165) << endl;

                    stringstream headerSS;
                    headerSS << left
                             << setw(10) << "Bike ID"
                             << setw(16) << "Model/Type"
                             << setw(14) << "Status"
                             << setw(14) << "Price (RM)"
                             << setw(16) << "Maintenance";
                    cout << getCenteredString(headerSS.str(), 165) << endl;
                    cout << getCenteredString(lineSingle, 165) << endl;

                    if (dm.bicycles.empty()) {
                        cout << getCenteredString("No bicycle records found.", 165) << endl;
                    } else {
                        for (const auto &b : dm.bicycles) {
                            stringstream rowSS;
                            rowSS << left
                                  << setw(10) << b.bikeId
                                  << setw(16) << b.bikeType
                                  << setw(14) << b.status
                                  << setw(14) << fixed << setprecision(2) << b.price
                                  << setw(16) << b.maintenance;

                            cout << getCenteredString(rowSS.str(), 165) << endl;
                        }
                    }

                    cout << getCenteredString(lineDouble, 165) << endl << endl;
                    cout << getCenteredString("1. Change Maintenance Status / Notes ", 165) << endl;
                    cout << getCenteredString("0. Return to Admin Menu              ", 165) << endl << endl;
                    cout << getCenteredString("Option: ", 165);

                    int subOpt;
                    if (!(cin >> subOpt)) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        continue;
                    }

                    if (subOpt == 1) {
                        updateBikeMaintenanceStatus(dm);
                    } else if (subOpt == 0) {
                        stayInMaint = false;
                    }
                }
                break;
            }
            case 4: {
                clearScreen();
                dm.rentals.clear();
                loadRentals(dm.rentals);

                cout << getCenteredString(lineDouble, 165) << endl;
                cout << getCenteredString("RENTAL AUDIT LOG", 165) << endl;
                cout << getCenteredString(lineDouble, 165) << endl;
                cout << getCenteredString("Total Historical Records Logged: " + to_string(dm.rentals.size()), 165) << endl;
                cout << getCenteredString(lineSingle, 165) << endl;

                stringstream headerSS;
                headerSS << left
                         << setw(10) << "Log ID"
                         << setw(11) << "Cust ID"
                         << setw(13) << "Price (RM)"
                         << setw(14) << "Duration"
                         << setw(11) << "Status"
                         << setw(11) << "Payment";
                cout << getCenteredString(headerSS.str(), 165) << endl;
                cout << getCenteredString(lineSingle, 165) << endl;

                if (dm.rentals.empty()) {
                    cout << getCenteredString("No rental audit logs available.", 165) << endl;
                } else {
                    for (const auto &r : dm.rentals) {
                        stringstream rowSS;
                        rowSS << left
                              << setw(10) << r.rentalId
                              << setw(11) << r.custId
                              << setw(13) << fixed << setprecision(2) << r.rentingPrice
                              << setw(14) << r.rentalDuration
                              << setw(11) << r.rentingStatus
                              << setw(11) << r.paymentStatus;

                        cout << getCenteredString(rowSS.str(), 165) << endl;
                    }
                }

                cout << getCenteredString(lineDouble, 165) << endl;
                cout << endl;
                cout << getCenteredString("Press Enter to return to admin menu...", 165);

                string dummy;
                getline(cin, dummy);
                break;
            }
            case 5: {
                clearScreen();
                dm.bicycles.clear();
                loadBicycles(dm.bicycles);

                int totalBikes = dm.bicycles.size();
                int totalAvailable = 0;
                int totalRented = 0;

                for (const auto &b : dm.bicycles) {
                    if (b.status == "Available") totalAvailable++;
                    else if (b.status == "Rented") totalRented++;
                }

                cout << getCenteredString(lineDouble, 165) << endl;
                cout << getCenteredString("BICYCLE INVENTORY OVERVIEW", 165) << endl;
                cout << getCenteredString(lineDouble, 165) << endl;
                cout << getCenteredString("Total Bikes: " + to_string(totalBikes) + "   |   In Stock: " + to_string(totalAvailable) + "   |   On Rent: " + to_string(totalRented), 165) << endl;
                cout << getCenteredString(lineSingle, 165) << endl;

                stringstream headerSS;
                headerSS << left
                         << setw(10) << "Bike ID"
                         << setw(18) << "Model / Type"
                         << setw(14) << "Hourly Rate"
                         << setw(14) << "Stock Status"
                         << setw(14) << "Condition";
                cout << getCenteredString(headerSS.str(), 165) << endl;
                cout << getCenteredString(lineSingle, 165) << endl;

                if (dm.bicycles.empty()) {
                    cout << getCenteredString("No inventory items found in database.", 165) << endl;
                } else {
                    for (const auto &b : dm.bicycles) {
                        stringstream rowSS;
                        rowSS << left
                              << setw(10) << b.bikeId
                              << setw(18) << b.bikeType
                              << setw(14) << ("RM " + to_string((int)b.price) + ".00")
                              << setw(14) << b.status
                              << setw(14) << b.maintenance;

                        cout << getCenteredString(rowSS.str(), 165) << endl;
                    }
                }

                cout << getCenteredString(lineDouble, 165) << endl;
                cout << endl;
                cout << getCenteredString("Press Enter to return to admin menu...", 165);

                string dummy;
                getline(cin, dummy);
                break;
            }
            case 6: {
                showFleetAnalytics(dm);
                break;
            }
            case 7: {
                clearScreen();
                cout << "\nLogging out of Admin...\n";
                cout << "Press Enter to continue...";
                string dummy;
                getline(cin, dummy);
                return;
            }
            
            default: {
                cout << "\nInvalid option!" << endl;
                cout << "Press Enter to continue...";
                string dummy;
                getline(cin, dummy);
                break;
            }
        }
    }
}