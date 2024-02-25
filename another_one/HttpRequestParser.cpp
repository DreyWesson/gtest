#include "HttpRequestParser.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <cctype>

HttpRequestParser::HttpRequestParser() {
    bodyOffset = 0;
    chunkSize = 0;
    status = FIRST_LINE;
    protocol = "HTTP/1.1";
    gettimeofday(&startTimer, NULL);
}

HttpRequestParser::~HttpRequestParser() {}

int HttpRequestParser::parse(std::string &buffer) {
    receivedData += buffer;
    switch (status) {
        case FIRST_LINE:
            return parseFirstLine();
        case HEADERS:
            return parseHeaders();
        case PREBODY:
            return parsePreBody();
        case BODY:
            return parseBody();
        case CHUNK:
            return parseChunk();
        default:
            return ERROR;
    }
}

bool HttpRequestParser::isValidURI(const std::string &uri) {
    // Scheme Validation:
    //     Ensures the scheme starts with a letter and contains only valid characters as per RFC 3986.
    // Authority Validation:
    //     Checks userinfo, host, and port components, ensuring each has valid characters.
    // Path Validation:
    //     Validates the path component, ensuring it contains only valid characters.
    // Query and Fragment Validation:
    //     Validates the query and fragment components, ensuring they contain only valid characters.
    // Helper Function isValidChar:
    //     A lambda function to check if a character is in a given set of valid characters.
    // URI Components Parsing:
    //     Splits the URI into scheme, authority, path, query, and fragment components.
    // Iterating Over Components:
    //     Checks each character in the URI components against the respective valid character sets.
    // Return:
    //     Returns true if all components are valid, otherwise false.

    // RFC 3986: URI Generic Syntax
    // Valid characters for different URI components
    const std::string validCharsUnreserved = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~";
    const std::string validCharsSubDelims = "!$&'()*+,;=";
    const std::string validCharsReserved = ":/?#[]@";
    const std::string validCharsPctEncoded = "0123456789ABCDEFabcdef";

    // RFC 3986: Scheme
    const std::string validCharsScheme = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-.";
    
    // RFC 3986: Authority
    const std::string validCharsUserInfo = validCharsUnreserved + validCharsSubDelims + ":";
    const std::string validCharsHost = validCharsUnreserved + validCharsSubDelims;
    const std::string validCharsPort = "0123456789";

    // RFC 3986: Path
    const std::string validCharsPath = validCharsPctEncoded + validCharsUnreserved + validCharsSubDelims + ":@/";

    // RFC 3986: Query
    const std::string validCharsQuery = validCharsPctEncoded + validCharsUnreserved + validCharsSubDelims + ":@/?";

    // RFC 3986: Fragment
    const std::string validCharsFragment = validCharsPctEncoded + validCharsUnreserved + validCharsSubDelims + ":@/?";

    // Helper lambda function to check if a character is in a given set of valid characters
    auto isValidChar = [](char c, const std::string &validSet) -> bool {
        return validSet.find(c) != std::string::npos;
    };

    // Parse URI components
    std::string scheme, authority, path, query, fragment;
    size_t posScheme = uri.find(':');
    if (posScheme != std::string::npos) {
        scheme = uri.substr(0, posScheme);
        if (!isValidChar(scheme[0], "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz")) {
            return false; // Scheme must start with a letter
        }
        for (char c : scheme) {
            if (!isValidChar(c, validCharsScheme)) {
                return false; // Invalid scheme character
            }
        }
    }

    size_t posAuthority = uri.find("//");
    if (posAuthority != std::string::npos) {
        size_t posEndAuthority = uri.find('/', posScheme + 3);
        if (posEndAuthority != std::string::npos) {
            authority = uri.substr(posScheme + 3, posEndAuthority - (posScheme + 3));
        } else {
            authority = uri.substr(posScheme + 3);
        }

        size_t posUserInfo = authority.find('@');
        std::string userinfo, host, port;
        if (posUserInfo != std::string::npos) {
            userinfo = authority.substr(0, posUserInfo);
            host = authority.substr(posUserInfo + 1);
        } else {
            host = authority;
        }

        size_t posPort = host.find(':');
        if (posPort != std::string::npos) {
            port = host.substr(posPort + 1);
            host = host.substr(0, posPort);
        }

        for (char c : userinfo) {
            if (!isValidChar(c, validCharsUserInfo)) {
                return false; // Invalid userinfo character
            }
        }

        for (char c : host) {
            if (!isValidChar(c, validCharsHost)) {
                return false; // Invalid host character
            }
        }

        for (char c : port) {
            if (!isValidChar(c, validCharsPort)) {
                return false; // Invalid port character
            }
        }
    }

    size_t posQuery = uri.find('?');
    if (posQuery != std::string::npos) {
        query = uri.substr(posQuery + 1);
        for (char c : query) {
            if (!isValidChar(c, validCharsQuery)) {
                return false; // Invalid query character
            }
        }
    }

    size_t posFragment = uri.find('#');
    if (posFragment != std::string::npos) {
        fragment = uri.substr(posFragment + 1);
        for (char c : fragment) {
            if (!isValidChar(c, validCharsFragment)) {
                return false; // Invalid fragment character
            }
        }
    }

    // Validate Path
    path = uri.substr(posAuthority + 2);
    for (char c : path) {
        if (!isValidChar(c, validCharsPath)) {
            return false; // Invalid path character
        }
    }

    return true;
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
    while (std::getline(iss, line, '\r')) {
        size_t separator = line.find(": ");
        if (separator != std::string::npos) {
            std::string headerName = line.substr(0, separator);
            std::string headerValue = line.substr(separator + 2);
            headers[headerName] = headerValue;
        }
    }

    receivedData.erase(0, pos + 4);

    if (method != "POST" && method != "PUT") {
        status = COMPLETE;
        return COMPLETE;
    }

    auto it = headers.find("content-length");
    if (it == headers.end()) {
        status = COMPLETE;
        return COMPLETE;
    }

    contentLength = std::stoul(it->second);

    status = BODY;
    return parseBody();
}

int HttpRequestParser::parsePreBody() {
    if (receivedData.size() >= contentLength) {
        body = receivedData.substr(0, contentLength);
        receivedData = receivedData.substr(contentLength);
        status = COMPLETE;
        return 0;
    } else {
        return 0; // Not enough data for full body
    }
}

int HttpRequestParser::parseBody() {
    if (receivedData.size() >= contentLength) {
        body = receivedData.substr(0, contentLength);
        receivedData.erase(0, contentLength);
        status = COMPLETE;
        return COMPLETE;
    }

    return ERROR;
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
                    chunkSize = std::stoul(chunkSizeStr, nullptr, 16);
                    std::cout << "Chunk Size: " << chunkSize << std::endl;
                    if (chunkSize == 0) {
                        status = COMPLETE;
                        return 0;
                    }
                } catch (const std::exception &e) {
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

const std::map<std::string, std::string>& HttpRequestParser::getHeaders() const {
    return headers;
}

std::string HttpRequestParser::getRequestBody() const {
    return body;
}

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
    status = FIRST_LINE;
}

HttpRequestParser::ParseStatus HttpRequestParser::getStatus() const {
    return status;
}