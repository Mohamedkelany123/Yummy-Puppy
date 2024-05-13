#include <TemplateManager.h>
#include <lms_entrytemplate_primitive_orm.h>


BlnkTemplateManager::BlnkTemplateManager(int template_id, json entry_json)
{
    this->entry_json = entry_json;
    cout << entry_json.dump();


}


void BlnkTemplateManager::loadTemplate (int template_id)
{
    lms_entrytemplate_primitive_orm_iterator * _lms_entrytemplate_primitive_orm_iterator = new lms_entrytemplate_primitive_orm_iterator("main");

    _lms_entrytemplate_primitive_orm_iterator->filter(
        UnaryOperator ("lms_entrytemplate.id",eq,template_id)
    );

    _lms_entrytemplate_primitive_orm_iterator->execute();

    lms_entrytemplate_primitive_orm * temp = _lms_entrytemplate_primitive_orm_iterator->next();
    json _template = temp->get_template();
    this->template_json = _template;
    // cout << "TEMPLATE" << _template.dump();

    delete(_lms_entrytemplate_primitive_orm_iterator);
}


void BlnkTemplateManager::buildLegs(json temp_amount_json)
{
    for (auto& leg : this->template_json["legs"]) { 
        TemplateLeg  template_leg;
        template_leg.setCashierIdRequired(leg["cashier_id"]);
        template_leg.setCreditAvailableIds(leg["credit_available_ids"]);
        template_leg.setCreditBondIdRequired(leg["credit_id"]);
        template_leg.setCustomerIdRequired(leg["customer_id"]);
        template_leg.setDebitAvailableIds(leg["debit_available_ids"]);
        template_leg.setDebitBondIdRequired(leg["debit_id"]);
        template_leg.setId(leg["id"]);
        template_leg.setInstallmentIdRequired(leg["installment_id"]);
        template_leg.setLoanIdRequired(leg["loan_id"]);
        template_leg.setMerchantIdRequired(leg["merchant_id"]);
        template_leg.setName(leg["name"]);
        template_leg.setLegRequired(leg["required"]);

        template_legs.push_back(template_leg); 
    }
    for (temp_amount_json)
    {
        LedgerCompositLeg lc;
        lc.build (template_leg,json item);
        ledger_amounts [lc.get_id()] = lc;
        
    }
    // for(int i=0 ; i<template_legs.size(); i++){
    //     cout << template_legs[i].getName() << endl;
    // }
}
bool BlnkTemplateManager::validate ()
{

}
bool BlnkTemplateManager::buildEntry (json temp_amount_json)
{
    this->loadTemplate(template_id);
    this->buildLegs();

}
BlnkTemplateManager::~BlnkTemplateManager()
{

}

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
const int TemplateLeg::getCreditBondIdRequired() { return credit_bond_id_required; }
const bool TemplateLeg::getCustomerIdRequired() { return customer_id_required; }
const vector<int>& TemplateLeg::getDebitAvailableIds() { return debit_available_ids; }
const int TemplateLeg::getDebitBondIdRequired() { return debit_bond_id_required; }
const int TemplateLeg::getId() { return id; }
const bool TemplateLeg::getInstallmentIdRequired() { return installment_id_required; }
const bool TemplateLeg::getLoanIdRequired() { return loan_id_required; }
const bool TemplateLeg::getMerchantIdRequired() { return merchant_id_required; }
const string& TemplateLeg::getName() { return name; }
const bool TemplateLeg::getLegRequired() { return leg_required; }