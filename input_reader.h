#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <tuple>
namespace project {
    namespace READING {
        std::string ReadLineAdd();//прочитать одну строку
        std::vector<std::string> SortInput(std::vector<std::string> lines);//отсортировать строки
        std::vector<std::string> ReadLineWithNumberAdd();//прочитать всё
    }
    namespace PARSING {
        bool IsAddBus(std::string& line);//является ли добавляемая информация маршрутом
        std::pair<std::string, std::vector<std::string>> ParseBus(std::string& line);//парсит маршрут
        std::tuple<std::string, double, double, std::string> ParseStop(std::string& line);//парсит остановку
    }
}