#include "ReturnModule.h"
#include "DatabaseEngine.h"
#include "Helpers.h"
#include "PaymentModule.h"
#include "PenaltyModule.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

using namespace std;

// Parses the leading integer out of a "N hours" duration string.
static int parsePlannedHours(const string &durationStr) {
    int hours = 0;
    stringstream ss(durationStr);
    ss >> hours;
    return hours;
}

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
        cout << getCenteredString("No active rentals found for your account.", 165) << endl << endl;
        cout << getCenteredString("Press Enter to return to main menu...", 165);
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
        return;
    }

    cout << getCenteredString("============================================", 165) << endl;
    cout << getCenteredString("                RETURN BIKES                ", 165) << endl;
    cout << getCenteredString("============================================", 165) << endl;
    cout << getCenteredString("Your currently rented bike(s):", 165) << endl << endl;

    for (size_t i = 0; i < returnable.size(); i++) {
        Bicycle *bike = findBicycleById(dm, returnable[i].bikeId);
        string bikeType = bike ? bike->bikeType : "Unknown";
        double hourlyRate = bike ? bike->price : 0.0;

        // Non-destructive preview so the customer knows what to expect before
        // they commit to returning this particular bike.
        int plannedHours = parsePlannedHours(returnable[i].rental->rentalDuration);
        LateFeeResult preview = calculateLateFee(plannedHours, returnable[i].rental->checkoutTime, hourlyRate);

        ostringstream line;
        line << (i + 1) << ". Bike " << returnable[i].bikeId
             << " (" << bikeType << ")"
             << " | Rental " << returnable[i].rental->rentalId
             << " | Payment: " << returnable[i].rental->paymentStatus;
        if (preview.isLate) {
            line << " | [LATE by " << (int)preview.lateHours << "h -- est. fee $"
                 << fixed << setprecision(2) << preview.feeAmount << "]";
        } else {
            line << " | [On Time]";
        }
        cout << getCenteredString(line.str(), 165) << endl;
    }

    int choice = 0;
    cout << endl << getCenteredString("Which bike are you returning (0 to cancel)? ", 165);
    while (true) {
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            checkEofOrExit();
            cout << getCenteredString("Invalid input, try again: ", 165);
            continue;
        }
        if (choice == 0) {
            cout << endl << getCenteredString("Return cancelled.", 165) << endl;
            cout << endl << getCenteredString("Press Enter to return to main menu...", 165);
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.get();
            return;
        }
        if (choice >= 1 && choice <= static_cast<int>(returnable.size())) {
            break;
        }
        cout << getCenteredString("Invalid choice, try again: ", 165);
    }

    Rental *r = returnable[choice - 1].rental;
    string bikeId = returnable[choice - 1].bikeId;

    ostringstream selMsg;
    selMsg << "You selected bike " << bikeId << " from Rental " << r->rentalId << ".";
    cout << endl << getCenteredString(selMsg.str(), 165) << endl;

    // A bike can't be returned while its rental is still unpaid -- send the
    // customer to settle payment first, then let them come back to return it.
    if (trimString(r->paymentStatus) != "Paid") {
        ostringstream unpaidMsg;
        unpaidMsg << "[!] Rental " << r->rentalId << " for bike " << bikeId
                  << " has not been paid yet. Payment must be settled before this bike can be returned.";
        cout << endl << getCenteredString(unpaidMsg.str(), 165) << endl;
        cout << getCenteredString("Redirecting to the Payment page...", 165) << endl << endl;
        cout << getCenteredString("Press Enter to proceed to payment...", 165);
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();

        processPayment(dm, currentCustomer);

        // Re-check after the payment flow -- the user may have cancelled,
        // paid for a different rental, or paid for this one.
        if (trimString(r->paymentStatus) != "Paid") {
            ostringstream failMsg;
            failMsg << "[!] Payment was not completed for Rental " << r->rentalId
                    << ". Please pay first, then return the bike again.";
            cout << endl << getCenteredString(failMsg.str(), 165) << endl;
            cout << endl << getCenteredString("Press Enter to return to main menu...", 165);
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.get();
            return;
        }

        ostringstream successPayMsg;
        successPayMsg << "[+] Payment settled for Rental " << r->rentalId
                      << ". You can now proceed with returning bike " << bikeId << ".";
        cout << endl << getCenteredString(successPayMsg.str(), 165) << endl;
        cout << getCenteredString("Press Enter to continue...", 165);
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
    }

    int confirm = 0;
    do {
        cout << endl << getCenteredString("Are you returning this bike?", 165) << endl;
        cout << getCenteredString("1. Yes", 165) << endl;
        cout << getCenteredString("2. No", 165) << endl;
        cout << getCenteredString("Option: ", 165);
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

        // Compute the late-return penalty (if any) BEFORE the planned
        // duration/cost bookkeeping below, since it depends on the original
        // planned hours and the rental's checkout timestamp.
        int hours = parsePlannedHours(r->rentalDuration);
        LateFeeResult lateFee = calculateLateFee(hours, r->checkoutTime, hourlyRate);
        bool wasFullyPaid = (trimString(r->paymentStatus) == "Paid");

        // Remove this bike from its rental's bike list (partial return);
        // the rest of that rental's bikes, if any, stay Active/returnable.
        r->bikeIdsStr.erase(
            remove(r->bikeIdsStr.begin(), r->bikeIdsStr.end(), bikeId),
            r->bikeIdsStr.end()
        );

        // Work out this bike's share of the rental's cost/deposit so the
        // remaining rental total reflects only the bike(s) still out.
        double costShare = hourlyRate * hours;

        r->rentingPrice = max(0.0, r->rentingPrice - costShare);
        r->deposit = max(0.0, r->deposit - DEPOSIT_PER_BIKE);

        // Once every bike from this rental has been returned, close it out.
        bool fullyClosed = r->bikeIdsStr.empty();
        if (fullyClosed) {
            r->rentingStatus = "Returned";
        }

        // Apply the tiered late fee, if any, on top of whatever remains.
        // A fresh charge on an already-Paid rental reopens it as Pending so
        // it correctly shows up in the customer's outstanding-payments list.
        if (lateFee.isLate) {
            if (fullyClosed && wasFullyPaid) {
                r->amountPaid = 0.0;
            }
            r->rentingPrice += lateFee.feeAmount;
            r->paymentStatus = "Pending";
        }

        saveBicycles(dm.bicycles);
        saveRentals(dm.rentals);

        ostringstream returnSuccessMsg;
        returnSuccessMsg << "[+] Bike " << bikeId << " returned successfully!";
        cout << endl << getCenteredString(returnSuccessMsg.str(), 165) << endl;

        if (lateFee.isLate) {
            ostringstream lateMsg1, lateMsg2;
            lateMsg1 << "[!] This bike was returned " << (int)lateFee.lateHours
                      << " hour(s) late (" << lateFee.tierLabel << ").";
            lateMsg2 << "    Late fee charged: $" << fixed << setprecision(2) << lateFee.feeAmount;
            cout << getCenteredString(lateMsg1.str(), 165) << endl;
            cout << getCenteredString(lateMsg2.str(), 165) << endl;
        }
        
        double balanceDue = (r->rentingPrice + r->deposit) - r->amountPaid;
        bool stillOwesMoney = (trimString(r->paymentStatus) != "Paid") && balanceDue > 0.005;

        if (stillOwesMoney) {
            ostringstream ssDue;
            ssDue << fixed << setprecision(2) << max(balanceDue, 0.0);
            ostringstream balMsg;
            balMsg << "[!] You have an outstanding balance of $" << ssDue.str()
                   << " on Rental " << r->rentalId
                   << " (deposit is only refundable once fully paid).";
            cout << getCenteredString(balMsg.str(), 165) << endl;
            cout << getCenteredString("Redirecting to the Payment page...", 165) << endl << endl;
            cout << getCenteredString("Press Enter to proceed to payment...", 165);
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.get();
            processPayment(dm, currentCustomer);
            return;
        } else {
            ostringstream depMsg;
            depMsg << "[+] Please claim your refundable deposit for this bike: $"
                   << fixed << setprecision(2) << DEPOSIT_PER_BIKE;
            cout << getCenteredString(depMsg.str(), 165) << endl;
        }
    } else {
        cout << endl << getCenteredString("Return cancelled. Your rental remains active.", 165) << endl;
    }

    cout << endl << getCenteredString("Press Enter to return to main menu...", 165);
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}