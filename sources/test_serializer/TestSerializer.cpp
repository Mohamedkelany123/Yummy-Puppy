#include "TestSerializer.h"




TestSerializer::TestSerializer(string  _store_location, PSQLJoinQueryIterator * _psqlQueryJoin)
{
    store_location = _store_location;
    psqlQueryJoin = _psqlQueryJoin;
   
}



bool TestSerializer::generate_simple()
{
   
    return false
}

bool TestSerializer::generate_aggregate()
{
   
    return false
}


bool TestSerializer::commit_to_stream()
{
   
    return false
}

bool TestSerializer::generate(bool is_aggregate)
{
   if (is_aggregate)
    this->generate_aggregate();
   else
    this->generate_simple(); 
   

}




bool TestSerializer::write_to_stream(string orm_serialized)
{
    ofstream file(this->store);
    file << orm_serialized << endl;
    file.close();
    return false;
}