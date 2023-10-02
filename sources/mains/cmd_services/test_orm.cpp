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

int main (int argc, char ** argv)
{
    psqlController.addDataSource("main",argv[1],atoi(argv[2]),argv[3],argv[4],argv[5]);
    loan_app_loan_primitive_orm_iterator * loan_app_loan  = new loan_app_loan_primitive_orm_iterator("main");
    crm_app_customer_primitive_orm_iterator * crm_app_customer  = new crm_app_customer_primitive_orm_iterator("main");
    crm_app_customer->filter(ANDOperator (
        new UnaryOperator ("id",gt,"50000"),new UnaryOperator ("id",lt,"100000")
    ));
    crm_app_customer->process([](crm_app_customer_primitive_orm * orm) {
            cout << "Customer Name ["<< orm->get_id() <<"]:  "<< orm->get_first_name() << endl;
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