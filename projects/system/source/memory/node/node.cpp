#include "node.hpp"

namespace solution
{
	namespace system
	{
		namespace memory
		{
			void Node::append(std::shared_ptr < Object > object)
			{
				RUN_LOGGER(logger);

				try
				{
					auto result = m_objects.insert(object);

					if (!result.second)
					{
						throw node_exception("already existed");
					}
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < node_exception > (logger, exception);
				}
			}

			void Node::remove(std::shared_ptr < Object > object)
			{
				RUN_LOGGER(logger);

				try
				{
					auto iterator = m_objects.find(object);

					if (iterator != std::end(m_objects))
					{
						m_objects.erase(iterator);
					}
					else
					{
						throw node_exception("object not found");
					}
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < node_exception > (logger, exception);
				}
			}

		} // namespace memory

	} // namespace system

} // namespace solution