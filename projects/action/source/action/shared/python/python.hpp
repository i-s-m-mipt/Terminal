#ifndef SOLUTION_ACTION_PYTHON_HPP
#define SOLUTION_ACTION_PYTHON_HPP

#include <boost/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#  pragma once
#endif // #ifdef BOOST_HAS_PRAGMA_ONCE

#include <string>

#define BOOST_PYTHON_STATIC_LIB

#include <boost/python.hpp>

namespace solution
{
	namespace action
	{
		class Python_Initializer
		{
		public:

			Python_Initializer()
			{
				static wchar_t directory[] = L"python";

				if (!Py_IsInitialized())
				{
					Py_SetPythonHome(directory);

					Py_Initialize();
				}
			}

			~Python_Initializer() noexcept
			{
				try
				{
					if (Py_IsInitialized())
					{
						// Py_Finalize();
					}
				}
				catch (...)
				{
					// std::abort()
				}
			}
		};

	} // namespace action

} // namespace solution

#endif // #ifndef SOLUTION_ACTION_PYTHON_HPP