#ifndef TEMPLATE_MANAGER_H
#define TEMPLATE_MANAGER_H

#include <common.h>
#include <common_orm.h>

#include <LedgerCompositLeg.h>



class BlnkTemplateManager {
    private:
        int template_id;
        bool parent_flag;
        json template_json;
        map <string , TemplateLeg> *template_legs;
        map <string,LedgerCompositLeg *> ledger_amounts;
        map <string,LedgerAmount*> * entry_data;
        ledger_entry_primitive_orm * entry;
        int cache_partition_number;
        void constructTemplateLegs(); 
        void loadTemplate ();
        bool buildLegs();
        bool validate ();
    public:
        BlnkTemplateManager(int _template_id,int _cache_partition_number);
        BlnkTemplateManager(BlnkTemplateManager * _blnkTemplateManager,int _cache_partition_number);
        map <string , TemplateLeg>* getTemplateLegs(); //return this->templatelegs

        ledger_entry_primitive_orm* buildEntry (BDate _entry_date);
        ledger_entry_primitive_orm* reverseEntry (vector <ledger_amount_primitive_orm*> * _ledger_amounts, BDate _entry_date);

        void setEntryData(map <string,LedgerAmount*> * _entry_data);
        void createEntry(BDate _entry_date);

        map <string,LedgerCompositLeg*> * get_ledger_amounts();
        ledger_amount_primitive_orm * getFirstLedgerAmountORM ();
        ~BlnkTemplateManager();

};


#endif