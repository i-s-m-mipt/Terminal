#include "view_base.hpp"

namespace solution
{
	namespace shared
	{
		namespace memory
		{
			void View_Base::Lock::initialize()
			{
				RUN_LOGGER(logger);

				try
				{
					m_view_base.acquire(m_mode, m_words);

					m_is_locked = true;
				}
				catch (const std::exception & exception)
				{
					catch_handler < view_base_exception > (logger, exception);
				}
			}

			void View_Base::Lock::uninitialize()
			{
				RUN_LOGGER(logger);

				try
				{
					m_view_base.release(m_mode, m_words);

					m_is_locked = false;
				}
				catch (const std::exception & exception)
				{
					catch_handler < view_base_exception > (logger, exception);
				}
			}

			void View_Base::acquire(Mode mode, const words_union_t & words) const
			{
				RUN_LOGGER(logger);

				try
				{
					if (m_status == Status::Acquired::shared)
					{
						throw view_base_exception("already acquired shared");
					}

					if (m_status == Status::Acquired::unique)
					{
						throw view_base_exception("already acquired unique");
					}

					acquire_implementation(mode, words);

					switch (mode)
					{
					case Mode::shared:
					{
						m_status = Status::Acquired::shared;

						break;
					}
					case Mode::unique:
					{
						m_status = Status::Acquired::unique;

						break;
					}
					}
				}
				catch (const std::exception & exception)
				{
					catch_handler < view_base_exception > (logger, exception);
				}
			}

			void View_Base::release(Mode mode, const words_union_t & words) const
			{
				RUN_LOGGER(logger);

				try
				{
					if (mode == Mode::shared && (m_status != Status::Acquired::shared))
					{
						throw view_base_exception("not acquired shared");
					}

					if (mode == Mode::unique && (m_status != Status::Acquired::unique))
					{
						throw view_base_exception("not acquired unique");
					}

					release_implementation(mode, words);

					m_status = Status::released;
				}
				catch (const std::exception & exception)
				{
					catch_handler < view_base_exception > (logger, exception);
				}
			}

		} // namespace memory

	} // namespace shared

} // namespace solution