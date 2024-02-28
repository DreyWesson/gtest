#include <iostream>
#include <string>
#include <cassert>
#include "HttpRequestParser.hpp"

void testHttpPostTypes() {
    // Test JSON Body
    {
        std::string data = "POST /submit HTTP/1.1\r\n"
                           "Host: www.example.com\r\n"
                           "Content-Type: application/json\r\n"
                           "Content-Length: 26\r\n"
                           "\r\n"
                           "{\"key1\": \"value1\", \"key2\": \"value2\"}";

        HttpRequestParser parser;
        int status = parser.parse(data);

        assert(status == HttpRequestParser::COMPLETE);
        assert(parser.getMethod() == "POST");
        assert(parser.getTarget() == "/submit");
        assert(parser.getProtocol() == "HTTP/1.1");
        const std::map<std::string, std::string>& headers = parser.getHeaders();

        // Check Content-Type header
        assert(headers.count("Content-Type"));
        assert(headers.at("Content-Type") == "application/json");

        std::string body = parser.getRequestBody();
        assert(body == "{\"key1\": \"value1\", \"key2\": \"value2\"}");
    }

    // Test Form URL Encoded Body
    {
        std::string data = "POST /submit-form HTTP/1.1\r\n"
                           "Host: www.example.com\r\n"
                           "Content-Type: application/x-www-form-urlencoded\r\n"
                           "Content-Length: 25\r\n"
                           "\r\n"
                           "name=JohnDoe&age=30&city=NewYork";

        HttpRequestParser parser;
        int status = parser.parse(data);

        assert(status == HttpRequestParser::COMPLETE);
        assert(parser.getMethod() == "POST");
        assert(parser.getTarget() == "/submit-form");
        assert(parser.getProtocol() == "HTTP/1.1");
        const std::map<std::string, std::string>& headers = parser.getHeaders();

        // Check Content-Type header
        assert(headers.count("Content-Type"));
        assert(headers.at("Content-Type") == "application/x-www-form-urlencoded");

        std::string body = parser.getRequestBody();
        assert(body == "name=JohnDoe&age=30&city=NewYork");
    }

    // Test Multipart Form Data
    {
        std::string data = "POST /upload HTTP/1.1\r\n"
                           "Host: www.example.com\r\n"
                           "Content-Type: multipart/form-data; boundary=---------------------------\r\n"
                           "\r\n"
                           "-----------------------------\r\n"
                           "Content-Disposition: form-data; name=\"file\"; filename=\"example.txt\"\r\n"
                           "Content-Type: text/plain\r\n"
                           "\r\n"
                           "(contents of the file example.txt)\r\n"
                           "-----------------------------\r\n"
                           "Content-Disposition: form-data; name=\"name\"\r\n"
                           "\r\n"
                           "John Doe\r\n"
                           "-----------------------------\r\n"
                           "Content-Disposition: form-data; name=\"age\"\r\n"
                           "\r\n"
                           "30\r\n"
                           "-----------------------------\r\n"
                           "Content-Disposition: form-data; name=\"city\"\r\n"
                           "\r\n"
                           "New York\r\n"
                           "-----------------------------\r\n";

        HttpRequestParser parser;
        int status = parser.parse(data);

        assert(status == HttpRequestParser::COMPLETE);
        assert(parser.getMethod() == "POST");
        assert(parser.getTarget() == "/upload");
        assert(parser.getProtocol() == "HTTP/1.1");
        const std::map<std::string, std::string>& headers = parser.getHeaders();

        // Check Content-Type header
        assert(headers.count("Content-Type"));
        assert(headers.at("Content-Type") == "multipart/form-data; boundary=---------------------------");

        std::string body = parser.getRequestBody();
        assert(body == "-----------------------------\r\n"
                       "Content-Disposition: form-data; name=\"file\"; filename=\"example.txt\"\r\n"
                       "Content-Type: text/plain\r\n"
                       "\r\n"
                       "(contents of the file example.txt)\r\n"
                       "-----------------------------\r\n"
                       "Content-Disposition: form-data; name=\"name\"\r\n"
                       "\r\n"
                       "John Doe\r\n"
                       "-----------------------------\r\n"
                       "Content-Disposition: form-data; name=\"age\"\r\n"
                       "\r\n"
                       "30\r\n"
                       "-----------------------------\r\n"
                       "Content-Disposition: form-data; name=\"city\"\r\n"
                       "\r\n"
                       "New York\r\n"
                       "-----------------------------\r\n");
    }

    // Test Empty Body
    {
        std::string data = "POST /empty HTTP/1.1\r\n"
                           "Host: www.example.com\r\n"
                           "Content-Type: text/plain\r\n"
                           "Content-Length: 0\r\n"
                           "\r\n";

        HttpRequestParser parser;
        int status = parser.parse(data);

        assert(status == HttpRequestParser::COMPLETE);
        assert(parser.getMethod() == "POST");
        assert(parser.getTarget() == "/empty");
        assert(parser.getProtocol() == "HTTP/1.1");
        const std::map<std::string, std::string>& headers = parser.getHeaders();

        // Check Content-Type header
        assert(headers.count("Content-Type"));
        assert(headers.at("Content-Type") == "text/plain");

        std::string body = parser.getRequestBody();
        assert(body.empty());
    }
}


