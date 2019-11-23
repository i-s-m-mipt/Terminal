#include "memory.hpp"

namespace solution
{
	namespace system
	{
		void Memory::Data::load(      nodes_container_t & nodes)
		{
			RUN_LOGGER(logger);

			try
			{
				json_t array;

				load(File::nodes_data, array);

				for (const auto & element : array)
				{
					auto word = element[Key::Node::word].get < std::string > ();

					auto node = std::make_shared < Node > (std::move(word));

					nodes[node->word()] = node;
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < memory_exception > (logger, exception);
			}
		}

		void Memory::Data::save(const nodes_container_t & nodes)
		{
			RUN_LOGGER(logger);

			try
			{
				auto array = json_t::array();

				for (const auto & node : nodes)
				{
					json_t element;

					element[Key::Node::word] = node.second->word();

					array.push_back(element);
				}

				save(File::nodes_data, array);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < memory_exception > (logger, exception);
			}
		}

		void Memory::Data::load(const path_t & path,       json_t & object)
		{
			RUN_LOGGER(logger);

			try
			{
				std::fstream fin(path.string(), std::ios::in);

				if (!fin)
				{
					throw memory_exception("cannot open file " + path.string());
				}

				object = json_t::parse(fin);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < memory_exception > (logger, exception);
			}
		}

		void Memory::Data::save(const path_t & path, const json_t & object)
		{
			RUN_LOGGER(logger);

			try
			{
				std::fstream fout(path.string(), std::ios::out);

				if (!fout)
				{
					throw memory_exception("cannot open file " + path.string());
				}

				fout << std::setw(4) << object;
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < memory_exception > (logger, exception);
			}
		}

		void Memory::initialize()
		{
			RUN_LOGGER(logger);

			try
			{
				load();
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < memory_exception > (logger, exception);
			}
		}

		void Memory::uninitialize()
		{
			RUN_LOGGER(logger);

			try
			{
				save();
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < memory_exception > (logger, exception);
			}
		}

		void Memory::load()
		{
			RUN_LOGGER(logger);

			try
			{
				load_nodes();
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < memory_exception > (logger, exception);
			}
		}

		void Memory::save()
		{
			RUN_LOGGER(logger);
			try
			{
				save_nodes();
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < memory_exception > (logger, exception);
			}
		}

		void Memory::load_nodes()
		{
			RUN_LOGGER(logger);

			try
			{
				Data::load(m_nodes);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < memory_exception >(logger, exception);
			}
		}

		void Memory::save_nodes()
		{
			RUN_LOGGER(logger);

			try
			{
				Data::save(m_nodes);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < memory_exception > (logger, exception);
			}
		}

		void Memory::append_node(const std::string & word)
		{
			RUN_LOGGER(logger);

			try
			{
				std::scoped_lock < mutex_t > lock(m_lingual_mutex);

				if (m_nodes.count(word) != 0)
				{
					throw memory_exception("node " + word + " already existed");
				}

				m_nodes[word] = std::make_shared < Node > (word);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < memory_exception > (logger, exception);
			}
		}

		void Memory::remove_node(const std::string & word)
		{
			RUN_LOGGER(logger);

			try
			{
				std::scoped_lock < mutex_t, mutex_t > lock(m_objects_mutex, m_lingual_mutex);

				m_nodes.erase(word);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < memory_exception > (logger, exception);
			}
		}

		void Memory::get_implementation(const words_container_t & words,       data_t & data) const
		{
			RUN_LOGGER(logger);

			try
			{
				switch (words.size())
				{
				case 0:
				{
					throw memory_exception("empty words container");

					break;
				}
				case 1:
				{
					auto node = m_nodes.at(words.at(0));

					switch (node->objects().size())
					{
					case 0:
					{
						throw memory_exception("object not found");

						break;
					}
					case 1:
					{
						data = (*(std::begin(node->objects())))->data();

						break;
					}
					default:
					{
						throw memory_exception("ambiguous object choice");

						break;
					}
					}
					break;
				}
				default:
				{
					objects_container_t intersection = find_intersection(words);

					switch (intersection.size())
					{
					case 0:
					{
						throw memory_exception("empty intersection");

						break;
					}
					case 1:
					{
						data = (*(std::begin(intersection)))->data();

						break;
					}
					default:
					{
						throw memory_exception("ambiguous object choice in intersection");

						break;
					}
					}
					break;
				}
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < memory_exception > (logger, exception);
			}
		}

		void Memory::set_implementation(const words_container_t & words, const data_t & data)
		{
			RUN_LOGGER(logger);

			try
			{
				switch (words.size())
				{
				case 0:
				{
					throw memory_exception("empty words container");

					break;
				}
				case 1:
				{
					auto node = m_nodes.at(words.at(0));

					switch (node->objects().size())
					{
					case 0:
					{
						node->append(std::make_shared < Node::Object > (data));

						break;
					}
					case 1:
					{
						(*(std::begin(node->objects())))->update(data);

						break;
					}
					default:
					{
						throw memory_exception("ambiguous object choice");

						break;
					}
					}
					break;
				}
				default:
				{
					objects_container_t intersection = find_intersection(words);

					switch (intersection.size())
					{
					case 0:
					{
						auto object = std::make_shared < Node::Object > (data);

						for (const auto & word : words)
						{
							m_nodes.at(word)->append(object);
						}

						break;
					}
					case 1:
					{
						(*(std::begin(intersection)))->update(data);

						break;
					}
					default:
					{
						throw memory_exception("ambiguous object choice in intersection");

						break;
					}
					}
					break;
				}
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < memory_exception > (logger, exception);
			}
		}

		void Memory::check_and_append_nodes(const words_container_t & words)
		{
			RUN_LOGGER(logger);

			try
			{
				for (const auto & word : words)
				{
					if (!m_nodes.count(word))
					{
						append_node(word);
					}
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < memory_exception > (logger, exception);
			}
		}

		Memory::objects_container_t Memory::find_intersection(const words_container_t & words) const
		{
			RUN_LOGGER(logger);

			try
			{
				auto basic_word = std::min_element(std::begin(words), std::end(words),
					[this](const auto & lhs_word, const auto & rhs_word)
				{
					return (m_nodes.at(lhs_word)->objects().size() < m_nodes.at(rhs_word)->objects().size());
				});

				auto basic_node = m_nodes.at(*basic_word);

				auto flag = true;

				objects_container_t intersection;

				for (const auto & object : basic_node->objects())
				{
					flag = true;

					for (const auto & word : words)
					{
						if (!(m_nodes.at(word)->objects().count(object)))
						{
							flag = false;

							break;
						}
					}

					if (flag)
					{
						intersection.insert(object);
					}
				}

				return intersection;
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < memory_exception > (logger, exception);
			}
		}

	} // namespace system

} // namespace solution