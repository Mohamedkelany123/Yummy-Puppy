#include <TemplateManager.h>
#include <lms_entrytemplate_primitive_orm.h>
#include <map>  
#include <string>


BlnkTemplateManager::BlnkTemplateManager(int template_id, map<string, LedgerAmount> _entry_data)
{
    entry_data = _entry_data;
    this->buildEntry(template_id);
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
        const std::string& leg_name = entry.first;     
        const LedgerAmount& entry_values = entry.second; 

        LedgerCompositLeg lc;
        bool is_built = lc.build(template_legs[leg_name], entry_values);

        if(!is_built){
            return false;
        }
    }

    return true;

}
bool BlnkTemplateManager::validate ()
{

}
bool BlnkTemplateManager::buildEntry (int template_id)
{
    this->loadTemplate(template_id);
    this->buildLegs();

}
BlnkTemplateManager::~BlnkTemplateManager()
{

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