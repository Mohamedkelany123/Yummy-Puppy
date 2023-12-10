#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <PSQLController.h>
#include <PSQLUpdateQuery.h>
#include <loan_app_loan_primitive_orm.h>
#include <loan_app_installment_primitive_orm.h>
#include <new_lms_installmentextension_primitive_orm.h>
#include <crm_app_customer_primitive_orm.h>
#include <new_lms_installmentlatefees_primitive_orm.h>
#include <loan_app_loanstatushistroy_primitive_orm.h>
#include <new_lms_installmentstatushistory_primitive_orm.h>

//enum closure_status { START,UNDUE_TO_DUE, DUE_TO_OVERDUE, UPDATE_LOAN_STATUS, MARGINALIZE_INCOME_STEP1,MARGINALIZE_INCOME_STEP2,MARGINALIZE_INCOME_STEP3,LONG_TO_SHORT_TERM,LAST_ACCRUED_DAY,PREPAID_TRANSACTION };
enum closure_status { START,UNDUE_TO_DUE, DUE_TO_OVERDUE, UPDATE_LOAN_STATUS, MARGINALIZE_INCOME_STEP1,LONG_TO_SHORT_TERM,LAST_ACCRUED_DAY,PREPAID_TRANSACTION };

enum blnk_buckets { NONE, CURRENT, BUCKET1, BUCKET2, BUCKET3, BUCKET4, SETTLED, WRITEOFF, SETTLED_PAID_OFF, BUCKET5,BUCKET6,BUCKET7,CANCELLED, CANCELLED_PARTIAL_REFUND,PARTIAL_SETTLED_CHARGE_OFF,SETTLED_CHARGE_OFF };

#define TIME_ZONE_OFFEST 2




class BDate
{

    private:
        struct tm tm; 
        bool is_null;
    public:
        bool isValid() const {
        return !is_null;
        }
        void set_date (string date_string="")
        {
            is_null = false;
            if (date_string != "")
            {
                date_string += " 00:00:00";
                strptime(date_string.c_str(), "%Y-%m-%d %H:%M:%S",&tm);    
                tm.tm_hour +=TIME_ZONE_OFFEST;
            }
            else
            {
                is_null = true;
                date_string = "1970-01-01 00:00:00";
                strptime(date_string.c_str(), "%Y-%m-%d %H:%M:%S",&tm);    
                tm.tm_hour +=TIME_ZONE_OFFEST;
            }
        }
        BDate(string date_string="")
        {
            set_date(date_string);
        }
        BDate (struct tm & _tm)
        {
            tm = _tm;
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
            memset ( buf,0,255);
            if ( !is_null)
                strftime(buf, sizeof(buf), "%Y-%m-%d", &tm);
            string date_string = buf;
            return date_string;
        }
        ~BDate () {}
};


BDate getMarginalizationDate (loan_app_loan_primitive_orm * lal_orm,new_lms_installmentextension_primitive_orm * ie_orm,loan_app_installment_primitive_orm * i_orm,bool is_partial = false)
{
    BDate marg_date; 
    vector<loan_app_loanstatushistroy_primitive_orm *>  * lsh_orms = lal_orm->get_loan_app_loanstatushistroy_loan_id(true);
    for ( int i = lsh_orms->size()-1 ; i  >= 0 ; i --)
    {  
        if ( (*lsh_orms)[i]->get_status_type() == 0 && (*lsh_orms)[i]->get_previous_status_id() == blnk_buckets::BUCKET1 && (*lsh_orms)[i]->get_status_id () > blnk_buckets::BUCKET1)
        {
            marg_date.set_date( (*lsh_orms)[i]->get_day());
            break;
        }
    }
    if ( i_orm->get_period() == 1)
    {
        if (is_partial)
            marg_date.set_date("");
    }
    else
    {
        loan_app_installment_primitive_orm * lai_prev_orm = NULL;
        vector <loan_app_installment_primitive_orm *>  * lai_orms = lal_orm->get_loan_app_installment_loan_id(true);
        for ( int  i = 0 ; i < lai_orms->size() ; i ++ )
        {
                if ( (* lai_orms)[i]->get_period() == i_orm->get_period()-1 )
                {
                    lai_prev_orm = (* lai_orms)[i];
                    break;
                }
        }
        vector <new_lms_installmentextension_primitive_orm *>  * prev_lai_ext_orms = lai_prev_orm->get_new_lms_installmentextension_installment_ptr_id(true);
        vector<new_lms_installmentpaymentstatushistory_primitive_orm *> * iph_orms = (*prev_lai_ext_orms)[0]->get_new_lms_installmentpaymentstatushistory_installment_extension_id (true);
        std::list<int> s {1,2,3,6};
        int iph_index=-1;
        for (int i = iph_orms->size()-1 ; i >=0 ; i --)
        {
            int status = (*iph_orms)[i]->get_status();
            std::list<int>::iterator findIter = std::find(s.begin(), s.end(), status);
            if ( s.end() != findIter )
            {
                iph_index = i;
                break;
            }
        }
        BDate prev_installment_payment_date;

        if ( iph_index != -1 )  
            prev_installment_payment_date.set_date((*iph_orms)[iph_index]->get_day());

        BDate last_inst_marginalization_date ((*prev_lai_ext_orms)[0]->get_marginalization_date());
        BDate inst_marginalization_date (ie_orm->get_marginalization_date());
        BDate inst_partial_accrual_date(ie_orm->get_partial_accrual_date());
        BDate inst_accrual_date(ie_orm->get_accrual_date());
        BDate inst_day(i_orm->get_day());
        if ( (*prev_lai_ext_orms)[0]->get_status_id() > lal_orm->get_marginalization_bucket_id() && 
               ( ((*prev_lai_ext_orms)[0]->get_payment_status()!= 1 && (*prev_lai_ext_orms)[0]->get_payment_status()!= 2 
                && (*prev_lai_ext_orms)[0]->get_payment_status()!= 3 && (*prev_lai_ext_orms)[0]->get_payment_status()!= 6)
                || prev_installment_payment_date() >= inst_day()))
        {
            if ( is_partial)
            {
                    if (inst_partial_accrual_date() >= marg_date())
                        marg_date.set_date(inst_partial_accrual_date.getDateString());
                    else  marg_date.set_date(""); 
            }
            else
            {
                if (inst_accrual_date() >= marg_date())
                    marg_date.set_date(inst_accrual_date.getDateString());
            }
        }
        else
        {
            if (marg_date.getDateString() != "")
            {
                if (is_partial)
                {

                    marg_date.set_date("");
                    if (lal_orm->get_status_id() >= lal_orm->get_sticky_bucket_id() 
                            && inst_partial_accrual_date() >= marg_date()) 
                                marg_date.set_date(inst_partial_accrual_date.getDateString());
                }
                else
                {
                    if (inst_accrual_date() >= marg_date())
                        marg_date.set_date(inst_accrual_date.getDateString());
                }
            }
        }

    }
    return marg_date;
}

int main (int argc, char ** argv)
{   
    if (argc != 8)
    {
        printf("usage: %s <address> <port_number> <database name> <username> <password> <step> <date>YYYY-mm-dd\n",argv[0]);
        exit(9);
    }
    //2023-11-15
    string closure_date_string = argv[7];
    psqlController.addDataSource("main",argv[1],atoi(argv[2]),argv[3],argv[4],argv[5]);
    psqlController.addDefault("created_at","now()",true,true);
    psqlController.addDefault("updated_at","now()",true,true);
    psqlController.addDefault("updated_at","now()",false,true);
    psqlController.setORMCacheThreads(10);
    BDate closure_date(closure_date_string);
    if ( strcmp (argv[6],"undue_to_due") == 0 || strcmp (argv[6],"full_closure") == 0)
    {
        PSQLJoinQueryIterator * psqlQueryJoin = new PSQLJoinQueryIterator ("main",
        {new new_lms_installmentextension_primitive_orm(),new loan_app_installment_primitive_orm(),new loan_app_loan_primitive_orm()},
        {{{"loan_app_installment","loan_id"},{"loan_app_loan","id"}},{{"loan_app_installment","id"},{"new_lms_installmentextension","installment_ptr_id"}}});

        psqlQueryJoin->filter(
            ANDOperator 
            (
                new UnaryOperator ("new_lms_installmentextension.undue_to_due_date",lte,closure_date_string),
                new UnaryOperator ("new_lms_installmentextension.payment_status",eq,"5"),
                new UnaryOperator ("loan_app_loan.lms_closure_status",eq,to_string(closure_status::UNDUE_TO_DUE-1)),
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
        psqlController.ORMCommit(true,true,true);   
        delete (psqlQueryJoin);
        PSQLUpdateQuery psqlUpdateQuery ("main","loan_app_loan",
        ANDOperator (
                new UnaryOperator ("loan_app_loan.lms_closure_status",lt,closure_status::UNDUE_TO_DUE),
                new UnaryOperator ("loan_app_loan.lms_closure_status",gte,0)
        ),
        {{"lms_closure_status",to_string(closure_status::UNDUE_TO_DUE)}}
        );
        psqlUpdateQuery.update();

        cout << "Undue to Due done" << endl;
    }

    if ( strcmp (argv[6],"due_to_overdue") == 0 || strcmp (argv[6],"full_closure") == 0)
    {

        PSQLJoinQueryIterator * psqlQueryJoin = new PSQLJoinQueryIterator ("main",
        {new new_lms_installmentextension_primitive_orm(),
        new loan_app_installment_primitive_orm(),
        new loan_app_loan_primitive_orm()},
        {{{"loan_app_installment","loan_id"},{"loan_app_loan","id"}},
        {{"loan_app_installment","id"},{"new_lms_installmentextension","installment_ptr_id"}}});

        psqlQueryJoin->filter(
            ANDOperator 
            (
                new UnaryOperator ("new_lms_installmentextension.due_to_overdue_date",lte,closure_date_string),
                new UnaryOperator ("new_lms_installmentextension.is_interest_paid",eq,"f"),
                new UnaryOperator ("new_lms_installmentextension.payment_status",in,"0,4"),
                new UnaryOperator ("loan_app_loan.lms_closure_status",eq,to_string(closure_status::DUE_TO_OVERDUE-1)),
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
                loan_app_installment_primitive_orm * lai_orm  = ORM(loan_app_installment,orms);


                int late_fees_count = lform_v->size();
                if ( ieorm->get_payment_status() != 0 )
                {
                    // buggy behavior must check the following condition based on python django
                    // if not ins_ext.loan.is_sticky or (ins_ext.loan.is_sticky and ins_ext.is_principal_paid==False)
                    if ( (lal_orm->get_status_id() != 11) or (lal_orm->get_status_id() == 11 and ieorm->get_is_principal_paid() == false))
                    {
                        BDate overdue_date; 
                        overdue_date.set_date(lai_orm->get_day());
                        overdue_date.inc_day();
                        new_lms_installmentpaymentstatushistory_primitive_orm * psh_orm = new new_lms_installmentpaymentstatushistory_primitive_orm(true);
                        psh_orm->set_day(overdue_date.getDateString());
                        psh_orm->set_installment_extension_id(ORM(new_lms_installmentextension,orms)->get_installment_ptr_id());
                        psh_orm->set_status(0); // 0
                    }
                    ieorm->set_payment_status(0); //0
                }

                // shared_lock->lock();
                // cout << ieorm->get_installment_ptr_id() << " -> " <<  lform_v->size() <<  "->";
                if (lform_v->size() > 0 )
                {
                    reference_date.set_date(((*lform_v)[lform_v->size()-1])->get_day());
                    reference_date.inc_month();
                    // cout << ((*lform_v)[lform_v->size()-1])->get_day();
                }
                else
                { 
                    reference_date.set_date(ieorm->get_due_to_overdue_date());
                    // cout << ieorm->get_due_to_overdue_date();
                }
                // cout << endl;
                // -------------------------------------------------reference_date.inc_month();
                int seq = lform_v->size()+1;
                int initial_status_id = 1;
                int status_index=1;
                if (lform_v->size() != 0)
                {
                    initial_status_id = ((*lform_v)[lform_v->size()-1])->get_installment_status_id();
                    status_index=-1;
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
        psqlController.ORMCommit(true,true,true);   
        delete (psqlQueryJoin);
        PSQLUpdateQuery psqlUpdateQuery ("main","loan_app_loan",
        ANDOperator (
                new UnaryOperator ("loan_app_loan.lms_closure_status",lt,closure_status::DUE_TO_OVERDUE),
                new UnaryOperator ("loan_app_loan.lms_closure_status",gte,0)
        ),
        {{"lms_closure_status",to_string(closure_status::DUE_TO_OVERDUE)}}
        );
        psqlUpdateQuery.update();

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
                new UnaryOperator ("new_lms_installmentextension.payment_status",nin,"1,2,3"),
                new UnaryOperator ("loan_app_loan.lms_closure_status",eq,to_string(closure_status::UPDATE_LOAN_STATUS-1)),
                new UnaryOperator ("loan_app_loan.status_id",nin,"6, 7, 8, 12, 13, 15")
            )
        );
        psqlQueryJoin->process (10,[&closure_date](map <string,PSQLAbstractORM *> * orms,int partition_number,mutex * shared_lock) { 
                vector <int> buckets =      {1,2,3,4,5,9,10,11};
                vector <int> fra_buckets =  {1,1,2,2,3,4,4,5};
                // vector <int> fra_buckets = {0,0,2,0,3,0,0,4};
                new_lms_installmentextension_primitive_orm * ie_orm  = ORM(new_lms_installmentextension,orms);
                loan_app_installment_primitive_orm * lai_orm  = ORM(loan_app_installment,orms);
                loan_app_loan_primitive_orm * lal_orm  = ORM(loan_app_loan,orms);
                BDate reference_date (lai_orm->get_day());

                if ( ie_orm->get_payment_status() == 6 && reference_date() <=  closure_date())
                {
                    ie_orm->set_payment_status(1);
                    new_lms_installmentpaymentstatushistory_primitive_orm * psh_orm = new new_lms_installmentpaymentstatushistory_primitive_orm(true);
                    psh_orm->set_day(reference_date.getDateString());
                    psh_orm->set_installment_extension_id(ie_orm->get_installment_ptr_id());
                    psh_orm->set_status(1); // 0       
                }
                else
                {
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

                            if ( fb > ie_orm->get_fra_status_id() )
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
                }
                lal_orm->set_lms_closure_status(closure_status::UPDATE_LOAN_STATUS);

        });
        cout << "processed " << psqlQueryJoin->getResultCount() << " record(s)" << endl;
        psqlController.ORMCommit(true,true,true);   
        delete (psqlQueryJoin);
        PSQLUpdateQuery psqlUpdateQuery ("main","loan_app_loan",
        ANDOperator (
                new UnaryOperator ("loan_app_loan.lms_closure_status",lt,closure_status::UPDATE_LOAN_STATUS),
                new UnaryOperator ("loan_app_loan.lms_closure_status",gte,0)
        ),
        {{"lms_closure_status",to_string(closure_status::UPDATE_LOAN_STATUS)}}
        );
        psqlUpdateQuery.update();

        cout << "Loan Status done" << endl;
    }

    if ( strcmp (argv[6],"marginalization") == 0 || strcmp (argv[6],"full_closure") == 0)
    {


        PSQLJoinQueryIterator *  psqlQueryJoin = new PSQLJoinQueryIterator ("main",
        {new new_lms_installmentextension_primitive_orm(),new loan_app_installment_primitive_orm(),new loan_app_loan_primitive_orm(),new crm_app_customer_primitive_orm()},
        {{{"loan_app_installment","loan_id"},{"loan_app_loan","id"}},
        {{"loan_app_installment","id"},{"new_lms_installmentextension","installment_ptr_id"}},
        {{"crm_app_customer","id"},{"loan_app_loan","customer_id"}}
        });

        psqlQueryJoin->filter(
            ANDOperator 
            (
                new UnaryOperator ("new_lms_installmentextension.is_marginalized",eq,"f"),
                new UnaryOperator ("new_lms_installmentextension.is_partially_marginalized",eq,"f"),
                new UnaryOperator ("new_lms_installmentextension.partial_accrual_date",lte,closure_date_string),
                new UnaryOperator ("loan_app_loan.loan_booking_day",lte,closure_date_string),
                new UnaryOperator ("new_lms_installmentextension.is_interest_paid",eq,"f"),
                new UnaryOperator ("new_lms_installmentextension.payment_status",in,"0,4,5"),
                new UnaryOperator ("loan_app_loan.status_id",gt,"loan_app_loan.marginalization_bucket_id",true),
                new UnaryOperator ("loan_app_loan.lms_closure_status",eq,to_string(closure_status::MARGINALIZE_INCOME_STEP1-1)),
                new UnaryOperator ("loan_app_loan.status_id",nin,"1,6, 7, 8, 12, 13, 14, 15"),
                new UnaryOperator ("loan_app_installment.interest_expected",ne,"0"),
                new UnaryOperator ("crm_app_customer.first_loan_cycle_id",ne,"1")
            )
        );

        psqlQueryJoin->process (10,[](map <string,PSQLAbstractORM *> * orms,int partition_number,mutex * shared_lock) { 
                new_lms_installmentextension_primitive_orm * ie_orm  = ORM(new_lms_installmentextension,orms);
                loan_app_installment_primitive_orm * i_orm  = ORM(loan_app_installment,orms);
                loan_app_loan_primitive_orm * lal_orm  = ORM(loan_app_loan,orms);
                bool is_partial = true;
                BDate marg_date = getMarginalizationDate(lal_orm,ie_orm,i_orm,is_partial);
                if ( marg_date.getDateString() != "" )
                {
                    ie_orm->set_is_partially_marginalized(true);
                    ie_orm->set_partial_marginalization_date(marg_date.getDateString());
                }
                // lal_orm->set_lms_closure_status(closure_status::MARGINALIZE_INCOME_STEP1);
                // shared_lock->lock();
                // if ( marg_date.getDateString() != "")
                //     cout << ie_orm->get_installment_ptr_id() << " , " << ie_orm->get_is_marginalized() << " , "<< marg_date.getDateString() << " , "<< ie_orm->get_expected_partial_accrual_amount() << " , "<< ie_orm->get_undue_to_due_date() << " , "<< lal_orm->get_lms_closure_status() << endl;
                // shared_lock->unlock();
            });

        cout << "processed " << psqlQueryJoin->getResultCount() << " record(s)" << endl;
        psqlController.ORMCommit(true,true,true);   
        delete (psqlQueryJoin);

        cout << "Marginalization Setp 1" << endl;

        psqlQueryJoin = new PSQLJoinQueryIterator ("main",
        {new new_lms_installmentextension_primitive_orm(),new loan_app_installment_primitive_orm(),new loan_app_loan_primitive_orm()},
        {{{"loan_app_installment","loan_id"},{"loan_app_loan","id"}},{{"loan_app_installment","id"},{"new_lms_installmentextension","installment_ptr_id"}}});

        psqlQueryJoin->filter(
            ANDOperator 
            (
                new UnaryOperator ("new_lms_installmentextension.is_marginalized",eq,"f"),
                new UnaryOperator ("new_lms_installmentextension.accrual_date",lte,closure_date_string),
                new UnaryOperator ("loan_app_loan.loan_booking_day",lte,closure_date_string),
                new UnaryOperator ("new_lms_installmentextension.is_interest_paid",eq,"f"),
                new UnaryOperator ("new_lms_installmentextension.payment_status",in,"0,4"),
                new UnaryOperator ("loan_app_loan.status_id",gt,"loan_app_loan.marginalization_bucket_id",true),
                new UnaryOperator ("loan_app_loan.lms_closure_status",eq,to_string(closure_status::MARGINALIZE_INCOME_STEP1-1)),
                new UnaryOperator ("loan_app_loan.status_id",nin,"1,6, 7, 8, 12, 13, 14, 15"),
                new UnaryOperator ("loan_app_installment.interest_expected",ne,"0")
                // new UnaryOperator ("loan_app_installment.id",eq,"327878")
            )
        );
        psqlQueryJoin->process (10,[](map <string,PSQLAbstractORM *> * orms,int partition_number,mutex * shared_lock) { 
                new_lms_installmentextension_primitive_orm * ie_orm  = ORM(new_lms_installmentextension,orms);
                loan_app_installment_primitive_orm * i_orm  = ORM(loan_app_installment,orms);
                loan_app_loan_primitive_orm * lal_orm  = ORM(loan_app_loan,orms);

                bool is_partial = false;
                BDate marg_date = getMarginalizationDate(lal_orm,ie_orm,i_orm,is_partial);
                if ( marg_date.getDateString() != "" )
                {
                    ie_orm->set_is_marginalized(true);
                    ie_orm->set_marginalization_date(marg_date.getDateString());
                }
                // lal_orm->set_lms_closure_status(closure_status::MARGINALIZE_INCOME_STEP2);

                // shared_lock->lock();
                // if ( marg_date.getDateString() != "") 
                //     cout << ie_orm->get_installment_ptr_id() << ": "<< marg_date.getDateString() << endl;
                // shared_lock->unlock();
            });
        cout << "processed " << psqlQueryJoin->getResultCount() << " record(s)" << endl;
        psqlController.ORMCommit(true,true,true);   
        delete (psqlQueryJoin);
        cout << "Marginalization Setp 2" << endl;




        psqlQueryJoin = new PSQLJoinQueryIterator ("main",
        {new new_lms_installmentextension_primitive_orm(),new loan_app_installment_primitive_orm(),new loan_app_loan_primitive_orm(),new new_lms_installmentlatefees_primitive_orm()},
        {{{"loan_app_installment","loan_id"},{"loan_app_loan","id"}},
            {{"loan_app_installment","id"},{"new_lms_installmentextension","installment_ptr_id"}},
            {{"new_lms_installmentextension","installment_ptr_id"},{"new_lms_installmentlatefees","installment_extension_id"}}
        });

        psqlQueryJoin->filter(
            ANDOperator 
            (
                new UnaryOperator ("new_lms_installmentlatefees.is_marginalized",eq,"f"),
                new UnaryOperator ("new_lms_installmentlatefees.is_paid",eq,"f"),
                new UnaryOperator ("new_lms_installmentlatefees.day",lte,closure_date_string),
                new UnaryOperator ("new_lms_installmentextension.accrual_date",lte,closure_date_string),
                new UnaryOperator ("loan_app_loan.loan_booking_day",lte,closure_date_string),
                new UnaryOperator ("new_lms_installmentextension.is_interest_paid",eq,"f"),
                new UnaryOperator ("new_lms_installmentextension.payment_status",in,"0,4"),
                new UnaryOperator ("loan_app_loan.status_id",gt,"loan_app_loan.marginalization_bucket_id",true),
                new UnaryOperator ("loan_app_loan.lms_closure_status",eq,to_string(closure_status::MARGINALIZE_INCOME_STEP1-1)),
                new UnaryOperator ("loan_app_loan.status_id",nin,"1,6, 7, 8, 12, 13, 14, 15"),
                new OROperator (
                    new UnaryOperator ("new_lms_installmentlatefees.is_cancelled",eq,"f"),
                    new ANDOperator (
                        new UnaryOperator ("new_lms_installmentlatefees.is_cancelled",eq,"t"),
                        new UnaryOperator ("new_lms_installmentlatefees.cancellation_date",gt,"new_lms_installmentlatefees.marginalization_date",true)
                    )
                )
            )
        );

        psqlQueryJoin->process (10,[](map <string,PSQLAbstractORM *> * orms,int partition_number,mutex * shared_lock) { 
                new_lms_installmentextension_primitive_orm * ie_orm  = ORM(new_lms_installmentextension,orms);
                new_lms_installmentlatefees_primitive_orm * lf_orm = ORM(new_lms_installmentlatefees,orms);
                loan_app_loan_primitive_orm * lal_orm  = ORM(loan_app_loan,orms);
                loan_app_installment_primitive_orm * lai_orm  = ORM(loan_app_installment,orms);

                BDate marg_date; 
                vector<loan_app_loanstatushistroy_primitive_orm *>  * lsh_orms = lal_orm->get_loan_app_loanstatushistroy_loan_id(true);
                for ( int i = lsh_orms->size()-1 ; i  >= 0 ; i --)
                {  
                    if ( (*lsh_orms)[i]->get_status_type() == 0 && (*lsh_orms)[i]->get_previous_status_id() == blnk_buckets::BUCKET1 && (*lsh_orms)[i]->get_status_id () > blnk_buckets::BUCKET1)
                    {
                        marg_date.set_date( (*lsh_orms)[i]->get_day());
                        break;
                    }
                }

                if ( lf_orm->get_sequence() == 1)
                {
                    if ( lai_orm->get_period() != 1)
                    {
                        loan_app_installment_primitive_orm * prev_lai_orm = NULL;
                        vector <loan_app_installment_primitive_orm *>  * lai_orms = lal_orm->get_loan_app_installment_loan_id(true);
                        for ( int  i = 0 ; i < lai_orms->size() ; i ++ )
                        {
                            if ( (* lai_orms)[i]->get_period() == lai_orm->get_period()-1 )
                            {
                                prev_lai_orm = (* lai_orms)[i];
                                break;
                            }
                        }
                        vector <new_lms_installmentextension_primitive_orm *>  * prev_lai_ext_orms = prev_lai_orm->get_new_lms_installmentextension_installment_ptr_id(true);
                        vector<new_lms_installmentpaymentstatushistory_primitive_orm *> * iph_orms = (*prev_lai_ext_orms)[0]->get_new_lms_installmentpaymentstatushistory_installment_extension_id (true);
                        std::list<int> s {1,2,3,6};
                        int iph_index=-1;
                        for (int i = iph_orms->size()-1 ; i >=0 ; i --)
                        {
                            int status = (*iph_orms)[i]->get_status();
                            std::list<int>::iterator findIter = std::find(s.begin(), s.end(), status);
                            if ( s.end() != findIter )
                            {
                                iph_index = i;
                                break;
                            }
                        }

                        BDate prev_installment_payment_date;
                        BDate ins_date(lai_orm->get_day());
                        BDate ie_marg_date(ie_orm->get_marginalization_date());
                        BDate lf_day(lf_orm->get_day());

                        if ( iph_index != -1 )  
                            prev_installment_payment_date.set_date((*iph_orms)[iph_index]->get_day());

                         
                        if (( prev_installment_payment_date()>= lf_day()  || std::find(s.begin(), s.end(), (*prev_lai_ext_orms)[0]->get_payment_status()) == s.end() )
                            && (*prev_lai_ext_orms)[0]->get_status_id() > lal_orm->get_marginalization_bucket_id())
                        {
                            if (ie_orm->get_marginalization_date() != "" && ie_marg_date() <= lf_day() 
                                || (lal_orm->get_status_id() > lal_orm->get_marginalization_bucket_id() && lai_orm->get_interest_expected() == 0)
                            )
                            {
                                marg_date.set_date(lf_orm->get_day());
                            }
                            else
                            {
                                ins_date.inc_day();
                                ins_date.inc_month();
                                marg_date.set_date(ins_date.getDateString());
                            }
                        }
                        else
                        {
                            if ( !(marg_date() > lf_day ()))
                                marg_date.set_date(lf_orm->get_day());
                        }
                    } // else is already handled above through the default value
                }
                else
                {

                    marg_date.set_date(lf_orm->get_day());
                }

                if ( marg_date.getDateString() != "" )
                {
                    lf_orm->set_is_marginalized(true);
                    lf_orm->set_marginalization_date(marg_date.getDateString());
                }
                lal_orm->set_lms_closure_status(closure_status::MARGINALIZE_INCOME_STEP1);
                // shared_lock->lock();
                // cout << lf_orm->get_id() << " - " << marg_date.getDateString() <<  " - " << lf_orm->get_day() << endl;
                // shared_lock->unlock();
            });

        cout << "processed " << psqlQueryJoin->getResultCount() << " record(s)" << endl;
        psqlController.ORMCommit(true,true,true);   
        delete (psqlQueryJoin);
        PSQLUpdateQuery psqlUpdateQuery ("main","loan_app_loan",
        ANDOperator (
                new UnaryOperator ("loan_app_loan.lms_closure_status",lt,closure_status::MARGINALIZE_INCOME_STEP1),
                new UnaryOperator ("loan_app_loan.lms_closure_status",gte,0)
        ),

        {{"lms_closure_status",to_string(closure_status::MARGINALIZE_INCOME_STEP1)}}
        );
        psqlUpdateQuery.update();

        cout << "Marginalization Setp 3" << endl;

    }

    if ( strcmp (argv[6],"long_to_short") == 0 || strcmp (argv[6],"full_closure") == 0)
    {
        PSQLJoinQueryIterator *  psqlQueryJoin = new PSQLJoinQueryIterator ("main",
        {new new_lms_installmentextension_primitive_orm(),new loan_app_installment_primitive_orm(),new loan_app_loan_primitive_orm()},
        {{{"loan_app_installment","loan_id"},{"loan_app_loan","id"}},
        {{"loan_app_installment","id"},{"new_lms_installmentextension","installment_ptr_id"}}
        });

        psqlQueryJoin->filter(
            ANDOperator 
            (
                new UnaryOperator ("new_lms_installmentextension.long_to_short_term_date",lte,closure_date_string),
                new UnaryOperator ("new_lms_installmentextension.is_long_term",eq,"t"),
                new UnaryOperator ("loan_app_loan.lms_closure_status",eq,to_string(closure_status::LONG_TO_SHORT_TERM-1)),
                new UnaryOperator ("new_lms_installmentextension.status_id",nin,"12, 13"),
                new OROperator (
                    new UnaryOperator ("new_lms_installmentextension.is_principal_paid",eq,"f"),
                    new ANDOperator (
                        new UnaryOperator ("new_lms_installmentextension.is_principal_paid",eq,"t"),
                        new UnaryOperator ("new_lms_installmentextension.long_to_short_term_date::date",lte,"new_lms_installmentextension.principal_paid_at::date",true)
                    )
                )
            )
        );

        psqlQueryJoin->process (10,[](map <string,PSQLAbstractORM *> * orms,int partition_number,mutex * shared_lock) { 
                new_lms_installmentextension_primitive_orm * ie_orm  = ORM(new_lms_installmentextension,orms);
                loan_app_loan_primitive_orm * lal_orm  = ORM(loan_app_loan,orms);
                ie_orm->set_is_long_term(false); 
                lal_orm->set_lms_closure_status(closure_status::LONG_TO_SHORT_TERM);
                // shared_lock->lock();
                //     cout << ie_orm->get_installment_ptr_id() << " : " << ie_orm->get_is_principal_paid() << " : " << ie_orm->get_principal_paid_at() << endl;
                // shared_lock->unlock();
            });
        cout << "processed " << psqlQueryJoin->getResultCount() << " record(s)" << endl;
        psqlController.ORMCommit(true,true,true);   
        delete (psqlQueryJoin);
        PSQLUpdateQuery psqlUpdateQuery ("main","loan_app_loan",
            ANDOperator (
                    new UnaryOperator ("loan_app_loan.lms_closure_status",lt,closure_status::LONG_TO_SHORT_TERM),
                    new UnaryOperator ("loan_app_loan.lms_closure_status",gte,0)
            ),

            {{"lms_closure_status",to_string(closure_status::LONG_TO_SHORT_TERM)}}
        );
        psqlUpdateQuery.update();
        cout << "Long to short term" << endl;

    }


    if ( strcmp (argv[6],"last_accrual_interest_date") == 0 || strcmp (argv[6],"full_closure") == 0)
    {
        PSQLJoinQueryIterator *  psqlQueryJoin = new PSQLJoinQueryIterator ("main",
        {new new_lms_installmentextension_primitive_orm(),new loan_app_installment_primitive_orm(),new loan_app_loan_primitive_orm()},
        {{{"loan_app_installment","loan_id"},{"loan_app_loan","id"}},
        {{"loan_app_installment","id"},{"new_lms_installmentextension","installment_ptr_id"}},
        });

        //CHANGED TO DECENDING
        psqlQueryJoin->setOrderBy("loan_app_loan.id desc ,new_lms_installmentextension.accrual_date desc");
        psqlQueryJoin->setDistinct(" distinct on (loan_app_loan.id) ");
        psqlQueryJoin->filter(
            ANDOperator 
            (
                // 7(WRITEOFF) and 14(PARTIAL-SETTLE-CHARGE-OFF) are not in django
                new UnaryOperator ("loan_app_loan.status_id",nin,"6, 8, 12, 13, 15"),
                new UnaryOperator ("loan_app_loan.lms_closure_status",eq,to_string(closure_status::LAST_ACCRUED_DAY-1)),                
                new OROperator (
                    new UnaryOperator ("new_lms_installmentextension.partial_accrual_date",lte,closure_date_string),
                    new ANDOperator (
                        new UnaryOperator ("new_lms_installmentextension.partial_accrual_date",isnull,"abc"),
                        new UnaryOperator ("loan_app_installment.day-1",lte,closure_date_string)
                    )
                )
            )
        );
        psqlQueryJoin->process (10,[&closure_date](map <string,PSQLAbstractORM *> * orms,int partition_number,mutex * shared_lock) { 
                new_lms_installmentextension_primitive_orm * ie_orm  = ORM(new_lms_installmentextension,orms);
                loan_app_loan_primitive_orm * lal_orm  = ORM(loan_app_loan,orms);
                BDate accrual_date(ie_orm->get_accrual_date());
                BDate partial_accrual_date(ie_orm->get_partial_accrual_date());
                BDate first_accrual_adjustment_date(lal_orm->get_first_accrual_adjustment_date());
                BDate last_accrued_interest_day(lal_orm->get_last_accrued_interest_day());
                BDate new_last_accrued_interest_day (lal_orm->loan_booking_day());
                new_last_accrued_interest_day.dec_day();

                //Missing Conditions From Django --> last_accrued_interest_day_service.py --> line: [21-27]
                //--------------------------------------------------------------------------------------------------------
                if ( !accrual_date.isValid() ) {
                    accrual_date.set_date(lal_orm->get_loan_booking_day());
                }else if( !partial_accrual_date.isValid() ){ 
                    partial_accrual_date.set_date(lal_orm->get_loan_booking_day());
                }else if( !first_accrual_adjustment_date.isValid() || first_accrual_adjustment_date() <= closure_date()){
                    first_accrual_adjustment_date.set_date(lal_orm->get_loan_booking_day());
                }
                //----------------------------------------------------------------------------------------------------------

                if ( accrual_date() >= partial_accrual_date() &&  accrual_date() >= first_accrual_adjustment_date () && accrual_date() <= closure_date ()) 
                    new_last_accrued_interest_day.set_date(accrual_date.getDateString());
                else if ( partial_accrual_date() >= first_accrual_adjustment_date () && partial_accrual_date() <= closure_date () ) 
                    new_last_accrued_interest_day.set_date(partial_accrual_date.getDateString());
                else if ( first_accrual_adjustment_date() <= closure_date ()) 
                    new_last_accrued_interest_day.set_date(first_accrual_adjustment_date.getDateString());

                if ( last_accrued_interest_day() <  new_last_accrued_interest_day())
                {
                    lal_orm->set_last_accrued_interest_day(new_last_accrued_interest_day.getDateString());
                    lal_orm->set_lms_closure_status(closure_status::LAST_ACCRUED_DAY);
                }
                // shared_lock->lock();
                // if ( last_accrued_interest_day() <  new_last_accrued_interest_day())
                //     cout << lal_orm->get_id() << " - " << accrual_date.getDateString() << " - " << partial_accrual_date.getDateString() << " - " << first_accrual_adjustment_date.getDateString() << " ---> " << last_accrued_interest_day.getDateString() << " ===== " << lal_orm->get_last_accrued_interest_day() << endl;
                // shared_lock->unlock();
            });
        cout << "processed " << psqlQueryJoin->getResultCount() << " record(s)" << endl;
        psqlController.ORMCommit(true,true,true);   
           delete (psqlQueryJoin);
        PSQLUpdateQuery psqlUpdateQuery ("main","loan_app_loan",
            ANDOperator (
                    new UnaryOperator ("loan_app_loan.lms_closure_status",lt,closure_status::LAST_ACCRUED_DAY),
                    new UnaryOperator ("loan_app_loan.lms_closure_status",gte,0)
            ),

            {{"lms_closure_status",to_string(closure_status::LAST_ACCRUED_DAY)}}
        );
        psqlUpdateQuery.update();
        cout << "Loan Last Accrual Day" << endl;

    }
    if (strcmp (argv[6],"full_closure") == 0)
    {
        PSQLUpdateQuery psqlUpdateQuery ("main","loan_app_loan",
            UnaryOperator ("loan_app_loan.lms_closure_status",gte,closure_status::LAST_ACCRUED_DAY-1),
            {{"lms_closure_status",to_string(0)}}
        );
        psqlUpdateQuery.update();
    }
    return 0;
}

//select lal.id,nli.accrual_date,nli.partial_accrual_date,lal.first_accrual_adjustment_date from loan_app_loan lal, loan_app_installment lai, new_lms_installmentextension nli where lal.id=lai.loan_id and lai.id = nli.installment_ptr_id and ((lai.day-1 <= '2023-11-15' and nli.partial_accrual_date is null )or nli.partial_accrual_date <= '2023-11-15') and lal.status_id not in (12,13,6,8,15) order by loan_id asc ,nli.accrual_date desc;

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