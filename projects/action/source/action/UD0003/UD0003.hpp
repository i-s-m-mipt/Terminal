#ifndef SOLUTION_ACTION_UD0003_HPP
#define SOLUTION_ACTION_UD0003_HPP

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
		namespace UD0003
		{
			using View = shared::memory::View_Base;

			using words_container_t = shared::Config::words_container_t;

			using stream_t = sfe::RichText;

			struct Key
			{
				static inline const std::string BO = "BO";
				static inline const std::string BC = "BC";
				static inline const std::string SO = "SO";
				static inline const std::string SC = "SC";
				static inline const std::string WW = "WW";
			};

			struct Probability
			{
				double BO;
				double BC;
				double SO;
				double SC;
				double WW;

				static inline const double threshold = 0.666;
			};

			enum class Signal
			{
				B,
				S,
				W
			};

			void run(std::shared_ptr < View > memory);

		} // namespace UD0003

	} // namespace action

} // namespace solution

#endif // #ifndef SOLUTION_ACTION_UD0003_HPP
