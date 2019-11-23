#include "object.hpp"

namespace solution
{
	namespace shared
	{
		void Object::update(data_t data)
		{
			RUN_LOGGER(logger);

			try
			{
				m_data = std::move(data);
			}
			catch (const std::exception & exception)
			{
				catch_handler < object_exception > (logger, exception);
			}
		}

	} // namespace shared

} // namespace solution