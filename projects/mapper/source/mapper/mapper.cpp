#include "mapper.hpp"

namespace solution
{
	namespace mapper
	{
		void Mapper::initialize()
		{
			RUN_LOGGER(logger);

			try
			{
				boost::filesystem::create_directory(directory);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < mapper_exception > (logger, exception);
			}
		}

		void Mapper::run()
		{
			RUN_LOGGER(logger);

			try
			{
				load();
				mark();
				save();
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < mapper_exception > (logger, exception);
			}
		}

		Mapper::Candle Mapper::parse(const std::string & s)
		{
			RUN_LOGGER(logger);

			try
			{
				Candle_Parser < std::string::const_iterator > parser;

				auto first = std::begin(s);
				auto last  = std::end(s);

				Candle candle;

				auto result = boost::spirit::qi::phrase_parse(
					first, last, parser, boost::spirit::qi::blank, candle);

				if (result)
				{
					return candle;
				}
				else
				{
					throw mapper_exception("cannot parse line " + s);
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < mapper_exception > (logger, exception);
			}
		}

		void Mapper::load()
		{
			RUN_LOGGER(logger);

			try
			{
				std::fstream fin(m_path.string(), std::ios::in);

				if (!fin)
				{
					throw mapper_exception("cannot open file " + m_path.string());
				}

				std::string s;

				while (std::getline(fin, s))
				{
					m_data.push_back(std::make_pair(parse(s), std::string()));
				}

				std::reverse(std::begin(m_data), std::end(m_data));
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < mapper_exception > (logger, exception);
			}
		}

		void Mapper::mark()
		{
			RUN_LOGGER(logger);

			try
			{
				static const auto min_price_change   = 0.002;
				static const auto max_price_rollback = 0.330;

				for (auto first = std::begin(m_data); first != std::end(m_data);)
				{
					auto flag = false;

					for (auto last = std::next(first); last != std::end(m_data); ++last)
					{
						auto result = std::minmax_element(first, std::next(last), [](const auto & lhs, const auto & rhs)
						{
							return lhs.first.price_close < rhs.first.price_close;
						});

						auto min_price = result.first->first.price_close;
						auto max_price = result.second->first.price_close;

						auto first_extremum = result.first;
						auto last_extremum  = result.second;

						if (std::distance(first, result.first) > std::distance(first, result.second))
						{
							first_extremum = result.second;
							last_extremum  = result.first;
						}

						if ((((max_price - min_price) / min_price < min_price_change) ||
							(result.first == last) ||
							(result.second == last) ||
							((last_extremum != last) &&
							((max_price - min_price) / min_price > min_price_change) &&
								(abs(last_extremum->first.price_close - last->first.price_close) < max_price_rollback *
									abs(first_extremum->first.price_close - last_extremum->first.price_close)))) &&
									(abs(last_extremum->first.price_close - last->first.price_close) /
										std::min(last_extremum->first.price_close, last->first.price_close) < min_price_change))
						{
							continue;
						}

						if (first_extremum->first.price_close < last_extremum->first.price_close)
						{
							first_extremum->second += "BO";
							last_extremum ->second += "SC";
						}
						else
						{
							first_extremum->second += "SO";
							last_extremum ->second += "BC";
						}

						first = last_extremum;

						flag = true;

						break;
					}

					if (!flag)
					{
						break;
					}
				}

				for (auto & record : m_data)
				{
					if (record.second.empty())
					{
						record.second += "WW";
					}
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < mapper_exception > (logger, exception);
			}
		}

		void Mapper::save()
		{
			RUN_LOGGER(logger);

			try
			{
				auto path = directory; path /= m_path.filename();

				std::fstream fout(path.string(), std::ios::out);

				if (!fout)
				{
					throw mapper_exception("cannot open file " + path.string());
				}

				static const auto separator = ',';

				for (const auto & record : m_data)
				{
					fout <<
						record.first.date		 << separator <<
						record.first.time		 << separator <<
						record.first.price_open  << separator <<
						record.first.price_high  << separator <<
						record.first.price_low   << separator <<
						record.first.price_close << separator <<
						record.first.volume		 << separator <<
						record.second			 << std::endl;
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < mapper_exception > (logger, exception);
			}
		}

	} // namespace mapper

} // namespace solution