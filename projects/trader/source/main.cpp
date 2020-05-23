#include <exception>
#include <stdexcept>
#include <string>

#include <boost/extended/serialization/singleton.hpp>

#include "trader/trader.hpp"

#include "../../shared/source/logger/logger.hpp"

using Logger = solution::shared::Logger;

template < typename T >
using singleton = boost::extended::serialization::singleton < T > ;

using Trader = singleton < solution::trader::Trader > ;

int main(int argc, char * argv[])
{
	RUN_LOGGER(logger);

	try
	{
		Trader trader;

		trader.instance().run();

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