#ifndef TEMPLATE_LEG_H
#define TEMPLATE_LEG_H

#include <common.h>
#include <ledger_amount_primitive_orm.h>


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
        void setDebitAccountId(int _debit_account_id);
        void setCreditAccountId(int _credit_account_id);
        void setBondIdRequired(bool _bond_id_required);
        void setLatefeeIdRequired(bool _latefee_id_required);

        // void setVariableName(const string& value) { variable_name = value; }
        //GETTERS
        const bool getCashierIdRequired();
        const vector<int>& getCreditAvailableIds();
        const bool getCreditBondIdRequired();
        const bool getCustomerIdRequired();
        const vector<int>& getDebitAvailableIds();
        const bool getDebitBondIdRequired();
        const int getId();
        const bool getInstallmentIdRequired();
        const bool getLoanIdRequired();
        const bool getMerchantIdRequired();
        const string& getName();
        const bool getLegRequired();
        const int getDebitAccountId();
        const int getCreditAccountId();
        const bool getBondIdRequired();
        const bool getLatefeeIdRequired();

        // const string& getVariableName() const { return variable_name; }        
        ledger_amount_primitive_orm * get_ledger_amount_primitive_orm();
        ~TemplateLeg();
};


#endif