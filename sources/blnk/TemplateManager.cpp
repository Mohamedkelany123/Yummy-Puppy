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

    //
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
void LedgerCompositLeg::build (TemplateLeg * template_leg, LedgerAmount * leg_json)
        {
        // bool bond_id_required;
        // bool latefee_id_required;

                ledger_amount_primitive_orm * debit ;
                ledger_amount_primitive_orm  * credit;

                debit->set_leg_temple_id(template_leg->getId());
                credit->set_leg_temple_id(template_leg->getId());

                debit->set_account_id(template_leg->getDebitAccountId());
                credit->set_account_id(template_leg->getCreditAccountId());
                
                if(template_leg->getCashierIdRequired() && leg_json->getCashierId() == 0){
              
                        throw std::invalid_argument( "cashier id is required" );
                }
                
                if(leg_json->getCashierId() != 0){
                     debit.setCashierId(leg_json->getCashierId());
                     credit.setCashierId(leg_json->getCashierId());
                }
                
                if(template_leg->getCustomerIdRequired() && leg_json->getCustomerId() == 0){
              
                        throw std::invalid_argument( "customer id is required" );
                }
                
                if(leg_json->getCustomerId() != 0){
                    debit.setCustomerId(leg_json->getCustomerId());
                    credit.setCustomerId(leg_json->getCustomerId());

                }


                if(template_leg->getLoanIdRequired() && leg_json->getLoanId() == 0){
              
                        throw std::invalid_argument( "loan id is required" );
                }
                
                if(leg_json->getLoanId() != 0){
                    debit.setLoanId(leg_json->getLoanId());
                    credit.setLoanId(leg_json->getLoanId());

                }
                
                if(template_leg->getInstallmentIdRequired() && leg_json->getInstallmentId() == 0){
              
                        throw std::invalid_argument( "Installment id is required" );
                }
                
                if(leg_json->getInstallmentId() != 0){
                    debit.setInstallmentId(leg_json->getInstallmentId());
                    credit.setInstallmentId(leg_json->getInstallmentId());

                }

                if(template_leg->getMerchantIdRequired() && leg_json->getMerchantId() == 0){
              
                        throw std::invalid_argument( "Merchant id is required" );
                }
                
                if(leg_json->getMerchantId() != 0){
                    debit.setMerchantId(leg_json->getMerchantId());
                    credit.setMerchantId(leg_json->getMerchantId());

                }


                if(template_leg->getBondIdRequired()){
                   if (!template_leg->getCreditBondIdRequired()){
                        credit.setBondId(-1);

                   }
                    if (!template_leg->getDebitBondIdRequired()){
                        debit.setBondId(-1);

                   }
                }

                credit->setAmount(leg_json->getAmount());
                debit->setAmount(-leg_json->getAmount());

              



                

                // some code to build credit debit

                leg.first = debit;
                leg.second = credit;

        }
