#include "AnalyticsModule.h"
#include "Helpers.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <map>
#include <vector>
#include <algorithm>
#include <limits>

using namespace std;

static string trimStr(const string &s) {
    size_t first = s.find_first_not_of(" \t\r\n");
    if (first == string::npos) return "";
    size_t last = s.find_last_not_of(" \t\r\n");
    return s.substr(first, (last - first + 1));
}

void showFleetAnalytics(DataManager &dm) {
    clearScreen();
    dm.bicycles.clear();
    loadBicycles(dm.bicycles);
    dm.rentals.clear();
    loadRentals(dm.rentals);

    string border     = "======================================================================";
    string lineSingle = "----------------------------------------------------------------------";

    map<string, int> typeTotal, typeRentedNow, typeTimesRented;
    map<string, double> typeRevenue;
    map<string, int> bikeRentalCount;
    map<string, string> bikeIdToType;

    for (const auto &b : dm.bicycles) {
        typeTotal[b.bikeType]++;
        if (trimStr(b.status) == "Rented") typeRentedNow[b.bikeType]++;
        bikeIdToType[b.bikeId] = b.bikeType;
        bikeRentalCount[b.bikeId] = 0; // ensure every bike appears, even with zero rentals
    }

    for (const auto &r : dm.rentals) {
        // Prefer the permanent original assignment so a bike's history
        // survives even after it has been returned and stripped out of
        // bikeIdsStr by ReturnModule's partial-return logic.
        const vector<string> &ids = !r.originalBikeIdsStr.empty() ? r.originalBikeIdsStr : r.bikeIdsStr;

        for (const auto &bikeId : ids) {
            string type = bikeIdToType.count(bikeId) ? bikeIdToType[bikeId] : "Unknown";
            typeTimesRented[type]++;
            bikeRentalCount[bikeId]++;

            if (trimStr(r.paymentStatus) == "Paid") {
                typeRevenue[type] += r.rentingPrice;
            }
        }
    }

    cout << getCenteredString(border, 165) << endl;
    cout << getCenteredString("DYNAMIC FLEET USAGE & BIKE POPULARITY ANALYTICS", 165) << endl;
    cout << getCenteredString(border, 165) << endl << endl;

    ostringstream headerSS;
    headerSS << left
             << setw(14) << "Category"
             << setw(9)  << "Fleet"
             << setw(12) << "Rented Now"
             << setw(13) << "Utilization"
             << setw(15) << "Times Rented"
             << setw(14) << "Revenue (RM)";
    cout << getCenteredString(headerSS.str(), 165) << endl;
    cout << getCenteredString(lineSingle, 165) << endl;

    string mostPopular, leastPopular;
    int maxRented = -1;
    int minRented = numeric_limits<int>::max();

    for (const auto &entry : typeTotal) {
        const string &type = entry.first;
        int total = entry.second;
        int rentedNow = typeRentedNow.count(type) ? typeRentedNow[type] : 0;
        int timesRented = typeTimesRented.count(type) ? typeTimesRented[type] : 0;
        double revenue = typeRevenue.count(type) ? typeRevenue[type] : 0.0;
        double utilization = total > 0 ? (rentedNow * 100.0 / total) : 0.0;

        ostringstream utilSS, revSS;
        utilSS << fixed << setprecision(1) << utilization << "%";
        revSS << fixed << setprecision(2) << revenue;

        ostringstream rowSS;
        rowSS << left
              << setw(14) << type
              << setw(9)  << total
              << setw(12) << rentedNow
              << setw(13) << utilSS.str()
              << setw(15) << timesRented
              << setw(14) << revSS.str();
        cout << getCenteredString(rowSS.str(), 165) << endl;

        if (timesRented > maxRented) { maxRented = timesRented; mostPopular = type; }
        if (timesRented < minRented) { minRented = timesRented; leastPopular = type; }
    }

    cout << getCenteredString(lineSingle, 165) << endl << endl;

    if (!typeTotal.empty()) {
        cout << getCenteredString("Most Popular Category  : " + mostPopular + " (" + to_string(maxRented) + " rental(s))", 165) << endl;
        cout << getCenteredString("Least Popular Category : " + leastPopular + " (" + to_string(minRented) + " rental(s))", 165) << endl << endl;
    }

    // Top individual bikes by usage
    vector<pair<string, int>> bikeRanking(bikeRentalCount.begin(), bikeRentalCount.end());
    sort(bikeRanking.begin(), bikeRanking.end(), [](const pair<string, int> &a, const pair<string, int> &b) {
        return a.second > b.second;
    });

    cout << getCenteredString("Top Individual Bikes by Usage:", 165) << endl;
    int shown = 0;
    for (const auto &entry : bikeRanking) {
        if (entry.second <= 0) continue;
        string type = bikeIdToType.count(entry.first) ? bikeIdToType[entry.first] : "Unknown";
        cout << getCenteredString("  " + entry.first + " (" + type + ") - " + to_string(entry.second) + " rental(s)", 165) << endl;
        shown++;
        if (shown >= 3) break;
    }
    if (shown == 0) {
        cout << getCenteredString("  No rental activity recorded yet.", 165) << endl;
    }

    int idleCount = 0;
    for (const auto &entry : bikeRentalCount) {
        if (entry.second == 0) idleCount++;
    }
    cout << endl << getCenteredString("Idle Bikes (never rented) : " + to_string(idleCount) + " / " + to_string(dm.bicycles.size()), 165) << endl;

    cout << endl << getCenteredString(border, 165) << endl;
    cout << getCenteredString("Press Enter to return to admin menu...", 165);
    string dummy;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, dummy);
}
