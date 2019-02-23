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
	: Rect{ Rect }, Text{ name, font, static_cast<unsigned>(Rect.height * 0.3) }
{
	Text.setPosition(Rect.left + Rect.width / 2 - Text.getGlobalBounds().width / 2,
		Rect.top + Rect.height / 2 - Text.getGlobalBounds().height);
}

void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	//Frame itself
	sf::RectangleShape Frame{ sf::Vector2f{Rect.width, Rect.height} };
	if(!onMouse)
	{
		Frame.setPosition(Rect.left, Rect.top);
		Frame.setFillColor(sf::Color::White);

		target.draw(Frame, states);
	}

	//Space inside of it
	Frame.setSize(sf::Vector2f{ Rect.width - 10, Rect.height - 10 });
	Frame.setPosition(Rect.left + 5, Rect.top + 5);
	Frame.setFillColor((onMouse ? sf::Color::White : sf::Color::Black));

	target.draw(Frame, states);

	target.draw(Text, states);
}

void Button::update(const sf::Window& window)
{
	if(Rect.contains(static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)))
		&& !locked)
	{
		onMouse = true;
		Text.setFillColor(sf::Color::Black);
	}
	else
	{
		onMouse = false;
		Text.setFillColor(sf::Color::White);
	}
}

void Button::setText(std::string name)
{
	Text.setString(name);
	Text.setPosition(Rect.left + Rect.width / 2 - Text.getGlobalBounds().width / 2,
		Rect.top + Rect.height / 2 - Text.getGlobalBounds().height);
}

bool Button::isClicked()
{
	if(onMouse && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && !locked)
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