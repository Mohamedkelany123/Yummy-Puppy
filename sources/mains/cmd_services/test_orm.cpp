#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <PSQLController.h>
#include <PSQLUpdateQuery.h>
#include <loan_app_loan_primitive_orm.h>
#include <loan_app_installment_primitive_orm.h>
#include <new_lms_installmentextension_primitive_orm.h>
#include <crm_app_customer_primitive_orm.h>
#include <new_lms_installmentlatefees_primitive_orm.h>
#include <loan_app_loanstatushistroy_primitive_orm.h>
#include <new_lms_installmentstatushistory_primitive_orm.h>
int main (int argc, char ** argv)
{   
    if (argc < 11 || argc > 12)
    {
        printf("usage: %s <address> <port_number> <database name> <username> <password> <step> <date>YYYY-mm-dd <threads count> <mod> <offset> <loan ids comma-seperated>\n",argv[0]);
        exit(12);
    }
    bool isLoanSpecific = argc >= 12; 
    string loan_ids = "";
    if (isLoanSpecific){
        loan_ids = argv[11];
        cout << "Loan ids to close: " << loan_ids << endl;
    }

    int mod_value = std::stoi(argv[9]);
    int offset = std::stoi(argv[10]);

    bool isMultiMachine = mod_value > 0; 


    int threadsCount = std::stoi(argv[8]);
    //2023-11-
    string closure_date_string = argv[7];
    psqlController.addDataSource("main",argv[1],atoi(argv[2]),argv[3],argv[4],argv[5]);
    psqlController.addDefault("created_at","now()",true,true);
    psqlController.addDefault("updated_at","now()",true,true);
    psqlController.addDefault("updated_at","now()",false,true);
    psqlController.setORMCacheThreads(threadsCount);
    BDate closure_date(closure_date_string);

     PSQLJoinQueryIterator * psqlQueryJoin = new PSQLJoinQueryIterator ("main",
        {new new_lms_installmentextension_primitive_orm("main"),
        new loan_app_installment_primitive_orm("main"),
        new loan_app_loan_primitive_orm("main")},
        {{{"loan_app_installment","loan_id"},{"loan_app_loan","id"}},
        {{"loan_app_installment","id"},{"new_lms_installmentextension","installment_ptr_id"}}});
        psqlQueryJoin->addExtraFromField("(select count(*) from new_lms_installmentlatefees where installment_extension_id=new_lms_installmentextension.installment_ptr_id)","late_fees_count");
        psqlQueryJoin->addExtraFromField("(select max(day) from new_lms_installmentlatefees where installment_extension_id=new_lms_installmentextension.installment_ptr_id)","late_fees_date");


        psqlQueryJoin->filter(
            ANDOperator 
            (
                new UnaryOperator ("new_lms_installmentextension.due_to_overdue_date",lte,closure_date_string),
                new UnaryOperator ("new_lms_installmentextension.is_interest_paid",eq,"f"),
                new UnaryOperator ("new_lms_installmentextension.payment_status",in,"0,4"),
                // new UnaryOperator ("loan_app_loan.lms_closure_status",eq,to_string(closure_status::DUE_TO_OVERDUE-1)),
                new UnaryOperator ("loan_app_loan.status_id",nin,"6, 7, 8, 12, 13, 15, 16"),
                isMultiMachine ? new BinaryOperator ("loan_app_loan.id",mod,mod_value,eq,offset) : new BinaryOperator(),
                isLoanSpecific ? new UnaryOperator ("loan_app_loan.id", in, loan_ids) : new UnaryOperator() 
            )
        );

        psqlQueryJoin->process (threadsCount,[&closure_date](map <string,PSQLAbstractORM *> * orms,int partition_number,mutex * shared_lock,void * extras) {
            shared_lock->lock();
            new_lms_installmentextension_primitive_orm * ieorm = ORM(new_lms_installmentextension,orms);
            PSQLGeneric_primitive_orm * gorm = ORM(PSQLGeneric,orms);
            cout << ieorm->get_installment_ptr_id() << " --- " << gorm->get("late_fees_count")<< " --- " << gorm->get("late_fees_date")<< endl;
            // cout << ieorm->get_installment_ptr_id() << endl;
            shared_lock->unlock();

        });

    return 0;
}