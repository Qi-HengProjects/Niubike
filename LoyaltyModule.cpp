#include "LoyaltyModule.h"
#include "Helpers.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <limits>

using namespace std;

static string trimStr(const string &s) {
    size_t first = s.find_first_not_of(" \t\r\n");
    if (first == string::npos) return ""; // String is entirely whitespace
    size_t last = s.find_last_not_of(" \t\r\n");
    return s.substr(first, (last - first + 1));
}

MembershipInfo getMembershipInfo(const DataManager &dm, const Customer &cust) {
    MembershipInfo info;

    // Standardize and evaluate the membership flag
    string memberFlag = trimStr(cust.isMember);
    info.isMember = (memberFlag == "True" || memberFlag == "true" || memberFlag == "1");

    // Aggregate completed and paid rental metrics for this customer
    for (const auto &r : dm.rentals) {
        if (trimStr(r.custId) == trimStr(cust.customerId) && trimStr(r.paymentStatus) == "Paid") {
            info.completedRentals++;
            info.totalSpend += r.rentingPrice;
        }
    }

    // Default response for customers not enrolled in the program
    if (!info.isMember) {
        info.tierName = "Non-Member";
        info.discountPercent = 0.0;
        return info;
    }

    // Determine tier assignment based on usage frequency or total lifetime spending.
    // Tiers unlock on either frequency (rental count) or lifetime spend,
    // whichever the customer reaches first.
    if (info.completedRentals >= 30 || info.totalSpend >= 600.0) {
        info.tierName = "Platinum";
        info.discountPercent = 0.20; // 20% discount
    } else if (info.completedRentals >= 15 || info.totalSpend >= 300.0) {
        info.tierName = "Gold";
        info.discountPercent = 0.15; // 15% discount
    } else if (info.completedRentals >= 5 || info.totalSpend >= 100.0) {
        info.tierName = "Silver";
        info.discountPercent = 0.10; // 10% discount
    } else {
        info.tierName = "Bronze";
        info.discountPercent = 0.05; // 5% default discount for members
    }
    return info;
}

void handleLoyaltyMenu(DataManager &dm, Customer &currentCustomer) {
    clearScreen();
    string border = "============================================";

    // Compute loyalty metrics for current session user
    MembershipInfo info = getMembershipInfo(dm, currentCustomer);

    // Display program header banner
    cout << getCenteredString(border, 165) << endl;
    cout << getCenteredString("LOYALTY PROGRAM", 165) << endl;
    cout << getCenteredString(border, 165) << endl << endl;

    // View for non-members: Show benefits and opt-in prompt
    if (!info.isMember) {
        cout << getCenteredString("You are not currently enrolled in the Loyalty Program.", 165) << endl;
        cout << getCenteredString("Members get an automatic discount on every rental, based on usage:", 165) << endl << endl;
        cout << getCenteredString("Bronze   (default)                          : 5% off", 165) << endl;
        cout << getCenteredString("Silver   (5+ paid rentals or RM100+ spent)  : 10% off", 165) << endl;
        cout << getCenteredString("Gold     (15+ paid rentals or RM300+ spent) : 15% off", 165) << endl;
        cout << getCenteredString("Platinum (30+ paid rentals or RM600+ spent) : 20% off", 165) << endl << endl;

        cout << getCenteredString("1. Join Loyalty Program (free)", 165) << endl;
        cout << getCenteredString("0. Back", 165) << endl << endl;
        cout << getCenteredString("Option: ", 165);

        // Read user input with error/stream closure handling
        int choice;
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            checkEofOrExit();
            choice = 0;
        }

        // Handle loyalty enrollment
        if (choice == 1) {
            currentCustomer.isMember = "True";
            // Sync status across the customer vector in DataManager
            for (auto &c : dm.customers) {
                if (trimStr(c.customerId) == trimStr(currentCustomer.customerId)) {
                    c.isMember = "True";
                    break;
                }
            }
            // Persist updated membership status to file
            saveCustomers(dm.customers);
            cout << endl << getCenteredString("[+] Welcome to the Loyalty Program! You're now a Bronze member (5% off).", 165) << endl;
        }
    } else {
        // View for existing members: Format and present current metrics
        ostringstream ssTier, ssDisc, ssCount, ssSpend;
        ssTier  << "Current Tier             : " << info.tierName;
        ssDisc  << "Active Discount          : " << fixed << setprecision(0) << (info.discountPercent * 100) << "% off every rental";
        ssCount << "Completed & Paid Rentals : " << info.completedRentals;
        ssSpend << "Lifetime Rental Spend    : $" << fixed << setprecision(2) << info.totalSpend;

        cout << getCenteredString(ssTier.str(), 165) << endl;
        cout << getCenteredString(ssDisc.str(), 165) << endl;
        cout << getCenteredString(ssCount.str(), 165) << endl;
        cout << getCenteredString(ssSpend.str(), 165) << endl << endl;

        // Display targeted goals for reaching the next tier level
        if (info.tierName == "Bronze") {
            cout << getCenteredString("Reach 5 paid rentals or $100 spend to unlock Silver (10% off).", 165) << endl;
        } else if (info.tierName == "Silver") {
            cout << getCenteredString("Reach 15 paid rentals or $300 spend to unlock Gold (15% off).", 165) << endl;
        } else if (info.tierName == "Gold") {
            cout << getCenteredString("Reach 30 paid rentals or $600 spend to unlock Platinum (20% off).", 165) << endl;
        } else {
            cout << getCenteredString("You've reached the highest tier -- thanks for being a loyal rider!", 165) << endl;
        }
    }

    // Navigation pause before returning to caller loop
    cout << endl << getCenteredString(border, 165) << endl;
    cout << getCenteredString("Press Enter to return to main menu...", 165);
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}