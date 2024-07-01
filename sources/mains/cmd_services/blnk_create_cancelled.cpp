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

    string closure_date_string = "2024-06-01"; 

    PSQLJoinQueryIterator * psqlQueryJoin = new PSQLJoinQueryIterator ("main",
    {new loan_app_loan_bl_orm("main"), new ledger_amount_primitive_orm("main")},
    {{{"loan_app_loan","id"},{"ledger_amount","loan_id"}}});

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

    psqlQueryJoin->setOrderBy("loan_app_loan.id asc, ledger_amount.id asc");
    psqlQueryJoin->setAggregates ({
        {"loan_app_loan",{"id", 1}}
    });

    CancelLoanStruct cancelLoanStruct;

    BlnkTemplateManager *  blnkTemplateManager_cancel = new BlnkTemplateManager(5, -1);
    cancelLoanStruct.blnkTemplateManager_cancel = blnkTemplateManager_cancel;
     BlnkTemplateManager * blnkTemplateManager_reverse = new BlnkTemplateManager(6, -1);
    cancelLoanStruct.blnkTemplateManager_reverse = blnkTemplateManager_reverse;
    

    psqlQueryJoin->process_aggregate(threadsCount, CancelLoanFunc,(void *)&cancelLoanStruct);


    delete(blnkTemplateManager_cancel);

    delete(blnkTemplateManager_reverse);

    delete(psqlQueryJoin);
    psqlController.ORMCommit(true,true,true, "main"); 
  return 0;
}
