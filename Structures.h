#ifndef NIUBIKE_STRUCTURES_H
#define NIUBIKE_STRUCTURES_H
# include <string>
# include <chrono>
# include <vector>

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
