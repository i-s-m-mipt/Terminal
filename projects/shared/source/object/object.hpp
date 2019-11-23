#ifndef SOLUTION_SHARED_OBJECT_HPP
#define SOLUTION_SHARED_OBJECT_HPP

#include <boost/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#  pragma once
#endif // #ifdef BOOST_HAS_PRAGMA_ONCE

#include <exception>
#include <stdexcept>
#include <string>
#include <type_traits>

#include "../config/config.hpp"
#include "../logger/logger.hpp"

namespace solution
{
	namespace shared
	{
		class object_exception : public std::exception
		{
		public:

			explicit object_exception(const std::string & message) noexcept :
				std::exception(message.c_str())
			{}

			explicit object_exception(const char * const message) noexcept :
				std::exception(message)
			{}

			~object_exception() noexcept = default;
		};

		class Object
		{
		public:

			using data_t = std::string;

		public:

			template < typename D, typename Enable = 
				std::enable_if_t < std::is_convertible_v < D, data_t > > >
			explicit Object(D && data) : m_data(std::forward < D > (data))
			{}

			~Object() noexcept = default;

		public:

			const auto & data() const noexcept
			{
				return m_data;
			}

			void update(data_t data);

		private:

			data_t m_data;
		};
		
	} // namespace shared

} // namespace solution

#endif // #ifndef SOLUTION_SHARED_OBJECT_HPP