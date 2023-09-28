#pragma once
#include <iostream>
#include "transport_catalogue.h"
namespace project {
    namespace READING {
        std::string ReadLineSearch(std::istream& input);//прочитать одну строку
        std::vector<std::string> ReadLineWithNumberSearch(std::istream& input);//прочитать всё
    }
    namespace PARSING {
        std::string ParseSearchLine(std::string line);
        std::string To_string(const double& d);
        bool IsSearchBus(std::string& line);
    }
}