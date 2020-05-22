#include "system.hpp"

namespace solution
{
	namespace system
	{
		using Severity = shared::Logger::Severity;

		void System::Data::load(      actions_container_t & actions)
		{
			RUN_LOGGER(logger);

			try
			{
				json_t array;
				
				load(File::actions_data, array);

				for (const auto & element : array)
				{
					auto path = element[Key::Action::path].get < std::string > ();
					auto name = element[Key::Action::name].get < Action::name_t > ();

					auto action = std::make_shared < Action > (std::move(path), std::move(name));

					actions[action->name()] = action;
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::Data::save(const actions_container_t & actions)
		{
			RUN_LOGGER(logger);

			try
			{
				auto array = json_t::array();

				for (const auto & action : actions)
				{
					json_t element;

					element[Key::Action::path] = action.second->path().string();
					element[Key::Action::name] = action.second->name();

					array.push_back(element);
				}

				save(File::actions_data, array);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::Data::load(const path_t & path,       json_t & object)
		{
			RUN_LOGGER(logger);

			try
			{
				std::fstream fin(path.string(), std::ios::in);

				if (!fin)
				{
					throw system_exception("cannot open file " + path.string());
				}

				object = json_t::parse(fin);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::Data::save(const path_t & path, const json_t & object)
		{
			RUN_LOGGER(logger);

			try
			{
				std::fstream fout(path.string(), std::ios::out);

				if (!fout)
				{
					throw system_exception("cannot open file " + path.string());
				}

				fout << std::setw(4) << object;
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::initialize()
		{
			RUN_LOGGER(logger);

			try
			{
				load();
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::uninitialize()
		{
			RUN_LOGGER(logger);

			try
			{
				stop_all_actions();

				save();
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::load()
		{
			RUN_LOGGER(logger);

			try
			{
				load_actions();
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::save()
		{
			RUN_LOGGER(logger);

			try
			{
				save_actions();
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::load_actions()
		{
			RUN_LOGGER(logger);

			try
			{
				Data::load(m_actions);

				for (const auto & action : m_actions)
				{
					action.second->set(make_view());
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::save_actions()
		{
			RUN_LOGGER(logger);

			try
			{
				Data::save(m_actions);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::run()
		{
			RUN_LOGGER(logger);

			try
			{
				std::cout << "Run system ? (y/n) ";

				if (getchar() == 'y')
				{
					run_all_actions();
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::stop()
		{
			RUN_LOGGER(logger);

			try
			{
				stop_all_actions();
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::append_action(const Action::path_t & path, const Action::name_t & name)
		{
			RUN_LOGGER(logger);

			try
			{
				if (m_actions.count(name) != 0)
				{
					throw action_exception("action " + name + " already existed");
				}

				auto action = std::make_shared < Action > (path, name);

				action->set(make_view());

				m_actions[action->name()] = action;
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::remove_action(const name_t & name)
		{
			RUN_LOGGER(logger);

			try
			{
				stop(name);

				m_actions.erase(name);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		std::shared_ptr < System::View > System::make_view() const
		{
			RUN_LOGGER(logger);

			try
			{
				return std::make_shared < View > (m_memory);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::run_all_actions() const
		{
			RUN_LOGGER(logger);

			try
			{
				for (const auto & action : m_actions)
				{
					run(action.first);
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::run(const name_t & name) const
		{
			RUN_LOGGER(logger);

			try
			{
				m_actions.at(name)->run();
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::stop_all_actions() const
		{
			RUN_LOGGER(logger);

			try
			{
				for (const auto & action : m_actions)
				{
					logger.write(Severity::trace, "stopping " + action.first);

					stop(action.first);

					logger.write(Severity::trace, action.first + " stopped");
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::stop(const name_t & name) const
		{
			RUN_LOGGER(logger);

			try
			{
				m_actions.at(name)->stop();
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

	} // namespace system

} // namespace solution