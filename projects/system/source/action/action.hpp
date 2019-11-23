#ifndef SOLUTION_SYSTEM_ACTION_HPP
#define SOLUTION_SYSTEM_ACTION_HPP

#include <boost/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#  pragma once
#endif // #ifdef BOOST_HAS_PRAGMA_ONCE

#include <atomic>
#include <chrono>
#include <cstdint>
#include <exception>
#include <future>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>
#include <type_traits>

#include <boost/dll.hpp>
#include <boost/extended/application/service.hpp>
#include <boost/filesystem.hpp>

#include "../../../shared/source/logger/logger.hpp"
#include "../../../shared/source/memory/view/view_base.hpp"

namespace solution
{
	namespace system
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

		class Action : public boost::extended::application::service
		{
		private:

			using View = shared::memory::View_Base;

		public:

			using path_t = boost::filesystem::path;
			using name_t = std::string; // wstring;

		public:

			enum class Status
			{
				running,
				stopped,
				failure
			};

		private:

			class Task
			{
			public:

				using argument_t = std::shared_ptr < View > ;

				using callback_t = void(argument_t);

				using function_t = std::function < callback_t > ;

			public:

				Task(argument_t argument, function_t function, std::atomic < Status > & status) :
					m_argument(argument), m_function(function), m_status(status)
				{}

				~Task() noexcept = default;

			public:

				void operator()() const;

			private:

				static const inline std::size_t critical_error_quantity = 60;

			private:

				argument_t m_argument;
				function_t m_function;

			private:

				std::atomic < Status > & m_status;

			private:

				mutable std::size_t m_error_counter = 0;
			};

		public:

			template < typename P, typename N, typename Enable =
				std::enable_if_t < 
					std::is_convertible_v < P, path_t > &&
					std::is_convertible_v < N, name_t > > >
			explicit Action(P && path, N && name) :
				m_path(std::forward < P > (path)), 
				m_name(std::forward < N > (name))
			{
				initialize();
			}

			~Action() noexcept
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

		public:

			const auto & path() const noexcept
			{
				return m_path;
			}

			const auto & name() const noexcept
			{
				return m_name;
			}

			const auto & library() const noexcept
			{
				return m_path.filename().stem().string();
			}

			auto status() const noexcept
			{
				return m_status.load();
			}

		public:

			void set(std::shared_ptr < View > view) noexcept
			{
				m_view = view;
			}

		public:

			virtual void run() override;

			virtual void stop() override;

		private:

			path_t m_path;
			name_t m_name;

			std::shared_ptr < View > m_view;

		private:

			mutable std::future < void >   m_future;

			mutable std::atomic < Status > m_status;
		};

	} // namespace system

} // namespace solution

#endif // #ifndef SOLUTION_SYSTEM_ACTION_HPP