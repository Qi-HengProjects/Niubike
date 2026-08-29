#ifndef NIUBIKE_PAYMENTMODULE_H
#define NIUBIKE_PAYMENTMODULE_H
#include <string>
#include <vector>
#include "MainMenu.h"
#include "DatabaseEngine.h"
#include "Helpers.h"
#include "History.h"
#include "RentalModule.h"
#include "PaymentModule.h"
using namespace std;


enum class PaymentOutcome {
    SUCCESS,
    FAILED,
    CANCELLED
};

int showPaymentGateway(double grandTotal);
void showPaymentSuccess(const std::string &transactionId, double amountPaid, double change = 0.0);
void paymentLogic(DataManager&dm,int methodChoice, double amountDue, const Customer &currentCustomer, const std::string &rentalId);
Rental* selectPendingRental(DataManager &dm, std::vector<Rental*> &pending);
void processPayment(DataManager &dm, const Customer &currentCustomer);
#endif //NIUBIKE_PAYMENTMODULE_H