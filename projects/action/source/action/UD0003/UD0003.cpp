#include "UD0003.hpp"

namespace solution
{
	namespace action
	{
		namespace UD0003
		{
			Signal check_probability(const Probability & probability)
			{
				RUN_LOGGER(logger);

				try
				{
					if (probability.WW >= Probability::threshold)
					{
						return Signal::W;
					}

					if (probability.BO >= Probability::threshold ||
						probability.BC >= Probability::threshold)
					{
						return Signal::B;
					}

					if (probability.SO >= Probability::threshold ||
						probability.SC >= Probability::threshold)
					{
						return Signal::S;
					}

					return Signal::W;
				}
				catch (std::exception & exception)
				{
					shared::catch_handler < action_exception > (logger, exception);
				}
			}

			bool make_text(stream_t & text, const Probability & probability, const std::string & asset, const std::string & scale)
			{
				RUN_LOGGER(logger);

				try
				{
					text << sf::Text::Bold;

					bool flag = true;

					switch (check_probability(probability))
					{
					case Signal::B:
					{
						text << sf::Color::Green;

						break;
					}
					case Signal::S:
					{
						text << sf::Color::Red;

						break;
					}
					default:
					{
						text << sf::Color::White; 

						flag = false;
						
						break;
					}
					}

					std::stringstream sout;

					sout << std::left << std::setw(4) << std::setfill(' ') << asset;

					text << sout.str();

					sout.str(std::string());

					sout << " : " << std::left << std::setw(3) << std::setfill(' ') << scale << " | " <<
						std::setprecision(3) << std::fixed <<
						Key::BO << " = " << probability.BO << " | " <<
						Key::BC << " = " << probability.BC << " | " <<
						Key::SO << " = " << probability.SO << " | " <<
						Key::SC << " = " << probability.SC << " | " << 
						Key::WW << " = " << probability.WW << " | " << std::endl;

					text << sf::Text::Regular << sf::Color::White << sout.str();

					return flag;
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
					sf::VideoMode mode(700, 600);

					sf::RenderWindow window(mode, "Technical Analysis", sf::Style::Close | sf::Style::Titlebar);

					window.setFramerateLimit(60);

					sf::Font font;

					if (!font.loadFromFile("window/fonts/consolas.ttf"))
					{
						throw action_exception("cannot load font");
					}

					Probability probability;

					stream_t text(font);

					const auto size = 16;

					text.setCharacterSize(size);
					text.setPosition(size / 4, 0);
					
					Market market;

					std::size_t sound_flag		= 0;
					std::size_t sound_flag_last = 0;

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

						for (const auto & asset : market.assets())
						{
							for (const auto & scale : market.scales())
							{
								while (true)
								{
									try
									{
										memory->get(
											std::make_pair(words_container_t({ asset, scale, "probability", Key::BO }), std::ref(probability.BO)),
											std::make_pair(words_container_t({ asset, scale, "probability", Key::BC }), std::ref(probability.BC)),
											std::make_pair(words_container_t({ asset, scale, "probability", Key::SO }), std::ref(probability.SO)),
											std::make_pair(words_container_t({ asset, scale, "probability", Key::SC }), std::ref(probability.SC)),
											std::make_pair(words_container_t({ asset, scale, "probability", Key::WW }), std::ref(probability.WW))
										);
									}
									catch (...)
									{
										std::this_thread::sleep_for(std::chrono::seconds(5));

										continue;
									}

									break;
								}

								if (make_text(text, probability, asset, scale))
								{
									++sound_flag;
								}
							}
						}

						window.clear();

						if (sound_flag_last < sound_flag)
						{
							std::cout << "\a";
						}

						sound_flag_last = sound_flag;

						sound_flag = 0;

						window.draw(text);

						window.display();
					}
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < action_exception > (logger, exception);
				}
			}

		} // namespace UD0003

	} // namespace action

} // namespace solution