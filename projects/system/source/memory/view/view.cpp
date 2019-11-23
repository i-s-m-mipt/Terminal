#include "view.hpp"

namespace solution
{
	namespace system
	{
		namespace memory
		{
			void View::acquire_implementation(Mode mode, const words_union_t & words) const
			{
				RUN_LOGGER(logger);

				try
				{
					switch (mode)
					{
					case Mode::shared:
					{
						for (const auto & word : words)
						{
							m_memory->m_nodes.at(word)->objects_mutex().lock_shared();
						}

						break;
					}
					case Mode::unique:
					{
						for (const auto & word : words)
						{
							m_memory->m_nodes.at(word)->objects_mutex().lock();
						}

						break;
					}
					}
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < view_exception > (logger, exception);
				}
			}

			void View::release_implementation(Mode mode, const words_union_t & words) const
			{
				RUN_LOGGER(logger);

				try
				{
					switch (mode)
					{
					case Mode::shared:
					{
						for (const auto & word : words)
						{
							m_memory->m_nodes.at(word)->objects_mutex().unlock_shared();
						}

						break;
					}
					case Mode::unique:
					{
						for (const auto & word : words)
						{
							m_memory->m_nodes.at(word)->objects_mutex().unlock();
						}

						break;
					}
					}
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < view_exception > (logger, exception);
				}
			}

			void View::get_implementation(const words_container_t & words, data_t & data) const
			{
				RUN_LOGGER(logger);

				try
				{
					m_memory->get_implementation(words, data);
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < view_exception > (logger, exception);
				}
			}

			void View::set_implementation(const words_container_t & words, const data_t & data) const
			{
				RUN_LOGGER(logger);

				try
				{
					m_memory->set_implementation(words, data);
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < view_exception > (logger, exception);
				}
			}

		} // namespace memory

	} // namespace system

} // namespace solution