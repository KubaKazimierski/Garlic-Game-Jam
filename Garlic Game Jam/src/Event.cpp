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

#include "Event.hpp"

const std::vector<std::string> TravelEvents = {
	"You are attacked by pirates!",
	"You are attacked by space amoeba!",
	"Big asteroid is in front of you!"
};

const std::vector<std::string> PlanetEvents = {
	"You are attacked by a sand worm!",
	"You are attacked by a enormous sloth!",
	"Giant is going to crash you w their foot!"
};

const std::vector<std::string> Shops = {
	"Welcome to the civilian shop!",
	"Welcome to the military shop!",
	"Welcome to the mechanic!"
};

const std::vector<std::string> Merchants = {
	"Merchant: It is uncommon to meet living soul in this area.\n"
	"          Do you want to take a look at my offer?"
};