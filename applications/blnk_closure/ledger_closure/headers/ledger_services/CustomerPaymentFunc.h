#ifndef _CUSTOMER_PAYMENT_FUNC_H_
#define _CUSTOMER_PAYMENT_FUNC_H_

#include <common.h>
#include <common_orm.h>
#include <functional>
#include <TemplateManager.h>
#include <CustomerPayment.h>



typedef struct {
    map<int, BlnkTemplateManager*>* blnkTemplateManagerMap;
    BDate closing_date;
} ReceiveCustomerPaymentStruct;

void receiveCustomerPaymentFunc(map<string, PSQLAbstractORM*>* orms, int partition_number, mutex* shared_lock,void * extras);

#endif