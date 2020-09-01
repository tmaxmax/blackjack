#include "System.h"
#include "Game.h"
#include <algorithm>
#include <functional>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <limits>

#ifdef max
#define __temp max
#undef max
#endif // max

System sys;
auto com{ [](std::string& command) {
		command.clear();
		while (command.empty())
			getline(std::cin, command);
		for (char& c : command)
			c = tolower(c);
	} };

void PlayerHit(Player& player, Game& game)
{
	game.StackInit();
	player.m_cards.push_back(std::move(game.m_stack.back()));
	game.m_stack.pop_back();
	if ((Card::Suit)player.m_cards.back() != Card::Suit::Ace)
		player.m_csum += player.m_cards.back().Numeral();
}

void Gameplay(Game& game)
{
	static std::string command, hit("hit"), stand("stand"), surrender("surrender");
	static std::uniform_int_distribution<int> dist(1, 100);
	static auto chance{ std::bind(dist, game.m_dev) };
	static auto dealerhit{ [&game]() mutable {
		game.m_dealer.Hit();
		if ((Card::Suit)game.m_dealer.Cards().back() == Card::Suit::Ace)
		{
			std::stringstream ss;
			if (game.m_dealer.m_csum + 11 > 21)
				ss << 1;
			else
				ss << 11;
			ss >> game.m_dealer.m_cards.back();
			game.m_dealer.m_csum += game.m_dealer.Cards().back().Numeral();
		}
	} };
	static auto cards{ [&game]() {
		std::cout << "Cartile tale:";
		for (const auto& card : game.m_player.Cards())
			std::cout << '\n' << card;
		std::cout << "\n\nCartile dealerului:";
		for (const auto& card : game.m_dealer.Cards())
			std::cout << '\n' << card;
	} };
	static auto& player{ game.m_player };
	static auto& dealer{ game.m_dealer };

	std::cout << "Introdu " << std::quoted("hit") << " pentru a lua o carte;"
		<< "\nIntrodu " << std::quoted("stand") << " pentru a nu mai extrage;"
		<< "\nIntrodu " << std::quoted("surrender") << " pentru a te da batut."
		<< "\nAtentie! Te poti da batut numai daca ai 2 sau mai multe carti."
		<< "\nValorile asilor le stabilesti dupa ce ai extras numarul de carti dorite.\n";
	do
	{
		std::cout << '\n' << std::quoted("hit") << ", " << std::quoted("stand") << " sau " << std::quoted("surrender") << "? ";
		com(command);
		sys.cls();
		if (command != surrender)
		{
			if (command == hit)
			{
				player.Hit();
				if (dealer.m_csum <= 16)
					dealerhit();
				std::cout << "Ai extras:";
				for (const auto& card : player.Cards())
					std::cout << '\n' << card;
				std::cout << "\nSuma cartilor tale (fara asi) este: " << player.m_csum << '.';
				if (player.m_csum > 21)
				{
					std::stringstream ss;
					for (auto& as : player.m_cards)
						if ((Card::Suit)as == Card::Suit::Ace)
						{
							ss << 1;
							ss >> as;
						}
					command = stand;
					std::cin.get();
					sys.cls();
				}
			}
			else if (command == stand)
			{
				if (player.Cards().size() == 0)
					std::cout << "Nu ai luat nicio carte!", command.clear();
			}
		}
		else
		{
			if (player.Cards().size() < 2)
				std::cout << "Nu te poti da batut inca!", command.clear();
		}
	} while (command != stand && command != surrender);

	if (command == surrender)
	{
		std::cout << "Te-ai dat batut!\n";
		++player.m_quit;
		++dealer.m_wins;
		cards();
	}
	else
	{
		while (dealer.m_csum <= 16)
			dealerhit();
		if (dealer.m_csum <= 19)
			if (chance() > 80)
				dealerhit();

		if (player.m_csum <= 21)
		{
			bool aces{ false };
			std::cout << "Asii tai:";
			for (const auto& card : player.Cards())
				if ((Card::Suit)card == Card::Suit::Ace)
				{
					std::cout << '\n' << card;
					aces = true;
				}
			std::cout << '\n';
			if (aces)
				player.ModifyAces();
			else
				std::cout << "Nu ai asi!\n";
			std::cout << "Apasa ENTER pentru a afla castigatorul . . . ";
			std::cin.get();
			sys.cls();
		}

		if (player.m_csum == dealer.m_csum)
		{
			std::cout << "Este egalitate! Punctaj: " << player.m_csum << '.';
			if (player.m_csum <= 21)
			{
				if (player.m_csum == 21)
				{
					std::cout << "\nBlackjack!";
					++player.m_blkj;
					++dealer.m_blkj;
				}
				++player.m_draw;
				++dealer.m_draw;
			}
			else
			{
				std::cout << "\nBust!";
				++player.m_bust;
				++dealer.m_bust;
			}
		}
		else
		{
			if (player.m_csum > 21)
			{
				std::cout << "Bust!";
				if (dealer.m_csum > 21)
				{
					std::cout << " Dealerul are, insa, " << dealer.m_csum << " puncte.";
					++dealer.m_bust;
				}
				++player.m_bust;
			}
			else if (player.m_csum == 21)
			{
				std::cout << "Blackjack, " << player.Name() << ", ai 21 de puncte! "
					<< "Dealerul are " << dealer.m_csum << '.';
				++player.m_wins;
				++player.m_blkj;
				++dealer.m_bust;
			}
			else
			{
				if (dealer.m_csum > 21)
				{
					std::cout << "Ai castigat, " << player.Name() << ", cu " << player.m_csum << " puncte!"
						<< "\nDealerul are " << dealer.m_csum << '.';
					++player.m_wins;
					++dealer.m_bust;
				}
				else if (dealer.m_csum == 21)
				{
					std::cout << "Ai pierdut, dealerul are Blackjack!\nPunctajul tau: " << player.m_csum << '.';
					++player.m_bust;
					++dealer.m_blkj;
					++dealer.m_wins;
				}
				else
				{
					if (player.m_csum > dealer.m_csum)
					{
						std::cout << "Ai castigat, " << player.Name() << ", cu " << player.m_csum << " puncte!"
							<< "\nDealerul are " << dealer.m_csum << '.';
						++player.m_wins;
						++dealer.m_bust;
					}
					else
					{
						std::cout << "Ai pierdut cu " << player.m_csum << " puncte. Dealerul are " << dealer.m_csum << '.';
						++player.m_bust;
						++dealer.m_wins;
					}
				}
			}
		}

		std::cout << "\n\n";
		cards();
	}
	
	player.m_cards.clear();
	dealer.m_cards.clear();
	player.m_csum = 0;
	dealer.m_csum = 0;

	std::cout << '\n';
	sys.hang();
}

void Stats(Game& game)
{
	const auto& player{ game.m_player };
	const auto& dealer{ game.m_dealer };
	auto stats{ [](const Player& player) {
		std::cout << "\nJucator: " << player.Name() << ';'
		<< "\nRunde castigate: " << player.m_wins << ';'
		<< "\nRunde busted: " << player.m_bust << ';'
		<< "\nBlackjackuri obtinute: " << player.m_blkj << ';'
		<< "\nAbandonuri: " << player.m_quit << ".\n";
	} };

	std::cout << "STATS:\n";
	stats(player);
	stats(dealer);
	std::cin.get();
	sys.cls();
}

void Game::StackInit()
{
	using Pip = Card::Pip;
	using Suit = Card::Suit;
	static Pip pips[]{ Pip::Spades, Pip::Hearts, Pip::Diamonds, Pip::Clubs };
	static Suit suits[]{ Suit::Numeral, Suit::Ace, Suit::King, Suit::Queen, Suit::Jack };

	if (m_stack.empty())
	{
		for (const auto& pip : pips)
			for (const auto& suit : suits)
				switch (suit)
				{
				case Suit::Numeral:
					for (int i{ 2 }; i <= 10; i++)
						m_stack.push_back(std::move(Card(i, pip, Suit::Numeral)));
					break;
				case Suit::Ace:
					m_stack.push_back(std::move(Card(0, pip, Suit::Ace)));
					break;
				default:
					m_stack.push_back(std::move(Card(10, pip, suit)));
					break;
				}
		std::shuffle(std::begin(m_stack), std::end(m_stack), m_dev);
	}
}

const std::string& Game::Menu(std::string& command)
{
	sys.cls();
	std::cout << "MENIU:"
		<< "\n - (" << std::quoted("play") << ") Joaca!"
		<< "\n - (" << std::quoted("exit") << ") Iesire."
		<< "\nIntrodu o optiune: ";
	com(command);
	sys.cls();
	return command;
}

void Game::Gameplay() { ::Gameplay(*this); }
void Game::Stats() { ::Stats(*this); }

Game::Game()
	: m_dev(std::random_device{}())
	, m_stack()
	, m_player(false, this, "")
	, m_dealer(true, this, "Dealer")
{
	StackInit();
	std::cout << "BLACKJACK 21\nBun venit la unicul joc corect din oras, unde cartile sunt amestecate aleator si nimeni nu te pacaleste!"
		<< "\n\nSpune-ne numele tau!\n\t";
	getline(std::cin, m_player);
	std::cout << "Salut, " << m_player.Name() << "! Jocul este pregatit, apasa ENTER cand esti gata!\n";
	std::cin.get();
}

void Game::Play()
{
	std::string command, exit("exit"), play("play");
	while (Menu(command) != exit)
		if (command == play)
			Gameplay();
	Stats();
	sys.hang();
}

#ifdef __temp
#define max __temp
#undef __temp
#endif // __temp