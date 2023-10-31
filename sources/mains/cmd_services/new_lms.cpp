#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <PSQLController.h>
#include <loan_app_loan_primitive_orm.h>
#include <loan_app_installment_primitive_orm.h>
#include <new_lms_installmentextension_primitive_orm.h>
#include <crm_app_customer_primitive_orm.h>
#include <new_lms_installmentlatefees_primitive_orm.h>

class BDate
{

    private:
        struct tm tm; 
    public:
        void set_date (string date_string)
        {
            if (date_string != "")
            {
                date_string += " 00:00:00";
                strptime(date_string.c_str(), "%Y-%m-%d %H:%M:%S",&tm);    
            }
        }
        BDate(string date_string="")
        {
            set_date(date_string);
        }
        time_t operator () ()
        {
            return std::mktime(&tm);
        }
        void inc_month ()
        {
            tm.tm_mon ++;
        }
        string getDateString()
        {
            char buf[255];
            strftime(buf, sizeof(buf), "%Y-%m-%d", &tm);
            string date_string = buf;
            return date_string;
        }
        ~BDate () {}
};


int main (int argc, char ** argv)
{

    // BDate start_date ("2022-10-09");
    // BDate end_date ("2023-10-09");


    // while (start_date() <= end_date()) {
    //         cout << start_date.getDateString() << endl;
    //         start_date.inc_month();
    // }
    // return 0; 
    psqlController.addDataSource("main",argv[1],atoi(argv[2]),argv[3],argv[4],argv[5]);
    // PSQLJoinQueryIterator * psqlQueryJoin = new PSQLJoinQueryIterator ("main",
    // {new new_lms_installmentextension_primitive_orm(),new loan_app_installment_primitive_orm(),new loan_app_loan_primitive_orm()},
    // {{{"loan_app_installment","loan_id"},{"loan_app_loan","id"}},{{"loan_app_installment","id"},{"new_lms_installmentextension","installment_ptr_id"}}});

    // psqlQueryJoin->filter(
    //     ANDOperator 
    //     (
    //         new UnaryOperator ("new_lms_installmentextension.undue_to_due_date",lte,"2023-11-15"),
    //         new UnaryOperator ("new_lms_installmentextension.payment_status",eq,"5"),
    //         new UnaryOperator ("loan_app_loan.lms_closure_status",eq,"0"),
    //         new UnaryOperator ("loan_app_loan.status_id",nin,"6, 7, 8, 12, 13, 15")
    //     )
    // );
    // psqlQueryJoin->process (10,[](map <string,PSQLAbstractORM *> * orms,int partition_number,mutex * shared_lock) { 
    //         ORM(new_lms_installmentextension,orms)->set_payment_status(50);
    //         new_lms_installmentpaymentstatushistory_primitive_orm * orm = new new_lms_installmentpaymentstatushistory_primitive_orm(true);
    //         orm->set_day(ORM(loan_app_installment,orms)->get_day());
    //         orm->set_installment_extension_id(ORM(new_lms_installmentextension,orms)->get_installment_ptr_id());
    //         orm->set_status(50); // 4
    // });

    // cout << "Undue to Due done" << endl;

    PSQLJoinQueryIterator * psqlQueryJoin = new PSQLJoinQueryIterator ("main",
    {new new_lms_installmentextension_primitive_orm(),new loan_app_installment_primitive_orm(),new loan_app_loan_primitive_orm()},
    {{{"loan_app_installment","loan_id"},{"loan_app_loan","id"}},{{"loan_app_installment","id"},{"new_lms_installmentextension","installment_ptr_id"}}});

    psqlQueryJoin->filter(
        ANDOperator 
        (
            new UnaryOperator ("new_lms_installmentextension.due_to_overdue_date",lte,"2023-11-15"),
            new UnaryOperator ("new_lms_installmentextension.is_interest_paid",eq,"f"),
            new UnaryOperator ("new_lms_installmentextension.payment_status",in,"0,4"),
            new UnaryOperator ("loan_app_loan.lms_closure_status",eq,"0"),
            new UnaryOperator ("loan_app_loan.status_id",nin,"6, 7, 8, 12, 13, 15")
            // new UnaryOperator ("new_lms_installmentextension.installment_ptr_id",eq,"1373603")
        )
    );
    BDate closure_date("2023-11-15");
    psqlQueryJoin->process (10,[&closure_date](map <string,PSQLAbstractORM *> * orms,int partition_number,mutex * shared_lock) {
            BDate reference_date; 
            new_lms_installmentextension_primitive_orm * ieorm = ORM(new_lms_installmentextension,orms);
            vector <new_lms_installmentlatefees_primitive_orm *> * lform_v = ORM(new_lms_installmentextension,orms)->get_new_lms_installmentlatefees_installment_extension_id();
            int late_fees_count = lform_v->size();
            shared_lock->lock();
            cout << ieorm->get_installment_ptr_id() << " -> " <<  lform_v->size() <<  "->";
            if (lform_v->size() > 0 )
            {
                reference_date.set_date(((*lform_v)[lform_v->size()-1])->get_day());
                cout << ((*lform_v)[lform_v->size()-1])->get_day();
            }
            else
            { 
                reference_date.set_date(ieorm->get_due_to_overdue_date());
                cout << ieorm->get_due_to_overdue_date();
            }
            cout << endl;
            reference_date.inc_month();
            while (reference_date() <= closure_date()) {
                cout << "---------->" << reference_date.getDateString() << endl;
                reference_date.inc_month();
            } 
            // cout << "connection_count: " << psqlController.getDataSourceConnectionCount("main") << endl;
            shared_lock->unlock();





            // ORM(new_lms_installmentextension,orms)->set_payment_status(50);
            // new_lms_installmentpaymentstatushistory_primitive_orm * orm = new new_lms_installmentpaymentstatushistory_primitive_orm(true);
            // orm->set_day(ORM(loan_app_installment,orms)->get_day());
            // orm->set_installment_extension_id(ORM(new_lms_installmentextension,orms)->get_installment_ptr_id());
            // orm->set_status(50); // 4
    });


    cout << "I m done with the lambda" << endl;
    psqlController.ORMCommit();   
    return 0;
}