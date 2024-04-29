#include "transport_router.h"
#include <cstdlib>

TransportRouter::TransportRouter(const RouterData data)
	: wait_time_(data.wait_time), velocity_(data.velocity), graph_(data.catalogue.GetAllStops().size() * 2), graph_router(nullptr), stops_by_name({}), stops_by_index({}) {
	BuildGraph(data.catalogue);
}

void TransportRouter::AddVertexes(std::unordered_map < std::string_view, project::Stop*> stops) {
	int index = 0;
	for (const auto& [name, ptr] : stops) {
		stops_by_name.emplace(name, index);
		stops_by_index.emplace(index, name);
		++index;
	}
}

void TransportRouter::ParseToEdges(const project::TransportCatalogue& tc, std::vector<std::string_view> road, std::string name) {
	for (int i = 0; i + 1 < road.size(); ++i) {
		double current_weight = 0.0;
		graph::Edge<double> wait_edge;
		wait_edge.name = name;
		wait_edge.from = stops_by_name.at(road[i]);
		wait_edge.to = stops_by_name.at(road[i]);
		wait_edge.span_count = 0;
		wait_edge.weight = wait_time_;
		graph_.AddEdge(wait_edge);
		for (int j = i + 1; j < road.size(); ++j) {
			graph::Edge<double> edge;
			edge.name = name;
			edge.from = stops_by_name.at(road[i]);
			edge.to = stops_by_name.at(road[j]);
			double metres_per_km = 1000.0;
			double min_per_hour = 60.0;
			current_weight += static_cast<double>(tc.GetDistance(tc.FindStop(road[j - 1]), tc.FindStop(road[j]))) / (static_cast<double>(velocity_) * metres_per_km / min_per_hour);
			edge.weight = wait_time_ + current_weight;
			// � ���� ������ ����� �������� � ��������� Edge ���� span_count
			edge.span_count = std::abs(i - j);
			graph_.AddEdge(edge);
		}
	}
}

void TransportRouter::AddEdges(const project::TransportCatalogue& tc) {
	for (const auto& bus : tc.GetAllBuses()) {
		std::vector<std::string_view> road(bus.road.begin(), bus.road.end());
		if (!bus.IsRoundtrip) {
			road.erase(road.begin() + (road.size() / 2 + 1), road.end());
			ParseToEdges(tc, road, bus.name);
			std::reverse(road.begin(), road.end());
			ParseToEdges(tc, road, bus.name);
		}
		else {
			ParseToEdges(tc, road, bus.name);
		}
	}
}

void TransportRouter::BuildGraph(const project::TransportCatalogue& tc) {
	AddVertexes(tc.GetAllStops());
    AddEdges(tc);
	graph_router = std::make_unique<graph::Router<double>>(graph_);
}

std::optional<OptimalRoute> TransportRouter::GetOptimalRoute(std::string_view from, std::string_view to) {
	if (!stops_by_name.count(from) || !stops_by_name.count(to)) {
		return std::nullopt;
	}
	size_t start_index = stops_by_name.at(from);
	size_t finish_index = stops_by_name.at(to);
	auto optional_result = graph_router->BuildRoute(start_index, finish_index);
	OptimalRoute optimal_route;
	if (optional_result.has_value()) {
		auto result = optional_result.value();
		std::vector<graph::EdgeId> edges = result.edges;
		optimal_route.total_time = 0.0;
		for (auto id : edges) {
			auto edge = graph_.GetEdge(id);
			auto start = stops_by_index.at(edge.from);
			auto finish = stops_by_index.at(edge.to);
			if (start == finish) {
				continue;
			}
			else {
				optimal_route.way.push_back({ ActionType::WAIT, std::string(start), 0, static_cast<double>(wait_time_) });
				optimal_route.way.push_back({ ActionType::BUS, edge.name, edge.span_count, (edge.weight - static_cast<double>(wait_time_)) });
				optimal_route.total_time += edge.weight;
			}
		}//� ������� �Ѩ � ���������-��������� ��� ������
		return optimal_route;
	}
	return std::nullopt;
}
