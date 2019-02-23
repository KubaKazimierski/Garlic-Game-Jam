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

#include "Star.hpp"

Star::Star(sf::Image& spriteSheet, sf::Vector2f pos, sf::Vector2f squereSize) 
	: SpriteSheet{ spriteSheet }, Pos{ pos }, SquereSize{ squereSize }
{
	Texture.loadFromImage(SpriteSheet, sf::IntRect{ 16, 0, 16, 16 });
	Sprite.setTexture(Texture);
	Sprite.setPosition(pos);
}

void Star::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(Sprite, states);
}

void Star::lock()
{
	locked = true;
}

void Star::unlock()
{
	locked = false;
}

void Star::select()
{
	lock();
	Texture.loadFromImage(SpriteSheet, sf::IntRect{ 32, 0, 16, 16 });
	Sprite.setTexture(Texture);
}

void Star::deselect()
{
	unlock();
	Texture.loadFromImage(SpriteSheet, sf::IntRect{ 16, 0, 16, 16 });
	Sprite.setTexture(Texture);
}

bool Star::isClicked(sf::RenderWindow& window)
{
	return Sprite.getGlobalBounds().contains(static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)))
		&& sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
}

sf::Vector2f Star::getPos()
{
	return sf::Vector2f{ static_cast<float>(static_cast<int>(Pos.x / SquereSize.x + 1)),
		static_cast<float>(static_cast<int>(Pos.y / SquereSize.y + 1)) };
}

Star::~Star()
{
}