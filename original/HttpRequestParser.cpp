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

// int HttpRequestParser::parse(std::string &buffer) {
//     size_t ret = 0;

//     gettimeofday(&lastTimer, NULL);

//     receivedData += buffer;
//     buffer.clear();

//     if (status == FIRST_LINE)
//         ret = parseFirstLine();
//     if (status == HEADERS)
//         ret = parseHeaders();
//     if (status == PREBODY)
//         ret = parsePreBody();
//     if (status == BODY)
//         ret = parseBody();
//     if (status == CHUNK)
//         ret = parseChunk();
//     if (status == COMPLETE || ret == 1) {
//         status = COMPLETE;
//         return ret;
//     }
//     else if (status == ERROR || ret > 1) {
//         status = ERROR;
//         return ret;
//     }
//     return ret;
// }

int HttpRequestParser::parse(std::string &buffer) {
    size_t ret = 0;

    gettimeofday(&lastTimer, NULL);

    receivedData += buffer;
    buffer.clear();

    size_t pos;
    while ((pos = receivedData.find("\r\n")) != std::string::npos) {
        std::string chunk = receivedData.substr(0, pos + 2); // Include '\r\n' in chunk
        receivedData = receivedData.substr(pos + 2);

        if (status == FIRST_LINE)
            ret = parseFirstLine();
        if (status == HEADERS)
            ret = parseHeaders();
        if (status == PREBODY)
            ret = parsePreBody();
        if (status == BODY)
            ret = parseBody();
        if (status == CHUNK)
            ret = parseChunk();
        if (status == COMPLETE || ret == 1) {
            status = COMPLETE;
            return ret;
        }
        else if (status == ERROR || ret > 1) {
            status = ERROR;
            return ret;
        }
    }

    return ret;
}


bool HttpRequestParser::isValidURI(const std::string &uri) {
    // Explanation:

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

    size_t posPath = posAuthority + authority.length() + 2;
    size_t posEndPath = std::min(posQuery, posFragment);
    if (posEndPath == std::string::npos) {
        posEndPath = uri.length();
    }
    path = uri.substr(posPath, posEndPath - posPath);
    for (char c : path) {
        if (!isValidChar(c, validCharsPath)) {
            return false; // Invalid path character
        }
    }

    // All components are valid
    return true;
}


bool HttpRequestParser::isValidMethod(const std::string &method) {
    static const std::set<std::string> validMethods = {
        "GET", "POST", "PUT", "DELETE", "HEAD", "OPTIONS", "PATCH", "CONNECT", "TRACE"
    };

    return validMethods.find(method) != validMethods.end();
}

std::string HttpRequestParser::getMethod() const {
    return method;
}

std::string HttpRequestParser::getTarget() const {
    return target;
}

std::string HttpRequestParser::getProtocol() const {
    return protocol;
}

const std::map<std::string, std::string>& HttpRequestParser::getHeaders() const {
    return headers;
}

std::string HttpRequestParser::getRequestBody() const {
    return requestBody;
}

// int HttpRequestParser::parseFirstLine() {
//     size_t pos = receivedData.find("\r\n");
//     if (pos == std::string::npos) return 0;

//     std::string line = receivedData.substr(0, pos);
//     receivedData = receivedData.substr(pos + 2);

//     std::istringstream iss(line);
//     std::string tmp;
//     if (iss >> tmp) {
//         if (isValidMethod(tmp)) {
//             method = tmp;
//         } else {
//             return 501;
//         }
//     } else {
//         return 400;
//     }

//     if (iss >> tmp) {
//         if (tmp[0] != '/') {
//             return 400;
//         }
//         if (!isValidURI(tmp)) {
//             return 403;
//         }
//         if (tmp.length() < 100000) {
//             target = tmp;
//         } else {
//             return 414;
//         }
//     } else {
//         return 400;
//     }

//     if (iss >> tmp) {
//         if (tmp != "HTTP/1.1") {
//             return 505;
//         } else {
//             protocol = tmp;
//         }
//     } else {
//         return 400;
//     }

//     status = HEADERS;
//     return 0;
// }

int HttpRequestParser::parseFirstLine() {
    size_t pos = receivedData.find("\r\n");
    if (pos == std::string::npos) return 0;

    std::string line = receivedData.substr(0, pos);
    receivedData = receivedData.substr(pos + 2);

    std::istringstream iss(line);
    std::string tmp;
    if (iss >> tmp) {
        if (isValidMethod(tmp)) {
            method = tmp;
        } else {
            return 501;
        }
    } else {
        return 400;
    }

    if (iss >> tmp) {
        if (tmp[0] != '/') {
            return 400;
        }
        if (!isValidURI(tmp)) {
            return 403;
        }
        target = tmp;
    } else {
        return 400;
    }

    if (iss >> tmp) {
        if (tmp != "HTTP/1.1") {
            return 505;
        } else {
            protocol = tmp;
        }
    } else {
        return 400;
    }

    status = HEADERS;
    return 0;
}

// int HttpRequestParser::parseHeaders() {
//     while (true) {
//         size_t pos = receivedData.find("\r\n");
//         if (pos == std::string::npos) return 0;

//         std::string line = receivedData.substr(0, pos);
//         receivedData = receivedData.substr(pos + 2);

//         if (line.empty()) {
//             status = PREBODY;
//             break;
//         }

//         std::istringstream iss(line);
//         std::string header, value;
//         if (std::getline(iss, header, ':') && std::getline(iss >> std::ws, value)) {
//             if (header.empty() || header[header.length() - 1] == ' ') {
//                 return 400;
//             }
//             if (header.length() > 1000 || value.length() > 4000) {
//                 return 400;
//             }
//             headers[header] = trim(value);
//             if (headers[header].empty()) {
//                 headers.erase(header);
//             }
//         } else {
//             return 400;
//         }
//     }
//     return 0;
// }

// int HttpRequestParser::parseHeaders() {
//     while (true) {
//         size_t pos = receivedData.find("\r\n");
//         if (pos == std::string::npos) return 0;

//         std::string line = receivedData.substr(0, pos);
//         receivedData = receivedData.substr(pos + 2);

//         if (line.empty()) {
//             status = PREBODY;
//             break;
//         }

//         std::istringstream iss(line);
//         std::string header, value;
//         if (std::getline(iss, header, ':') && std::getline(iss >> std::ws, value)) {
//             if (header.empty() || header[header.length() - 1] == ' ') {
//                 return 400;
//             }
//             headers[header] = trim(value);
//             if (headers[header].empty()) {
//                 headers.erase(header);
//             }
//         } else {
//             return 400;
//         }
//     }
//     return 0;
// }
int HttpRequestParser::parseHeaders() {
    while (true) {
        size_t pos = receivedData.find("\r\n");
        if (pos == std::string::npos) return 0;

        std::string line = receivedData.substr(0, pos);
        receivedData = receivedData.substr(pos + 2);

        if (line.empty()) {
            status = PREBODY;
            break;
        }

        std::istringstream iss(line);
        std::string header, value;
        if (std::getline(iss, header, ':') && std::getline(iss >> std::ws, value)) {
            if (header.empty() || header[header.length() - 1] == ' ') {
                return 400;
            }
            headers[header] = trim(value);
            if (headers[header].empty()) {
                headers.erase(header);
            }
            
            // Print out Content-Length header
            if (header == "Content-Length") {
                std::cout << "Content-Length: " << headers[header] << std::endl;
            }
        } else {
            return 400;
        }
    }
    return 0;
}


int HttpRequestParser::parsePreBody() {
    bodyOffset = 0;

    if (headers.find("Host") == headers.end() || headers["Host"].empty()) {
        return 400;
    }

    if (headers["Host"].find("@") != std::string::npos) {
        return 400;
    }

    if (headers.find("Transfer-Encoding") != headers.end() && headers["Transfer-Encoding"] == "chunked") {
        status = CHUNK;
        chunkStatus = CHUNK_SIZE;
    } else if (headers.find("Content-Length") != headers.end()) {
        if (headers["Content-Length"].find_first_not_of("0123456789") != std::string::npos) {
            return 400;
        }
        try {
            length = std::stoi(headers["Content-Length"]);
            if (length < 0) {
                throw std::invalid_argument("negative content-length");
            }
        }
        catch (std::exception &e) {
            return 400;
        }
        status = BODY;
    } else {
        return 1;
    }

    if (method != "POST" && method != "PUT") {
        return 1;
    }

    return 0;
}

int HttpRequestParser::parseChunkTrailer() {
    while (true) {
        size_t pos = receivedData.find("\r\n");
        if (pos == std::string::npos) return 0;

        std::string line = receivedData.substr(0, pos);
        receivedData = receivedData.substr(pos + 2);

        if (line.empty()) {
            break;
        }

        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string header = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 1);
            headers[header] = trim(value);
        } else {
            return 400;
        }
    }
    return 1;
}

int HttpRequestParser::parseChunk() {
    while (true) {
        size_t pos = receivedData.find("\r\n");
        if (pos == std::string::npos) return 0;

        std::string line = receivedData.substr(0, pos);
        receivedData = receivedData.substr(pos + 2);

        if (chunkStatus == CHUNK_SIZE) {
            std::string hex = line;
            chunkSize = toHex(hex);
            chunkStatus = CHUNK_BODY;
        } else if (chunkStatus == CHUNK_BODY) {
            if (chunkSize == 0) {
                if (!receivedData.empty()) {
                    return parseChunkTrailer();
                }
                return 1;
            }
            requestBody += line;
            chunkSize = 0;
            chunkStatus = CHUNK_SIZE;
        }
    }
}

// int HttpRequestParser::parseBody() {
//     if (receivedData.length() >= length) {
//         requestBody.insert(bodyOffset, receivedData, 0, length);
//         bodyOffset += receivedData.length();
//         receivedData.clear();

//         if (requestBody.length() == length) {
//             return 1;
//         } else {
//             return 400;
//         }
//     }
//     return 0;
// }

// int HttpRequestParser::parseBody() {
//     if (receivedData.length() >= length) {
//         requestBody = receivedData.substr(0, length);
//         receivedData = receivedData.substr(length);

//         if (requestBody.length() == length) {
//             return 1;
//         } else {
//             return 400;
//         }
//     }
//     return 0;
// }

// int HttpRequestParser::parseBody() {
//     std::cout << "Parsing Body. Length: " << length << ", Received: " << receivedData.length() << std::endl;
    
//     if (receivedData.length() >= length) {
//         requestBody = receivedData.substr(0, length);
//         receivedData = receivedData.substr(length);

//         if (requestBody.length() == length) {
//             std::cout << "Body parsed successfully." << std::endl;
//             return 1;
//         } else {
//             std::cout << "Body length mismatch." << std::endl;
//             return 400;
//         }
//     }

//     std::cout << "Not enough data for body." << std::endl;
//     return 0;
// }

int HttpRequestParser::parseBody() {
    std::cout << "Parsing Body. Length: " << length << ", Received: " << receivedData.length() << std::endl;
    std::cout << "Received Data: " << receivedData << std::endl;
    
    if (receivedData.length() >= length) {
        requestBody = receivedData.substr(0, length);
        receivedData = receivedData.substr(length);

        if (requestBody.length() == length) {
            std::cout << "Body parsed successfully." << std::endl;
            std::cout << "Request Body: " << requestBody << std::endl;
            return 1;
        } else {
            std::cout << "Body length mismatch." << std::endl;
            return 400;
        }
    }

    std::cout << "Not enough data for body." << std::endl;
    return 0;
}



bool HttpRequestParser::isTimeout() {
    if (status != COMPLETE) {
        status = ERROR;
        return true;
    }
    return false;
}

int HttpRequestParser::getStatus() {
    return status;
}

time_t HttpRequestParser::getStartTimeInSeconds() {
    return startTimer.tv_sec;
}

time_t HttpRequestParser::getLastTimeInSeconds() {
    return lastTimer.tv_sec;
}

std::string HttpRequestParser::trim(const std::string &s) {
    std::string result = s;
    result.erase(result.begin(), std::find_if(result.begin(), result.end(), [](int ch) {
        return !std::isspace(ch);
    }));
    result.erase(std::find_if(result.rbegin(), result.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), result.end());
    return result;
}

int HttpRequestParser::toHex(const std::string &hexStr) {
    std::stringstream ss;
    ss << std::hex << hexStr;
    int result;
    ss >> result;
    return result;
}

void HttpRequestParser::parseQueryString(const std::string &queryString) {
    std::istringstream iss(queryString);
    std::string token;
    while (std::getline(iss, token, '&')) {
        size_t pos = token.find('=');
        if (pos != std::string::npos) {
            std::string key = token.substr(0, pos);
            std::string value = token.substr(pos + 1);
            queryParameters[key] = value;
        }
    }
}

void HttpRequestParser::parseCookies(const std::string &cookieHeader) {
    std::istringstream iss(cookieHeader);
    std::string token;
    while (std::getline(iss, token, ';')) {
        size_t pos = token.find('=');
        if (pos != std::string::npos) {
            std::string key = token.substr(0, pos);
            std::string value = token.substr(pos + 1);
            cookies[key] = value;
        }
    }
}

void HttpRequestParser::parseContentEncoding(const std::string &encodingHeader) {
    std::istringstream iss(encodingHeader);
    std::string token;
    while (std::getline(iss, token, ',')) {
        contentEncodings.push_back(trim(token));
    }
}

void HttpRequestParser::parseTransferEncoding(const std::string &encodingHeader) {
    std::istringstream iss(encodingHeader);
    std::string token;
    while (std::getline(iss, token, ',')) {
        transferEncodings.push_back(trim(token));
    }
}

