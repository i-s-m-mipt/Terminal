#include "action.hpp"

namespace solution
{
	namespace system
	{
		void Action::Task::operator()() const
		{
			RUN_LOGGER(logger);

			try
			{
				while (m_status.load() == Status::running)
				{
					try
					{
						m_function(m_argument);
					}
					catch (...)
					{
						++m_error_counter;

						std::this_thread::sleep_for(std::chrono::seconds(1));

						if (m_error_counter > critical_error_quantity)
						{
							throw action_exception("critical error quantity");
						}
						else
						{
							continue;
						}
					}

					m_error_counter = 0;

					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}
			}
			catch (const std::exception & exception)
			{
				m_status.store(Status::failure);

				shared::catch_handler < action_exception > (logger, exception);
			}
		}

		void Action::initialize()
		{
			RUN_LOGGER(logger);

			try
			{
				m_status.store(Status::stopped);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < action_exception > (logger, exception);
			}
		}

		void Action::uninitialize()
		{
			RUN_LOGGER(logger);

			try
			{
				stop();
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < action_exception > (logger, exception);
			}
		}

		void Action::run()
		{
			RUN_LOGGER(logger);

			try
			{
				if (!m_view)
				{
					throw action_exception("invalid view");
				}
				
				if (m_status.exchange(Status::running) != Status::running)
				{
					Task::argument_t argument = m_view;

					Task::function_t function = 
						boost::dll::import_alias < Task::callback_t > (m_path, m_name);

					m_future = std::async(std::launch::async, Task(argument, function, m_status));
				}
			}
			catch (const std::exception & exception)
			{
				m_status.store(Status::failure);

				shared::catch_handler < action_exception > (logger, exception);
			}
		}

		void Action::stop()
		{
			RUN_LOGGER(logger);

			try
			{
				m_status.store(Status::stopped);

				if (m_future.valid())
				{
					m_future.wait();
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < action_exception > (logger, exception);
			}
		}

	} // namespace system

} // namespace solution