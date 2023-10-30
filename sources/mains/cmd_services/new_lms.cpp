#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <PSQLController.h>
#include <loan_app_loan_primitive_orm.h>
#include <loan_app_installment_primitive_orm.h>
#include <new_lms_installmentextension_primitive_orm.h>
#include <crm_app_customer_primitive_orm.h>




int main (int argc, char ** argv)
{
    psqlController.addDataSource("main",argv[1],atoi(argv[2]),argv[3],argv[4],argv[5]);
    PSQLJoinQueryIterator * psqlQueryJoin = new PSQLJoinQueryIterator ("main",
    {new new_lms_installmentextension_primitive_orm(),new loan_app_installment_primitive_orm(),new loan_app_loan_primitive_orm()},
    {{{"loan_app_installment","loan_id"},{"loan_app_loan","id"}},{{"loan_app_installment","id"},{"new_lms_installmentextension","installment_ptr_id"}}});

    psqlQueryJoin->filter(
        ANDOperator 
        (
            new UnaryOperator ("new_lms_installmentextension.undue_to_due_date",lte,"2023-11-15"),
            new UnaryOperator ("new_lms_installmentextension.payment_status",eq,"5"),
            new UnaryOperator ("loan_app_loan.lms_closure_status",eq,"0"),
            new UnaryOperator ("loan_app_loan.status_id",nin,"6, 7, 8, 12, 13, 15")
        )
    );
    psqlQueryJoin->process (10,[](map <string,PSQLAbstractORM *> * orms,int partition_number,mutex * shared_lock) { 
            ORM(new_lms_installmentextension,orms)->set_payment_status(50);            
    });
    cout << "I m done with the lambda" << endl;
    psqlController.ORMCommit();   
    return 0;
}