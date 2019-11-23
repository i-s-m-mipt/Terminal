#ifndef SOLUTION_SYSTEM_MEMORY_HPP
#define SOLUTION_SYSTEM_MEMORY_HPP

#include <boost/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#  pragma once
#endif // #ifdef BOOST_HAS_PRAGMA_ONCE

#include <algorithm>
#include <exception>
#include <fstream>
#include <iterator>
#include <memory>
#include <mutex>
#include <numeric>
#include <shared_mutex>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>

#include <boost/extended/serialization/json.hpp>
#include <boost/filesystem/path.hpp>

#include "node/node.hpp"

#include "../../../shared/source/config/config.hpp"
#include "../../../shared/source/logger/logger.hpp"

namespace solution
{
	namespace system
	{
		class memory_exception : public std::exception
		{
		public:

			explicit memory_exception(const std::string & message) noexcept :
				std::exception(message.c_str())
			{}

			explicit memory_exception(const char * const message) noexcept :
				std::exception(message)
			{}

			~memory_exception() noexcept = default;
		};

		namespace memory
		{
			class View;
		}

		class Memory
		{
		private:

			using View = memory::View;
			using Node = memory::Node; 

			using nodes_container_t = 
				std::unordered_map < std::string, std::shared_ptr < Node > > ;

			using objects_container_t = Node::objects_container_t;

			using mutex_t = std::shared_mutex;

		public:

			using words_container_t = shared::Config::words_container_t;

			using data_t = shared::Object::data_t;

		private:

			friend class View;

		private:

			class Data
			{
			private:

				using json_t = boost::extended::serialization::json;

			private:

				struct File
				{
					using path_t = boost::filesystem::path;

					static inline const path_t nodes_data = "memory/data/nodes.data";
				};

			private:

				using path_t = File::path_t;

			private:

				struct Key
				{
					struct Node
					{
						static inline const std::string word = "word";
					};
				};

			public:

				static void load(	   nodes_container_t & actions);

				static void save(const nodes_container_t & actions);

			private:

				static void load(const path_t & path,	    json_t & object);

				static void save(const path_t & path, const json_t & object);
			};

		public:

			Memory()
			{
				initialize();
			}

			~Memory() noexcept
			{
				try
				{
					uninitialize();
				}
				catch (...)
				{
					// std::abort();
				}
			}

		private:

			void initialize();

			void uninitialize();

		private:

			void load();

			void save();

		private:

			void load_nodes();

			void save_nodes();

		public:

			void append_node(const std::string & word);

			void remove_node(const std::string & word);

		private:

			void get_implementation(const words_container_t & words,       data_t & data) const;
			
			void set_implementation(const words_container_t & words, const data_t & data);

		private:

			void check_and_append_nodes(const words_container_t & words);

			objects_container_t find_intersection(const words_container_t & words) const;

		private:

			nodes_container_t m_nodes;

		private:

			mutable mutex_t m_objects_mutex;
			mutable mutex_t m_lingual_mutex;
		};

	} // namespace system

} // namespace solution

#endif // #ifndef SOLUTION_SYSTEM_MEMORY_HPP