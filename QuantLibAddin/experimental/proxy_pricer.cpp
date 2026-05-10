#include <pybind11/embed.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>

namespace py = pybind11;

int main() {

	py::scoped_interpreter guard{};  // start the Python interpreter

	try 
	{
		py::module_ sys = py::module_::import("sys");

		const char* site_packages = std::getenv("PYTHON_SITE_PACKAGES");

		if (!site_packages) {
			throw std::runtime_error("PYTHON_SITE_PACKAGES environment variable is not set");
		}

		sys.attr("path").attr("append")(site_packages);

		std::ifstream json_request_file("pricers/mid_curve_price_request.json");

		std::stringstream json_request;
		json_request << json_request_file.rdbuf();

		auto midcurve_price_request = json_request.str();
		std::cout << "JSON Request : " << midcurve_price_request << std::endl;

		py::module_ mid_curve_pricer = py::module_::import("pricers.mid_curve_pricer");
		py::object PriceMidCurves = mid_curve_pricer.attr("PriceMidCurves");
		py::object mid_curves = PriceMidCurves(midcurve_price_request);

		std::cout << "MidCurves : " << mid_curves.cast<std::string>() << std::endl;

	}
	catch (const std::exception &e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}

	return 0;
}
