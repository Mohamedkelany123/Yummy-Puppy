#include <common.h>
#include <ledger_amount_primitive_orm.h>
#include <ledger_entry_primitive_orm.h>
class LedgerLeg
{
    private:
        int account_id;
        int customer_id;
        int loan_id;
        int installment_id;
        // and more to be filled in later
        float amount;
        bool debit;
    public:
        LedgerLeg(){}
        LedgerLeg(bool _debit){}
        void setAmount (float _amount);
        ledger_amount_primitive_orm * get_ledger_amount_primitive_orm();
        ~LedgerLeg(){} 
};

class LedgerCompositLeg
{

    private:
        int leg_id;
        std::pair <LedgerLeg,LedgerLeg> leg;
    public:
        LedgerCompositLeg(){}
        void setAmount (float _amount);
        std::pair <ledger_amount_primitive_orm *,ledger_amount_primitive_orm *> getLedgerCompositeLeg ();
        ~LedgerCompositLeg(){}

};

class BlnkTemplateManager {
    private:
        map <string,LedgerCompositLeg> legs;
        vector <PSQLAbstractORM *> entry_orms;
        void buildLegs();
        void loadTemplate (int template_id);
        bool validate ();
    public:
        BlnkTemplateManager(int template_id);
        bool buildEntry (json temp_amount_json);
        ~BlnkTemplateManager();

};



/*

BlnkTemplateManager * blnkTemplateManager = new BlnkTemplateManager(4);

blnkTemplateManager->buildEntry(nlohmann::json({'Booking new loan - short term; and': [{'amount': Decimal('2000.00'), 'loan_id': 133416, 'customer_id': 359786, 'cashier_id': 126935, 'merchant_id': 2}], 'Booking new loan - long term, if applicable': [{'amount': 0, 'loan_id': 133416, 'customer_id': 359786, 'cashier_id': 126935, 'merchant_id': 2}], 'Booking the transaction upfront fee': [{'amount': 0.0, 'loan_id': 133416, 'customer_id': 359786, 'cashier_id': 126935, 'merchant_id': 2}], "Booking the merchant's commission expense": [{'amount': Decimal('0.00'), 'loan_id': 133416, 'customer_id': 359786, 'cashier_id': 126935, 'merchant_id': 2}], "Booking the cashier's commission expense": [{'amount': Decimal('0.00'), 'loan_id': 133416, 'customer_id': 359786, 'cashier_id': 126935, 'merchant_id': 2}], 'Booking an impairment provision': [{'amount': Decimal('20.00'), 'loan_id': 133416, 'customer_id': 359786, 'cashier_id': 126935, 'merchant_id': 2}], 'Booking the merchant's commission income': [{'amount': Decimal('280.00'), 'loan_id': 133416, 'customer_id': 359786, 'cashier_id': 126935, 'merchant_id': 2}]}));
    
delete (blnkTemplateManager);


*/