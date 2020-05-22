#ifndef SOLUTION_TRADER_SYSTEM_HPP
#define SOLUTION_TRADER_SYSTEM_HPP

#include <boost/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#  pragma once
#endif // #ifdef BOOST_HAS_PRAGMA_ONCE

#include <exception>
#include <stdexcept>
#include <string>

#include <boost/extended/application/service.hpp>

#include "../../../shared/source/logger/logger.hpp"
#include "../../../shared/source/python/python.hpp"

namespace solution 
{
	namespace trader
	{
		class system_exception : public std::exception
		{
		public:

			explicit system_exception(const std::string & message) noexcept : 
				std::exception(message.c_str()) 
			{}

			explicit system_exception(const char * const message) noexcept :
				std::exception(message)
			{}

			~system_exception() noexcept = default;
		};

		class System : public boost::extended::application::service
		{
		public:

			~System() noexcept
			{
				try
				{
					uninitialize();
				}
				catch (...)
				{
					// std::abort();
				}
			}

		private:

			void initialize() {};

			void uninitialize() {};

		public:

			virtual void run() override {};

			virtual void stop() override {};
		};

	} // namespace trader

} // namespace solution

#endif // #ifndef SOLUTION_TRADER_SYSTEM_HPP