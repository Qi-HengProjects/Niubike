#ifndef ANALYTICSMODULE_H
#define ANALYTICSMODULE_H
#include "DatabaseEngine.h"

// Reloads bicycles/rentals from disk and prints a fleet-wide utilization,
// revenue, and bike-popularity report for admins.
void showFleetAnalytics(DataManager &dm);

#endif // ANALYTICSMODULE_H
