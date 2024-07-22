#include <UnmarginalizeFunc.h>

void UnmarginalizeFunc (vector<map <string,PSQLAbstractORM *> * > * orms_list, int partition_number, mutex* shared_lock,void * extras) {
    loan_app_loan_primitive_orm * lal_orm  = ORML(loan_app_loan,orms_list,0);

    //map<paid_at, map<installment_id,pair<pair<ins_ext, unmarginalization_amount>, vector<latefees>>>>
    map <string,map<int,pair<pair<new_lms_installmentextension_primitive_orm*,float>*,vector<new_lms_installmentlatefees_primitive_orm*> *> *> *> * date_map = Unmarginalize::get_date_map(orms_list);
    for (auto itr = date_map->begin(); itr != date_map->end(); ++itr) 
    {
        //PER DATE LOOP (ENTRY)
        map<string,LedgerAmount*>* entryLedgerAmounts = new map<string,LedgerAmount*>();
        BlnkTemplateManager* localTemplateManager = new BlnkTemplateManager(((UnmarginalizeStruct *) extras)->blnkTemplateManager,partition_number);
        localTemplateManager->createEntry(BDate(itr->first));

        for (auto itr1 = itr->second->begin(); itr1 != itr->second->end(); ++itr1) 
        {
            BlnkTemplateManager* loopTemplateManager = new BlnkTemplateManager(localTemplateManager, partition_number);
            loopTemplateManager->setEntry(localTemplateManager->get_entry());
            Unmarginalize unmarginalize = Unmarginalize(lal_orm,itr1->first,itr1->second->first->first,itr1->second->first->second,itr1->second->second);
            LedgerClosureService* ledgerClosureService = new LedgerClosureService(&unmarginalize);
            unmarginalize.setupLedgerClosureService(ledgerClosureService);
            map<string, LedgerAmount*>* ledgerAmounts = ledgerClosureService->inference();
            if(ledgerAmounts != nullptr){
               loopTemplateManager->setEntryData(ledgerAmounts);
               ledger_entry_primitive_orm * entry = loopTemplateManager->buildEntry(BDate(itr->first));
               vector<ledger_amount_primitive_orm*> * amounts =  entry->get_ledger_amount_entry_id();
               if(entry){
                    unmarginalize.stampORMs(loopTemplateManager->get_ledger_amounts());
                }
            }
            delete(ledgerClosureService);
            delete(loopTemplateManager); 
        }
        delete(entryLedgerAmounts);
        delete(localTemplateManager);
    }
    delete(date_map);
};
