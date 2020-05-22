#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>

#include <boost/extended/serialization/singleton.hpp>

#include "../../shared/source/logger/logger.hpp"

using Logger = solution::shared::Logger;

template < typename T >
using singleton = boost::extended::serialization::singleton < T > ;

// using System = singleton < solution::system::System > ;

void f()
{
	RUN_LOGGER(logger);

	try
	{
		// ...

		LOGGER_WRITE(logger, "message");

		// ...
	}
	catch (std::exception & exception)
	{
		solution::shared::catch_handler < std::logic_error > (logger, exception); // change std::logic_error to custom exception type
	}
}

int main(int argc, char * argv[])
{
	RUN_LOGGER(logger);

	try
	{
		f();

		// System system;

		// system.instance().run();

		// std::this_thread::sleep_for(std::chrono::minutes(60 * 8 + 10 + 40 + 10));

		return EXIT_SUCCESS;
	}
	catch (const std::exception & exception)
	{
		logger.write(Logger::Severity::fatal, exception.what());

		return EXIT_FAILURE;
	}
	catch (...)
	{
		logger.write(Logger::Severity::fatal, "unknown exception");

		return EXIT_FAILURE;
	}
}
