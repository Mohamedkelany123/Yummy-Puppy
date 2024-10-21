#include <PSQLController.h>
#include <common.h>
#include <loan_app_loan_primitive_orm.h>
#include <loan_app_installment_primitive_orm.h>
#include <new_lms_installmentextension_primitive_orm.h>

using namespace ostaz;

int main (int argc, char ** argv)
{
    
    string databaseName = "django_ostaz_28082024_sherif";
    int threadsCount = 10;   
    string loan_ids = "";
    int mod_value = 0;
    int offset = 0;
    
    bool connect = psqlController.addDataSource("main","192.168.65.216",5432,databaseName,"development","5k6MLFM9CLN3bD1");
    if (connect){
        cout << "--------------------------------------------------------" << endl;
        cout << "Connected to DATABASE->[" << databaseName << "]" << endl;
        cout << "Threads Count->[" << threadsCount << "]" << endl;      
        cout << "--------------------------------------------------------" << endl;
    }
    

    PSQLJoinQueryIterator * psqlQueryJoin = new PSQLJoinQueryIterator ("main",
    {
        new new_lms_installmentextension_primitive_orm("main",false,true,-1,{"installment_ptr_id", "payment_status"}),
        new loan_app_installment_primitive_orm("main",false,true,-1,{"id", "day"}),
        new loan_app_loan_primitive_orm("main",false,true,-1,{"id", "lms_closure_status"})
    },
    {{{"loan_app_installment","loan_id"},{"loan_app_loan","id"}},{{"loan_app_installment","id"},{"new_lms_installmentextension","installment_ptr_id"}}});



    psqlQueryJoin->filter(
        UnaryOperator ("loan_app_loan.id",lte,"10")
    );

    auto beforeProcess = std::chrono::high_resolution_clock::now();
    psqlQueryJoin->process (threadsCount,[](map <string,PSQLAbstractORM *> * orms,int partition_number,mutex * shared_lock,void * extras) { 
        loan_app_installment_primitive_orm * iorm = ORM(loan_app_installment,orms);
        loan_app_loan_primitive_orm * lal_orm = ORM(loan_app_loan,orms);
        printf("Thread %d, with loan_id: %d, installment_id: %d\n", partition_number, lal_orm->get_id(), iorm->get_id());
    });

    auto afterProcess = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(afterProcess - beforeProcess);
    printf("Time took: %.3f\n", elapsed.count() * 1e-9);

    psqlController.ORMCommit(true,true,true, "main");   

    delete (psqlQueryJoin);
}
