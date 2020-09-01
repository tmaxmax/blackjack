#pragma once
#ifndef TEO_BLACKJACK_CARD_
#define TEO_BLACKJACK_CARD_
#include <iostream>

class Card
{
public:
	enum class Pip { Spades, Hearts, Diamonds, Clubs };
	enum class Suit { Numeral, Ace, King, Queen, Jack };

	Card(const int&, Pip, Suit);
	Card(Card&&) noexcept;
	const int& Numeral() const;
	explicit operator Pip() const;
	explicit operator Suit() const;
	Card& operator=(const Card&);
	friend std::istream& operator>>(std::istream&, Card&);
	friend std::ostream& operator<<(std::ostream&, const Card&);

private:
	int m_numeral;
	Pip m_piptype;
	Suit m_suit;
};

#endif // !TEO_BLACKJACK_CARD_