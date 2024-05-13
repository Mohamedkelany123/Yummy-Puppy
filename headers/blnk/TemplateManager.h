#include <common.h>
#include <ledger_amount_primitive_orm.h>
#include <ledger_entry_primitive_orm.h>

class x
{
    protected:
        int customer_id;
        int loan_id;
        int installment_id;
        int merchant_id;
        int bond_id;
        int latefee_id;


}

class LedgerAmount: public x
{
    private:
        int id;
        string name;
        int debit_account_id;
        int cashier_id;
        int credit_account_id;
        int customer_id;
        int loan_id;
        int installment_id;
        int merchant_id;
        int bond_id;
        int latefee_id;
        int leg_id;
        int entry_id;
        float amount;
        int account_id;
        bool is_credit;
    public:
        LedgerAmount();
        // Setters
        void setId(int id);
        void setName(string name);
        void setDebitAccountId(int debit_account_id);
        void setCashierId(int cashier_id);
        void setCreditAccountId(int credit_account_id);
        void setCustomerId(int customer_id);
        void setLoanId(int loan_id);
        void setInstallmentId(int installment_id);
        void setMerchantId(int merchant_id);
        void setBondId(int bond_id);
        void setLatefeeId(int latefee_id);
        void setLegId(int leg_id);
        void setEntryId(int entry_id);
        void setAmount(float amount);
        void setAccountId(float account_id);
        void setIsCredit(bool is_credit);



        // Getters
        int getId();
        string getName();
        int getDebitAccountId();
        int getCashierId();
        int getCreditAccountId();
        int getCustomerId();
        int getLoanId();
        int getInstallmentId();
        int getMerchantId();
        int getBondId();
        int getLatefeeId();
        int getLegId();
        int getEntryId();
        float getAmount();
        int getAccountId();
        bool getIsCredit();
        ~LedgerAmount();
};

class TemplateLeg
{
    private:
        int id;
        string name;
        int debit_account_id;
        bool cashier_id_required;
        int credit_account_id;
        bool customer_id_required;
        bool loan_id_required;
        bool installment_id_required;
        bool merchant_id_required;
        bool bond_id_required;
        bool latefee_id_required;
        bool credit_bond_id_required;
        bool debit_bond_id_required;
        bool leg_required;
        vector<int> credit_available_ids;
        vector<int> debit_available_ids;
        // string variable_name;
    public:
        TemplateLeg();
        TemplateLeg(bool _debit);

        //SETTERS
        void setCashierIdRequired(bool cashier_id_required);
        void setCreditAvailableIds(const vector<int>& credit_available_ids);
        void setCreditBondIdRequired(int credit_bond_id_required);
        void setCustomerIdRequired(bool customer_id_required);
        void setDebitAvailableIds(const vector<int>& debit_available_ids);
        void setDebitBondIdRequired(int debit_bond_id_required);
        void setId(int id);
        void setInstallmentIdRequired(bool installment_id_required);
        void setLoanIdRequired(bool loan_id_required);
        void setMerchantIdRequired(bool merchant_id_required);
        void setName(const string& name);
        void setLegRequired(bool leg_required);
        // void setVariableName(const string& value) { variable_name = value; }

        //GETTERS
        const bool getCashierIdRequired();
        const vector<int>& getCreditAvailableIds();
        const int getCreditBondIdRequired();
        const bool getCustomerIdRequired();
        const vector<int>& getDebitAvailableIds();
        const int getDebitBondIdRequired();
        const int getId();
        const bool getInstallmentIdRequired();
        const bool getLoanIdRequired();
        const bool getMerchantIdRequired();
        const string& getName();
        const bool getLegRequired();
        // const string& getVariableName() const { return variable_name; }        
        ledger_amount_primitive_orm * get_ledger_amount_primitive_orm();
        ~TemplateLeg();
};

class LedgerCompositLeg
{
    private:
        int leg_id;
        std::pair <LedgerAmount,LedgerAmount> leg;
    public:
        LedgerCompositLeg(){}
        void setAmount (float _amount);
        std::pair <ledger_amount_primitive_orm *,ledger_amount_primitive_orm *> getLedgerCompositeLeg ();

        void build (TemplateLeg * template_leg, json leg_json)
        {
                LedgerAmount debit;
                LedgerAmount credit;

                // some code to build credit debit

                leg.first = debit;
                leg.second = credit;

        }
        ~LedgerCompositLeg(){}

};

class BlnkTemplateManager {
    private:
        json template_json;
        json entry_json;
        vector <TemplateLeg> template_legs;
        map <int,LedgerCompositLeg> ledger_amounts;
        vector <PSQLAbstractORM *> entry_orms;
        void buildLegs();
        void loadTemplate (int template_id);
        bool validate ();
    public:
        BlnkTemplateManager(int template_id, json entry_json);
        TemplateLeg getTemplateLegByName(string name);
        bool buildEntry (json temp_amount_json);
        ~BlnkTemplateManager();

};



/*

BlnkTemplateManager * blnkTemplateManager = new BlnkTemplateManager(4);

blnkTemplateManager->buildEntry(nlohmann::json({'Booking new loan - short term; and': [{'amount': Decimal('2000.00'), 'loan_id': 133416, 'customer_id': 359786, 'cashier_id': 126935, 'merchant_id': 2}], 'Booking new loan - long term, if applicable': [{'amount': 0, 'loan_id': 133416, 'customer_id': 359786, 'cashier_id': 126935, 'merchant_id': 2}], 'Booking the transaction upfront fee': [{'amount': 0.0, 'loan_id': 133416, 'customer_id': 359786, 'cashier_id': 126935, 'merchant_id': 2}], "Booking the merchant's commission expense": [{'amount': Decimal('0.00'), 'loan_id': 133416, 'customer_id': 359786, 'cashier_id': 126935, 'merchant_id': 2}], "Booking the cashier's commission expense": [{'amount': Decimal('0.00'), 'loan_id': 133416, 'customer_id': 359786, 'cashier_id': 126935, 'merchant_id': 2}], 'Booking an impairment provision': [{'amount': Decimal('20.00'), 'loan_id': 133416, 'customer_id': 359786, 'cashier_id': 126935, 'merchant_id': 2}], 'Booking the merchant's commission income': [{'amount': Decimal('280.00'), 'loan_id': 133416, 'customer_id': 359786, 'cashier_id': 126935, 'merchant_id': 2}]}));
    
delete (blnkTemplateManager);


*/