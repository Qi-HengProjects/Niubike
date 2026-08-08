#ifndef HISTORY_H
#define HISTORY_H

#include "Structures.h"
#include <string>

class History {
public:
    History() = default;

    // Main method to display rental history for a given customer ID
    void displayUserHistory(const DataManager &dm, const std::string &currentCustId) const;

private:
    // Helper method to clean string whitespace and carriage returns (\r)
    std::string trimStr(const std::string &s) const;
};

#endif // HISTORY_H