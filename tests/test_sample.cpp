#include <gtest/gtest.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <PSQLController.h>
#include <PSQLUpdateQuery.h>
#include <PSQLAbstractQueryIterator.h>
#include <loan_app_loan_primitive_orm.h>
#include <loan_app_installment_primitive_orm.h>
#include <new_lms_installmentextension_primitive_orm.h>
#include <crm_app_customer_primitive_orm.h>
#include <new_lms_installmentlatefees_primitive_orm.h>
#include <loan_app_loanstatushistroy_primitive_orm.h>
#include <new_lms_installmentstatushistory_primitive_orm.h>

/* 
    Tables To Test:

            1-Installment Extention Excluding[ext_created_at , ext_updated_at]
    [FAILED]2-Loan App Loan Excluding[created_at, updated_at]
            3-Installment Excluding[created_at, updated_at]
            4-Installment Late Fees Exclude-ID-(Step-4 Marginalization Creates Late Fees So We cant Compare the [ids] as C++ runs using Multithreads)
            5-Installment Payment Status History Exclude
            6-Installment Status History 
            7-Loan Status History
*/



//TEST(TestSuiteName, TestCaseName) 
TEST(ClosureTest, InstallmentExtention) {
    
    // PSQLConnectionManager psqlConnectionManager;
    // psqlConnectionManager.addDataSource("main", "192.168.65.203", 5432, "7_full_closure_c", "postgres", "postgres");
    // psqlController.addDataSource("main", "192.168.65.203", 5432, "7_full_closure_c", "postgres", "postgres");
    psqlController.addDataSource("main", "127.0.0.1", 5432, "django_ostaz_25102023", "postgres", "postgres");


    new_lms_installmentextension_primitive_orm_iterator * nliei_old = new new_lms_installmentextension_primitive_orm_iterator("main_old");
    new_lms_installmentextension_primitive_orm_iterator * nliei_new = new new_lms_installmentextension_primitive_orm_iterator("main_new");
    nliei_old->execute();
    nliei_new->execute();
    for ( ;  ; )
    {
        new_lms_installmentextension_primitive_orm * x_old = nliei_old->next();
        new_lms_installmentextension_primitive_orm * x_new = nliei_new->next();
        x_old->generateFieldsMap()
        jsonb x_old->compare(x_new);
        if (x == NULL ) break;
        cout << x->get_installment_ptr_id() << endl;

    }
    delete (nliei);

    // PSQLJoinQueryIterator *  psqlQueryJoin = new PSQLJoinQueryIterator ("main",
    //     {new new_lms_installmentextension_primitive_orm(),new loan_app_installment_primitive_orm(),new loan_app_loan_primitive_orm(),new crm_app_customer_primitive_orm()},
    //     {{{"loan_app_installment","loan_id"},{"loan_app_loan","id"}},
    //     {{"loan_app_installment","id"},{"new_lms_installmentextension","installment_ptr_id"}},
    //     {{"crm_app_customer","id"},{"loan_app_loan","customer_id"}}
    //     });

    //     psqlQueryJoin->filter(
    //         ANDOperator 
    //         (
    //             new UnaryOperator ("new_lms_installmentextension.is_marginalized",eq,"f"),
    //             // new UnaryOperator ("new_lms_installmentextension.payment_status",in,"0,4,5"),
    //             new UnaryOperator ("new_lms_installmentextension.installment_ptr_id",lte,"1000")
    //         )
    //     );

    // // Process the results and print information
    // psqlQueryJoin->process (10,[](map <string,PSQLAbstractORM *> * orms,int partition_number,mutex * shared_lock) { 
    //     new_lms_installmentextension_primitive_orm * ieorm = ORM(new_lms_installmentextension,orms);
    //     loan_app_loan_primitive_orm * lal_orm = ORM(loan_app_loan,orms);
    //     cout << "hiiiii" << endl;
    //     });
    // cout << "C++ Results:" << psqlQueryJoin->getResultCount() << " record(s)" << endl;

    // delete (psqlQueryJoin);


    EXPECT_EQ(5, 5);
}







// // Run the tests
// int main(int argc, char** argv) {
//     ::testing::InitGoogleTest(&argc, argv);
//     return RUN_ALL_TESTS();
// }
