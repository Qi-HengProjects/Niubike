#ifndef NIUBIKE_STRUCTURES_H
#define NIUBIKE_STRUCTURES_H
# include <iostream>
# include <string>
# include <chrono>


using namespace std;

struct Bicycle {
    string bike_id;
    string bike_type;
    double price;
    string status;
    string maintenance;
};

struct Rental {
    string rental_id;
    string rental_duration;
    string payment_status;
    string renting_status;
    double renting_price;
    double deposit;
};

struct Customer {
    string customer_id;
    string customer_name;
    string customer_ic;
    string payStatus;
    string history;
};

struct DataManager {
    vector<Bicycle> bicycles;
    vector<Rental> rentals;
    vector<Customer> customers;
};

#endif //NIUBIKE_STRUCTURES_H
