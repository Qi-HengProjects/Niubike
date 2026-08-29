#include "ReturnModule.h"
#include "DatabaseEngine.h"
#include "Helpers.h"
#include "PaymentModule.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

using namespace std;

// Must match DEPOSIT_PER_BIKE used in RentalModule.cpp when the rental was created.
static const double DEPOSIT_PER_BIKE = 15.00;

// Helper to trim whitespace
static string trimString(const string &s) {
    size_t first = s.find_first_not_of(" \t\r\n");
    if (first == string::npos) return "";
    size_t last = s.find_last_not_of(" \t\r\n");
    return s.substr(first, (last - first + 1));
}

// One selectable row in the return list: a single bike belonging to a
// specific active rental (a rental can contain several bikes; each is
// listed and returned separately instead of forcing the whole rental,
// or only the most-recently-added bike, to be returned at once).
struct ReturnableBike {
    Rental *rental;
    string bikeId;
};

void returnBikeLogic(DataManager &dm, const Customer &currentCustomer) {
    clearScreen();

    // Flatten every bike from every active rental belonging to this customer
    // into one list, instead of only ever offering one rental/bike at a time.
    vector<ReturnableBike> returnable;
    for (auto &r : dm.rentals) {
        if (trimString(r.rentingStatus) == "Active" && trimString(r.custId) == currentCustomer.customerId) {
            for (const string &bikeId : r.bikeIdsStr) {
                returnable.push_back({&r, bikeId});
            }
        }
    }

    if (returnable.empty()) {
        cout << "No active rentals found for your account.\n\n";
        cout << "Press Enter to return to main menu...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
        return;
    }

    cout << "============================================" << endl;
    cout << "                RETURN BIKES                " << endl;
    cout << "============================================" << endl;
    cout << "Your currently rented bike(s):\n\n";

    for (size_t i = 0; i < returnable.size(); i++) {
        Bicycle *bike = findBicycleById(dm, returnable[i].bikeId);
        string bikeType = bike ? bike->bikeType : "Unknown";
        cout << "  " << (i + 1) << ". Bike " << returnable[i].bikeId
             << " (" << bikeType << ")"
             << " | Rental " << returnable[i].rental->rentalId
             << " | Payment: " << returnable[i].rental->paymentStatus << endl;
    }

    int choice = 0;
    cout << "\nWhich bike are you returning (0 to cancel)? ";
    while (true) {
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            checkEofOrExit();
            cout << "Invalid input, try again: ";
            continue;
        }
        if (choice == 0) {
            cout << "\nReturn cancelled.\n";
            cout << "\nPress Enter to return to main menu...";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.get();
            return;
        }
        if (choice >= 1 && choice <= static_cast<int>(returnable.size())) {
            break;
        }
        cout << "Invalid choice, try again: ";
    }

    Rental *r = returnable[choice - 1].rental;
    string bikeId = returnable[choice - 1].bikeId;

    cout << "\nYou selected bike " << bikeId << " from Rental " << r->rentalId << ".\n";

    int confirm = 0;
    do {
        cout << "\nAre you returning this bike?\n1. Yes\n2. No\nOption: ";
        if (!(cin >> confirm)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            checkEofOrExit();
        }
    } while (confirm != 1 && confirm != 2);

    if (confirm == 1) {
        // Mark the bike itself as "Available" again
        double hourlyRate = 0.0;
        for (auto &b : dm.bicycles) {
            if (trimString(b.bikeId) == trimString(bikeId)) {
                b.status = "Available";
                hourlyRate = b.price;
                break;
            }
        }

        // Remove this bike from its rental's bike list (partial return);
        // the rest of that rental's bikes, if any, stay Active/returnable.
        r->bikeIdsStr.erase(
            remove(r->bikeIdsStr.begin(), r->bikeIdsStr.end(), bikeId),
            r->bikeIdsStr.end()
        );

        // Work out this bike's share of the rental's cost/deposit so the
        // remaining rental total reflects only the bike(s) still out.
        int hours = 0;
        stringstream durSS(r->rentalDuration);
        durSS >> hours;
        double costShare = hourlyRate * hours;

        r->rentingPrice = max(0.0, r->rentingPrice - costShare);
        r->deposit = max(0.0, r->deposit - DEPOSIT_PER_BIKE);

        // Once every bike from this rental has been returned, close it out.
        if (r->bikeIdsStr.empty()) {
            r->rentingStatus = "Returned";
        }

        saveBicycles(dm.bicycles);
        saveRentals(dm.rentals);

        cout << "\n[+] Bike " << bikeId << " returned successfully!" << endl;

        // Only offer a deposit refund for this bike if the rental (as it now
        // stands) has no outstanding balance; otherwise send the customer to
        // the payment page instead, matching how the rest of the app treats
        // "Pending" rentals.
        double balanceDue = (r->rentingPrice + r->deposit) - r->amountPaid;
        bool stillOwesMoney = (trimString(r->paymentStatus) != "Paid") && balanceDue > 0.005;

        if (stillOwesMoney) {
            ostringstream ssDue;
            ssDue << fixed << setprecision(2) << max(balanceDue, 0.0);
            cout << "[!] You have an outstanding balance of $" << ssDue.str()
                 << " on Rental " << r->rentalId
                 << " (deposit is only refundable once fully paid)." << endl;
            cout << "Redirecting to the Payment page...\n\n";
            cout << "Press Enter to proceed to payment...";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.get();
            processPayment(dm, currentCustomer);
            return;
        } else {
            cout << "[+] Please claim your refundable deposit for this bike: $"
                 << fixed << setprecision(2) << DEPOSIT_PER_BIKE << endl;
        }
    } else {
        cout << "\nReturn cancelled. Your rental remains active." << endl;
    }

    cout << "\nPress Enter to return to main menu...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}