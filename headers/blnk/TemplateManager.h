#include <common.h>
#include <ledger_amount_primitive_orm.h>
#include <ledger_entry_primitive_orm.h>

class LedgerAmount
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
        LedgerAmount(){}
        // Setters
        void setId(int id) {this->id = id; }
        void setName(string name) { this->name = name; }
        void setDebitAccountId(int debit_account_id) { this->debit_account_id = debit_account_id; }
        void setCashierId(int cashier_id) { this->cashier_id = cashier_id; }
        void setCreditAccountId(int credit_account_id) { this->credit_account_id = credit_account_id; }
        void setCustomerId(int customer_id) { this->customer_id = customer_id; }
        void setLoanId(int loan_id) { this->loan_id = loan_id; }
        void setInstallmentId(int installment_id) { this->installment_id = installment_id; }
        void setMerchantId(int merchant_id) { this->merchant_id = merchant_id; }
        void setBondId(int bond_id) {this->bond_id = bond_id; }
        void setLatefeeId(int latefee_id) { this->latefee_id = latefee_id; }
        void setLegId(int leg_id) { this->leg_id = leg_id; }
        void setEntryId(int entry_id) { this->entry_id = entry_id; }
        void setAmount(float amount) { this->amount = amount; }
        void setAccountId(float account_id) { this->account_id = account_id; }
        void setIsCredit(bool is_credit) {this->is_credit = is_credit; }



        // Getters
        int getId() { return id; }
        string getName() { return name; }
        int getDebitAccountId() { return debit_account_id; }
        int getCashierId() { return cashier_id; }
        int getCreditAccountId() { return credit_account_id; }
        int getCustomerId() { return customer_id; }
        int getLoanId() { return loan_id; }
        int getInstallmentId() { return installment_id; }
        int getMerchantId() { return merchant_id; }
        int getBondId() { return bond_id; }
        int getLatefeeId() { return latefee_id; }
        int getLegId() { return leg_id; }
        int getEntryId() { return entry_id; }
        float getAmount() { return amount; }
        int getAccountId() { return account_id; }
        bool getIsCredit() { return is_credit; }
        ~LedgerAmount(){}
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
        TemplateLeg(){}
        TemplateLeg(bool _debit){}

        //SETTERS
        void setCashierIdRequired(bool cashier_id_required) { this->cashier_id_required = cashier_id_required; }
        void setCreditAvailableIds(const vector<int>& credit_available_ids) { this->credit_available_ids = credit_available_ids; }
        void setCreditBondIdRequired(int credit_bond_id_required) { this->credit_bond_id_required = credit_bond_id_required; }
        void setCustomerIdRequired(bool customer_id_required) { this->customer_id_required = customer_id_required; }
        void setDebitAvailableIds(const vector<int>& debit_available_ids) { this->debit_available_ids = debit_available_ids; }
        void setDebitBondIdRequired(int debit_bond_id_required) { this->debit_bond_id_required = debit_bond_id_required; }
        void setId(int id) { this->id = id; }
        void setInstallmentIdRequired(bool installment_id_required) { this->installment_id_required = installment_id_required; }
        void setLoanIdRequired(bool loan_id_required) { this->loan_id_required = loan_id_required; }
        void setMerchantIdRequired(bool merchant_id_required) { this->merchant_id_required = merchant_id_required; }
        void setName(const string& name) { this->name = name; }
        void setLegRequired(bool leg_required) { this->leg_required = leg_required; }
        // void setVariableName(const string& value) { variable_name = value; }

        //GETTERS
        bool getCashierIdRequired() const { return cashier_id_required; }
        const vector<int>& getCreditAvailableIds() const { return credit_available_ids; }
        int getCreditBondIdRequired() const { return credit_bond_id_required; }
        bool getCustomerIdRequired() const { return customer_id_required; }
        const vector<int>& getDebitAvailableIds() const { return debit_available_ids; }
        int getDebitBondIdRequired() const { return debit_bond_id_required; }
        int getId() const { return id; }
        bool getInstallmentIdRequired() const { return installment_id_required; }
        bool getLoanIdRequired() const { return loan_id_required; }
        bool getMerchantIdRequired() const { return merchant_id_required; }
        const string& getName() const { return name; }
        bool getLegRequired() const { return leg_required; }
        // const string& getVariableName() const { return variable_name; }        
        ledger_amount_primitive_orm * get_ledger_amount_primitive_orm();
        ~TemplateLeg(){} 
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
        ~LedgerCompositLeg(){}

};

class BlnkTemplateManager {
    private:
        json template_json;
        json entry_json;
        vector <TemplateLeg> template_legs;
        map <string,LedgerCompositLeg> ledger_amounts;
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