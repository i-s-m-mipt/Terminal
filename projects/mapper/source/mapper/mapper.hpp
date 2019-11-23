#ifndef SOLUTION_MAPPER_MAPPER_HPP
#define SOLUTION_MAPPER_MAPPER_HPP

#include <boost/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#  pragma once
#endif // #ifdef BOOST_HAS_PRAGMA_ONCE

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <exception>
#include <iterator>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <boost/config/warning_disable.hpp>
#include <boost/filesystem.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>

#include "../../../shared/source/logger/logger.hpp"

namespace solution
{
	namespace mapper
	{
		class mapper_exception : public std::exception
		{
		public:

			explicit mapper_exception(const std::string & message) noexcept :
				std::exception(message.c_str())
			{}

			explicit mapper_exception(const char * const message) noexcept :
				std::exception(message)
			{}

			~mapper_exception() noexcept = default;
		};

		class Mapper
		{
		private:

			using path_t = boost::filesystem::path;

		public:

			struct Candle
			{
				using date_t = std::uint32_t;
				using time_t = std::uint32_t;

				using price_t = double;

				using volume_t = std::uint64_t;

				date_t date;
				time_t time;

				price_t price_open  = 0.0;
				price_t price_high  = 0.0;
				price_t price_low   = 0.0;
				price_t price_close = 0.0;

				volume_t volume = 0;
			};

		private:

			using data_t = std::vector < std::pair < Candle, std::string > > ;

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

			template < typename P, typename Enable =
				std::enable_if_t < std::is_convertible_v < P, path_t > > >
			explicit Mapper(P && path) : m_path(std::forward < P > (path))
			{
				initialize();
			}

			~Mapper() noexcept = default;

		private:

			void initialize();

		public:

			void run();

		public:

			static Candle parse(const std::string & s);

		private:

			void load();

			void mark();

			void save();

		private:

			static inline const path_t directory = "mapper/candles";

		private:

			const path_t m_path;

		private:

			data_t m_data;
		};

	} // namespace mapper

} // namespace solution

BOOST_FUSION_ADAPT_STRUCT
(
	 solution::mapper::Mapper::Candle,
	(solution::mapper::Mapper::Candle::date_t,   date)
	(solution::mapper::Mapper::Candle::time_t,   time)
	(solution::mapper::Mapper::Candle::price_t,  price_open)
	(solution::mapper::Mapper::Candle::price_t,  price_high)
	(solution::mapper::Mapper::Candle::price_t,  price_low)
	(solution::mapper::Mapper::Candle::price_t,  price_close)
	(solution::mapper::Mapper::Candle::volume_t, volume)
)

#endif // #ifndef SOLUTION_MAPPER_MAPPER_HPP