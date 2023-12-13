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


TEST(AddTest, PositiveNumbers) {
    
    PSQLConnectionManager psqlConnectionManager;
    psqlConnectionManager.addDataSource("main", "192.168.65.203", 5432, "database", "postgres", "postgres");


    PSQLJoinQueryIterator *  psqlQueryJoin = new PSQLJoinQueryIterator ("main",
        {new new_lms_installmentextension_primitive_orm(),new loan_app_installment_primitive_orm(),new loan_app_loan_primitive_orm(),new crm_app_customer_primitive_orm()},
        {{{"loan_app_installment","loan_id"},{"loan_app_loan","id"}},
        {{"loan_app_installment","id"},{"new_lms_installmentextension","installment_ptr_id"}},
        {{"crm_app_customer","id"},{"loan_app_loan","customer_id"}}
        });

        psqlQueryJoin->filter(
            ANDOperator 
            (
                new UnaryOperator ("new_lms_installmentextension.is_marginalized",eq,"f"),
                new UnaryOperator ("new_lms_installmentextension.payment_status",in,"0,4,5")
            )
        );

    // Process the results and print information
    psqlQueryJoin->process(1, [](map<string, PSQLAbstractORM *> *orms, int partition_number, mutex *shared_lock) {
        for (auto &orm_pair : *orms) {
            new_lms_installmentextension_primitive_orm * ie_orm  = ORM(new_lms_installmentextension,orms);
            
            // Replace the following lines with your specific field names and getter methods
            cout << "ID: " << ie_orm->get_installment_ptr_id() << endl;
            cout << "HIIIIIIIIIIIIIII"  << endl;

        }
    });

    delete (psqlQueryJoin);


    EXPECT_EQ(5, 5);
}







// // Run the tests
// int main(int argc, char** argv) {
//     ::testing::InitGoogleTest(&argc, argv);
//     return RUN_ALL_TESTS();
// }
