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

extern "C" int main_closure (char* address, int port, char* database_name, char* username, char* password, char* step, char* closure_date_string, int threadsCount, int mod_value, int offset, char* loan_ids=NULL);

bool closure_go (string phone_numbers);

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
                    BDate tempDate;

                    tempDate.set_date(marg_date.getDateString());
                    marg_date.set_date("");

                    if (lal_orm->get_status_id() >= lal_orm->get_marginalization_bucket_id() 
                            && inst_partial_accrual_date() >= tempDate()) 
                                {
                                    marg_date.set_date(inst_partial_accrual_date.getDateString());
                                }
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


int main (int argc, char ** argv) {
    if (argc < 11 || argc > 12)
    {
        printf("usage: %s <address> <port_number> <database name> <username> <password> <step> <date>YYYY-mm-dd <threads count> <mod> <offset> <loan ids comma-seperated>\n",argv[0]);
        exit(12);
    }

    bool isLoanSpecific = argc >= 12; 
    //TODO: Change char* to string
    char* loan_ids = NULL;
    if (isLoanSpecific) {
        loan_ids = argv[11];
    }

    return main_closure(argv[1],atoi(argv[2]),argv[3],argv[4],argv[5],argv[6],argv[7],stoi(argv[8]),stoi(argv[9]),stoi(argv[10]),loan_ids);
}



// extern "c" not garbling function names
extern "C" int main_closure (char* address, int port, char* database_name, char* username, char* password, char* step, char* closure_date_string, int threadsCount, int mod_value, int offset, char* loan_ids)
{   
    bool isLoanSpecific = (loan_ids != NULL && strcmp(loan_ids,"") !=0); 
    if (isLoanSpecific){
        cout << "Loan ids to close: " << loan_ids << endl;
    }

    bool isMultiMachine = mod_value > 0; 

    psqlController.addDataSource("main",address,port,database_name,username,password);
    cout << "Connected to " << database_name << endl;
    
    psqlController.addDefault("created_at","now()",true,true);
    psqlController.addDefault("updated_at","now()",true,true);
    psqlController.addDefault("updated_at","now()",false,true);
    psqlController.setORMCacheThreads(threadsCount);
    BDate closure_date(closure_date_string);

    PSQLUpdateQuery psqlUpdateQuery ("main","loan_app_loan",
        ANDOperator(
            new UnaryOperator ("loan_app_loan.id",ne,"14312"),
            isMultiMachine ? new BinaryOperator ("loan_app_loan.id",mod,mod_value,eq,offset) : new BinaryOperator(),
            isLoanSpecific ? new UnaryOperator ("loan_app_loan.id", in, loan_ids) : new UnaryOperator()
        ),
        {{"lms_closure_status",to_string(0)}}
        );
    psqlUpdateQuery.update();   

    if ( strcmp (step,"undue_to_due") == 0 || strcmp (step,"full_closure") == 0 || strcmp (step,"payment_closure") == 0)
    {

        auto begin = std::chrono::high_resolution_clock::now();

        PSQLJoinQueryIterator * psqlQueryJoin = new PSQLJoinQueryIterator ("main",
        {new new_lms_installmentextension_primitive_orm("main"),new loan_app_installment_primitive_orm("main"),new loan_app_loan_primitive_orm("main")},
        {{{"loan_app_installment","loan_id"},{"loan_app_loan","id"}},{{"loan_app_installment","id"},{"new_lms_installmentextension","installment_ptr_id"}}});



        psqlQueryJoin->filter(
            ANDOperator 
            (
                new UnaryOperator ("new_lms_installmentextension.undue_to_due_date",lte,closure_date_string),
                new UnaryOperator ("new_lms_installmentextension.payment_status",eq,"5"),
                new UnaryOperator ("loan_app_loan.lms_closure_status",eq,to_string(closure_status::UNDUE_TO_DUE-1)),
                new UnaryOperator ("loan_app_loan.status_id",nin,"6, 7, 8, 12, 13, 15, 16"),
                isMultiMachine ? new BinaryOperator ("loan_app_loan.id",mod,mod_value,eq,offset) : new BinaryOperator(),
                isLoanSpecific ? new UnaryOperator ("loan_app_loan.id", in, loan_ids) : new UnaryOperator()
            )
        );

        auto beforeProcess = std::chrono::high_resolution_clock::now();
        
        cout << "THREADTIME --> undue_to_due" << endl;

        psqlQueryJoin->process (threadsCount,[](map <string,PSQLAbstractORM *> * orms,int partition_number,mutex * shared_lock) { 
                new_lms_installmentextension_primitive_orm * ieorm = ORM(new_lms_installmentextension,orms);
                loan_app_loan_primitive_orm * lal_orm = ORM(loan_app_loan,orms);
                ieorm->set_payment_status(4);
                new_lms_installmentpaymentstatushistory_primitive_orm * orm = new new_lms_installmentpaymentstatushistory_primitive_orm("main",true);
                orm->set_day(ORM(loan_app_installment,orms)->get_day());
                orm->set_installment_extension_id(ORM(new_lms_installmentextension,orms)->get_installment_ptr_id());
                orm->set_status(4); // 4
                lal_orm->set_lms_closure_status(closure_status::UNDUE_TO_DUE);
        });

        auto afterProcess = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(afterProcess - beforeProcess);
        printf("PROCESSTIME undue_to_due->: %.3f seconds.\n", elapsed.count() * 1e-9);

        cout << "processed " << psqlQueryJoin->getResultCount() << " record(s)" << endl;
        psqlController.ORMCommit(true,true,true, "main");   

        auto afterCommit = std::chrono::high_resolution_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(afterCommit - afterProcess);
        printf("COMMITTIME undue_to_due->: %.3f seconds.\n", elapsed.count() * 1e-9);


        delete (psqlQueryJoin);
        PSQLUpdateQuery psqlUpdateQuery ("main","loan_app_loan",
        OROperator (
            new UnaryOperator ("loan_app_loan.lms_closure_status",isnull,"",true),
            new ANDOperator (
                    new UnaryOperator ("loan_app_loan.lms_closure_status",lt,closure_status::UNDUE_TO_DUE),
                    new UnaryOperator ("loan_app_loan.id",ne,"14312"),
                    new UnaryOperator ("loan_app_loan.lms_closure_status",gte,0),
                    isMultiMachine ? new BinaryOperator ("loan_app_loan.id",mod,mod_value,eq,offset) : new BinaryOperator(),
                    isLoanSpecific ? new UnaryOperator ("loan_app_loan.id", in, loan_ids) : new UnaryOperator()  
            )          
        ),
        {{"lms_closure_status",to_string(closure_status::UNDUE_TO_DUE)}}
        );
        psqlUpdateQuery.update();

        cout << "Undue to Due done" << endl;

        // Stop measuring time and calculate the elapsed time
        auto end = std::chrono::high_resolution_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
        printf("Total time-> %.3f seconds.\n", elapsed.count() * 1e-9);
    }

    //-----------------------------------------------------------------------------------------------------------------------------------------//
    //-----------------------------------------------------------------------------------------------------------------------------------------//

    if ( strcmp (step,"due_to_overdue") == 0 || strcmp (step,"full_closure") == 0 || strcmp (step,"payment_closure") == 0)
    {

        auto begin = std::chrono::high_resolution_clock::now();

        PSQLJoinQueryIterator * psqlQueryJoin = new PSQLJoinQueryIterator ("main",
        {new new_lms_installmentextension_primitive_orm("main"),
        new loan_app_installment_primitive_orm("main"),
        new loan_app_loan_primitive_orm("main")},
        {{{"loan_app_installment","loan_id"},{"loan_app_loan","id"}},
        {{"loan_app_installment","id"},{"new_lms_installmentextension","installment_ptr_id"}}});


        psqlQueryJoin->addExtraFromField("(select count(*) from new_lms_installmentlatefees where installment_extension_id=new_lms_installmentextension.installment_ptr_id)","late_fees_count");
        psqlQueryJoin->addExtraFromField("(select max(day) from new_lms_installmentlatefees where installment_extension_id=new_lms_installmentextension.installment_ptr_id)","late_fees_date");
        psqlQueryJoin->addExtraFromField("(select max(installment_status_id) from new_lms_installmentlatefees nli where installment_extension_id=new_lms_installmentextension.installment_ptr_id)","late_fees_installment_status_id");
        

        psqlQueryJoin->filter(
            ANDOperator 
            (
                new UnaryOperator ("new_lms_installmentextension.due_to_overdue_date",lte,closure_date_string),
                new UnaryOperator ("new_lms_installmentextension.is_interest_paid",eq,"f"),
                new UnaryOperator ("new_lms_installmentextension.payment_status",in,"0,4"),
                new UnaryOperator ("loan_app_loan.lms_closure_status",eq,to_string(closure_status::DUE_TO_OVERDUE-1)),
                new UnaryOperator ("loan_app_loan.status_id",nin,"6, 7, 8, 12, 13, 15, 16"),
                isMultiMachine ? new BinaryOperator ("loan_app_loan.id",mod,mod_value,eq,offset) : new BinaryOperator(),
                isLoanSpecific ? new UnaryOperator ("loan_app_loan.id", in, loan_ids) : new UnaryOperator() 
            )
        );

  
        auto beforeProcess = std::chrono::high_resolution_clock::now();

        cout << "THREADTIME --> due_to_overdue" << endl;

        psqlQueryJoin->process (threadsCount,[&closure_date](map <string,PSQLAbstractORM *> * orms,int partition_number,mutex * shared_lock) {

                vector <int> buckets = {1,2,3,4,5,9,10,11};

                BDate reference_date; 
                new_lms_installmentextension_primitive_orm * ieorm = ORM(new_lms_installmentextension,orms);
                loan_app_loan_primitive_orm * lal_orm = ORM(loan_app_loan,orms);
                loan_app_installment_primitive_orm * lai_orm  = ORM(loan_app_installment,orms);

                PSQLGeneric_primitive_orm * gorm = ORM(PSQLGeneric,orms);
                int late_fees_count = gorm->toInt("late_fees_count");
                string late_fees_date = gorm->get("late_fees_date");
                int late_fees_installment_status_id = gorm->toInt("late_fees_installment_status_id");

                if ( ieorm->get_payment_status() != 0 )
                {
                    if ( (lal_orm->get_status_id() != 11) or (lal_orm->get_status_id() == 11 and ieorm->get_is_principal_paid() == false))
                    {
                        BDate overdue_date; 
                        overdue_date.set_date(lai_orm->get_day());
                        overdue_date.inc_day();
                        new_lms_installmentpaymentstatushistory_primitive_orm * psh_orm = new new_lms_installmentpaymentstatushistory_primitive_orm("main",true);
                        psh_orm->set_day(overdue_date.getDateString());
                        psh_orm->set_installment_extension_id(ORM(new_lms_installmentextension,orms)->get_installment_ptr_id());
                        psh_orm->set_status(0); // 0
                    }
                    ieorm->set_payment_status(0); //0
                }

                if (late_fees_count > 0 )
                {
                    reference_date.set_date(late_fees_date);
                    reference_date.inc_month();
                }
                else
                { 
                    reference_date.set_date(ieorm->get_due_to_overdue_date());
                }
                
                int seq = late_fees_count+1;
                int initial_status_id = 1;
                int status_index=1;
                if (late_fees_count != 0)
                {
                    initial_status_id = late_fees_installment_status_id;
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
                    new_lms_installmentlatefees_primitive_orm * lf_orm = new new_lms_installmentlatefees_primitive_orm("main",true);
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

        });

        auto afterProcess = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(afterProcess - beforeProcess);
        printf("PROCESSTIME due_to_overdue->: %.3f seconds.\n", elapsed.count() * 1e-9);

        cout << "processed " << psqlQueryJoin->getResultCount() << " record(s)" << endl;
        psqlController.ORMCommit(true,true,true, "main");   

        auto afterCommit = std::chrono::high_resolution_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(afterCommit - afterProcess);
        printf("COMMITTIME due_to_overdue->: %.3f seconds.\n", elapsed.count() * 1e-9);


        delete (psqlQueryJoin);
        PSQLUpdateQuery psqlUpdateQuery ("main","loan_app_loan",
        ANDOperator (
                new UnaryOperator ("loan_app_loan.lms_closure_status",lt,closure_status::DUE_TO_OVERDUE),
                new UnaryOperator ("loan_app_loan.id",ne,"14312"),
                new UnaryOperator ("loan_app_loan.lms_closure_status",gte,0),
                isMultiMachine ? new BinaryOperator ("loan_app_loan.id",mod,mod_value,eq,offset) : new BinaryOperator(),
                isLoanSpecific ? new UnaryOperator ("loan_app_loan.id", in, loan_ids) : new UnaryOperator() 
        ),
        {{"lms_closure_status",to_string(closure_status::DUE_TO_OVERDUE)}}
        );
        psqlUpdateQuery.update();

        cout << "Due to OverDue done" << endl;

        // Stop measuring time and calculate the elapsed time
        auto end = std::chrono::high_resolution_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);

        printf("Total Time-> %.3f seconds.\n", elapsed.count() * 1e-9);
    }

    //-----------------------------------------------------------------------------------------------------------------------------------------//
    //-----------------------------------------------------------------------------------------------------------------------------------------//

    if ( strcmp (step,"status") == 0 || strcmp (step,"full_closure") == 0 || strcmp (step,"payment_closure") == 0)
    {   

        auto begin = std::chrono::high_resolution_clock::now();

        BDate closure_yesterday = closure_date;
        closure_yesterday.dec_day();
        PSQLJoinQueryIterator * psqlQueryJoin = new PSQLJoinQueryIterator ("main",
        {new new_lms_installmentextension_primitive_orm("main"),new loan_app_installment_primitive_orm("main"),new loan_app_loan_primitive_orm("main")},
        {{{"loan_app_installment","loan_id"},{"loan_app_loan","id"}},{{"loan_app_installment","id"},{"new_lms_installmentextension","installment_ptr_id"}}});
        

        psqlQueryJoin->filter(
            ANDOperator 
            (
                new UnaryOperator ("new_lms_installmentextension.undue_to_due_date",lte,closure_yesterday.getDateString()),
                new UnaryOperator ("new_lms_installmentextension.payment_status",nin,"1,2,3"),
                new UnaryOperator ("new_lms_installmentextension.status_id",ne,"11"),
                new UnaryOperator ("loan_app_loan.lms_closure_status",eq,to_string(closure_status::UPDATE_LOAN_STATUS-1)),
                new UnaryOperator ("loan_app_loan.status_id",nin,"6, 7, 8, 12, 13, 15, 16"),
                isMultiMachine ? new BinaryOperator ("loan_app_loan.id",mod,mod_value,eq,offset) : new BinaryOperator(),
                isLoanSpecific ? new UnaryOperator ("loan_app_loan.id", in, loan_ids) : new UnaryOperator() 
            )
        );


        auto beforeProcess = std::chrono::high_resolution_clock::now();

        cout << "THREADTIME --> status" << endl;

        psqlQueryJoin->process (threadsCount,[&closure_date](map <string,PSQLAbstractORM *> * orms,int partition_number,mutex * shared_lock) { 
                vector <int> buckets =      {1,2,3,4,5,9,10,11};
                vector <int> fra_buckets =  {1,1,2,2,3,4,4,5};
                new_lms_installmentextension_primitive_orm * ie_orm  = ORM(new_lms_installmentextension,orms);
                loan_app_installment_primitive_orm * lai_orm  = ORM(loan_app_installment,orms);
                loan_app_loan_primitive_orm * lal_orm  = ORM(loan_app_loan,orms);
                BDate reference_date (lai_orm->get_day());

                if ( ie_orm->get_payment_status() == 6 && reference_date() <=  closure_date()) //Paid From Escrow
                {
                    ie_orm->set_payment_status(1);
                    new_lms_installmentpaymentstatushistory_primitive_orm * psh_orm = new new_lms_installmentpaymentstatushistory_primitive_orm("main",true);
                    psh_orm->set_day(reference_date.getDateString());
                    psh_orm->set_installment_extension_id(ie_orm->get_installment_ptr_id());
                    psh_orm->set_status(1); // 0  
                    psh_orm->set_order_id(ie_orm->get_principal_order_id());
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
                            new_lms_installmentstatushistory_primitive_orm * ish_orm = new new_lms_installmentstatushistory_primitive_orm("main",true, false);
                            ish_orm->set_day(reference_date.getDateString());
                            ish_orm->set_status_type (0);
                            ish_orm->set_installment_id (ie_orm->get_installment_ptr_id());
                            ish_orm->set_status_id(b);
                            ish_orm->set_previous_status_id(ie_orm->get_status_id());
                            if ( b > lal_orm->get_status_id() )
                            {
                                loan_app_loanstatushistroy_primitive_orm * lsh_orm = new loan_app_loanstatushistroy_primitive_orm("main",true);
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
                                    if ( (lal_orm->get_status_id() != 11) or (lal_orm->get_status_id() == 11 and ie_orm->get_is_principal_paid() == false))
                                    {
                                        new_lms_installmentpaymentstatushistory_primitive_orm * psh_orm = new new_lms_installmentpaymentstatushistory_primitive_orm("main",true);
                                        psh_orm->set_day(reference_date.getDateString());
                                        psh_orm->set_installment_extension_id(ie_orm->get_installment_ptr_id());
                                        psh_orm->set_status(0); // 0
                                    }
                                }
                            }

                            if ( fb > ie_orm->get_fra_status_id() )
                            {
                                new_lms_installmentstatushistory_primitive_orm * ish_orm = new new_lms_installmentstatushistory_primitive_orm("main",true, false);
                                ish_orm->set_day(reference_date.getDateString());
                                ish_orm->set_status_type (1);
                                ish_orm->set_installment_id (ie_orm->get_installment_ptr_id());
                                ish_orm->set_status_id(fb);
                                ish_orm->set_previous_status_id(ie_orm->get_fra_status_id());

                                if ( fb > lal_orm->get_fra_status_id() )
                                {
                                    loan_app_loanstatushistroy_primitive_orm * lsh_orm = new loan_app_loanstatushistroy_primitive_orm("main",true);
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

        auto afterProcess = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(afterProcess - beforeProcess);
        printf("PROCESSTIME status->: %.3f seconds.\n", elapsed.count() * 1e-9);

        cout << "processed " << psqlQueryJoin->getResultCount() << " record(s)" << endl;
        psqlController.ORMCommit(true,true,true, "main");   

        auto afterCommit = std::chrono::high_resolution_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(afterCommit - afterProcess);
        printf("COMMITTIME status->: %.3f seconds.\n", elapsed.count() * 1e-9);


        delete (psqlQueryJoin);
        PSQLUpdateQuery psqlUpdateQuery ("main","loan_app_loan",
        ANDOperator (
                new UnaryOperator ("loan_app_loan.lms_closure_status",lt,closure_status::UPDATE_LOAN_STATUS),
                new UnaryOperator ("loan_app_loan.id",ne,"14312"),
                new UnaryOperator ("loan_app_loan.lms_closure_status",gte,0),
                isMultiMachine ? new BinaryOperator ("loan_app_loan.id",mod,mod_value,eq,offset) : new BinaryOperator(),
                isLoanSpecific ? new UnaryOperator ("loan_app_loan.id", in, loan_ids) : new UnaryOperator() 
        ),
        {{"lms_closure_status",to_string(closure_status::UPDATE_LOAN_STATUS)}}
        );
        psqlUpdateQuery.update();

        cout << "Loan Status done" << endl;

        auto end = std::chrono::high_resolution_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);

        printf("Total Time-> %.3f seconds.\n", elapsed.count() * 1e-9);
    }

    //-----------------------------------------------------------------------------------------------------------------------------------------//
    //-----------------------------------------------------------------------------------------------------------------------------------------//


    if ( strcmp (step,"marginalization") == 0 || strcmp (step,"full_closure") == 0 || strcmp (step,"payment_closure") == 0)
    {
        auto begin = std::chrono::high_resolution_clock::now();

        PSQLJoinQueryIterator *  psqlQueryJoin = new PSQLJoinQueryIterator ("main",
        {new new_lms_installmentextension_primitive_orm("main"),new loan_app_installment_primitive_orm("main"),new loan_app_loan_primitive_orm("main"),new new_lms_installmentlatefees_primitive_orm("main")},
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
                new UnaryOperator ("loan_app_loan.lms_closure_status",eq,to_string(closure_status::MARGINALIZE_INCOME_STEP1-1)),
                new UnaryOperator ("loan_app_loan.status_id",nin,"1,6, 7, 8, 12, 13, 14, 15, 16"),
                new UnaryOperator ("new_lms_installmentlatefees.is_cancelled",eq,"f"),

                //New
                new UnaryOperator ("new_lms_installmentextension.is_marginalized", eq, "t"),
                new UnaryOperator ("new_lms_installmentextension.marginalization_date", lte,closure_date_string),
                isMultiMachine ? new BinaryOperator ("loan_app_loan.id",mod,mod_value,eq,offset) : new BinaryOperator(),
                isLoanSpecific ? new UnaryOperator ("loan_app_loan.id", in, loan_ids) : new UnaryOperator() 
            )
        );


        auto beforeProcess = std::chrono::high_resolution_clock::now();
        cout << "THREADTIME --> marginalization step 1" << endl;

        psqlQueryJoin->process (threadsCount,[closure_date_string](map <string,PSQLAbstractORM *> * orms,int partition_number,mutex * shared_lock) { 
                new_lms_installmentextension_primitive_orm * ie_orm  = ORM(new_lms_installmentextension,orms);
                new_lms_installmentlatefees_primitive_orm * lf_orm = ORM(new_lms_installmentlatefees,orms);
                loan_app_loan_primitive_orm * lal_orm  = ORM(loan_app_loan,orms);
                loan_app_installment_primitive_orm * lai_orm  = ORM(loan_app_installment,orms);

                    BDate marg_date; 
                    marg_date.set_date(lf_orm->get_day());

                    if ( marg_date.getDateString() != "" )
                    {
                        lf_orm->set_is_marginalized(true);
                        lf_orm->set_marginalization_date(marg_date.getDateString());
                    }
        });

        auto afterProcess = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(afterProcess - beforeProcess);
        printf("PROCESSTIME Marginalization step 1->: %.3f seconds.\n", elapsed.count() * 1e-9);

        cout << "processed " << psqlQueryJoin->getResultCount() << " record(s)" << endl;
        psqlController.ORMCommit(true,true,true, "main");   

        auto afterCommit = std::chrono::high_resolution_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(afterCommit - afterProcess);
        printf("COMMITTIME Marg 1->: %.3f seconds.\n", elapsed.count() * 1e-9);

        delete (psqlQueryJoin);

        cout << "Marginalization Setp 1" << endl;

        // Stop measuring time and calculate the elapsed time
        auto end = std::chrono::high_resolution_clock::now();
         elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);

        printf("Total Time-> %.3f seconds.\n", elapsed.count() * 1e-9);
    
    //-----------------------------------------------------------------------------------------------------------------//
    //-----------------------------------------------------------------------------------------------------------------//
    

        begin = std::chrono::high_resolution_clock::now();


        psqlQueryJoin = new PSQLJoinQueryIterator ("main",
        {new new_lms_installmentextension_primitive_orm("main"),new loan_app_installment_primitive_orm("main"),new loan_app_loan_primitive_orm("main"),new crm_app_customer_primitive_orm("main")},
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
                new UnaryOperator ("loan_app_loan.status_id",nin,"1,6, 7, 8, 12, 13, 14, 15, 16"),
                new UnaryOperator ("loan_app_installment.interest_expected",ne,"0"),
                new UnaryOperator ("crm_app_customer.first_loan_cycle_id",ne,"1"),
                isMultiMachine ? new BinaryOperator ("loan_app_loan.id",mod,mod_value,eq,offset) : new BinaryOperator(),
                isLoanSpecific ? new UnaryOperator ("loan_app_loan.id", in, loan_ids) : new UnaryOperator() 
            )
        );

        beforeProcess = std::chrono::high_resolution_clock::now();

        cout << "THREADTIME --> marginalization step 2" << endl;


        psqlQueryJoin->process (threadsCount,[](map <string,PSQLAbstractORM *> * orms,int partition_number,mutex * shared_lock) { 
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
            });

        afterProcess = std::chrono::high_resolution_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(afterProcess - beforeProcess);
        printf("PROCESSTIME Marginalization step 2->: %.3f seconds.\n", elapsed.count() * 1e-9);

        cout << "processed " << psqlQueryJoin->getResultCount() << " record(s)" << endl;
        psqlController.ORMCommit(true,true,true, "main");   

        afterCommit = std::chrono::high_resolution_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(afterCommit - afterProcess);
        printf("COMMITTIME Marg 2->: %.3f seconds.\n", elapsed.count() * 1e-9);

        delete (psqlQueryJoin);

        cout << "Marginalization Setp 2" << endl;

        // Stop measuring time and calculate the elapsed time
         end = std::chrono::high_resolution_clock::now();
         elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);

        printf("Total Time-> %.3f seconds.\n", elapsed.count() * 1e-9);
    //-----------------------------------------------------------------------------------------------------------------------------------------//
    //-----------------------------------------------------------------------------------------------------------------------------------------//

         begin = std::chrono::high_resolution_clock::now();


        psqlQueryJoin = new PSQLJoinQueryIterator ("main",
        {new new_lms_installmentextension_primitive_orm("main"),new loan_app_installment_primitive_orm("main"),new loan_app_loan_primitive_orm("main")},
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
                new UnaryOperator ("loan_app_loan.status_id",nin,"1,6, 7, 8, 12, 13, 14, 15, 16"),
                new UnaryOperator ("loan_app_installment.interest_expected",ne,"0"),
                isMultiMachine ? new BinaryOperator ("loan_app_loan.id",mod,mod_value,eq,offset) : new BinaryOperator(),
                isLoanSpecific ? new UnaryOperator ("loan_app_loan.id", in, loan_ids) : new UnaryOperator() 
            )
        );

        beforeProcess = std::chrono::high_resolution_clock::now();
        
        cout << "THREADTIME --> marginalization step 3" << endl;

        psqlQueryJoin->process (threadsCount,[](map <string,PSQLAbstractORM *> * orms,int partition_number,mutex * shared_lock) { 
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
            });

        afterProcess = std::chrono::high_resolution_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(afterProcess - beforeProcess);
        printf("PROCESSTIME Marginalization Step 3->: %.3f seconds.\n", elapsed.count() * 1e-9);

        cout << "processed " << psqlQueryJoin->getResultCount() << " record(s)" << endl;
        psqlController.ORMCommit(true,true,true, "main");  

        afterCommit = std::chrono::high_resolution_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(afterCommit - afterProcess);
        printf("COMMITTIME Marg3->: %.3f seconds.\n", elapsed.count() * 1e-9);

        delete (psqlQueryJoin);
        cout << "Marginalization Setp 3" << endl;

        // Stop measuring time and calculate the elapsed time
         end = std::chrono::high_resolution_clock::now();
         elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);

        printf("Total Time-> %.3f seconds.\n", elapsed.count() * 1e-9);
    //-----------------------------------------------------------------------------------------------------------------------------------------//
    //-----------------------------------------------------------------------------------------------------------------------------------------//

         begin = std::chrono::high_resolution_clock::now();


        psqlQueryJoin = new PSQLJoinQueryIterator ("main",
        {new new_lms_installmentextension_primitive_orm("main"),new loan_app_installment_primitive_orm("main"),new loan_app_loan_primitive_orm("main"),new new_lms_installmentlatefees_primitive_orm("main")},
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
                new UnaryOperator ("loan_app_loan.lms_closure_status",eq,to_string(closure_status::MARGINALIZE_INCOME_STEP1-1)),
                new UnaryOperator ("loan_app_loan.status_id",nin,"1,6, 7, 8, 12, 13, 14, 15, 16"),
                new UnaryOperator ("new_lms_installmentlatefees.is_cancelled",eq,"f"),

                //New
                new UnaryOperator ("loan_app_loan.status_id", gt, "loan_app_loan.marginalization_bucket_id", true),
                //

                isMultiMachine ? new BinaryOperator ("loan_app_loan.id",mod,mod_value,eq,offset) : new BinaryOperator(),
                isLoanSpecific ? new UnaryOperator ("loan_app_loan.id", in, loan_ids) : new UnaryOperator() 
            )
        );


        beforeProcess = std::chrono::high_resolution_clock::now();
        
        cout << "THREADTIME --> marginalization step 3" << endl;

        psqlQueryJoin->process (threadsCount,[closure_date_string](map <string,PSQLAbstractORM *> * orms,int partition_number,mutex * shared_lock) { 
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
    });

        afterProcess = std::chrono::high_resolution_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(afterProcess - beforeProcess);
        printf("PROCESSTIME Marginalization step 4->: %.3f seconds.\n", elapsed.count() * 1e-9);

        cout << "processed " << psqlQueryJoin->getResultCount() << " record(s)" << endl;
        psqlController.ORMCommit(true,true,true, "main");   

        afterCommit = std::chrono::high_resolution_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(afterCommit - afterProcess);
        printf("COMMITTIME Marg 4->: %.3f seconds.\n", elapsed.count() * 1e-9);

        delete (psqlQueryJoin);
        PSQLUpdateQuery psqlUpdateQuery ("main","loan_app_loan",
        ANDOperator (
                new UnaryOperator ("loan_app_loan.lms_closure_status",lt,closure_status::MARGINALIZE_INCOME_STEP1),
                new UnaryOperator ("loan_app_loan.id",ne,"14312"),
                new UnaryOperator ("loan_app_loan.lms_closure_status",gte,0),
                isMultiMachine ? new BinaryOperator ("loan_app_loan.id",mod,mod_value,eq,offset) : new BinaryOperator(),
                isLoanSpecific ? new UnaryOperator ("loan_app_loan.id", in, loan_ids) : new UnaryOperator() 
        ),

        {{"lms_closure_status",to_string(closure_status::MARGINALIZE_INCOME_STEP1)}}
        );
        psqlUpdateQuery.update();
        cout << "Marginalization Setp 4" << endl;

        end = std::chrono::high_resolution_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);

        printf("Total Time-> %.3f seconds.\n", elapsed.count() * 1e-9);
    }


    //-----------------------------------------------------------------------------------------------------------------------------------------//
    //-----------------------------------------------------------------------------------------------------------------------------------------//

    if ( strcmp (step,"long_to_short") == 0 || strcmp (step,"full_closure") == 0 || strcmp (step,"payment_closure") == 0)
    {
        auto begin = std::chrono::high_resolution_clock::now();


        PSQLJoinQueryIterator *  psqlQueryJoin = new PSQLJoinQueryIterator ("main",
        {new new_lms_installmentextension_primitive_orm("main"),new loan_app_installment_primitive_orm("main"),new loan_app_loan_primitive_orm("main")},
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
                ),
                isMultiMachine ? new BinaryOperator ("loan_app_loan.id",mod,mod_value,eq,offset) : new BinaryOperator(),
                isLoanSpecific ? new UnaryOperator ("loan_app_loan.id", in, loan_ids) : new UnaryOperator() 
            )
        );

        auto beforeProcess = std::chrono::high_resolution_clock::now();

        cout << "THREADTIME --> long_to_short" << endl;

        psqlQueryJoin->process (threadsCount,[](map <string,PSQLAbstractORM *> * orms,int partition_number,mutex * shared_lock) { 
                new_lms_installmentextension_primitive_orm * ie_orm  = ORM(new_lms_installmentextension,orms);
                loan_app_loan_primitive_orm * lal_orm  = ORM(loan_app_loan,orms);
                ie_orm->set_is_long_term(false); 
                lal_orm->set_lms_closure_status(closure_status::LONG_TO_SHORT_TERM);
            });

        auto afterProcess = std::chrono::high_resolution_clock::now();
        auto  elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(afterProcess - beforeProcess);
        printf("PROCESSTIME long_to_short->: %.3f seconds.\n", elapsed.count() * 1e-9);

        cout << "processed " << psqlQueryJoin->getResultCount() << " record(s)" << endl;
        psqlController.ORMCommit(true,true,true, "main");   

        auto afterCommit = std::chrono::high_resolution_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(afterCommit - afterProcess);
        printf("COMMITTIME long_to_short->: %.3f seconds.\n", elapsed.count() * 1e-9);

        delete (psqlQueryJoin);
        PSQLUpdateQuery psqlUpdateQuery ("main","loan_app_loan",
            ANDOperator (
                    new UnaryOperator ("loan_app_loan.lms_closure_status",lt,closure_status::LONG_TO_SHORT_TERM),
                    new UnaryOperator ("loan_app_loan.id",ne,"14312"),
                    new UnaryOperator ("loan_app_loan.lms_closure_status",gte,0),
                    isMultiMachine ? new BinaryOperator ("loan_app_loan.id",mod,mod_value,eq,offset) : new BinaryOperator(),
                    isLoanSpecific ? new UnaryOperator ("loan_app_loan.id", in, loan_ids) : new UnaryOperator() 
            ),

            {{"lms_closure_status",to_string(closure_status::LONG_TO_SHORT_TERM)}}
        );
        psqlUpdateQuery.update();
        cout << "Long to short term" << endl;

        auto end = std::chrono::high_resolution_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);

        printf("Total Time-> %.3f seconds.\n", elapsed.count() * 1e-9);
    }

    //-----------------------------------------------------------------------------------------------------------------------------------------//
    //-----------------------------------------------------------------------------------------------------------------------------------------//



    if ( strcmp (step,"last_accrual_interest_date") == 0 || strcmp (step,"full_closure") == 0 || strcmp (step,"payment_closure") == 0)
    {
        auto begin = std::chrono::high_resolution_clock::now();


        PSQLJoinQueryIterator *  psqlQueryJoin = new PSQLJoinQueryIterator ("main",
        {new new_lms_installmentextension_primitive_orm("main"),new loan_app_installment_primitive_orm("main"),new loan_app_loan_primitive_orm("main")},
        {{{"loan_app_installment","loan_id"},{"loan_app_loan","id"}},
        {{"loan_app_installment","id"},{"new_lms_installmentextension","installment_ptr_id"}},
        });



        //CHANGED TO DECENDING
        psqlQueryJoin->setOrderBy("loan_app_loan.id desc ,new_lms_installmentextension.accrual_date desc");
        psqlQueryJoin->filter(
            ANDOperator 
            (
                new UnaryOperator ("loan_app_loan.status_id",nin,"6, 8, 12, 13, 15, 16"),
                new UnaryOperator ("loan_app_loan.lms_closure_status",eq,to_string(closure_status::LAST_ACCRUED_DAY-1)), 
                new OROperator (
                    new ANDOperator(
                        new UnaryOperator ("new_lms_installmentextension.partial_accrual_date",lte,closure_date_string),
                        new UnaryOperator ("loan_app_loan.last_accrued_interest_day",lt,"new_lms_installmentextension.partial_accrual_date", true)
                    ),
                    new ANDOperator(
                        new UnaryOperator ("new_lms_installmentextension.accrual_date",lte,closure_date_string),
                        new UnaryOperator ("loan_app_loan.last_accrued_interest_day",lt,"new_lms_installmentextension.accrual_date", true)
                    ),
                    new ANDOperator(
                        new UnaryOperator ("loan_app_loan.first_accrual_adjustment_date",lte,closure_date_string),
                        new UnaryOperator ("loan_app_loan.last_accrued_interest_day",lt,"loan_app_loan.first_accrual_adjustment_date", true)
                    )
                ),
                isMultiMachine ? new BinaryOperator ("loan_app_loan.id",mod,mod_value,eq,offset) : new BinaryOperator(),
                isLoanSpecific ? new UnaryOperator ("loan_app_loan.id", in, loan_ids) : new UnaryOperator() 
            )
        );

        auto beforeProcess = std::chrono::high_resolution_clock::now();

        cout << "THREADTIME --> last_accrual_interest_date" << endl;

        psqlQueryJoin->process (threadsCount,[&closure_date](map <string,PSQLAbstractORM *> * orms,int partition_number,mutex * shared_lock) { 
                new_lms_installmentextension_primitive_orm * ie_orm  = ORM(new_lms_installmentextension,orms);
                loan_app_loan_primitive_orm * lal_orm  = ORM(loan_app_loan,orms);
                BDate accrual_date(ie_orm->get_accrual_date());
                BDate partial_accrual_date(ie_orm->get_partial_accrual_date());
                BDate first_accrual_adjustment_date(lal_orm->get_first_accrual_adjustment_date());
                BDate last_accrued_interest_day(lal_orm->get_last_accrued_interest_day());
                BDate new_last_accrued_interest_day (lal_orm->get_loan_booking_day());
                // new_last_accrued_interest_day.dec_day();

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
            });

        auto afterProcess = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(afterProcess - beforeProcess);
        printf("PROCESSTIME last_accrual_interest_date->: %.3f seconds.\n", elapsed.count() * 1e-9);

        cout << "processed " << psqlQueryJoin->getResultCount() << " record(s)" << endl;
        psqlController.ORMCommit(true,true,true, "main");   

        auto afterCommit = std::chrono::high_resolution_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(afterCommit - afterProcess);
        printf("COMMITTIME last_accrual_interest_date->: %.3f seconds.\n", elapsed.count() * 1e-9);

        delete (psqlQueryJoin);
        PSQLUpdateQuery psqlUpdateQuery ("main","loan_app_loan",
            ANDOperator (
                    new UnaryOperator ("loan_app_loan.lms_closure_status",lt,closure_status::LAST_ACCRUED_DAY),
                    new UnaryOperator ("loan_app_loan.id",ne,"14312"),
                    new UnaryOperator ("loan_app_loan.lms_closure_status",gte,0),
                    isMultiMachine ? new BinaryOperator ("loan_app_loan.id",mod,mod_value,eq,offset) : new BinaryOperator(),
                    isLoanSpecific ? new UnaryOperator ("loan_app_loan.id", in, loan_ids) : new UnaryOperator() 
            ),

            {{"lms_closure_status",to_string(closure_status::LAST_ACCRUED_DAY)}}
        );
        psqlUpdateQuery.update();
        cout << "Loan Last Accrual Day" << endl;

        auto end = std::chrono::high_resolution_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);

        printf("Total Time Step-> %.3f seconds.\n", elapsed.count() * 1e-9);

    }


    if ((strcmp (step,"customer_wallet") == 0 || strcmp (step, "full_closure") == 0) || strcmp (step,"payment_closure") == 0) 
    {   
        printf("Beginning customer wallet closure\n");
        
        auto begin = std::chrono::high_resolution_clock::now();

        PSQLJoinQueryIterator *  psqlQueryJoin = new PSQLJoinQueryIterator ("main",
            {
                new crm_app_customer_primitive_orm("main"), new new_lms_customerwallet_primitive_orm("main"),
                new loan_app_loan_primitive_orm("main"), new new_lms_installmentextension_primitive_orm("main"),new loan_app_installment_primitive_orm("main"),},
            {   
                {{"crm_app_customer","id"},{"loan_app_loan","customer_id"}},
                {{"crm_app_customer","id"},{"new_lms_customerwallet","customer_id"}},
                {{"loan_app_installment","loan_id"},{"loan_app_loan","id"}},
                {{"loan_app_installment","id"},{"new_lms_installmentextension","installment_ptr_id"}},
        });

        psqlQueryJoin->filter(
            ANDOperator 
            (
                isLoanSpecific ? new UnaryOperator ("loan_app_loan.id", in, loan_ids) : new UnaryOperator(),
                new UnaryOperator ("loan_app_loan.status_id",eq,11),
                new UnaryOperator ("loan_app_loan.lms_closure_status",eq,to_string(closure_status::CUSTOMER_WALLET-1)), 
                new UnaryOperator ("loan_app_installment.interest_expected",ne,0),
                new UnaryOperator ("loan_app_installment.interest_expected",lte, "new_lms_customerwallet.total_amount", true),
                new UnaryOperator ("new_lms_installmentextension.is_principal_paid",eq,true),
                new UnaryOperator ("new_lms_installmentextension.is_interest_paid",eq,false),
                new UnaryOperator ("new_lms_installmentextension.undue_to_due_date",lte,closure_date_string),
                isMultiMachine ? new BinaryOperator ("loan_app_loan.id",mod,mod_value,eq,offset) : new BinaryOperator()
            )
        );

        cout << "THREADTIME --> Wallet" << endl;


        // TODO:     change to new implementation when implemented
        psqlQueryJoin->setDistinct("distinct phone_number as \"162_phone_number\", \"crm_app_customer\".\"id\" as \"162_id\"");

        auto beforeProcess = std::chrono::high_resolution_clock::now();
                
        string failed_customers_ids = "";
        psqlQueryJoin->process (threadsCount,[&failed_customers_ids](map <string,PSQLAbstractORM *> * orms,int partition_number,mutex * shared_lock) {
            crm_app_customer_primitive_orm * cac_orm  = ORM(crm_app_customer,orms);
            cout << partition_number << ": " << cac_orm->get_phone_number() << endl; 
            bool success = closure_go(cac_orm->get_phone_number());
            if (!success){
                cout << cac_orm->get_phone_number() << ": failed" << endl; 
                failed_customers_ids += (to_string(cac_orm->get_id()) + ",");
            }
        });

        auto afterProcess = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(afterProcess - beforeProcess);
        printf("PROCESSTIME Wallet->: %.3f seconds.\n", elapsed.count() * 1e-9);

        // update loans of failed customers
        if (failed_customers_ids.length() > 0) {
            failed_customers_ids.pop_back();
            PSQLUpdateQuery failedUpdateQuery ("main","loan_app_loan",
            ANDOperator (
                    new UnaryOperator ("loan_app_loan.lms_closure_status",lt,closure_status::CUSTOMER_WALLET),
                    new UnaryOperator ("loan_app_loan.lms_closure_status",gte,0),
                    new UnaryOperator ("loan_app_loan.id",ne,"14312"),
                    new UnaryOperator ("loan_app_loan.customer_id", in, failed_customers_ids),
                    isMultiMachine ? new BinaryOperator ("loan_app_loan.id",mod,mod_value,eq,offset) : new BinaryOperator()
            ),
            {{"lms_closure_status",to_string(closure_status::CUSTOMER_WALLET * -1)}}
            );
            failedUpdateQuery.update();
        }

        // update all non-failed loans
        PSQLUpdateQuery successUpdateQuery ("main","loan_app_loan",
        ANDOperator (
                new UnaryOperator ("loan_app_loan.lms_closure_status",lt,closure_status::CUSTOMER_WALLET),
                new UnaryOperator ("loan_app_loan.lms_closure_status",gte,0),
                new UnaryOperator ("loan_app_loan.id",ne,"14312"),
                isLoanSpecific ? new UnaryOperator ("loan_app_loan.id", in, loan_ids) : new UnaryOperator(),
                isMultiMachine ? new BinaryOperator ("loan_app_loan.id",mod,mod_value,eq,offset) : new BinaryOperator()
        ),
        {{"lms_closure_status",to_string(closure_status::CUSTOMER_WALLET)}}
        );
        successUpdateQuery.update();
        
        auto end = std::chrono::high_resolution_clock::now();
        auto total_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
        printf("Total Time Step-> %.3f seconds.\n", total_time.count() * 1e-9);
        printf("\n");
    }


    map <string,string> set_map;
    if (strcmp (step,"full_closure") == 0)
    {
        BDate closure_yesterday = closure_date;
        closure_yesterday.dec_day();

        set_map = {{"lms_closure_status",to_string(0)}, {"last_lms_closing_day",closure_yesterday.getDateString()}};
    } 
    else if (strcmp (step,"payment_closure") == 0) 
    {    
        set_map = {{"last_lms_closing_day",closure_date.getDateString()}};
    }

    if (set_map.size() > 0) {
        PSQLUpdateQuery lastUpdateQuery ("main","loan_app_loan",
            ANDOperator(
                new UnaryOperator ("loan_app_loan.lms_closure_status",gte,closure_status::CUSTOMER_WALLET),
                new UnaryOperator ("loan_app_loan.id",ne,"14312"),
                new UnaryOperator ("loan_app_loan.lms_closure_status",gte,0),
                isMultiMachine ? new BinaryOperator ("loan_app_loan.id",mod,mod_value,eq,offset) : new BinaryOperator(),
                isLoanSpecific ? new UnaryOperator ("loan_app_loan.id", in, loan_ids) : new UnaryOperator()
            ),
            {set_map}
        );
        lastUpdateQuery.update();
    }
   
    return 0;
}

//-----------------------------------------------------------------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------------------------------------------------------------//
// Closure Wallet in .so file from GO
//-----------------------------------------------------------------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------------------------------------------------------------//

typedef long long go_int;

typedef struct {
    char* data;
    go_int len;
} go_string;

 typedef struct {
    go_string* data;
    go_int len;
    go_int cap;
} go_slice;

typedef bool (*Func)(go_string);

void *handle = nullptr;
Func closure = nullptr;


bool closure_go (string phone_number) {
    const string so_name = "./shared/closure_wallet.so";
    const string func_name = "ProcessCustomerChargedWallets";

    if (handle == nullptr) {
        // use dlopen to load shared object
        handle = dlopen (so_name.c_str(), RTLD_LAZY);
        if (!handle) {
            cout << dlerror() << '\n';
            return 0;
        }
        printf("*******Loaded .so file\n");
    }

    // resolve function symbol
    if (closure == nullptr){
        closure = (Func)dlsym(handle, func_name.c_str());
        if (closure == nullptr)  {
            cout << dlerror() << '\n';
            return 0;
        }
    }

    // Prepare data
    go_string go_phonenumber;
    go_phonenumber.data = const_cast<char*>(phone_number.c_str());
    go_phonenumber.len = phone_number.length();

    // Call function
    bool output = closure(go_phonenumber);

    cout << "***********called closure go\n";

    return output;
}
