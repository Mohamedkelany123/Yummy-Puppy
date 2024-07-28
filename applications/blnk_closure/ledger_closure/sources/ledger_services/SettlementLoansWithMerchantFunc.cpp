#include "SettlementLoansWithMerchantFunc.h"



double _get_request_amount(settlement_dashboard_merchantpaymentrequest_primitive_orm* sdm_orm, settlement_dashboard_settlementrequest_primitive_orm* sds_orm, loan_app_loan_primitive_orm* lal_orm)
{
    int request_type = sds_orm->get_type();
    
    //getting variables from loan table
    double principle = lal_orm->get_principle();
    double cashier_fee = lal_orm->get_cashier_fee();
    // double merchant_commission = lal_orm->get_merchant_commission();
    double loan_upfront_fee = lal_orm->get_loan_upfront_fee();
    double blnk_to_merchant_fee = lal_orm->get_bl_t_mer_fee();
    double merchant_to_blnk_fee = lal_orm->get_bl_t_mer_fee();
    
    //calculate values from the loan
    double cashier_commision = (principle*cashier_fee)/100;
    cashier_commision = ROUND(cashier_commision);
    double merchant_commission = (principle*blnk_to_merchant_fee)/100;
    merchant_commission = ROUND(merchant_commission);
    double upfront_fee = (loan_upfront_fee/100)*principle;
    upfront_fee = ROUND(upfront_fee);
    double rebate_commission = (principle*merchant_to_blnk_fee)/100;
    rebate_commission = ROUND(rebate_commission);
    double amount = 0;
    switch (request_type)
    {
    case 0:
        amount = ROUND(principle);
        break;
    case 1:
        amount = cashier_commision;
        break;
    case 2:
        amount = merchant_commission ;
        break;
    case 3:
        amount = -upfront_fee;
        break;
    case 4:
        amount = -rebate_commission;
        break;
    }

    return amount;
}

double _get_request_amount(int request_type, double principle, double cashier_fee, double mer_t_bl_fee, double bl_t_mer_fee, double loan_upfront_fee)
{

    //calculate values from the loan
    double cashier_commision = (principle*cashier_fee)/100;
    cashier_commision = ROUND(cashier_commision);
    double merchant_commission = (principle*bl_t_mer_fee)/100;
    merchant_commission = ROUND(merchant_commission);
    double upfront_fee = (loan_upfront_fee/100)*principle;
    upfront_fee = ROUND(upfront_fee);
    double rebate_commission = (principle*mer_t_bl_fee)/100;
    rebate_commission = ROUND(rebate_commission);
    double amount = 0;
    switch (request_type)
    {
    case 0:
        amount = ROUND(principle);
        break;
    case 1:
        amount = cashier_commision;
        break;
    case 2:
        amount = merchant_commission ;
        break;
    case 3:
        amount = -upfront_fee;
        break;
    case 4:
        amount = -rebate_commission;
        break;
    }

    return amount;
}

void settleLoansWithMerchantFunc(vector<map<string, PSQLAbstractORM *> *> *orms_list, int partition_number, mutex *shared_lock, void *extras)
{
    BlnkTemplateManager* templateManager = (BlnkTemplateManager*)(((SettlementLoansWithMerchantStruct*)extras)->blnkTemplateManager);
    double loan_value = 0, parent_loan_value = 0;
    settlement_dashboard_merchantpaymentrequest_primitive_orm* sdm_orm;
    settlement_dashboard_settlementrequest_primitive_orm* sds_orm;
    loan_app_loan_primitive_orm* lal_orm;
    PSQLGeneric_primitive_orm * gorm;


    for (int i=0; i<ORML_SIZE(orms_list); i++) {
        sdm_orm = ORML(settlement_dashboard_merchantpaymentrequest, orms_list, i);
        sds_orm = ORML(settlement_dashboard_settlementrequest, orms_list, i);
        lal_orm = ORML(loan_app_loan, orms_list, i);
        gorm = ORML(PSQLGeneric,orms_list,i);
        string type = gorm->get("type");
        double parent_principle = gorm->toDouble("parent_principle");
        double parent_cashier_fee = gorm->toDouble("parent_cashier_fee");
        double parent_mer_t_bl_fee = gorm->toDouble("parent_mer_t_bl_fee");
        double parent_bl_t_mer_fee = gorm->toDouble("parent_bl_t_mer_fee");
        double parent_loan_upfront_fee = gorm->toDouble("parent_loan_upfront_fee");

        int request_type = sds_orm->get_type();


        int category = sds_orm->get_category();
        int activation_user_id = sds_orm->get_activation_user_id();

        loan_value += _get_request_amount(sdm_orm, sds_orm, lal_orm);
        if (type == "Partial Refund") {
            parent_loan_value += _get_request_amount(request_type, parent_principle, parent_cashier_fee, parent_mer_t_bl_fee, parent_bl_t_mer_fee, parent_loan_upfront_fee);
        }
    }
    gorm = ORML(PSQLGeneric,orms_list,0);
    sdm_orm = ORML(settlement_dashboard_merchantpaymentrequest, orms_list, 0);
    sds_orm = ORML(settlement_dashboard_settlementrequest, orms_list, 0);
    lal_orm = ORML(loan_app_loan, orms_list, 0);
    string entry_date_string = sds_orm->get_entry_date();
    BDate entry_date(entry_date);
    string type = gorm->get("type");
    double balance_106 = gorm->toDouble("balance_106"), balance_179 = gorm->toDouble("balance_179");
    bool check_bool = gorm->toBool("check_bool");
    bool can_settle_bool = gorm->toBool("check_bool");
    int settled_pay_id = lal_orm->get_settled_pay_id();
    int category = sds_orm->get_category();
    int parent_loan_id = gorm->toInt("parent_loan_id");
    int parent_merchant_id = gorm->toInt("parent_merchant_id");
    int parent_settle_pay_id = gorm->toInt("parent_settle_pay_id");
    double parent_balance_106 = gorm->toDouble("parent_balance_106");
    double parent_balance_179 = gorm->toDouble("parent_balance_179");
    BlnkTemplateManager *localTemplateManager = new BlnkTemplateManager(templateManager, partition_number);
    if (check_bool && can_settle_bool) {
        if (type == "Full Refund") {
            double amount = loan_value;
            bool check_flag = true;
            if (balance_106 == 0 && balance_179 == 0 && settled_pay_id != 0) {
                check_flag = false;
            }
            if (check_flag) {
                SettlementLoansWithMerchant settlementWithMerchant(loan_value, lal_orm->get_id(), sdm_orm->get_id(), lal_orm->get_merchant_id(), lal_orm->get_customer_id(), category, sds_orm->get_activation_user_id());
                LedgerClosureService* ledgerClosureService = new LedgerClosureService(&settlementWithMerchant);
                settlementWithMerchant.setupLedgerClosureService(ledgerClosureService);
                map<string, LedgerAmount*>* ledgerAmounts = ledgerClosureService->inference();
                localTemplateManager->setEntryData(ledgerAmounts);
                localTemplateManager->buildEntry(entry_date);
            }
            settlement_dashboard_loanentryrequest_primitive_orm* sdl_orm = new settlement_dashboard_loanentryrequest_primitive_orm("main");
            sdl_orm->setUpdateRefernce("request", sdm_orm);
            sdl_orm->setUpdateRefernce("loan", lal_orm);
            sdl_orm->set_link(sds_orm->get_link());
            lal_orm->setUpdateRefernce("settled_cancel", sdm_orm);
        }
        else if (type == "Partial Refund") {
            double amount = parent_loan_value;
            bool check_flag = true;
            if (balance_106 == 0 && balance_179 == 0 && parent_settle_pay_id != 0) {
                check_flag = false;
            }
            if (check_flag) {
                SettlementLoansWithMerchant settlementWithMerchant(loan_value, parent_loan_id, sdm_orm->get_id(), parent_merchant_id, lal_orm->get_customer_id(), category, sds_orm->get_activation_user_id());
                LedgerClosureService* ledgerClosureService = new LedgerClosureService(&settlementWithMerchant);
                settlementWithMerchant.setupLedgerClosureService(ledgerClosureService);
                map<string, LedgerAmount*>* ledgerAmounts = ledgerClosureService->inference();
                localTemplateManager->setEntryData(ledgerAmounts);
                localTemplateManager->buildEntry(entry_date);
            }
            settlement_dashboard_loanentryrequest_primitive_orm* sdl_orm_parent = new settlement_dashboard_loanentryrequest_primitive_orm("main");
            sdl_orm_parent->set_request_id(sdm_orm->get_id());
            sdl_orm_parent->set_loan_id(parent_loan_id);
            sdl_orm_parent->set_link(sds_orm->get_link());
            // stamp parent loan and not child loan
            //lal_orm->set_settled_cancel_id(sdm_orm->get_id());

            amount = loan_value;     
            SettlementLoansWithMerchant settlementWithMerchant(loan_value, lal_orm->get_id(), sdm_orm->get_id(), lal_orm->get_merchant_id(), lal_orm->get_customer_id(), category, sds_orm->get_activation_user_id());
            LedgerClosureService* ledgerClosureService = new LedgerClosureService(&settlementWithMerchant);
            settlementWithMerchant.setupLedgerClosureService(ledgerClosureService);
            map<string, LedgerAmount*>* ledgerAmounts = ledgerClosureService->inference();
            localTemplateManager->setEntryData(ledgerAmounts);
            localTemplateManager->buildEntry(entry_date);
            settlement_dashboard_loanentryrequest_primitive_orm* sdl_orm = new settlement_dashboard_loanentryrequest_primitive_orm("main");
            sdl_orm->set_request_id(sdm_orm->get_id());
            sdl_orm->set_loan_id(parent_loan_id);
            sdl_orm->set_link(sds_orm->get_link());
            lal_orm->set_settled_cancel_id(sdm_orm->get_id());
        }
        else {
            double amount = loan_value;     
            SettlementLoansWithMerchant settlementWithMerchant(loan_value, lal_orm->get_id(), sdm_orm->get_id(), lal_orm->get_merchant_id(), lal_orm->get_customer_id(), category, sds_orm->get_activation_user_id());
            LedgerClosureService* ledgerClosureService = new LedgerClosureService(&settlementWithMerchant);
            settlementWithMerchant.setupLedgerClosureService(ledgerClosureService);
            map<string, LedgerAmount*>* ledgerAmounts = ledgerClosureService->inference();
            localTemplateManager->setEntryData(ledgerAmounts);
            localTemplateManager->buildEntry(entry_date);
            settlement_dashboard_loanentryrequest_primitive_orm* sdl_orm = new settlement_dashboard_loanentryrequest_primitive_orm("main");
            sdl_orm->set_request_id(sdm_orm->get_id());
            sdl_orm->set_loan_id(parent_loan_id);
            sdl_orm->set_link(sds_orm->get_link());
            lal_orm->set_settled_cancel_id(sdm_orm->get_id());
        }
    }
}
