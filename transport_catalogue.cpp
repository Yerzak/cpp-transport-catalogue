#include "transport_catalogue.h"
#include "geo.h"
#include <algorithm>
#include<exception>
#include<iomanip>
#include<utility>

namespace project {
    void TransportCatalogue::AddBus(BusBefore& bus_data) {
        Bus bus;//создали автобус
        bus.name = std::move(bus_data.name);///присвоили имя автобусу
        bus.road.reserve(bus_data.road.size());//зарезервировали место для быстрой вставки
        bus.IsRoundtrip = bus_data.IsRoundtrip;
        for (auto& stop_data : bus_data.road) {//идем по маршруту
            if (stops.find(stop_data) == stops.end()) {//если остановка еще не появлялась
                Stop stop;//создали объект структуры
                stop.name = std::move(stop_data);//присвоили ему имя
                list_of_stops.emplace_back(std::move(stop));//вставили его в дек (ссылки и указатели не инвалидируются)
                bus.road.push_back(list_of_stops.back().name);//взяли имя помещенного элемента и вставили это имя в вектор остановок структуры Bus
                stops[list_of_stops.back().name] = &list_of_stops.back();//вставили имя последнего элемента и указатель на этот элемент в хеш-таблицу
            }
            else {//если остановка уже появлялась
                bus.road.push_back(stops.at(stop_data)->name);
            }
        }
        if (!bus_data.IsRoundtrip) {//если маршрут кольцевой, добавляем в его конец все остановки в обратном порядке, кроме последней
            std::vector<std::string_view> road_copy(bus.road.begin(), bus.road.end());
            road_copy.pop_back();
            std::reverse(road_copy.begin(), road_copy.end());
            std::for_each(road_copy.begin(), road_copy.end(), [&bus](auto& elem) {
                bus.road.push_back(elem);
                });
        }
        if (bus.road.size() < 2) {
            return;
        }
        list_of_buses.emplace_back(std::move(bus));//вставили автобус в дек (ссылки и указатели не инвалидируются)
        buses.insert({ list_of_buses.back().name, &list_of_buses.back() });//вставили имя помещенного элемента и указатель на этот элемент в хеш-таблицу
        Bus* current_bus = &list_of_buses.back();
        std::vector<std::string_view> list = current_bus->road;
        for (const auto& current_stop : list) {
            stop_with_buses[current_stop].insert(current_bus);
        }
    }

    void TransportCatalogue::AddStop(StopBefore& stop_data) {
        if (stop_data.lat > 90.0 || stop_data.lat < -90.0 || stop_data.lng > 180.0 || stop_data.lng < -180.0) {
            return;
        }
        auto iter = stops.find(stop_data.name);
        if (iter != stops.end()) {
            iter->second->latitude = stop_data.lat;
            iter->second->longtitude = stop_data.lng;
        }
        else {
            Stop stop;
            stop.name = stop_data.name;
            stop.latitude = stop_data.lat;
            stop.longtitude = stop_data.lng;
            list_of_stops.emplace_back(std::move(stop));
            stops.insert({ list_of_stops.back().name, &list_of_stops.back() });
        }
        for (auto& [point, len] : stop_data.road_lengths) {//вносим расстояния до соседних остановок
            if (stops.count(point)) {//если пункт назначения уже забит в список
                SetDistance(stops.at(stop_data.name), stops.at(point), len);
            }
            else {//если пункт назначения еще не добавлялся
                Stop point_stop;
                point_stop.name = point;
                list_of_stops.emplace_back(std::move(point_stop));
                stops.insert({ list_of_stops.back().name, &list_of_stops.back() });
                SetDistance(stops.at(stop_data.name), stops.at(list_of_stops.back().name), len);
            }
        }
        std::unordered_set<Bus*> empty_set;
        stop_with_buses.emplace(stops.at(stop_data.name)->name, empty_set);
    }

    void TransportCatalogue::SetDistance(Stop* from, Stop* to, size_t distance) {
        route_lengths[std::make_pair(from, to)] = distance;
    }

    BusStat TransportCatalogue::FindBusInfo(const std::string& name) const {
        BusStat info;//создали объект
        info.name = std::move(name);//присвоили ему имя
        auto iter = buses.find(info.name);//ищем имя в мапе автобусов
        if (iter != buses.end()) {//если есть такое имя
            info.road = iter->second->road;//копируем вектор остановок
            std::unordered_set<std::string_view> unique_stops(iter->second->road.begin(), iter->second->road.end());//создаем мапу уникальных остановок
            info.unique_stops = unique_stops;//передаем мапу в поле структуры
            size_t road_length = 0;//вводим переменную длины маршрут
            for (size_t i = 0; i < info.road.size() - 1; i++) {
                road_length += GetDistance(stops.at(info.road[i]), stops.at(info.road[i + 1]));
            }
            info.road_length = road_length;//присваиваем значение длины в поле структуры
            info.curvature = GetCurvature(info.road_length, info.road);//вычисляем искривленность маршрута
            info.IsExist = true;
        }
        else {
            info.IsExist = false;
        }
        return info;//возвращаем структуру
    }

    size_t TransportCatalogue::GetDistance(Stop* from, Stop* to) const {
        auto iter = route_lengths.find(std::make_pair(from, to));
        if (iter != route_lengths.end()) {
            return iter->second;
        }
        else {
            iter = route_lengths.find(std::make_pair(to, from));
            if (iter != route_lengths.end()) {
                return iter->second;
            }
        }
        return 0;
    }

    double TransportCatalogue::GetCurvature(size_t fact_distance, const std::vector<std::string_view>& road) const {
        double road_length = 0.0;
        for (size_t i = 0; i < road.size() - 1; i++) {
            geo::Coordinates from = { stops.at(road.at(i))->latitude, stops.at(road.at(i))->longtitude };
            geo::Coordinates to = { stops.at(road.at(i + 1))->latitude, stops.at(road.at(i + 1))->longtitude };
            double len = geo::ComputeDistance(from, to);
            road_length += len;
        }
        return (static_cast<double>(fact_distance) / road_length);
    }

    Bus* TransportCatalogue::FindBus(std::string_view bus) const {
        auto iter = buses.find(bus);
        return iter == buses.end() ? nullptr : iter->second;
    }

    Stop* TransportCatalogue::FindStop(std::string_view stop) const {
        auto iter = stops.find(stop);
        return iter == stops.end() ? nullptr : iter->second;
    }

    std::pair<std::string, std::unordered_set<Bus*>> TransportCatalogue::FindStopInfo(const std::string& name) const {
        std::unordered_set<Bus*> result;
        auto iter = stop_with_buses.find(name);
        return iter == stop_with_buses.end() ? std::make_pair(name, result) : std::make_pair(name, iter->second);
    }

    size_t TransportCatalogue::GetBusCount() {
        return list_of_buses.size();
    }
    size_t TransportCatalogue::GetStopCount() const {
        return list_of_stops.size();
    }

    const std::unordered_set<Bus*>* TransportCatalogue::GetBusesForStop(std::string_view stop_name) const {
        /*если стапс содержит остановку, но стапвизбасис не содержит остановку, нужно создать в стапвизбасиз эту остановку без автобусов
        если оба хранилища содержат остановку, нужно вернуть ее автобусы
        если стапс не содержит остановку, нужно вернуть налл*/
        if (stop_with_buses.count(stop_name)) {
            return &(stop_with_buses.at(stop_name));
        }
        return nullptr;
    }

    std::deque<Bus> TransportCatalogue::GetAllBuses() const {
        return list_of_buses;
    }

    std::unordered_map<std::string_view, Stop*> TransportCatalogue::GetAllStops() const {
        return stops;
    }

}//namespace project