#pragma once
#include "graph.h"
#include "router.h"
#include "transport_catalogue.h"
#include <memory>
#include <optional>
#include <string_view>
#include <unordered_map>


enum class ActionType
{
	ZERO,
	WAIT,
	BUS,
	OUT
};

struct RouterData {
	size_t wait_time = 0;
	size_t velocity = 0;
	const project::TransportCatalogue& catalogue;
};

struct RouteItem {
	ActionType type = ActionType::ZERO;
	std::string name;
	size_t span_count = 0;
	double total_time = 0;
};
struct OptimalRoute {
	double total_time = 0;
	std::vector<RouteItem> way;
};
using  MyGraph = graph::DirectedWeightedGraph<double>;
class TransportRouter {
public:

	TransportRouter(const RouterData data);
	std::optional<OptimalRoute> GetOptimalRoute(std::string_view from, std::string_view to);

private:
	size_t wait_time_ = 0;
	size_t velocity_ = 0;
	MyGraph graph_;
	std::unique_ptr<graph::Router<double>> graph_router;
	std::unordered_map<std::string_view, size_t> stops_by_name;
	std::unordered_map<size_t, std::string_view> stops_by_index;

	void BuildGraph(const project::TransportCatalogue& tc);
	void AddVertexes(std::unordered_map < std::string_view, project::Stop*> stops);
	void ParseToEdges(const project::TransportCatalogue& tc, std::vector<std::string_view> road, std::string name);
	void AddEdges(const project::TransportCatalogue& tc);
};