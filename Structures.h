#ifndef NIUBIKE_STRUCTURES_H
#define NIUBIKE_STRUCTURES_H
# include <iostream>
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
};

struct Customer {
    string customerId;
    string customerName;
    string customerIc;
    string payStatus;
    string history;
    string username;
    string password;
};

struct DataManager {
    vector<Bicycle> bicycles;
    vector<Rental> rentals;
    vector<Customer> customers;
};

#endif //NIUBIKE_STRUCTURES_H
