#ifndef SOLUTION_SHARED_MEMORY_VIEW_BASE_HPP
#define SOLUTION_SHARED_MEMORY_VIEW_BASE_HPP

#include <boost/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#  pragma once
#endif // #ifdef BOOST_HAS_PRAGMA_ONCE

#include <algorithm>
#include <cstdint>
#include <exception>
#include <iterator>
#include <memory>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

#include "../../config/config.hpp"
#include "../../logger/logger.hpp"
#include "../../object/object.hpp"

namespace solution
{
	namespace shared
	{
		namespace memory
		{
			class view_base_exception : public std::exception
			{
			public:

				explicit view_base_exception(const std::string & message) noexcept :
					std::exception(message.c_str())
				{}

				explicit view_base_exception(const char * const message) noexcept :
					std::exception(message)
				{}

				~view_base_exception() noexcept = default;
			};

			class View_Base
			{
			public:

				using words_container_t = Config::words_container_t;

				using words_union_t = std::set < std::string > ;

				using data_t = Object::data_t;

			public:

				enum class Mode
				{
					shared,
					unique
				};

			private:

				struct Status
				{
					using status_t = std::uint8_t;

					static inline const status_t released = 0;

					struct Acquired
					{
						static inline const status_t shared = 1;
						static inline const status_t unique = 2;
					};
				};

			private:

				using status_t = Status::status_t;

			private:

				class Lock
				{
				public:

					template < typename W, typename Enable = 
						std::enable_if_t < std::is_convertible_v < W, words_union_t > > >
					explicit Lock(const View_Base & view_base, Mode mode, W && words) :
						m_view_base(view_base), m_mode(mode), m_words(std::forward < W > (words))
					{
						initialize();
					}

					~Lock() noexcept
					{
						try
						{
							uninitialize();
						}
						catch (...)
						{
							// std::abort
						}
					}

				private:

					void initialize();

					void uninitialize();

				private:

					const View_Base & m_view_base;

				private:

					const Mode m_mode;

					const words_union_t m_words;

				private:

					mutable bool m_is_locked = false;
				};

			public:

				View_Base() noexcept = default;

				virtual ~View_Base() noexcept = default;

			public:

				template < typename ... Types >
				void get(const std::pair < words_container_t, Types > & ... pairs) const
				{
					RUN_LOGGER(logger);

					try
					{
						Lock lock(*this, Mode::shared, extract_words(pairs...));

						get_pairs(pairs...);
					}
					catch (const std::exception & exception)
					{
						shared::catch_handler < view_base_exception > (logger, exception);
					}
				}

				template < typename ... Types >
				void set(const std::pair < words_container_t, Types > & ... pairs) const
				{
					RUN_LOGGER(logger);
					
					try
					{
						Lock lock(*this, Mode::unique, extract_words(pairs...));
						
						set_pairs(pairs...);
					}
					catch (const std::exception & exception)
					{
						shared::catch_handler < view_base_exception > (logger, exception);
					}
				}

			private:
				
				template < typename Type, typename ... Types >
				words_union_t extract_words(
					const std::pair < words_container_t, Type  > &     pair, 
					const std::pair < words_container_t, Types > & ... pairs) const
				{
					RUN_LOGGER(logger);

					try
					{
						words_union_t words(std::begin(pair.first), std::end(pair.first));
						words_union_t right;

						if constexpr (sizeof...(pairs) > 0)
						{
							right = extract_words(pairs...);
						}

						words.insert(std::begin(right), std::end(right));

						return words;
					}
					catch (const std::exception & exception)
					{
						shared::catch_handler < view_base_exception > (logger, exception);
					}
				}

			private:

				void acquire(Mode mode, const words_union_t & words) const;

				void release(Mode mode, const words_union_t & words) const;

			private:

				template < typename Type, typename ... Types >
				void get_pairs(
					const std::pair < words_container_t, Type  > &     pair,
					const std::pair < words_container_t, Types > & ... pairs) const
				{
					RUN_LOGGER(logger);

					try
					{
						get < std::remove_reference_t < Type > > (pair.first, pair.second);

						if constexpr (sizeof...(pairs) > 0)
						{
							get_pairs(pairs...);
						}
					}
					catch (const std::exception & exception)
					{
						shared::catch_handler < view_base_exception > (logger, exception);
					}
				}
				
				template < typename Type, typename ... Types >
				void set_pairs(
					const std::pair < words_container_t, Type  > &     pair,
					const std::pair < words_container_t, Types > & ... pairs) const
				{
					RUN_LOGGER(logger);

					try
					{
						set < Type > (pair.first, pair.second);

						if constexpr (sizeof...(pairs) > 0)
						{
							set_pairs(pairs...);
						}
					}
					catch (const std::exception & exception)
					{
						shared::catch_handler < view_base_exception > (logger, exception);
					}
				}
				
			private:

				template < typename T >
				T get(const words_container_t & words) const
				{
					RUN_LOGGER(logger);

					try
					{
						T result;

						if (m_status == Status::Acquired::shared)
						{
							data_t data;

							get_implementation(words, data);

							std::stringstream sin(data);

							sin >> result;
						}
						else
						{
							throw view_base_exception("memory not acquired shared");
						}

						return result;
					}
					catch (const std::exception & exception)
					{
						catch_handler < view_base_exception > (logger, exception);
					}
				}

				template < typename T >
				void get(const words_container_t & words, T & result) const
				{
					RUN_LOGGER(logger);

					try
					{
						if (m_status == Status::Acquired::shared)
						{
							data_t data;

							get_implementation(words, data);

							std::stringstream sin(data);

							sin >> result;
						}
						else
						{
							throw view_base_exception("memory not acquired shared");
						}
					}
					catch (const std::exception & exception)
					{
						catch_handler < view_base_exception > (logger, exception);
					}
				}

				template < typename T >
				void set(const words_container_t & words, T data) const
				{
					RUN_LOGGER(logger);

					try
					{
						if (m_status == Status::Acquired::unique)
						{
							std::stringstream sout;

							sout << data;

							set_implementation(words, sout.str());
						}
						else
						{
							throw view_base_exception("memory not acquired unique");
						}
					}
					catch (const std::exception & exception)
					{
						catch_handler < view_base_exception > (logger, exception);
					}
				}

			private:

				virtual void acquire_implementation(Mode mode, const words_union_t & words) const = 0;

				virtual void release_implementation(Mode mode, const words_union_t & words) const = 0;

				virtual void get_implementation(const words_container_t & words,       data_t & data) const = 0;

				virtual void set_implementation(const words_container_t & words, const data_t & data) const = 0;

			private:

				mutable status_t m_status = Status::released;
			};

		} // namespace memory
		
	} // namespace shared

} // namespace solution

#endif // #ifndef SOLUTION_SHARED_MEMORY_VIEW_BASE_HPP