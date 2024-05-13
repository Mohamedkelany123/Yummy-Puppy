#include <LedgerAmount.h>



LedgerAmount::LedgerAmount(int _id, std::string _name, int _debit_account_id, int _cashier_id, int _credit_account_id, int _customer_id, int _loan_id, int _installment_id, int _merchant_id, int _bond_id, int _latefee_id, int _leg_id, int _entry_id, float _amount, int _account_id, bool _is_credit) {
    id = _id;
    name = _name;
    debit_account_id = _debit_account_id;
    cashier_id = _cashier_id;
    credit_account_id = _credit_account_id;
    customer_id = _customer_id;
    loan_id = _loan_id;
    installment_id = _installment_id;
    merchant_id = _merchant_id;
    bond_id = _bond_id;
    latefee_id = _latefee_id;
    leg_id = _leg_id;
    entry_id = _entry_id;
    amount = _amount;
    account_id = _account_id;
    is_credit = _is_credit;
}
LedgerAmount::LedgerAmount(){}
LedgerAmount::~LedgerAmount(){}

void LedgerAmount::setId(int _id) {id = _id; }
void LedgerAmount::setName(string _name) { name = _name; }
void LedgerAmount::setDebitAccountId(int _debit_account_id) { debit_account_id = _debit_account_id; }
void LedgerAmount::setCashierId(int _cashier_id) { cashier_id = _cashier_id; }
void LedgerAmount::setCreditAccountId(int _credit_account_id) { credit_account_id = _credit_account_id; }
void LedgerAmount::setCustomerId(int _customer_id) { customer_id = _customer_id; }
void LedgerAmount::setLoanId(int _loan_id) { loan_id = loan_id; }
void LedgerAmount::setInstallmentId(int _installment_id) { installment_id = _installment_id; }
void LedgerAmount::setMerchantId(int _merchant_id) { merchant_id = _merchant_id; }
void LedgerAmount::setBondId(int _bond_id) {bond_id = _bond_id; }
void LedgerAmount::setLatefeeId(int _latefee_id) { latefee_id = _latefee_id; }
void LedgerAmount::setLegId(int _leg_id) { leg_id = _leg_id; }
void LedgerAmount::setEntryId(int _entry_id) { entry_id = _entry_id; }
void LedgerAmount::setAmount(float _amount) { amount = _amount; }
void LedgerAmount::setAccountId(float _account_id) { account_id = _account_id; }
void LedgerAmount::setIsCredit(bool _is_credit) {is_credit = _is_credit; }

int LedgerAmount::getId() { return id; }
string LedgerAmount::getName() { return name; }
int LedgerAmount::getDebitAccountId() { return debit_account_id; }
int LedgerAmount::getCashierId() { return cashier_id; }
int LedgerAmount::getCreditAccountId() { return credit_account_id; }
int LedgerAmount::getCustomerId() { return customer_id; }
int LedgerAmount::getLoanId() { return loan_id; }
int LedgerAmount::getInstallmentId() { return installment_id; }
int LedgerAmount::getMerchantId() { return merchant_id; }
int LedgerAmount::getBondId() { return bond_id; }
int LedgerAmount::getLatefeeId() { return latefee_id; }
int LedgerAmount::getLegId() { return leg_id; }
int LedgerAmount::getEntryId() { return entry_id; }
float LedgerAmount::getAmount() { return amount; }
int LedgerAmount::getAccountId() { return account_id; }
bool LedgerAmount::getIsCredit() { return is_credit; }