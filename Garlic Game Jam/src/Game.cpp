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

const sf::Time Game::ButtonDelay = sf::milliseconds(250);

Game::Game() 
	: Window{ sf::VideoMode{1960, 1080},
	"Monochrome Space", /*sf::Style::Titlebar | sf::Style::Close */ sf::Style::Fullscreen }
{
	SpriteSheet.loadFromFile("assets\\spritesheet.png");
	MainFont.loadFromFile("assets\\LCD_Solid.ttf");

	init();
}

void Game::init()
{
	auto WindowSize = Window.getSize();
	MapSize = sf::Vector2f{ static_cast<float>(WindowSize.x) * 0.4f, static_cast<float>(WindowSize.y - 70) };

	Messeges = std::make_unique<MsgQueue>(sf::FloatRect{ static_cast<float>(WindowSize.x) * 0.4f + 50.0f, 110.0f,
		(static_cast<float>(WindowSize.x) - static_cast<float>(WindowSize.x) * 0.4f - 40) / 12.5f,
		WindowSize.y * 0.7f / 19.5f },
		MainFont, 20);

	Buttons.push_back(Button{ sf::FloatRect{ static_cast<float>(WindowSize.x) * 0.4f + 30.0f,
		static_cast<float>(WindowSize.y) - 80,
		static_cast<float>(WindowSize.x) - static_cast<float>(WindowSize.x) * 0.4f - 40,
		60 }, "Next Turn", MainFont });

	Buttons.push_back(Button{ sf::FloatRect{ static_cast<float>(WindowSize.x) * 0.4f + 30.0f,
		static_cast<float>(WindowSize.y) - 80,
		(static_cast<float>(WindowSize.x) - static_cast<float>(WindowSize.x) * 0.4f - 40) / 2,
		60 }, "Option 1", MainFont });

	Buttons.push_back(Button{ sf::FloatRect{ static_cast<float>(WindowSize.x) * 0.70f + 15.0f,
		static_cast<float>(WindowSize.y) - 80,
		(static_cast<float>(WindowSize.x) - static_cast<float>(WindowSize.x) * 0.4f - 40) / 2,
		60 }, "Option 2", MainFont });


	SpaceShipTexture.loadFromImage(SpriteSheet, sf::IntRect{ 0, 0, 16, 16 });
	SpaceShip.setTexture(SpaceShipTexture);

	generateMap();

	*Messeges << "Welcome to Monochrome Space!\n"
		"   __  ___                   __                        ____                 \n"
		"  /  |/  /__  ___  ___  ____/ /  _______  __ _  ___   / __/__  ___ ________ \n"
		" / /|_/ / _ \\/ _ \\/ _ \\/ __/ _ \\/ __/ _ \\/  ' \\/ -_) _\\ \\/ _ \\/ _ `/ __/ -_)\n"
		"/_/  /_/\\___/_//_/\\___/\\__/_//_/_/  \\___/_/_/_/\\__/ /___/ .__/\\_,_/\\__/\\__/ \n"
		"                                                       /_/                  \n"
		"Your goal is to earn 1000$.\n"
		"Enjoy yourself while playing!\n"
		"-- INFO --\n"
		"Click Escape key on your keyboard to exit game.\n"
		"To start game choose your destination on the map and click next turn.\n"
		"----------\n"
		<< MsgQueue::flush;
}

void Game::clear()
{
	Buttons.clear();
	Map.clear();
	Event CurrentEvent = None;

	ppos = 0;
	tpos = -1;
	ttime = 0;
	EventVariant = -1;

	PlayerStats = Stats{};
	bool MsgShown = false;

	CurrentEvent = None;
}

void Game::start()
{
	const sf::Time frameStep = sf::milliseconds(1000/60);
	sf::Clock MainClock;

	while(Window.isOpen())
	{
		handleEvents();
		update();

		if(MainClock.getElapsedTime() > frameStep)
		{
			draw();
			MainClock.restart();
		}
	}
}

void Game::update()
{
	for(size_t i = 0; i < Buttons.size(); ++i)
	{
		if((i == 0 && CurrentEvent == None) || (i != 0 && CurrentEvent != None))
		{
			Buttons.at(i).update(Window);

			switch(CurrentEvent)
			{
			case Travel:
			case Planet:
			{
				if(i == 1)
					Buttons.at(i).setText("Attack");
				else
					Buttons.at(i).setText("Flee");
			} break;

			case Shop:
			case Merchant:
			{
				if(i == 1)
					Buttons.at(i).setText("Buy");
				else
					Buttons.at(i).setText("Exit");
			} break;

			case GameOver:
			case GameWon:
				if(i == 1)
					Buttons.at(i).setText("Restart");
				else
					Buttons.at(i).setText("Quit");
			}
		}
	}

	if(PlayerStats.Hp <= 0)
	{
		CurrentEvent = GameOver;
	}

	if(PlayerStats.Money >= 1000)
	{
		CurrentEvent = GameWon;
	}
}

void Game::handleEvents()
{
	sf::Event event;
	while(Window.pollEvent(event))
	{
		switch(event.type)
		{

		case sf::Event::Closed:
		{
			Window.close();
		} break;

		default:
			break;
		}
	}

	if(ButtonClock.getElapsedTime() > ButtonDelay)
	{
		for(auto& button : Buttons)
		{
			button.unlock();
		}
	}

	if(Buttons[NewTurn].isClicked() && CurrentEvent == None)
	{
		*Messeges << "Next turn ..." << MsgQueue::flush;
		lockButtons();

		if(--ttime > 0)
		{
			for(auto& star : Map)
			{
				star->lock();
			}

			*Messeges << "You need to wait " << ttime << " turn to achive your destination."
				<< MsgQueue::flush;

			//In-game travel event
			std::random_device rd;
			std::uniform_int_distribution<int> dist(0, 100);

			if(dist(rd) < 40)
			{
				*Messeges << "Something unexpected heppend during your travel:" << MsgQueue::flush;
				CurrentEvent = Travel;
			}
			else if(dist(rd) < 10)
			{
				*Messeges << "You have encoutred travelling merchant during your travel:" << MsgQueue::flush;
				CurrentEvent = Merchant;

			}
		}
		else if(tpos != -1)
		{
			ppos = tpos;
			tpos = -1;

			Map.at(ppos)->deselect();
			for(auto& star : Map)
			{
				star->unlock();
			}

			*Messeges << "You have achived your destination." << MsgQueue::flush;

			//In-game planet event
			std::random_device rd;
			std::uniform_int_distribution<int> dist(0, 100);

			if(dist(rd) < 20)
			{
				*Messeges << "You have encoutred shop on the planet's surface:" << MsgQueue::flush;
				CurrentEvent = Shop;
			}
			else if(dist(rd) < 30)
			{
				*Messeges << "Something unexpected happened on the planet's surface:" << MsgQueue::flush;
				CurrentEvent = Planet;
			}
		}

			
	}

	for(size_t i = 0; i < Map.size(); ++i)
	{
		if(Map.at(i)->isClicked(Window) 
			&& ButtonClock.getElapsedTime() > ButtonDelay
			&& tpos != i)
		{
			if(tpos != -1)
			{
				Map.at(tpos)->deselect();
			}

			Map.at(i)->select();
			tpos = i;
			ttime = static_cast<int>(ceil(sqrt(pow(Map.at(i)->getPos().x - Map.at(ppos)->getPos().x, 2)
				+ pow(Map.at(i)->getPos().y - Map.at(ppos)->getPos().y, 2)) / (100 + PlayerStats.Speed)));

			*Messeges << "Travel to this star is going to take " << ttime
				<< " turns." << MsgQueue::flush;

			ButtonClock.restart();
		}
	}

	if(CurrentEvent != None)
	{
		performEvent();
	}

	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
	{
		Window.close();
	}
}

void Game::performEvent()
{
	std::random_device rd;
	if(EventVariant == -1)
	{
		std::uniform_int_distribution<unsigned> dist{ 0, 2 };

		EventVariant = dist(rd);

		if(CurrentEvent == Shop || CurrentEvent == Merchant)
		{
			EventVariant = EventVariant % 3;
		}
	}
	
	if(!MsgShown)
	{
		showEventMsg();
		MsgShown = true;
	}

	switch(CurrentEvent)
	{
	case Merchant:
	{
		if(Buttons[Option1].isClicked())
		{
			buyOffer();

			lockButtons();
			resetEvent();
		}
		else if(Buttons[Option2].isClicked())
		{
			*Messeges << "You departed from the merchant." << MsgQueue::flush;

			lockButtons();

			resetEvent();
		}
		
	} break;

	case Shop:
	{
		if(Buttons[Option1].isClicked())
		{
			buyOffer();

			if(EventVariant == 2)
			{
				PlayerStats.Hp += HPrecovery;
			}

			lockButtons();
			resetEvent();
		}
		else if(Buttons[Option2].isClicked())
		{
			*Messeges << "You departed from the store." << MsgQueue::flush;
			lockButtons();
			resetEvent();
		}
		
	} break;

	case Planet:
	case Travel:
	{
		
		if(Buttons[Option1].isClicked())
		{
			attack();
			lockButtons();
		}	
		else if(Buttons[Option2].isClicked())
		{
			flee();
			lockButtons();
		}
			
	} break;

	case GameOver:
	case GameWon:
		if(Buttons[Option1].isClicked())
		{
			clear();
			init();
			lockButtons();
		}
		else if(Buttons[Option2].isClicked())
		{
			Window.close();
		}
	}

}

void Game::merchant()
{
	*Messeges << Merchants[0] << MsgQueue::flush;
	for(int i = 0; i < 5; ++i)
	{
		Offer[i] = 0;
	}

	std::random_device rd;
	std::uniform_int_distribution<int> distCost{ 100, 1000}, distValue{ 10, 50 }, distOffer{ 0, 3 };

	Offer[0] = distCost(rd);
	for(int i = 0; i < 3; ++i)
	{
		Offer[distOffer(rd) + 1] += distValue(rd);
	}

	printOffer();
}

void Game::civilianShop()
{
	for(int i = 0; i < 5; ++i)
	{
		Offer[i] = 0;
	}

	std::random_device rd;
	std::uniform_int_distribution<int> distCost{ 10, 100 }, distValue{ 1, 15 }, distOffer{ 1, 2 };

	Offer[0] = distCost(rd);
	for(int i = 0; i < 2; ++i)
	{
		Offer[distOffer(rd)] += distValue(rd);
	}

	printOffer();
}

void Game::militaryShop()
{
	for(int i = 0; i < 2; ++i)
	{
		Offer[i] = 0;
	}

	std::random_device rd;
	std::uniform_int_distribution<int> distCost{ 10, 100 }, distValue{ 1, 15 }, distOffer{ 3, 4 };

	Offer[0] = distCost(rd);
	for(int i = 0; i < 3; ++i)
	{
		Offer[distOffer(rd)] += distValue(rd);
	}

	printOffer();
}


void Game::mechanic()
{
	HPrecovery = 0;
	std::random_device rd;
	std::uniform_int_distribution<int> distCost{ 1, 50 }, distValue{ 1, 15 };

	HPrecovery = distValue(rd);

	*Messeges << "-- OFFER --\n"
		<< "HP: +" << HPrecovery
		<< "\nCost: " << Offer[0] << "$\n"
		<< "\n-----------" << MsgQueue::flush;
}


void Game::showEventMsg()
{
	switch(CurrentEvent)
	{
	case Merchant:
		merchant();
		break;
	case Shop:
		*Messeges << Shops[EventVariant] << MsgQueue::flush;
		if(EventVariant == 0)
		{
			civilianShop();
		}
		else if(EventVariant == 1)
		{
			militaryShop();
		}
		else
		{
			mechanic();
		}
		break;
	case Planet:
		*Messeges << PlanetEvents[EventVariant] << MsgQueue::flush;
		break;
	case Travel:
		*Messeges << TravelEvents[EventVariant] << MsgQueue::flush;
		break;
	case GameOver:
		*Messeges << "Game Over!" << MsgQueue::flush;
		break;
	case GameWon:
		*Messeges << "You won!" << MsgQueue::flush;
		break;
	}
}

void Game::resetEvent()
{
	CurrentEvent = None;
	EventVariant = -1;
	MsgShown = false;
}

void Game::attack()
{
	std::random_device rd;
	std::uniform_int_distribution<int> dist{ 0, 100 };

	int result = dist(rd) + PlayerStats.Attack - dist(rd) - PlayerStats.Defense;

	if(result < 0)
	{
		std::uniform_int_distribution<int> distMoney{ 0, static_cast<int>(PlayerStats.Money) },
			distDmg{0, static_cast<int>(PlayerStats.MaxHp - 1)};

		int lostMoney = distMoney(rd), damage = distDmg(rd);

		*Messeges << "You are damaged by enemy!\n"
			"-- RESULT --\n"
			"HP: -" << damage
			<< "\nMoney: -" << lostMoney << "$\n"
			<< "------------\n"
			<< MsgQueue::flush;

		PlayerStats.Hp -= damage;
		PlayerStats.Money -= lostMoney;
			
	}
	else if(result > 0)
	{
		std::uniform_int_distribution<int> distMoney{ 0, 200 };

		int lootedMoney = distMoney(rd);

		*Messeges << "You killed the enemy!\n"
			"-- RESULT --\n"
			<< "\nMoney: +" << lootedMoney << "$\n"
			<< "\n------------\n"
			<< MsgQueue::flush;

		PlayerStats.Money += lootedMoney;
	}
	else
	{
		*Messeges << "Enemy fled!\n" << MsgQueue::flush;
	}

	resetEvent();
}

void Game::flee()
{
	std::random_device rd;
	std::uniform_int_distribution<int> dist(0, 100);

	int result = dist(rd) + 50 + PlayerStats.Speed - dist(rd) - PlayerStats.Defense;

	if(result < 0)
	{
		std::uniform_int_distribution<int> distMoney{ 0, static_cast<int>(PlayerStats.Money) },
			distDmg{ 0, static_cast<int>(PlayerStats.MaxHp - 1) };

		int lostMoney = distMoney(rd), damage = distDmg(rd);

		*Messeges << "You are damaged by enemy!\n"
			"-- RESULT --\n"
			"HP: -" << damage
			<< "\nMoney: -" << lostMoney << "$\n"
			<< "------------\n"
			<< MsgQueue::flush;

		PlayerStats.Hp -= damage;
		PlayerStats.Money -= lostMoney;

	}
	else
	{
		*Messeges << "You sucessfully fled!\n" << MsgQueue::flush;
	}

	resetEvent();
}

void Game::buyOffer()
{
	if(PlayerStats.Money >= Offer[0])
	{
		PlayerStats.Money -= Offer[0];

		PlayerStats.MaxHp += Offer[1];
		PlayerStats.Speed += Offer[2];
		PlayerStats.Defense += Offer[3];
		PlayerStats.Attack += Offer[4];

		*Messeges << "Thank you for transaction!" << MsgQueue::flush;
	}
	else
	{
		*Messeges << "You don't have enough money!" << MsgQueue::flush;
	}

	resetEvent();
	
}

void Game::draw()
{
	Window.clear();

	drawMap();
	drawSpaceShip();
	drawStatus();
	drawMsgBox();

	for(size_t i = 0; i < Buttons.size(); ++i)
	{
		if((i == 0 && CurrentEvent == None) || (i != 0 && CurrentEvent != None))
			Window.draw(Buttons.at(i));
	}

	Window.display();
}

void Game::drawFrame(sf::FloatRect Rect, std::string title = "")
{
	//Frame itself
	sf::RectangleShape Frame{ sf::Vector2f{Rect.width, Rect.height} };
	Frame.setPosition(Rect.left, Rect.top);
	Frame.setFillColor(sf::Color::White);

	Window.draw(Frame);

	//Space inside of it
	Frame.setSize(sf::Vector2f{ Rect.width - 20, Rect.height - 20 });
	Frame.setPosition(Rect.left + 10, Rect.top + 10);
	Frame.setFillColor(sf::Color::Black);

	Window.draw(Frame);

	//Frame title
	if(title != "")
	{
		sf::Text titleText{ title, MainFont, 40 };
		titleText.setPosition(Rect.left,
			Rect.top - titleText.getGlobalBounds().height - 15);
		Window.draw(titleText);
	}
	
}

void Game::drawMap()
{
	auto WindowSize = Window.getSize();
	

	sf::FloatRect Rect{20.0f, 50.0f,
		MapSize.x, MapSize.y };
	drawFrame(Rect, "Map");

	for(auto& star : Map)
	{
		Window.draw(*star);
	}
}

void Game::drawStatus()
{
	std::stringstream stringGen;

	if(PlayerStats.Hp < 0)
		PlayerStats.Hp = 0;

	//HP Bar
	stringGen << "HP: " << PlayerStats.Hp << '/' << PlayerStats.MaxHp << "\n";

	std::string title; getline(stringGen, title);

	auto WindowSize = Window.getSize();
	sf::FloatRect Rect{ static_cast<float>(WindowSize.x) * 0.4f + 30.0f, 50.0f,
		static_cast<float>(WindowSize.x) - static_cast<float>(WindowSize.x) * 0.4f - 40,
		50.f
	};
	drawFrame(Rect, title);

	sf::RectangleShape Bar;
	Bar.setSize(sf::Vector2f{ (Rect.width - 30) * (static_cast<float>(PlayerStats.Hp) / PlayerStats.MaxHp), Rect.height - 30 });
	Bar.setPosition(static_cast<float>(WindowSize.x) * 0.4f + 45.0f, 65.0f);
	Bar.setFillColor(sf::Color::White);

	Window.draw(Bar);

	//Rest of stats
	stringGen << "Money:" << PlayerStats.Money << "$"
		<< " Attack:" << PlayerStats.Attack
		<< " Defense:" << PlayerStats.Defense
		<< " Speed:" << PlayerStats.Speed << "\n";
	getline(stringGen, title);
	sf::Text StatDisplay{ title, MainFont, 30 };
	StatDisplay.setPosition(static_cast<float>(WindowSize.x) * 0.4f + 30.0f,
		static_cast<float>(WindowSize.y * 0.75) + 130.0f);
	Window.draw(StatDisplay);

}

void Game::drawMsgBox()
{
	auto WindowSize = Window.getSize();
	sf::FloatRect Rect{ static_cast<float>(WindowSize.x) * 0.4f + 30.0f, 110.0f,
		static_cast<float>(WindowSize.x) - static_cast<float>(WindowSize.x) * 0.4f - 40,
		static_cast<float>(WindowSize.y * 0.75)
	};
	drawFrame(Rect);

	Window.draw(*Messeges);
}

void Game::drawSpaceShip()
{
	auto StarPos = Map.at(ppos)->getPos();
	auto SpaceShipPos = StarPos + sf::Vector2f{ 18, 2 };

	SpaceShip.setPosition(SpaceShipPos);
	Window.draw(SpaceShip);
}

void Game::generateMap()
{
	sf::Vector2f SquereSize{ MapSize.x / 5, MapSize.y / 10 };

	std::random_device rd;
	std::uniform_real_distribution<float> distributionX{ 0, SquereSize.x },
		distributionY{ 0, SquereSize.y };

	for(int i = 0; i < 4; ++i)
	{
		for(int j = 0; j < 9; ++j)
		{
			Map.push_back(std::make_unique<Star>( SpriteSheet,
				sf::Vector2f{30 + distributionX(rd) + (SquereSize.x + 5) * i, 60 + distributionY(rd) + (SquereSize.y + 5) * j},
				SquereSize ));
		}
	}
}

void Game::lockButtons()
{
	for(auto& button : Buttons)
	{
		button.lock();
	};
	ButtonClock.restart();
}

void Game::printOffer()
{
	*Messeges << "-- OFFER --\n";

	if(Offer[1] != 0)
	{
		*Messeges << "Max HP: +" << Offer[1] << "\n";
	}

	if(Offer[2] != 0)
	{
		*Messeges << "Speed: +" << Offer[2] << "\n";
	}

	if(Offer[3] != 0)
	{
		*Messeges << "Defense: +" << Offer[3] << "\n";
	}

	if(Offer[4] != 0)
	{
		*Messeges << "Attack: +" << Offer[4] << "\n";
	}

	*Messeges << "\nCost: " << Offer[0] << "$\n"
		<< "\n-----------" << MsgQueue::flush;

}

Game::~Game()
{

}