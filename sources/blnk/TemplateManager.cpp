#include <TemplateManager.h>
#include <lms_entrytemplate_primitive_orm.h>
#include <map>  
#include <string>


BlnkTemplateManager::BlnkTemplateManager(int template_id, map<string, LedgerAmount> _entry_data)
{
    entry_data = _entry_data;
    this->buildEntry(template_id);
    entry = NULL;
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


bool BlnkTemplateManager::buildLegs()
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

        template_legs[template_leg.getName()] = template_leg; 
    }

    for (const auto& entry : this->entry_data) {
        if (this->entry_data.empty()) {
            cout << "entry_data is empty!" << endl;
            break;
        }
        const std::string& leg_name = entry.first;     
        const LedgerAmount& entry_values = entry.second; 

        cout << "Leg Name:" << leg_name << endl;

        // LedgerCompositLeg lc;
        // bool is_built = lc.build(template_legs[leg_name], entry_values);

        // if(!is_built){
        //     return false;
        // }
    }

    return true;

}
bool BlnkTemplateManager::validate ()
{

}
bool BlnkTemplateManager::buildEntry (int template_id)
{
    this->loadTemplate(template_id);
    bool is_built = this->buildLegs();
    createEntry ();
    return is_built;
}
BlnkTemplateManager::~BlnkTemplateManager()
{

}

bool LedgerCompositLeg::buildLeg (ledger_amount_primitive_orm * leg)
{
                debit->set_leg_temple_id(template_leg->getId());
                credit->set_leg_temple_id(template_leg->getId());

                debit->set_account_id(template_leg->getDebitAccountId());
                credit->set_account_id(template_leg->getCreditAccountId());
                
                if(template_leg->getCashierIdRequired() && leg_json->getCashierId() == 0){              
                        throw std::invalid_argument( "cashier id is required" );
                }
                
                if(leg_json->getCashierId() != 0){
                     debit->set_cashier_id(leg_json->getCashierId());
                     credit->set_cashier_id(leg_json->getCashierId());
                }
                
                if(template_leg->getCustomerIdRequired() && leg_json->getCustomerId() == 0){
              
                        throw std::invalid_argument( "customer id is required" );
                }
                
                if(leg_json->getCustomerId() != 0){
                    debit->set_customer_id(leg_json->getCustomerId());
                    credit->set_customer_id(leg_json->getCustomerId());

                }


                if(template_leg->getLoanIdRequired() && leg_json->getLoanId() == 0){
              
                        throw std::invalid_argument( "loan id is required" );
                }
                
                if(leg_json->getLoanId() != 0){
                    debit->set_loan_id(leg_json->getLoanId());
                    credit->set_loan_id(leg_json->getLoanId());

                }
                
                if(template_leg->getInstallmentIdRequired() && leg_json->getInstallmentId() == 0){
              
                        throw std::invalid_argument( "Installment id is required" );
                }
                
                if(leg_json->getInstallmentId() != 0){
                    debit->set_installment_id(leg_json->getInstallmentId());
                    credit->set_installment_id(leg_json->getInstallmentId());

                }

                if(template_leg->getMerchantIdRequired() && leg_json->getMerchantId() == 0){
              
                        throw std::invalid_argument( "Merchant id is required" );
                }
                
                if(leg_json->getMerchantId() != 0){
                    debit->set_merchant_id(leg_json->getMerchantId());
                    credit->set_merchant_id(leg_json->getMerchantId());

                }


                if(template_leg->getBondIdRequired()){
                   if (!template_leg->getCreditBondIdRequired()){
                        credit->set_bond_id(-1);

                   }
                    if (!template_leg->getDebitBondIdRequired()){
                        debit->set_bond_id(-1);

                   }
                }

                credit->set_amount(leg_json->getAmount());
                debit->set_amount(-leg_json->getAmount());

}
bool LedgerCompositLeg::build (TemplateLeg * template_leg, LedgerAmount * leg_json)
        {
        // bool bond_id_required;
        // bool latefee_id_required;

                ledger_amount_primitive_orm * debit = new ledger_amount_primitive_orm();
                ledger_amount_primitive_orm  * credit  = new ledger_amount_primitive_orm();
                try{

                buildLeg (debit);
                buildLeg (credit);
                }
                catch ()
                {


                }
              



                

                // some code to build credit debit

                leg.first = debit;
                leg.second = credit;
                return true;

        }


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


