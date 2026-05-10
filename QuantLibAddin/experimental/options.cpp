#include <pybind11/embed.h>
#include <iostream>
#include <iomanip>

namespace py = pybind11;

int main() {

	py::scoped_interpreter guard{};  // start the Python interpreter

	try {
        
		py::module_ sys = py::module_::import("sys");

		// site package path with QuantLib
		const char* site_packages = std::getenv("PYTHON_SITE_PACKAGES");

		if (!site_packages) {
			throw std::runtime_error("PYTHON_SITE_PACKAGES environment variable is not set");
		}

		sys.attr("path").attr("append")(site_packages);

		// Import QuantLib
		py::module_ ql = py::module_::import("QuantLib");

		// Import QLO Scripts
		sys.attr("path").attr("append")("qlo");
		
		py::print("sys.path =", sys.attr("path"));


		// Import QLO like modules, files from qlo/*
		py::module_ qlo_date = py::module_::import("qlo.date");
		py::module_ qlo_quotes = py::module_::import("qlo.quotes");
		py::module_ qlo_volatilities = py::module_::import("qlo.volatilities");
		py::module_ qlo_yieldtermstructures = py::module_::import("qlo.yieldtermstructures");
		py::module_ qlo_processes = py::module_::import("qlo.processes");

		// QLO functions
		py::object QLO_DateHandle = qlo_date.attr("Date");
		py::object QLO_QuoteHandle = qlo_quotes.attr("QuoteHandle");
		py::object QLO_BlackConstantVolHandle = qlo_volatilities.attr("BlackVolTermStructureHandle");
		py::object QLO_YieldTermStructureHandle = qlo_yieldtermstructures.attr("YieldTermStructureHandle");
		py::object QLO_BlackScholesMertonProcess = qlo_processes.attr("BlackScholesMertonProcess");

		// Pricing
		py::object today = ql.attr("Date")(43950);
		ql.attr("Settings").attr("instance")().attr("evaluationDate") = today;

		py::object valuation_date = QLO_DateHandle(43952);
		py::object maturity_date = QLO_DateHandle(43962);
		py::object volatility = QLO_QuoteHandle(0.02);
		py::object spot = QLO_QuoteHandle(101.5);

		py::object risk_free_rate = QLO_QuoteHandle(0.025);
		py::object dividend_yield = QLO_QuoteHandle(0.01);

		py::object dc = ql.attr("Actual365Fixed")();

		py::object risk_free_rate_ts = QLO_YieldTermStructureHandle(valuation_date, risk_free_rate, dc);
		py::object dividend_yield_ts = QLO_YieldTermStructureHandle(valuation_date, dividend_yield, dc);
		py::object vol_ts = QLO_BlackConstantVolHandle(valuation_date, volatility);

		py::object process = QLO_BlackScholesMertonProcess(spot, dividend_yield_ts, risk_free_rate_ts, vol_ts);

		py::object optionType = ql.attr("Option").attr("Call");
		int strike = 100;

		py::object payoff = ql.attr("PlainVanillaPayoff")( optionType, strike );
		py::object exercise = ql.attr("EuropeanExercise")( maturity_date );
		py::object option = ql.attr("VanillaOption")( payoff, exercise );

		py::object engine = ql.attr("AnalyticEuropeanEngine")( process );

		option.attr("setPricingEngine")(engine);

		std::cout << std::fixed << std::setprecision(4)
			<< "npv:   " << option.attr("NPV")().cast<double>()   << std::endl
			<< "delta: " << option.attr("delta")().cast<double>() << std::endl
			<< "gamma: " << option.attr("gamma")().cast<double>() << std::endl
			<< "vega:  " << option.attr("vega")().cast<double>()  << std::endl
			<< "theta: " << option.attr("theta")().cast<double>() << std::endl
			<< "rho:   " << option.attr("rho")().cast<double>()   << std::endl;

	}
	catch (const std::exception &e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}

	return 0;
}
