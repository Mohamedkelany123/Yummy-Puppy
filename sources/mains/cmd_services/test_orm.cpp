#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <PSQLController.h>
#include <loan_app_loan_primitive_orm.h>
#include <crm_app_customer_primitive_orm.h>

/*


            loan_app_installment_loan_id = new vector <loan_app_installment_primitive_orm *> ();
            loan_app_installment_primitive_orm_iterator * i = new loan_app_installment_primitive_orm_iterator("main");
			i->filter (ANDOperator(new UnaryOperator("loan_id",eq,id)));
			i->execute();
			loan_app_installment_primitive_orm * l = NULL;
			do {
					l = i->next();
					if (l!= NULL) loan_app_installment_loan_id->push_back(l);
			} while (l != NULL);

*/

void process_query(crm_app_customer_primitive_orm_iterator * crm_app_customer,std::function<void(crm_app_customer_primitive_orm * orm)> f)
{
    if (crm_app_customer->execute())
    {
        crm_app_customer_primitive_orm * cac = NULL;
        do {
            cac =crm_app_customer->next();
            if (cac != NULL) 
            {
                f(cac);
            }
        } while (cac != NULL);
    }
}

void func (vector <pair<pair<string,string>,pair<string,string>>>)
{
    printf ("This is a test");

}

#define ORM(T,O) ((T##_primitive_orm *)((*O)[#T]))

int main (int argc, char ** argv)
{

// select principle from loan_app_loan where id in (100,200,300);
//  principle 
// -----------
//    1234.00
//   23899.00
//   22000.00


// django_ostaz_15082023_old=# select principle from loan_app_loan where id=100;
//  principle 
// -----------
//    7943.00
// (1 row)
    //ORM(crm_app_customer,orms)
    psqlController.addDataSource("main",argv[1],atoi(argv[2]),argv[3],argv[4],argv[5]);
    PSQLJoinQueryIterator * psqlQueryJoin = new PSQLJoinQueryIterator ("main",
    {new crm_app_customer_primitive_orm(),new loan_app_loan_primitive_orm()},{{{"crm_app_customer","id"},{"loan_app_loan","customer_id"}}});


    psqlQueryJoin->process (10,[](map <string,PSQLAbstractORM *> * orms,int partition_number,mutex * shared_lock) {
        shared_lock->lock();
        if (ORM(loan_app_loan,orms)->get_id() == 100 || ORM(loan_app_loan,orms)->get_id() == 200 || ORM(loan_app_loan,orms)->get_id() == 300)
        {
            cout << ORM(crm_app_customer,orms)->get_first_name() << " - "<<  ORM(loan_app_loan,orms)->get_principle() << endl;
//            ORM(loan_app_loan,orms)->set_principle(7943.00);
            ORM(loan_app_loan,orms)->set_principle(1234.00);
            // ORM(loan_app_loan,orms)->update();
        }
//        else cout << "____________" << ORM(loan_app_loan,orms)->get_id() << endl;
        shared_lock->unlock();
    });
    psqlController.ORMCommit();
    // for (;psqlQueryJoin->fetchNextRow();)
    // {
    //     cout << psqlQueryJoin->getValue("crm_app_customer_first_name") << ": "<< psqlQueryJoin->getValue("loan_app_loan_id") <<  endl;
    // }
    delete (psqlQueryJoin);
    return 0;

    loan_app_loan_primitive_orm_iterator * loan_app_loan  = new loan_app_loan_primitive_orm_iterator("main");
    crm_app_customer_primitive_orm_iterator * crm_app_customer  = new crm_app_customer_primitive_orm_iterator("main");
    crm_app_customer->filter(ANDOperator (
        new UnaryOperator ("id",gt,"0"),new UnaryOperator ("id",lt,"1000000")
    ));
    loan_app_loan->filter(ANDOperator 
    (
        new UnaryOperator ("customer_id",gt,"100000"),new UnaryOperator ("customer_id",lt,"110000")
    ));

    crm_app_customer->process(10,[](crm_app_customer_primitive_orm * orm,int partition_number,mutex * shared_lock) {
            shared_lock->lock();
            cout << "(" << partition_number <<  ") Customer Name ["<< orm->get_id() <<"]:  "<< orm->get_first_name() << endl;
            shared_lock->unlock();
    });

    // if (crm_app_customer->execute())
    // {
    //     crm_app_customer_primitive_orm * cac = NULL;
    //     do {
    //         cac =crm_app_customer->next();
    //         if (cac != NULL) 
    //         {
    //             cout << "Customer Name: "<< cac->get_first_name() << endl;
    //         }
    //     } while (cac != NULL);

    // }
    return 0;
    loan_app_loan->filter(ANDOperator 
    (
        new UnaryOperator ("customer_id",gt,"100000"),new UnaryOperator ("customer_id",lt,"110000")
    ));
    if (loan_app_loan->execute())
    {
        loan_app_loan_primitive_orm * lal = NULL;
        do {
            lal =loan_app_loan->next();
            if (lal != NULL) 
            {
                cout << "Loan ID: "<< lal->get_id() << endl;
                cout << "Customer ID: "<< lal->get_customer_id() << endl;
                cout << "disable_early_repayment_fees: "<< lal->get_disable_early_repayment_fees() << endl;
                // vector <loan_app_installment_primitive_orm *> * ins = lal->get_loan_app_installment_loan_id();

                for ( auto a : *(lal->get_loan_app_installment_loan_id()))
                    cout << a->get_id()<< endl; 
                // for ( int i = 0 ; i < ins->size(); i++)
                //     cout << (*ins)[i]->get_id()<< endl; 
            }
            break;
        } while (lal != NULL);
    }
    delete (loan_app_loan);
    return 0;
}