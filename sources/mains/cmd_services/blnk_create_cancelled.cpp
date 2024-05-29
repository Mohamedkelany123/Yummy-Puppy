#include <PSQLController.h>
#include <TemplateManager.h>
#include <DisburseLoans.h>
#include <common_orm.h>
#include <common.h>
#include <Disbursefunc.h>
#include <CancelFunc.h>


//TODO: create special type for 
// Testing make file
//<BuckedId,Percentage>
map<int,float> get_loan_status_provisions_percentage()
{
        //Query to return percentage from loan_app_provision
        PSQLJoinQueryIterator * psqlQueryJoinProvisions = new PSQLJoinQueryIterator ("main",
        {new loan_app_loanstatus_primitive_orm("main"),new loan_app_provision_primitive_orm("main")},
        {{{"loan_app_loanstatus","id"},{"loan_app_provision","status_id"}}});

        
        map<int,float> bucket_percentage;

        psqlQueryJoinProvisions->execute();
        map<string, PSQLAbstractORM *>* orms = psqlQueryJoinProvisions->next(true);
        loan_app_loanstatus_primitive_orm * lals_orm;
        loan_app_provision_primitive_orm * lap_orm; 
        while (orms != NULL){
            lals_orm = ORM(loan_app_loanstatus,orms);
            lap_orm = ORM(loan_app_provision,orms);
            bucket_percentage[lals_orm->get_id()] = lap_orm->get_percentage();
            delete(lals_orm);
            delete(lap_orm);
            delete(orms);
            orms = psqlQueryJoinProvisions->next(true);
        }
        delete (psqlQueryJoinProvisions);
        

        return bucket_percentage;
}


int main (int argc, char ** argv)
{

    int threadsCount = 10;
    bool connect = psqlController.addDataSource("main","192.168.1.51",5432,"c_plus_plus","postgres","postgres");
    if (connect){
        cout << "Connected to DATABASE"  << endl;
    }
    psqlController.addDefault("created_at","now()",true,true);
    psqlController.addDefault("updated_at","now()",true,true);
    psqlController.addDefault("updated_at","now()",false,true);
    psqlController.setORMCacheThreads(threadsCount);


    // PSQLJoinQueryIterator * psqlQueryJoin = new PSQLJoinQueryIterator ("main",
    // {new loan_app_loan_bl_orm("main"),new loan_app_loanproduct_primitive_orm("main"), new crm_app_customer_primitive_orm("main"), new crm_app_purchase_primitive_orm("main")},
    // {{{"loan_app_loanproduct","id"},{"loan_app_loan","loan_product_id"}}, {{"loan_app_loan", "id"}, {"crm_app_purchase", "loan_id"}}, {{"loan_app_loan", "customer_id"}, {"crm_app_customer", "id"}}});

    // psqlQueryJoin->addExtraFromField("(SELECT SUM(lai.principal_expected) FROM loan_app_installment lai INNER JOIN new_lms_installmentextension nli on nli.installment_ptr_id  = lai.id where nli.is_long_term = false and loan_app_loan.id = lai.loan_id)","short_term_principal");
    // psqlQueryJoin->addExtraFromField("(SELECT SUM(lai.principal_expected) FROM loan_app_installment lai INNER JOIN new_lms_installmentextension nli on nli.installment_ptr_id  = lai.id where nli.is_long_term = true and loan_app_loan.id = lai.loan_id)","long_term_principal");
    // psqlQueryJoin->addExtraFromField("(SELECT cap2.is_rescheduled FROM crm_app_purchase cap INNER JOIN crm_app_purchase cap2 ON cap.parent_purchase_id = cap2.id WHERE  cap.id = crm_app_purchase.id)","is_rescheduled");

    // string closure_date_string = "2024-05-15"; 
    
    // psqlQueryJoin->filter(
    //     ANDOperator 
    //     (
    //         new UnaryOperator ("loan_app_loan.closure_status",eq,to_string(ledger_status::DISBURSE_LOAN-1)),
    //         new UnaryOperator ("loan_app_loan.loan_creation_ledger_entry_id",isnull,"",true),
    //         new UnaryOperator ("loan_app_loan.loan_booking_day",lte,closure_date_string)
    //     )
    // );

    // BlnkTemplateManager * blnkTemplateManager = new BlnkTemplateManager(4);
    // map<int,float> status_provision_percentage =  get_loan_status_provisions_percentage();

    // DisburseLoanStruct disburseLoanStruct;
    // disburseLoanStruct.blnkTemplateManager = blnkTemplateManager;
    // disburseLoanStruct.current_provision_percentage = status_provision_percentage[1];

    // psqlQueryJoin->process(threadsCount, DisburseLoanFunc,(void *)&disburseLoanStruct);


    // delete(blnkTemplateManager);
    // delete(psqlQueryJoin);
    // psqlController.ORMCommit(true,true,true, "main");  

    //  -------------------------------------------- setp 2 --------------------------------------------
    // cout<<"hereeeerrrrrr at first"<<endl;
    string closure_date_string = "2024-06-01"; 

    loan_app_loan_bl_orm_iterator *  psqlQueryJoin = new loan_app_loan_bl_orm_iterator ("main");

    psqlQueryJoin->addExtraFromField("(select count(*)>0 from loan_app_loanstatushistroy lal where lal.status_id in (12,13) and lal.day::date <= \'"+ closure_date_string +"\' and lal.loan_id = loan_app_loan.id)","is_included");
    psqlQueryJoin->addExtraFromField("(select distinct lal.day from loan_app_loanstatushistroy lal where lal.status_id in (12,13) and lal.loan_id = loan_app_loan.id)","cancellation_day");
    psqlQueryJoin->filter(
        ANDOperator 
        (
    //         // new UnaryOperator ("loan_app_loan.closure_status",eq,to_string(ledger_status::CANCEL_LOAN-1)),
            new UnaryOperator ("loan_app_loan.cancel_ledger_entry_id",isnull,"",true),
            new UnaryOperator ("loan_app_loan.loan_booking_day",lte,closure_date_string),
            new UnaryOperator ("loan_app_loan.status_id",in,"12,13")

        )
    );
    // cout<<"after query"<<endl;

    CancelLoanStruct cancelLoanStruct;

    // cout << "1" << endl;
    BlnkTemplateManager *  blnkTemplateManager_cancel = new BlnkTemplateManager(5, -1);
    // cout << "2" << endl;
    cancelLoanStruct.blnkTemplateManager_cancel = blnkTemplateManager_cancel;
    // cout << "3" << endl;
     BlnkTemplateManager * blnkTemplateManager_reverse = new BlnkTemplateManager(6, -1);
    // cout << "4" << endl;
    cancelLoanStruct.blnkTemplateManager_reverse = blnkTemplateManager_reverse;
    // cout << "5" << endl;
    
    // cout<<"before processs   "<<endl;

    psqlQueryJoin->process(threadsCount, CancelLoanFunc,(void *)&cancelLoanStruct);
    // cout<<"after processs"<<endl;


    delete(blnkTemplateManager_cancel);
        cout<<"delete cancel"<<endl;

    delete(blnkTemplateManager_reverse);
    cout<<"delete reverse "<<endl;

    delete(psqlQueryJoin);
        cout<<"delete iterator "<<endl;
    psqlController.ORMCommit(true,true,true, "main"); 
  return 0;
}
