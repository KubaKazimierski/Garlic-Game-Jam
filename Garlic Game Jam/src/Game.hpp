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
#include "Button.hpp"
#include "MsgQueue.hpp"
#include "Star.hpp"
#include "Stats.hpp"
#include "Event.hpp"
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <memory>
#include <random>

class Game
{
public:
	Game();
	void start();
	~Game();
private:
	enum Buttons : int
	{
		NewTurn = 0,
		Option1,
		Option2
	};

	static const sf::Time BUTTON_DELAY;

	sf::RenderWindow window;
	sf::Font main_font;
	sf::Image spritesheet;
	sf::Vector2f map_size;
	sf::Sprite spaceship;
	sf::Texture spaceship_texture;
	sf::Clock button_clock;

	std::vector<Button> buttons;
	std::vector<std::unique_ptr<Star>> map;

	std::unique_ptr<MsgQueue> messeges;

	Stats player_stats;
	Event current_event = None;

	int ppos = 0, 
	    tpos = -1,
	    ttime = 0,
	    event_variant = -1,
	    offer[5] = { 0, 0, 0, 0, 0 },
	    hp_recovery = 0;
	bool msg_shown = false;

	void init();
	void clear();

	//Main functions
	void update();
	void handleEvents();
	void draw();

	//Draw functions
	void drawFrame(sf::FloatRect, std::string);
	void drawMap();
	void drawStatus();
	void drawMsgBox();
	void drawSpaceShip();

	//Map functions
	void generateMap();

	//Event functions
	void performEvent();
	void showEventMsg();
	void resetEvent();

	void attack();
	void flee();

	////Shops:
	void merchant();
	void civilianShop();
	void mechanic();
	void militaryShop();
	void buyOffer();

	//Helpers
	void lockButtons();
	void printOffer();

};