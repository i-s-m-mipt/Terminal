#ifndef SOLUTION_ACTION_UD0001_HPP
#define SOLUTION_ACTION_UD0001_HPP

#include <boost/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#  pragma once
#endif // #ifdef BOOST_HAS_PRAGMA_ONCE

#include <exception>
#include <future>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>

#define BOOST_PYTHON_STATIC_LIB

#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/extended/serialization/json.hpp>
#include <boost/python.hpp>

#include "../action.hpp"
#include "../shared/market/market.hpp"
#include "../shared/python/python.hpp"

#include "../../../../shared/source/config/config.hpp"
#include "../../../../shared/source/logger/logger.hpp"
#include "../../../../shared/source/memory/view/view_base.hpp"

namespace solution
{
	namespace action
	{
		namespace UD0001
		{
			using View = shared::memory::View_Base;

			using words_container_t = shared::Config::words_container_t;

			using Severity = shared::Logger::Severity;

			struct Key
			{
				static inline const std::string BO = "BO";
				static inline const std::string BC = "BC";
				static inline const std::string SO = "SO";
				static inline const std::string SC = "SC";
				static inline const std::string WW = "WW";
			};

			void run(std::shared_ptr < View > memory);

		} // namespace UD0001

	} // namespace action

} // namespace solution

#endif // #ifndef SOLUTION_ACTION_UD0001_HPP


