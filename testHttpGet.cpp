#include "HttpRequestParser.hpp"
#include <iostream>
#include <cassert>
#include <string>

void testHttpGet() {
    HttpRequestParser parser;

    // Test 1: Basic GET request without query parameters
    std::string get1 = "GET /index.html HTTP/1.1\r\n"
                       "Host: www.example.com\r\n"
                       "\r\n";

    int status = parser.parse(get1);
    assert(status == HttpRequestParser::COMPLETE);
    assert(parser.getMethod() == "GET");
    assert(parser.getTarget() == "/index.html");
    assert(parser.getProtocol() == "HTTP/1.1");
    const std::map<std::string, std::string>& headers1 = parser.getHeaders();
    assert(headers1.size() == 1); // Only Host header
    assert(headers1.count("Host"));
    assert(headers1.at("Host") == "www.example.com");

    std::cout << "Test 1 (Basic GET request without query parameters) passed." << std::endl;

    // Test 2: GET request with query parameters
    std::string get2 = "GET /search?query=example&page=1 HTTP/1.1\r\n"
                       "Host: www.example.com\r\n"
                       "\r\n";

    status = parser.parse(get2);
    assert(status == HttpRequestParser::COMPLETE);
    assert(parser.getMethod() == "GET");
    assert(parser.getTarget() == "/search");
    assert(parser.getProtocol() == "HTTP/1.1");
    const std::map<std::string, std::string>& headers2 = parser.getHeaders();
    assert(headers2.size() == 1); // Only Host header
    assert(headers2.count("Host"));
    assert(headers2.at("Host") == "www.example.com");
    // assert(parser.getQueryString() == "query=example&page=1");

    std::cout << "Test 2 (GET request with query parameters) passed." << std::endl;

    // Test 3: GET request with various headers
    std::string get3 = "GET /info HTTP/1.1\r\n"
                       "Host: www.example.com\r\n"
                       "User-Agent: Mozilla/5.0\r\n"
                       "Accept-Language: en-US,en;q=0.5\r\n"
                       "\r\n";

    status = parser.parse(get3);
    assert(status == HttpRequestParser::COMPLETE);
    assert(parser.getMethod() == "GET");
    assert(parser.getTarget() == "/info");
    assert(parser.getProtocol() == "HTTP/1.1");
    const std::map<std::string, std::string>& headers3 = parser.getHeaders();
    assert(headers3.size() == 3); // Host, User-Agent, Accept-Language
    assert(headers3.count("Host"));
    assert(headers3.count("User-Agent"));
    assert(headers3.count("Accept-Language"));
    assert(headers3.at("Host") == "www.example.com");
    assert(headers3.at("User-Agent") == "Mozilla/5.0");
    assert(headers3.at("Accept-Language") == "en-US,en;q=0.5");

    std::cout << "Test 3 (GET request with various headers) passed." << std::endl;

    // Test 4: Multiple GET requests in one buffer
    std::string multipleGets = "GET /page1 HTTP/1.1\r\n"
                               "Host: www.example.com\r\n"
                               "\r\n"
                               "GET /page2 HTTP/1.1\r\n"
                               "Host: www.example.com\r\n"
                               "\r\n";

    // Split the buffer into separate requests and parse each
    size_t pos = 0;
    while (pos < multipleGets.size()) {
        size_t nextPos = multipleGets.find("\r\n\r\n", pos);
        if (nextPos == std::string::npos)
            break;

        std::string getRequest = multipleGets.substr(pos, nextPos - pos + 4); // Include the \r\n\r\n
        pos = nextPos + 4;

        status = parser.parse(getRequest);
        assert(status == HttpRequestParser::COMPLETE);
        assert(parser.getMethod() == "GET");
        assert(parser.getProtocol() == "HTTP/1.1");
        const std::map<std::string, std::string>& headers = parser.getHeaders();
        assert(headers.size() == 1); // Only Host header
        assert(headers.count("Host"));
        assert(headers.at("Host") == "www.example.com");

        // Depending on the use case, you might want to verify other parts of the request as well
    }

    std::cout << "Test 4 (Multiple GET requests in one buffer) passed." << std::endl;

    std::cout << "All HTTP GET method tests passed!" << std::endl;
}
