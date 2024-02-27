// #include "HttpRequestParser.hpp"
// #include <iostream>
// #include <sstream>
// #include <algorithm>
// #include <iterator>
// #include <cctype>

// HttpRequestParser::HttpRequestParser() {
//     reset();
// }

// HttpRequestParser::~HttpRequestParser() {}

// int HttpRequestParser::parse(std::string &buffer) {
//     receivedData += buffer;
//     switch (status) {
//         case START_LINE:
//             return parseFirstLine();
//         case HEADERS:
//             return parseHeaders();
//         case PREBODY:
//             return parsePreBody();
//         case BODY:
//             return parseBody();
//         case CHUNK:
//             return parseChunk();
//         default:
//             return ERROR;
//     }
// }

// bool HttpRequestParser::isValidURI(const std::string &uri) {
//     // Implementation of URI validation
//     // This is left out for brevity, as it was provided in the previous examples
//     return true;
// }

// int HttpRequestParser::parseFirstLine() {
//     size_t posMethod = receivedData.find(" ");
//     size_t posUri = receivedData.find(" ", posMethod + 1);

//     if (posMethod == std::string::npos || posUri == std::string::npos)
//         return ERROR;

//     method = receivedData.substr(0, posMethod);
//     target = receivedData.substr(posMethod + 1, posUri - (posMethod + 1));
//     size_t posQuery = target.find("?");
//     if (posQuery != std::string::npos) {
//         queryString = target.substr(posQuery + 1);
//         target = target.substr(0, posQuery);
//     }

//     protocol = receivedData.substr(posUri + 1, receivedData.find("\r\n") - (posUri + 1));

//     status = HEADERS;
//     receivedData.erase(0, receivedData.find("\r\n") + 2);
//     return parseHeaders();
// }

// int HttpRequestParser::parseHeaders() {
//     size_t pos = receivedData.find("\r\n\r\n");
//     if (pos == std::string::npos)
//         return ERROR;

//     std::string headersStr = receivedData.substr(0, pos);

//     std::istringstream iss(headersStr);
//     std::string line;
//     while (std::getline(iss, line, '\n')) {  // Change delimiter to '\n'
//         // Skip empty lines
//         if (line == "\r")
//             continue;

//         size_t separator = line.find(": ");
//         if (separator != std::string::npos) {
//             std::string headerName = line.substr(0, separator);
//             std::string headerValue = line.substr(separator + 2);

//             // Trim leading and trailing whitespace from header name and value
//             headerName.erase(0, headerName.find_first_not_of(" \t\r\n"));
//             headerName.erase(headerName.find_last_not_of(" \t\r\n") + 1);
//             headerValue.erase(0, headerValue.find_first_not_of(" \t\r\n"));
//             headerValue.erase(headerValue.find_last_not_of(" \t\r\n") + 1);

//             headers[headerName] = headerValue;
//         }
//     }

//     receivedData.erase(0, pos + 4);

//     if (method != "POST" && method != "PUT") {
//         status = COMPLETE;
//         return COMPLETE;
//     }

//     auto it = headers.find("content-length");
//     if (it == headers.end()) {
//         // Check for chunked transfer encoding
//         auto chunkedIt = headers.find("transfer-encoding");
//         if (chunkedIt != headers.end() && chunkedIt->second == "chunked") {
//             status = CHUNK;
//             return parseChunk();
//         }

//         status = COMPLETE;
//         return COMPLETE;
//     }

//     contentLength = std::stoul(it->second);

//     status = BODY;
//     return parseBody();
// }

// int HttpRequestParser::parsePreBody() {
//     if (receivedData.size() >= contentLength) {
//         body = receivedData.substr(0, contentLength);
//         receivedData = receivedData.substr(contentLength);
//         status = COMPLETE;
//         return 0;
//     } else {
//         return 0; // Not enough data for full body
//     }
// }

// int HttpRequestParser::parseBody() {
//     if (receivedData.size() >= contentLength) {
//         body = receivedData.substr(0, contentLength);
//         receivedData.erase(0, contentLength);
//         status = COMPLETE;
//         return COMPLETE;
//     }

//     return ERROR;
// }

// int HttpRequestParser::parseChunk() {
//     while (status == CHUNK) {
//         if (receivedData.empty())
//             return 0;

//         if (chunkSize == 0) {
//             size_t pos = receivedData.find("\r\n");
//             if (pos != std::string::npos) {
//                 std::string chunkSizeStr = receivedData.substr(0, pos);
//                 receivedData = receivedData.substr(pos + 2);
//                 try {
//                     chunkSize = std::stoul(chunkSizeStr, nullptr, 16);
//                     if (chunkSize == 0) {
//                         // Last chunk
//                         status = COMPLETE;
//                         return 0;
//                     }
//                 } catch (const std::exception &e) {
//                     status = ERROR;
//                     return -1; // Invalid chunk size
//                 }
//             } else {
//                 return 0; // Not enough data for chunk size
//             }
//         }

//         size_t chunkEnd = chunkSize + 2; // '\r\n' at end of chunk
//         if (receivedData.size() >= chunkEnd) {
//             std::string chunk = receivedData.substr(0, chunkSize);
//             receivedData = receivedData.substr(chunkEnd);
//             body += chunk;
//             chunkSize = 0;
//         } else {
//             return 0; // Not enough data for full chunk
//         }
//     }

//     return 0;
// }

// const std::string& HttpRequestParser::getMethod() const {
//     return method;
// }

// const std::string& HttpRequestParser::getTarget() const {
//     return target;
// }

// const std::string& HttpRequestParser::getProtocol() const {
//     return protocol;
// }

// const std::map<std::string, std::string>& HttpRequestParser::getHeaders() const {
//     return headers;
// }

// std::string HttpRequestParser::getRequestBody() const {
//     return body;
// }

// void HttpRequestParser::reset() {
//     receivedData.clear();
//     method.clear();
//     target.clear();
//     queryString.clear();
//     protocol = "HTTP/1.1";
//     headers.clear();
//     body.clear();
//     contentLength = 0;
//     chunkSize = 0;
//     status = START_LINE;
// }

// HttpRequestParser::ParseStatus HttpRequestParser::getStatus() const {
//     return status;
// }
