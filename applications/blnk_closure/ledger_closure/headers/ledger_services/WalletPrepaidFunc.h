#ifndef WALLETPREPAIDFUNC_H
#define WALLETPREPAIDFUNC_H

#include <functional>
#include <common.h>
#include <TemplateManager.h>
#include <WalletPrepaid.h>
#include <common_orm.h>

typedef struct {
    BlnkTemplateManager *blnkTemplateManager;
    BDate closing_day;
} WalletPrepaidStruct;

void WalletPrepaidFunc (new_lms_customerwallettransaction_primitive_orm* transaction_orm, int partition_number, mutex* shared_lock,void * extras);

#endif 
