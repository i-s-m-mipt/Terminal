#ifndef SOLUTION_SHARED_CONFIG_HPP
#define SOLUTION_SHARED_CONFIG_HPP

#include <boost/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#  pragma once
#endif // #ifdef BOOST_HAS_PRAGMA_ONCE

#include <string>
#include <vector>

namespace solution
{
	namespace shared
	{
		struct Config
		{
			using words_container_t = std::vector < std::string > ;
		};

	} // namespace shared

} // namespace solution

#endif // #ifndef SOLUTION_SHARED_CONFIG_HPP