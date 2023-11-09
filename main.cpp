#include<fstream>
#include<iostream>
#include<string>
#include "json.h"
#include "json_reader.h"


int main() {
    project::TransportCatalogue tc;
    auto res = std::move(MakeRequests(std::cin, tc));
    json::Document doc(json::Node(json::Array(res.begin(), res.end())));
    json::Print(doc, std::cout);
    return 0;
}