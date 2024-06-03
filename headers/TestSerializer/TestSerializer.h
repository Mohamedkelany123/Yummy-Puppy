#ifndef TESTSERIALIZER_H
#define TESTSERIALIZER_H

#include <PSQLAbstractQueryIterator.h>



class TestSerializer {

    private:
        string store
        PSQLJoinQueryIterator * psqlQueryJoin;
        bool write_to_stream(string orm_serialized);
        bool commit_to_stream ();
        bool generate_simple();
        bool generate_aggregate();
    public:
        TestSerializer(string store_location,PSQLJoinQueryIterator * _psqlQueryJoin);
        bool generate();
        ~TestSerializer();

};

#endif