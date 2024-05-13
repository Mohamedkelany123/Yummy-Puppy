#include <common.h>
#include <TemplateLeg.h>
#include <LedgerCompositLeg.h>
#include <lms_entrytemplate_primitive_orm.h>

class BlnkTemplateManager {
    private:
        json template_json;
        map<string, LedgerAmount> entry_data;
        map <string , TemplateLeg> template_legs;
        map <string,LedgerCompositLeg> ledger_amounts;
        vector <PSQLAbstractORM *> entry_orms;
        bool buildLegs();
        void loadTemplate (int template_id);
        bool validate ();
    public:
        BlnkTemplateManager(int template_id, map <string, LedgerAmount> _entry_json);
        TemplateLeg getTemplateLegByName(string name);
        bool buildEntry (int template_id);
        ~BlnkTemplateManager();

};



/*

BlnkTemplateManager * blnkTemplateManager = new BlnkTemplateManager(4);

blnkTemplateManager->buildEntry(nlohmann::json({'Booking new loan - short term; and': [{'amount': Decimal('2000.00'), 'loan_id': 133416, 'customer_id': 359786, 'cashier_id': 126935, 'merchant_id': 2}], 'Booking new loan - long term, if applicable': [{'amount': 0, 'loan_id': 133416, 'customer_id': 359786, 'cashier_id': 126935, 'merchant_id': 2}], 'Booking the transaction upfront fee': [{'amount': 0.0, 'loan_id': 133416, 'customer_id': 359786, 'cashier_id': 126935, 'merchant_id': 2}], "Booking the merchant's commission expense": [{'amount': Decimal('0.00'), 'loan_id': 133416, 'customer_id': 359786, 'cashier_id': 126935, 'merchant_id': 2}], "Booking the cashier's commission expense": [{'amount': Decimal('0.00'), 'loan_id': 133416, 'customer_id': 359786, 'cashier_id': 126935, 'merchant_id': 2}], 'Booking an impairment provision': [{'amount': Decimal('20.00'), 'loan_id': 133416, 'customer_id': 359786, 'cashier_id': 126935, 'merchant_id': 2}], 'Booking the merchant's commission income': [{'amount': Decimal('280.00'), 'loan_id': 133416, 'customer_id': 359786, 'cashier_id': 126935, 'merchant_id': 2}]}));
    
delete (blnkTemplateManager);


*/