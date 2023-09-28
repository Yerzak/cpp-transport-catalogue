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
        std::string ReadLineAdd(std::istream& input);//��������� ���� ������
        std::vector<std::string> SortInput(const std::vector<std::string>& lines);//������������� ������
        std::vector<std::string> ReadLineWithNumberAdd(std::istream& input);//��������� ��
    }
    namespace PARSING {
        bool IsAddBus(std::string& line);//�������� �� ����������� ���������� ���������
        //std::pair<std::string, std::vector<std::string>> ParseBus(std::string& line);//������ �������
        BusBefore ParseBus(std::string& line);//������ �������
        //std::tuple<std::string, double, double, std::string> ParseStop(std::string& line);//������ ���������
        StopBefore ParseStop(std::string& line);//������ ���������
    }
}

