#ifndef NIUBIKE_PAYMENTMODULE_H
#define NIUBIKE_PAYMENTMODULE_H
#include <string>
using namespace std;

int showPaymentGateway(double grandTotal);
void showPaymentSuccess(const std::string &transactionId, double amountPaid);

#endif //NIUBIKE_PAYMENTMODULE_H
