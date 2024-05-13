#include <TemplateManager.h>

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
    
    return is_built;
}
BlnkTemplateManager::~BlnkTemplateManager()
{

}






