#ifndef HTTP_REQUEST_PARSER_HPP
#define HTTP_REQUEST_PARSER_HPP

#include <string>
#include <map>
#include <sys/time.h>

class HttpRequestParser {
public:
    enum ParseStatus {
        START_LINE,
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
    const std::string& getQueryString() const;
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
    std::string removeWhitespace(const std::string& str);

    int parseFirstLine();
    int parseHeaders();
    int parsePreBody();
    int parseBody();
    int parseChunk();


    struct IsValidChar {
        const std::string& validSet;
        IsValidChar(const std::string& set) : validSet(set) {}
        bool operator()(char c) const {
            return validSet.find(c) != std::string::npos;
        }
    };
};

#endif

