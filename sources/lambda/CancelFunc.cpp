#include <CancelFunc.h>

void CancelLoanFunc (loan_app_loan_bl_orm * _lal_orm, int partition_number, mutex* shared_lock,void * extras) {
    cout << "FIRST LEGGG AMOUNTTT" <<endl;
    string is_included = _lal_orm->getExtra("is_included");
    string cancellation_day = _lal_orm->getExtra("cancellation_day");
    cout<< "loan id is "<<_lal_orm->get_id()<<endl;

    if (is_included=="t"){

        BlnkTemplateManager* localTemplateManager = new BlnkTemplateManager(((CancelLoanStruct *) extras)->blnkTemplateManager_reverse, partition_number);
        vector <ledger_amount_primitive_orm*>* la_orms = _lal_orm->get_ledger_amount_loan_id() ;   
        ledger_entry_primitive_orm* entry = localTemplateManager->reverseEntry(la_orms,BDate(cancellation_day));

        if (_lal_orm->get_loan_upfront_fee()>0 && !_lal_orm->get_refund_upfront_fee_bool()){
            BlnkTemplateManager* localTemplateManager_cancel = new BlnkTemplateManager(((CancelLoanStruct *) extras)->blnkTemplateManager_cancel);
            CancelLoan cancelLoan(_lal_orm);

            LedgerClosureService* ledgerClosureService = new LedgerClosureService(&cancelLoan);
            cancelLoan.setupLedgerClosureService(ledgerClosureService);
            map<string, LedgerAmount*>* ledgerAmounts = ledgerClosureService->inference();
            localTemplateManager_cancel->setEntryData(ledgerAmounts);
            entry = localTemplateManager_cancel->buildEntry(BDate(cancellation_day));
            delete localTemplateManager_cancel;
            delete ledgerClosureService;


        }
        _lal_orm->setUpdateRefernce("cancel_ledger_entry_id", entry);
        delete localTemplateManager;
    }
};
