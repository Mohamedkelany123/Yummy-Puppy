#include <SettlementByCustomerFunc.h>


void settlementByCustomerFunc(vector<map <string,PSQLAbstractORM *> * > * orms_list, int partition_number, mutex* shared_lock,void * extras) {
    payments_loanorder_primitive_orm * pl = ORML(payments_loanorder, orms_list, 0);

    cout << "LOANORDERID=" << pl->get_id() << endl;


    // BlnkTemplateManager* localTemplateManager = new BlnkTemplateManager(((SettlementByCustomerStruct *) extras)->blnkTemplateManager, partition_number);
    // BDate closing_date = ((SettlementByCustomerStruct*)extras)->closing_day;
  
    // int template_id = 14;
    // SettlementByCustomer settlementByCustomer(orms_list, closing_date);

    // LedgerClosureService* ledgerClosureService = new LedgerClosureService(&settlementByCustomer);
    // settlementByCustomer.setupLedgerClosureService(ledgerClosureService);
    // map<string, LedgerAmount*>* ledgerAmounts = ledgerClosureService->inference();

    // if (ledgerAmounts != nullptr) {
    //     // localTemplateManager->setEntryData(ledgerAmounts);
    //     // ledger_entry_primitive_orm* entry = localTemplateManager->buildEntry(order_paid_at);
    //     // if (entry) {
    //     //     customerPayment.stampORMS(entry);
    //     // }
    //     // else {
    //     //     lal_orm->set_closure_status(-1*ledger_status::REPAYMENT_BY_CUSTOMER);
    //     // }
    // }
    
}
