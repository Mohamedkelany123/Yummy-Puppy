#include <LedgerCompositLeg.h>

LedgerCompositLeg::LedgerCompositLeg()
{
        leg_id = -1;
        bond_id = -1;
        leg = new pair<ledger_amount_primitive_orm*, ledger_amount_primitive_orm*>();
}


void LedgerCompositLeg::buildLeg (TemplateLeg * template_leg,  LedgerAmount * entry_data, ledger_amount_primitive_orm * leg_side, bool is_debit)
{
        leg_side->set_leg_temple_id(template_leg->getId());
        leg_side->set_currency_id(1);
        leg_side->set_reversal_bool(0);
        if (is_debit){
                leg_side->set_account_id(template_leg->getDebitAccountId());
                leg_side->set_amount(-entry_data->getAmount());
                leg_side->set_amount_local(-entry_data->getAmount());

                if(template_leg->getBondIdRequired() &&  template_leg->getDebitBondIdRequired() && entry_data->getInstallmentId() != 0 ){
                if (this->bond_id != 0){
                        leg_side->set_bond_id(this->bond_id);
                       }
                }
        } else{
                
                leg_side->set_account_id(template_leg->getCreditAccountId());
                leg_side->set_amount(entry_data->getAmount());
                leg_side->set_amount_local(entry_data->getAmount());
                if(template_leg->getBondIdRequired() &&  template_leg->getCreditBondIdRequired() && entry_data->getInstallmentId() != 0 ){
                if (this->bond_id != 0){
                        leg_side->set_bond_id(this->bond_id);
                       }
                }
        }

        if(entry_data->getCashierId() != 0){
               leg_side->set_cashier_id(entry_data->getCashierId());
        }
        
   
        if(entry_data->getCustomerId() != 0){
                leg_side->set_customer_id(entry_data->getCustomerId());

        }
 
        if(entry_data->getLoanId() != 0){
                leg_side->set_loan_id(entry_data->getLoanId());
        }
        
      
        if(entry_data->getInstallmentId() != 0){
                leg_side->set_installment_id(entry_data->getInstallmentId());

        }
  
        if(entry_data->getMerchantId() != 0){
                leg_side->set_merchant_id(entry_data->getMerchantId());

        }

        leg_side->set_reversal_bool(entry_data->getReveresalBool());

}

void LedgerCompositLeg::validateEntry(TemplateLeg * template_leg,  LedgerAmount * entry_data){

        if(template_leg->getCashierIdRequired() && entry_data->getCashierId() == 0){              
                throw std::invalid_argument( "cashier id is required" );
        }

        if(template_leg->getCustomerIdRequired() && entry_data->getCustomerId() == 0){
        
                throw std::invalid_argument( "customer id is required" );
        }

        if(template_leg->getLoanIdRequired() && entry_data->getLoanId() == 0){
        
                throw std::invalid_argument( "loan id is required" );
        }
        
        if(template_leg->getInstallmentIdRequired() && entry_data->getInstallmentId() == 0){
        
                throw std::invalid_argument( "Installment id is required" );
        }

        if(template_leg->getMerchantIdRequired() && entry_data->getMerchantId() == 0){
        
                throw std::invalid_argument( "Merchant id is required" );
        }
        
        if (template_leg->getLatefeeIdRequired() && entry_data->getLatefeeId() == 0){

                 throw std::invalid_argument( "latefee id is required" );
        }

}
std::pair <ledger_amount_primitive_orm*,ledger_amount_primitive_orm*>* LedgerCompositLeg::build (TemplateLeg * template_leg,  LedgerAmount * entry_data, ledger_entry_primitive_orm * entry,int _cache_partition_number)
{
        ledger_amount_primitive_orm * debit = new ledger_amount_primitive_orm("main", true,true,_cache_partition_number);
        ledger_amount_primitive_orm  * credit  = new ledger_amount_primitive_orm("main", true, true , _cache_partition_number);

        debit->setAddRefernce("entry_id",entry);
        credit->setAddRefernce("entry_id",entry);
        
        try{
                validateEntry(template_leg, entry_data);
        }
        catch (const std::invalid_argument& e) {
                std::cerr << "Validation error: " << e.what() << std::endl;
                return NULL;
        }
        
        // this->bond_id =  getBondId(entry_data->getInstallmentId());
        buildLeg (template_leg, entry_data,debit, true);
        buildLeg (template_leg, entry_data,credit, false);
        leg->first = debit;
        leg->second = credit;

        return leg;

}



int LedgerCompositLeg::getBondId (int installment_id)
{
        tms_app_bond_primitive_orm_iterator * _tms_app_bond_primitive_orm_iterator = new tms_app_bond_primitive_orm_iterator ("main");
        _tms_app_bond_primitive_orm_iterator->execute();
        
        vector<int> bond_ids;
        int x = _tms_app_bond_primitive_orm_iterator->getRowCount();

        tms_app_bond_primitive_orm * bond = _tms_app_bond_primitive_orm_iterator->next(true);
        while(bond != nullptr){
                int _bond = bond->get_fundingfacility_ptr_id();
                bond_ids.push_back(_bond);
                delete(bond);
                bond = _tms_app_bond_primitive_orm_iterator->next(true);
        }



        tms_app_loaninstallmentfundingrequest_primitive_orm_iterator * _tms_app_loaninstallmentfundingrequest_primitive_orm_iterator = new tms_app_loaninstallmentfundingrequest_primitive_orm_iterator ("main");

        _tms_app_loaninstallmentfundingrequest_primitive_orm_iterator->filter(
                UnaryOperator ("tms_app_loaninstallmentfundingrequest.installment_id",eq,installment_id)
        );

        _tms_app_loaninstallmentfundingrequest_primitive_orm_iterator->execute();

        tms_app_loaninstallmentfundingrequest_primitive_orm * funding_req = _tms_app_loaninstallmentfundingrequest_primitive_orm_iterator->next();
        if(funding_req == 0){
                return 0;
        }
        int _funding_req_id =   static_cast<int>(funding_req->get_funding_facility_id());
        if(std::find( bond_ids.begin(),  bond_ids.end(), _funding_req_id) !=  bond_ids.end()) {
                return _funding_req_id;
        } else {
                return 0;

        }

        delete(_tms_app_loaninstallmentfundingrequest_primitive_orm_iterator);
        delete(_tms_app_bond_primitive_orm_iterator);
}

std::pair <ledger_amount_primitive_orm *,ledger_amount_primitive_orm *> * LedgerCompositLeg::getLedgerCompositeLeg ()
{
return leg;
}

LedgerCompositLeg::~LedgerCompositLeg()
{
        delete (leg);
}

