#include <TemplateManager.h>
#include <lms_entrytemplate_primitive_orm.h>


BlnkTemplateManager::BlnkTemplateManager(int template_id, json entry_json)
{
    this->entry_json = entry_json;
    cout << entry_json.dump();
    this->loadTemplate(template_id);
    this->buildLegs();


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


void BlnkTemplateManager::buildLegs()
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
    // for(int i=0 ; i<template_legs.size(); i++){
    //     cout << template_legs[i].getName() << endl;
    // }
}
bool BlnkTemplateManager::validate ()
{

}
bool BlnkTemplateManager::buildEntry (json temp_amount_json)
{

}
BlnkTemplateManager::~BlnkTemplateManager()
{

}
