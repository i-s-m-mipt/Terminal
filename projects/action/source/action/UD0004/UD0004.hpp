#ifndef SOLUTION_ACTION_UD0004_HPP
#define SOLUTION_ACTION_UD0004_HPP

#include <boost/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#  pragma once
#endif // #ifdef BOOST_HAS_PRAGMA_ONCE

#include <algorithm>
#include <chrono>
#include <exception>
#include <iomanip>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include "../action.hpp"
#include "../shared/market/market.hpp"
#include "../shared/stream/stream.hpp"

#include "../../../../shared/source/config/config.hpp"
#include "../../../../shared/source/logger/logger.hpp"
#include "../../../../shared/source/memory/view/view_base.hpp"

namespace solution
{
	namespace action
	{
		namespace UD0004
		{
			using View = shared::memory::View_Base;

			using words_container_t = shared::Config::words_container_t;

			using stream_t = sfe::RichText;

			using Quotes = Market::Quotes;
			
			void run(std::shared_ptr < View > memory);

		} // namespace UD0004

	} // namespace action

} // namespace solution

#endif // #ifndef SOLUTION_ACTION_UD0004_HPP
