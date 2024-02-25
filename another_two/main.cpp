#include <iostream>
#include <string>
#include "HttpRequestParser.hpp"

int main() {
    std::string data = "GET /index.html HTTP/1.1\r\n"
                       "Host: www.example.com\r\n"
                       "User-Agent: Mozilla/5.0\r\n"
                       "Accept-Language: en-US,en;q=0.5\r\n"
                       "Accept-Encoding: gzip, deflate\r\n"
                       "\r\n";

    HttpRequestParser parser;
    int status = parser.parse(data);

    if (status == HttpRequestParser::COMPLETE) {
        std::cout << "Method: " << parser.getMethod() << std::endl;
        std::cout << "Target: " << parser.getTarget() << std::endl;
        std::cout << "Protocol: " << parser.getProtocol() << std::endl;

        const std::map<std::string, std::string>& headers = parser.getHeaders();
        std::cout << "Headers:" << std::endl;
        for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
            std::cout << it->first << ": " << it->second << std::endl;
        }
    } else if (status == HttpRequestParser::ERROR) {
        std::cerr << "Error parsing HTTP request." << std::endl;
    }

    return 0;
}
