#ifndef LOAN_APP_LOAN_BL_ORM_H
#define LOAN_APP_LOAN_BL_ORM_H

#include <loan_app_loan_primitive_orm.h>
#include <new_lms_installmentextension_primitive_orm.h>

class loan_app_loan_bl_orm : public loan_app_loan_primitive_orm
{
    private:

    protected:
		bool new_lms_installmentextension_loan_id_read_only;
		bool ledger_amount_loan_id_read_only;
		vector <new_lms_installmentextension_primitive_orm *> * new_lms_installmentextension_loan_id;
		vector <ledger_amount_primitive_orm *> * ledger_amount_loan_id;


    public:
		vector <new_lms_installmentextension_primitive_orm *> * get_new_lms_installmentextension_loan_id(bool _read_only=false);
		vector <ledger_amount_primitive_orm *> * get_ledger_amount_loan_id(bool _read_only=false);

		loan_app_loan_bl_orm(string data_source_name, bool add_to_cache=false, bool orm_transactional=true);
		PSQLAbstractORM * clone ();

		 virtual ~loan_app_loan_bl_orm();

};

class loan_app_loan_bl_orm_iterator : public PSQLAbstractQueryIterator
{
    private:
        static void  process_internal(loan_app_loan_bl_orm_iterator * psqlAbstractQueryIterator,string data_source_name, PSQLQueryPartition * psqlQueryPartition,int partitions_count,mutex * shared_lock,void * extra_params,std::function<void(loan_app_loan_bl_orm * orm,int partition_number,mutex * shared_lock,void * extra_params)> f);
    public:
        loan_app_loan_bl_orm_iterator(string _data_source_name);
        loan_app_loan_bl_orm * operator [] (long index);
        loan_app_loan_bl_orm * next (bool _read_only=false);
        loan_app_loan_bl_orm * back ();
        int getRowCount();
        void process(int partitions_count,std::function<void(loan_app_loan_bl_orm * orm,int partition_number,mutex * shared_lock,void * extra_params)> f, void * extra_params);
        ~loan_app_loan_bl_orm_iterator ();
};


#endif