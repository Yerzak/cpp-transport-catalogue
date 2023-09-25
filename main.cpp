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
        std::vector<std::string> final_result;//создаем результат
        for (auto& line : READING::ReadLineWithNumberSearch()) {//читаем построчно
            if (PARSING::IsSearchBus(line)) {//если ищем автобус
                BusInfo res = tc.FindBusInfo(std::move(PARSING::ParseSearchLine(line)));//парсим запрос и ищем инфу
                std::string query;//создаем строчку
                if (res.road.size() > 0) {//если у результата поиска есть маршрут
                    double curvature = tc.GetCurvature(res.road_length, res.road);
                    query = "Bus "s + res.name + ": "s + std::to_string(res.road.size()) + " stops on route, "s + std::to_string(res.unique_stops.size()) + " unique stops, "s + std::to_string(res.road_length) + " route length, "s + PARSING::To_string(static_cast<double>(curvature)) + " curvature"s;//в строчку записываем полное выражение, которое будем выводить, используя конкатенацию строк
                }
                else {//если нет такого маршрута
                    query = "Bus "s + res.name + ": "s + "not found"s;//в строчку записываем выражение, что не найден маршрут
                }
                final_result.push_back(query);//саму строчку добавляем в результат
            }
            else {//если запрашивается инфа об остановке
                auto [stop_name, bus_list] = tc.FindStopInfo(std::move(PARSING::ParseSearchLine(line)));//парсим запрос и ищем инфу по остановке
                std::string query;//создаем строчку
                if (tc.FindStop(stop_name) == nullptr) {//если такую остановку не добавляли
                    query = "Stop " + stop_name + ": not found";//записываем, что не найдено
                }
                else {//если есть такая остановка в справочнике
                    if (bus_list.size() > 0) {//если через нее проходят автобусы
                        query = "Stop " + stop_name + ": buses ";//записываем в строчку часть выражения
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
                            }//циклом добавляем к выражению перечень остановок
                        }
                    }
                    else {//если через нашу остановку не проходят маршруты
                        query = "Stop " + stop_name + ": no buses";//записываем в строчку, что нет автобусов
                    }
                }
                final_result.push_back(query);//саму строчку добавляем в результат
            }
        }
        for (const auto& query : final_result) {
            std::cout << query << std::endl;//выводим результат построчно
        }
    }
}

int main() {
    project::TransportCatalogue catalogue;
    project::AddInfo(catalogue);//передать данные для добавления
    project::SearchInfo(catalogue);//передать запросы
    return 0;
}