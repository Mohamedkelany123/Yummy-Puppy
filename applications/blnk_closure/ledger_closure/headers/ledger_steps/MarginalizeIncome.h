#ifndef _MARGINALIZEINCOME_H_
#define _MARGINALIZEINCOME_H_

#include <common.h>
#include <common_orm.h>
#include <TemplateManager.h>
#include <LedgerClosureStep.h>
#include <LedgerClosureService.h>
#include <PSQLController.h>
#include <PSQLUpdateQuery.h>




class MarginalizeIncome : public LedgerClosureStep
{
    private:
        loan_app_loan_primitive_orm* lal_orm;
        loan_app_installment_primitive_orm* lai_orm;
        new_lms_installmentextension_primitive_orm* nlie_orm;
        vector<new_lms_installmentlatefees_primitive_orm*>* nlilf_orms;

        int template_id;
        bool is_installment;
        // float prov_percentage;
        BDate closing_day;
    public:
        map<string, funcPtr> funcMap;
        MarginalizeIncome(loan_app_loan_primitive_orm* _lal_orm, loan_app_installment_primitive_orm* _lai_orm, new_lms_installmentextension_primitive_orm* _nlie_orm, vector<new_lms_installmentlatefees_primitive_orm*>* _nlilf_orms, bool _is_installment);
        
        //Setters
        void set_loan_app_loan(loan_app_loan_primitive_orm* _lal_orm);
        void set_loan_app_installment(loan_app_installment_primitive_orm* _lai_orm);
        void set_new_lms_installmentextension(new_lms_installmentextension_primitive_orm* _nlie_orm);
        void set_new_lms_installmentlatefees(vector<new_lms_installmentlatefees_primitive_orm*>* _nlilf_orms);
        
        void set_template_id(int _template_id);
        void set_closing_day(BDate _closing_day);
        //Getters
        loan_app_loan_primitive_orm* get_loan_app_loan();
        loan_app_installment_primitive_orm* get_loan_app_installment();
        new_lms_installmentextension_primitive_orm* get_new_lms_installment_extention();
        vector<new_lms_installmentlatefees_primitive_orm*>* get_new_lms_installmentlatefees();
        int get_template_id();
        BDate get_closing_day();

        LedgerAmount * _init_ledger_amount();
        void stampORMs(map<string, pair<ledger_amount_primitive_orm *, ledger_amount_primitive_orm *>*>* ledger_amount_orms);

        // //static methods
        static LedgerAmount* _marginalize_interest(LedgerClosureStep* marginalizeIncome);
        static LedgerAmount* _marginalize_late_fee(LedgerClosureStep* marginalizeIncome);
        

        bool checkAmounts();

        void setupLedgerClosureService (LedgerClosureService * ledgerClosureService);
        static PSQLJoinQueryIterator* aggregator(QueryExtraFeilds * query_fields);
        static void update_step(); 

    ~MarginalizeIncome();
};





#endif