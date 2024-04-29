/*#include "transport_router.h"
#include <cstdlib>

TransportRouter::TransportRouter(size_t wait_time, size_t velocity, GRAPH& graph)
	: wait_time_(wait_time), velocity_(velocity), graph_(graph), graph_router(nullptr), stops_by_name({}), stops_by_index({}) {
}

void TransportRouter::AddVertexes(std::unordered_map < std::string_view, project::Stop*> stops) {
	int index = 0;
	for (const auto& [name, ptr] : stops) {
		stops_by_name.emplace(name, index);
		stops_by_index.emplace(index, name);
		++index;
	}
}

GRAPH TransportRouter::AddEdges(const project::TransportCatalogue& tc) {
	GRAPH current_graph(tc.GetAllStops().size() * 2);
	for (const auto& bus : tc.GetAllBuses()) {
		//проверка на кольцевой
		//если не кольцевой, то делим на 2 и вычисл€ем обратный путь
		//если кольцевой, то идем по нему пр€мо
		//нужны сеты пар??
		std::vector<std::string_view> road(bus.road.begin(), bus.road.end());
		if (!bus.IsRoundtrip) {
			road.erase(road.begin() + (road.size() / 2 + 1), road.end());
		}
		std::vector<double> dist_sums_forw(road.size());
		std::vector<double> dist_sums_rev(road.size());
		int dist_sum = 0;
		int dist_sum_reverse = 0;
		for (size_t index = 1; index < road.size(); index++) {
			dist_sum += tc.GetDistance(tc.FindStop(road.at(index - 1)), tc.FindStop(road.at(index)));
			dist_sums_forw[index] = dist_sum;
			dist_sum_reverse += tc.GetDistance(tc.FindStop(road.at(index)), tc.FindStop(road.at(index - 1)));
			dist_sums_rev[index] = dist_sum_reverse;
		}
		for (int i = 0; i + 1 < road.size(); ++i) {
			for (int j = i + 1; j < road.size(); ++j) {
				graph::Edge<double> edge;
				edge.from = stops_by_name.at(std::string(road.at(i)));
			    edge.to = stops_by_name.at(std::string(road.at(j)));
			    edge.name = bus.name;
				edge.span_count = std::abs(i - j);
				edge.weight = static_cast<double>(wait_time_) + 
					(static_cast<double>(i < j ? dist_sums_forw[j] - dist_sums_forw[i] : 
						dist_sums_rev[i] - dist_sums_rev[j]) 
						/ (static_cast<double>(velocity_) * 100.0 / 6.0));
				current_graph.AddEdge(edge);
				if (!bus.IsRoundtrip) {
					//если не кольцевой, рассчитать и добавить в обратном пор€дке
					edge.from = stops_by_name.at(std::string(road.at(j)));
					edge.to = stops_by_name.at(std::string(road.at(i)));
					edge.weight = static_cast<double>(wait_time_) + 
						(static_cast<double>(j < i ? dist_sums_forw[i] - dist_sums_forw[j] : 
							dist_sums_rev[j] - dist_sums_rev[i]) 
							/ (static_cast<double>(velocity_) * 100.0 / 6.0));
					current_graph.AddEdge(edge);
				}
			}
		}
	}
	return current_graph;
}

void TransportRouter::BuildGraph(const project::TransportCatalogue& tc) {
	AddVertexes(tc.GetAllStops());
	graph_ = AddEdges(tc);
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
				optimal_route.way.push_back({ ActionType::BUS, edge.name, edge.span_count, edge.weight });
				optimal_route.total_time += (static_cast<double>(wait_time_) + edge.weight);
			}
		}//» —Ћќ∆»“№ ¬—® ¬ —“–” “”–”-–≈«”Ћ№“ј“ ƒЋя ¬џ¬ќƒј
		return optimal_route;
	}
	return std::nullopt;
}
*/