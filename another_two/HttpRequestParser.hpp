#ifndef HTTP_REQUEST_PARSER_HPP
#define HTTP_REQUEST_PARSER_HPP

#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <cctype>
#include <chrono>
#include <iostream>
#include <sstream>
#include <iterator>
#include <ctime>

class HttpRequestParser {
public:
    enum ParseStatus {
        FIRST_LINE,
        HEADERS,
        PREBODY,
        BODY,
        CHUNK,
        COMPLETE,
        ERROR
    };

    HttpRequestParser();
    ~HttpRequestParser();

    int parse(std::string &buffer);
    bool isValidURI(const std::string &uri);

    const std::string& getMethod() const;
    const std::string& getTarget() const;
    const std::string& getProtocol() const;
    const std::map<std::string, std::string>& getHeaders() const;
    std::string getRequestBody() const;
    void reset();

    ParseStatus getStatus() const; // Add this line

private:
    std::string receivedData;
    std::string method;
    std::string target;
    std::string queryString;
    std::string protocol;
    std::map<std::string, std::string> headers;
    std::string body;
    unsigned long contentLength;
    unsigned long bodyOffset;
    unsigned long chunkSize;
    struct timeval startTimer;
    ParseStatus status;

    int parseFirstLine();
    int parseHeaders();
    int parsePreBody();
    int parseBody();
    int parseChunk();

    // Helper methods
    void parseHeaderLine(const std::string& line);
    std::vector<std::string> split(const std::string& str, char delimiter);
    std::string trim(const std::string& str);
};

#endif // HTTP_REQUEST_PARSER_HPP

