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

#include "Button.hpp"

Button::Button(sf::FloatRect Rect, std::string name, sf::Font& font) 
	: rect{ Rect }, text{ name, font, static_cast<unsigned>(Rect.height * 0.3) }
{
	text.setPosition(Rect.left + Rect.width / 2 - text.getGlobalBounds().width / 2,
		Rect.top + Rect.height / 2 - text.getGlobalBounds().height);
}

void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	//Frame itself
	sf::RectangleShape frame{ sf::Vector2f{rect.width, rect.height} };
	if(!on_mouse)
	{
		frame.setPosition(rect.left, rect.top);
		frame.setFillColor(sf::Color::White);

		target.draw(frame, states);
	}

	//Space inside of it
	frame.setSize(sf::Vector2f{ rect.width - 10, rect.height - 10 });
	frame.setPosition(rect.left + 5, rect.top + 5);
	frame.setFillColor((on_mouse ? sf::Color::White : sf::Color::Black));

	target.draw(frame, states);

	target.draw(text, states);
}

void Button::update(const sf::Window& window)
{
	if(rect.contains(static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)))
		&& !locked)
	{
		on_mouse = true;
		text.setFillColor(sf::Color::Black);
	}
	else
	{
		on_mouse = false;
		text.setFillColor(sf::Color::White);
	}
}

void Button::setText(std::string name)
{
	text.setString(name);
	text.setPosition(rect.left + rect.width / 2 - text.getGlobalBounds().width / 2,
		rect.top + rect.height / 2 - text.getGlobalBounds().height);
}

bool Button::isClicked()
{
	if(on_mouse && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && !locked)
	{
		return true;
	}

	return false;
}

void Button::lock()
{
	locked = true;
}

void Button::unlock()
{
	locked = false;
}

Button::~Button()
{
}