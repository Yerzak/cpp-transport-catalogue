#include "stat_reader.h"
#include<string>
#include<iomanip>
#include<sstream>
namespace project {
    namespace READING {
        std::string ReadLineSearch(std::istream& input) {
            std::string s;
            getline(input, s);
            return s;
        }
        std::vector<std::string> ReadLineWithNumberSearch(std::istream& input) {
            size_t command_count;
            input >> command_count;
            std::vector<std::string> lines;
            lines.reserve(command_count + 1);
            std::string line;
            for (size_t i = 0; i <= command_count; ++i) {
                lines.push_back(ReadLineSearch(input));
            }
            lines.erase(lines.begin());
            return lines;
        }
    }
    namespace PARSING {

        std::string To_string(const double& d) {//функция для передачи длин в формат строк
            std::ostringstream strm;
            strm << std::setprecision(6) << d;
            return strm.str();
        }

        std::string ParseSearchLine(std::string line) {
            size_t begin = line.find_first_not_of(' ');
            line = std::move(line.substr(begin, line.size()));
            if (line[0] == 'B') {
                begin = 4;
            }
            else {
                begin = 5;
            }
            return(std::move(line.substr(begin, line.size())));
        }

        bool IsSearchBus(std::string& line) {
            size_t begin = line.find_first_not_of(' ');
            size_t len = 3;
            if (begin == std::string::npos) {
                std::cout << line << std::endl;
                throw std::invalid_argument(std::string("Searching element is invalid"));
            }
            std::string type_word = line.substr(0, len);
            if (type_word == "Bus") {
                return true;
            }
            return false;
        }
    }
}