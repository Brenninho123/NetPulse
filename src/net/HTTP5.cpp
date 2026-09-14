#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <algorithm>

namespace NetPulse {
    namespace Net {
        struct HTTP5Request {
            std::string method;
            std::string path;
            std::string version;
            std::unordered_map<std::string, std::string> headers;
            std::string body;
        };

        struct HTTP5Response {
            int status_code;
            std::string status_message;
            std::unordered_map<std::string, std::string> headers;
            std::string body;

            std::string serialize() const {
                std::ostringstream ss;
                ss << "HTTP/1.1 " << status_code << " " << status_message << "\r\n";
                for (const auto& [key, value] : headers) {
                    ss << key << ": " << value << "\r\n";
                }
                ss << "Content-Length: " << body.length() << "\r\n";
                ss << "Connection: close\r\n\r\n";
                ss << body;
                return ss.str();
            }
        };

        class HTTP5Parser {
        public:
            static HTTP5Request parse(const std::string& raw_request) {
                HTTP5Request req;
                std::istringstream stream(raw_request);
                std::string line;

                if (std::getline(stream, line)) {
                    if (!line.empty() && line.back() == '\r') {
                        line.pop_back();
                    }
                    std::istringstream line_stream(line);
                    line_stream >> req.method >> req.path >> req.version;
                }

                while (std::getline(stream, line) && line != "\r" && !line.empty()) {
                    if (!line.empty() && line.back() == '\r') {
                        line.pop_back();
                    }
                    auto pos = line.find(':');
                    if (pos != std::string::npos) {
                        std::string key = line.substr(0, pos);
                        std::string value = line.substr(pos + 1);
                        key.erase(0, key.find_first_not_of(" \t"));
                        key.erase(key.find_last_not_of(" \t") + 1);
                        value.erase(0, value.find_first_not_of(" \t"));
                        value.erase(value.find_last_not_of(" \t") + 1);
                        req.headers[key] = value;
                    }
                }

                std::ostringstream body_stream;
                body_stream << stream.rdbuf();
                req.body = body_stream.str();

                return req;
            }

            static HTTP5Response create_response(int code, const std::string& message, const std::string& body, const std::string& content_type = "application/json") {
                HTTP5Response res;
                res.status_code = code;
                res.status_message = message;
                res.body = body;
                res.headers["Content-Type"] = content_type;
                res.headers["Server"] = "NetPulse/0.1.0 Engine";
                return res;
            }
        };
    }
}
