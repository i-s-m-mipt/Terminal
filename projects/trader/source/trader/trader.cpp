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
				std::filesystem::create_directory(directory);

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
						if (scale == initial_scale)
						{
							auto points = make_points(m_market.get(asset, scale, first, last));

							m_levels[asset][Level_Resolution::week] =
								reduce_levels(make_levels(points, Level_Resolution::week));
							m_levels[asset][Level_Resolution::month] =
								reduce_levels(make_levels(points, Level_Resolution::month));

							save_levels(asset);
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

		std::vector < Trader::Level > Trader::reduce_levels(std::vector < Level > && levels) const
		{
			RUN_LOGGER(logger);

			try
			{
				if (levels.size() > 1)
				{
					for (auto first = levels.begin(); first != std::prev(levels.end()); ++first)
					{
						for (auto current = std::next(first); current != levels.end();)
						{
							if (std::abs(first->price - current->price) / first->price <= price_deviation)
							{
								++first->strength;

								current = levels.erase(current);
							}
							else
							{
								++current;
							}
						}
					}
				}

				return levels;
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < trader_exception > (logger, exception);
			}
		}

		std::vector < Trader::Level > Trader::make_levels(
			const std::vector < Point > & points, Level_Resolution level_resolution) const
		{
			RUN_LOGGER(logger);

			try
			{
				auto frame = level_resolution_to_frame(level_resolution);

				std::vector < Level > levels;

				for (auto first = points.begin(); first != points.end(); )
				{
					auto last = std::next(first, std::min(frame, 
						static_cast < decltype(frame) > (std::distance(first, points.end()))));

					auto extremum = std::minmax_element(
						first, last, [](const auto & lhs, const auto & rhs)
					{
						return (lhs.price < rhs.price);
					});

					if ((extremum.first == first && first != points.begin() &&
						std::prev(first)->price > extremum.first->price) ||
						(extremum.first == std::prev(last) && last != points.end() &&
						last->price > extremum.first->price) ||
						(extremum.first != first && extremum.first != std::prev(last)))
					{
						levels.push_back(Level{ extremum.first->time, extremum.first->price, 0U });
					}

					if ((extremum.second == first && first != points.begin() &&
						std::prev(first)->price < extremum.second->price) ||
						(extremum.second == std::prev(last) && last != points.end() &&
						last->price > extremum.second->price) ||
						(extremum.second != first && extremum.second != std::prev(last)))
					{
						levels.push_back(Level{ extremum.second->time, extremum.second->price, 0U });
					}

					first = last;
				}

				return levels;
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < trader_exception > (logger, exception);
			}
		}

		std::vector < Trader::Point > Trader::make_points(
			const std::filesystem::path & path) const
		{
			RUN_LOGGER(logger);

			try
			{
				std::fstream fin(path.string(), std::ios::in);

				if (!fin)
				{
					throw trader_exception("cannot open file " + path.string());
				}

				std::vector < Point > plot;
				
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

		Trader::time_point_t Trader::parse(
			const Candle::date_t & date, const Candle::time_t & time) const
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

		std::size_t Trader::level_resolution_to_frame(Level_Resolution level_resolution) const
		{
			RUN_LOGGER(logger);

			try
			{
				switch (level_resolution)
				{
				case Level_Resolution::week:
					return 9U * 5U;
				case Level_Resolution::month:
					return 9U * 5U * 4U;
				default:
					throw trader_exception("unknown level resolution");
					break;
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < trader_exception > (logger, exception);
			}
		}

		void Trader::save_levels(const std::string & asset) const
		{
			RUN_LOGGER(logger);

			try
			{
				const std::filesystem::path file = asset + Extension::txt;
				const std::filesystem::path path = directory / file;

				std::fstream fout(path.string(), std::ios::out);

				if (!fout)
				{
					throw trader_exception("cannot open file " + path.string());
				}

				for (const auto & level_resolution : m_levels.at(asset))
				{
					fout << "[" << asset << "] " <<
						"resolution: " << level_resolution_to_string(level_resolution.first) << std::endl;

					fout << std::endl;

					for (const auto & level : level_resolution.second)
					{
						fout << level << std::endl;
					}

					fout << std::endl;
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < trader_exception > (logger, exception);
			}
		}

		void Trader::print_levels(
			const std::string & asset, Level_Resolution level_resolution) const
		{
			RUN_LOGGER(logger);

			try
			{
				std::cout << "[" << asset << "] " << 
					"resolution: " << level_resolution_to_string(level_resolution) << std::endl;

				std::cout << std::endl;

				for (const auto & level : m_levels.at(asset).at(level_resolution))
				{
					std::cout << level << std::endl;
				}

				std::cout << std::endl;
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < trader_exception > (logger, exception);
			}
		}

		std::string Trader::level_resolution_to_string(Level_Resolution level_resolution) const
		{
			RUN_LOGGER(logger);

			try
			{
				switch (level_resolution)
				{
				case Level_Resolution::week:
					return "W";
					break;
				case Level_Resolution::month:
					return "M";
					break;
				default:
					throw trader_exception("unknown level resolution");
					break;
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < trader_exception > (logger, exception);
			}
		}

		template < typename Duration, typename Clock >
		Duration duration_since_time_point(typename Clock::time_point time_point)
		{
			return std::chrono::duration_cast < Duration > (Clock::now() - time_point);
		}

		std::ostream & operator<<(std::ostream & stream, const Trader::Level & level)
		{
			RUN_LOGGER(logger);

			try
			{
				using days = std::chrono::duration < int, std::ratio < 60 * 60 * 24 > > ;

				auto time = decltype(level.time)::clock::to_time_t(level.time);

				auto tm = *std::localtime(&time);

				return (stream << 
					"price: " << std::setw(8) << std::setfill(' ') << std::right << 
						std::setprecision(2) << std::fixed << level.price << " " <<
					"since: " << std::put_time(&tm, "%y.%m.%d") << " "
					"alive: " << std::setw(3) << std::setfill(' ') << std::right <<
						duration_since_time_point < days, Trader::clock_t > (level.time).count() << " " <<
					"power: " << level.strength);
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
					run_implementation();
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

		void Trader::run_implementation() const
		{
			RUN_LOGGER(logger);

			try
			{
				//while (!is_session_open())
				//{
				//	std::this_thread::sleep_for(std::chrono::seconds(1));
				//}

				while(/*is_session_open()*/true)
				{
					system("cls");

					for (const auto & asset : m_levels)
					{
						const auto price = m_market.get_current_price(asset.first);

						std::cout << "[" << asset.first << "]     price: " << price << std::endl;

						for (const auto & level_resolution : asset.second)
						{
							for (const auto & level : level_resolution.second)
							{
								if (std::abs(level.price - price) / price <= price_deviation)
								{
									std::cout <<
										"[" << asset.first << "] " <<
										"(" << level_resolution_to_string(level_resolution.first) << ") " <<
										level << std::endl;
								}
							}
						}
					}

					std::this_thread::sleep_for(std::chrono::seconds(1));
				} 
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < trader_exception > (logger, exception);
			}
		}

		bool Trader::is_session_open() const
		{
			RUN_LOGGER(logger);

			try
			{
				std::time_t time = std::time(nullptr);

				std::tm tm = *std::localtime(&time);

				return ((tm.tm_hour >= 10) && 
					((tm.tm_hour < 18) || (tm.tm_hour == 18 && tm.tm_min < 40)));
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < trader_exception > (logger, exception);
			}
		}

	} // namespace trader

} // namespace solution