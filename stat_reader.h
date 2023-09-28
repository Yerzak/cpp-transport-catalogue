#pragma once
#include <iostream>
#include "transport_catalogue.h"
namespace project {
    namespace READING {
        std::string ReadLineSearch(std::istream& input);//��������� ���� ������
        std::vector<std::string> ReadLineWithNumberSearch(std::istream& input);//��������� ��
    }
    namespace PARSING {
        std::string ParseSearchLine(std::string line);
        std::string To_string(const double& d);
        bool IsSearchBus(std::string& line);
    }
}