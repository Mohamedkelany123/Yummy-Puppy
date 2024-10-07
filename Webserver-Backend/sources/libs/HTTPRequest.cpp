#include "HTTPRequest.h" 
static inline void ltrim(std::string &s) {
    // s.erase(s.begin(), std::find_if(s.begin(), s.end(),
    //         std::not1(std::ptr_fun<int, int>(std::isspace))));
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int c) {return !std::isspace(c);}));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    // s.erase(std::find_if(s.rbegin(), s.rend(),
    //         std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int c) {return !std::isspace(c);}).base(),s.end());

}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}
// Add header key/value fields to the header map
void HTTPRequest::addToHeaderMap(string header_item)
{
    // header_item is in the form of <key>: <value>\r\n
    stringstream iss(header_item); // stringstream for parsing a header item
    string key=""; // A string to store key value
    string value = ""; // A string to store value
    getline(iss,key,':'); // fetch key token before ':' character 
    getline(iss,value,'\r'); // fetch value token between ':' and '\r' characters 
    trim(value);
    header[key] = value;  // add to the map
}
// Constructor setting the target request TCPSocket
HTTPRequest::HTTPRequest(TCPSocket * p_tcpSocket)
{
    tcpSocket = p_tcpSocket; // Set tcpSocket data member
    binary_body = NULL;
    header_size = 0;
    binary_size = 0;
}

// Read the header from the socket and parse it. 
// Notice that a descendant class is instantiated based on the type of the method HTTPTransaction and hence some data were read from the socket and this is passed in initial_header
void HTTPRequest::readAndParse(string initial_header, long sz,char * binary_buffer)
{
    char buffer[1024];// A buffer to read data in
    memset (buffer,0,1024); // Initialize buffer
    string http_stream=initial_header; // copy initial header into HTTP stream
    binary_size=sz;

    if ( binary_body != NULL) 
    {
        free(binary_body);
        binary_body = NULL;
    }
    binary_body = (char *) calloc (binary_size+10,sizeof(char));
    memcpy(binary_body,binary_buffer,binary_size);
    for ( ;http_stream.find("\r\n\r\n") ==std::string::npos; )
    { // keep on reading as long as we cannot find the "\r\n\r\n" of the header
        int just_read = tcpSocket->readFromSocket(buffer,1023);
        http_stream +=buffer; // Append what you have got from the socket
        if ( binary_body == NULL)  binary_body = (char *) calloc (binary_size+10,sizeof(char));
        else binary_body = (char *) realloc (binary_body,(binary_size+just_read+10*(sizeof(char))));
        memcpy(binary_body+binary_size,buffer,just_read);
        binary_size += just_read;
        memset (buffer,0,1024); // Reinitialize the read buffer
    }
    char * header_ptr = strstr(binary_body,"\r\n\r\n");
    if ( header_ptr != NULL)
        header_size = header_ptr-binary_body;

    stringstream iss(http_stream); // stringstream for parsing the header
    // Get method, URI, and protocol from the first line
    getline(iss,method,' '); 
    getline(iss,resource,' ');
    getline(iss,protocol,'\n');
    
    protocol.pop_back(); // protocol now has '\r' in it so we want to remove it

    string line = " "; // set line to anything
    for (;!line.empty();) // keep on looping until you get an empty line indicating "\r\n\r\n" was found
    {
        getline(iss,line,'\n'); // get token until '\n'
        line.pop_back(); // eat up the '\r'
        //cout << line << endl;
        // now we have the header item in line
        // if line is not empty then we did not reach the end of the header, so we parse rge item
        if ( !line.empty()) addToHeaderMap(line); 
    }
    // extract any data, that have been read from the socket, after the header and store it in body    
    getline(iss,line,'\0'); 
    body = line;
}
// Selector returning the resource URI of the header
string HTTPRequest::getResource ()
{
    return resource;
}

string HTTPRequest::getHeaderValue(string header_item_name)
{
    if (header.find(header_item_name) != header.end())
        return header[header_item_name];
    else return "";
}

map<string, map<string, string>> *HTTPRequest::getContext()
{
    return &context;
}

void HTTPRequest::addContext(string _key, map<string, string> _values)
{
    context[_key] = _values;
}

// A selector returning a reference to the body of the request
string & HTTPRequest::getBody()
{
    return body;
}

char * HTTPRequest::getBinaryBody()
{
    return binary_body;
}
long HTTPRequest::getBinaryBodySize()
{
    return binary_size;
}

// Destructor
HTTPRequest::~HTTPRequest()
{
    if (binary_body != NULL ) free (binary_body);
}
