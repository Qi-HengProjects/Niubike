#include "PenaltyModule.h"
#include <cmath>

// Short window after the planned duration ends before any penalty kicks in
// (covers normal walking-to-the-counter type delays).
static const double GRACE_PERIOD_HOURS = 0.10; // ~6 minutes

// Tier boundaries, measured in whole hours late.
static const double TIER1_MAX_HOURS = 2.0;
static const double TIER2_MAX_HOURS = 5.0;

// Per-hour surcharge rate multiplier applied on top of the bike's own hourly
// rate for each tier, plus a flat penalty once a rental reaches Tier 3.
static const double TIER1_RATE_MULTIPLIER = 0.5;
static const double TIER2_RATE_MULTIPLIER = 1.0;
static const double TIER3_RATE_MULTIPLIER = 2.0;
static const double TIER3_FLAT_SURCHARGE = 10.00;

LateFeeResult calculateLateFee(double plannedHours, time_t checkoutTime, double hourlyRate) {
    LateFeeResult result;

    // Legacy rentals created before this feature existed have no recorded
    // checkout timestamp -- skip the penalty rather than fabricate one.
    if (checkoutTime <= 0) {
        return result;
    }

    time_t now = time(nullptr);
    double elapsedHours = difftime(now, checkoutTime) / 3600.0;
    double overHours = elapsedHours - plannedHours;

    if (overHours <= GRACE_PERIOD_HOURS) {
        return result; // returned on time, or within the grace window
    }

    double lateHours = ceil(overHours);
    if (lateHours < 1.0) lateHours = 1.0;

    double fee;
    string tier;

    if (lateHours <= TIER1_MAX_HOURS) {
        tier = "Tier 1 - Minor Delay";
        fee = lateHours * hourlyRate * TIER1_RATE_MULTIPLIER;
    } else if (lateHours <= TIER2_MAX_HOURS) {
        tier = "Tier 2 - Moderate Delay";
        fee = (TIER1_MAX_HOURS * hourlyRate * TIER1_RATE_MULTIPLIER)
            + ((lateHours - TIER1_MAX_HOURS) * hourlyRate * TIER2_RATE_MULTIPLIER);
    } else {
        tier = "Tier 3 - Severe Delay";
        fee = (TIER1_MAX_HOURS * hourlyRate * TIER1_RATE_MULTIPLIER)
            + ((TIER2_MAX_HOURS - TIER1_MAX_HOURS) * hourlyRate * TIER2_RATE_MULTIPLIER)
            + ((lateHours - TIER2_MAX_HOURS) * hourlyRate * TIER3_RATE_MULTIPLIER)
            + TIER3_FLAT_SURCHARGE;
    }

    result.isLate = true;
    result.lateHours = lateHours;
    result.feeAmount = fee;
    result.tierLabel = tier;
    return result;
}
