#ifndef SOLUTION_TRADER_TRADER_HPP
#define SOLUTION_TRADER_TRADER_HPP

#include <boost/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#  pragma once
#endif // #ifdef BOOST_HAS_PRAGMA_ONCE

#include <exception>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <vector>

#include <boost/extended/application/service.hpp>

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

		private:

			class Data
			{
			private:

				struct File
				{
					using path_t = std::filesystem::path;

					static inline const path_t assets = "trader/data/assets.data";
					static inline const path_t scales = "trader/data/scales.data";
				};

			private:

				using path_t = File::path_t;

			public:

				static void load_assets(assets_container_t & assets);
				static void load_scales(scales_container_t & scales);
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

		public:

			virtual void run() override;

			virtual void stop() override;

		private:

			assets_container_t m_assets;
			scales_container_t m_scales;

			Market m_market;
		};

	} // namespace trader

} // namespace solution

#endif // #ifndef SOLUTION_TRADER_TRADER_HPP