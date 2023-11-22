#pragma once
#include <string>
#include <vector>
#include <map>
#include "domain.h"
# include "json.h"
# include "transport_catalogue.h"
#include "request_handler.h"
#include "map_renderer.h"
#include "svg.h"
/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */
class JSONReader {
public:
	explicit JSONReader(project::TransportCatalogue& tc_)
		: tc(tc_) {
	}
	StopBefore ParseStop(std::map<std::string, json::Node> stop);
	BusBefore ParseBus(std::map<std::string, json::Node> bus);
	void MakeBaseRequests(std::vector<json::Node>& base);
	std::vector<std::map<std::string, json::Node>> MakeStatRequests(std::vector<json::Node>& stat, RequestHandler& rh);
	renderer::MapRenderer MakeRenderSettings(std::map<std::string, json::Node> settings);
	std::vector<std::map<std::string, json::Node>> MakeRequests(std::istream& input);
private:
	project::TransportCatalogue& tc;
};

