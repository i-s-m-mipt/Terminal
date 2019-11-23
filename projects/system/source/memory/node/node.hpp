#ifndef SOLUTION_SYSTEM_MEMORY_NODE_HPP
#define SOLUTION_SYSTEM_MEMORY_NODE_HPP

#include <boost/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#  pragma once
#endif // #ifdef BOOST_HAS_PRAGMA_ONCE

#include <exception>
#include <memory>
#include <shared_mutex>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_set>

#include "../../../../shared/source/config/config.hpp"
#include "../../../../shared/source/logger/logger.hpp"
#include "../../../../shared/source/object/object.hpp"

namespace solution
{
	namespace system
	{
		namespace memory
		{
			class node_exception : public std::exception
			{
			public:

				explicit node_exception(const std::string & message) noexcept :
					std::exception(message.c_str())
				{}

				explicit node_exception(const char * const message) noexcept :
					std::exception(message)
				{}

				~node_exception() noexcept = default;
			};

			class Node
			{
			public:

				using Object = shared::Object;

				using objects_container_t = 
					std::unordered_set < std::shared_ptr < Object > > ;

				using mutex_t = std::shared_mutex;

			public:

				template < typename W, typename Enable =
					std::enable_if_t < std::is_convertible_v < W, std::string > > >
				explicit Node(W && word) : m_word(std::forward < W > (word))
				{}

				~Node() noexcept = default;

			public:

				const auto & word() const noexcept
				{
					return m_word;
				}

				const auto & objects() const noexcept
				{
					return m_objects;
				}

				auto & objects_mutex() const noexcept
				{
					return m_objects_mutex;
				}

			public:

				void append(std::shared_ptr < Object > object);

				void remove(std::shared_ptr < Object > object);

			private:

				const std::string m_word;

			private:

				objects_container_t m_objects;

			private:

				mutable mutex_t m_objects_mutex;
			};

		} // namespace memory

	} // namespace system

} // namespace solution

#endif // #ifndef SOLUTION_SYSTEM_MEMORY_NODE_HPP