#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <PSQLController.h>
#include <PSQLUpdateQuery.h>
// #include <loan_app_loan_primitive_orm.h>
// #include <loan_app_installment_primitive_orm.h>
// #include <new_lms_installmentextension_primitive_orm.h>
#include <crm_app_customer_primitive_orm.h>
#include <new_lms_installmentlatefees_primitive_orm.h>
#include <loan_app_loanstatushistroy_primitive_orm.h>
#include <new_lms_installmentstatushistory_primitive_orm.h>
#include <loan_app_provision_primitive_orm.h>
#include <loan_app_loanproduct_primitive_orm.h>
// #include <loan_app_loanstatus_primitive_orm.h>
#include <DiburseLoans.h>




//enum closure_status { START,UNDUE_TO_DUE, DUE_TO_OVERDUE, UPDATE_LOAN_STATUS, MARGINALIZE_INCOME_STEP1,MARGINALIZE_INCOME_STEP2,MARGINALIZE_INCOME_STEP3,LONG_TO_SHORT_TERM,LAST_ACCRUED_DAY,PREPAID_TRANSACTION };

extern "C" int main_closure (char* address, int port, char* database_name, char* username, char* password, char* step, char* closure_date_string, int threadsCount, int mod_value, int offset, char* loan_ids="");




int main (int argc, char ** argv) {
    if (argc < 11 || argc > 12)
    {
        printf("usage: %s <address> <port_number> <database name> <username> <password> <step> <date>YYYY-mm-dd <threads count> <mod> <offset> <loan ids comma-seperated>\n",argv[0]);
        exit(12);
    }

    bool isLoanSpecific = argc >= 12; 
    char* loan_ids = "";
    if (isLoanSpecific) {
        loan_ids = argv[11];
    }

    return main_closure(argv[1],atoi(argv[2]),argv[3],argv[4],argv[5],argv[6],argv[7],stoi(argv[8]),stoi(argv[9]),stoi(argv[10]),loan_ids);
}



// extern "c" not garbling function names
extern "C" int main_closure (char* address, int port, char* database_name, char* username, char* password, char* step, char* closure_date_string, int threadsCount, int mod_value, int offset, char* loan_ids)
{   
    bool isLoanSpecific = loan_ids != ""; 
    if (isLoanSpecific){
        cout << "Loan ids to close: " << loan_ids << endl;
    }

    bool isMultiMachine = mod_value > 0; 

    psqlController.addDataSource("main",address,port,database_name,username,password);
    cout << "Connected to " << database_name << endl;
    
    psqlController.addDefault("created_at","now()",true,true);
    psqlController.addDefault("updated_at","now()",true,true);
    psqlController.addDefault("updated_at","now()",false,true);
    psqlController.setORMCacheThreads(threadsCount);
    BDate closure_date(closure_date_string);


    // Modify this to reset ledger closure status
    PSQLUpdateQuery psqlUpdateQuery ("main","loan_app_loan",
        ANDOperator(
            new UnaryOperator ("loan_app_loan.id",ne,"14312"),
            isMultiMachine ? new BinaryOperator ("loan_app_loan.id",mod,mod_value,eq,offset) : new BinaryOperator(),
            isLoanSpecific ? new UnaryOperator ("loan_app_loan.id", in, loan_ids) : new UnaryOperator()
        ),
        {{"closure_status",to_string(ledger_status::LEDGER_START)}}
        );
    psqlUpdateQuery.update();   

if ( strcmp (step,"disburse_loan") == 0 || strcmp (step,"full_closure") == 0 )
    {

        auto begin = std::chrono::high_resolution_clock::now();

        PSQLJoinQueryIterator * psqlQueryJoin = new PSQLJoinQueryIterator ("main",
        {new loan_app_loan_primitive_orm("main"),new loan_app_loanproduct_primitive_orm("main")},
        {{{"loan_app_loanproduct","id"},{"loan_app_loan","loan_product_id"}}});


        // psqlQueryJoin->addExtraFromField("(SELECT SUM(lai.principal_expected) FROM loan_app_installment lai INNER JOIN new_lms_installmentextension nli on nli.installment_ptr_id  = lai.id where nli.is_long_term = false and loan_app_loan.id = lai.loan_id)","short_term_principal");
        // psqlQueryJoin->addExtraFromField("(SELECT SUM(lai.principal_expected) FROM loan_app_installment lai INNER JOIN new_lms_installmentextension nli on nli.installment_ptr_id  = lai.id where nli.is_long_term = true and loan_app_loan.id = lai.loan_id)","long_term_principal");


        
        psqlQueryJoin->filter(
            ANDOperator 
            (
                new UnaryOperator ("loan_app_loan.closure_status",eq,to_string(ledger_status::DISBURSE_LOAN-1)),
                new UnaryOperator ("loan_app_loan.loan_creation_ledger_entry_id",isnull,"",true),
                new UnaryOperator ("loan_app_loan.loan_booking_day",lte,closure_date_string),
                isMultiMachine ? new BinaryOperator ("loan_app_loan.id",mod,mod_value,eq,offset) : new BinaryOperator(),
                isLoanSpecific ? new UnaryOperator ("loan_app_loan.id", in, loan_ids) : new UnaryOperator()
            )
        );

        auto beforeProcess = std::chrono::high_resolution_clock::now();
        
        cout << "THREADTIME --> disburse_loan" << endl;


         
        float percentage = get_provisions_percentage();

        psqlQueryJoin->process (threadsCount,[percentage](map <string,PSQLAbstractORM *> * orms,int partition_number,mutex * shared_lock) {
                cout << "INsidee processsssssss" << endl;
                loan_app_loan_primitive_orm * lal_orm = ORM(loan_app_loan,orms);
                cout << percentage << endl;
                cout << lal_orm->get_id() << endl;


        });

        auto afterProcess = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(afterProcess - beforeProcess);
        printf("PROCESSTIME undue_to_due->: %.3f seconds.\n", elapsed.count() * 1e-9);

        cout << "processed " << psqlQueryJoin->getResultCount() << " record(s)" << endl;
        // psqlController.ORMCommit(true,true,true, "main");   

        auto afterCommit = std::chrono::high_resolution_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(afterCommit - afterProcess);
        printf("COMMITTIME undue_to_due->: %.3f seconds.\n", elapsed.count() * 1e-9);


        delete (psqlQueryJoin);

        // Modify this to update closure
        PSQLUpdateQuery psqlUpdateQuery ("main","loan_app_loan",
        OROperator (
            new UnaryOperator ("loan_app_loan.lms_closure_status",isnull,"",true),
            new ANDOperator (
                    new UnaryOperator ("loan_app_loan.lms_closure_status",lt,ledger_status::LEDGER_START),
                    new UnaryOperator ("loan_app_loan.id",ne,"14312"),
                    new UnaryOperator ("loan_app_loan.lms_closure_status",eq,0),
                    isMultiMachine ? new BinaryOperator ("loan_app_loan.id",mod,mod_value,eq,offset) : new BinaryOperator(),
                    isLoanSpecific ? new UnaryOperator ("loan_app_loan.id", in, loan_ids) : new UnaryOperator()  
            )          
        ),
        {{"lms_closure_status",to_string(ledger_status::DISBURSE_LOAN)}}
        );
        psqlUpdateQuery.update();

        cout << "Undue to Due done" << endl;

        // Stop measuring time and calculate the elapsed time
        auto end = std::chrono::high_resolution_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
        printf("Total time-> %.3f seconds.\n", elapsed.count() * 1e-9);
    }

    return 0;

}
