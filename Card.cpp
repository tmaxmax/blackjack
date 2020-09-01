#include "Card.h"
#include <algorithm>
#include <limits>

Card::Card(const int& v, Pip p, Suit s)
	: m_numeral(v)
	, m_piptype(p)
	, m_suit(s)
{}

Card::Card(Card&& c) noexcept
	: m_numeral(std::move(c.m_numeral))
	, m_piptype(std::move(c.m_piptype))
	, m_suit(std::move(c.m_suit))
{}

const int& Card::Numeral() const
{
	return m_numeral;
}

Card::operator Pip() const
{
	return m_piptype;
}

Card::operator Suit() const
{
	return m_suit;
}

Card& Card::operator=(const Card& cc)
{
	m_numeral = cc.m_numeral;
	m_piptype = cc.m_piptype;
	m_suit = cc.m_suit;
	return *this;
}

std::istream& operator>>(std::istream& is, Card& cc)
{
	if (cc.m_numeral == 0 && cc.m_suit == Card::Suit::Ace)
		while (cc.m_numeral != 1 && cc.m_numeral != 11)
			while (!(is >> cc.m_numeral))
			{
				is.clear();
				is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			}
	return is;
}

std::ostream& operator<<(std::ostream& os, const Card& cc)
{
	using Pip = Card::Pip;
	using Suit = Card::Suit;

	switch ((Suit)cc)
	{
	case Suit::Numeral:
		os << cc.Numeral();
		break;
	case Suit::Ace:
		os << "As";
		if (cc.Numeral())
			os << " (" << cc.Numeral() << ')';
		break;
	case Suit::King:
		os << "Rege";
		break;
	case Suit::Queen:
		os << "Regina";
		break;
	case Suit::Jack:
		os << "Valet";
		break;
	}

	os << " de ";
	switch ((Pip)cc)
	{
	case Pip::Spades:
		os << "pica";
		break;
	case Pip::Hearts:
		os << "inima rosie";
		break;
	case Pip::Diamonds:
		os << "romb";
		break;
	case Pip::Clubs:
		os << "trefla";
	}

	return os;
}