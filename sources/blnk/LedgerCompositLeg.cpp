#include <LedgerCompositLeg.h>

bool LedgerCompositLeg::build (TemplateLeg * template_leg, LedgerAmount * leg_json)
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

              



                

                // some code to build credit debit

                leg.first = debit;
                leg.second = credit;
                return true;

        }
