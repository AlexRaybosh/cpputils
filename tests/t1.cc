#include <iostream>
#include <jsonutils.h>
#include <exception>

int main() {

	auto j=json::parse("{\"a\" : {\"z\" : [1.0,2,3], \"a\" : 33.333}}");

	std::cout<<"Parsed: "<<j<<std::endl;
	std::cout<<"Pretty: "<<json::pretty(j)<<std::endl;

	bool exPassed=false;
	try {
		json::parse("{garbage");
	} catch (...) {
		exPassed=true;
		std::exception_ptr ex = std::current_exception();
		try {std::rethrow_exception(ex);} catch (const std::exception& e) {
			std::cout<<"Correct ex: "<<e.what()<<std::endl;
		}
	}
	if (!exPassed) {
		std::cout<<"Expected exception above "<<__FILE__<<":"<<__LINE__<<std::endl;
		return 1;
	}
	std::cout<<"zzz: "<<json::parse("\"zzz\"")<<std::endl;

	return 0;
}
