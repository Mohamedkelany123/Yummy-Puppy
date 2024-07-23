#ifndef _SETTLEMENTLOANSWITHMERCHANT_H_
#define _SETTLEMENTLOANSWITHMERCHANT_H_

#include <common.h>
#include <common_orm.h>
#include <TemplateManager.h>
#include <LedgerClosureStep.h>
#include <LedgerClosureService.h>
#include <PSQLController.h>
#include <PSQLUpdateQuery.h>




class SettlementLoansWithMerchant : public LedgerClosureStep
{
    private:
        settlement_dashboard_settlementrequest_primitive_orm * sds_orm;
        settlement_dashboard_merchantpaymentrequest_primitive_orm* sdm_orm;
        loan_app_loan_primitive_orm* lal_orm; 

        int template_id;
        // float prov_percentage;
        BDate closing_day;
        BDate due_to_overdue_date;

        PSQLJoinQueryIterator* installments_becoming_overdue_agg(string _closure_date_string);
    public:
        map<string, funcPtr> funcMap;
        SettlementLoansWithMerchant();
        SettlementLoansWithMerchant(settlement_dashboard_settlementrequest_primitive_orm *_sds_orm, settlement_dashboard_merchantpaymentrequest_primitive_orm *_sdm_orm, loan_app_loan_primitive_orm* _lal_orm);
        // DueToOverdue(map <string,PSQLAbstractORM *> * _orms, BDate _closing_day, int _ledger_closure_service_type=1);
        
        //Setters
        void set_settlement_dashboard_settlementrequest(settlement_dashboard_settlementrequest_primitive_orm *_sds_orm);
        void set_settlement_dashboard_merchantpaymentrequest(settlement_dashboard_merchantpaymentrequest_primitive_orm *_sdm_orm);
        
        void set_template_id(int _template_id);
        void set_closing_day(BDate _closing_day);

        //Getters
        settlement_dashboard_merchantpaymentrequest_primitive_orm* get_settlement_dashboard_merchantpaymentrequest();
        settlement_dashboard_settlementrequest_primitive_orm* get_settlement_dashboard_settlementrequest();
        loan_app_loan_primitive_orm* get_loan();

        int get_template_id();
        BDate get_closing_day();
        // BDate get_due_to_overdue_day();

        LedgerAmount * _init_ledger_amount();

        void stampORMs(map<string, pair<ledger_amount_primitive_orm *, ledger_amount_primitive_orm *>*>* ledger_amount_orms);
        void setupLedgerClosureService (LedgerClosureService * ledgerClosureService);


        // //static methods
        static void unstampLoans();
        static LedgerAmount * _get_request_amount(LedgerClosureStep *settlementLoansWithMerchant);
        static LedgerAmount* _settle_with_merchant(LedgerClosureStep* settlementLoansWithMerchant);

        bool checkAmounts();

        static PSQLJoinQueryIterator* paymentRequestAggregator(string _closure_date_string);
        static PSQLJoinQueryIterator* loanAggregator(string _closure_date_string, vector<int>* loan_ids);

        static void update_step(); 

        ~SettlementLoansWithMerchant();
};





#endif