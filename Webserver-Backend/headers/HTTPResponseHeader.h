#ifndef HTTPRESPONSEHEADER_H
#define HTTPRESPONSEHEADER_H
#include "common_ws.h"
#include "TCPSocket.h"


class HTTPResponseHeader // A class represening an HTTP response header
{
    private:
        std::map <string,string,ignorecase> header; //A map to store header tuples 
        string status; // Respose header status string
        int status_code; // Response header status code
        string protocol; // HTTP protocol user
        TCPSocket * tcpSocket; // A pointer to the TCPsocket object of the current connections
    public:
        // Constructor seeting up the differen data members 
        HTTPResponseHeader(TCPSocket * p_tcpSocket,string p_status,int p_status_code,string p_protocol);
        void setHeader(string p_key,string p_value); // Setting a header tuple
        void respond (); // responding to the client with the header over the tcpSocket
        ~HTTPResponseHeader(); // Destructor
};

#endif // HTTPRESPONSEHEADER_H
