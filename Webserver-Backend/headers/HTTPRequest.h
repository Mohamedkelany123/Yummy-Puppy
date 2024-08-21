#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H
#include "common_ws.h"
#include "TCPSocket.h"

// A class representing an HTTP Request Header
class HTTPRequest
{
protected:
    std::map<string, string, ignorecase> header; // A map that represent the HTTP Request Header types
    string method;                               // The HTTP Request Method
    string resource;                             // The HTTP Request Resource URI
    string protocol;                             // The HTTP protocol supported by the HTTP client
    TCPSocket *tcpSocket;                        // TCP Socket to communicate over with the HTTP client
    string body;                                 // A string representing the request body; for simplification we will consider only ASCII bodies.
    map<string, map<string, string>> context;
    void addToHeaderMap(string header_item);     // Parse and add a header item to the header map
public:
    HTTPRequest(TCPSocket *p_tcpSocket);              // Constructor
    virtual void readAndParse(string initial_header); // Read header from socket and parse it
    string getResource();                             // Selector: return the resource data member.
    string &getBody();                                // Selector: return the request body
    // A pure virtual method that should be implemented by all descendants to clone and create new object
    string getHeaderValue(string header_item_name);
    string getMethod() { return method; }
    map<string, map<string, string>> * getContext();
    void addContext(string _key, map<string, string> _values);
    virtual HTTPRequest *clone(TCPSocket *p_tcpSocket) = 0;
    virtual ~HTTPRequest(); // Destructor
};

#endif // HTTPREQUEST_H
