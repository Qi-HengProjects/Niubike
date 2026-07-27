#include "DatabaseEngine.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
using namespace std;

void loadCustomers(vector<Customer> &customers) {
    ifstream file("customers.txt");
    if (!file.is_open()) {
        cout << "Warning: Could not open customers.txt. Starting with empty customer list." << endl;
        return;
    }

    string line;
    while (getline(file,line )) {
        if (line.empty()) {
            continue;
        }
        stringstream ss(line);
        string id, name, ic, hist, payStatus;
        if (getline(ss, id, ',') &&
            getline(ss, name, ',') &&
            getline(ss, ic, ',') &&
            getline(ss, payStatus, ',')&&
            getline(ss, hist, ',')) {
            Customer temp;
            temp.customer_id = id;
            temp.customer_name = name;
            temp.customer_ic = ic;
            temp.payStatus = payStatus;
            temp.history = hist;

            customers.push_back(temp);
            }
    }
    file.close();
}

void loadBicycles(vector<Bicycle> &bicycles) {
    ifstream file("bicycles.txt");
    if (!file.is_open()) {
        cout << "Warning: Could not open bicycles.txt. Starting with empty bicycle list." << endl;
        return;
    }
    string line;
    while (getline(file,line )) {
        if (line.empty()) {
            continue;
        }
        stringstream ss(line);
        string id, type, price, status, maintenance;
        if (getline(ss, id, ',') &&
            getline(ss, type, ',') &&
            getline(ss, price, ',') &&
            getline(ss, status, ',') &&
            getline(ss, maintenance, ',')) {
            Bicycle temp;
            temp.bike_id = id;
            temp.bike_type = type;
            temp.price = stod(price);
            temp.status = status;
            temp.maintenance = maintenance;

            bicycles.push_back(temp);
            }
    }
    file.close();
}

void loadRentals(vector<Rental> &rentals) {
    ifstream file("rentals.txt");
    if (!file.is_open()) {
        cout << "Warning: Could not open rentals.txt. Starting with empty rental list." << endl;
        return;
    }
    string line;
    while (getline(file,line)) {
        if (line.empty()) {
            continue;
        }
        stringstream ss(line);
        string id, duration, payment_status, renting_status, renting_price, deposit;
        if (getline(ss, id, ',') &&
            getline(ss, duration, ',') &&
            getline(ss, payment_status, ',') &&
            getline(ss, renting_status, ',') &&
            getline(ss, renting_price, ',') &&
            getline(ss, deposit, ',')) {
            Rental temp;
            temp.rental_id= id;
            temp.rental_duration = duration;
            temp.payment_status = payment_status;
            temp.renting_status = renting_status;
            temp.renting_price = stod(renting_price);
            temp.deposit = stod(deposit);
            rentals.push_back(temp);
            }
    }
    file.close();
}

void saveCustomers(const vector<Customer> &customers) {
    ofstream file("customers.txt");
    if (file.is_open()) {
        for (const auto &c : customers) {
            file << c.customer_id << ','
            << c.customer_name << ','
            << c.customer_ic << ','
            << c.payStatus << ','
            << c.history
            << endl;
        }
        file.close();
    }
}

void saveBicycles(const vector<Bicycle> &bicycles) {
    ofstream file("bicycles.txt");
    if (file.is_open()) {
        for (const auto &b : bicycles) {
            file <<  b.bike_id << ','
            << b.bike_type << ','
            << b.price << ','
            << b.status << ','
            << b.maintenance
            <<endl;
        }
        file.close();
    }
}

void saveRentals(const vector<Rental> &rentals) {
    ofstream file("rentals.txt");
    if (file.is_open()) {
        for (const auto &r : rentals) {
            file << r.rental_id << ','
            << r.rental_duration << ','
            << r.payment_status << ','
            << r.renting_status << ','
            << r.renting_price << ','
            << r.deposit
            << endl;
        }
        file.close();
    }
}

void loadAllDatabases(DataManager &dm) {
    loadCustomers(dm.customers);
    loadBicycles(dm.bicycles);
    loadRentals(dm.rentals);
}

void saveAllDatabases(const DataManager &dm) {
    saveCustomers(dm.customers);
    saveBicycles(dm.bicycles);
    saveRentals(dm.rentals);
}
