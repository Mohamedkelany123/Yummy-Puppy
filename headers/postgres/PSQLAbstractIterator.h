#ifndef PSQLABSTRACTITERATOR_H
#define PSQLABSTRACTITERATOR_H

#include <PSQLAbstractORM.h>

class PSQLAbstractIterator {

    protected:
        PSQLConnection * psqlConnection;
        AbstractDBQuery * psqlQuery;

    public:
        PSQLAbstractIterator();
        ~PSQLAbstractIterator();
};

#endif