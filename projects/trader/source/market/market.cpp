#include "market.hpp"

namespace solution
{
	namespace trader
	{
		void Market::initialize()
		{
			RUN_LOGGER(logger);

			try
			{
				std::filesystem::create_directory(directory);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		std::filesystem::path Market::get(
			const std::string & asset, const std::string & scale, time_point_t first, time_point_t last) const
		{
			RUN_LOGGER(logger);

			try
			{
				const std::filesystem::path file = asset + "_" + scale + Extension::txt;
				const std::filesystem::path path = directory / file;

				shared::Python python;

				try
				{
					boost::python::exec("from market import get", 
						python.global(), python.global());

					static auto last_execution = clock_t::now();

					std::this_thread::sleep_until(last_execution + std::chrono::milliseconds(1000));

					python.global()["get"]
						(asset.c_str(), scale.c_str(), make_date(first).c_str(), make_date(last).c_str(), path.string().c_str());
				
					last_execution = clock_t::now();
				}
				catch (const boost::python::error_already_set &)
				{
					logger.write(Severity::error, python.exception());
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < market_exception > (logger, exception);
				}

				return path;
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}
		
		double Market::get_current_price(const std::string & asset) const
		{
			RUN_LOGGER(logger);

			try
			{
				using shared_memory_t = boost::interprocess::managed_shared_memory;

				using price_t = double;

				using mutex_t = boost::interprocess::interprocess_mutex;

				const std::string default_scale = "H1";

				const auto shared_memory_name = "QUIK_" + asset + "_" + default_scale;

				shared_memory_t shared_memory(boost::interprocess::open_only, shared_memory_name.c_str());

				auto price = shared_memory.find < price_t > (boost::interprocess::unique_instance).first;
				auto mutex = shared_memory.find < mutex_t > (boost::interprocess::unique_instance).first;
			
				boost::interprocess::scoped_lock < mutex_t > lock(*mutex);
				
				return *price;
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}
		
		Market::date_t Market::make_date(time_point_t time_point) const
		{
			RUN_LOGGER(logger);

			try
			{
				auto time = clock_t::to_time_t(time_point);

				std::stringstream sout;

				sout << std::put_time(std::localtime(&time), "%y%m%d");

				return sout.str();
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

	} // namespace trader

} // namespace solution