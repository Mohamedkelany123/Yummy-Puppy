#include <OnboardingCommissionFunc.h>



void OnboardingCommissionFunc (map<string, PSQLAbstractORM*>* orms, int partition_number, mutex* shared_lock,void * extras){
    BlnkTemplateManager* localTemplateManager = new BlnkTemplateManager(((OnboardingCommissionStruct *) extras)->blnkTemplateManager, partition_number);
    
    OnboardingCommission onboardingCommission(orms);
    ekyc_app_onboardingsession_primitive_orm * ekyco_orm = onboardingCommission.get_ekyc_app_onboardingsession();
    

    LedgerClosureService* ledgerClosureService = new LedgerClosureService(&onboardingCommission);    
    onboardingCommission.setupLedgerClosureService(ledgerClosureService);
    map<string, LedgerAmount*>* ledgerAmounts = ledgerClosureService->inference();

    if (ledgerAmounts != nullptr)
    {    
        localTemplateManager->setEntryData(ledgerAmounts);
        
        ledger_entry_primitive_orm* entry = localTemplateManager->buildEntry(BDate(ekyco_orm->get_onboarding_commission_ledger_entry_date()));
        map <string,LedgerCompositLeg*> * leg_amounts = localTemplateManager->get_ledger_amounts();

        if (leg_amounts) {
            onboardingCommission.stampORMs(entry);
        } else {
            cout << "Failed To Stamp Onboarding session\n";
        }
    }
    delete localTemplateManager;
    delete ledgerClosureService;

    


}
