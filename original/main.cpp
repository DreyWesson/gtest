

#include <iostream>
#include <sstream>
#include "HttpRequestParser.hpp"

// int main() {
//     // Create an instance of HttpRequestParser
//     HttpRequestParser parser;

//     // Simulate receiving HTTP request data in chunks
//     std::string requestData =
//         "POST /api/data HTTP/1.1\r\n"
//         "Host: localhost\r\n"
//         "Content-Type: application/json\r\n"
//         "Content-Length: 39\r\n"
//         "\r\n"
//         "{\"key1\": \"value1\", \"key2\": \"value2\"}\r\n";

//     std::istringstream dataStream(requestData);
//     std::string chunk;
//     while (std::getline(dataStream, chunk, '\n')) {
//         // Ensure each chunk ends with '\r\n' as per HTTP specification
//         chunk += "\r\n";
//         int parseStatus = parser.parse(chunk);
//         std::cout << "Parsing status for chunk: " << parseStatus << std::endl;
//         if (parseStatus != 0) {
//             std::cout << "Parsing error: " << parseStatus << std::endl;
//             break;
//         }
//     }

//     // Check the status of the request parsing
//     int status = parser.getStatus();
//     if (status == HttpRequestParser::COMPLETE) {
//         std::cout << "Request parsing successful!" << std::endl;
//         std::cout << "Method: " << parser.getMethod() << std::endl;
//         std::cout << "Target: " << parser.getTarget() << std::endl;
//         std::cout << "Protocol: " << parser.getProtocol() << std::endl;
//         std::cout << "Headers:" << std::endl;
//         const std::map<std::string, std::string>& headers = parser.getHeaders();
//         std::map<std::string, std::string>::const_iterator it;
//         for (it = headers.begin(); it != headers.end(); ++it) {
//             std::cout << it->first << ": " << it->second << std::endl;
//         }
//         std::cout << "Request Body: " << parser.getRequestBody() << std::endl;
//     } else if (status == HttpRequestParser::ERROR) {
//         std::cout << "Error occurred during request parsing." << std::endl;
//     } else {
//         std::cout << "Incomplete request, more data needed." << std::endl;
//     }

//     return 0;
// }

#include <iostream>
#include <sstream>
#include "HttpRequestParser.hpp"

void simulateHttpRequest(HttpRequestParser& parser, const std::string& requestData) {
    // Split the request into headers and body
    size_t bodyStart = requestData.find("\r\n\r\n");
    std::string headers = requestData.substr(0, bodyStart + 4); // Include the blank line
    std::string body = requestData.substr(bodyStart + 4); // Exclude the blank line

    // Send headers
    std::istringstream headerStream(headers);
    std::string headerChunk;
    while (std::getline(headerStream, headerChunk, '\n')) {
        headerChunk += "\r\n";
        int parseStatus = parser.parse(headerChunk);
        std::cout << "Parsing status for chunk: " << parseStatus << std::endl;
        if (parseStatus != 0) {
            std::cout << "Parsing error: " << parseStatus << std::endl;
            return;
        }
    }

    // Send body
    std::istringstream bodyStream(body);
    std::string bodyChunk;
    while (std::getline(bodyStream, bodyChunk, '\n')) {
        bodyChunk += "\r\n";
        int parseStatus = parser.parse(bodyChunk);
        std::cout << "Parsing status for chunk: " << parseStatus << std::endl;
        if (parseStatus != 0) {
            std::cout << "Parsing error: " << parseStatus << std::endl;
            return;
        }
    }
}

int main() {
    // Create an instance of HttpRequestParser
    HttpRequestParser parser;

    // Simulate receiving HTTP request data in chunks
    std::string requestData =
        "POST /api/data HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: 39\r\n"
        "\r\n"
        "{\"key1\": \"value1\", \"key2\": \"value2\"}\r\n";

    // Simulate the complete request
    simulateHttpRequest(parser, requestData);

    // Check the status of the request parsing
    int status = parser.getStatus();
    if (status == HttpRequestParser::COMPLETE) {
        std::cout << "Request parsing successful!" << std::endl;
        std::cout << "Method: " << parser.getMethod() << std::endl;
        std::cout << "Target: " << parser.getTarget() << std::endl;
        std::cout << "Protocol: " << parser.getProtocol() << std::endl;
        std::cout << "Headers:" << std::endl;
        const std::map<std::string, std::string>& headers = parser.getHeaders();
        std::map<std::string, std::string>::const_iterator it;
        for (it = headers.begin(); it != headers.end(); ++it) {
            std::cout << it->first << ": " << it->second << std::endl;
        }
        std::cout << "Request Body: " << parser.getRequestBody() << std::endl;
    } else if (status == HttpRequestParser::ERROR) {
        std::cout << "Error occurred during request parsing." << std::endl;
    } else {
        std::cout << "Incomplete request, more data needed." << std::endl;
    }

    return 0;
}
