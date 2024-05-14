#include <TemplateManager.h>

BlnkTemplateManager::BlnkTemplateManager(int template_id, map<string, LedgerAmount> _entry_data, BDate entry_date)
{
    entry_data = _entry_data;
    this->buildEntry(template_id,entry_date );
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
        if (leg["required"]){
            auto it = entry_data.find(leg["name"]);

            if (it == entry_data.end()) {
                throw std::invalid_argument( "Leg is required but not found in entry data" );

        }}
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
   
    for (auto& ent : this->entry_data) {
        // if (this->entry_data.empty()) {
        //     cout << "entry_data is empty!" << endl;
        //     break;
        // }
        string leg_name = ent.first;     
        LedgerAmount * entry_values = &ent.second; 
        // LedgerAmount* pntrAmount= const_cast< LedgerAmount*>(entry_values);

        cout << "Leg Name:" << leg_name << endl;

        
        LedgerCompositLeg lc;
        bool is_built = lc.build(&template_legs[leg_name],  entry_values,entry);

        if(!is_built){
            return false;
        }
    }

    return true;

}
bool BlnkTemplateManager::validate ()
{

}
bool BlnkTemplateManager::buildEntry (int template_id, BDate entry_date)
{
    this->loadTemplate(template_id);
    this->createEntry(template_id, entry_date);
    bool is_built = this->buildLegs();
    return is_built;
}
int BlnkTemplateManager::createEntry (int template_id, BDate entry_date)
{
    entry  = new ledger_entry_primitive_orm("main");
    entry->set_entry_date(entry_date.getDateString());
    entry->set_template_id(template_id);
    entry->set_month_code(1);
    return 0;
}

BlnkTemplateManager::~BlnkTemplateManager()
{

}











