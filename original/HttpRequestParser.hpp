// #ifndef HTTP_REQUEST_PARSER_HPP
// #define HTTP_REQUEST_PARSER_HPP

// #include <iostream>
// #include <sstream>
// #include <string>
// #include <map>
// #include <vector>
// #include <algorithm>
// #include <set> // Added for std::set
// #include <sys/time.h>

// class HttpRequestParser {
// public:
//     HttpRequestParser();
//     ~HttpRequestParser();

//     int parse(std::string &buffer);
//     int parse(char buffer[], int nbytes);
//     int parseFirstLine();
//     int parseHeaders();
//     int parsePreBody();
//     int parseBody();
//     int parseChunk();
//     int parseChunkTrailer();

//     bool isTimeout();
//     time_t getStartTimeInSeconds();
//     time_t getLastTimeInSeconds();

//     int getStatus();
//     std::string getMethod() const;
//     std::string getTarget() const;
//     std::string getProtocol() const;
//     const std::map<std::string, std::string>& getHeaders() const;
//     std::string getRequestBody() const;

//     enum Status {
//         FIRST_LINE,
//         HEADERS,
//         PREBODY,
//         BODY,
//         CHUNK,
//         COMPLETE,
//         ERROR
//     };

//     enum ChunkStatus {
//         CHUNK_BODY,
//         CHUNK_SIZE,
//     };

//     struct MultipartData {
//         std::string name;
//         std::string filename;
//         std::string contentType;
//         std::string data;
//     };

// private:
//     std::string buffer;
//     std::string receivedData;  // Added receivedData

//     std::string method;  // Changed from 'method' to 'method_' for consistency
//     std::string target;
//     std::string queryString;
//     std::string protocol;
//     std::string requestBody;
//     std::map<std::string, std::string> headers;

//     std::map<std::string, std::string> queryParameters; // Added for query parameters
//     std::map<std::string, std::string> cookies; // Added for cookies
//     std::vector<std::string> contentEncodings; // Added for content encodings
//     std::vector<std::string> transferEncodings; // Added for transfer encodings



//     int bodyOffset;
//     int chunkSize;
//     struct timeval startTimer;
//     struct timeval lastTimer;
//     size_t length;

//     Status status;
//     ChunkStatus chunkStatus;

//     bool isValidURI(const std::string &uri);
//     bool isValidMethod(const std::string &method);
//     int toHex(const std::string &hexStr);
//     std::string trim(const std::string &s);
//     void parseQueryString(const std::string &queryString);
//     void parseCookies(const std::string &cookieHeader);
//     void parseContentEncoding(const std::string &encodingHeader);
//     void parseTransferEncoding(const std::string &encodingHeader);
// };

// #endif

#ifndef HTTP_REQUEST_PARSER_HPP
#define HTTP_REQUEST_PARSER_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <set> // Added for std::set
#include <sys/time.h>

class HttpRequestParser {
public:
    HttpRequestParser();
    ~HttpRequestParser();

    int parse(std::string &buffer);
    int parse(char buffer[], int nbytes);
    int parseFirstLine();
    int parseHeaders();
    int parsePreBody();
    int parseBody();
    int parseChunk();
    int parseChunkTrailer();

    bool isTimeout();
    time_t getStartTimeInSeconds();
    time_t getLastTimeInSeconds();

    int getStatus();
    std::string getMethod() const;
    std::string getTarget() const;
    std::string getProtocol() const;
    const std::map<std::string, std::string>& getHeaders() const;
    std::string getRequestBody() const;

    enum Status {
        FIRST_LINE,
        HEADERS,
        PREBODY,
        BODY,
        CHUNK,
        COMPLETE,
        ERROR
    };

    enum ChunkStatus {
        CHUNK_BODY,
        CHUNK_SIZE,
    };

    struct MultipartData {
        std::string name;
        std::string filename;
        std::string contentType;
        std::string data;
    };

private:
    std::string buffer;
    std::string receivedData;  // Added receivedData

    std::string method;  // Changed from 'method' to 'method_' for consistency
    std::string target;
    std::string queryString;
    std::string protocol;
    std::string requestBody;
    std::map<std::string, std::string> headers;

    std::map<std::string, std::string> queryParameters; // Added for query parameters
    std::map<std::string, std::string> cookies; // Added for cookies
    std::vector<std::string> contentEncodings; // Added for content encodings
    std::vector<std::string> transferEncodings; // Added for transfer encodings

    int bodyOffset;
    int chunkSize;
    struct timeval startTimer;
    struct timeval lastTimer;
    size_t length;

    Status status;
    ChunkStatus chunkStatus;

    bool isValidURI(const std::string &uri);
    bool isValidMethod(const std::string &method);
    int toHex(const std::string &hexStr);
    std::string trim(const std::string &s);
    void parseQueryString(const std::string &queryString);
    void parseCookies(const std::string &cookieHeader);
    void parseContentEncoding(const std::string &encodingHeader);
    void parseTransferEncoding(const std::string &encodingHeader);
};

#endif
