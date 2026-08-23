#include "Admin.h"
#include "History.h"
#include "Helpers.h"
#include "DatabaseEngine.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <limits>
#include <map>
#include <algorithm>

using namespace std;

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

        cout << getCenteredString("1. View Sales      ", 165) << endl;
        cout << getCenteredString("2. Rental Status   ", 165) << endl;
        cout << getCenteredString("3. Bike Maintenance", 165) << endl;
        cout << getCenteredString("4. Rental Log      ", 165) << endl;
        cout << getCenteredString("5. Inventory       ", 165) << endl;
        cout << getCenteredString("6. Exit            ", 165) << endl;
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
            case 3: {
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

                cout << getCenteredString(lineDouble, 165) << endl;
                cout << endl;
                cout << getCenteredString("Press Enter to return to admin menu...", 165);

                string dummy;
                getline(cin, dummy);
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
                clearScreen();
                cout << "\nLogging out of Admin...\n";
                cout << "Press Enter to continue...";
                string dummy;
                getline(cin, dummy);
                return; // exit admin() entirely -> back to login screen in main.cpp
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
void manageInventoryAndFleet(DataManager &dm) {
    const string lineDouble = "======================================================================";
    const string lineSingle = "----------------------------------------------------------------------";

    while (true) {
        clearScreen();
        dm.bicycles.clear();
        loadBicycles(dm.bicycles);

        int totalBikes = dm.bicycles.size();
        int totalAvailable = 0, totalRented = 0, totalMaint = 0;

        for (const auto &b : dm.bicycles) {
            if (b.status == "Available") totalAvailable++;
            else if (b.status == "Rented") totalRented++;
            else if (b.status == "Maintenance" || b.status == "Repair") totalMaint++;
        }

        cout << getCenteredString(lineDouble, 165) << endl;
        cout << getCenteredString("INVENTORY CONTROL & FLEET MAINTENANCE", 165) << endl;
        cout << getCenteredString(lineDouble, 165) << endl;
        cout << getCenteredString("Total: " + to_string(totalBikes) + " | Available: " + to_string(totalAvailable) + " | Rented: " + to_string(totalRented) + " | Maintenance: " + to_string(totalMaint), 165) << endl;
        cout << getCenteredString(lineSingle, 165) << endl;

        stringstream headerSS;
        headerSS << left
                 << setw(10) << "Bike ID"
                 << setw(18) << "Model / Type"
                 << setw(14) << "Hourly Rate"
                 << setw(14) << "Status"
                 << setw(20) << "Repair / Log Notes";
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
                      << setw(20) << b.maintenance;

                cout << getCenteredString(rowSS.str(), 165) << endl;
            }
        }
        cout << getCenteredString(lineDouble, 165) << endl << endl;

        cout << getCenteredString("1. Add New Physical Bicycle       ", 165) << endl;
        cout << getCenteredString("2. Update Pricing Matrix          ", 165) << endl;
        cout << getCenteredString("3. Update Maintenance Flag / Status", 165) << endl;
        cout << getCenteredString("4. Log Mechanical Repair Notes     ", 165) << endl;
        cout << getCenteredString("5. Return to Main Menu            ", 165) << endl;
        cout << getCenteredString("Option: ", 165);

        int opt;
        if (!(cin >> opt)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (opt == 5) break;

        switch (opt) {
            case 1: { // Add New Physical Bicycle
                Bicycle newBike;
                cout << "\nEnter New Bike ID (e.g., B005): ";
                cin >> newBike.bikeId;

                auto it = find_if(dm.bicycles.begin(), dm.bicycles.end(), [&](const Bicycle &b) {
                    return b.bikeId == newBike.bikeId;
                });

                if (it != dm.bicycles.end()) {
                    cout << "Error: Bicycle ID already exists!\nPress Enter to continue...";
                    string dummy; cin.ignore(numeric_limits<streamsize>::max(), '\n'); getline(cin, dummy);
                    break;
                }

                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Enter Bike Model/Type (e.g., Mountain.b, Road.b, Electric.b): ";
                getline(cin, newBike.bikeType);

                cout << "Enter Base Rental Hourly Rate (RM): ";
                while (!(cin >> newBike.price) || newBike.price <= 0) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid price! Enter a positive numeric value: ";
                }

                newBike.status = "Available";
                newBike.maintenance = "Good";

                dm.bicycles.push_back(newBike);
                saveBicycles(dm.bicycles);
                cout << "\nBicycle " << newBike.bikeId << " successfully added to fleet. ";
                cout << "\nPress Enter to continue...";
                string dummy; cin.ignore(numeric_limits<streamsize>::max(), '\n'); getline(cin, dummy);
                break;
            }
            case 2: { // Update Pricing Matrix
                cout << "\nUpdate Pricing Matrix By:";
                cout << "\n1. Specific Bike ID";
                cout << "\n2. Entire Bike Type Category";
                cout << "\nChoice: ";
                int subOpt;
                cin >> subOpt;

                if (subOpt == 1) {
                    string targetId;
                    cout << "Enter Bike ID: ";
                    cin >> targetId;
                    auto it = find_if(dm.bicycles.begin(), dm.bicycles.end(), [&](const Bicycle &b) {
                        return b.bikeId == targetId;
                    });
                    if (it != dm.bicycles.end()) {
                        cout << "Current Price: RM " << it->price << "\nEnter New Hourly Rate (RM): ";
                        cin >> it->price;
                        saveBicycles(dm.bicycles);
                        cout << "Price updated successfully.";
                    } else {
                        cout << "Bike ID not found!";
                    }
                } else if (subOpt == 2) {
                    string targetType;
                    double newPrice;
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Enter Bike Type/Category (e.g., Mountain): ";
                    getline(cin, targetType);
                    cout << "Enter New Hourly Rate (RM) for all " << targetType << " bikes: ";
                    cin >> newPrice;

                    int updatedCount = 0;
                    for (auto &b : dm.bicycles) {
                        if (b.bikeType == targetType) {
                            b.price = newPrice;
                            updatedCount++;
                        }
                    }
                    saveBicycles(dm.bicycles);
                    cout << "Updated pricing for " << updatedCount << " bicycles.";
                }
                cout << "\nPress Enter to continue...";
                string dummy; cin.ignore(numeric_limits<streamsize>::max(), '\n'); getline(cin, dummy);
                break;
            }
            case 3: { // Manage Maintenance Flag / Status
                string targetId;
                cout << "\nEnter Bike ID to change status: ";
                cin >> targetId;

                auto it = find_if(dm.bicycles.begin(), dm.bicycles.end(), [&](const Bicycle &b) {
                    return b.bikeId == targetId;
                });

                if (it != dm.bicycles.end()) {
                    cout << "\nSelect New Status for " << targetId << ":";
                    cout << "\n1. Available";
                    cout << "\n2. Maintenance";
                    cout << "\n3. Repair";
                    cout << "\n4. Retired";
                    cout << "\nChoice: ";
                    int statusChoice;
                    cin >> statusChoice;

                    switch (statusChoice) {
                        case 1: it->status = "Available"; break;
                        case 2: it->status = "Maintenance"; break;
                        case 3: it->status = "Repair"; break;
                        case 4: it->status = "Retired"; break;
                        default: cout << "Invalid choice! Status unchanged."; break;
                    }
                    saveBicycles(dm.bicycles);
                    cout << "\nStatus updated to: " << it->status;
                } else {
                    cout << "Bike ID not found!";
                }
                cout << "\nPress Enter to continue...";
                string dummy; cin.ignore(numeric_limits<streamsize>::max(), '\n'); getline(cin, dummy);
                break;
            }
            case 4: { // Log Mechanical Repair
                string targetId;
                cout << "\nEnter Bike ID to log repair notes: ";
                cin >> targetId;

                auto it = find_if(dm.bicycles.begin(), dm.bicycles.end(), [&](const Bicycle &b) {
                    return b.bikeId == targetId;
                });

                if (it != dm.bicycles.end()) {
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Current Maintenance Note: " << it->maintenance << endl;
                    cout << "Enter New Mechanical Repair / Inspection Note: ";
                    getline(cin, it->maintenance);

                    saveBicycles(dm.bicycles);
                    cout << "\nRepair notes successfully recorded and saved!";
                } else {
                    cout << "Bike ID not found!";
                }
                cout << "\nPress Enter to continue...";
                string dummy; cin.ignore(numeric_limits<streamsize>::max(), '\n'); getline(cin, dummy);
                break;
            }
            default:
                break;
        }
    }
}