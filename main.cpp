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
	std::string input_way = "test 2.txt";
	std::string output_way = "result.txt";
	std::ifstream fin;
	fin.open(input_way);
	if (!fin.is_open()) {
		std::cout << "Opening error from input" << std::endl;
		return 1;
	}
	auto res = std::move(reader.MakeRequests(fin));
	std::ofstream fout;
	fout.open(output_way);
	if (!fout.is_open()) {
		std::cout << "Opening error from output" << std::endl;
		return 2;
	}
	json::Print(res, fout);
	
	return 0;
}