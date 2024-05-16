#ifndef DISBURSE_LOANS_H
#define DISBURSE_LOANS_H

#include <common.h>
#include <common_orm.h>
#include <TemplateManager.h>

class LedgerClosureService;

class LedgerClosureStep {
    protected:
    public:
        LedgerClosureStep(){}
        virtual void setupLedgerClosureService (LedgerClosureService * ledgerClosureService) = 0;
        virtual ~LedgerClosureStep(){}

};
typedef LedgerAmount (*funcPtr)(LedgerClosureStep *);

class DisburseLoan : public LedgerClosureStep
{
    private:
        loan_app_loan_primitive_orm * lal_orm;
        BlnkTemplateManager* temp_manager;
        int template_id;

        //<Leg Name VariableName(Function)>

        //init funcMap method


    public:
        map<string, funcPtr> funcMap;
        DisburseLoan(BlnkTemplateManager * _temp_manager, loan_app_loan_primitive_orm * _lal_orm, float short_term_principal, float long_term_principal, float percentage);

        // void generateFuncMap();
        
        //Setters
        void set_loan_app_loan(loan_app_loan_primitive_orm* _lal_orm);
        void set_template_manager(BlnkTemplateManager* _temp_manager);
        void set_template_id(int _template_id);

        //Getters
        loan_app_loan_primitive_orm* get_loan_app_loan();
        BlnkTemplateManager* get_template_manager();
        int get_template_id();


        // //static methods
        static LedgerAmount _calc_short_term_receivable_balance(LedgerClosureStep *disburseLoan);
        static LedgerAmount _calc_mer_t_bl_fee(LedgerClosureStep *disburseLoan);
        static LedgerAmount _calc_provision_percentage(LedgerClosureStep *disburseLoan);
        static LedgerAmount _calc_cashier_fee(LedgerClosureStep *disburseLoan);
        static LedgerAmount _calc_bl_t_mer_fee(LedgerClosureStep *disburseLoan);
        static LedgerAmount _calc_loan_upfront_fee(LedgerClosureStep *disburseLoan);
        static LedgerAmount _calc_long_term_receivable_balance(LedgerClosureStep *disburseLoan);

        void setupLedgerClosureService (LedgerClosureService * ledgerClosureService);
        

    ~DisburseLoan();
};




class LedgerClosureService {

    private:
            map <string,funcPtr> funcMap;
            LedgerClosureStep * ledgerClosureStep;
    public:
        LedgerClosureService (LedgerClosureStep * _ledgerClosureStep)
        {
            ledgerClosureStep = _ledgerClosureStep;
        }
        void addHandler (string legName, funcPtr func)
        {
            funcMap [legName]= func;
        }
        map <string,LedgerAmount> inference ()
        {
            map <string,LedgerAmount> la ;
            for (auto f : funcMap)
            {
                la[f.first] = (f.second)(ledgerClosureStep);
            }
            return la;
        }
};


class DisburseLoanCalculator {

    public:
        
        
};

// float get_provisions_percentage();


// float get_provisions_percentage()
// {
//         //Query to return percentage from loan_app_provision
//         PSQLJoinQueryIterator * psqlQueryJoinProvisions = new PSQLJoinQueryIterator ("main",
//         {new loan_app_loanstatus_primitive_orm("main"),new loan_app_provision_primitive_orm("main")},
//         {{{"loan_app_loanstatus","id"},{"loan_app_provision","status_id"}}});

//         psqlQueryJoinProvisions->filter(
//             UnaryOperator ("loan_app_loanstatus.name",eq,"CURRENT")
//         );

//         psqlQueryJoinProvisions->execute();
//         map <string,PSQLAbstractORM *> * orms =  psqlQueryJoinProvisions->next();
//         if (orms == nullptr){
//             throw std::runtime_error( "Query Returns Null");
//         }
//         loan_app_provision_primitive_orm * lap_orm = ORM(loan_app_provision,orms);
//         float percentage = lap_orm->get_percentage();
//         // cout << "PERCENTAGE: " << percentage << endl;

//         return percentage;
// }


#endif