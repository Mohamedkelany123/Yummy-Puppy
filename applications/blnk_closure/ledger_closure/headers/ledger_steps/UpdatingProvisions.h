#ifndef _UPDATING_PROVISIONS_H
#define _UPDATING_PROVISIONS_H

#include <PSQLController.h>
#include <TemplateManager.h>
#include <DisburseLoans.h>
#include <common_orm.h>
#include <common.h>

class UpdatingProvisions : public LedgerClosureStep {
    private:
        loan_app_loan_primitive_orm * lal_orm;
        tms_app_loanfundingrequest_primitive_orm * tal_orm;
        float loans_rec_balance;
        float long_term_balance;
        float impairment_provisions_balance;
        string start_date;
        string end_date;
        float history_provision_percentage;
        float loan_provision_percentage;
        float missing_provisions;
        bool on_balance;
        int funding_facility_id;

    public:
    map<string, funcPtr> funcMap;
    UpdatingProvisions(map<string,PSQLAbstractORM*>* _orms,string start_date, string end_date);
    UpdatingProvisions(loan_app_loan_primitive_orm * lal_orm,string start_date, string end_date);
    void calculateMissingProvisions();
    void setupLedgerClosureService(LedgerClosureService * ledgerClosureService);
    LedgerAmount * _init_ledger_amount();
    static PSQLJoinQueryIterator* aggregator(QueryExtraFeilds * query_fields,string start_fiscal_year,string end_fiscal_year,string end_date);
    static loan_app_loan_primitive_orm_iterator* aggregator_onbalance(QueryExtraFeilds * query_fields,string start_fiscal_year,string end_fiscal_year,string end_date);
    static PSQLJoinQueryIterator* aggregator_offbalance(QueryExtraFeilds * query_fields,string start_fiscal_year,string end_fiscal_year,string end_date);
    static void update_step(); 

    //Getters
    float get_missing_provisions();

    //Setters
    void set_missing_provisions(float x);

    static LedgerAmount * _increasing_provisions(LedgerClosureStep *UpdatingProvisions);
    static LedgerAmount * _decreasing_provisions(LedgerClosureStep *UpdatingProvisions);

    ~UpdatingProvisions();
};

 #endif