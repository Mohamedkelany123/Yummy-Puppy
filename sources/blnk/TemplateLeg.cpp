#include <TemplateLeg.h>

TemplateLeg::TemplateLeg(){}
TemplateLeg::~TemplateLeg(){}
void TemplateLeg::setCashierIdRequired(bool _cashier_id_required) { cashier_id_required = _cashier_id_required; }
void TemplateLeg::setCreditAvailableIds(const vector<int>& _credit_available_ids) { credit_available_ids = _credit_available_ids; }
void TemplateLeg::setCreditBondIdRequired(int _credit_bond_id_required) { credit_bond_id_required = _credit_bond_id_required; }
void TemplateLeg::setCustomerIdRequired(bool _customer_id_required) { customer_id_required = _customer_id_required; }
void TemplateLeg::setDebitAvailableIds(const vector<int>& _debit_available_ids) { debit_available_ids = _debit_available_ids; }
void TemplateLeg::setDebitBondIdRequired(int _debit_bond_id_required) { debit_bond_id_required = _debit_bond_id_required; }
void TemplateLeg::setId(int _id) { id = _id; }
void TemplateLeg::setInstallmentIdRequired(bool _installment_id_required) { installment_id_required = _installment_id_required; }
void TemplateLeg::setLoanIdRequired(bool _loan_id_required) { loan_id_required = _loan_id_required; }
void TemplateLeg::setMerchantIdRequired(bool _merchant_id_required) { merchant_id_required = _merchant_id_required; }
void TemplateLeg::setName(const string& _name) { name = _name; }
void TemplateLeg::setLegRequired(bool _leg_required) { leg_required = _leg_required; }




const bool TemplateLeg::getCashierIdRequired() { return cashier_id_required; }
const vector<int>& TemplateLeg::getCreditAvailableIds() { return credit_available_ids; }
const bool TemplateLeg::getCreditBondIdRequired() { return credit_bond_id_required; }
const bool TemplateLeg::getCustomerIdRequired() { return customer_id_required; }
const vector<int>& TemplateLeg::getDebitAvailableIds() { return debit_available_ids; }
const bool TemplateLeg::getDebitBondIdRequired() { return debit_bond_id_required; }
const int TemplateLeg::getId() { return id; }
const bool TemplateLeg::getInstallmentIdRequired() { return installment_id_required; }
const bool TemplateLeg::getLoanIdRequired() { return loan_id_required; }
const bool TemplateLeg::getMerchantIdRequired() { return merchant_id_required; }
const string& TemplateLeg::getName() { return name; }
const bool TemplateLeg::getLegRequired() { return leg_required; }
const int TemplateLeg::getDebitAccountId() { return debit_account_id; }
const int TemplateLeg::getCreditAccountId() { return credit_account_id; }
const bool TemplateLeg::getBondIdRequired() { return bond_id_required; }