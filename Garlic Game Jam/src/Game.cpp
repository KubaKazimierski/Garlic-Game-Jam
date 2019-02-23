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

const sf::Time Game::ButtonDelay = sf::milliseconds(200);

Game::Game() 
	: Window{ sf::VideoMode{1960, 1040},
	"Monochrome Space", sf::Style::Titlebar | sf::Style::Close /*| sf::Style::Fullscreen*/ }
{
	SpriteSheet.loadFromFile("assets\\spritesheet.png");
	MainFont.loadFromFile("assets\\LCD_Solid.ttf");

	auto WindowSize = Window.getSize();
	MapSize = sf::Vector2f{ static_cast<float>(WindowSize.x) * 0.4f, static_cast<float>(WindowSize.y - 70) };

	Messeges = std::make_unique<MsgQueue>(sf::FloatRect{ static_cast<float>(WindowSize.x) * 0.4f + 50.0f, 110.0f,
		(static_cast<float>(WindowSize.x) - static_cast<float>(WindowSize.x) * 0.4f - 40) / 12.5f,
		WindowSize.y / 55.0f},
		MainFont, 20);

	Buttons.push_back(Button{ sf::FloatRect{ static_cast<float>(WindowSize.x) * 0.4f + 30.0f,
		static_cast<float>(WindowSize.y) - 80,
		static_cast<float>(WindowSize.x) - static_cast<float>(WindowSize.x) * 0.4f - 40,
		60 }, "Next Turn", MainFont });

	SpaceShipTexture.loadFromImage(SpriteSheet, sf::IntRect{0, 0, 16, 16});
	SpaceShip.setTexture(SpaceShipTexture);

	generateMap();
}

void Game::start()
{
	const sf::Time frameStep = sf::milliseconds(1000/60);
	sf::Clock MainClock;
	
	*Messeges << "Welcome to Monochrome Space!\nYour goal is to earn 1000000$.\n"
		<< "Enjoy yourself while playing it!"
		<< MsgQueue::flush;

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
	for(auto& button : Buttons)
	{
		button.update(Window);
	}

	if(ButtonClock.getElapsedTime() > ButtonDelay)
	{
		for(auto& button : Buttons)
		{
			button.unlock();
		}
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

	if(Buttons[NewTurn].isClicked())
	{
		*Messeges << "Next turn ..." << MsgQueue::flush;
		Buttons[NewTurn].lock();
		ButtonClock.restart();

		if(--ttime > 0)
		{
			for(auto& star : Map)
			{
				star->lock();
			}

			*Messeges << "You need to wait " << ttime << " turn to achive your destination."
				<< MsgQueue::flush;
			
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
				+ pow(Map.at(i)->getPos().y - Map.at(ppos)->getPos().y, 2)) / 100));

			*Messeges << "Travel to this star is going to take " << ttime
				<< " turns." << MsgQueue::flush;

			ButtonClock.restart();
		}
	}


	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
	{
		Window.close();
	}
}

void Game::draw()
{
	Window.clear();

	drawMap();
	drawSpaceShip();
	drawStatus();
	drawMsgBox();

	for(auto& button : Buttons)
	{
		Window.draw(button);
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

	//HP Bar
	stringGen << "HP: " << PlayerStats.Hp << '/' << PlayerStats.MaxHp << '\n';

	std::string title; getline(stringGen, title);

	auto WindowSize = Window.getSize();
	sf::FloatRect Rect{ static_cast<float>(WindowSize.x) * 0.4f + 30.0f, 50.0f,
		static_cast<float>(WindowSize.x) - static_cast<float>(WindowSize.x) * 0.4f - 40,
		50.f
	};
	drawFrame(Rect, title);

	sf::RectangleShape Bar;
	Bar.setSize(sf::Vector2f{ (Rect.width - 30) * (PlayerStats.Hp / PlayerStats.MaxHp), Rect.height - 30 });
	Bar.setPosition(static_cast<float>(WindowSize.x) * 0.4f + 45.0f, 65.0f);
	Bar.setFillColor(sf::Color::White);

	Window.draw(Bar);

	//Rest of stats
	stringGen << "Money:" << PlayerStats.Money << '$'
		<< " Attack:" << PlayerStats.Attack
		<< " Defense:" << PlayerStats.Defense
		<< " Speed:" << PlayerStats.Speed << '\n';
	getline(stringGen, title);
	sf::Text StatDisplay{ title, MainFont, 30 };
	StatDisplay.setPosition(static_cast<float>(WindowSize.x) * 0.4f + 30.0f, 550.0f);
	Window.draw(StatDisplay);

}

void Game::drawMsgBox()
{
	auto WindowSize = Window.getSize();
	sf::FloatRect Rect{ static_cast<float>(WindowSize.x) * 0.4f + 30.0f, 110.0f,
		static_cast<float>(WindowSize.x) - static_cast<float>(WindowSize.x) * 0.4f - 40,
		static_cast<float>(WindowSize.y * 0.4)
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

Game::~Game()
{

}