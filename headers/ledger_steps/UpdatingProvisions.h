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
        tms_app_loaninstallmentfundingrequest_primitive_orm * tal_orm;
        float loans_rec_balance;
        float long_term_balance;
        float impairment_provisions_balance;
        string start_date;
        string end_date;
        int history_provision_percentage;
        int loan_provision_percentage;
        float missing_provisions;

    public:
    map<string, funcPtr> funcMap;
    UpdatingProvisions(map<string,PSQLAbstractORM*>* _orms,string start_date, string end_date);
    void setMissingProvisions();
    void setupLedgerClosureService(LedgerClosureService * ledgerClosureService);
    LedgerAmount * _init_ledger_amount();
    static PSQLJoinQueryIterator* aggregator(string _closure_date_string,string start_fiscal_year,string end_fiscal_year,string end_date);
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