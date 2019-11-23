#ifndef SOLUTION_SYSTEM_SYSTEM_HPP
#define SOLUTION_SYSTEM_SYSTEM_HPP

#include <boost/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#  pragma once
#endif // #ifdef BOOST_HAS_PRAGMA_ONCE

#include <chrono>
#include <exception>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <type_traits>
#include <unordered_map>

#include <boost/extended/application/service.hpp>
#include <boost/extended/serialization/json.hpp>
#include <boost/filesystem/path.hpp>

#include "../action/action.hpp"
#include "../memory/memory.hpp"
#include "../memory/view/view.hpp"

#include "../../../shared/source/logger/logger.hpp"

namespace solution 
{
	namespace system
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
		private:

			using name_t = Action::name_t;

			using actions_container_t = 
				std::unordered_map < name_t, std::shared_ptr < Action > > ;

			using View = memory::View;

		private:

			class Data
			{
			private:

				using json_t = boost::extended::serialization::json;

			private:

				struct File
				{
					using path_t = boost::filesystem::path;

					static inline const path_t actions_data = "system/data/actions.data";
				};

			private:

				using path_t = File::path_t;

			private:

				struct Key
				{
					struct Action
					{
						static inline const std::string path = "path";
						static inline const std::string name = "name";
					};
				};

			public:

				static void load(	   actions_container_t & actions);

				static void save(const actions_container_t & actions);

			private:

				static void load(const path_t & path,		json_t & object);

				static void save(const path_t & path, const json_t & object);
			};

		public:

			System() : m_memory(std::make_shared < Memory > ())
			{
				initialize();
			}

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

			void initialize();

			void uninitialize();

		private:

			void load();

			void save();

		private:

			void load_actions();

			void save_actions();

		public:

			virtual void run() override;

			virtual void stop() override;

		public:

			void append_action(const Action::path_t & path, const Action::name_t & name);

			void remove_action(const name_t & name);

		private:

			std::shared_ptr < View > make_view() const;

		public:

			void run_all_actions() const;

			void run(const name_t & name) const;

			void stop_all_actions() const;

			void stop(const name_t & name) const;

		private:

			actions_container_t m_actions;

			std::shared_ptr < Memory > m_memory;
		};

	} // namespace system

} // namespace solution

#endif // #ifndef SOLUTION_SYSTEM_SYSTEM_HPP