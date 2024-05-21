#include <CancelFunc.h>

void CancelLoanFunc (map<string, PSQLAbstractORM*>* orms, int partition_number, mutex* shared_lock,void * extras) {
    PSQLGeneric_primitive_orm * gorm = ORM(PSQLGeneric,orms);
    bool is_included = gorm->toBool("is_included");   
    if (is_included){

        ledger_entry_primitive_orm* entry = new ledger_entry_primitive_orm("main");
        BlnkTemplateManager* localTemplateManager = new BlnkTemplateManager(((CancelLoanStruct *) extras)->blnkTemplateManager_reverse);
        loan_app_loan_bl_orm * _lal_orm = ORMBL(loan_app_loan, orms);
        vector <ledger_amount_primitive_orm*> * la_orms = _lal_orm->get_ledger_amount_loan_id();
        entry = localTemplateManager->reverseEntry(la_orms,BDate("2024-05-15"));

        if (_lal_orm->get_loan_upfront_fee()>0 & !_lal_orm->get_refund_upfront_fee_bool()){
            BlnkTemplateManager* localTemplateManager_cancel = new BlnkTemplateManager(((CancelLoanStruct *) extras)->blnkTemplateManager_cancel);
            CancelLoan cancelLoan(_lal_orm);
            LedgerClosureService* ledgerClosureService = new LedgerClosureService(&cancelLoan);
            cancelLoan.setupLedgerClosureService(ledgerClosureService);
            map<string, LedgerAmount*>* ledgerAmounts = ledgerClosureService->inference();
            localTemplateManager_cancel->setEntryData(ledgerAmounts);
            entry = localTemplateManager_cancel->buildEntry(BDate("2024-05-15"));
            delete localTemplateManager_cancel;
            delete ledgerClosureService;


        }
        _lal_orm->setUpdateRefernce("cancel_ledger_entry_id", entry);

        delete localTemplateManager;
    }
};
