#include "quotes.hpp"

namespace solution
{
	namespace plugin
	{
		namespace market
		{
			using Severity = shared::Logger::Severity;

			void Quotes::initialize()
			{
				RUN_LOGGER(logger);

				try
				{
					if (!m_api.Subscribe_Level_II_Quotes(m_class_code.c_str(), m_asset_code.c_str()))
					{
						throw quotes_exception("cannot subscribe to " + m_class_code + " : " + m_asset_code);
					}

					const auto shared_memory_name = make_shared_memory_name();

					const auto shared_memory_size = 1024U;

					boost::interprocess::shared_memory_object::remove(shared_memory_name.c_str());

					m_shared_memory = shared_memory_t(boost::interprocess::create_only,
						shared_memory_name.c_str(), shared_memory_size);

					m_mutex = m_shared_memory.construct < mutex_t > (boost::interprocess::unique_instance) ();

					m_total_ask = m_shared_memory.construct < volume_t > (Name::total_ask.c_str())();
					m_total_bid = m_shared_memory.construct < volume_t > (Name::total_bid.c_str())();

					m_asks.resize(depth);
					m_bids.resize(depth);

					reset_orders();

					boost::filesystem::create_directory(directory);

					path_t file = m_asset_code + Extension::txt;

					(m_path = directory) /= file;
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < quotes_exception > (logger, exception);
				}
			}

			void Quotes::uninitialize()
			{
				RUN_LOGGER(logger);

				try
				{
					m_shared_memory.destroy_ptr(m_mutex);

					m_shared_memory.destroy_ptr(m_total_ask);
					m_shared_memory.destroy_ptr(m_total_bid);

					const auto shared_memory_name = make_shared_memory_name();

					boost::interprocess::shared_memory_object::remove(shared_memory_name.c_str());
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < quotes_exception > (logger, exception);
				}
			}

			std::string Quotes::make_shared_memory_name() const
			{
				RUN_LOGGER(logger);

				try
				{
					return "QUIK_" + m_asset_code + "_quotes";
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < quotes_exception > (logger, exception);
				}
			}

			void Quotes::update()
			{
				RUN_LOGGER(logger);

				try
				{
					reset_orders();

					try
					{
						m_api.getQuoteLevel2(m_class_code.c_str(), m_asset_code.c_str(), [this](const auto & quotes)
						{
							auto bid = quotes.bid();
							auto ask = quotes.offer();

							volume_t total_ask = 0;
							volume_t total_bid = 0;

							for (std::size_t i = 0; i < std::min(depth, ask.size()); ++i)
							{
								auto & ask_ref = ask[i];

								m_asks.at(i) = std::make_pair(
									atof(ask_ref.price.c_str()), atoi(ask_ref.quantity.c_str()));

								total_ask += m_asks.at(i).second;
							}

							for (std::size_t i = 0; i < std::min(depth, bid.size()); ++i)
							{
								auto & bid_ref = bid[bid.size() - 1 - i];

								m_bids.at(i) = std::make_pair(
									atof(bid_ref.price.c_str()), atoi(bid_ref.quantity.c_str()));

								total_bid += m_bids.at(i).second;
							}

							boost::interprocess::scoped_lock < mutex_t > lock(*m_mutex);

							*m_total_ask = total_ask;
							*m_total_bid = total_bid;
						});
					}
					catch (...)
					{
						logger.write(Severity::error, m_asset_code + " quotes update failed");
					}

					save();
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < quotes_exception > (logger, exception);
				}
			}

			void Quotes::reset_orders()
			{
				RUN_LOGGER(logger);

				try
				{
					for (auto & order : m_asks)
					{
						order = std::make_pair(0.0, 0);
					}

					for (auto & order : m_bids)
					{
						order = std::make_pair(0.0, 0);
					}
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < quotes_exception > (logger, exception);
				}
			}

			void Quotes::save() const
			{
				RUN_LOGGER(logger);

				try
				{
					auto time = std::time(nullptr);

					if (time > m_last_time)
					{
						std::fstream fout(m_path.string(), std::ios::app);

						if (!fout)
						{
							throw quotes_exception("cannot open file " + m_path.string());
						}

						tm * time_info = std::localtime(&time); // !

						static const auto delimeter = ',';

						fout <<
							std::setfill('0') << std::setw(4) << time_info->tm_year + 1900 <<
							std::setfill('0') << std::setw(2) << time_info->tm_mon + 1 <<
							std::setfill('0') << std::setw(2) << time_info->tm_mday << delimeter;

						fout << 
							std::setfill('0') << std::setw(2) << time_info->tm_hour <<
							std::setfill('0') << std::setw(2) << time_info->tm_min <<
							std::setfill('0') << std::setw(2) << time_info->tm_sec << delimeter;
						
						for (std::size_t i = 0; i < depth; ++i)
						{
							fout << 
								m_asks.at(depth - 1 - i).first  << delimeter << 
								m_asks.at(depth - 1 - i).second << delimeter;
						}

						for (std::size_t i = 0; i < depth; ++i)
						{
							fout << m_bids.at(i).first << delimeter << m_bids.at(i).second;

							if (i != depth - 1)
							{
								fout << delimeter;
							}
						}

						fout << std::endl;
						
						m_last_time = time;
					}
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < quotes_exception > (logger, exception);
				}
			}

		} // namespace market

	} // namespace plugin

} // namespace solution