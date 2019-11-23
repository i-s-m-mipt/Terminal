#include <algorithm>
#include <chrono>
#include <exception>
#include <fstream>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>

#include "market/market.hpp"
#include "mapper/mapper.hpp"

#include "../../shared/source/logger/logger.hpp"

using Logger = solution::shared::Logger;

using Market = solution::mapper::Market;
using Mapper = solution::mapper::Mapper;

using Candle = Mapper::Candle;

using path_t = Market::path_t;

auto load_m1_candles(const Market & market, const std::string & asset)
{
	RUN_LOGGER(logger);

	try
	{
		auto file = market.get(asset, "M1");

		std::fstream fin(file.string(), std::ios::in);

		if (!fin)
		{
			throw std::runtime_error("cannot open file " + file.string());
		}

		std::vector < Candle > m1_candles;

		std::string s;

		while (std::getline(fin, s))
		{
			m1_candles.push_back(Mapper::parse(s));
		}

		return m1_candles;
	}
	catch (const std::exception & exception)
	{
		solution::shared::catch_handler < std::runtime_error > (logger, exception);
	}
}

auto save_m5_candles(const path_t & path, const std::vector < Candle > & m5_candles)
{
	RUN_LOGGER(logger);

	try
	{
		std::fstream fout(path.string(), std::ios::out);

		if (!fout)
		{
			throw std::runtime_error("cannot open file " + path.string());
		}

		static const auto separator = ',';

		for (const auto & m5_candle : m5_candles)
		{
			fout <<
				m5_candle.date		  << separator <<
				m5_candle.time		  << separator <<
				m5_candle.price_open  << separator <<
				m5_candle.price_high  << separator <<
				m5_candle.price_low   << separator <<
				m5_candle.price_close << separator <<
				m5_candle.volume	  << std::endl;
		}

		return path;
	}
	catch (const std::exception & exception)
	{
		solution::shared::catch_handler < std::runtime_error > (logger, exception);
	}
}

auto combine_m1_to_m5_candles(const std::vector < Candle > & m1_candles, std::size_t shift)
{
	RUN_LOGGER(logger);

	try
	{
		if (m1_candles.size() < 5)
		{
			throw std::runtime_error("not enougth M1 candles");
		}

		std::vector < Candle > m5_candles;

		for (std::size_t j = shift; j <= m1_candles.size() - 5; j += 5)
		{
			Candle m5_candle;

			m5_candle.date = m1_candles.at(j).date;
			m5_candle.time = m1_candles.at(j).time;

			m5_candle.price_open = m1_candles.at(j).price_open;

			m5_candle.price_close = m1_candles.at(j + 4).price_close;

			m5_candle.price_high =
				std::max_element(std::begin(m1_candles) + j, std::begin(m1_candles) + j + 5,
					[](const auto & lhs, const auto & rhs)
			{
				return (lhs.price_high < rhs.price_high);
			})->price_high;

			m5_candle.price_low =
				std::min_element(std::begin(m1_candles) + j, std::begin(m1_candles) + j + 5,
					[](const auto & lhs, const auto & rhs)
			{
				return (lhs.price_low < rhs.price_low);
			})->price_low;

			m5_candle.volume = 0;

			std::for_each(std::begin(m1_candles) + j, std::begin(m1_candles) + j + 5,
				[&m5_candle](const auto & m1_candle)
			{
				m5_candle.volume += m1_candle.volume;
			});

			m5_candles.push_back(m5_candle);
		}

		return m5_candles;
	}
	catch (const std::exception & exception)
	{
		solution::shared::catch_handler < std::runtime_error > (logger, exception);
	}
}

void map_combined_m5_from_m1(const Market & market)
{
	RUN_LOGGER(logger);

	try
	{
		for (const auto & asset : market.assets())
		{
			auto m1_candles = load_m1_candles(market, asset);

			std::reverse(std::begin(m1_candles), std::end(m1_candles));

			for (std::size_t i = 0; i < 5; ++i)
			{
				auto m5_candles = combine_m1_to_m5_candles(m1_candles, i);

				std::reverse(std::begin(m5_candles), std::end(m5_candles));

				Market::path_t temporary_file = 
					asset + "_M5_v" + std::to_string(i) + Market::Extension::txt;

				save_m5_candles(temporary_file, m5_candles);

				Mapper mapper(temporary_file);

				mapper.run();

				std::remove(temporary_file.string().c_str());
			}
		}
	}
	catch (const std::exception & exception)
	{
		solution::shared::catch_handler < std::runtime_error > (logger, exception);
	}
}

int main(int argc, char * argv[])
{
	RUN_LOGGER(logger);

	try
	{
		Market market;

		path_t file = "market/candles/GAZP_M1.txt";

		Mapper mapper(file);

		// mapper.run();

		system("pause");

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
