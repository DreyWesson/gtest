#include "HttpRequestParser.hpp"
#include <iostream>
#include <cassert>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <cctype>

HttpRequestParser::HttpRequestParser() {
    reset();
}

HttpRequestParser::~HttpRequestParser() {}

void HttpRequestParser::reset() {
    receivedData.clear();
    method.clear();
    target.clear();
    queryString.clear();
    protocol = "HTTP/1.1";
    headers.clear();
    body.clear();
    contentLength = 0;
    bodyOffset = 0;
    chunkSize = 0;
    status = START_LINE;
}

int HttpRequestParser::parse(std::string &buffer) {
    receivedData += buffer;
    std::cout << "Received Data: [\n" << receivedData << "]" << std::endl;
    std::cout << "Received Data Size: " << receivedData.size() << std::endl;

    switch (status) {
        case START_LINE:
            // std::cout << "Current Status: START_LINE" << std::endl;
            return parseFirstLine();
        case HEADERS:
            // std::cout << "Current Status: HEADERS" << std::endl;
            return parseHeaders();
        case PREBODY:
            // std::cout << "Current Status: PREBODY" << std::endl;
            return parsePreBody();
        case BODY:
            // std::cout << "Current Status: BODY" << std::endl;
            return parseBody();
        case CHUNK:
            // std::cout << "Current Status: CHUNK" << std::endl;
            return parseChunk();
        case COMPLETE:
            // std::cout << "Current Status: COMPLETE" << std::endl;
            return COMPLETE;
        case ERROR:
            // std::cout << "Current Status: ERROR" << std::endl;
            return ERROR;
        default:
            return ERROR;
    }
}

int HttpRequestParser::parseFirstLine() {
    size_t posMethod = receivedData.find(" ");
    size_t posUri = receivedData.find(" ", posMethod + 1);

    if (posMethod == std::string::npos || posUri == std::string::npos)
        return ERROR;

    method = receivedData.substr(0, posMethod);
    target = receivedData.substr(posMethod + 1, posUri - (posMethod + 1));
    size_t posQuery = target.find("?");
    if (posQuery != std::string::npos) {
        queryString = target.substr(posQuery + 1);
        target = target.substr(0, posQuery);
    }

    protocol = receivedData.substr(posUri + 1, receivedData.find("\r\n") - (posUri + 1));

    // Remove whitespace from method, target, and protocol
    method = removeWhitespace(method);
    target = removeWhitespace(target);
    protocol = removeWhitespace(protocol);

    status = HEADERS;
    receivedData.erase(0, receivedData.find("\r\n") + 2);
    return parseHeaders();
}

int HttpRequestParser::parseHeaders() {
    size_t pos = receivedData.find("\r\n\r\n");
    if (pos == std::string::npos)
        return ERROR;

    std::string headersStr = receivedData.substr(0, pos);

    std::istringstream iss(headersStr);
    std::string line;
    while (std::getline(iss, line, '\n')) {  // Change delimiter to '\n'
        // Skip empty lines
        if (line == "\r")
            continue;

        size_t separator = line.find(": ");
        if (separator != std::string::npos) {
            std::string headerName = line.substr(0, separator);
            std::string headerValue = line.substr(separator + 2);

            // Trim leading and trailing whitespace from header name and value
            headerName.erase(0, headerName.find_first_not_of(" \t\r\n"));
            headerName.erase(headerName.find_last_not_of(" \t\r\n") + 1);
            headerValue.erase(0, headerValue.find_first_not_of(" \t\r\n"));
            headerValue.erase(headerValue.find_last_not_of(" \t\r\n") + 1);

            headers[headerName] = headerValue;
        }
    }

    receivedData.erase(0, pos + 4);

    if (method != "POST" && method != "PUT") {
        status = COMPLETE;
        return COMPLETE;
    }

    status = BODY;
    return parseBody();
}

int HttpRequestParser::parsePreBody() {
    if (receivedData.size() >= contentLength) {
        body = receivedData.substr(0, contentLength);
        receivedData = receivedData.substr(contentLength); // Update receivedData
        status = COMPLETE;
        return COMPLETE;
    } else {
        return 0; // Not enough data for full body, wait for more data
    }
}

int HttpRequestParser::parseBody() {
    // Find the position of the first non-whitespace character
    size_t startPos = receivedData.find_first_not_of(" \t\r\n");
    if (startPos == std::string::npos) {
        // If only whitespace, return, waiting for more data
        return 0;
    }

    // Find the position of the last non-whitespace character
    size_t endPos = receivedData.find_last_not_of(" \t\r\n");
    if (endPos == std::string::npos) {
        // Unreachable, as there should always be some non-whitespace after finding the startPos
        endPos = receivedData.size() - 1;
    }

    // Extract the body without leading/trailing whitespace, tabs, and newlines
    body = receivedData.substr(startPos, endPos - startPos + 1);
    body = removeWhitespace(body);
    receivedData.clear(); // Clear the received data as it's been consumed
    status = COMPLETE;

    return COMPLETE;
}

int HttpRequestParser::parseChunk() {
    while (status == CHUNK) {
        if (receivedData.empty())
            return 0;

        if (chunkSize == 0) {
            size_t pos = receivedData.find("\r\n");
            if (pos != std::string::npos) {
                std::string chunkSizeStr = receivedData.substr(0, pos);
                receivedData = receivedData.substr(pos + 2);
                try {
                    chunkSize = std::stoul(chunkSizeStr, NULL, 16);
                    if (chunkSize == 0) {
                        status = COMPLETE;
                        return 0;
                    }
                } catch (...) {
                    status = ERROR;
                    return -1; // Invalid chunk size
                }
            } else {
                return 0; // Not enough data for chunk size
            }
        }

        size_t chunkEnd = chunkSize + 2; // '\r\n' at end of chunk
        if (receivedData.size() >= chunkEnd) {
            std::string chunk = receivedData.substr(0, chunkSize);
            receivedData = receivedData.substr(chunkEnd);
            body += chunk;
            chunkSize = 0;
        } else {
            return 0; // Not enough data for full chunk
        }
    }

    return 0;
}

const std::string& HttpRequestParser::getMethod() const {
    return method;
}

const std::string& HttpRequestParser::getTarget() const {
    return target;
}

const std::string& HttpRequestParser::getProtocol() const {
    return protocol;
}

const std::string& HttpRequestParser::getQueryString() const {
    return queryString;
}

const std::map<std::string, std::string>& HttpRequestParser::getHeaders() const {
    return headers;
}

std::string HttpRequestParser::getRequestBody() const {
    return body;
}

HttpRequestParser::ParseStatus HttpRequestParser::getStatus() const {
    return status;
}

std::string HttpRequestParser::removeWhitespace(const std::string& str) {
    std::string result;
    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
        if (!std::isspace(*it)) {
            result += *it;
        }
    }
    return result;
}
