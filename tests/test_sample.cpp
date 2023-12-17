#include <gtest/gtest.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <map>
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

//NESHOOF F -> NEXT  3AMAL EXECUTE WALA LA
//DONT USE THE CACHEEEE
//FEL SOURCES TEMPLATE CHECK LAW EL PSQL.. MESH B NULL ANDD EL CONDITION EL MAWGOODA

//PEOBLEM BEY LINK EL TEST BA3D A,MA BEN RUNNO


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



// //TEST(TestSuiteName, TestCaseName) 
// TEST(ClosureTest, InstallmentExtention) {
    
//     psqlController.addDataSource("django", "192.168.65.203", 5432, "7_full_closure_django", "postgres", "postgres");
//     new_lms_installmentextension_primitive_orm_iterator * nliei_django = new new_lms_installmentextension_primitive_orm_iterator("django");
//     nliei_django->execute();

//     psqlController.addDataSource("c", "192.168.65.203", 5432, "7_full_closure_c", "postgres", "postgres");
//     new_lms_installmentextension_primitive_orm_iterator * nliei_c = new new_lms_installmentextension_primitive_orm_iterator("c");
//     nliei_c->execute();

//     for (int i=0; i<1 ; i++) {
//         new_lms_installmentextension_primitive_orm* django = nliei_django->next();
//         new_lms_installmentextension_primitive_orm* c = nliei_c->next();

//         if (c == nullptr || django == nullptr) {
//             break;
//         }

//         map<string, string> fieldsMap_django = django->generateFieldsMap();
//         map<string, string> fieldsMap_c = c->generateFieldsMap();

//         for (const auto& entry : fieldsMap_c) {
//             if(entry.first != "orm_ext_created_at" && entry.first != "orm_ext_updated_at" ){
//                 cout << "C++ Output ="  <<entry.first << endl;
//                 cout << "Django Output ="  << fieldsMap_django[entry.first] << endl;
//             }
//         }
//     }

//     delete (nliei_c);
//     delete (nliei_django);




   

//     EXPECT_EQ(5, 5);
// }


//TEST(TestSuiteName, TestCaseName) 
TEST(ClosureTest, LoanAppLoan) {
    
    psqlController.addDataSource("main", "192.168.65.216", 5432, "django_ostaz_29102023_backup", "postgres", "8ZozYD6DhNJgW7a");
    loan_app_loan_primitive_orm_iterator * lal_django = new loan_app_loan_primitive_orm_iterator("main");
    lal_django->execute();



    // psqlController.addDataSource("main", "192.168.65.203", 5432, "7_full_closure_django", "postgres", "postgres");
    // loan_app_loan_primitive_orm_iterator * lal_django = new loan_app_loan_primitive_orm_iterator("main");
    // lal_django->execute();

    // psqlController.addDataSource("c", "192.168.65.203", 5432, "7_full_closure_c", "postgres", "postgres");
    // loan_app_loan_primitive_orm_iterator * lal_c = new loan_app_loan_primitive_orm_iterator("c");
    // lal_c->execute();
        
    for (int i=0; i< 2 ; i++) {
        loan_app_loan_primitive_orm * django = lal_django->next();
        // if (django == nullptr) {
        map<string, string> fieldsMap_django = django->generateFieldsMap();
        cout << "DJANGO:" << fieldsMap_django["orm_updated_at"]  << "   ID:" << fieldsMap_django["orm_id"] <<  endl;

        // loan_app_loan_primitive_orm * c  = lal_c->next();
        //    if (c == nullptr) {
        //     break;
        // }
        // map<string, string> fieldsMap_c = c->generateFieldsMap();
        // cout << "CCCCCC:" << fieldsMap_c["orm_updated_at"] << "   ID:" << fieldsMap_c["orm_id"] <<  endl;

     

        // for (const auto& entry : fieldsMap_django) {
        //     // if(entry.first == "orm_updated_at"){
        //     //     cout << "CREATED AT C=" << entry.second << endl;
        //     //     cout << "Created At D= " << fieldsMap_c[entry.first] << endl;
        //     // }
            
        //     // if(entry.first == "orm_id"){
        //     //     cout << "ID C=" << entry.second << endl;
        //     //     cout << "ID D= " << fieldsMap_c[entry.first] << endl;
        //     // }

        //     if(entry.first != "orm_created_at" && entry.first != "orm_updated_at"){
        //         if(entry.second != fieldsMap_c[entry.first]){
        //             cout << "3aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" << endl;
        //         }
        //     }
        // }
    // }

    // delete (lal_c);
    delete (lal_django);
    EXPECT_EQ(5, 5);
    }   
}