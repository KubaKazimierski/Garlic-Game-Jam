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

static std::string attackOf(std::string attacker)
{
	return std::string("You are attacked by ") + attacker + "!";
}

const std::vector<std::string> TRAVEL_EVENTS = {
	attackOf("pirates"),
	attackOf("space amoeba"),
	attackOf("hostile mining drones"),
	attackOf("swarm of space insectioids"),
	attackOf("living crystals"),
	attackOf("garlic worshipers"),
	attackOf("swarm of nanites"),
	attackOf("a space squid"),
	"Big asteroid is in front of you!"
};

const std::vector<std::string> PLANET_EVENTS = {
	attackOf("a sand worm"),
	attackOf("a enormous sloth"),
	attackOf("a giagantic pigeon"),
	attackOf("a hostile garlic like creature"),
	attackOf("a zoophagous plant"),
	attackOf("primitive natives"),
	attackOf("robots"),
	attackOf("onion cultists"),
	attackOf("bountyhunters"),
	"Giant is going to crash you with their foot!"
};

const std::vector<std::string> SHOPS = {
	"Welcome to the civilian shop!",
	"Welcome to the military shop!",
	"Welcome to the mechanic!"
};

const std::string MERCHANT = {
	"Merchant: It is uncommon to meet living soul in this area.\n"
	"          Do you want to take a look at my offer?"
};