
#include "HTTPResponseHeader.h"
// Constructor that sets data members using initialization list. status, status_code, and protocol are used to set up the first line of the header
HTTPResponseHeader::HTTPResponseHeader(string p_status, int p_status_code, string p_protocol) : status(p_status), status_code(p_status_code), protocol(p_protocol)
{
}
// Add a new response header "key: value"
void HTTPResponseHeader::setHeader(string p_key, string p_value)
{
    header[p_key] = p_value; // add to the header map
}

// Build up the HTTP header from the header map and write it on the socket back to the client
/**
 * @brief This function builds up the HTTP header from the header map and writes it on the socket back to the client.
 *
 * @param tcpSocket A pointer to the TCPSocket object representing the client's connection.
 *
 * The function first composes the first line of the header using the protocol, status code, and status.
 * Then, it iterates through the header map using a for_each loop with a lambda function to build up the rest of the header.
 * Each key-value pair in the header map is added to the header string in the format "key: value".
 * After building up the entire header, an empty line is added as the end of header indicator.
 * Finally, the header string is written to the client over the tcpSocket using the writeToSocket method.
 * @author kmsobh
 * @date 10-Jun-2024
 */
void HTTPResponseHeader::respond(TCPSocket *tcpSocket)
{
    // Compose the first line in the header
    string header_string = protocol;
    header_string += " ";
    header_string += to_string(status_code);
    header_string += " ";
    header_string += status;
    header_string += "\r\n"; // first line delimiter
    // A for_each iterator base loop with lambda function for building up the rest of the header
    for_each(header.begin(), header.end(), [&header_string](const std::pair<string, string> &it) -> bool
             {
            header_string +=  std::get<0>(it); 
            header_string += ": " ;
            header_string += std::get<1>(it);
            header_string += "\r\n";
            return true; });
    header_string += "\r\n"; // Empty line as end of header indicator
    // Write header to client over tcpSocket
    tcpSocket->writeToSocket(header_string.c_str(), header_string.length());
}
void HTTPResponseHeader::setStatus(string p_status)
{
    status = p_status;
}
void HTTPResponseHeader::setStatusCode(int p_status_code)
{
    status_code = p_status_code;
}
void HTTPResponseHeader::setProtocol(string p_protocol)
{
    protocol = p_protocol;
}
// Destructor
HTTPResponseHeader::~HTTPResponseHeader() {}
