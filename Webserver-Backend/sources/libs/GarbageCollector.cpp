#include "GarbageCollector.h"
// Default Constructor 
GarbageCollector::GarbageCollector(){ }

// Add a new HTTPTransaction object pointers
void GarbageCollector::addHTTPTransaction(HTTPTransaction * p_httpTransaction) {
    http_transaction_store.push_back(p_httpTransaction); // push back to the vector
}
// Clean up all finished threads
void GarbageCollector::cleanup() {
    // Loop to clean up all expired HTTP Transactions
    for (int i = 0 ; i < http_transaction_store.size(); )
    {
        if ( !http_transaction_store[i]->isRunning()) 
        { // If the HTTP Transaction is not runnings
            http_transaction_store[i]->waitForRunToFinish(); // Join: should return immediatly
            HTTPTransaction * httpTransaction = http_transaction_store[i]; // fecth the Object pointer at location
            delete (httpTransaction); // Delete object
            http_transaction_store.erase(http_transaction_store.begin()+i); // erase the entry from the vector
        }
        else i++; // only increment if the HTTP Transaction is still running
    }
}
// Invoked upon program termination to Gracefully wait for all running threads to finish
void GarbageCollector::terminate() {
    // Loop to clean up all HTTP Transactions, and pause/wait for running ones until they finish
    for ( ; http_transaction_store.size() > 0; )
    {
            http_transaction_store[0]->waitForRunToFinish(); //Block until thread is done
            HTTPTransaction * httpTransaction = http_transaction_store[0]; // Fetch a pointer to the object at 0
            delete (httpTransaction); // Delete object
            http_transaction_store.erase(http_transaction_store.begin()); // erase the entry from the vector
    }
}
GarbageCollector::~GarbageCollector()
{
    terminate(); // Invoke terminate to dispose all connections
}

