#include "TestSerializer.h"




TestSerializer::TestSerializer(string  _store_location, PSQLJoinQueryIterator * _psqlQueryJoin)
{
    store_location = _store_location;
    psqlQueryJoin = _psqlQueryJoin;
   
}

void TestSerializer::gen_simple(map <string,PSQLAbstractORM *> * orms,int partition_number,mutex * shared_lock,void * extras){
    
    int len = orms->size(), i=0;
    stringstream ss;
    ss << "{" ;
    for (auto it = orms->begin(); it != orms->end(); ++it) {
        if(it->first != "PSQLGeneric"){
            ss << it->second->serialize();
            i++;
            if (i!=len-1) {
                ss << ",";
            }
        }
    }
    ss << ",\"extra\":{";
    PSQLGeneric_primitive_orm * gorm = ORM(PSQLGeneric,orms);
    if (gorm != nullptr){
        map<string, string> extraFields = gorm->getExtraFieldsMap();
        int extra_len=extraFields.size(), j=0;
        for(auto extras = extraFields.begin(); extras != extraFields.end(); ++extras){
                if (extras->second != ""){
                ss<<"\""<<extras->first<<"\":\""<< extras->second << "\"";
                j++;
                if(j!=extra_len) {
                    ss << ",";
                }
            }
        }
    }   
    ss << "}";
    ss << "}";
    ss<<endl;
    cout << ss.str();
    ((TestSerializer *)extras)->write_to_stream(ss.str());
}


bool TestSerializer::generate_simple()
{
    psqlQueryJoin->process_sequential(gen_simple, (void*)this);
    return false;
}

bool TestSerializer::generate_aggregate()
{
   
    return false;
}


bool TestSerializer::commit_to_stream()
{
   
    return false;
}

bool TestSerializer::generate(bool is_aggregate)
{
   if (is_aggregate)
    this->generate_aggregate();
   else
    this->generate_simple(); 
   return true;

}




bool TestSerializer::write_to_stream(string orm_serialized)
{
    ofstream file(this->store_location);
    file << orm_serialized << endl;
    file.close();
    return false;
}