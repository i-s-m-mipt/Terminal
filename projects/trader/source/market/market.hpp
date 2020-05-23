#ifndef SOLUTION_TRADER_MARKET_HPP
#define SOLUTION_TRADER_MARKET_HPP

#include <boost/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#  pragma once
#endif // #ifdef BOOST_HAS_PRAGMA_ONCE

#include <chrono>
#include <cstdint>
#include <ctime>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <sstream>

#define BOOST_PYTHON_STATIC_LIB

//#include <boost/interprocess/allocators/allocator.hpp>
//#include <boost/interprocess/containers/deque.hpp>
//#include <boost/interprocess/containers/string.hpp>
//#include <boost/interprocess/managed_shared_memory.hpp>
//#include <boost/interprocess/sync/interprocess_mutex.hpp>
//#include <boost/interprocess/sync/named_mutex.hpp>
//#include <boost/interprocess/sync/scoped_lock.hpp>
//#include <boost/interprocess/managed_windows_shared_memory.hpp>
#include <boost/python.hpp>

#include "../../../shared/source/logger/logger.hpp"
#include "../../../shared/source/python/python.hpp"

namespace solution
{
	namespace trader
	{
		class market_exception : public std::exception
		{
		public:

			explicit market_exception(const std::string & message) noexcept :
				std::exception(message.c_str())
			{}

			explicit market_exception(const char * const message) noexcept :
				std::exception(message)
			{}

			~market_exception() noexcept = default;
		};

		class Market
		{
		public:

			using clock_t = std::chrono::system_clock;

		private:

			using time_point_t = std::chrono::time_point < clock_t > ;

			using date_t = std::string;

			using Severity = shared::Logger::Severity;

		private:

			struct Extension
			{
				using extension_t = std::string;

				static inline const extension_t txt = ".txt";
				static inline const extension_t csv = ".csv";
				static inline const extension_t dat = ".dat";

				static inline const extension_t empty = "";
			};

		public:

			Market()
			{
				initialize();
			}

			~Market() noexcept = default;

		private:

			void initialize();

		public:

			std::filesystem::path get(
				const std::string & asset, const std::string & scale, time_point_t first, time_point_t last) const;
			
			// std::filesystem::path get(
			//	const std::string & asset, const std::string & scale, std::size_t quantity) const;
			
		private:

			date_t make_date(time_point_t time_point) const;
			
		private:

			static inline const std::filesystem::path directory = "market/data";
		};

	} // namespace trader

} // namespace solution

#endif // #ifndef SOLUTION_TRADER_MARKET_HPP