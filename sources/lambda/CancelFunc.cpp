#include <CancelFunc.h>

void CancelLoanFunc (vector<map <string,PSQLAbstractORM *> * > * orms_list, int partition_number, mutex* shared_lock,void * extras) {
    loan_app_loan_primitive_orm * _lal_orm  = ORML(loan_app_loan,orms_list,0);
    
    PSQLGeneric_primitive_orm * gorm = ORML(PSQLGeneric,orms_list,0);
    string is_included = gorm->get("is_included");
    string cancellation_day = gorm->get("cancellation_day");


    if (is_included=="t"){
        BlnkTemplateManager* localTemplateManager = new BlnkTemplateManager(((CancelLoanStruct *) extras)->blnkTemplateManager_reverse,partition_number);

        vector <ledger_amount_primitive_orm*>* la_orms = new vector<ledger_amount_primitive_orm*>;
        for ( int i = 0 ;i < ORML_SIZE(orms_list) ; i ++)
        {
            la_orms->push_back(ORML(ledger_amount,orms_list,i));
        }
        ledger_entry_primitive_orm* entry = localTemplateManager->reverseEntry(la_orms,BDate(cancellation_day));
        if (_lal_orm->get_loan_upfront_fee()>0 && !_lal_orm->get_refund_upfront_fee_bool()){
            BlnkTemplateManager* localTemplateManager_cancel = new BlnkTemplateManager(((CancelLoanStruct *) extras)->blnkTemplateManager_cancel,partition_number);
            CancelLoan cancelLoan(_lal_orm);

            LedgerClosureService* ledgerClosureService = new LedgerClosureService(&cancelLoan);
            cancelLoan.setupLedgerClosureService(ledgerClosureService);
            map<string, LedgerAmount*>* ledgerAmounts = ledgerClosureService->inference();
            if (ledgerAmounts != nullptr)
            {   
                localTemplateManager_cancel->setEntryData(ledgerAmounts);
                entry = localTemplateManager_cancel->buildEntry(BDate(cancellation_day));
                delete localTemplateManager_cancel;
                delete ledgerClosureService;
            }else cout << "Nothing created for cancelled loans" << endl;
        }
        if(entry != nullptr){
            _lal_orm->setUpdateRefernce("cancel_ledger_entry_id", entry);
        }else cout << "Nothing created for reverse loans" << endl;
        delete localTemplateManager;
    }
};

