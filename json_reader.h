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
#include "json_builder.h"
/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */
class JSONReader {
public:
	explicit JSONReader(project::TransportCatalogue& tc_)
		: tc(tc_) {
	}
	StopBefore ParseStop(const std::map<std::string, json::Node>& stop);
	BusBefore ParseBus(const std::map<std::string, json::Node>& bus);
	void MakeBaseRequests(const std::vector<json::Node>& base);
	json::Document MakeStatRequests(std::vector<json::Node>& stat, RequestHandler& rh);
	renderer::MapRenderer MakeRenderSettings(const std::map<std::string, json::Node>& settings);
	json::Document MakeRequests(std::istream& input);
private:
	project::TransportCatalogue& tc;
};

