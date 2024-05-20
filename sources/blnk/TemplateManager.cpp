#include <TemplateManager.h>

BlnkTemplateManager::BlnkTemplateManager(int _template_id)
{
    template_id = _template_id;
    loadTemplate();
    constructTemplateLegs();
    entry = NULL;
}

map <string , TemplateLeg> BlnkTemplateManager::getTemplateLegs(){return template_legs;}


void BlnkTemplateManager::constructTemplateLegs()
{
     for (auto& leg : this->template_json["legs"]) {
        
        TemplateLeg  template_leg;
        //TODO:: Reiterate To make sure if we need to add any conditions for variable to check if available or not.
        template_leg.setCashierIdRequired(leg["cashier_id"]);
        template_leg.setCreditAvailableIds(leg["credit_available_ids"]);
        
        if (leg.find("credit_bond_id") != leg.end()){
            template_leg.setCreditBondIdRequired(leg["credit_bond_id"]);
        }
        else {
            template_leg.setCreditBondIdRequired(0);
        }
        template_leg.setCustomerIdRequired(leg["customer_id"]);
        template_leg.setDebitAvailableIds(leg["debit_available_ids"]);
        
        if (leg.find("debit_bond_id") != leg.end()){
            template_leg.setDebitBondIdRequired(leg["debit_bond_id"]);
        }
        else {
            template_leg.setDebitBondIdRequired(0);
        }
        if (leg.find("late_fee_id") != leg.end()){
            template_leg.setLatefeeIdRequired(leg["late_fee_id"]);
        }
        else {
            template_leg.setLatefeeIdRequired(0);
        }

        template_leg.setId(leg["id"]);
        template_leg.setInstallmentIdRequired(leg["installment_id"]);
        template_leg.setLoanIdRequired(leg["loan_id"]);
        template_leg.setMerchantIdRequired(leg["merchant_id"]);
        template_leg.setName(leg["name"]);
        template_leg.setLegRequired(leg["required"]);
        template_leg.setCreditAccountId(leg["credit_id"]);
        template_leg.setDebitAccountId(leg["debit_id"]);
        template_legs[template_leg.getName()] = template_leg; 
    }
}

void BlnkTemplateManager::loadTemplate ()
{
    lms_entrytemplate_primitive_orm_iterator * _lms_entrytemplate_primitive_orm_iterator = new lms_entrytemplate_primitive_orm_iterator("main");

    _lms_entrytemplate_primitive_orm_iterator->filter(
        UnaryOperator ("lms_entrytemplate.id",eq, template_id)
    );

    _lms_entrytemplate_primitive_orm_iterator->execute();

    lms_entrytemplate_primitive_orm * temp = _lms_entrytemplate_primitive_orm_iterator->next();
    json _template = temp->get_template();
    this->template_json = _template;

    delete(_lms_entrytemplate_primitive_orm_iterator);
}


bool BlnkTemplateManager::buildLegs(map <string,LedgerAmount*> * ledgerAmounts)
{
    for (auto ent : *(ledgerAmounts)) {
        
        string leg_name = ent.first;     
        LedgerAmount * entry_values = ent.second; 


        LedgerCompositLeg * lc = new LedgerCompositLeg();
        std::pair <ledger_amount_primitive_orm*,ledger_amount_primitive_orm*>* leg_pair = lc->build(&template_legs[leg_name],  entry_values,entry);
        ledger_amounts[leg_name]=lc;
        if(leg_pair == NULL){
            return false;
        }
    }

    return true;

}
bool BlnkTemplateManager::validate (map <string,LedgerAmount*> * ledgerAmounts)
{
    for (auto& leg : this->template_json["legs"]) {
        if (leg["required"]){
            auto it = ledgerAmounts->find(leg["name"]);

            if (it == ledgerAmounts->end()) {
                return false;
            }

        }
    }
    return true;
}
ledger_entry_primitive_orm* BlnkTemplateManager::buildEntry (BDate entry_date, map <string,LedgerAmount*> * ledgerAmounts)
{
    this->createEntry(entry_date);
    bool is_built = this->buildLegs(ledgerAmounts);
    bool is_valid = this->validate(ledgerAmounts);
    if(!(is_valid && is_built)){
        return NULL;
    }
    return entry;
}
void BlnkTemplateManager::createEntry (BDate entry_date)
{
    entry  = new ledger_entry_primitive_orm("main", true);
    entry->set_entry_date(entry_date.getDateString());
    entry->set_template_id(template_id);
    entry->set_month_code(1);
    entry->set_type("NO");
    entry->set_status("PO");
    entry->set_created_by(0);
}

BlnkTemplateManager::~BlnkTemplateManager()
{
    for (auto la :ledger_amounts)
        delete (la.second);
}

map <string,LedgerCompositLeg *> * BlnkTemplateManager::get_ledger_amounts()
{
    return & ledger_amounts;
}

ledger_amount_primitive_orm * BlnkTemplateManager::getFirstLedgerAmountORM ()
{
    if (ledger_amounts.size() > 0)
        return (ledger_amounts.begin()->second)->getLedgerCompositeLeg()->first;
    else return NULL;
}











