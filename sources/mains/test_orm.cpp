#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <PSQLController.h>
#include <loan_app_loan_primitive_orm.h>

/*


			cout << "Heeeeeeer" << endl;
			 loan_app_installment_loan_id = new vector <loan_app_installment_primitive_orm *> ();
			 loan_app_installment_primitive_orm_iterator * i = new loan_app_installment_primitive_orm_iterator("main");
			cout << "Heeeeeeer111    " << id << endl;

			i->filter (ANDOperator(new UnaryOperator("loan_id",eq,id)));
			i->execute();
			loan_app_installment_primitive_orm * l = NULL;
			do {
					l = i->next();
					if (l!= NULL) loan_app_installment_loan_id->push_back(l);


			} while (l != NULL);

*/
int main (int argc, char ** argv)
{
    psqlController.addDataSource("main",argv[1],atoi(argv[2]),argv[3],argv[4],argv[5]);
    loan_app_loan_primitive_orm_iterator * loan_app_loan  = new loan_app_loan_primitive_orm_iterator("main");
    
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
                cout << lal->get_customer_id() << endl;
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