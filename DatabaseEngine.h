#ifndef NIUBIKE_DATABASEENGINE_H
#define NIUBIKE_DATABASEENGINE_H
#include "Structures.h"
#include <vector>

void loadCustomers(vector<Customer> &customers);
void loadBicycles(vector<Bicycle> &bicycles);
void loadRentals(vector<Rental> &rentals);
void saveCustomers(const vector<Customer> &customers);
void saveBicycles(const vector<Bicycle> &bicycles);
void saveRentals(const vector<Rental> &rentals);
void loadAllDatabases(DataManager &dm);
void saveAllDatabases(const DataManager &dm);
Rental* findRentalById(DataManager &dm, const string &rentalId);
Bicycle* findBicycleById(DataManager &dm, const string &bikeId);

#endif //NIUBIKE_DATABASEENGINE_H