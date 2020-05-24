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
						if (scale == "M1")
						{
							auto plot = make_plot(m_market.get(asset, scale, first, last));
						}
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

		Trader::plot_t Trader::make_plot(const std::filesystem::path & path) const
		{
			RUN_LOGGER(logger);

			try
			{
				std::fstream fin(path.string(), std::ios::in);

				if (!fin)
				{
					throw trader_exception("cannot open file " + path.string());
				}

				plot_t plot; 
				
				plot.reserve(365);

				std::string line;

				while (std::getline(fin, line))
				{
					plot.push_back(parse(line));
				}

				return plot;
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < trader_exception > (logger, exception);
			}
		}

		Trader::Point Trader::parse(const std::string & line) const
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
					return Point { parse(candle.date, candle.time), candle.price_close };
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

		Trader::time_point_t Trader::parse(const Candle::date_t & date, const Candle::time_t & time) const
		{
			RUN_LOGGER(logger);

			try
			{
				std::tm tm;

				auto source = date + " " + time;

				std::istringstream sin(source);

				sin >> std::get_time(&tm, "%d/%m/%y %H:%M:%S");

				if (sin.fail())
				{
					throw trader_exception("cannot parse date-time " + source);
				}

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