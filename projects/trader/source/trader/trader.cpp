#include "trader.hpp"

namespace solution
{
	namespace trader
	{
		void Trader::Data::load_assets(assets_container_t & assets)
		{
			RUN_LOGGER(logger);

			try
			{
				std::fstream fin(File::assets.string(), std::ios::in);

				if (!fin)
				{
					throw trader_exception("cannot open file " + File::assets.string());
				}

				std::string asset;

				while (std::getline(fin, asset))
				{
					assets.push_back(asset);
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < trader_exception > (logger, exception);
			}
		}

		void Trader::Data::load_scales(scales_container_t & scales)
		{
			RUN_LOGGER(logger);

			try
			{
				std::fstream fin(File::scales.string(), std::ios::in);

				if (!fin)
				{
					throw trader_exception("cannot open file " + File::scales.string());
				}

				std::string scale;

				while (std::getline(fin, scale))
				{
					scales.push_back(scale);
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < trader_exception > (logger, exception);
			}
		}

		void Trader::initialize()
		{
			RUN_LOGGER(logger);

			try
			{
				load();
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < trader_exception > (logger, exception);
			}
		}

		void Trader::load()
		{
			RUN_LOGGER(logger);

			try
			{
				load_assets();
				load_scales();

				using years = std::chrono::duration < int, std::ratio < 60 * 60 * 24 * 365 > > ;
				
				auto first = Market::clock_t::now() - years(1);
				auto last  = Market::clock_t::now();

				for (const auto & asset : m_assets)
				{
					for (const auto & scale : m_scales)
					{
						parse_market_data(asset, m_market.get(asset, scale, first, last));
					}
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < trader_exception > (logger, exception);
			}
		}

		void Trader::load_assets()
		{
			RUN_LOGGER(logger);

			try
			{
				Data::load_assets(m_assets);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < trader_exception > (logger, exception);
			}
		}

		void Trader::load_scales()
		{
			RUN_LOGGER(logger);

			try
			{
				Data::load_scales(m_scales);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < trader_exception > (logger, exception);
			}
		}

		void Trader::parse_market_data(
			const std::string & asset, const std::filesystem::path & path)
		{
			RUN_LOGGER(logger);

			try
			{
				std::fstream fin(path.string(), std::ios::in);

				if (!fin)
				{
					throw trader_exception("cannot open file " + path.string());
				}

				std::string line;

				std::vector < record_t > data;

				while (std::getline(fin, line))
				{
					data.push_back(parse_market_data_line(line));
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < trader_exception > (logger, exception);
			}
		}

		Trader::record_t Trader::parse_market_data_line(const std::string & line)
		{
			RUN_LOGGER(logger);

			try
			{
				Candle_Parser < std::string::const_iterator > parser;

				auto first = std::begin(line);
				auto last  = std::end  (line);

				Candle candle;

				auto result = boost::spirit::qi::phrase_parse(
					first, last, parser, boost::spirit::qi::blank, candle);

				if (result)
				{
					//auto source = std::to_string(candle.date) + std::to_string(candle.time);
					//source.insert(std::next(source.begin(), 4 + 0 + 0), time_separator);
					//source.insert(std::next(source.begin(), 4 + 2 + 1), time_separator);
					//source.insert(std::next(source.begin(), 4 + 4 + 2), time_separator);
					//source.insert(std::next(source.begin(), 4 + 6 + 3), time_separator);
					//source.insert(std::next(source.begin(), 4 + 8 + 4), time_separator);

					return std::make_pair(parse_date_time(candle.date, candle.time), candle.price_close);
				}
				else
				{
					throw trader_exception("cannot parse line " + line);
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < trader_exception > (logger, exception);
			}
		}

		Trader::time_point_t Trader::parse_date_time(Candle::date_t date, Candle::time_t time)
		{
			RUN_LOGGER(logger);

			try
			{
				std::tm tm = {};

				//std::istringstream sin(source);
				//sin.imbue(std::locale(setlocale(LC_ALL, nullptr)));
				//// https://stackoverflow.com/questions/35041344/trying-to-use-stdget-time-to-parse-yymmdd-and-failing#comment69466398_35041344
				//sin >> std::get_time(&time, "%Y.%m.%d.%H.%M.%S");
				//if (sin.fail())
				//{
				//	throw trader_exception("cannot parse date-time " + source);
				//}

				tm.tm_year = date / 10000 - 1900;
				tm.tm_mon  = date % 10000 / 100 - 1;
				tm.tm_mday = date % 100;

				tm.tm_hour = time / 10000;
				tm.tm_min  = time % 100 / 100;
				tm.tm_sec  = time % 100;

				return clock_t::from_time_t(std::mktime(&tm));
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < trader_exception > (logger, exception);
			}
		}

		void Trader::run()
		{
			RUN_LOGGER(logger);

			try
			{
				std::cout << "Run trader ? (y/n) ";

				if (getchar() == 'y')
				{
					// ...
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < trader_exception > (logger, exception);
			}
		}

		void Trader::stop()
		{
			RUN_LOGGER(logger);

			try
			{
				// ...
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < trader_exception > (logger, exception);
			}
		}

	} // namespace trader

} // namespace solution