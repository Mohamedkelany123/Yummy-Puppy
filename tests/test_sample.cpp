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

template <class T, class U> 
bool compare(list<string> removed_attributes){
    T * iterator_d = new T(std::string("django"));
    T * iterator_c = new T(std::string("c"));


    iterator_d->execute();
    iterator_c->execute();
    cout << "QUERY FETCHED\n";

    while(true) {
        U * django = iterator_d->next();
        if (django == nullptr) {
            break;
        }
        U * c  = iterator_c->next();
        if (c == nullptr) {
            break;
        }
        map<string, string> fieldsMap_django = django->generateFieldsMap();
        map<string, string> fieldsMap_c = c->generateFieldsMap();

        cout << "ID:" << fieldsMap_c["orm_id"];


        for (const auto& entry : fieldsMap_django) {
            if (find(removed_attributes.begin(), removed_attributes.end(), entry.first) == removed_attributes.end()){
                // cout << "\nHAMDAAAAA:-->" << fieldsMap_django["orm_id"];
                EXPECT_EQ(entry.second, fieldsMap_c[entry.first]);
            }        
        }
    }
    delete (iterator_d);
    delete (iterator_c);

}


//TEST(TestSuiteName, TestCaseName) 
TEST(ClosureTest, LoanAppLoan) {
    
    psqlController.addDataSource("django",  "192.168.65.203", 5432, "7_full_closure_django", "postgres", "postgres");
    loan_app_loan_primitive_orm_iterator * lal_django = new loan_app_loan_primitive_orm_iterator("django");

    psqlController.addDataSource("c", "192.168.65.203", 5432, "11_step5", "postgres", "postgres");
    loan_app_loan_primitive_orm_iterator * lal_c = new loan_app_loan_primitive_orm_iterator("c");
    
    list<string> removed_attributes = {"orm_updated_at", "orm_created_at","orm_last_lms_closing_day" };

    // compare<loan_app_loan_primitive_orm_iterator * ,loan_app_loan_primitive_orm>(removed_attributes);

    lal_django->execute();
    lal_c->execute();
    cout << "QUERY FETCHED\n";

    while(true) {
        loan_app_loan_primitive_orm * django = lal_django->next();
        if (django == nullptr) {
            break;
        }
        loan_app_loan_primitive_orm * c  = lal_c->next();
        if (c == nullptr) {
            break;
        }
        map<string, string> fieldsMap_django = django->generateFieldsMap();
        map<string, string> fieldsMap_c = c->generateFieldsMap();

        // cout << "ID:" << fieldsMap_c["orm_id"];


        for (const auto& entry : fieldsMap_django) {
            if (find(removed_attributes.begin(), removed_attributes.end(), entry.first) == removed_attributes.end()){
                // cout << "\nHAMDAAAAA:-->" << fieldsMap_django["orm_id"];
                EXPECT_EQ(entry.second, fieldsMap_c[entry.first]);
            }        
        }
    }

    delete (lal_c);
    delete (lal_django);
}



