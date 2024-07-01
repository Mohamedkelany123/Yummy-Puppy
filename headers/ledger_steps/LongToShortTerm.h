#ifndef LONG_TO_SHORT_TERM_H
#define LONG_TO_SHORT_TERM_H

#include <common.h>
#include <common_orm.h>
#include <TemplateManager.h>
#include <LedgerClosureStep.h>
#include <LedgerClosureService.h>
#include <PSQLUpdateQuery.h>


class LongToShortTerm : public LedgerClosureStep
{
    private:
        loan_app_loan_primitive_orm * lal_orm;
        loan_app_installment_primitive_orm* lai_orm;
        new_lms_installmentextension_primitive_orm* nli_orm;

        float inst_balance;
        float merchant_balance;


    public:
        map<string, funcPtr> funcMap;
        LongToShortTerm(map <string,PSQLAbstractORM *> * _orms_list, float _percentage);
        
        //Setters
        void set_loan_app_loan(loan_app_loan_primitive_orm* _lal_orm);
        void set_new_lms_installmentextension(new_lms_installmentextension_primitive_orm* _nli_orm);
        void set_loan_app_installment(loan_app_installment_primitive_orm* _lai_orm);
        // void set_template_id(int _template_id);

        // void set_inst_balance(float _principal);
        // void set_merchant_balance(float _principal);

        

        //Getters
        loan_app_loan_primitive_orm* get_loan_app_loan();
        loan_app_installment_primitive_orm* get_loan_app_installment();

        // int get_template_id();
        // float get_inst_balance();
        // float get_merchant_balance();




        LedgerAmount * _init_ledger_amount();

        void stampORMs(map<string, LedgerCompositLeg *> *leg_amounts);

        // //static methods
        static LedgerAmount * _reclassify_inst_balance(LedgerClosureStep *LongToShortTerm); 
        static LedgerAmount * _reclassify_merchant_balance(LedgerClosureStep *LongToShortTerm); 

        void setupLedgerClosureService (LedgerClosureService * ledgerClosureService);

        static PSQLJoinQueryIterator* aggregator(string _closure_date_string);
        static void update_step(); 



        ~LongToShortTerm();
};


#endif