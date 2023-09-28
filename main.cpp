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
        for (auto& line : READING::ReadLineWithNumberAdd(std::cin)) {
            if (PARSING::IsAddBus(line)) {
                auto one_bus = std::move(PARSING::ParseBus(line));
                tc.AddBus(one_bus);
            }
            else {
                auto one_stop = std::move(PARSING::ParseStop(line));
                tc.AddStop(one_stop);
            }
        }
    }

    void SearchInfo(TransportCatalogue& tc) {
        std::vector<std::string> final_result;//создаем результат
        for (auto& line : READING::ReadLineWithNumberSearch(std::cin)) {//читаем построчно
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

    namespace Tests {
        /*void PrintInfo(BusInfo& info) {
            if (info.road.size() > 0) {
                std::cout << "Bus "s << info.name << ": "s << info.road.size() << " stops on route, "s << info.unique_stops.size() << " unique stops, "s << static_cast<double>(info.road_length) << " route length"s << std::endl;
            }
            else {
                cout << "Bus "s << info.name << ": "s << "not found"s << endl;
            }
        }
        void AddingStop() {
            TransportCatalogue one;
            auto first = std::make_tuple("Tolstopaltsevo"s, 55.611087, 37.208290, "3900m to Marushkino"s);
            auto second = std::make_tuple("Marushkino"s, 55.595884, 37.209755, "");
            auto third = std::make_tuple("Rasskazovka"s, 55.632761, 37.333324, "1000m to Tolstopaltsevo"s);

            one.AddStop(first);
            auto iter1 = one.FindStop("Tolstopaltsevo"s);
            assert(iter1->latitude == 55.611087);
            assert(iter1->longtitude == 37.208290);
            assert(one.route_lengths[std::make_pair(one.stops["Tolstopaltsevo"], one.stops["Marushkino"])] == 3900);

            one.AddStop(second);
            auto iter2 = one.FindStop("Marushkino"s);
            assert(iter2->latitude == 55.595884);
            assert(iter2->longtitude == 37.209755);

            one.AddStop(third);
            auto iter3 = one.FindStop("Rasskazovka"s);
            assert(iter3->latitude == 55.632761);
            assert(iter3->longtitude == 37.333324);
            assert(one.route_lengths[std::make_pair(one.stops["Rasskazovka"], one.stops["Tolstopaltsevo"])] == 3900);

            size_t value = one.GetStopCount();
            assert(value == 3);
            assert(one.route_lengths.size() == 3);
        }
        void AddingBus() {
            TransportCatalogue one;
            auto first = std::make_tuple("Tolstopaltsevo"s, 55.611087, 37.208290, "");
            one.AddStop(first);

            std::string name = "345"s;
            std::vector<std::string> road = { "Tolstopaltsevo"s, "Marushkino"s, "Rasskazovka" };
            std::pair<std::string, std::vector<std::string>> our_bus = std::make_pair(name, road);
            one.AddBus(our_bus);
            auto iter = one.FindBus("345");
            size_t stops_in_road = iter->road.size();
            assert(stops_in_road == 3);
            size_t stops_in_catalogue = one.GetStopCount();
            assert(stops_in_catalogue == 3);
            size_t bus_count = one.GetBusCount();
            assert(bus_count == 1);
            for (auto stop_ : iter->road) {
                cout << stop_ << ": "s << one.stops.at(stop_)->latitude << ", "s << one.stops.at(stop_)->longtitude << " > "s;
            }
            cout << endl;
        }*/
        /*
            void BusInfo() {
                TransportCatalogue one;
                auto first = std::make_tuple("Tolstopaltsevo"s, 55.611087, 37.208290, "");
                auto second = std::make_tuple("Marushkino"s, 55.595884, 37.209755, "");

                one.AddStop(first);
                one.AddStop(second);


                std::string name = "750"s;
                std::vector<std::string> road = { "Tolstopaltsevo"s, "Marushkino"s, "Rasskazovka"s, "Marushkino"s, "Tolstopaltsevo"s };
                std::pair<std::string, std::vector<std::string>> our_bus = pair(name, road);
                one.AddBus(our_bus);
                auto third = std::make_tuple("Rasskazovka"s, 55.632761, 37.333324, "");
                one.AddStop(third);
                std::string for_search = "750"s;
                auto result = one.FindBusInfo(for_search);
                PrintInfo(result);

                for_search = "751"s;
                result = one.FindBusInfo(for_search);
                PrintInfo(result);
            }*/


            /*void ParsingStop() {
                std::string text = "Stop Universam: 55.587655, 37.645687"s;
                auto result = ParseStop(text);
                assert(std::get<0>(result) == "Universam"s);
                double lat = 55.587655;
                double lng = 37.645687;
                cout << lat << " and " << lng << " is same as " << std::get<1>(result) << " and " << std::get<2>(result) << std::endl;
            }

            void ParsingBusNormal() {
                std::cout << "Testing to parse normal road..."s << std::endl;
                std::string text = "Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye"s;
                auto result = ParseBus(text);
                assert(result.second.size() == 6);
                cout << result.first << endl;
                bool IsFirst = true;
                for (auto word : result.second) {
                    if (IsFirst) {
                        cout << word;
                        IsFirst = false;
                    }
                    else {
                        cout << ", " << word;
                    }
                }
                cout << endl << endl;
            }

            void ParsingBusLap() {
                std::cout << "Testing to parse lap road..."s << endl;
                std::string text = "Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka"s;
                auto result = ParseBus(text);
                assert(result.second.size() == 5);
                cout << result.first << endl;
                bool IsFirst = true;
                for (auto word : result.second) {
                    if (IsFirst) {
                        cout << word;
                        IsFirst = false;
                    }
                    else {
                        cout << ", " << word;
                    }
                }
                cout << endl << endl;
            }
            void IsBus() {
                std::string text = "Stop Universam: 55.587655, 37.645687"s;
                assert(IsAddBus(text) == false);
            }
            void ReadLineNum() {
                auto result = ReadLineWithNumberAdd();
                for (size_t i = 0; i < result.size(); ++i) {
                    std::cout << i + 1 << ". " << result[i] << std::endl;
                }
            }
            void SortInputLines() {
                std::vector<std::string> input_lines = { "Stop Tolstopaltsevo : 55.611087, 37.208290, 3900m to Marushkino"s,
        "Stop Marushkino : 55.595884, 37.209755"s,
        "Bus 256 : Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye"s,
        "Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka"s,
        "Stop Rasskazovka : 55.632761, 37.333324"s,
        "Stop Biryulyovo Zapadnoye : 55.574371, 37.651700"s };
                auto result = std::move(SortInput(input_lines));
                for (const auto& line : result) {
                    std::cout << line << std::endl;
                }
            }

            void FinalAddSearch() {
                TransportCatalogue tc;
                std::vector<std::string> lines = { "Stop Tolstopaltsevo: 55.611087, 37.208290"s, "Stop Marushkino: 55.595884, 37.209755"s, "Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka"s, "Stop Rasskazovka: 55.632761, 37.333324"};
                lines = std::move(SortInput(lines));
                for (auto& line : lines) {
                    if (IsAddBus(line)) {
                        auto one_bus = std::move(ParseBus(line));
                        tc.AddBus(std::move(one_bus));
                    }
                    else {
                        auto one_stop = std::move(ParseStop(line));
                        tc.AddStop(std::move(one_stop));
                    }
                }
                auto res = std::move(tc.FindBusInfo(std::move(ParseSearchLine("Bus 750"s))));
                if (res.road.size() > 0) {
                    std::cout << std::string("Bus "s) << res.name << std::string(": "s) << res.road.size() << std::string(" stops on route, "s) << res.unique_stops.size() << std::string(" unique stops, "s) << std::setprecision(6) << static_cast<double>(res.road_length) << std::string(" route length"s) << std::endl;
                }
                else {
                    cout << std::string("Bus "s) << res.name << ": "s << std::string("not found"s) << endl;
                }

            }*/
    }

}

int main() {
    project::TransportCatalogue catalogue;
    project::AddInfo(catalogue);//передать данные для добавления
    project::SearchInfo(catalogue);//передать запросы
    return 0;
}