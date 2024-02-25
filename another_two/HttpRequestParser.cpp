#include "HttpRequestParser.hpp"
#include <sys/time.h>

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


void HttpRequestParser::parseHeaderLine(const std::string& line) {
    std::vector<std::string> parts = split(line, ':');
    if (parts.size() == 2) {
        std::string key = trim(parts[0]);
        std::string value = trim(parts[1]);
        headers[key] = value;
    }
}

int HttpRequestParser::parseHeaders() {
    size_t pos = receivedData.find("\r\n\r\n");
    if (pos != std::string::npos) {
        std::istringstream ss(receivedData.substr(0, pos));
        std::string line;
        while (std::getline(ss, line, '\r')) {
            parseHeaderLine(line);
        }
        receivedData = receivedData.substr(pos + 4);
        status = PREBODY;
        return parsePreBody();
    }
    return ERROR;
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

HttpRequestParser::ParseStatus HttpRequestParser::getStatus() const {
    return status;
}

std::vector<std::string> HttpRequestParser::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::istringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

std::string HttpRequestParser::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) {
        return "";
    }
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

int HttpRequestParser::parseFirstLine() {
    size_t pos = receivedData.find("\r\n");
    if (pos != std::string::npos) {
        std::istringstream ss(receivedData.substr(0, pos));
        std::vector<std::string> parts;
        std::string part;
        while (ss >> part) {
            parts.push_back(part);
        }
        if (parts.size() == 3) {
            method = parts[0];
            target = parts[1];
            protocol = parts[2];
            status = HEADERS;
            receivedData = receivedData.substr(pos + 2);
            return parseHeaders();
        }
    }
    return ERROR;
}

int HttpRequestParser::parsePreBody() {
    std::map<std::string, std::string>::iterator it = headers.find("Content-Length");
    if (it != headers.end()) {
        contentLength = std::strtoul(it->second.c_str(), NULL, 10);
        status = BODY;
        return parseBody();
    }
    status = CHUNK;
    return parseChunk();
}

int HttpRequestParser::parseBody() {
    if (receivedData.size() >= contentLength + bodyOffset) {
        body = receivedData.substr(bodyOffset, contentLength);
        receivedData = receivedData.substr(bodyOffset + contentLength);
        return COMPLETE;
    }
    return ERROR;
}

int HttpRequestParser::parseChunk() {
    size_t pos = receivedData.find("\r\n");
    if (pos != std::string::npos) {
        std::istringstream ss(receivedData.substr(0, pos));
        ss >> std::hex >> chunkSize;
        if (chunkSize > 0) {
            receivedData = receivedData.substr(pos + 2);
            if (receivedData.size() >= chunkSize + 2) {
                body += receivedData.substr(0, chunkSize);
                receivedData = receivedData.substr(chunkSize + 2);
                return parseChunk();
            }
        } else {
            // Last chunk received
            status = COMPLETE;
            return COMPLETE;
        }
    }
    return ERROR;
}
