#include <iostream>
#include <string>
#include <cassert>
#include "HttpRequestParser.hpp"

// void testHttpGet() {
//     // Test HTTP GET request
//     std::string data = "GET /index.html HTTP/1.1\r\n"
//                        "Host: www.example.com\r\n"
//                        "\r\n";
//     HttpRequestParser parser;
//     int status = parser.parse(data);

//     std::cout << "Status after parsing: " << status << std::endl;
//     std::cout << "Method: " << parser.getMethod() << std::endl;
//     std::cout << "Target: " << parser.getTarget() << std::endl;
//     std::cout << "Protocol: " << parser.getProtocol() << std::endl;
//     const std::map<std::string, std::string>& headers = parser.getHeaders();
//     std::cout << "Received Headers:" << std::endl;
//     for (const auto& header : headers) {
//         std::cout << header.first << ": " << header.second << std::endl;
//     }

//     // Check status, method, target, protocol, and headers
//     assert(status == HttpRequestParser::COMPLETE);
//     assert(parser.getMethod() == "GET");
//     assert(parser.getTarget() == "/index.html");
//     assert(parser.getProtocol() == "HTTP/1.1");
//     assert(headers.size() == 1);
//     if (headers.count("Host")) {
//         assert(headers.at("Host") == "www.example.com");
//     } else {
//         std::cerr << "Host header not found" << std::endl;
//         assert(false); // Fail the assertion if Host header is not found
//     }

//     std::cout << "All assertions passed!" << std::endl;
// }






// void testHttpPost() {
//     // Test HTTP POST request
//     std::string data = "POST /submit-form HTTP/1.1\r\n"
//                        "Host: www.example.com\r\n"
//                        "Content-Type: application/json\r\n"
//                        "Content-Length: 27\r\n"
//                        "\r\n"
//                        "{\"username\": \"john_doe\"}";
//     HttpRequestParser parser;
//     int status = parser.parse(data);
//     assert(status == HttpRequestParser::COMPLETE);
//     assert(parser.getMethod() == "POST");
//     assert(parser.getTarget() == "/submit-form");
//     assert(parser.getProtocol() == "HTTP/1.1");
//     const std::map<std::string, std::string>& headers = parser.getHeaders();

//     std::cout << "Received Headers:" << std::endl;
//     for (const auto& header : headers) {
//         std::cout << header.first << ": " << header.second << std::endl;
//     }

//     assert(headers.size() == 3);

//     // Check Host header
//     if (headers.count("Host")) {
//         assert(headers.at("Host") == "www.example.com");
//     } else {
//         assert(false && "Host header not found");
//     }

//     // Check Content-Type header
//     if (headers.count("Content-Type")) {
//         std::cout << "Content-Type: " << headers.at("Content-Type") << std::endl;
//         assert(headers.at("Content-Type") == "application/json");
//     } else {
//         std::cerr << "Content-Type header not found" << std::endl;
//         assert(false && "Content-Type header not found");
//     }

//     // Check Content-Length header
//     if (headers.count("Content-Length")) {
//         assert(headers.at("Content-Length") == "27");
//     } else {
//         assert(false && "Content-Length header not found");
//     }
// }




// void testHttpPut() {
//     // Test HTTP PUT request
//     std::string data = "PUT /update HTTP/1.1\r\n"
//                        "Host: www.example.com\r\n"
//                        "Content-Type: text/plain\r\n"
//                        "Content-Length: 13\r\n"
//                        "\r\n"
//                        "Hello, World!";
//     HttpRequestParser parser;
//     int status = parser.parse(data);
//     assert(status == HttpRequestParser::COMPLETE);
//     assert(parser.getMethod() == "PUT");
//     assert(parser.getTarget() == "/update");
//     assert(parser.getProtocol() == "HTTP/1.1");
//     const std::map<std::string, std::string>& headers = parser.getHeaders();
//     assert(headers.size() == 3);
//     assert(headers.at("Host") == "www.example.com");
//     assert(headers.at("Content-Type") == "text/plain");
//     assert(headers.at("Content-Length") == "13");
// }

// void testHttpDelete() {
//     // Test HTTP DELETE request
//     std::string data = "DELETE /resource/123 HTTP/1.1\r\n"
//                        "Host: www.example.com\r\n"
//                        "\r\n";
//     HttpRequestParser parser;
//     int status = parser.parse(data);
//     assert(status == HttpRequestParser::COMPLETE);
//     assert(parser.getMethod() == "DELETE");
//     assert(parser.getTarget() == "/resource/123");
//     assert(parser.getProtocol() == "HTTP/1.1");
//     const std::map<std::string, std::string>& headers = parser.getHeaders();
//     assert(headers.size() == 1);
//     assert(headers.at("Host") == "www.example.com");
// }

// void testHttpOptions() {
//     // Test HTTP OPTIONS request
//     std::string data = "OPTIONS * HTTP/1.1\r\n"
//                        "Host: www.example.com\r\n"
//                        "\r\n";
//     HttpRequestParser parser;
//     int status = parser.parse(data);
//     assert(status == HttpRequestParser::COMPLETE);
//     assert(parser.getMethod() == "OPTIONS");
//     assert(parser.getTarget() == "*");
//     assert(parser.getProtocol() == "HTTP/1.1");
//     const std::map<std::string, std::string>& headers = parser.getHeaders();
//     assert(headers.size() == 1);
//     assert(headers.at("Host") == "www.example.com");
// }

// void testHttpHead() {
//     // Test HTTP HEAD request
//     std::string data = "HEAD /index.html HTTP/1.1\r\n"
//                        "Host: www.example.com\r\n"
//                        "\r\n";
//     HttpRequestParser parser;
//     int status = parser.parse(data);
//     assert(status == HttpRequestParser::COMPLETE);
//     assert(parser.getMethod() == "HEAD");
//     assert(parser.getTarget() == "/index.html");
//     assert(parser.getProtocol() == "HTTP/1.1");
//     const std::map<std::string, std::string>& headers = parser.getHeaders();
//     assert(headers.size() == 1);
//     assert(headers.at("Host") == "www.example.com");
// }

// void testHttpPatch() {
//     // Test HTTP PATCH request
//     std::string data = "PATCH /update HTTP/1.1\r\n"
//                        "Host: www.example.com\r\n"
//                        "Content-Type: application/json\r\n"
//                        "Content-Length: 20\r\n"
//                        "\r\n"
//                        "{\"status\": \"done\"}";
//     HttpRequestParser parser;
//     int status = parser.parse(data);
//     assert(status == HttpRequestParser::COMPLETE);
//     assert(parser.getMethod() == "PATCH");
//     assert(parser.getTarget() == "/update");
//     assert(parser.getProtocol() == "HTTP/1.1");
//     const std::map<std::string, std::string>& headers = parser.getHeaders();
//     assert(headers.size() == 3);
//     assert(headers.at("Host") == "www.example.com");
//     assert(headers.at("Content-Type") == "application/json");
//     assert(headers.at("Content-Length") == "20");
// }

// void testHttpTrace() {
//     // Test HTTP TRACE request
//     std::string data = "TRACE / HTTP/1.1\r\n"
//                        "Host: www.example.com\r\n"
//                        "\r\n";
//     HttpRequestParser parser;
//     int status = parser.parse(data);
//     assert(status == HttpRequestParser::COMPLETE);
//     assert(parser.getMethod() == "TRACE");
//     assert(parser.getTarget() == "/");
//     assert(parser.getProtocol() == "HTTP/1.1");
//     const std::map<std::string, std::string>& headers = parser.getHeaders();
//     assert(headers.size() == 1);
//     assert(headers.at("Host") == "www.example.com");
// }

// int main() {
//     testHttpGet();
//     // testHttpPost();
//     // testHttpPut();
//     // testHttpDelete();
//     // testHttpOptions();
//     // testHttpHead();
//     // testHttpPatch();
//     // testHttpTrace();

//     std::cout << "All HTTP method tests passed!" << std::endl;
//     return 0;
// }
