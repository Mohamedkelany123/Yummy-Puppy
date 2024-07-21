#include <InitialLoanInterestAccrualFunc.h>


void InitialLoanInterestAccrualFunc (loan_app_loan_primitive_orm * lal_orm, int partition_number, mutex* shared_lock,void * extras){
    bool is_first_date = ((InitialLoanInterestAccrualStruct *) extras)->is_first_date;
    BlnkTemplateManager* blnkTemplateManager = ((InitialLoanInterestAccrualStruct *) extras)->blnkTemplateManager;
    BlnkTemplateManager* localTemplateManager = new BlnkTemplateManager(blnkTemplateManager, partition_number);
    InitialLoanInterestAccrual initialLoanInterestAccrual(lal_orm, is_first_date);

    if (is_first_date && (lal_orm->get_status_id() == 8 || lal_orm->get_status_id() == 16) && ((initialLoanInterestAccrual.get_last_installment_principal_paid_at())() < (BDate(lal_orm->get_first_accrual_adjustment_date())())))
    {
        cout << "Didn't Create Entry For Loan-> " << lal_orm->get_id() << endl;  
    }else{
        LedgerClosureService* ledgerClosureService = new LedgerClosureService(&initialLoanInterestAccrual);

        initialLoanInterestAccrual.setupLedgerClosureService(ledgerClosureService);
        map<string, LedgerAmount*>* ledgerAmounts = ledgerClosureService->inference();
        
        ledger_entry_primitive_orm* entry = nullptr;
        if (ledgerAmounts != nullptr)
        {   
            localTemplateManager->setEntryData(ledgerAmounts);
            if(is_first_date)
                entry = localTemplateManager->buildEntry(lal_orm->get_first_accrual_adjustment_date());
            else  
                entry = localTemplateManager->buildEntry(lal_orm->get_second_accrual_adjustment_date());
            }

            if (entry) {
                initialLoanInterestAccrual.stampORMs(entry);
            } else {
                cerr << "Cannot stamp ORM objects\n";
                exit(1);
            }
        delete localTemplateManager;
        delete ledgerClosureService;
        }
}
