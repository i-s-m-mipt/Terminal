#include "source.hpp"

namespace solution
{
	namespace plugin
	{
		namespace market
		{
			void Source::initialize(const api_t & api)
			{
				RUN_LOGGER(logger);

				try
				{
					const auto timeframe = api.constant < unsigned int > (("INTERVAL_" + m_scale_code).c_str());

					m_source = std::make_unique < source_t > (
						api.CreateDataSource(m_class_code.c_str(), m_asset_code.c_str(), timeframe));

					const auto shared_memory_name = make_shared_memory_name();

					const auto shared_memory_size = 65536U;

					boost::interprocess::shared_memory_object::remove(shared_memory_name.c_str());

					m_shared_memory = shared_memory_t(boost::interprocess::create_only, 
						shared_memory_name.c_str(), shared_memory_size);

					record_allocator_t allocator(m_shared_memory.get_segment_manager());

					m_deque = m_shared_memory.construct < deque_t > (boost::interprocess::unique_instance) (allocator);
					m_price = m_shared_memory.construct < price_t > (boost::interprocess::unique_instance) ();
					m_mutex = m_shared_memory.construct < mutex_t > (boost::interprocess::unique_instance) ();
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < source_exception > (logger, exception);
				}
			}

			void Source::uninitialize()
			{
				RUN_LOGGER(logger);

				try
				{
					m_shared_memory.destroy_ptr(m_deque);
					m_shared_memory.destroy_ptr(m_mutex);

					const auto shared_memory_name = make_shared_memory_name();

					boost::interprocess::shared_memory_object::remove(shared_memory_name.c_str());
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < source_exception > (logger, exception);
				}
			}

			std::string Source::make_shared_memory_name() const
			{
				RUN_LOGGER(logger);

				try
				{
					return "QUIK_" + m_asset_code + "_" + m_scale_code;
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < source_exception > (logger, exception);
				}
			}

			void Source::update() const
			{
				RUN_LOGGER(logger);

				try
				{
					const auto size = m_source->Size();

					{
						boost::interprocess::scoped_lock < mutex_t > lock(*m_mutex);

						*m_price = make_candle(size).price_close;
					}

					if (size >= m_size)
					{
						if (m_deque->size() == 0)
						{
							boost::interprocess::scoped_lock < mutex_t > lock(*m_mutex);

							for (index_t index = size - m_size + 1; index <= size; ++index)
							{
								m_deque->push_back(make_record(make_candle(index)));

								m_last_index = index;
							}
						}

						if (m_last_index < size)
						{
							auto delta = std::min(size - m_last_index, m_size);

							boost::interprocess::scoped_lock < mutex_t > lock(*m_mutex);

							for (index_t index = std::max(size - m_size + 1, m_last_index + 1); index <= size; ++index)
							{
								m_deque->push_back(make_record(make_candle(index)));

								m_last_index = index;
							}

							for (index_t index = 0; index < delta; ++index)
							{
								m_deque->pop_front();
							}
						}
						else
						{
							boost::interprocess::scoped_lock < mutex_t > lock(*m_mutex);

							m_deque->pop_back();

							m_deque->push_back(make_record(make_candle(size)));
						}
					}
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < source_exception > (logger, exception);
				}
			}

			Source::Candle Source::make_candle(index_t index) const
			{
				RUN_LOGGER(logger);

				try
				{
					Candle candle;

					candle.index = index;

					std::stringstream stream;

					auto time = m_source->T(index);

					stream <<
						std::setfill('0') << std::setw(4) << time.year  <<
						std::setfill('0') << std::setw(2) << time.month <<
						std::setfill('0') << std::setw(2) << time.day;
						
					stream >> candle.date;

					stream.clear();

					stream <<
						std::setfill('0') << std::setw(2) << time.hour <<
						std::setfill('0') << std::setw(2) << time.min  <<
						std::setfill('0') << std::setw(2) << time.sec;

					stream >> candle.time;

					candle.price_open  = m_source->O(index);
					candle.price_high  = m_source->H(index);
					candle.price_low   = m_source->L(index);
					candle.price_close = m_source->C(index);

					candle.volume = static_cast < Candle::volume_t > (m_source->V(index));

					return candle;
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < source_exception > (logger, exception);
				}
			}

			Source::record_t Source::make_record(const Candle & candle) const
			{
				RUN_LOGGER(logger);

				try
				{
					static const char delimeter = ',';

					std::stringstream sout;

					sout << std::setprecision(6) << std::fixed <<
						candle.date        << delimeter <<
						candle.time        << delimeter <<
						candle.price_open  << delimeter <<
						candle.price_high  << delimeter <<
						candle.price_low   << delimeter <<
						candle.price_close << delimeter <<
						candle.volume;

					return record_t(sout.str().c_str(), m_shared_memory.get_segment_manager());
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < source_exception > (logger, exception);
				}
			}

		} // namespace market

	} // namespace plugin

} // namespace solution