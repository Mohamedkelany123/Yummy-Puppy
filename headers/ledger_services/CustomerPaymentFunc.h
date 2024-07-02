#ifndef _CUSTOMER_PAYMENT_FUNC_H_
#define _CUSTOMER_PAYMENT_FUNC_H_

#include <common.h>
#include <functional>
#include <TemplateManager.h>
#include <common_orm.h>
#include <CustomerPayment.h>

typedef struct {
    BlnkTemplateManager *blnkTemplateManager;
} ReceiveCustomerPaymentStruct;

void receiveCustomerPaymentFunc(vector<map <string,PSQLAbstractORM *> * > * orms_list, int partition_number, mutex* shared_lock,void * extras);

#endif