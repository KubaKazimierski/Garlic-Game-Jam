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
#include <SFML/Graphics.hpp>

class Star : public sf::Drawable
{
public:
	Star(sf::Image&, sf::Vector2f, sf::Vector2f);
	~Star();

	virtual void draw(sf::RenderTarget&, sf::RenderStates) const override;

	void lock();
	void unlock();
	void select();
	void deselect();
	bool isClicked(sf::RenderWindow&);

	sf::Vector2f getPos();
private:
	sf::Image& SpriteSheet;
	sf::Texture Texture;
	sf::Sprite Sprite;
	sf::Vector2f Pos, SquereSize;

	bool locked = false;
};