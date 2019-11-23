#include "UD0001.hpp"

namespace solution
{
	namespace action
	{
		namespace UD0001
		{
			void run(std::shared_ptr < View > memory)
			{
				RUN_LOGGER(logger);

				try
				{
					static Market market;

					// ================================================================================================

					static Python_Initializer python_initializer;

					static boost::python::object global = boost::python::import("__main__").attr("__dict__");

					static boost::python::object result = 
						boost::python::exec("from analysis.src.main import run", global, global);

					static boost::python::object module = global["run"];

					// ================================================================================================
					
					for (const auto & asset : market.assets())
					{
						for (const auto & scale : market.scales())
						{
							auto file = market.get(asset, scale, 250);

							std::string message = boost::python::extract < std::string > (
								module(asset.c_str(), scale.c_str(), file.string().c_str()));

							auto object = boost::extended::serialization::json::parse(std::move(message));

							memory->set(
								std::make_pair(words_container_t({ asset, scale, "probability", Key::BO }), object[Key::BO].get < double > ()),
								std::make_pair(words_container_t({ asset, scale, "probability", Key::BC }), object[Key::BC].get < double > ()),
								std::make_pair(words_container_t({ asset, scale, "probability", Key::SO }), object[Key::SO].get < double > ()),
								std::make_pair(words_container_t({ asset, scale, "probability", Key::SC }), object[Key::SC].get < double > ()),
								std::make_pair(words_container_t({ asset, scale, "probability", Key::WW }), object[Key::WW].get < double > ())
							);
						}

						auto quotes = market.get(asset);

						memory->set(
							std::make_pair(words_container_t({ asset, "total", "ask" }), quotes.total_ask),
							std::make_pair(words_container_t({ asset, "total", "bid" }), quotes.total_bid)
						);
					}
				}
				catch (const boost::python::error_already_set &)
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

					logger.write(Severity::error, message);
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < action_exception > (logger, exception);
				}
			}

		} // namespace UD0001

	} // namespace action

} // namespace solution