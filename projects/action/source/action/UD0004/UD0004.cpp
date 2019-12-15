#include "UD0004.hpp"

namespace solution
{
	namespace action
	{
		namespace UD0004
		{
			bool make_text(stream_t & text, const Quotes & quotes, const std::string & asset)
			{
				RUN_LOGGER(logger);

				try
				{
					std::stringstream sout;

					sout << std::left << std::setw(4) << std::setfill(' ') << asset;

					text << sf::Text::Bold << sf::Color::White << sout.str() << sf::Text::Regular << " ";

					sout.str(std::string());

					if (1.0 * quotes.total_ask / std::max(quotes.total_bid, Quotes::volume_t(1)) - 1.0 > Quotes::threshold)
					{
						sout << std::right << std::setw(8) << std::setfill(' ') << quotes.total_ask;

						text << sf::Color::Red << sout.str();

						sout.str(std::string());

						sout << std::right << std::setw(8) << std::setfill(' ') << quotes.total_bid;
						
						text << sf::Color::White << " " << sout.str() << "\n";
					}
					else
					if (1.0 * quotes.total_bid / std::max(quotes.total_ask, Quotes::volume_t(1)) - 1.0 > Quotes::threshold)
					{
						sout << std::right << std::setw(8) << std::setfill(' ') << quotes.total_ask;

						text << sf::Color::White << sout.str() << " ";

						sout.str(std::string());
						
						sout << std::right << std::setw(8) << std::setfill(' ') << quotes.total_bid;
						
						text << sf::Color::Green << sout.str() << sf::Color::White << "\n";
					}
					else
					{
						sout <<
							std::right << std::setw(8) << std::setfill(' ') << quotes.total_ask << " " <<
							std::right << std::setw(8) << std::setfill(' ') << quotes.total_bid;

						text << sf::Color::White << sout.str() << "\n";
					}
				}
				catch (std::exception & exception)
				{
					shared::catch_handler < action_exception > (logger, exception);
				}
			}

			void run(std::shared_ptr < View > memory)
			{
				RUN_LOGGER(logger);

				try
				{
					sf::VideoMode mode(260, 400);

					sf::RenderWindow window(mode, "Quotes Analysis", sf::Style::Close | sf::Style::Titlebar);

					window.setFramerateLimit(60);

					sf::Font font;

					if (!font.loadFromFile("window/fonts/consolas.ttf"))
					{
						throw action_exception("cannot load font");
					}
					
					stream_t text(font);

					const auto size = 16;

					text.setCharacterSize(size);
					text.setPosition(size / 4, 0);

					Market market;
					
					while (window.isOpen())
					{
						sf::Event event;

						while (window.pollEvent(event))
						{
							if (event.type == sf::Event::Closed)
							{
								window.close();
							}
						}

						text.clear();
						
						Quotes quotes;

						for (const auto & asset : market.assets())
						{
							while (true)
							{
								try
								{
									memory->get(
										std::make_pair(words_container_t({ asset, "total", "ask" }), std::ref(quotes.total_ask)),
										std::make_pair(words_container_t({ asset, "total", "bid" }), std::ref(quotes.total_bid))
									);
								}
								catch (...)
								{
									std::this_thread::sleep_for(std::chrono::seconds(5));

									continue;
								}

								break;
							}

							make_text(text, quotes, asset);
						}

						window.clear();

						window.draw(text);

						window.display();
					}
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < action_exception > (logger, exception);
				}
			}

		} // namespace UD0004

	} // namespace action

} // namespace solution