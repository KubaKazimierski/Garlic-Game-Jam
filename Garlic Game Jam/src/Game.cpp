/*
MIT License

Copyright (c) 2019 Jakub Kazimierski

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once
#include "Game.hpp"

const sf::Time Game::BUTTON_DELAY = sf::milliseconds(300);

Game::Game() 
	: window{ /* sf::VideoMode{980, 540} */ sf::VideoMode::getFullscreenModes().at(0) ,
	"Monochrome Space", /*sf::Style::Titlebar | sf::Style::Close */ sf::Style::Fullscreen }
{
	//Loading assets 
	spritesheet.loadFromFile("assets\\spritesheet.png");
	main_font.loadFromFile("assets\\LCD_Solid.ttf");

	init();
}

void Game::init()
{
	auto window_size = window.getSize();
	map_size = sf::Vector2f{ static_cast<float>(window_size.x) * 0.4f, static_cast<float>(window_size.y - 70) };

	messeges = std::make_unique<MsgQueue>(sf::FloatRect{ static_cast<float>(window_size.x) * 0.4f + 50.0f, 110.0f,
		(static_cast<float>(window_size.x) - static_cast<float>(window_size.x) * 0.4f - 40) / 12.5f,
		window_size.y * 0.7f / 19.5f },
		main_font, 20);

	buttons.push_back(Button{ sf::FloatRect{ static_cast<float>(window_size.x) * 0.4f + 30.0f,
		static_cast<float>(window_size.y) - 80,
		static_cast<float>(window_size.x) - static_cast<float>(window_size.x) * 0.4f - 40,
		60 }, "Next Turn", main_font });

	buttons.push_back(Button{ sf::FloatRect{ static_cast<float>(window_size.x) * 0.4f + 30.0f,
		static_cast<float>(window_size.y) - 80,
		(static_cast<float>(window_size.x) - static_cast<float>(window_size.x) * 0.4f - 40) / 2,
		60 }, "Option 1", main_font });

	buttons.push_back(Button{ sf::FloatRect{ static_cast<float>(window_size.x) * 0.70f + 15.0f,
		static_cast<float>(window_size.y) - 80,
		(static_cast<float>(window_size.x) - static_cast<float>(window_size.x) * 0.4f - 40) / 2,
		60 }, "Option 2", main_font });


	spaceship_texture.loadFromImage(spritesheet, sf::IntRect{ 0, 0, 16, 16 });
	spaceship.setTexture(spaceship_texture);

	generateMap();

	*messeges << "Welcome to Monochrome Space!\n"
		"   __  ___                   __                        ____                 \n"
		"  /  |/  /__  ___  ___  ____/ /  _______  __ _  ___   / __/__  ___ ________ \n"
		" / /|_/ / _ \\/ _ \\/ _ \\/ __/ _ \\/ __/ _ \\/  ' \\/ -_) _\\ \\/ _ \\/ _ `/ __/ -_)\n"
		"/_/  /_/\\___/_//_/\\___/\\__/_//_/_/  \\___/_/_/_/\\__/ /___/ .__/\\_,_/\\__/\\__/ \n"
		"                                                       /_/                  \n"
		"Your goal is to earn 1000$.\n"
		"Enjoy yourself while playing!\n"
		"-- INFO --\n"
		"Click Escape to exit game.\n"
		"To start game choose your destination on the map and click next turn.\n"
		"----------\n"
		<< MsgQueue::flush;
}

void Game::clear()
{
	buttons.clear();
	map.clear();
	current_event = None;

	ppos = 0;
	tpos = -1;
	ttime = 0;
	event_variant = -1;

	player_stats = Stats{};
	bool MsgShown = false;
}

void Game::start()
{
	const sf::Time frame_step = sf::milliseconds(1000/60);
	sf::Clock main_clock;

	while(window.isOpen())
	{
		handleEvents();
		update();

		if(main_clock.getElapsedTime() > frame_step)
		{
			draw();
			main_clock.restart();
		}
	}
}

void Game::update()
{
	updateSpaceship();

	if(tpos == -1)
	{
		buttons[NewTurn].lock();
	}

	for(size_t i = 0; i < buttons.size(); ++i)
	{
		if((i == 0 && current_event == None) || (i != 0 && current_event != None))
		{
			buttons.at(i).update(window);

			switch(current_event)
			{
			case Travel:
			case Planet:
			{
				if(i == 1)
					buttons.at(i).setText("Attack");
				else
					buttons.at(i).setText("Flee");
			} break;

			case Shop:
			case Merchant:
			{
				if(i == 1)
					buttons.at(i).setText("Buy");
				else
					buttons.at(i).setText("Exit");
			} break;

			case GameOver:
			case GameWon:
				if(i == 1)
					buttons.at(i).setText("Restart");
				else
					buttons.at(i).setText("Quit");
			}
		}
	}

	if(player_stats.hp <= 0)
	{
		current_event = GameOver;
	}

	if(player_stats.money >= 1000)
	{
		current_event = GameWon;
	}
}

void Game::updateSpaceship()
{
	static float travel_time = 0.0f;

	auto star_pos = map.at(ppos)->getPos();
	auto spaceship_pos = star_pos + sf::Vector2f{ 18, 2 };

	//Update ship in orbit of star
	if(ttime == 0)
	{
		spaceship.setPosition(spaceship_pos);
		travel_time = 0.0f;
	}
	else if(traveling)
	{
		if(travel_time == 0)
		{
			travel_time = static_cast<float>(ttime + 1);
		}

		auto destination_pos = map.at(tpos)->getPos() + sf::Vector2f{ 18, 2 };

		//Interpolate position between two stars
		sf::Vector2f mov_vector{
			destination_pos.x - spaceship_pos.x,
			destination_pos.y - spaceship_pos.y
		};
		
		spaceship.setPosition(mov_vector * (static_cast<float>(travel_time - ttime) / travel_time) + spaceship_pos);
	}
}

void Game::handleEvents()
{
	sf::Event event;
	while(window.pollEvent(event))
	{
		switch(event.type)
		{

		case sf::Event::Closed:
		{
			window.close();
		} break;

		default:
			break;
		}
	}

	if(button_clock.getElapsedTime() > BUTTON_DELAY)
	{
		for(auto& button : buttons)
		{
			button.unlock();
		}
	}

	if(buttons[NewTurn].isClicked() && current_event == None)
	{
		*messeges << "Next turn ..." << MsgQueue::flush;
		lockButtons();

		if(--ttime > 0)
		{
			traveling = true;

			for(auto& star : map)
			{
				star->lock();
			}

			*messeges << "You need to wait " << ttime << " turn to achive your destination."
				<< MsgQueue::flush;

			//In-game travel event
			std::random_device rd;
			std::uniform_int_distribution<int> dist(0, 100);

			if(dist(rd) < 40)
			{
				*messeges << "Something unexpected heppend during your travel:" << MsgQueue::flush;
				current_event = Travel;
			}
			else if(dist(rd) > 95)
			{
				*messeges << "You have encoutred travelling merchant during your travel:" << MsgQueue::flush;
				current_event = Merchant;

			}
		}
		else if(tpos != -1)
		{
			ppos = tpos;
			tpos = -1;
			traveling = false;

			map.at(ppos)->deselect();
			for(auto& star : map)
			{
				star->unlock();
			}

			*messeges << "You have achived your destination." << MsgQueue::flush;

			//In-game planet event
			std::random_device rd;
			std::uniform_int_distribution<int> dist(0, 100);

			if(dist(rd) < 20)
			{
				*messeges << "You have encoutred shop on the planet's surface:" << MsgQueue::flush;
				current_event = Shop;
			}
			else if(dist(rd) > 70)
			{
				*messeges << "Something unexpected happened on the planet's surface:" << MsgQueue::flush;
				current_event = Planet;
			}
		}

			
	}

	for(size_t i = 0; i < map.size(); ++i)
	{
		if(map.at(i)->isClicked(window) 
			&& button_clock.getElapsedTime() > BUTTON_DELAY
			&& tpos != i)
		{
			if(tpos != -1)
			{
				map.at(tpos)->deselect();
			}

			map.at(i)->select();
			tpos = i;
			ttime = static_cast<int>(std::ceil(std::sqrt(std::pow(map.at(i)->getPos().x - map.at(ppos)->getPos().x, 2)
				+ std::pow(map.at(i)->getPos().y - map.at(ppos)->getPos().y, 2)) / (100 + player_stats.speed)));

			*messeges << "Travel to this star is going to take " << ttime
				<< " turns." << MsgQueue::flush;

			button_clock.restart();
		}
	}

	if(current_event != None)
	{
		for(auto& star : map)
		{
			star->lock();
		}

		performEvent();
	}
	else
	{
		for(auto& star : map)
		{
			star->unlock();
		}
	}

	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
	{
		window.close();
	}
}

void Game::performEvent()
{
	std::random_device rd;
	if(event_variant == -1)
	{
		std::uniform_int_distribution<unsigned> dist{ 0, TRAVEL_EVENTS.size() - 1 };
		if(current_event == Shop)
		{
			dist = std::uniform_int_distribution<unsigned>{ 0, 2 };
			event_variant = event_variant % 3;
		}
		
		event_variant = dist(rd);
	}
	
	if(!msg_shown)
	{
		showEventMsg();
		msg_shown = true;
	}

	switch(current_event)
	{
	case Merchant:
	{
		if(buttons[Option1].isClicked())
		{
			buyOffer();

			lockButtons();
			resetEvent();
		}
		else if(buttons[Option2].isClicked())
		{
			*messeges << "You departed from the merchant." << MsgQueue::flush;

			lockButtons();
			resetEvent();
		}
		
	} break;

	case Shop:
	{
		if(buttons[Option1].isClicked())
		{
			//Mechanic unique code
			if(event_variant == 2 && static_cast<int>(player_stats.money) >= offer[0])
			{
				if(player_stats.hp != player_stats.max_hp)
				{
					player_stats.hp += hp_recovery;

					if(player_stats.hp > static_cast<int>(player_stats.max_hp))
					{
						player_stats.hp = player_stats.max_hp;
					}
					buyOffer();
				}
				else
				{
					*messeges << "Your ship is perfectly fine!" << MsgQueue::flush;
				}
			}
			else
				buyOffer();

			lockButtons();
			resetEvent();
		}
		else if(buttons[Option2].isClicked())
		{
			*messeges << "You departed from the store." << MsgQueue::flush;

			lockButtons();
			resetEvent();
		}
		
	} break;

	case Planet:
	case Travel:
	{
		
		if(buttons[Option1].isClicked())
		{
			attack();

			lockButtons();
			resetEvent();
		}	
		else if(buttons[Option2].isClicked())
		{
			flee();

			lockButtons();
			resetEvent();
		}
			
	} break;

	case GameOver:
	case GameWon:
		if(buttons[Option1].isClicked())
		{
			clear();
			init();

			lockButtons();
		}
		else if(buttons[Option2].isClicked())
		{
			window.close();
		}
	}

}

void Game::merchant()
{
	*messeges << MERCHANT << MsgQueue::flush;
	for(int i = 0; i < 5; ++i)
	{
		offer[i] = 0;
	}

	std::random_device rd;
	std::uniform_int_distribution<int> dist_cost{ 100, 1000}, dist_value{ 10, 50 }, dist_offer{ 0, 3 };

	offer[0] = dist_cost(rd);
	for(int i = 0; i < 3; ++i)
	{
		offer[dist_offer(rd) + 1] += dist_value(rd);
	}

	printOffer();
}

void Game::civilianShop()
{
	for(int i = 0; i < 5; ++i)
	{
		offer[i] = 0;
	}

	std::random_device rd;
	std::uniform_int_distribution<int> dist_cost{ 10, 100 }, dist_value{ 1, 15 }, dist_offer{ 1, 2 };

	offer[0] = dist_cost(rd);
	for(int i = 0; i < 2; ++i)
	{
		offer[dist_offer(rd)] += dist_value(rd);
	}

	printOffer();
}

void Game::militaryShop()
{
	for(int i = 0; i < 2; ++i)
	{
		offer[i] = 0;
	}

	std::random_device rd;
	std::uniform_int_distribution<int> dist_cost{ 10, 100 }, dist_value{ 1, 15 }, dist_offer{ 3, 4 };

	offer[0] = dist_cost(rd);
	for(int i = 0; i < 3; ++i)
	{
		offer[dist_offer(rd)] += dist_value(rd);
	}

	printOffer();
}


void Game::mechanic()
{
	hp_recovery = 0;
	std::random_device rd;
	std::uniform_int_distribution<int> dist_cost{ 1, 50 }, dist_value{ 1, 15 };

	hp_recovery = dist_value(rd);

	*messeges << "-- OFFER --\n"
		<< "HP: +" << hp_recovery
		<< "\nCost: " << offer[0] << "$\n"
		<< "\n-----------" << MsgQueue::flush;
}


void Game::showEventMsg()
{
	switch(current_event)
	{
	case Merchant:
		merchant();
		break;
	case Shop:
		*messeges << SHOPS[event_variant] << MsgQueue::flush;
		if(event_variant == 0)
		{
			civilianShop();
		}
		else if(event_variant == 1)
		{
			militaryShop();
		}
		else
		{
			mechanic();
		}
		break;
	case Planet:
		*messeges << PLANET_EVENTS[event_variant] << MsgQueue::flush;
		break;
	case Travel:
		*messeges << TRAVEL_EVENTS[event_variant] << MsgQueue::flush;
		break;
	case GameOver:
		*messeges << "Game Over!" << MsgQueue::flush;
		break;
	case GameWon:
		*messeges << "You won!" << MsgQueue::flush;
		break;
	}
}

void Game::resetEvent()
{
	current_event = None;
	event_variant = -1;
	msg_shown = false;
}

void Game::attack()
{
	std::random_device rd;
	std::uniform_int_distribution<int> dist{ 0, 100 };

	int result = dist(rd) + player_stats.attack - dist(rd) - player_stats.defense;

	if(result < 0)
	{
		std::uniform_int_distribution<int> dist_money{ 0, static_cast<int>(player_stats.money) },
			dist_dmg{0, static_cast<int>(player_stats.max_hp - 1)};

		int lostMoney = dist_money(rd), damage = dist_dmg(rd);

		*messeges << "You are damaged by enemy!\n"
			"-- RESULT --\n"
			"HP: -" << damage
			<< "\nMoney: -" << lostMoney << "$\n"
			<< "------------\n"
			<< MsgQueue::flush;

		player_stats.hp -= damage;
		player_stats.money -= lostMoney;
			
	}
	else if(result > 0)
	{
		std::uniform_int_distribution<int> dist_money{ 0, 250 };

		int looted_money = dist_money(rd);

		*messeges << "You killed the enemy!\n"
			"-- RESULT --\n"
			<< "\nMoney: +" << looted_money << "$\n"
			<< "\n------------\n"
			<< MsgQueue::flush;

		player_stats.money += looted_money;
	}
	else
	{
		*messeges << "Enemy fled!\n" << MsgQueue::flush;
	}
}

void Game::flee()
{
	std::random_device rd;
	std::uniform_int_distribution<int> dist(0, 100);

	int result = dist(rd) + 40 + player_stats.speed - dist(rd) - player_stats.defense;

	if(result < 0)
	{
		std::uniform_int_distribution<int> dist_money{ 0, static_cast<int>(player_stats.money) },
			dist_dmg{ 0, static_cast<int>(player_stats.max_hp - 1) };

		int lost_money = dist_money(rd), damage = dist_dmg(rd);

		*messeges << "You are damaged by enemy!\n"
			"-- RESULT --\n"
			"HP: -" << damage
			<< "\nMoney: -" << lost_money << "$\n"
			<< "------------\n"
			<< MsgQueue::flush;

		player_stats.hp -= damage;
		player_stats.money -= lost_money;

	}
	else
	{
		*messeges << "You sucessfully fled!\n" << MsgQueue::flush;
	}
}

void Game::buyOffer()
{
	if(static_cast<int>(player_stats.money) >= offer[0])
	{
		player_stats.money -= offer[0];

		player_stats.max_hp += offer[1];
		player_stats.hp += offer[1];
		player_stats.speed += offer[2];
		player_stats.defense += offer[3];
		player_stats.attack += offer[4];

		*messeges << "Thank you for transaction!" << MsgQueue::flush;
	}
	else
	{
		*messeges << "You don't have enough money!" << MsgQueue::flush;
	}

	resetEvent();
	
}

void Game::draw()
{
	window.clear();

	drawMap();

	window.draw(spaceship);

	drawStatus();
	drawMsgBox();

	for(size_t i = 0; i < buttons.size(); ++i)
	{
		if((i == 0 && current_event == None && tpos != -1) 
			|| (i != 0 && current_event != None))
			window.draw(buttons.at(i));
	}

	window.display();
}

void Game::drawFrame(sf::FloatRect Rect, std::string title = "")
{
	//Frame itself
	sf::RectangleShape Frame{ sf::Vector2f{Rect.width, Rect.height} };
	Frame.setPosition(Rect.left, Rect.top);
	Frame.setFillColor(sf::Color::White);

	window.draw(Frame);

	//Space inside of it
	Frame.setSize(sf::Vector2f{ Rect.width - 20, Rect.height - 20 });
	Frame.setPosition(Rect.left + 10, Rect.top + 10);
	Frame.setFillColor(sf::Color::Black);

	window.draw(Frame);

	//Frame title
	if(title != "")
	{
		sf::Text titleText{ title, main_font, 40 };
		titleText.setPosition(Rect.left,
			Rect.top - titleText.getGlobalBounds().height - 15);
		window.draw(titleText);
	}
	
}

void Game::drawMap()
{
	auto WindowSize = window.getSize();
	

	sf::FloatRect rect{20.0f, 50.0f,
		map_size.x, map_size.y };
	drawFrame(rect, "Map");

	for(auto& star : map)
	{
		window.draw(*star);
	}
}

void Game::drawStatus()
{
	std::stringstream string_gen;

	if(player_stats.hp < 0)
		player_stats.hp = 0;

	//HP Bar
	string_gen << "HP: " << player_stats.hp << '/' << player_stats.max_hp << "\n";

	std::string title; getline(string_gen, title);

	auto WindowSize = window.getSize();
	sf::FloatRect rect{ static_cast<float>(WindowSize.x) * 0.4f + 30.0f, 50.0f,
		static_cast<float>(WindowSize.x) - static_cast<float>(WindowSize.x) * 0.4f - 40,
		50.f
	};
	drawFrame(rect, title);

	sf::RectangleShape Bar;
	Bar.setSize(sf::Vector2f{ (rect.width - 30) * (static_cast<float>(player_stats.hp) / player_stats.max_hp), rect.height - 30 });
	Bar.setPosition(static_cast<float>(WindowSize.x) * 0.4f + 45.0f, 65.0f);
	Bar.setFillColor(sf::Color::White);

	window.draw(Bar);

	//Rest of stats
	string_gen << "Money:" << player_stats.money << "$"
		<< " Attack:" << player_stats.attack
		<< " Defense:" << player_stats.defense
		<< " Speed:" << player_stats.speed << "\n";
	getline(string_gen, title);
	sf::Text StatDisplay{ title, main_font, 30 };
	StatDisplay.setPosition(static_cast<float>(WindowSize.x) * 0.4f + 30.0f,
		static_cast<float>(WindowSize.y * 0.75) + 130.0f);
	window.draw(StatDisplay);

}

void Game::drawMsgBox()
{
	auto window_size = window.getSize();
	sf::FloatRect Rect{ static_cast<float>(window_size.x) * 0.4f + 30.0f, 110.0f,
		static_cast<float>(window_size.x) - static_cast<float>(window_size.x) * 0.4f - 40,
		static_cast<float>(window_size.y * 0.75)
	};
	drawFrame(Rect);

	window.draw(*messeges);
}

void Game::generateMap()
{
	sf::Vector2f squere_size{ map_size.x / 5, map_size.y / 10 };

	std::random_device rd;
	std::uniform_real_distribution<float> distributionX{ 0, squere_size.x },
		distributionY{ 0, squere_size.y };

	for(int i = 0; i < 4; ++i)
	{
		for(int j = 0; j < 9; ++j)
		{
			map.push_back(std::make_unique<Star>( spritesheet,
				sf::Vector2f{30 + distributionX(rd) + (squere_size.x + 5) * i, 60 + distributionY(rd) + (squere_size.y + 5) * j},
				squere_size ));
		}
	}
}

void Game::lockButtons()
{
	for(auto& button : buttons)
	{
		button.lock();
	};
	button_clock.restart();
}

void Game::printOffer()
{
	*messeges << "-- OFFER --\n";

	if(offer[1] != 0)
	{
		*messeges << "Max HP: +" << offer[1] << "\n";
	}

	if(offer[2] != 0)
	{
		*messeges << "Speed: +" << offer[2] << "\n";
	}

	if(offer[3] != 0)
	{
		*messeges << "Defense: +" << offer[3] << "\n";
	}

	if(offer[4] != 0)
	{
		*messeges << "Attack: +" << offer[4] << "\n";
	}

	*messeges << "\nCost: " << offer[0] << "$\n"
		<< "\n-----------" << MsgQueue::flush;

}

Game::~Game()
{

}