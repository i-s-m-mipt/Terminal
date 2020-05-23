#ifndef SOLUTION_TRADER_TRADER_HPP
#define SOLUTION_TRADER_TRADER_HPP

#include <boost/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#  pragma once
#endif // #ifdef BOOST_HAS_PRAGMA_ONCE

#include <chrono>
#include <ctime>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <locale>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <boost/config/warning_disable.hpp>
#include <boost/extended/application/service.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>

#include "../market/market.hpp"

#include "../../../shared/source/logger/logger.hpp"

namespace solution 
{
	namespace trader
	{
		class trader_exception : public std::exception
		{
		public:

			explicit trader_exception(const std::string & message) noexcept : 
				std::exception(message.c_str()) 
			{}

			explicit trader_exception(const char * const message) noexcept :
				std::exception(message)
			{}

			~trader_exception() noexcept = default;
		};

		class Trader : public boost::extended::application::service
		{
		private:

			using assets_container_t = std::vector < std::string > ;
			using scales_container_t = std::vector < std::string > ;

			using clock_t = std::chrono::system_clock;

			using time_point_t = clock_t::time_point;

			using record_t = std::pair < time_point_t, double > ;

		private:

			class Data
			{
			private:

				struct File
				{
					static inline const std::filesystem::path assets = "trader/data/assets.data";
					static inline const std::filesystem::path scales = "trader/data/scales.data";
				};

			public:

				static void load_assets(assets_container_t & assets);
				static void load_scales(scales_container_t & scales);
			};

		public:

			struct Candle
			{
				using date_t = std::uint32_t;
				using time_t = std::uint32_t;

				using price_t = double;

				using volume_t = std::uint64_t;

				date_t date;
				time_t time;

				price_t price_open;
				price_t price_high;
				price_t price_low;
				price_t price_close;

				volume_t volume;
			};

		private:

			template < typename Iterator >
			class Candle_Parser :
				public boost::spirit::qi::grammar < Iterator, Candle(), boost::spirit::qi::blank_type >
			{
			private:

				using rule_t = boost::spirit::qi::rule < Iterator, Candle(), boost::spirit::qi::blank_type > ;

			public:

				Candle_Parser() : Candle_Parser::base_type(start)
				{
					static const auto separator = ',';

					start %=
						boost::spirit::qi::uint_   >> separator >>
						boost::spirit::qi::uint_   >> separator >>
						boost::spirit::qi::double_ >> separator >>
						boost::spirit::qi::double_ >> separator >>
						boost::spirit::qi::double_ >> separator >>
						boost::spirit::qi::double_ >> separator >>
						boost::spirit::qi::ulong_long;
				}

				~Candle_Parser() noexcept = default;

			private:

				rule_t start;
			};

		public:

			Trader()
			{
				initialize();
			}

			~Trader() noexcept = default;

		private:

			void initialize();

		private:

			void load();

		private:

			void load_assets();
			void load_scales();

			void parse_market_data(
				const std::string & asset, const std::filesystem::path & path);

			record_t parse_market_data_line(const std::string & line);

			time_point_t parse_date_time(Candle::date_t date, Candle::time_t time);

		public:

			virtual void run() override;

			virtual void stop() override;

		private:

			static inline const char time_separator = '.';

		private:

			assets_container_t m_assets;
			scales_container_t m_scales;

			Market m_market;
		};

	} // namespace trader

} // namespace solution

BOOST_FUSION_ADAPT_STRUCT
(
	 solution::trader::Trader::Candle,
	(solution::trader::Trader::Candle::date_t,   date)
	(solution::trader::Trader::Candle::time_t,   time)
	(solution::trader::Trader::Candle::price_t,  price_open)
	(solution::trader::Trader::Candle::price_t,  price_high)
	(solution::trader::Trader::Candle::price_t,  price_low)
	(solution::trader::Trader::Candle::price_t,  price_close)
	(solution::trader::Trader::Candle::volume_t, volume)
)

#endif // #ifndef SOLUTION_TRADER_TRADER_HPP