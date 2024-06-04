#ifndef TESTSERIALIZER_H
#define TESTSERIALIZER_H

#include <PSQLAbstractQueryIterator.h>
#include <fstream>



class TestSerializer {

    private:
        string store_location;
        PSQLJoinQueryIterator * psqlQueryJoin;
        bool write_to_stream(string orm_serialized);
        bool commit_to_stream ();
        bool generate_simple();
        bool generate_aggregate();
    public:
        TestSerializer(string _store_location,PSQLJoinQueryIterator * _psqlQueryJoin);
        bool generate(is_aggregate);
        ~TestSerializer();

};

#endif