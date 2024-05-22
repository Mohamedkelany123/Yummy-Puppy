#include <PSQLController.h>
#include <TemplateManager.h>
#include <DisburseLoans.h>
#include <common_orm.h>
#include <common.h>
#include <PSQLAbstractQueryIterator.h>

//TODO: create special type for 

//<BuckedId,Percentage>


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

    // PSQLJoinQueryIterator * psqlQueryJoin = new PSQLJoinQueryIterator ("main",
    // {new loan_app_loan_bl_orm("main"),new loan_app_loanproduct_primitive_orm("main"), new crm_app_customer_primitive_orm("main"), new crm_app_purchase_primitive_orm("main")},
    // {{{"loan_app_loanproduct","id"},{"loan_app_loan","loan_product_id"}}, {{"loan_app_loan", "id"}, {"crm_app_purchase", "loan_id"}}, {{"loan_app_loan", "customer_id"}, {"crm_app_customer", "id"}}});


    PSQLJoinQueryIterator * psqlQueryJoin = new PSQLJoinQueryIterator ("main",
        {   
            new loan_app_loan_primitive_orm("main"),
            new loan_app_installment_primitive_orm("main"), 
            new crm_app_customer_primitive_orm("main")
        },
        {
            {{"loan_app_loan","id"},{"loan_app_installment","loan_id"}}, 
            {{"loan_app_loan","customer_id"}, {"crm_app_customer", "id"}}
        });

    // psqlQueryJoin->addExtraFromField("(SELECT SUM(lai.principal_expected) FROM loan_app_installment lai INNER JOIN new_lms_installmentextension nli on nli.installment_ptr_id  = lai.id where nli.is_long_term = false and loan_app_loan.id = lai.loan_id)","short_term_principal");
    // psqlQueryJoin->addExtraFromField("(SELECT SUM(lai.principal_expected) FROM loan_app_installment lai INNER JOIN new_lms_installmentextension nli on nli.installment_ptr_id  = lai.id where nli.is_long_term = true and loan_app_loan.id = lai.loan_id)","long_term_principal");
    // psqlQueryJoin->addExtraFromField("(SELECT cap2.is_rescheduled FROM crm_app_purchase cap INNER JOIN crm_app_purchase cap2 ON cap.parent_purchase_id = cap2.id WHERE  cap.id = crm_app_purchase.id)","is_rescheduled");

    // string closure_date_string = "2024-05-15"; 
    
    psqlQueryJoin->filter(
        ANDOperator 
        (
            new UnaryOperator ("loan_app_loan.id",lte,500)
        )
    );

    // psqlQueryJoin->setDistinct ({
    //         {"crm_app_customer","id"},
    //         {"loan_app_loan","id"}
    // });


    psqlQueryJoin->process(threadsCount, [](map <string,PSQLAbstractORM *> * orms,int partition_number,mutex * shared_lock,void * extras) {
            crm_app_customer_primitive_orm * cac_orm  = ORM(crm_app_customer,orms);
            loan_app_loan_primitive_orm * lal_orm  = ORM(loan_app_loan,orms);
            loan_app_installment_primitive_orm * lai_orm  = ORM(loan_app_installment,orms);

            cout << cac_orm->get_id() << "\t" << lal_orm->get_id() <<"\t" << lai_orm->get_id() << endl;
            

        });



    psqlController.ORMCommit(true,true,true, "main");  
    return 0;
}