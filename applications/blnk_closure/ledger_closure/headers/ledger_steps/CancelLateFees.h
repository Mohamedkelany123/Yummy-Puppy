#ifndef CANCEL_LATEFEES_H
#define CANCEL_LATEFEES_H

#include <common.h>
#include <common_orm.h>
#include <TemplateManager.h>
#include <LedgerClosureStep.h>
#include <LedgerClosureService.h>
#include <PSQLUpdateQuery.h>
#include <CancelLateFees.h>


class CancelLateFees : public LedgerClosureStep
{
    private:
        loan_app_installment_primitive_orm * lai_orm;
        loan_app_loan_primitive_orm * lal_orm;
        vector<new_lms_installmentlatefees_primitive_orm *> * nli_orms;
        map <string,pair< vector<new_lms_installmentlatefees_primitive_orm *> *, double> *> *  latefees_partition;
        double amount;
        int template_id;
      


    public:
        map<string, funcPtr> funcMap;
        CancelLateFees(vector<map <string,PSQLAbstractORM *> * > * _orms_list);
        
        //Setters
        void set_loan_app_loan(loan_app_loan_primitive_orm* _lal_orm);
        void set_loan_app_installment(loan_app_installment_primitive_orm* _installment_orm);
        void set_late_fees(vector<new_lms_installmentlatefees_primitive_orm *> *_nli_orms);
        void set_template_id(int _template_id);
        void set_amount(double _amount);
        void set_latefees_partition( map <BDate,pair< vector<new_lms_installmentlatefees_primitive_orm *> *, double> *> * _latefees_partition);
      
        

        //Getters
        loan_app_loan_primitive_orm* get_loan_app_loan();
        loan_app_installment_primitive_orm* get_loan_app_installment();
        vector<new_lms_installmentlatefees_primitive_orm *> * get_late_fees(); 
        map <string,pair< vector<new_lms_installmentlatefees_primitive_orm *> *, double> *> * get_latefees_partition();
        double get_amount();
 
        int get_template_id();


        LedgerAmount * _init_ledger_amount();

        // // //static methods
        static LedgerAmount * _calculate_late_fee_amount(LedgerClosureStep *disburseLoan);


        void setupLedgerClosureService (LedgerClosureService * ledgerClosureService);

        static PSQLJoinQueryIterator* aggregator(QueryExtraFeilds * query_fields);
        static void update_step(); 
        void stampORMs(ledger_amount_primitive_orm * la_orm);


        ~CancelLateFees();
};

#endif
