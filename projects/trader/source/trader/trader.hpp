#ifndef SOLUTION_TRADER_TRADER_HPP
#define SOLUTION_TRADER_TRADER_HPP

#include <boost/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#  pragma once
#endif // #ifdef BOOST_HAS_PRAGMA_ONCE

#include <algorithm>
#include <chrono>
#include <ctime>
#include <cstdint>
#include <exception>
#include <execution>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <locale>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
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

		private:

			struct Point
			{
				time_point_t time;
				double price;
			};

		private:

			enum class Level_Resolution
			{
				hour,
				day,
				week,
				month,
			};

		private:

			struct Level
			{
				time_point_t time;
				double price;
				std::size_t strength;
			};

		private:

			using levels_container_t = std::unordered_map < std::string, 
				std::unordered_map < Level_Resolution, std::vector < Level > > > ;

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
				using date_t = std::string;
				using time_t = std::string;

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
						+(boost::spirit::qi::char_ - separator) >> separator >>
						+(boost::spirit::qi::char_ - separator) >> separator >>
						  boost::spirit::qi::double_			>> separator >>
						  boost::spirit::qi::double_			>> separator >>
						  boost::spirit::qi::double_			>> separator >>
						  boost::spirit::qi::double_			>> separator >>
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

			std::vector < Level > make_levels(
				const std::vector < Point > & points, Level_Resolution level_resolution) const;

			std::vector < Point > make_points(const std::filesystem::path & path) const;

			Point parse(const std::string & line) const;

			time_point_t parse(const Candle::date_t & date, const Candle::time_t & time) const;

			std::size_t resolution_to_frame(Level_Resolution level_resolution) const;
			
			template < typename D = std::chrono::duration < int, std::ratio < 60 * 60 * 24 > > >
			D duration_since_time_point(time_point_t time_point) const;

		public:

			virtual void run() override;

			virtual void stop() override;

		private:

			assets_container_t m_assets;
			scales_container_t m_scales;

			Market m_market;

			levels_container_t m_levels;
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