#include <common.h>
#include <loan_app_loan_primitive_orm.h>
#include <loan_app_installment_primitive_orm.h>
#include <new_lms_installmentextension_primitive_orm.h>
#include <loan_app_loanstatus_primitive_orm.h>
#include <TemplateManager.h>


// typedef LedgerAmount (*funcPtr)(DisburseLoan*);


class DisburseLoan
{
    private:
        loan_app_installment_primitive_orm * lai_orm;
        new_lms_installmentextension_primitive_orm * nli_orm ;
        loan_app_loan_primitive_orm * lal_orm;
        BlnkTemplateManager* temp_manager;
        int template_id = 4;

        //<Leg Name VariableName(Function)>

        //init funcMap method


    public:
        // map<string, funcPtr> funcMap;
        DisburseLoan(BlnkTemplateManager *, loan_app_installment_primitive_orm *, new_lms_installmentextension_primitive_orm *,loan_app_loan_primitive_orm *);

        void generateFuncMap();
        
        //Setters
        void set_loan_app_installment(loan_app_installment_primitive_orm* _lai_orm);
        void set_new_lms_installmentextension(new_lms_installmentextension_primitive_orm* _nli_orm);
        void set_loan_app_loan(loan_app_loan_primitive_orm* _lal_orm);
        void set_template_manager(BlnkTemplateManager* _temp_manager);
        void set_template_id(int _template_id);

        //Getters
        loan_app_installment_primitive_orm* get_loan_app_installment();
        new_lms_installmentextension_primitive_orm* get_new_lms_installmentextension();
        loan_app_loan_primitive_orm* get_loan_app_loan();
        BlnkTemplateManager* get_template_manager();
        int get_template_id();


        // //static methods
        // static LedgerAmount _calc_short_term_receivable_balance(DisburseLoan *disburseLoan);
        // static LedgerAmount _calc_mer_t_bl_fee(DisburseLoan *disburseLoan);
        // static LedgerAmount _calc_provision_percentage(DisburseLoan *disburseLoan);
        // static LedgerAmount _calc_cashier_fee(DisburseLoan *disburseLoan);
        // static LedgerAmount _calc_bl_t_mer_fee(DisburseLoan *disburseLoan);
        // static LedgerAmount _calc_loan_upfront_fee(DisburseLoan *disburseLoan);
        // static LedgerAmount _calc_long_term_receivable_balance(DisburseLoan *disburseLoan);


        

    ~DisburseLoan();
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
