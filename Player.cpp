#include "Player.h"
#include "Game.h"
#include <algorithm>

void ReadCard(Card& card)
{
	while (!(std::cin >> card))
	{
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::cout << "Valoare invalida, incearca din nou: ";
	}
}

void Player::ModifyAces()
{
	for (auto&& card : m_cards)
		if ((Card::Suit)card == Card::Suit::Ace)
		{
			std::cout << "Introdu valoarea asului: ";
			ReadCard(card);
		}
	for (const auto& card : m_cards)
		if ((Card::Suit)card == Card::Suit::Ace)
			m_csum += card.Numeral();
	std::cin.ignore();
}

Player::Player(bool dealer, Game* current_game, const std::string& name = "")
	: m_name(name)
	, m_cards()
	, m_csum(0)
	, m_wins(0)
	, m_draw(0)
	, m_bust(0)
	, m_blkj(0)
	, m_quit(0)
	, m_isdealer(dealer)
	, m_game(current_game)
{}

void Player::Hit() { PlayerHit(*this, *m_game); }

const std::string& Player::Name() const
{
	return m_name;
}

const std::vector<Card>& Player::Cards() const
{
	return m_cards;
}

std::istream& getline(std::istream& is, Player& p)
{
	getline(is, p.m_name);
	return is;
}