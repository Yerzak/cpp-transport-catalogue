#include "transport_catalogue.h"
#include "geo.h"
#include<utility>
#include<exception>
#include<iomanip>
namespace project {
    void TransportCatalogue::AddBus(BusBefore& bus_data) {
        Bus bus;//создали автобус
        bus.name = std::move(bus_data.name);//присвоили имя автобусу
        //std::vector<std::string> road = std::move(bus_data.road);//распаковали маршрут из пары
        bus.road.reserve(bus_data.road.size());//зарезервировали место для быстрой вставки
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
        //auto& [name, lat, lng, length_info] = stop_data;
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
        size_t begin = stop_data.stop_info.find_first_not_of(' ');
        size_t len_end = stop_data.stop_info.find('m');
        while (begin != stop_data.stop_info.npos || len_end != stop_data.stop_info.npos) {
            size_t len = atoi(stop_data.stop_info.substr(begin, len_end - begin).c_str());
            size_t to_stop_begin = len_end + 5;
            stop_data.stop_info = std::move(stop_data.stop_info.substr(to_stop_begin, stop_data.stop_info.size()));
            size_t point3 = stop_data.stop_info.find(',');
            std::string to_stop = std::move(stop_data.stop_info.substr(0, std::min(stop_data.stop_info.size(), point3)));
            if (stops.count(to_stop)) {//если пункт назначения уже забит в список
                SetDistance(stops.at(stop_data.name), stops.at(to_stop), len);
            }
            else {//если пункт назначения еще не добавлялся
                Stop point_stop;
                point_stop.name = to_stop;
                list_of_stops.emplace_back(std::move(point_stop));
                stops.insert({ list_of_stops.back().name, &list_of_stops.back() });
                SetDistance(stops.at(stop_data.name), stops.at(list_of_stops.back().name), len);
            }
            stop_data.stop_info = stop_data.stop_info.substr(std::min(stop_data.stop_info.find_last_not_of(' '), point3) + 1, stop_data.stop_info.size());
            begin = stop_data.stop_info.find_first_not_of(' ');
            len_end = stop_data.stop_info.find('m');
        }
    }

    BusInfo TransportCatalogue::FindBusInfo(const std::string& name) {
        BusInfo info;//создали объект
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
        }
        return info;//возвращаем структуру
    }
    void TransportCatalogue::SetDistance(Stop* from, Stop* to, size_t distance) {
        route_lengths[std::make_pair(from, to)] = distance;
    }

    size_t TransportCatalogue::GetDistance(Stop* from, Stop* to) {
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

    double TransportCatalogue::GetCurvature(size_t fact_distance, const std::vector<std::string_view>& road) {
        double road_length = 0.0;
        for (size_t i = 0; i < road.size() - 1; i++) {
            geo::Coordinates from = { stops[road[i]]->latitude, stops[road[i]]->longtitude };
            geo::Coordinates to = { stops[road[i + 1]]->latitude, stops[road[i + 1]]->longtitude };
            double len = geo::ComputeDistance(from, to);
            road_length += len;
        }
        return (static_cast<double>(fact_distance) / road_length);
    }

    Bus* TransportCatalogue::FindBus(std::string_view bus) {
        auto iter = buses.find(bus);
        return iter == buses.end() ? nullptr : iter->second;
    }

    Stop* TransportCatalogue::FindStop(std::string_view stop) {
        auto iter = stops.find(stop);
        return iter == stops.end() ? nullptr : iter->second;
    }

    std::pair<std::string, std::unordered_set<Bus*>> TransportCatalogue::FindStopInfo(const std::string& name) {
        std::unordered_set<Bus*> result;
        auto iter = stop_with_buses.find(name);
        return iter == stop_with_buses.end() ? std::make_pair(name, result) : std::make_pair(name, iter->second);
    }

    size_t TransportCatalogue::GetBusCount() {
        return list_of_buses.size();
    }
    size_t TransportCatalogue::GetStopCount() {
        return list_of_stops.size();
    }

}
