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
        string id, name, ic, hist, payStatus, user, pwd;
        if (getline(ss, id, ',') &&
            getline(ss, name, ',') &&
            getline(ss, ic, ',') &&
            getline(ss, payStatus, ',')&&
            getline(ss, hist, ',') &&
            getline(ss, user, ',') &&
            getline(ss, pwd, ','))
{
            Customer temp;
            temp.customerId = id;
            temp.customerName = name;
            temp.customerIc = ic;
            temp.payStatus = payStatus;
            temp.history = hist;
            temp.username = user;
            temp.password = pwd;

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
            temp.bikeId = id;
            temp.bikeType = type;
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
            temp.rentalId= id;
            temp.rentalDuration = duration;
            temp.paymentStatus = payment_status;
            temp.rentingStatus = renting_status;
            temp.rentingPrice = stod(renting_price);
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
            file << c.customerId << ','
            << c.customerName << ','
            << c.customerIc << ','
            << c.payStatus << ','
            << c.history << ','
            << c.username << ','
            << c.password
            << endl;
        }
        file.close();
    }
}

void saveBicycles(const vector<Bicycle> &bicycles) {
    ofstream file("bicycles.txt");
    if (file.is_open()) {
        for (const auto &b : bicycles) {
            file <<  b.bikeId << ','
            << b.bikeType << ','
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
            file << r.rentalId << ','
            << r.rentalDuration << ','
            << r.paymentStatus << ','
            << r.rentingStatus << ','
            << r.rentingPrice << ','
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
