#include "input_reader.h"
#include <exception>
#include <iomanip>
namespace project {

    namespace READING {
        std::string ReadLineAdd(std::istream& input) {
            std::string s;
            getline(input, s);
            return s;
        }

        std::vector<std::string> SortInput(const std::vector<std::string>& lines) {
            std::vector<std::string> result;
            std::vector<std::string> buses;
            std::vector<std::string> stops_with_length;
            result.reserve(lines.size());
            buses.reserve(lines.size());
            for (auto& line : lines) {
                size_t begin = line.find_first_not_of(' ');
                if (line.substr(begin, begin + 3) == "Bus") {
                    buses.push_back(std::move(line));
                }
                else {
                    size_t point1 = line.find(',');
                    std::string second_part = line.substr(point1 + 1, line.size());
                    size_t point2 = second_part.find(',');
                    if (point2 == second_part.npos) {
                        result.push_back(std::move(line));
                    }
                    else {
                        stops_with_length.push_back(std::move(line));
                    }

                }
            }
            for (auto& line : stops_with_length) {
                result.push_back(std::move(line));
            }
            for (auto& line : buses) {
                result.push_back(std::move(line));
            }
            return result;
        }
        std::vector<std::string> ReadLineWithNumberAdd(std::istream& input) {
            size_t command_count;
            input >> command_count;
            std::vector<std::string> lines;
            lines.reserve(command_count + 1);
            std::string line;
            for (size_t i = 0; i <= command_count; ++i) {
                lines.push_back(ReadLineAdd(input));
            }
            lines.erase(lines.begin());
            return std::move(SortInput(lines));
        }
    }

    namespace PARSING {
        bool IsAddBus(std::string& line) {
            size_t point = line.find(':');
            size_t space = line.find(' ');
            if (point == std::string::npos) {
                std::cout << line << std::endl;
                throw std::invalid_argument(std::string("Adding element is invalid"));
            }
            std::string type_word = line.substr(0, space);
            if (type_word == "Bus") {
                return true;
            }
            return false;
        }

        BusBefore ParseBus(std::string& line) {
            size_t point = line.find(':');
            size_t space = line.find(' ');
            size_t begin = line.find_first_not_of(' ', space);
            std::string name = line.substr(begin, point - begin);//нашли имя
            std::vector<std::string> road;//создали вектор остановок
            road.reserve(100);//зарезервировали место
            bool IsLap;//показатель кругового маршрута
            std::string query = std::move(line.substr(point + 2, line.size()));//отделили маршрут от имени
            query.find('-') == query.npos ? IsLap = false : IsLap = true;//показатель кругового маршрута
            size_t begin_pos = query.find_first_not_of(' ');
            size_t end_pos = std::min(query.find('-'), query.find('>')) - 1;
            while (!query.empty() && query.find_first_not_of(' ') != query.npos) {
                road.push_back(query.substr(begin_pos, end_pos - begin_pos));
                query = query.substr(end_pos + 2, query.size());
                begin_pos = query.find_first_not_of(' ');
                end_pos = std::min((std::min(query.find('-'), query.find('>')) - 1), query.find_last_not_of(' '));
                if (std::min(query.find('-'), query.find('>')) == query.npos) {
                    road.push_back(std::move(query.substr(query.find_first_not_of(' '), query.size())));//если остановка последняя, то добавляем ее в вектор, не отыскивая следующий разделитель
                    break;
                }
            }
            if (IsLap) {//если кольцо - добавляем все остановки, кроме последней, в обратном порядке
                std::vector<std::string> second_part = road;
                second_part.pop_back();
                for (int i = static_cast<int>(second_part.size() - 1); i >= 0; --i) {
                    road.push_back(second_part[i]);
                }
            }
            road.shrink_to_fit();
            return { name, road };
        }

        StopBefore ParseStop(std::string& line) {
            size_t point = line.find(':');
            size_t space = line.find(' ');
            size_t begin = line.find_first_not_of(' ', space);
            StopBefore exemp;
            exemp.name = line.substr(begin, point - begin);//нашли имя
            std::string query = line.substr(point + 2, line.size());//отделили координаты и, возможно, расстояние от имени
            size_t lat_pos_begin = query.find_first_not_of(' ');
            size_t lat_pos_end = query.find(',');
            std::string lat = std::move(query.substr(lat_pos_begin, lat_pos_end - lat_pos_begin));
            exemp.lat = atof(lat.c_str());
            query = query.substr(lat_pos_end + 2, query.size());
            size_t lon_pos_begin = query.find_first_not_of(' ');
            size_t point2 = query.find(',');
            size_t lon_pos_end = std::min(query.find_last_not_of(' ') + 1, point2);
            std::string lon = std::move(query.substr(lon_pos_begin, lon_pos_end - lon_pos_begin));
            exemp.lng = atof(lon.c_str());
            //std::string length_info;
            if (point2 != query.npos) {
                query = query.substr(point2 + 1, query.size());
                size_t length_begin = query.find_first_not_of(' ');
                exemp.stop_info = std::move(query.substr(length_begin, query.size()));
            }
            return exemp;
        }
    }
}