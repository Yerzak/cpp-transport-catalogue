#include "input_reader.h"
#include "stat_reader.h"
#include "transport_catalogue.h"
#include <cassert>
#include <string>
#include<tuple>
#include<iomanip>
#include<cstdlib>
#include<algorithm>
using namespace std;
namespace project {
    void AddInfo(TransportCatalogue& tc) {
        for (auto& line : READING::ReadLineWithNumberAdd()) {
            if (PARSING::IsAddBus(line)) {
                auto one_bus = std::move(PARSING::ParseBus(line));
                tc.AddBus(std::move(one_bus));
            }
            else {
                auto one_stop = std::move(PARSING::ParseStop(line));
                tc.AddStop(std::move(one_stop));
            }
        }
    }

    void SearchInfo(TransportCatalogue& tc) {
        std::vector<std::string> final_result;//������� ���������
        for (auto& line : READING::ReadLineWithNumberSearch()) {//������ ���������
            if (PARSING::IsSearchBus(line)) {//���� ���� �������
                BusInfo res = tc.FindBusInfo(std::move(PARSING::ParseSearchLine(line)));//������ ������ � ���� ����
                std::string query;//������� �������
                if (res.road.size() > 0) {//���� � ���������� ������ ���� �������
                    double curvature = tc.GetCurvature(res.road_length, res.road);
                    query = "Bus "s + res.name + ": "s + std::to_string(res.road.size()) + " stops on route, "s + std::to_string(res.unique_stops.size()) + " unique stops, "s + std::to_string(res.road_length) + " route length, "s + PARSING::To_string(static_cast<double>(curvature)) + " curvature"s;//� ������� ���������� ������ ���������, ������� ����� ��������, ��������� ������������ �����
                }
                else {//���� ��� ������ ��������
                    query = "Bus "s + res.name + ": "s + "not found"s;//� ������� ���������� ���������, ��� �� ������ �������
                }
                final_result.push_back(query);//���� ������� ��������� � ���������
            }
            else {//���� ������������� ���� �� ���������
                auto [stop_name, bus_list] = tc.FindStopInfo(std::move(PARSING::ParseSearchLine(line)));//������ ������ � ���� ���� �� ���������
                std::string query;//������� �������
                if (tc.FindStop(stop_name) == nullptr) {//���� ����� ��������� �� ���������
                    query = "Stop " + stop_name + ": not found";//����������, ��� �� �������
                }
                else {//���� ���� ����� ��������� � �����������
                    if (bus_list.size() > 0) {//���� ����� ��� �������� ��������
                        query = "Stop " + stop_name + ": buses ";//���������� � ������� ����� ���������
                        std::vector<std::string> buses_;
                        buses_.reserve(bus_list.size());
                        for (const auto& bus_ptr : bus_list) {
                            buses_.push_back(bus_ptr->name);
                        }
                        std::sort(buses_.begin(), buses_.end());
                        bool IsFirst = true;
                        for (const auto& bus_name : buses_) {
                            if (IsFirst) {
                                query += bus_name;
                                IsFirst = false;
                            }
                            else {
                                query += (" " + bus_name);
                            }//������ ��������� � ��������� �������� ���������
                        }
                    }
                    else {//���� ����� ���� ��������� �� �������� ��������
                        query = "Stop " + stop_name + ": no buses";//���������� � �������, ��� ��� ���������
                    }
                }
                final_result.push_back(query);//���� ������� ��������� � ���������
            }
        }
        for (const auto& query : final_result) {
            std::cout << query << std::endl;//������� ��������� ���������
        }
    }
}

int main() {
    project::TransportCatalogue catalogue;
    project::AddInfo(catalogue);//�������� ������ ��� ����������
    project::SearchInfo(catalogue);//�������� �������
    return 0;
}