#ifndef GARBAGECOLLECTOR_H
#define GARBAGECOLLECTOR_H

#include "HTTPTransaction.h"

// A class to manage expired HTTPTransaction Objects and dispose them one by one
class GarbageCollector
{ // It manages vector of pointers to HTTP Transactions 
    private:
        // A vector for storing pointers to HTTP Transactions
        vector <HTTPTransaction *> http_transaction_store;
    public:
        GarbageCollector();// Constructor
        void addHTTPTransaction(HTTPTransaction * p_http_transaction); // Add a connection
        void cleanup();	// Perform a clean up by disposing expired connections.
	    void terminate (); // Wait for all thread connections to terminate and cleanup all
        ~GarbageCollector(); // Destructor
};
#endif // GARBAGECOLLECTOR_H
