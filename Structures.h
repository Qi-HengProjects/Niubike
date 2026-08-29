#ifndef NIUBIKE_STRUCTURES_H
#define NIUBIKE_STRUCTURES_H
# include <string>
# include <chrono>
# include <vector>
# include <ctime>

using namespace std;

struct Bicycle {
    string bikeId;
    string bikeType;
    double price;
    string status;
    string maintenance;
};

struct Rental {
    string rentalId;
    string rentalDuration;
    string paymentStatus;
    string rentingStatus;
    double rentingPrice;
    double deposit;
    double amountPaid = 0.0;
    string custId;
    vector<string> bikeIdsStr;

    // When the rental was checked out (epoch seconds). Used by the late-return
    // penalty calculator to measure real elapsed time against the planned
    // duration. 0 means "unknown" (e.g. a rental saved before this feature
    // existed) -- late fees are skipped for those rather than guessed.
    time_t checkoutTime = 0;

    // Permanent record of which bike(s) this rental was created for. Unlike
    // bikeIdsStr (which shrinks as bikes are returned, and can end up empty),
    // this never changes after checkout, so fleet/popularity analytics can
    // still attribute historical rentals to a bike/category after return.
    vector<string> originalBikeIdsStr;

    // True once this rental has had hours added via Top-Up. Cancellation is
    // only offered for rentals that are still Pending AND have never been
    // topped up -- this flag is how that second condition is checked, since
    // rentalDuration/rentingPrice alone don't reveal whether they were ever
    // extended after checkout.
    bool toppedUp = false;
};

struct Customer {
    string customerId;
    string customerName;
    string customerIc;
    string payStatus;
    string username;
    string password;
    string isMember;
};

struct DataManager {
    vector<Bicycle> bicycles;
    vector<Rental> rentals;
    vector<Customer> customers;
};

#endif //NIUBIKE_STRUCTURES_H
