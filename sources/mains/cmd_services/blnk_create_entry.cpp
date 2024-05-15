#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <PSQLController.h>
#include <TemplateManager.h>
#include <lms_entrytemplate_primitive_orm.h>
#include <loan_app_loanstatus_primitive_orm.h>
//TODO: create special type for 

int main (int argc, char ** argv)
{

    int threadsCount = 1;
    bool connect = psqlController.addDataSource("main","192.168.65.216",5432,"django_ostaz_30042024_omneya","postgres","8ZozYD6DhNJgW7a");
    if (connect){
        cout << "Connected to DATABASE"  << endl;
    }
    psqlController.addDefault("created_at","now()",true,true);
    psqlController.addDefault("updated_at","now()",true,true);
    psqlController.addDefault("updated_at","now()",false,true);
    psqlController.setORMCacheThreads(threadsCount);


    PSQLJoinQueryIterator * psqlQueryJoin = new PSQLJoinQueryIterator ("main",
    {new loan_app_loan_primitive_orm("main"),new loan_app_loanproduct_primitive_orm("main")},
    {{{"loan_app_loanproduct","id"},{"loan_app_loan","loan_product_id"}}});
    // PSQLJoinQueryIterator * psqlQueryJoin = new PSQLJoinQueryIterator ("main",
    // {new loan_app_loan_primitive_orm("main"),new loan_app_installment_primitive_orm("main")},
    // {{{"loan_app_installment","loan_id"},{"loan_app_loan","id"}}});

    // psqlQueryJoin->addExtraFromField("(SELECT SUM(lai.principal_expected) FROM loan_app_installment lai INNER JOIN new_lms_installmentextension nli on nli.installment_ptr_id  = lai.id where nli.is_long_term = false and loan_app_loan.id = lai.loan_id)","short_term_principal");
    // psqlQueryJoin->addExtraFromField("(SELECT SUM(lai.principal_expected) FROM loan_app_installment lai INNER JOIN new_lms_installmentextension nli on nli.installment_ptr_id  = lai.id where nli.is_long_term = true and loan_app_loan.id = lai.loan_id)","long_term_principal");

    string closure_date_string = "2024-05-15"; 
    
    psqlQueryJoin->filter(
        ANDOperator 
        (
            new UnaryOperator ("loan_app_loan.closure_status",eq,to_string(ledger_status::DISBURSE_LOAN-1)),
            new UnaryOperator ("loan_app_loan.loan_creation_ledger_entry_id",isnull,"",true),
            new UnaryOperator ("loan_app_loan.loan_booking_day",lte,closure_date_string)
            // isMultiMachine ? new BinaryOperator ("loan_app_loan.id",mod,mod_value,eq,offset) : new BinaryOperator(),
            // isLoanSpecific ? new UnaryOperator ("loan_app_loan.id", in, loan_ids) : new UnaryOperator()
        )
    );

    // psqlQueryJoin->execute();
    // cout <<"COUNT : " <<psqlQueryJoin->getResultCount() << endl;
    // map <string,PSQLAbstractORM *> * orms =  psqlQueryJoin->next();
    // if (orms == nullptr){
    //     throw std::runtime_error( "Query Returns Null");
    // }
    // loan_app_loan_primitive_orm * lal_orm = ORM(loan_app_loan,orms);
    // cout << "LOANid: " << lal_orm->get_id() << endl;

    psqlQueryJoin->process (threadsCount,[](map <string,PSQLAbstractORM *> * orms,int partition_number,mutex * shared_lock) {
            cout << "INsidee processsssssss" << endl;

            // loan_app_installment_primitive_orm * lai_orm = ORM(loan_app_installment,orms);
            // new_lms_installmentextension_primitive_orm * ieorm = ORM(new_lms_installmentextension,orms);
            loan_app_loan_primitive_orm * lal_orm = ORM(loan_app_loan,orms);

            cout << lal_orm->get_id() << endl;

    });

    
    // // json entry_json = { {"Booking new loan - short term; and", { {"amount" ,2000.00}, {"loan_id", 133416}, {"customer_id", 359786}, {"cashier_id", 126935}, {"merchant_id", 2} }} };
    // LedgerAmount demo(1, 101, 233, 301, 401, 501, 601, 2, 801, 901, 1001, 1101, 1201.50, 1301, true);
    // std::map<std::string, LedgerAmount> temp;
    // temp["Booking new loan - short term; and"] = demo;


    // BDate entry_date =  BDate("2024-04-13");
    // // BlnkTemplateManager blnkTemplateManager(4, temp, entry_date);

    return 0;
}

// int main (int argc, char ** argv)
// {
//     int threadsCount = 1;
//     psqlController.addDataSource("main","192.168.1.51",5432,"c_plus_plus","postgres","postgres");
//     // cout << "Connected to " << database_name << endl;
    
//     psqlController.addDefault("created_at","now()",true,true);
//     psqlController.addDefault("updated_at","now()",true,true);
//     psqlController.addDefault("updated_at","now()",false,true);
//     psqlController.setORMCacheThreads(threadsCount);

//     PSQLJoinQueryIterator * psqlQueryJoin = new PSQLJoinQueryIterator ("main",
//         {new lms_entrytemplate_primitive_orm("main"),new ledger_entry_primitive_orm("main")},
//         {{{"lms_entrytemplate","id"},{"ledger_entry","template_id"}}});



//         psqlQueryJoin->filter(
//             ANDOperator 
//             (
//                 new UnaryOperator ("lms_entrytemplate.id",eq,4)
//             )
//         );

//     psqlQueryJoin->process (threadsCount,[](map <string,PSQLAbstractORM *> * orms,int partition_number,mutex * shared_lock) { 
//         lms_entrytemplate_primitive_orm * lep_orm = ORM(lms_entrytemplate, orms);
//         json _template = lep_orm->get_template();
//         cout << "TEMPLATE" << _template.dump();
//     });
    
//     cout << "hi" << endl;
//     return 0;
// }
