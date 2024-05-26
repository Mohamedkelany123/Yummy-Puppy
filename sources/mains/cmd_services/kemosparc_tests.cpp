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
    bool connect = psqlController.addDataSource("main","192.168.65.216",5432,"django_ostaz_30042024_omneya","development","5k6MLFM9CLN3bD1");
    // bool connect = psqlController.addDataSource("main","localhost",5432,"django_ostaz_25102023","postgres","postgres");
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
            new UnaryOperator ("crm_app_customer.id",lte,510)
        )
    );

    // psqlQueryJoin->setDistinct ({
    //         {"crm_app_customer","id"},
    //         {"loan_app_loan","id"}
    // });

    psqlQueryJoin->setAggregates ({
            {"crm_app_customer","id"},
            {"loan_app_loan","id"}
    });
    psqlQueryJoin->setOrderBy(" crm_app_customer.id asc , loan_app_loan.id asc, loan_app_installment.id asc ");

    class JoinResults
    {
        map <string,PSQLAbstractORM *> * orms;
        map <string,vector<PSQLAbstractORM *>> * aggregates;
    };

    psqlQueryJoin->process(threadsCount, [](map <string,PSQLAbstractORM *> * orms,int partition_number,mutex * shared_lock,void * extras) {
            crm_app_customer_primitive_orm * cac_orm  = ORM(crm_app_customer,orms);
            loan_app_loan_primitive_orm * lal_orm  = ORM(loan_app_loan,orms);
            loan_app_installment_primitive_orm * lai_orm  = ORM(loan_app_installment,orms);
            PSQLGeneric_primitive_orm * gorm = ORM(PSQLGeneric,orms);
            shared_lock->lock();
            if (gorm != NULL)
                cout << gorm->get("aggregate") << "\t";
            cout << lal_orm->get_num_periods() << "\t" <<cac_orm->get_id() << "\t" << lal_orm->get_id() <<"\t" << lai_orm->get_id() << endl;
            shared_lock->unlock();
        });



    psqlController.ORMCommit(true,true,true, "main");  
    return 0;
}