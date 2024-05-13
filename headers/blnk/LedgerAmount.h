#include <common.h>

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
        LedgerAmount();
        LedgerAmount(int _id, std::string _name, int _debit_account_id, int _cashier_id, int _credit_account_id, int _customer_id, int _loan_id, int _installment_id, int _merchant_id, int _bond_id, int _latefee_id, int _leg_id, int _entry_id, float _amount, int _account_id, bool _is_credit);

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