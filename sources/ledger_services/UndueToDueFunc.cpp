#include <UndueToDueFunc.h>

void InstallmentBecomingDueFunc (map<string, PSQLAbstractORM*>* orms, int partition_number, mutex* shared_lock,void * extras) {
    cout << "LoanAppLoan: " <<  ORMBL(loan_app_loan,orms)->get_id() << endl;

    BlnkTemplateManager* localTemplateManager = new BlnkTemplateManager(((UndueToDueStruct *) extras)->blnkTemplateManager, partition_number);
    BDate closing_day = ((UndueToDueStruct *) extras)->closing_day;
    
    UndueToDue undueToDue(orms, closing_day);

    LedgerClosureService* ledgerClosureService = new LedgerClosureService(&undueToDue);    
    undueToDue.setupLedgerClosureService(ledgerClosureService);
    map<string, LedgerAmount*>* ledgerAmounts = ledgerClosureService->inference();
    if (ledgerAmounts != nullptr)
    {    
        localTemplateManager->setEntryData(ledgerAmounts);
        
        new_lms_installmentextension_primitive_orm* nli_orm = ORM(new_lms_installmentextension,orms);
        ledger_entry_primitive_orm* entry = localTemplateManager->buildEntry(nli_orm->get_undue_to_due_date());
        map <string,LedgerCompositLeg*> * leg_amounts = localTemplateManager->get_ledger_amounts();

        if (leg_amounts) {
            undueToDue.stampORMs(leg_amounts);
        } else {
            cout << "Failed To Stamp DueToOverDue\n";
        }
    }
    delete localTemplateManager;
    delete ledgerClosureService;
};


void StickyInstallmentBecomingDueFunc (map<string, PSQLAbstractORM*>* orms, int partition_number, mutex* shared_lock,void * extras) {
    cout << "-----------------------------" << endl;
    cout << "LoanAppLoan: " <<  ORMBL(loan_app_loan,orms)->get_id() << endl;
    cout << "Installment: " <<  ORM(loan_app_installment,orms)->get_id() << endl;

    BlnkTemplateManager* localTemplateManager1 = nullptr;
    BlnkTemplateManager* localTemplateManager2 = nullptr;
    BlnkTemplateManager* localTemplateManager3 = nullptr;

    BDate closing_day = ((UndueToDueStruct *) extras)->closing_day;
    

    loan_app_installment_primitive_orm * lai_orm = ORM(loan_app_installment,orms);
    new_lms_installmentextension_primitive_orm* nli_orm = ORM(new_lms_installmentextension,orms);
    PSQLGeneric_primitive_orm * gorm = ORM(PSQLGeneric,orms);

    BDate lsh_settle_charge_off_day(gorm->get("settled_charge_off_day_status"));
    BDate installment_day(lai_orm->get_day());

    //INTEREST--------------------------------------------------------------------------------------//
    UndueToDue undueToDueInterest = UndueToDue(orms, closing_day, 2);
    LedgerClosureService* interest_service = new LedgerClosureService(&undueToDueInterest);    
    undueToDueInterest.setupLedgerClosureService(interest_service);
    map<string, LedgerAmount*>* ledgerInterestAmounts = interest_service->inference();
    if (ledgerInterestAmounts != nullptr)
    {    
        if (lsh_settle_charge_off_day.getDateString() != ""){
            if ((*ledgerInterestAmounts).find("Interest income becoming due") != (*ledgerInterestAmounts).end())
            {
                if((lsh_settle_charge_off_day() <= closing_day()) && (lsh_settle_charge_off_day() < installment_day()) && (nli_orm->get_is_interest_paid()) && ((*ledgerInterestAmounts)["Interest income becoming due"]->getAmount() != 0) )
                {
                    cout << "MMM1111111111111111111111" << endl; 
                    localTemplateManager1 = new BlnkTemplateManager(((UndueToDueStruct *) extras)->blnkTemplateManager, partition_number);
                    localTemplateManager1->setEntryData(ledgerInterestAmounts);
                    ledger_entry_primitive_orm* entry = localTemplateManager1->buildEntry(lsh_settle_charge_off_day);
                    map <string,LedgerCompositLeg*> *leg_amounts = localTemplateManager1->get_ledger_amounts();
                    // undueToDueInterest.stampORMs(leg_amounts);
                    
                
                }else if(nli_orm->get_is_extra_interest_paid() == true && nli_orm->get_undue_to_due_extra_interest_ledger_amount_id() != 0 && (BDate(nli_orm->get_extra_interest_paid_at()))() < (BDate(lai_orm->get_day()))()){
                    cout << "MMM22222222222222222222222" << endl;
                    localTemplateManager2 = new BlnkTemplateManager(((UndueToDueStruct *) extras)->blnkTemplateManager, partition_number);
                    localTemplateManager2->setEntryData(ledgerInterestAmounts);
                    ledger_entry_primitive_orm* entry = localTemplateManager2->buildEntry(nli_orm->get_extra_interest_paid_at());
                    map <string,LedgerCompositLeg*> *leg_amounts = localTemplateManager2->get_ledger_amounts();
                    // undueToDueInterest.stampORMs(leg_amounts);                
                }
            }
        }
    }else{
        cout << "Interest : Nothing Created For Sticky Undue To Due For InsId:" << lai_orm->get_id() << endl;
    }

    cout << "FINISHEDDD THE CODEEE" << endl;

    //PRINCIPAL--------------------------------------------------------------------------------------//
    UndueToDue undueToDuePrincipal = UndueToDue(orms, closing_day, 3);
    LedgerClosureService* principal_service = new LedgerClosureService(&undueToDuePrincipal);    
    undueToDuePrincipal.setupLedgerClosureService(principal_service);
    map<string, LedgerAmount*>* ledgerPrincipalAmounts = principal_service->inference();
    if (ledgerPrincipalAmounts != nullptr)
    {
        // cout << "333333333333333333333333" << endl;
        localTemplateManager3 = new BlnkTemplateManager(((UndueToDueStruct *) extras)->blnkTemplateManager, partition_number);
        localTemplateManager3->setEntryData(ledgerPrincipalAmounts);
        BDate principal_entry_date(nli_orm->get_principal_paid_at());
        ledger_entry_primitive_orm* entry = localTemplateManager3->buildEntry(principal_entry_date);
        map <string,LedgerCompositLeg*> *leg_amounts = localTemplateManager3->get_ledger_amounts();
        // undueToDueInterest.stampORMs(leg_amounts);     
        }else{
        cout << "Principal : Nothing Created For Sticky Undue To Due For InsID:" << lai_orm->get_id() << endl;
    }

    //--------------------------------------------------------------------------------------//

    delete localTemplateManager1;
    delete localTemplateManager2;
    delete localTemplateManager3;
    delete principal_service;
    delete interest_service;

    cout << "-----------------------------" << endl;

};


