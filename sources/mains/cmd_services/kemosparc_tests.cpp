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

    int threadsCount = 10 ;
    // bool connect = psqlController.addDataSource("main","192.168.65.216",5432,"django_ostaz_30042024_omneya","development","5k6MLFM9CLN3bD1");
   bool connect = psqlController.addDataSource("main","192.168.1.51",5432,"c_plus_plus_kelany","postgres","postgres");
    // bool connect = psqlController.addDataSource("main","192.168.1.51",5432,"django_ostaz_before_closure","postgres","postgres");
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
            new loan_app_loan_primitive_orm("main" ,false , true, -1, {}, true),
            new loan_app_installment_primitive_orm("main"),
            new new_lms_installmentextension_primitive_orm("main"),
            new loan_app_loanstatus_primitive_orm("main"), 
        },
        {
            {{"loan_app_loan","status_id"},{"loan_app_loanstatus","id"}},
            {{"new_lms_installmentextension","installment_ptr_id"},{"loan_app_installment","id"}},
            {{"loan_app_loan","id"},{"loan_app_installment","loan_id"}},
        });

    // psqlQueryJoin->addExtraFromField("(SELECT SUM(lai.principal_expected) FROM loan_app_installment lai INNER JOIN new_lms_installmentextension nli on nli.installment_ptr_id  = lai.id where nli.is_long_term = false and loan_app_loan.id = lai.loan_id)","short_term_principal");
    // psqlQueryJoin->addExtraFromField("(SELECT SUM(lai.principal_expected) FROM loan_app_installment lai INNER JOIN new_lms_installmentextension nli on nli.installment_ptr_id  = lai.id where nli.is_long_term = true and loan_app_loan.id = lai.loan_id)","long_term_principal");
    // psqlQueryJoin->addExtraFromField("(SELECT cap2.is_rescheduled FROM crm_app_purchase cap INNER JOIN crm_app_purchase cap2 ON cap.parent_purchase_id = cap2.id WHERE  cap.id = crm_app_purchase.id)","is_rescheduled");

    // string closure_date_string = "2024-05-15"; 
    
    psqlQueryJoin->filter(
        ANDOperator 
        (
            new UnaryOperator ("loan_app_loan.id",lte,30000)
        )
    );

    // psqlQueryJoin->setDistinct ({
    //         {"crm_app_customer","id"},
    //         {"loan_app_loan","id"}
    // });

    // psqlQueryJoin->setAggregates ({
    //         {"crm_app_customer", {"id", 1}}  
    //         // ,{"loan_app_loan","id"}
    // });
    psqlQueryJoin->setOrderBy("loan_app_installment.day");

    

    psqlQueryJoin->process(threadsCount, [](map <string,PSQLAbstractORM*> * orms,int partition_number,mutex * shared_lock,void * extras) {

            loan_app_loan_primitive_orm * lal = ORM(loan_app_loan, orms);
            loan_app_loanstatus_primitive_orm * lai = ORM(loan_app_loanstatus, orms);

            cout << "ID->" <<  lal->get_id() << endl;
            // cout << "Installment: " << lai->get_fra_cycles() << endl; 
            // lai->set_ndays(1011);
            // cout << "Installment: " << lai->get_fra_cycles() << endl; 

            // shared_lock->lock();
            // cout << "P#[" << partition_number << "]\t"<< lal_orm->get_num_periods() << "\t" <<cac_orm->get_id() << "\t" << lal_orm->get_id() <<"\t"<< ORML_SIZE(orms_list) << "\t";
            // for ( int i = 0 ;i < ORML_SIZE(orms_list) ; i ++)
            // {
            //     // loan_app_loan_bl_orm * abc  = ORML(za3bola,orms_list,i);
            //     // printf ("abs: %p \n",abc);
            //     loan_app_installment_primitive_orm * lai_orm  = ORML(loan_app_installment,orms_list,i);
            //     // PSQLGeneric_primitive_orm * gorm = ORML(PSQLGeneric,orms_list,i);
            //         // if (gorm != NULL)
            //         //     cout << gorm->get("aggregate") << "\t";
            //         if ( i > 0 ) cout <<", " ;
            //         cout << lai_orm->get_id();
            // }
            // cout << endl;
            // shared_lock->unlock();
        });



    psqlController.ORMCommit(true,true,true, "main");  
    return 0;
}