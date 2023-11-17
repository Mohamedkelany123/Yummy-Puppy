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
#include <loan_app_loanstatushistroy_primitive_orm.h>
#include <new_lms_installmentstatushistory_primitive_orm.h>

enum closure_status { START,UNDUE_TO_DUE, DUE_TO_OVERDUE, UPDATE_LOAN_STATUS, MARGINALIZE_INCOME,LONG_TO_SHORT_TERM,LAST_ACCRUED_DAY,PREPAID_TRANSACTION };



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
        void dec_day ()
        {
            tm.tm_mday --;
        }
        void inc_day ()
        {
            tm.tm_mday ++;
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
    psqlController.addDataSource("main",argv[1],atoi(argv[2]),argv[3],argv[4],argv[5]);
    psqlController.setORMCacheThreads(10);
    BDate closure_date("2023-11-15");
    if ( strcmp (argv[6],"undue_to_due") == 0 || strcmp (argv[6],"full_closure") == 0)
    {
        PSQLJoinQueryIterator * psqlQueryJoin = new PSQLJoinQueryIterator ("main",
        {new new_lms_installmentextension_primitive_orm(),new loan_app_installment_primitive_orm(),new loan_app_loan_primitive_orm()},
        {{{"loan_app_installment","loan_id"},{"loan_app_loan","id"}},{{"loan_app_installment","id"},{"new_lms_installmentextension","installment_ptr_id"}}});

        psqlQueryJoin->filter(
            ANDOperator 
            (
                new UnaryOperator ("new_lms_installmentextension.undue_to_due_date",lte,"2023-11-15"),
                new UnaryOperator ("new_lms_installmentextension.payment_status",eq,"5"),
                new UnaryOperator ("loan_app_loan.lms_closure_status",lt,to_string(closure_status::UNDUE_TO_DUE)),
                new UnaryOperator ("loan_app_loan.status_id",nin,"6, 7, 8, 12, 13, 15")
            )
        );
        psqlQueryJoin->process (10,[](map <string,PSQLAbstractORM *> * orms,int partition_number,mutex * shared_lock) { 
                new_lms_installmentextension_primitive_orm * ieorm = ORM(new_lms_installmentextension,orms);
                loan_app_loan_primitive_orm * lal_orm = ORM(loan_app_loan,orms);
                ieorm->set_payment_status(4);
                new_lms_installmentpaymentstatushistory_primitive_orm * orm = new new_lms_installmentpaymentstatushistory_primitive_orm(true);
                orm->set_day(ORM(loan_app_installment,orms)->get_day());
                orm->set_installment_extension_id(ORM(new_lms_installmentextension,orms)->get_installment_ptr_id());
                orm->set_status(4); // 4
                lal_orm->set_lms_closure_status(closure_status::UNDUE_TO_DUE);
        });
        cout << "processed " << psqlQueryJoin->getResultCount() << " record(s)" << endl;
        psqlController.ORMCommit(true,true);   
        delete (psqlQueryJoin);
        cout << "Undue to Due done" << endl;
    }

    if ( strcmp (argv[6],"due_to_overdue") == 0 || strcmp (argv[6],"full_closure") == 0)
    {

        PSQLJoinQueryIterator * psqlQueryJoin = new PSQLJoinQueryIterator ("main",
        {new new_lms_installmentextension_primitive_orm(),new loan_app_installment_primitive_orm(),new loan_app_loan_primitive_orm()},
        {{{"loan_app_installment","loan_id"},{"loan_app_loan","id"}},{{"loan_app_installment","id"},{"new_lms_installmentextension","installment_ptr_id"}}});

        psqlQueryJoin->filter(
            ANDOperator 
            (
                new UnaryOperator ("new_lms_installmentextension.due_to_overdue_date",lte,"2023-11-15"),
                new UnaryOperator ("new_lms_installmentextension.is_interest_paid",eq,"f"),
                new UnaryOperator ("new_lms_installmentextension.payment_status",in,"0,4"),
                new UnaryOperator ("loan_app_loan.lms_closure_status",lt,to_string(closure_status::DUE_TO_OVERDUE)),
                new UnaryOperator ("loan_app_loan.status_id",nin,"6, 7, 8, 12, 13, 15")
                // new UnaryOperator ("new_lms_installmentextension.installment_ptr_id",eq,"1373603")
            )
        );

        psqlQueryJoin->process (10,[&closure_date](map <string,PSQLAbstractORM *> * orms,int partition_number,mutex * shared_lock) {

                vector <int> buckets = {1,2,3,4,5,9,10,11};

                BDate reference_date; 
                new_lms_installmentextension_primitive_orm * ieorm = ORM(new_lms_installmentextension,orms);
                vector <new_lms_installmentlatefees_primitive_orm *> * lform_v = ORM(new_lms_installmentextension,orms)->get_new_lms_installmentlatefees_installment_extension_id();
                loan_app_loan_primitive_orm * lal_orm = ORM(loan_app_loan,orms);

                int late_fees_count = lform_v->size();
                if ( ieorm->get_payment_status() != 0 )
                {
                    ieorm->set_payment_status(0); //0
                    new_lms_installmentpaymentstatushistory_primitive_orm * psh_orm = new new_lms_installmentpaymentstatushistory_primitive_orm(true);
                    psh_orm->set_day(ieorm->get_due_to_overdue_date());
                    psh_orm->set_installment_extension_id(ORM(new_lms_installmentextension,orms)->get_installment_ptr_id());
                    psh_orm->set_status(0); // 0
                }


                // shared_lock->lock();
                // cout << ieorm->get_installment_ptr_id() << " -> " <<  lform_v->size() <<  "->";
                if (lform_v->size() > 0 )
                {
                    reference_date.set_date(((*lform_v)[lform_v->size()-1])->get_day());
                    // cout << ((*lform_v)[lform_v->size()-1])->get_day();
                }
                else
                { 
                    reference_date.set_date(ieorm->get_due_to_overdue_date());
                    // cout << ieorm->get_due_to_overdue_date();
                }
                // cout << endl;
                reference_date.inc_month();
                int seq = lform_v->size()+1;
                int initial_status_id = 1;
                int status_index=1;
                if (lform_v->size() != 0)
                {
                    initial_status_id = ((*lform_v)[lform_v->size()-1])->get_installment_status_id();
                    int status_index=-1;
                    for ( int i = 0 ; i < buckets.size() ; i ++)
                        if (buckets[i] == initial_status_id)
                        {
                            status_index = i+1;
                            if (status_index >= buckets.size()) status_index = buckets.size()-1;
                            break;
                        }
                }
                while (reference_date() <= closure_date()) {
                    // cout << "---------->" << reference_date.getDateString() << endl;
                    new_lms_installmentlatefees_primitive_orm * lf_orm = new new_lms_installmentlatefees_primitive_orm(true);
                    lf_orm->set_amount(ieorm->get_late_fees_amount());
                    lf_orm->set_installment_extension_id(ieorm->get_installment_ptr_id());
                    lf_orm->set_day(reference_date.getDateString());
                    lf_orm->set_sequence(seq);
                    seq ++;
                    lf_orm->set_installment_status_id(buckets[status_index]);
                    lf_orm->set_is_cancelled(false);
                    lf_orm->set_is_disabled(false);
                    lf_orm->set_is_cancelled_without_marginalization(false);
                    lf_orm->set_is_marginalized(false);
                    lf_orm->set_is_paid(false);
                    lf_orm->set_is_partially_cancelled(false);
                    reference_date.inc_month();
                    if (status_index < buckets.size()-1) status_index ++;
                } 
                lal_orm->set_lms_closure_status(closure_status::DUE_TO_OVERDUE);

                // cout << "connection_count: " << psqlController.getDataSourceConnectionCount("main") << endl;
                // cout << "Finished iteration" << endl;
                // shared_lock->unlock();
        });
        cout << "processed " << psqlQueryJoin->getResultCount() << " record(s)" << endl;
        psqlController.ORMCommit(true,true);   
        delete (psqlQueryJoin);
        cout << "Due to OverDue done" << endl;
    }
    if ( strcmp (argv[6],"status") == 0 || strcmp (argv[6],"full_closure") == 0)
    {
        BDate closure_yesterday = closure_date;
        closure_yesterday.dec_day();
        PSQLJoinQueryIterator * psqlQueryJoin = new PSQLJoinQueryIterator ("main",
        {new new_lms_installmentextension_primitive_orm(),new loan_app_installment_primitive_orm(),new loan_app_loan_primitive_orm()},
        {{{"loan_app_installment","loan_id"},{"loan_app_loan","id"}},{{"loan_app_installment","id"},{"new_lms_installmentextension","installment_ptr_id"}}});
        psqlQueryJoin->filter(
            ANDOperator 
            (
                new UnaryOperator ("new_lms_installmentextension.undue_to_due_date",lte,closure_yesterday.getDateString()),
                new UnaryOperator ("new_lms_installmentextension.payment_status",nin,"1,2,3,6"),
                new UnaryOperator ("loan_app_loan.lms_closure_status",lt,to_string(closure_status::UPDATE_LOAN_STATUS)),
                new UnaryOperator ("loan_app_loan.status_id",nin,"6, 7, 8, 12, 13, 15")
            )
        );
        psqlQueryJoin->process (10,[&closure_date](map <string,PSQLAbstractORM *> * orms,int partition_number,mutex * shared_lock) { 
                vector <int> buckets = {1,2,3,4,5,9,10,11};
                vector <int> fra_buckets = {0,0,2,0,3,0,0,4};
                new_lms_installmentextension_primitive_orm * ie_orm  = ORM(new_lms_installmentextension,orms);
                loan_app_installment_primitive_orm * lai_orm  = ORM(loan_app_installment,orms);
                loan_app_loan_primitive_orm * lal_orm  = ORM(loan_app_loan,orms);
                BDate reference_date (lai_orm->get_day());
                reference_date.inc_day();
                int bucket_index =1;
                int b = buckets[bucket_index];
                int fb = fra_buckets[bucket_index];

                while (reference_date() <= closure_date()) 
                {
                    if ( b > ie_orm->get_status_id() )
                    {
                        new_lms_installmentstatushistory_primitive_orm * ish_orm = new new_lms_installmentstatushistory_primitive_orm(true);
                        ish_orm->set_day(reference_date.getDateString());
                        ish_orm->set_status_type (0);
                        ish_orm->set_installment_id (ie_orm->get_installment_ptr_id());
                        ish_orm->set_status_id(b);
                        ish_orm->set_previous_status_id(ie_orm->get_status_id());
                        if ( b > lal_orm->get_status_id() )
                        {
                            loan_app_loanstatushistroy_primitive_orm * lsh_orm = new loan_app_loanstatushistroy_primitive_orm(true);
                            lsh_orm->set_day(reference_date.getDateString());
                            lsh_orm->set_status_type (0);
                            lsh_orm->set_loan_id (lal_orm->get_id());
                            lsh_orm->set_status_id(b);
                            lsh_orm->set_previous_status_id(lal_orm->get_status_id());
                            lal_orm->set_status_id(b);
                        }
                        ie_orm->set_status_id(b);
                        if ( b == 2)
                        {
                            if ( ie_orm->get_payment_status() != 0 )
                            {
                                ie_orm->set_payment_status(0); //0
                                new_lms_installmentpaymentstatushistory_primitive_orm * psh_orm = new new_lms_installmentpaymentstatushistory_primitive_orm(true);
                                psh_orm->set_day(reference_date.getDateString());
                                psh_orm->set_installment_extension_id(ie_orm->get_installment_ptr_id());
                                psh_orm->set_status(0); // 0
                            }
                        }

                        if ( fb != 0 )
                        {
                            new_lms_installmentstatushistory_primitive_orm * ish_orm = new new_lms_installmentstatushistory_primitive_orm(true);
                            ish_orm->set_day(reference_date.getDateString());
                            ish_orm->set_status_type (1);
                            ish_orm->set_installment_id (ie_orm->get_installment_ptr_id());
                            ish_orm->set_status_id(fb);
                            ish_orm->set_previous_status_id(ie_orm->get_fra_status_id());

                            if ( fb > lal_orm->get_fra_status_id() )
                            {
                                loan_app_loanstatushistroy_primitive_orm * lsh_orm = new loan_app_loanstatushistroy_primitive_orm(true);
                                lsh_orm->set_day(reference_date.getDateString());
                                lsh_orm->set_status_type (1);
                                lsh_orm->set_loan_id (lal_orm->get_id());
                                lsh_orm->set_status_id(fb);
                                lsh_orm->set_previous_status_id(lal_orm->get_fra_status_id());
                                lal_orm->set_fra_status_id(fb);
                                if (lsh_orm->get_loan_id() ==0 )
                                {
                                    cout << "Loan app Loan ID is zero" << endl;
                                }
                            }
                            ie_orm->set_fra_status_id(fb);
                        }
                    }
                    if (bucket_index < buckets.size()-1) bucket_index++;
                    b = buckets[bucket_index];
                    fb = fra_buckets[bucket_index];
                    reference_date.inc_month();
                }
                lal_orm->set_lms_closure_status(closure_status::UPDATE_LOAN_STATUS);

        });
        cout << "processed " << psqlQueryJoin->getResultCount() << " record(s)" << endl;
        psqlController.ORMCommit(true,true);   
        delete (psqlQueryJoin);
        cout << "Loan Status done" << endl;
    }

    if ( strcmp (argv[6],"marginalization") == 0 || strcmp (argv[6],"full_closure") == 0)
    {

        PSQLJoinQueryIterator *  psqlQueryJoin = new PSQLJoinQueryIterator ("main",
        {new new_lms_installmentextension_primitive_orm(),new loan_app_installment_primitive_orm(),new loan_app_loan_primitive_orm()},
        {{{"loan_app_installment","loan_id"},{"loan_app_loan","id"}},{{"loan_app_installment","id"},{"new_lms_installmentextension","installment_ptr_id"}}});

        psqlQueryJoin->filter(
            ANDOperator 
            (
                new UnaryOperator ("new_lms_installmentextension.is_marginalized",eq,"f"),
                new UnaryOperator ("new_lms_installmentextension.accrual_date",lte,"2023-11-15"),
                new UnaryOperator ("loan_app_loan.loan_booking_day",lte,"2023-11-15"),
                new UnaryOperator ("new_lms_installmentextension.is_interest_paid",eq,"f"),
                new UnaryOperator ("new_lms_installmentextension.payment_status",in,"0,4"),
                new UnaryOperator ("loan_app_loan.status_id",gt,"loan_app_loan.marginalization_bucket_id",true),
                new UnaryOperator ("loan_app_loan.lms_closure_status",lt,to_string(closure_status::MARGINALIZE_INCOME)),
                new UnaryOperator ("loan_app_loan.status_id",nin,"1,6, 7, 8, 12, 13, 14, 15"),
                new UnaryOperator ("loan_app_installment.interest_expected",ne,"0")
            )
        );
        psqlQueryJoin->process (10,[](map <string,PSQLAbstractORM *> * orms,int partition_number,mutex * shared_lock) { 
                new_lms_installmentextension_primitive_orm * ie_orm  = ORM(new_lms_installmentextension,orms);
                shared_lock->lock();
                cout << ie_orm->get_installment_ptr_id() << endl;
                shared_lock->unlock();
            });
        cout << "processed " << psqlQueryJoin->getResultCount() << " record(s)" << endl;
        psqlController.ORMCommit(true,true);   
        delete (psqlQueryJoin);
        cout << "Marginalization Setp 1" << endl;

        psqlQueryJoin = new PSQLJoinQueryIterator ("main",
        {new new_lms_installmentextension_primitive_orm(),new loan_app_installment_primitive_orm(),new loan_app_loan_primitive_orm()},
        {{{"loan_app_installment","loan_id"},{"loan_app_loan","id"}},{{"loan_app_installment","id"},{"new_lms_installmentextension","installment_ptr_id"}}});

        psqlQueryJoin->filter(
            ANDOperator 
            (
                new UnaryOperator ("new_lms_installmentextension.is_marginalized",eq,"f"),
                new UnaryOperator ("new_lms_installmentextension.is_partially_marginalized",eq,"f"),
                new UnaryOperator ("new_lms_installmentextension.partial_accrual_date",lte,"2023-11-15"),
                new UnaryOperator ("loan_app_loan.loan_booking_day",lte,"2023-11-15"),
                new UnaryOperator ("new_lms_installmentextension.is_interest_paid",eq,"f"),
                new UnaryOperator ("new_lms_installmentextension.payment_status",in,"0,4"),
                new UnaryOperator ("loan_app_loan.status_id",gt,"loan_app_loan.marginalization_bucket_id",true),
                new UnaryOperator ("loan_app_loan.lms_closure_status",lt,to_string(closure_status::MARGINALIZE_INCOME)),
                new UnaryOperator ("loan_app_loan.status_id",nin,"1,6, 7, 8, 12, 13, 14, 15"),
                new UnaryOperator ("loan_app_installment.interest_expected",ne,"0")
            )
        );

        psqlQueryJoin->process (10,[](map <string,PSQLAbstractORM *> * orms,int partition_number,mutex * shared_lock) { 
                new_lms_installmentextension_primitive_orm * ie_orm  = ORM(new_lms_installmentextension,orms);
                shared_lock->lock();
                cout << ie_orm->get_installment_ptr_id() << endl;
                shared_lock->unlock();
            });

        cout << "processed " << psqlQueryJoin->getResultCount() << " record(s)" << endl;
        psqlController.ORMCommit(true,true);   
        delete (psqlQueryJoin);
        cout << "Marginalization Setp 2" << endl;



        psqlQueryJoin = new PSQLJoinQueryIterator ("main",
        {new new_lms_installmentextension_primitive_orm(),new loan_app_installment_primitive_orm(),new loan_app_loan_primitive_orm()},
        {{{"loan_app_installment","loan_id"},{"loan_app_loan","id"}},{{"loan_app_installment","id"},{"new_lms_installmentextension","installment_ptr_id"}}});

        psqlQueryJoin->filter(
            ANDOperator 
            (
                new UnaryOperator ("new_lms_installmentextension.is_marginalized",eq,"f"),
                new UnaryOperator ("new_lms_installmentextension.is_interest_paid",eq,"f"),
                new UnaryOperator ("new_lms_installmentextension.accrual_date",lte,"2023-11-15"),
                new UnaryOperator ("loan_app_loan.loan_booking_day",lte,"2023-11-15"),
                new UnaryOperator ("new_lms_installmentextension.is_interest_paid",eq,"f"),
                new UnaryOperator ("new_lms_installmentextension.payment_status",in,"0,4"),
                new UnaryOperator ("loan_app_loan.status_id",gt,"loan_app_loan.marginalization_bucket_id",true),
                new UnaryOperator ("loan_app_loan.lms_closure_status",lt,to_string(closure_status::MARGINALIZE_INCOME)),
                new UnaryOperator ("loan_app_loan.status_id",nin,"1,6, 7, 8, 12, 13, 14, 15"),
                new UnaryOperator ("loan_app_installment.interest_expected",ne,"0"), // check this
                new UnaryOperator ("loan_app_installment.day",lte,"2023-11-15"), // check this
                new OROperator (
                    new UnaryOperator ("new_lms_installmentextension.is_cancelled",eq,"f"),
                    new ANDOperator (
                        new UnaryOperator ("new_lms_installmentextension.is_cancelled",eq,"t"),
                        new UnaryOperator ("new_lms_installmentextension.cancellation_date",gt,"new_lms_installmentextension.marginalization_date",true)
                    )
                )
            )
        );

        psqlQueryJoin->process (10,[](map <string,PSQLAbstractORM *> * orms,int partition_number,mutex * shared_lock) { 
                new_lms_installmentextension_primitive_orm * ie_orm  = ORM(new_lms_installmentextension,orms);
                shared_lock->lock();
                cout << ie_orm->get_installment_ptr_id() << endl;
                shared_lock->unlock();
            });

        cout << "processed " << psqlQueryJoin->getResultCount() << " record(s)" << endl;
        psqlController.ORMCommit(true,true);   
        delete (psqlQueryJoin);
        cout << "Marginalization Setp 3" << endl;

    }




    return 0;
}



// select max(id) from new_lms_installmentlatefees;
//    max   
// ---------
//  3758086

// select max(id) from new_lms_installmentpaymentstatushistory;
//    max   
// ---------
//  5617368
            // ORM(new_lms_installmentextension,orms)->set_payment_status(50);
            // new_lms_installmentpaymentstatushistory_primitive_orm * orm = new new_lms_installmentpaymentstatushistory_primitive_orm(true);
            // orm->set_day(ORM(loan_app_installment,orms)->get_day());
            // orm->set_installment_extension_id(ORM(new_lms_installmentextension,orms)->get_installment_ptr_id());
            // orm->set_status(50); // 4
// delete from new_lms_installmentlatefees where id > 3758086;
// delete from new_lms_installmentpaymentstatushistory where id > 5617368;