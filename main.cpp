#include <iostream>
#include "Structures.h"
#include "DatabaseEngine.h"

using namespace std;

int main() {
    DataManager dm;

    cout << "=== Testing DatabaseEngine ===" << endl;

    // 1. Try loading existing text files
    loadAllDatabases(dm);

    cout << "Initial records loaded into RAM:" << endl;
    cout << "  - Customers: " << dm.customers.size() << endl;
    cout << "  - Bicycles:  " << dm.bicycles.size() << endl;
    cout << "  - Rentals:   " << dm.rentals.size() << endl;

    // 2. Add sample data if vectors are empty
    if (dm.customers.empty()) {
        Customer c1 = {"C101", "Tan Ah Kow", "010203-14-1234", "Paid", "No history"};
        dm.customers.push_back(c1);
        cout << "\nAdded test customer: " << c1.customer_name << endl;
    }

    if (dm.bicycles.empty()) {
        Bicycle b1 = {"B001", "Mountain", 15.50, "Available", "Good"};
        dm.bicycles.push_back(b1);
        cout << "Added test bicycle:  " << b1.bike_id << endl;
    }

    // 3. Save memory back out to disk
    saveAllDatabases(dm);
    cout << "\nData saved to disk successfully!" << endl;

    return 0;
}