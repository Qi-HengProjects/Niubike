#ifndef PENALTYMODULE_H
#define PENALTYMODULE_H
#include <string>
#include <ctime>

using namespace std;

// Result of a late-return penalty calculation for a single bike/rental.
struct LateFeeResult {
    bool isLate = false;
    double lateHours = 0.0;      // rounded up to the next whole hour
    double feeAmount = 0.0;
    string tierLabel = "On Time";
};

// Compares real elapsed time since checkout against the planned rental
// duration and computes an escalating (tiered) late fee if the bike is
// overdue. `checkoutTime` of 0 means "not tracked" (legacy rental) and
// always returns a non-late result rather than guessing a penalty.
LateFeeResult calculateLateFee(double plannedHours, time_t checkoutTime, double hourlyRate);

#endif // PENALTYMODULE_H
