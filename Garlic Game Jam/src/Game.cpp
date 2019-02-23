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

Game::Game() : Window{ sf::VideoMode{960, 540}, "Monochrome Space", sf::Style::Titlebar | sf::Style::Close }
{
	MainFont.loadFromFile("assets\\LCD_Solid.ttf");
}

void Game::start()
{
	const sf::Time frameStep = sf::milliseconds(1000/60);
	sf::Clock MainClock;

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
}

void Game::draw()
{
	Window.clear();

	drawMap();
	drawStatus();
	drawMsgBox();

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
		static_cast<float>(WindowSize.x) * 0.4f,
		static_cast<float>(WindowSize.y - 70)
	};
	drawFrame(Rect, "Map");
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
		220.f
	};
	drawFrame(Rect);
}

Game::~Game()
{

}