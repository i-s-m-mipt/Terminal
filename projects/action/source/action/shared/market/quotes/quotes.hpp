#ifndef SOLUTION_ACTION_MARKET_QUOTES_HPP
#define SOLUTION_ACTION_MARKET_QUOTES_HPP

#include <boost/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#  pragma once
#endif // #ifdef BOOST_HAS_PRAGMA_ONCE

#include <cstdint>
#include <exception>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace solution
{
	namespace action
	{
		namespace market
		{
			struct Quotes
			{
				using volume_t = std::uint64_t;

				struct Name
				{
					static inline const std::string total_ask = "total_ask";
					static inline const std::string total_bid = "total_bid";
				};

				static inline const double threshold = 0.1;

				volume_t total_ask;
				volume_t total_bid;
			};

		} // namespace market

	} // namespace action

} // namespace solution

#endif // #ifndef SOLUTION_ACTION_MARKET_QUOTES_HPP