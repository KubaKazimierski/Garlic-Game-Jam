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

#include "MsgQueue.hpp"

const MsgQueue::Manipulator MsgQueue::flush;

MsgQueue::MsgQueue(sf::FloatRect rect, sf::Font& font, unsigned charSize)
	: Rect{ rect }, Font{ font }, CharSize{ charSize }
{
}

void MsgQueue::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	sf::Text text{"", Font, CharSize};
	text.setPosition(Rect.left, Rect.top);

	for(size_t i = 0; i < Messeges.size(); ++i)
	{
		text.setPosition(text.getPosition().x, text.getPosition().y + CharSize);
		text.setString(Messeges.at(i));
		target.draw(text, states);
	}
}

void MsgQueue::clear()
{
	Messeges.clear();
}

MsgQueue::~MsgQueue()
{
}

MsgQueue& MsgQueue::operator<<(std::string string)
{
	NewMsg << string;
	return *this;
}

MsgQueue& MsgQueue::operator<<(int n)
{
	NewMsg << n;
	return *this;
}

MsgQueue& MsgQueue::operator<<(Manipulator)
{
	NewMsg << '\n';

	std::string tmp;

	do
	{
		std::getline(NewMsg, tmp);

		if(tmp.size() >= Rect.width)
		{
			tmp.erase(tmp.begin() + static_cast<int>(Rect.width) - 1, tmp.end());
		}

		if(tmp != "")
			Messeges.push_back(tmp);

		if(Messeges.size() >= Rect.height)
		{
			Messeges.erase(Messeges.begin());
		}
	}
	while(!NewMsg.eof());
	NewMsg.clear();
	

	return *this;
}