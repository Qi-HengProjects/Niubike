#ifndef LOYALTYMODULE_H
#define LOYALTYMODULE_H
#include "Structures.h"
#include "DatabaseEngine.h"
#include <string>

using namespace std;

struct MembershipInfo {
    bool isMember = false;
    string tierName = "Non-Member";
    double discountPercent = 0.0;  // e.g. 0.10 == 10% off
    int completedRentals = 0;
    double totalSpend = 0.0;
};

// Computes the customer's current membership tier and active discount from
// their paid rental history. Non-members always get Non-Member/0%.
MembershipInfo getMembershipInfo(const DataManager &dm, const Customer &cust);

// Interactive screen: shows current tier/progress, or lets a non-member
// join. Takes a non-const Customer reference so joining is reflected
// immediately in the active session as well as in dm.customers/disk.
void handleLoyaltyMenu(DataManager &dm, Customer &currentCustomer);

#endif // LOYALTYMODULE_H
