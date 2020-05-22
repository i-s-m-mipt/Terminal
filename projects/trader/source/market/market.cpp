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

		Market::path_t Market::get(const std::string & asset, const std::string & scale, time_point_t first, time_point_t last) const
		{
			RUN_LOGGER(logger);

			try
			{
				const path_t file = asset + "_" + scale + Extension::txt;
				const path_t path = directory / file;

				shared::Python python;

				try
				{
					boost::python::exec("from market import get", 
						python.global(), python.global());

					python.global()["get"]
						(asset.c_str(), scale.c_str(), make_date(first).c_str(), make_date(last).c_str(), path.string().c_str());
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
		/*
		Market::path_t Market::get(const std::string & asset, const std::string & scale, std::size_t quantity) const
		{
			RUN_LOGGER(logger);

			try
			{
				using shared_memory_t = boost::interprocess::managed_shared_memory;

				using record_t = 
					boost::interprocess::basic_string < char, std::char_traits < char >,
					    boost::interprocess::allocator < char, shared_memory_t::segment_manager > > ;
				
				using record_allocator_t =
				    boost::interprocess::allocator < record_t, shared_memory_t::segment_manager > ;

				using deque_t = boost::interprocess::deque < record_t, record_allocator_t > ;

				using mutex_t = boost::interprocess::interprocess_mutex;

				const auto shared_memory_name = "QUIK_" + asset + "_" + scale;

				shared_memory_t shared_memory(boost::interprocess::open_only, shared_memory_name.c_str());

				auto deque = shared_memory.find < deque_t > (boost::interprocess::unique_instance).first;
				auto mutex = shared_memory.find < mutex_t > (boost::interprocess::unique_instance).first;
				
				auto path = directory; path /= asset + "_" + scale + Extension::txt;

				{
					boost::interprocess::scoped_lock < mutex_t > lock(*mutex);

					std::fstream fout(path.string(), std::ios::out);

					if (!fout)
					{
						throw market_exception("cannot open file " + path.string());
					}

					fout << "date,time,open,high,low,close,v" << std::endl;

					for (const auto & record : *deque)
					{
						fout << record << std::endl;
					}
				}

				return path;
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}
		*/
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