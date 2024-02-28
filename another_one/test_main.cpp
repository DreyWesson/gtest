#include <iostream>
#include <string>
#include <cassert>
#include "HttpRequestParser.hpp"



std::string loadGet(int num) {
    return (num == 1)
        ? "GET /index.html HTTP/1.1\r\n"
            "Host: www.example.com\r\n"
            "\r\n"
        : "GET /index.html HTTP/1.1\r\n"
            "Host: www.example.com\r\n"
            "User-Agent: Mozilla/5.0\r\n"
            "Accept-Language: en-US,en;q=0.5\r\n"
            "Accept-Encoding: gzip, deflate\r\n"
            "\r\n";
};

void testHttpGet() {
    HttpRequestParser parser;
    size_t headerNum = 4;
    std::string data = loadGet(headerNum);
    int status = parser.parse(data);
    const std::map<std::string, std::string>& headers = parser.getHeaders();
    
    assert(status == HttpRequestParser::COMPLETE);
    assert(parser.getMethod() == "GET");
    assert(parser.getTarget() == "/index.html");
    assert(parser.getProtocol() == "HTTP/1.1");
    assert(headers.size() == headerNum);

    std::cout << "Method: " << parser.getMethod() << std::endl;
    std::cout << "Target: " << parser.getTarget() << std::endl;
    std::cout << "Protocol: " << parser.getProtocol() << std::endl;
    std::cout << "Number of headers: " << headers.size() << std::endl;

    std::cout << "Headers:" << std::endl;
    for (const auto& header : headers) {
        std::cout << "\t" << header.first << ": " << header.second << std::endl;
    }

    if (headers.count("Host")) {
        assert(headers.at("Host") == "www.example.com");
    } else {
        assert(false && "Host header not found");
    }
}




void testHttpPost() {
    // Test HTTP POST request
    std::string data = "POST /submit-form HTTP/1.1\r\n"
        "Host: www.example.com\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: 27\r\n"
        "\r\n"
        "{\"username\": \"john_doe\"}\r\n";

    HttpRequestParser parser;
    int status = parser.parse(data);
    assert(status == HttpRequestParser::COMPLETE);
    assert(parser.getMethod() == "POST");
    assert(parser.getTarget() == "/submit-form");
    assert(parser.getProtocol() == "HTTP/1.1");
    const std::map<std::string, std::string>& headers = parser.getHeaders();
    std::cout << "DEBUG\n";

    // Print headers
    std::cout << "Method: " << parser.getMethod() << std::endl;
    std::cout << "Target: " << parser.getTarget() << std::endl;
    std::cout << "Protocol: " << parser.getProtocol() << std::endl;
    std::cout << "Number of headers: " << headers.size() << std::endl;
    for (const auto& header : headers) {
        std::cout << "\t" << header.first << ": " << header.second << std::endl;
    }

    // Check Host header
    if (headers.count("Host")) {
        assert(headers.at("Host") == "www.example.com");
    } else {
        assert(false && "Host header not found");
    }

    // Check Content-Type header (case-insensitive)
    bool contentTypeFound = false;
    for (const auto& header : headers) {
        std::string headerName = header.first;
        std::transform(headerName.begin(), headerName.end(), headerName.begin(), ::tolower);
        if (headerName == "content-type") {
            contentTypeFound = true;
            assert(header.second == "application/json");
            break;
        }
    }
    if (!contentTypeFound) {
        assert(false && "Content-Type header not found");
    }
    // Check Content-Length header
    if (headers.count("Content-Length")) {
        assert(headers.at("Content-Length") == "27");
    } else {
        assert(false && "Content-Length header not found");
    }

    // Check request body
    // std::string requestBody = removeWhitespace(parser.getRequestBody());
        std::string requestBody = parser.getRequestBody();
    std::cout << "Request Body: [" << requestBody << "]" << std::endl; // Print with brackets to visualize any leading/trailing whitespace
    assert(requestBody == "{\"username\":\"john_doe\"}");
}


void testHttpPut() {
    // Test HTTP PUT request
    std::string data = "PUT /update HTTP/1.1\r\n"
                       "Host: www.example.com\r\n"
                       "Content-Type: text/plain\r\n"
                       "Content-Length: 13\r\n"
                       "\r\n"
                       "Hello, World!";
//    std::string data = "PUT /update_user/123 HTTP/1.1"
//         "Host: api.example.com"
//         "Content-Type: application/json"
//         "Authorization: Bearer your_access_token"

//     "{\"name\": \"John Doe\",\"email\": \"johndoe@example.com\",\"age\": 30,\"active\": true}";

    HttpRequestParser parser;
    int status = parser.parse(data);
    assert(status == HttpRequestParser::COMPLETE);
    assert(parser.getMethod() == "PUT");
    assert(parser.getTarget() == "/update");
    assert(parser.getProtocol() == "HTTP/1.1");
    const std::map<std::string, std::string>& headers = parser.getHeaders();
    assert(headers.size() == 3);
    assert(headers.at("Host") == "www.example.com");
    assert(headers.at("Content-Type") == "text/plain");
    assert(headers.at("Content-Length") == "13");
}



int main() {
    testHttpGet();
    std::cout << "\n\n";
    testHttpPost();
    // std::cout << "\n\n";
    // testHttpPut();

    std::cout << "All HTTP method tests passed!" << std::endl;
    return 0;
}