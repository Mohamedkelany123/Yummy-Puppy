#include "TCPServerSocket.h"
#include "HTTPTransaction.h"
#include "GarbageCollector.h"
#include "HTTPRequestManager.h"
#include "ConfigFile.h"
#include "Logger.h"
#include "MiddlewareManager.h"
#include "TeamThread.h"

using namespace std;
int main(int argc, char **argv)
{
    ConfigFile *conf = new ConfigFile(argc, argv);
    conf->setConfInstance(conf);
    Logger::getInstanceInitial(conf);
    if (conf->daemon == true)
    {
        int nochdir = 0;
        int noclose = 0;
        if (daemon(nochdir, noclose)) // running the process in the background as a daemon
            perror("daemon");
    }
    LOG_INFO("Initialize Logger Class Object, Log level: LOG_INFO");
    // Instantiate a garbage collector object
    GarbageCollector *garbageCollector = new GarbageCollector();
    // Instantiate an HTTPServiceManager Factory
    MiddlewareManager *middlewareManager = new MiddlewareManager(conf, Logger::getInstance());

    HTTPServiceManager *httpServiceManager = new HTTPServiceManager(conf, Logger::getInstance(), middlewareManager);

    // Instantiate an HTTPRequestManager Factory
    HTTPRequestManager *httpRequestManager = new HTTPRequestManager();
    // Create a TCP socket that listens on the localhost loopback address and port 9999
    cout << conf->listen_address << " " << conf->port_number << " " << conf->backlog_queue << endl;
    TCPServerSocket *tcpServerSocket = new TCPServerSocket(conf->listen_address.c_str(), conf->port_number, conf->backlog_queue);

    tcpServerSocket->initializeSocket(); // Initialize the server socket
    while (true)                         // loop until socket errors
    {
        // Wait for connection indefinitely
        TCPSocket *tcpSocket = tcpServerSocket->getConnection(-1, -1, -1, -1);
        if (tcpSocket == NULL)
            break;                   // if returns a NULL break the loop and exist
        garbageCollector->cleanup(); // Apply garbage collection cleanup
        // Instantiate a new HTTPTransaction Object and pass the TCPSocket pointer and different factories to it
        HTTPTransaction *httpTransaction = new HTTPTransaction(tcpSocket, httpServiceManager, httpRequestManager, middlewareManager);
        // Add the HTTPTransaction just created to the garbage collector for clean up
        TeamThread *t = createTeamLeadIfNot(HTTPTransaction::startHTTPTransaction, httpTransaction); // Start the connection thread to communicate with the client
        httpTransaction->setThread(t);
        garbageCollector->addHTTPTransaction(httpTransaction);
        // Start the HTTPTransaction thread
    }
    delete (tcpServerSocket);    // Delete the TCPServerSocket
    delete (httpServiceManager); // Delete the HTTPServiceManager
    delete (httpRequestManager); // Delete the HTTPRequestManager
    delete (garbageCollector);   // Delete the Garbage Collector
    delete (middlewareManager);
    return 0;
}