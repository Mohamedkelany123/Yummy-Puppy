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
#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 60

void printProgress(double percentage) {
    int val = (int) (percentage * 100);
    int lpad = (int) (percentage * PBWIDTH);
    int rpad = PBWIDTH - lpad;
    printf("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
    fflush(stdout);
}

/*
    Tables To Test:

            1-Installment Extention Excluding[ext_created_at , ext_updated_at]
            2-Loan App Loan Excluding[created_at, updated_at]
            3-Installment Excluding[created_at, updated_at]
            4-Installment Late Fees Exclude-ID-(Step-4 Marginalization Creates Late Fees So We cant Compare the [ids] as C++ runs using Multithreads)
            5-Installment Payment Status History Exclude
            6-Installment Status History 
            7-Loan Status History
*/

void print_fieldsMap(map<string, string> map){
    for(const auto& entry : map){
        cout << entry.first << "->" << entry.second << "\t";
    }
}


template <class U, class T> 
bool compare(vector<string> removed_attributes,T iterator_d,T iterator_c, string ordering, string tableName){
    bool flag = false;

    cout << "\nTABLE: "<< tableName << endl;

    iterator_d->setOrderBy(ordering);
    iterator_c->setOrderBy(ordering);
    iterator_d->execute();
    iterator_c->execute();
    cout << "QUERY FETCHED\n";

    int count = iterator_d->getRowCount();
    cout << "DATA COUNT: -->" << count << endl;
    EXPECT_EQ(iterator_d->getRowCount(),iterator_c->getRowCount());

    cout << "Removed Attriutes: ";
    for(int i=0; i< removed_attributes.size(); i++){
        cout << "[" << removed_attributes[i] << "] ";
    }
    cout << endl;

    for( int i=0 ; i<iterator_d->getRowCount() ; i++ ) {
        printProgress((double)i/count);

        if(flag == true){
            break;
        }

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

        for (const auto& entry : fieldsMap_django) {
            if (find(removed_attributes.begin(), removed_attributes.end(), entry.first) == removed_attributes.end()){
                if(entry.second != fieldsMap_c[entry.first]){
                    flag = true;
                    cout << "\nDjango:" << endl;
                    print_fieldsMap(fieldsMap_django);
                    cout << "\nC++:" << endl;
                    print_fieldsMap(fieldsMap_c);
                    cout << endl;
                    break;
                }
            }        
        }
    }
    cout << endl;

    EXPECT_EQ(flag, false);

    return true;
}


//TEST(TestSuiteName, TestCaseName) 
TEST(ClosureTest, LoanAppLoan) {
    string database_d = "7_full_closure_django";
    string database_c = "kemosparc_1";


    psqlController.addDataSource("django",  "192.168.65.203", 5432, database_d, "postgres", "postgres");
    psqlController.addDataSource("c", "192.168.65.203", 5432, database_c, "postgres", "postgres");

    cout << "Database Django: " << database_d << endl;
    cout << "Database c++: " << database_c << endl;


    //------------------------------------LOAN APP LOAN--------------------------------------------------//
    string tableName = "Loan App Loan";

    loan_app_loan_primitive_orm_iterator * lal_django = new loan_app_loan_primitive_orm_iterator("django");
    loan_app_loan_primitive_orm_iterator * lal_c = new loan_app_loan_primitive_orm_iterator("c");
    
    string ordering = "id desc";
    vector<string> removed_attributes = {"orm_updated_at", "orm_created_at","orm_last_lms_closing_day" };
    compare<loan_app_loan_primitive_orm>(removed_attributes,lal_django,lal_c,ordering, tableName);
    delete (lal_c);
    delete (lal_django);

    //------------------------------------INSTALLMENT EXTENTION--------------------------------------------------//
    tableName = "Installment Extention";


    new_lms_installmentextension_primitive_orm_iterator * nli_django = new new_lms_installmentextension_primitive_orm_iterator("django");
    new_lms_installmentextension_primitive_orm_iterator * nli_c = new new_lms_installmentextension_primitive_orm_iterator("c");
    
    ordering = "installment_ptr_id  desc";
    removed_attributes = {"orm_ext_created_at", "orm_ext_updated_at"};
    compare<new_lms_installmentextension_primitive_orm>(removed_attributes,nli_django,nli_c, ordering, tableName);
    delete (nli_django);
    delete (nli_c);

    //------------------------------------INSTALLMENT --------------------------------------------------//
    tableName = "Installment";


    loan_app_installment_primitive_orm_iterator * nli2_django = new loan_app_installment_primitive_orm_iterator("django");
    loan_app_installment_primitive_orm_iterator * nli2_c = new loan_app_installment_primitive_orm_iterator("c");

    ordering = "id desc";
    removed_attributes = {"orm_created_at", "orm_updated_at"};
    compare<loan_app_installment_primitive_orm>(removed_attributes,nli2_django,nli2_c, ordering, tableName);
    delete (nli2_django);
    delete (nli2_c);

    //------------------------------------Installment Late Fees--------------------------------------------------//
    tableName = "Installment Late Fees";

    
    new_lms_installmentlatefees_primitive_orm_iterator * nlilf_django = new new_lms_installmentlatefees_primitive_orm_iterator("django");
    new_lms_installmentlatefees_primitive_orm_iterator * nlilf_c = new new_lms_installmentlatefees_primitive_orm_iterator("c");

    ordering = "installment_extension_id  desc, \"day\" desc, installment_status_id desc";
    removed_attributes = {"orm_id", "orm_created_at", "orm_updated_at"};
    compare<new_lms_installmentlatefees_primitive_orm>(removed_attributes,nlilf_django,nlilf_c, ordering, tableName);
    delete (nlilf_django);
    delete (nlilf_c);

    //------------------------------------Installment Payment Status--------------------------------------------------//
    tableName = "Installment Payment Status History";

    new_lms_installmentpaymentstatushistory_primitive_orm_iterator * nlipsh_django = new new_lms_installmentpaymentstatushistory_primitive_orm_iterator("django");
    new_lms_installmentpaymentstatushistory_primitive_orm_iterator * nlipsh_c = new new_lms_installmentpaymentstatushistory_primitive_orm_iterator("c");
    
    ordering =  "installment_extension_id desc,  \"day\" desc, status desc";
    removed_attributes = {"orm_id", "orm_created_at", "orm_updated_at"};
    compare<new_lms_installmentpaymentstatushistory_primitive_orm>(removed_attributes,nlipsh_django,nlipsh_c, ordering, tableName);
    delete (nlipsh_django);
    delete (nlipsh_c);

    //--------------------------------------------Installment Status History------------------------------------------------------------------//
    
    tableName = "Installment Status History";

    new_lms_installmentstatushistory_primitive_orm_iterator * nlish_django = new new_lms_installmentstatushistory_primitive_orm_iterator("django");
    new_lms_installmentstatushistory_primitive_orm_iterator * nlish_c = new new_lms_installmentstatushistory_primitive_orm_iterator("c");

    ordering =  "installment_id desc, \"day\" desc, status_type desc, status_id desc";
    removed_attributes = {"orm_id", "orm_created_at", "orm_updated_at"};
    compare<new_lms_installmentstatushistory_primitive_orm>(removed_attributes,nlish_django,nlish_c, ordering, tableName);
    delete (nlish_django);
    delete (nlish_c);

    //--------------------------------------------Loan Status History------------------------------------------------------------------//
    
    tableName = "Loan Status History";

    loan_app_loanstatushistroy_primitive_orm_iterator * lalsh_django = new loan_app_loanstatushistroy_primitive_orm_iterator("django");
    loan_app_loanstatushistroy_primitive_orm_iterator * lalsh_c = new loan_app_loanstatushistroy_primitive_orm_iterator("c");

    ordering =  "loan_id desc,previous_status_id desc, \"day\" desc, status_id desc, status_type desc";
    removed_attributes = {"orm_id", "orm_created_at", "orm_updated_at"};
    compare<loan_app_loanstatushistroy_primitive_orm>(removed_attributes,lalsh_django,lalsh_c, ordering, tableName);
    delete (lalsh_django);
    delete (lalsh_c);

}
