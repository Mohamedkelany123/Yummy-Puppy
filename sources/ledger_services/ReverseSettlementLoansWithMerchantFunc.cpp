#include "ReverseSettlementLoansWithMerchantFunc.h"



void processLoanOrms(vector<map <string,PSQLAbstractORM *> * > * orms_list, int partition_number, mutex* shared_lock,void * extras)
{
    map<int, loan_app_loan_primitive_orm*>* loanMap = (map<int, loan_app_loan_primitive_orm*>*)extras;
    loan_app_loan_primitive_orm* lal_orm;
    int loanId;
    for (int i=0; i<ORML_SIZE(orms_list); i++) {
        lal_orm = ORML(loan_app_loan, orms_list, i);
        loanId = lal_orm->get_id();
        (*loanMap)[loanId] = lal_orm;
    }
}

void getMerchantPaymentRequestLoansFunc(vector<map <string,PSQLAbstractORM *> * > * orms_list, int partition_number, mutex* shared_lock,void * extras)
{
    set<int>* loan_ids = (set<int>*)extras;
    settlement_dashboard_merchantpaymentrequest_primitive_orm * ;
sdm_orm
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

void settleLoansWithMerchant(vector<map<string, PSQLAbstractORM *> *> *orms_list, int partition_number, mutex *shared_lock, void *extras)
{
    BlnkTemplateManager* templateManager = (BlnkTemplateManager*)(((SettlementLoansWithMerchantStruct*)extras)->blnkTemplateManager);
    map<int, loan_app_loan_primitive_orm*>* loanMap = (map<int, loan_app_loan_primitive_orm*>*)(((SettlementLoansWithMerchantStruct*)extras)->loanMap);
    map<int, BlnkTemplateManager*> templateManagerMap;
    settlement_dashboard_settlementrequest_primitive_orm* sds_orm;
    settlement_dashboard_merchantpaymentrequest_primitive_orm* sdm_orm;
    loan_app_loan_primitive_orm* lal_orm;
    int loanId, linkId;

    sdm_orm = ORML(settlement_dashboard_merchantpaymentrequest, orms_list, 0);
    for (int i=0; i<transactionsSize; i++){
        json transaction = sdm_orm->get_transactions();
        loanId = transaction["transactions"][i]["loan_id"];
        lal_orm = loanMap[loanId];
        double loan_value = 0, parent_loan_value = 0;
        for (int j=0; j<ORML_SIZE(orms_list); j++) {
            sds_orm = ORML(settlement_dashboard_settlementrequest, orms_list, i);
            linkId = sds_orm->get_link();
            if (templateManagerMap.find(linkId)!=templateManagerMap.end()) {
                templateManagerMap[linkId] = new BlnkTemplateManager(templateManager, partition_number);
                templateManagerMap[linkId]->createEntry();
            }
            bool check_bool = transaction
            int category = sds_orm->get_category();
            string entry_date_string = sds_orm->get_entry_date();
        }
    }


    for (int i=0; i<ORML_SIZE(orms_list); i++) {
        
        
        json transaction = sdm_orm->get_transactions();
        int transactionsSize = transaction["transactions"].size();
        for (int j=0; j<transactionsSize; j++){
            loanId = transaction["transactions"][j]["loan_id"];
            lal_orm = loanMap[loanId];
        }
    }
}
