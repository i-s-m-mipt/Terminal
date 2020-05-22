#ifndef SOLUTION_SHARED_PYTHON_HPP
#define SOLUTION_SHARED_PYTHON_HPP

#include <boost/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#  pragma once
#endif // #ifdef BOOST_HAS_PRAGMA_ONCE

#include <exception>
#include <stdexcept>
#include <string>

#define BOOST_PYTHON_STATIC_LIB

#include <boost/python.hpp>

#include "../../../shared/source/logger/logger.hpp"

namespace solution
{
	namespace shared
	{
		class python_exception : public std::exception
		{
		public:

			explicit python_exception(const std::string & message) noexcept :
				std::exception(message.c_str())
			{}

			explicit python_exception(const char * const message) noexcept :
				std::exception(message)
			{}

			~python_exception() noexcept = default;
		};

		class Python
		{
		public:

			Python()
			{
				initialize();
			}

			~Python() noexcept
			{
				try
				{
					uninitialize();
				}
				catch (...)
				{
					std::abort();
				}
			}

		private:

			void initialize();

			void uninitialize();

		public:

			auto & global() const noexcept
			{
				return m_global;
			}

		public:

			std::string exception() noexcept;

		private:

			static inline wchar_t directory[] = L"python";

		private:

			boost::python::api::object m_global;

		private:

			mutable PyGILState_STATE m_state;
		};

	} // namespace shared

} // namespace solution

#endif // #ifndef SOLUTION_SHARED_PYTHON_HPP