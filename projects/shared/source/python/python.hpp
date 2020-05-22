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

#include "../logger/logger.hpp"

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

		class Python_GIL
		{
		public:

			Python_GIL() : 
				m_state(PyGILState_Ensure())
			{}

			~Python_GIL() noexcept
			{
				try
				{
					PyGILState_Release(m_state);
				}
				catch (...)
				{
					std::abort();
				}
			}

		private:

			mutable PyGILState_STATE m_state;
		};

		class Python
		{
		public:

			static auto & global()
			{
				RUN_LOGGER(logger);

				try
				{
					if (!Py_IsInitialized())
					{
						Py_SetPythonHome(directory);

						Py_Initialize();
					}

					static auto global = boost::python::import("__main__").attr("__dict__");

					return global;
				}
				catch (const std::exception & exception)
				{
					catch_handler < python_exception > (logger, exception);
				}
			}

		private:

			static inline wchar_t directory[] = L"python";
		};

	} // namespace shared

} // namespace solution

#endif // #ifndef SOLUTION_SHARED_PYTHON_HPP