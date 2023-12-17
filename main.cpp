#include "json_builder.h"
#include <iostream>
#include<fstream>
#include<string>
#include "json.h"
#include "json_reader.h"

using namespace std;

int main() {
	project::TransportCatalogue tc;
	JSONReader reader(tc);
	auto res = std::move(reader.MakeRequests(std::cin));
	json::Print(res, std::cout);
	return 0;
}