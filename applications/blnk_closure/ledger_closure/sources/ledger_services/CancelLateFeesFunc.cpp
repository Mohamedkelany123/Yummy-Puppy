#include <CancelLateFeesFunc.h>

void CancelLateFeesFunc (vector<map <string,PSQLAbstractORM *> * > * orms_list, int partition_number, mutex* shared_lock,void * extras) {
        CancelLateFees cancelLateFees(orms_list);
        LedgerClosureService* ledgerClosureService = new LedgerClosureService(&cancelLateFees);
        cancelLateFees.setupLedgerClosureService(ledgerClosureService);
        map <string,pair< vector<new_lms_installmentlatefees_primitive_orm *> *, double> *> * latefeesmap = cancelLateFees.get_latefees_partition();
        if (latefeesmap != nullptr){
        for (auto itr = latefeesmap->cbegin(); itr != latefeesmap->cend(); ++itr) 
        {
            if (latefeesmap != nullptr)
                cout<<"the latefeesmap is not null\n"<<endl;
            else
                cout<<"the latefeesmap is null\n"<<endl;

            BlnkTemplateManager* localTemplateManager = new BlnkTemplateManager(((CancelLateFeesStruct *) extras)->blnkTemplateManager,partition_number);
            cout<<"the amount is \n"<< itr->first<<endl;
            cancelLateFees.set_amount(itr->second->second);
            cancelLateFees.set_late_fees(itr->second->first);
            map<string, LedgerAmount*>* ledgerAmounts = ledgerClosureService->inference();
            localTemplateManager->setEntryData(ledgerAmounts);
            localTemplateManager->buildEntry(BDate(itr->first));
            ledger_amount_primitive_orm* la_orm = localTemplateManager->getFirstLedgerAmountORM();
            cancelLateFees.stampORMs(la_orm);
  
            delete localTemplateManager;
            delete ledgerClosureService;
        }
    } 

};
