#include "python.hpp"

namespace solution
{
	namespace shared
	{
		void Python::initialize()
		{
			RUN_LOGGER(logger);

			try
			{
				if (!Py_IsInitialized())
				{
					Py_SetPythonHome(directory);

					Py_Initialize();
				}

				m_state = PyGILState_Ensure();

				m_global = boost::python::import("__main__").attr("__dict__");
			}
			catch (const std::exception & exception)
			{
				catch_handler < python_exception > (logger, exception);
			}
		}

		void Python::uninitialize()
		{
			RUN_LOGGER(logger);

			try
			{
				PyGILState_Release(m_state);
			}
			catch (const std::exception & exception)
			{
				catch_handler < python_exception > (logger, exception);
			}
		}

		std::string Python::exception() noexcept
		{
			PyObject * error;
			PyObject * value;
			PyObject * stack;

			PyErr_Fetch				(&error, &value, &stack);
			PyErr_NormalizeException(&error, &value, &stack);

			boost::python::handle <> handle_error(error);

			boost::python::handle <> handle_value(boost::python::allow_null(value));
			boost::python::handle <> handle_stack(boost::python::allow_null(stack));

			std::string message = boost::python::extract < std::string > (
				handle_value ? boost::python::str(handle_value) : boost::python::str(handle_error));

			return message;
		}

	} // namespace shared

} // namespace solution