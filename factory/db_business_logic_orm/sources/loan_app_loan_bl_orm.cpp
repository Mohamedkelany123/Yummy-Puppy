#include <loan_app_loan_bl_orm.h>

vector <new_lms_installmentextension_primitive_orm *> * loan_app_loan_bl_orm::get_new_lms_installmentextension_loan_id(bool _read_only){
		if (new_lms_installmentextension_loan_id== NULL) {
			new_lms_installmentextension_loan_id_read_only = _read_only;
			new_lms_installmentextension_loan_id = new vector <new_lms_installmentextension_primitive_orm *> ();
			new_lms_installmentextension_primitive_orm_iterator * i = new new_lms_installmentextension_primitive_orm_iterator("main");
			string sub_query = "(select id from loan_app_installment where loan_id="+to_string(get_id())+")";
			i->filter (ANDOperator(new UnaryOperator("installment_ptr_id",in,sub_query)));
			i->execute();
			new_lms_installmentextension_primitive_orm * orm = NULL;
			do {
				orm = i->next(_read_only);
				if (orm!= NULL) new_lms_installmentextension_loan_id->push_back(orm);
			} while (orm != NULL);
			delete(i);
		}
		return new_lms_installmentextension_loan_id;
}

vector <ledger_amount_primitive_orm *> * loan_app_loan_bl_orm::get_ledger_amount_loan_id(bool _read_only){
		if (ledger_amount_loan_id == NULL) {
			ledger_amount_loan_id_read_only = _read_only;
			ledger_amount_loan_id = new vector <ledger_amount_primitive_orm *> ();
			ledger_amount_primitive_orm_iterator * i = new ledger_amount_primitive_orm_iterator("main", enforced_partition_number);
			i->filter (ANDOperator(new UnaryOperator("loan_id",eq,to_string(get_id()))));
			i->execute();
			ledger_amount_primitive_orm * orm = NULL;
			do {
				orm = i->next(_read_only);
				if (orm!= NULL) ledger_amount_loan_id->push_back(orm);
			} while (orm != NULL);
			delete(i);
		}
		return ledger_amount_loan_id;
}


loan_app_loan_bl_orm::loan_app_loan_bl_orm(string _data_source_name, bool add_to_cache, bool orm_transactional,int _enforced_partition_number,vector <string> _field_clear_mask): loan_app_loan_primitive_orm(_data_source_name,add_to_cache,  orm_transactional,_enforced_partition_number,_field_clear_mask) {
	new_lms_installmentextension_loan_id_read_only = false;
	new_lms_installmentextension_loan_id=NULL;
	ledger_amount_loan_id=NULL;
	ledger_amount_loan_id_read_only= false;

	// orm_name is not set right in this

}
PSQLAbstractORM * loan_app_loan_bl_orm::clone (){
			return new loan_app_loan_bl_orm(data_source_name);
}
loan_app_loan_bl_orm::~loan_app_loan_bl_orm(){
	if (new_lms_installmentextension_loan_id!= NULL){
		if (new_lms_installmentextension_loan_id_read_only)
			for (auto orm :(*new_lms_installmentextension_loan_id)) delete (orm);
		delete (new_lms_installmentextension_loan_id);
	}
	if ( ledger_amount_loan_id != NULL){
		if (ledger_amount_loan_id_read_only)
			for (auto orm :(*ledger_amount_loan_id)) delete (orm);
		delete (ledger_amount_loan_id);
	}
}
#ifndef SKIP_ENTRY_POINT
extern "C" PSQLAbstractORM *create_object() {
	return new loan_app_loan_bl_orm();
}
#endif


loan_app_loan_bl_orm_iterator::loan_app_loan_bl_orm_iterator(string _data_source_name):PSQLAbstractQueryIterator(_data_source_name,"loan_app_loan"){
    loan_app_loan_bl_orm * orm = new loan_app_loan_bl_orm(_data_source_name);
    from_string =orm->getFromString();
    orderby_string = orm->getIdentifierName() + " asc";
	delete (orm); // This was originally missing and need to be tested
}
loan_app_loan_bl_orm * loan_app_loan_bl_orm_iterator::operator [] (long index)
{
    return NULL;
}
loan_app_loan_bl_orm * loan_app_loan_bl_orm_iterator::next (bool _read_only)
{
    if (psqlQuery->fetchNextRow())
    {
		loan_app_loan_bl_orm * obj = NULL;

		if ( _read_only)
		{
			obj = new loan_app_loan_bl_orm(data_source_name);
			obj->assignResults(psqlQuery,_read_only);
		}
		else
		{
			loan_app_loan_bl_orm * seeder = new loan_app_loan_bl_orm(data_source_name);
            obj= (loan_app_loan_bl_orm *)psqlController.addToORMCache(seeder,psqlQuery,-1,data_source_name);
			delete (seeder);

		}
        for (auto e: extras)
			obj->setExtra(e.first,psqlQuery->getValue(e.first));
        return obj;
    }
    else return NULL;
}
loan_app_loan_bl_orm * loan_app_loan_bl_orm_iterator::back ()
{
    return NULL;
}

void  loan_app_loan_bl_orm_iterator::process_internal(loan_app_loan_bl_orm_iterator * psqlAbstractQueryIterator,string data_source_name, PSQLQueryPartition * psqlQueryPartition,int partition_number,mutex * shared_lock,void * extra_params,std::function<void(loan_app_loan_bl_orm * orm,int partition_number,mutex * shared_lock,void * extra_params)> f)
{
        PSQLQueryPartitionIterator <loan_app_loan_bl_orm> psqlQueryPartitionIterator (psqlQueryPartition, data_source_name, extra_params, partition_number);
        loan_app_loan_bl_orm * orm = NULL;
        do {
            orm =psqlQueryPartitionIterator.next();
            if (orm != NULL) 
            {
                for (auto e: psqlAbstractQueryIterator->extras)
					orm->setExtra(e.first,psqlQueryPartition->getValue(e.first));
                f(orm,partition_number,shared_lock, extra_params);
                shared_lock->lock();
                orm->unlock_me();
                shared_lock->unlock();
            }
        } while (orm != NULL);
        psqlController.unlock_current_thread_orms(data_source_name);    
}

void loan_app_loan_bl_orm_iterator::process(int partitions_count,std::function<void(loan_app_loan_bl_orm * orm,int partition_number,mutex * shared_lock,void * extras)> f, void * extra_params)
{
    if (this->execute() && ((PSQLQuery *)this->psqlQuery)->getRowCount() > 0)
    {
        vector <PSQLQueryPartition * > * p = ((PSQLQuery *)this->psqlQuery)->partitionResults(partitions_count);
        vector <thread *> threads;
        mutex shared_lock;

        for ( int i  = 0 ; i < p->size() ; i ++)
        {
            thread * t = new thread(process_internal, this,data_source_name, (*p)[i],i,&shared_lock,extra_params,f);
            threads.push_back(t);
        }
        for ( int i  = 0 ; i < p->size() ; i ++)
        {
                thread * t = threads[i];
                t->join();
                delete (t);
        }
    }
}

int loan_app_loan_bl_orm_iterator::getRowCount(){
    return this->psqlQuery->getRowCount();
}

loan_app_loan_bl_orm_iterator::~loan_app_loan_bl_orm_iterator ()
{
    
}