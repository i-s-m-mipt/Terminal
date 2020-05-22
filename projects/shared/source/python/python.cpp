#include "python.hpp"

namespace solution
{
	namespace shared
	{
		boost::python::api::object_attribute & Python::global()
		{
			if (!Py_IsInitialized())
			{
				Py_SetPythonHome(directory);

				Py_Initialize();
			}

			static auto global = boost::python::import("__main__").attr("__dict__");

			return global;
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
				!handle_value ? boost::python::str(handle_error) : boost::python::str(handle_value));

			return message;
		}

	} // namespace shared

} // namespace solution