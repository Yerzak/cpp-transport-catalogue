#pragma once
#include<algorithm>
#include<deque>
#include<iostream>
#include <string>
#include<string_view>
#include<tuple>
#include<unordered_set>
#include<unordered_map>
#include<vector>
#include "domain.h"
namespace project {

    namespace detail {
        struct RouteLengthHash {
            static const size_t koeff_first = 28;
            size_t operator() (const std::pair<Stop*, Stop*>& segment) const {
                size_t from_name = static_cast<size_t>(text_hasher(segment.first->name));
                size_t to_name = static_cast<size_t>(text_hasher(segment.second->name));
                return from_name + to_name * koeff_first;
            }
        private:
            std::hash<std::string> text_hasher;
        };
    }
    class TransportCatalogue {
    public:
        void AddBus(BusBefore& bus_data);
        void AddStop(StopBefore& stop_data);
        void SetDistance(Stop* from, Stop* to, size_t distance);
        BusStat FindBusInfo(const std::string& name) const;
        Bus* FindBus(std::string_view bus) const;
        size_t GetDistance(Stop* from, Stop* to) const;
        double GetCurvature(size_t fact_distance, const std::vector<std::string_view>& road) const;
        Stop* FindStop(std::string_view stop) const;
        std::pair<std::string, std::unordered_set<Bus*>> FindStopInfo(const std::string& name) const;
        size_t GetBusCount();
        size_t GetStopCount() const;
        const std::unordered_set<Bus*>* GetBusesForStop(std::string_view stop_name) const;
        std::deque<Bus> GetAllBuses() const;
        std::unordered_map<std::string_view, Stop*> GetAllStops() const;

    private:
        std::deque<Stop> list_of_stops;
        std::deque <Bus> list_of_buses;
        std::unordered_map<std::string_view, Stop*> stops;
        std::unordered_map <std::string_view, Bus*> buses;
        std::unordered_map<std::pair<Stop*, Stop*>, size_t, detail::RouteLengthHash> route_lengths;
        std::unordered_map<std::string_view, std::unordered_set<Bus*>> stop_with_buses;
    };
}//namespace project