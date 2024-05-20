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
        void constructTemplateLegs(); 
        void loadTemplate ();
        bool buildLegs();
        bool validate ();
    public:
        BlnkTemplateManager(int _template_id);
        BlnkTemplateManager(BlnkTemplateManager * _blnkTemplateManager=NULL);
        map <string , TemplateLeg>* getTemplateLegs(); //return this->templatelegs

        ledger_entry_primitive_orm* buildEntry (BDate _entry_date);
        void setEntryData(map <string,LedgerAmount*> * _entry_data);
        void createEntry(BDate _entry_date);

        map <string,LedgerCompositLeg*> * get_ledger_amounts();
        ledger_amount_primitive_orm * getFirstLedgerAmountORM ();
        ~BlnkTemplateManager();

};



/*

BlnkTemplateManager * blnkTemplateManager = new BlnkTemplateManager(4);

blnkTemplateManager->buildEntry(nlohmann::json({'Booking new loan - short term; and': [{'amount': Decimal('2000.00'), 'loan_id': 133416, 'customer_id': 359786, 'cashier_id': 126935, 'merchant_id': 2}], 'Booking new loan - long term, if applicable': [{'amount': 0, 'loan_id': 133416, 'customer_id': 359786, 'cashier_id': 126935, 'merchant_id': 2}], 'Booking the transaction upfront fee': [{'amount': 0.0, 'loan_id': 133416, 'customer_id': 359786, 'cashier_id': 126935, 'merchant_id': 2}], "Booking the merchant's commission expense": [{'amount': Decimal('0.00'), 'loan_id': 133416, 'customer_id': 359786, 'cashier_id': 126935, 'merchant_id': 2}], "Booking the cashier's commission expense": [{'amount': Decimal('0.00'), 'loan_id': 133416, 'customer_id': 359786, 'cashier_id': 126935, 'merchant_id': 2}], 'Booking an impairment provision': [{'amount': Decimal('20.00'), 'loan_id': 133416, 'customer_id': 359786, 'cashier_id': 126935, 'merchant_id': 2}], 'Booking the merchant's commission income': [{'amount': Decimal('280.00'), 'loan_id': 133416, 'customer_id': 359786, 'cashier_id': 126935, 'merchant_id': 2}]}));
    
delete (blnkTemplateManager);


*/

#endif