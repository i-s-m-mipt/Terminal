#ifndef SOLUTION_PLUGIN_MARKET_QUOTES_HPP
#define SOLUTION_PLUGIN_MARKET_QUOTES_HPP

#include <boost/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#  pragma once
#endif // #ifdef BOOST_HAS_PRAGMA_ONCE

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <exception>
#include <fstream>
#include <iomanip>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/deque.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/managed_windows_shared_memory.hpp>

#include <qluacpp/qlua>

#include "../../config/config.hpp"

#include "../../../../shared/source/logger/logger.hpp"

namespace solution
{
	namespace plugin
	{
		namespace market
		{
			class quotes_exception : public std::exception
			{
			public:

				explicit quotes_exception(const std::string & message) noexcept :
					std::exception(message.c_str())
				{}

				explicit quotes_exception(const char * const message) noexcept :
					std::exception(message)
				{}

				~quotes_exception() noexcept = default;
			};

			class Quotes
			{
			public:

				using api_t = Config::api_t;

			private:

				using shared_memory_t = boost::interprocess::managed_shared_memory;
				
				using mutex_t = boost::interprocess::interprocess_mutex;

				using price_t = double;

				using volume_t = std::uint64_t;

				using orders_container_t = std::vector < std::pair < price_t, volume_t > > ;

				using path_t = boost::filesystem::path;

			private:

				struct Name
				{
					static inline const std::string total_ask = "total_ask";
					static inline const std::string total_bid = "total_bid";
				};

			private:

				struct Extension
				{
					using extension_t = std::string;

					static inline const extension_t txt = ".txt";
					static inline const extension_t csv = ".csv";

					static inline const extension_t empty = "";
				};

			public:

				template < typename C, typename A, typename Enable =
					std::enable_if_t <
						std::is_convertible_v < C, std::string > &&
						std::is_convertible_v < A, std::string > > >
				explicit Quotes(C && class_code, A && asset_code, const api_t & api) :
					m_class_code(std::forward < C > (class_code)),
					m_asset_code(std::forward < A > (asset_code)), m_api(api)
				{
					initialize();
				}

				~Quotes() noexcept
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

				void initialize();

				void uninitialize();

			private:

				std::string make_shared_memory_name() const;

			public:

				const auto & class_code() const noexcept
				{
					return m_class_code;
				}

				const auto & asset_code() const noexcept
				{
					return m_asset_code;
				}

			public:

				void update();

			private:

				void save() const;

				void reset_orders();

			private:

				static inline const std::size_t depth = 20U;

				static inline const path_t directory = "quotes";

			private:

				const std::string m_class_code;
				const std::string m_asset_code;

			private:

				api_t m_api;

				shared_memory_t m_shared_memory;

				mutex_t * m_mutex;

				volume_t * m_total_ask;
				volume_t * m_total_bid;

				orders_container_t m_asks;
				orders_container_t m_bids;

				path_t m_path;

			private:

				mutable std::time_t m_last_time = 0;
			};

		} // namespace market

	} // namespace plugin

} // namespace solution

#endif // #ifndef SOLUTION_PLUGIN_MARKET_QUOTES_HPP