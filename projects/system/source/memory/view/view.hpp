#ifndef SOLUTION_SYSTEM_MEMORY_VIEW_HPP
#define SOLUTION_SYSTEM_MEMORY_VIEW_HPP

#include <boost/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#  pragma once
#endif // #ifdef BOOST_HAS_PRAGMA_ONCE

#include <exception>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>

#include "../memory.hpp"

#include "../../../../shared/source/logger/logger.hpp"
#include "../../../../shared/source/memory/view/view_base.hpp"

namespace solution
{
	namespace system
	{
		class Memory;

		namespace memory
		{
			class view_exception : public shared::memory::view_base_exception
			{
			public:

				explicit view_exception(const std::string & message) noexcept :
					shared::memory::view_base_exception(message.c_str())
				{}

				explicit view_exception(const char * const message) noexcept :
					shared::memory::view_base_exception(message)
				{}

				~view_exception() noexcept = default;
			};

			class View : public shared::memory::View_Base
			{
			private:

				static_assert(std::is_convertible_v < words_container_t, Memory::words_container_t >,
					"View::words_container_t is not convertible to Memory::words_container_t");

				static_assert(std::is_convertible_v < Memory::data_t, data_t >,
					"Memory::data_t is not convertible to View::data_t");

			public:

				explicit View(std::shared_ptr < Memory > memory) noexcept : m_memory(memory)
				{}

				~View() noexcept = default;

			private:

				virtual void acquire_implementation(Mode mode, const words_union_t & words) const override;

				virtual void release_implementation(Mode mode, const words_union_t & words) const override;

				virtual void get_implementation(const words_container_t & words,       data_t & data) const override;

				virtual void set_implementation(const words_container_t & words, const data_t & data) const override;

			private:

				std::shared_ptr < Memory > m_memory;
			};

		} // namespace memory

	} // namespace system

} // namespace solution

#endif // #ifndef SOLUTION_SYSTEM_MEMORY_VIEW_HPP