#include <iostream>
#include <string>
#include <cassert>
#include "HttpRequestParser.hpp"

void testHttpRequestParser() {
    // Test case 1: Valid HTTP GET request
    std::string data1 = "GET /index.html HTTP/1.1\r\n"
                        "Host: www.example.com\r\n"
                        "User-Agent: Mozilla/5.0\r\n"
                        "Accept-Language: en-US,en;q=0.5\r\n"
                        "Accept-Encoding: gzip, deflate\r\n"
                        "\r\n";
    HttpRequestParser parser1;
    int status1 = parser1.parse(data1);
    assert(status1 == HttpRequestParser::COMPLETE);
    assert(parser1.getMethod() == "GET");
    assert(parser1.getTarget() == "/index.html");
    assert(parser1.getProtocol() == "HTTP/1.1");
    const std::map<std::string, std::string>& headers1 = parser1.getHeaders();
    assert(headers1.size() == 4);
    assert(headers1.at("Host") == "www.example.com");
    assert(headers1.at("User-Agent") == "Mozilla/5.0");
    assert(headers1.at("Accept-Language") == "en-US,en;q=0.5");
    assert(headers1.at("Accept-Encoding") == "gzip, deflate");

    // Test case 2: POST request with empty body
    std::string data2 = "POST /submit-form HTTP/1.1\r\n"
                        "Host: www.example.com\r\n"
                        "Content-Length: 0\r\n"
                        "\r\n";
    HttpRequestParser parser2;
    int status2 = parser2.parse(data2);
    assert(status2 == HttpRequestParser::COMPLETE);
    assert(parser2.getMethod() == "POST");
    assert(parser2.getTarget() == "/submit-form");
    assert(parser2.getProtocol() == "HTTP/1.1");
    const std::map<std::string, std::string>& headers2 = parser2.getHeaders();
    assert(headers2.size() == 2);
    assert(headers2.at("Host") == "www.example.com");
    assert(headers2.at("Content-Length") == "0");

    // Test case 3: Invalid request (missing protocol)
    std::string data3 = "PUT /update HTTP/1.0\r\n"
                        "Host: www.example.com\r\n"
                        "\r\n";
    HttpRequestParser parser3;
    int status3 = parser3.parse(data3);
    assert(status3 == HttpRequestParser::ERROR);

    // Test case 4: Edge case - Long request
    std::string longData(100000, 'A'); // Create a long request
    longData += "\r\n";
    HttpRequestParser parser4;
    int status4 = parser4.parse(longData);
    assert(status4 == HttpRequestParser::ERROR);

    std::cout << "All tests passed!" << std::endl;
}

int main() {
    testHttpRequestParser();
    return 0;
}
