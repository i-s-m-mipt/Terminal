#ifndef SOLUTION_ACTION_ACTION_HPP
#define SOLUTION_ACTION_ACTION_HPP

#include <boost/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#  pragma once
#endif // #ifdef BOOST_HAS_PRAGMA_ONCE

#include <exception>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace solution
{
	namespace action
	{
		class action_exception : public std::exception
		{
		public:

			explicit action_exception(const std::string & message) noexcept :
				std::exception(message.c_str())
			{}

			explicit action_exception(const char * const message) noexcept :
				std::exception(message)
			{}

			~action_exception() noexcept = default;
		};

	} // namespace action

} // namespace solution

#endif // #ifndef SOLUTION_ACTION_ACTION_HPP
