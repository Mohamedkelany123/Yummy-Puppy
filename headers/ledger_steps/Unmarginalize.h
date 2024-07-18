#ifndef UNMARGINLIZE_H
#define UNMARGINLIZE_H

#include <common.h>
#include <common_orm.h>
#include <TemplateManager.h>
#include <LedgerClosureStep.h>
#include <LedgerClosureService.h>
#include <PSQLUpdateQuery.h>






class Unmarginalize : public LedgerClosureStep
{
    private:
        loan_app_loan_primitive_orm * lal_orm;
        int installment_id;
        new_lms_installmentextension_primitive_orm * installment_extension;
        vector<new_lms_installmentlatefees_primitive_orm*>* late_fees;
        float unmarginalized_amount;

        

    public:
        map<string, funcPtr> funcMap;
        Unmarginalize(loan_app_loan_primitive_orm * loan,int ins_id,new_lms_installmentextension_primitive_orm * installment_ext, float unmarginalized_inst_amount ,vector<new_lms_installmentlatefees_primitive_orm *>* late_fees);
        
        //Setters


        

        //Getters
        vector<new_lms_installmentlatefees_primitive_orm*>* get_late_fees();
        new_lms_installmentextension_primitive_orm * get_installment_extension();
        int get_installment_id();
        float get_unmarginalized_amount();




        // float _calculate_loan_upfront_fee();
        LedgerAmount * _init_ledger_amount();

        void stampORMs(ledger_entry_primitive_orm* entry, ledger_amount_primitive_orm * la_orm);
        void setupLedgerClosureService (LedgerClosureService * ledgerClosureService);         
        //static methods
        static LedgerAmount *_unmarginalize_late_fee(LedgerClosureStep *Unmarginalize);
        static LedgerAmount *_unmarginalize_interest(LedgerClosureStep *Unmarginalize);
        static PSQLJoinQueryIterator* aggregator(string _closure_date_string);
        static void update_step(); 
        static map <string,map<int,pair<pair<new_lms_installmentextension_primitive_orm*,float>*,vector<new_lms_installmentlatefees_primitive_orm*> *> *> *> * get_date_map(vector<map <string,PSQLAbstractORM *> * > * orms);
        

    ~Unmarginalize();
};




#endif