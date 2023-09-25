#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <tuple>
namespace project {
    namespace READING {
        std::string ReadLineAdd();//��������� ���� ������
        std::vector<std::string> SortInput(std::vector<std::string> lines);//������������� ������
        std::vector<std::string> ReadLineWithNumberAdd();//��������� ��
    }
    namespace PARSING {
        bool IsAddBus(std::string& line);//�������� �� ����������� ���������� ���������
        std::pair<std::string, std::vector<std::string>> ParseBus(std::string& line);//������ �������
        std::tuple<std::string, double, double, std::string> ParseStop(std::string& line);//������ ���������
    }
}