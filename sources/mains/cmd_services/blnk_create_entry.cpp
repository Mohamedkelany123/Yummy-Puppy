#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <PSQLController.h>
#include <TemplateManager.h>
#include <DisburseLoans.h>
#include <common_orm.h>

#include <lms_entrytemplate_primitive_orm.h>
#include <loan_app_loanstatus_primitive_orm.h>
#include <loan_app_loan_bl_orm.h>
//TODO: create special type for 

//<BuckedId,Percentage>
map<int,float> get_loan_status_provisions_percentage()
{
        //Query to return percentage from loan_app_provision
        PSQLJoinQueryIterator * psqlQueryJoinProvisions = new PSQLJoinQueryIterator ("main",
        {new loan_app_loanstatus_primitive_orm("main"),new loan_app_provision_primitive_orm("main")},
        {{{"loan_app_loanstatus","id"},{"loan_app_provision","status_id"}}});

        
        map<int,float> bucket_percentage;

        psqlQueryJoinProvisions->execute();
        map<string, PSQLAbstractORM *>* orms = psqlQueryJoinProvisions->next();
        loan_app_loanstatus_primitive_orm * lals_orm;
        loan_app_provision_primitive_orm * lap_orm; 
        while (orms != NULL){
            lals_orm = ORM(loan_app_loanstatus,orms);
            lap_orm = ORM(loan_app_provision,orms);
            bucket_percentage[lals_orm->get_id()] = lap_orm->get_percentage();
            orms = psqlQueryJoinProvisions->next();
        }

        return bucket_percentage;
}


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


    PSQLJoinQueryIterator * psqlQueryJoin = new PSQLJoinQueryIterator ("main",
    {new loan_app_loan_bl_orm("main"),new loan_app_loanproduct_primitive_orm("main"), new crm_app_customer_primitive_orm("main"), new crm_app_purchase_primitive_orm("main")},
    {{{"loan_app_loanproduct","id"},{"loan_app_loan","loan_product_id"}}, {{"loan_app_loan", "id"}, {"crm_app_purchase", "loan_id"}}, {{"loan_app_loan", "customer_id"}, {"crm_app_customer", "id"}}});

    psqlQueryJoin->addExtraFromField("(SELECT SUM(lai.principal_expected) FROM loan_app_installment lai INNER JOIN new_lms_installmentextension nli on nli.installment_ptr_id  = lai.id where nli.is_long_term = false and loan_app_loan.id = lai.loan_id)","short_term_principal");
    psqlQueryJoin->addExtraFromField("(SELECT SUM(lai.principal_expected) FROM loan_app_installment lai INNER JOIN new_lms_installmentextension nli on nli.installment_ptr_id  = lai.id where nli.is_long_term = true and loan_app_loan.id = lai.loan_id)","long_term_principal");
    psqlQueryJoin->addExtraFromField("(SELECT cap2.is_rescheduled FROM crm_app_purchase cap INNER JOIN crm_app_purchase cap2 ON cap.parent_purchase_id = cap2.id WHERE  cap.id = crm_app_purchase.id)","is_rescheduled");

    string closure_date_string = "2024-05-15"; 
    
    psqlQueryJoin->filter(
        ANDOperator 
        (
            new UnaryOperator ("loan_app_loan.closure_status",eq,to_string(ledger_status::DISBURSE_LOAN-1)),
            new UnaryOperator ("loan_app_loan.loan_creation_ledger_entry_id",isnull,"",true),
            new UnaryOperator ("loan_app_loan.loan_booking_day",lte,closure_date_string)
            // isMultiMachine ? new BinaryOperator ("loan_app_loan.id",mod,mod_value,eq,offset) : new BinaryOperator(),
            // isLoanSpecific ? new UnaryOperator ("loan_app_loan.id", in, loan_ids) : new UnaryOperator()
        )
    );

    BlnkTemplateManager * blnkTemplateManager = new BlnkTemplateManager(4);
    map<int,float> status_provision_percentage =  get_loan_status_provisions_percentage();


    float current_provision_percentage = status_provision_percentage[1];
    psqlQueryJoin->process (threadsCount,[blnkTemplateManager, current_provision_percentage](map <string,PSQLAbstractORM *> * orms,int partition_number,mutex * shared_lock) {
            cout << "INsidee processsssssss" << endl;
            loan_app_loan_bl_orm * lal_orm = ORMBL(loan_app_loan,orms);
            loan_app_loanproduct_primitive_orm * lalp_orm = ORM(loan_app_loanproduct,orms);
            crm_app_customer_primitive_orm * cac_orm = ORM(crm_app_customer,orms);
            PSQLGeneric_primitive_orm * gorm = ORM(PSQLGeneric,orms);
            float short_term_principal = gorm->toFloat("short_term_principal");
            float long_term_principal = gorm->toFloat("long_term_principal");
            bool is_rescheduled = gorm->toBool("is_rescheduled");

            cout << "HEREEE--> " << lal_orm->get_id() << "--" << is_rescheduled << endl;
            
            
            //TODO: Send orms iteself as a param
            DisburseLoan disburseLoan (lal_orm,cac_orm, lalp_orm, short_term_principal,long_term_principal, current_provision_percentage, is_rescheduled);

            
            LedgerClosureService * ledgerClosureService = new LedgerClosureService(&disburseLoan);
            disburseLoan.setupLedgerClosureService(ledgerClosureService);
            map <string,LedgerAmount> ledgerAmounts = ledgerClosureService->inference ();
            blnkTemplateManager->setEntryData(ledgerAmounts);
            ledger_entry_primitive_orm* entry = blnkTemplateManager->buildEntry(4,BDate("2024-05-15"));
            lal_orm->setUpdateRefernce("loan_creation_ledger_entry_id",entry);
            
            

            for(map<string, LedgerAmount>::iterator it=ledgerAmounts.begin(); it!=ledgerAmounts.end();it++) {
                cout << "leg name: " << it->first << " calculated amount: " << it->second.getAmount() << endl;
            }

            delete (ledgerClosureService);

            vector <pair <ledger_amount_primitive_orm*,ledger_amount_primitive_orm*>*>* leg_pairs = blnkTemplateManager->get_entry_orms();


            vector <new_lms_installmentextension_primitive_orm *> * ie_list = lal_orm->get_new_lms_installmentextension_loan_id();
            printf ("ie_list: %p \n",ie_list );
            cout << ie_list->size() << endl;
            for ( auto i : *ie_list)
            {
                cout << "_______________" << i->get_installment_ptr_id() << endl;
                if(!i->get_is_long_term()){
                    i->setUpdateRefernce("short_term_ledger_amount_id", (*leg_pairs)[0]->first);
                    cout << "LegTamplateID-->" <<  (*leg_pairs)[0]->first->get_leg_temple_id() << endl;
                }

            }



    });

    cout << "processed " << psqlQueryJoin->getResultCount() << " record(s)" << endl;
    psqlController.ORMCommit(true,true,true, "main");  

    return 0;
}
