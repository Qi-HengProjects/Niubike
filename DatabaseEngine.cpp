#include "DatabaseEngine.h"
#include "Helpers.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <stdexcept>
#include <algorithm>
using namespace std;

static double safeStod(const string &field, const string &context, double fallback = 0.0) {
    try {
        size_t consumed = 0;
        double value = stod(field, &consumed);
        return value;
    } catch (const std::exception &e) {
        cout << "Warning: could not parse numeric value \"" << field
             << "\" for " << context << ". Using " << fallback << " instead." << endl;
        return fallback;
    }
}



static long long extractIdNumber(const string &id) {
    if (id.size() < 2) return -1;
    string digits = id.substr(1);
    if (digits.empty() || !std::all_of(digits.begin(), digits.end(), ::isdigit)) {
        return -1;
    }
    try {
        return stoll(digits);
    } catch (const std::exception &) {
        return -1;
    }
}

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
        string id, name, ic, hist, payStatus, user, pwd, isMember;
        if (getline(ss, id, ',') &&
            getline(ss, name, ',') &&
            getline(ss, ic, ',') &&
            getline(ss, payStatus, ',')&&
            getline(ss, user, ',') &&
            getline(ss, pwd, ',') &&
            getline(ss, isMember, ',')){
            Customer temp;
            temp.customerId = id;
            temp.customerName = name;
            temp.customerIc = ic;
            temp.payStatus = payStatus;
            temp.username = user;
            temp.password = pwd;
            temp.isMember = isMember;

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
            temp.price = safeStod(price, "bicycle " + id + " price");
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
        string id, duration, payment_status, renting_status, renting_price, deposit, amount_paid, custId, bikeIdsStr;
        if (getline(ss, id, ',') &&
            getline(ss, duration, ',') &&
            getline(ss, payment_status, ',') &&
            getline(ss, renting_status, ',') &&
            getline(ss, renting_price, ',') &&
            getline(ss, deposit, ',') &&
            getline(ss, amount_paid, ',') &&
            getline(ss, custId, ',') &&
            getline(ss, bikeIdsStr, ',')) {
            Rental temp;
            temp.rentalId= id;
            temp.rentingStatus = renting_status;
            temp.rentalDuration = duration;
            temp.paymentStatus = payment_status;
            temp.rentingPrice = safeStod(renting_price, "rental " + id + " rentingPrice");
            temp.deposit = safeStod(deposit, "rental " + id + " deposit");
            temp.amountPaid = safeStod(amount_paid, "rental " + id + " amountPaid");
            temp.custId = custId;

            stringstream bikeSS(bikeIdsStr);
            string singleBikeId;

            // Order MUST be: (1) Stream, (2) String variable, (3) Char delimiter
            while (getline(bikeSS, singleBikeId, ';')) {
                if (!singleBikeId.empty()) {
                    if (singleBikeId.back() == '\r') singleBikeId.pop_back();
                    temp.bikeIdsStr.push_back(singleBikeId);
                }
            }

            rentals.push_back(temp);
            }
    }
    file.close();
}

void saveCustomers(const vector<Customer> &customers) {
    ofstream file("customers.txt");
    if (file.is_open()) {
        for (const auto &c : customers) {
            // sanitizeForCsv() is a safety net: it strips/replaces any stray
            // commas or newlines in free-text fields so a corrupted value can
            // never shift the column layout of the row it's written into.
            file << sanitizeForCsv(c.customerId) << ','
            << sanitizeForCsv(c.customerName) << ','
            << sanitizeForCsv(c.customerIc) << ','
            << sanitizeForCsv(c.payStatus) << ','
            << sanitizeForCsv(c.username) << ','
            << sanitizeForCsv(c.password) << ','
            << sanitizeForCsv(c.isMember)
            << endl;
        }
        file.close();
    }
}

void saveBicycles(const vector<Bicycle> &bicycles) {
    ofstream file("bicycles.txt");
    if (file.is_open()) {
        for (const auto &b : bicycles) {
            file <<  sanitizeForCsv(b.bikeId) << ','
            << sanitizeForCsv(b.bikeType) << ','
            << b.price << ','
            << sanitizeForCsv(b.status) << ','
            << sanitizeForCsv(b.maintenance)
            <<endl;
        }
        file.close();
    }
}

void saveRentals(const vector<Rental> &rentals) {
    ofstream file("rentals.txt");
    if (file.is_open()) {
        for (const auto &r : rentals) {
            file << sanitizeForCsv(r.rentalId) << ','
                 << sanitizeForCsv(r.rentalDuration) << ','
                 << sanitizeForCsv(r.paymentStatus) << ','
                 << sanitizeForCsv(r.rentingStatus) << ','
                 << r.rentingPrice << ','
                 << r.deposit << ','
                 << r.amountPaid << ','
                 << sanitizeForCsv(r.custId) << ',';

            // Join vector<string> bikeIds using ';' as a sub-delimiter
            for (size_t i = 0; i < r.bikeIdsStr.size(); ++i) {
                file << sanitizeForCsv(r.bikeIdsStr[i]);
                if (i + 1 < r.bikeIdsStr.size()) {
                    file << ';';
                }
            }

            file << endl; // Ended line without a trailing comma
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

Rental* findRentalById(DataManager &dm, const string &rentalId) {
    for (auto &r : dm.rentals) {
        if (r.rentalId == rentalId) {
            return &r;
        }
    }
    return nullptr;
}

Bicycle* findBicycleById(DataManager &dm, const string &bikeId) {
    for (auto &b : dm.bicycles) {
        if (b.bikeId == bikeId) {
            return &b;
        }
    }
    return nullptr;
}

string generateNextCustomerId(const DataManager &dm) {
    long long maxId = 0;
    for (const auto &c : dm.customers) {
        maxId = max(maxId, extractIdNumber(c.customerId));
    }
    
    for (const auto &r : dm.rentals) {
        maxId = max(maxId, extractIdNumber(r.custId));
    }
    return "C" + to_string(maxId + 1);
}

string generateNextRentalId(const DataManager &dm) {
    long long maxId = 0;
    for (const auto &r : dm.rentals) {
        maxId = max(maxId, extractIdNumber(r.rentalId));
    }
    return "R" + to_string(maxId + 1);
}