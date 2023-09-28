#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <tuple>
namespace project {
    struct BusBefore {
        std::string name;
        std::vector<std::string> road;
    };
    struct StopBefore {
        std::string name, stop_info;
        double lat, lng;
    };
    namespace READING {
        std::string ReadLineAdd(std::istream& input);//прочитать одну строку
        std::vector<std::string> SortInput(const std::vector<std::string>& lines);//отсортировать строки
        std::vector<std::string> ReadLineWithNumberAdd(std::istream& input);//прочитать всё
    }
    namespace PARSING {
        bool IsAddBus(std::string& line);//является ли добавляемая информация маршрутом
        //std::pair<std::string, std::vector<std::string>> ParseBus(std::string& line);//парсит маршрут
        BusBefore ParseBus(std::string& line);//парсит маршрут
        //std::tuple<std::string, double, double, std::string> ParseStop(std::string& line);//парсит остановку
        StopBefore ParseStop(std::string& line);//парсит остановку
    }
}

