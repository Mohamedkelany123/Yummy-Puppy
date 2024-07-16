#include <DueForSettlementWithMerchantFunc.h>


void dueForSettlementWithMerchantFunc(loan_app_loan_primitive_orm * _lal_orm, int partition_number, mutex* shared_lock,void * extras){
    cout << "LoanID:" << _lal_orm->get_id() << endl;
    BlnkTemplateManager * localBlnkTemplateManager = ((DueForSettlementStruct*)extras)->blnkTemplateManager;
    
    DueForSettlement dueForSettlementWithMerchant(_lal_orm);
    // LedgerClosureService* ledgerClosureService = new LedgerClosureService(&dueForSettlementWithMerchant);


}
