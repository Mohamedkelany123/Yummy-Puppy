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
loan_app_loan_bl_orm::loan_app_loan_bl_orm(string _data_source_name, bool add_to_cache, bool orm_transactional): loan_app_loan_primitive_orm(_data_source_name,  add_to_cache,  orm_transactional) {
	new_lms_installmentextension_loan_id_read_only = false;
	new_lms_installmentextension_loan_id=NULL;
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
}
#ifndef SKIP_ENTRY_POINT
extern "C" PSQLAbstractORM *create_object() {
	return new loan_app_loan_bl_orm();
}
#endif


// loan_app_loan_primitive_orm_iterator::loan_app_loan_primitive_orm_iterator(string _data_source_name):PSQLAbstractQueryIterator(_data_source_name,"loan_app_loan"){
//     loan_app_loan_primitive_orm * orm = new loan_app_loan_primitive_orm(_data_source_name);
//     from_string =orm->getFromString();
//     orderby_string = orm->getIdentifierName() + " asc";
// }
// loan_app_loan_primitive_orm * loan_app_loan_primitive_orm_iterator::operator [] (long index)
// {
//     return NULL;
// }
// loan_app_loan_primitive_orm * loan_app_loan_primitive_orm_iterator::next (bool _read_only)
// {
//     if (psqlQuery->fetchNextRow())
//     {
//         loan_app_loan_primitive_orm * obj = new loan_app_loan_primitive_orm(data_source_name);
//         obj->assignResults(psqlQuery,_read_only);
//         return obj;
//     }
//     else return NULL;
// }
// loan_app_loan_primitive_orm * loan_app_loan_primitive_orm_iterator::back ()
// {
//     return NULL;
// }

// void  loan_app_loan_primitive_orm_iterator::process_internal(string data_source_name, PSQLQueryPartition * psqlQueryPartition,int partition_number,mutex * shared_lock,std::function<void(loan_app_loan_primitive_orm * orm,int partition_number,mutex * shared_lock)> f)
// {
//         PSQLQueryPartitionIterator <loan_app_loan_primitive_orm> psqlQueryPartitionIterator (psqlQueryPartition, data_source_name);
//         loan_app_loan_primitive_orm * orm = NULL;
//         do {
//             orm =psqlQueryPartitionIterator.next();
//             if (orm != NULL) 
//             {
//                 f(orm,partition_number,shared_lock);
//             }
//         } while (orm != NULL);
    
// }

// void loan_app_loan_primitive_orm_iterator::process(int partitions_count,std::function<void(loan_app_loan_primitive_orm * orm,int partition_number,mutex * shared_lock)> f)
// {
//     if (this->execute())
//     {
//         vector <PSQLQueryPartition * > * p = ((PSQLQuery *)this->psqlQuery)->partitionResults(partitions_count);
//         vector <thread *> threads;
//         mutex shared_lock;
//         for ( int i  = 0 ; i < p->size() ; i ++)
//         {
//             thread * t = new thread(process_internal, data_source_name, (*p)[i],i,&shared_lock,f);
//             threads.push_back(t);
//         }
//         for ( int i  = 0 ; i < p->size() ; i ++)
//         {
//                 thread * t = threads[i];
//                 t->join();
//                 delete (t);
//         }
//     }
// }

// int loan_app_loan_primitive_orm_iterator::getRowCount(){
//     return this->psqlQuery->getRowCount();
// }

// loan_app_loan_primitive_orm_iterator::~loan_app_loan_primitive_orm_iterator ()
// {
    
// }