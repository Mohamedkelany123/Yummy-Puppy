#include "TestSerializer.h"

TestSerializer::TestSerializer(string store_location, PSQLJoinQueryIterator *_psqlQueryJoin)
{
    this->store = store_location;
}

bool TestSerializer::write_to_stream(string orm_serialized)
{
    ofstream file(this->store);
    file << orm_serialized << endl;
    file.close();
    return false;
}