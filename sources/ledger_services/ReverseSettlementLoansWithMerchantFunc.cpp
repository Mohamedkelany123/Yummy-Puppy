#include "ReverseSettlementLoansWithMerchantFunc.h"

void reversesettlementLoansWithMerchantFunc(vector<map <string,PSQLAbstractORM *> * > * orms_list, int partition_number, mutex* shared_lock,void * extras)
{
    vector <ledger_amount_primitive_orm *> * amounts = new vector <ledger_amount_primitive_orm * >;

    settlement_dashboard_merchantpaymentrequest_primitive_orm * merchant_payment_request = ORML(settlement_dashboard_merchantpaymentrequest, orms_list, 0);

    for (int i=0; i<ORML_SIZE(orms_list); i++){
        ledger_amount_primitive_orm * la_orm = ORML(ledger_amount, orms_list, i);
        amounts->push_back(la_orm);
    }

    BlnkTemplateManager * template_manager = new BlnkTemplateManager(((ReverseSettlementLoansWithMerchantStruct *)extras)->blnkTemplateManager);
    ledger_entry_primitive_orm *entry = template_manager->reverseEntry(la_orms, BDate(merchant_payment_request->get_reverse_entry_date()));

    merchant_payment_request->set_status(3);
    merchant_payment_request->set_reversed(true);

    settlement_dashboard_settlementrequest_primitive_orm * settlement_request = ORML(settlement_dashboard_settlementrequest, orms_list, 0);
    settlement_request->set_status(4);
    settlement_request->set_is_settled(false);    
}

void settlementLoansWithMerchantFunc(vector<map <string,PSQLAbstractORM *> * > * orms_list, int partition_number, mutex* shared_lock,void * extras)
{
    vector <ledger_amount_primitive_orm *> * amounts = new vector <ledger_amount_primitive_orm * >;

    settlement_dashboard_merchantpaymentrequest_primitive_orm * merchant_payment_request = ORML(settlement_dashboard_merchantpaymentrequest, orms_list, 0);

    for (int i=0; i<ORML_SIZE(orms_list); i++){
        ledger_amount_primitive_orm * link_sub_requests = ORML(ledger_amount, orms_list, i);
        amounts->push_back(link_sub_requests);
    }

    BlnkTemplateManager * template_manager = new BlnkTemplateManager(((ReverseSettlementLoansWithMerchantStruct *)extras)->blnkTemplateManager);
    ledger_entry_primitive_orm *entry = template_manager->reverseEntry(link_sub_requests, BDate(merchant_payment_request->get_reverse_entry_date()));

    merchant_payment_request->set_status(3);
    merchant_payment_request->set_reversed(true);

    settlement_dashboard_settlementrequest_primitive_orm * settlement_request = ORML(settlement_dashboard_settlementrequest, orms_list, 0);
    settlement_request->set_status(4);
    settlement_request->set_is_settled(false);    
}

void getMerchantPaymentRequestLoansFunc(vector<map <string,PSQLAbstractORM *> * > * orms_list, int partition_number, mutex* shared_lock,void * extras)
{
    set<int>* loan_ids = (set<int>*)extras;
    settlement_dashboard_merchantpaymentrequest_primitive_orm * sdm_orm;

    for(int i=0; i<ORML_SIZE(orms_list); i++)
    {
        sdm_orm = ORML(settlement_dashboard_merchantpaymentrequest, orms_list, i);
        json transaction = sdm_orm->get_transactions();

        for (int j=0; j<transaction["transactions"].size(); j++){
            int loan_id = transaction["transactions"][j]["loan_id"];
            loan_ids->insert(loan_id);
        }
    }
}