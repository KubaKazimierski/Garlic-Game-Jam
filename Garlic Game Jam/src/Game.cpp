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

Game::Game() 
	: Window{ sf::VideoMode{960, 540}, "Monochrome Space", sf::Style::Titlebar | sf::Style::Close | sf::Style::Fullscreen }
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

	generateMap();
}

void Game::start()
{
	const sf::Time frameStep = sf::milliseconds(1000/60);
	sf::Clock MainClock;
	
	*Messeges << "Welcome to Monochrome Space!\nEnjoy yourself while playing it!"
		<< MsgQueue::flush;

	while(Window.isOpen())
	{
		update();
		handleEvents();

		if(MainClock.getElapsedTime() > frameStep)
		{
			draw();
			MainClock.restart();
		}
	}
}

void Game::update()
{
	static sf::Clock ButtonClock;
	static const sf::Time ButtonDelay = sf::milliseconds(200);

	for(auto& button : Buttons)
	{
		button.update(Window);
	}

	if(Buttons[NewTurn].isClicked())
	{
		*Messeges << "Next turn ..." << MsgQueue::flush;
		Buttons[NewTurn].lock();
		ButtonClock.restart();
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

	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
	{
		Window.close();
	}
}

void Game::draw()
{
	Window.clear();

	drawMap();
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
	auto WindowSize = Window.getSize();
	sf::FloatRect Rect{ static_cast<float>(WindowSize.x) * 0.4f + 30.0f, 50.0f,
		static_cast<float>(WindowSize.x) - static_cast<float>(WindowSize.x) * 0.4f - 40,
		50.f
	};
	drawFrame(Rect, "HP");
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
				sf::Vector2f{30 + distributionX(rd) + SquereSize.x * i, 60 + distributionY(rd) + SquereSize.y * j},
				SquereSize ));
		}
	}
}

Game::~Game()
{

}