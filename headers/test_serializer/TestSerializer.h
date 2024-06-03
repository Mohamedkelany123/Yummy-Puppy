#ifndef TESTSERIALIZER_H
#define TESTSERIALIZER_H

#include <PSQLAbstractQueryIterator.h>



class TestSerializer {

    private:
        
        bool generate_simple();
        bool generate_aggregate();
    public:
        TestSerializer(string store_location,PSQLJoinQueryIterator * psqlQueryJoin);
        bool generate();
        ~TestSerializer();

};

#endif