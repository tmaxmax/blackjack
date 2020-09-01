#pragma once
#ifndef TEO_BLACKJACK_PLAYER_
#define TEO_BLACKJACK_PLAYER
#include <istream>
#include <string>
#include <vector>

class Game;
class Card;

class Player
{
	std::string m_name;
	std::vector<Card> m_cards;
	int m_csum, m_wins, m_draw, m_bust, m_blkj, m_quit;
	bool m_isdealer;
	Game* m_game;

	void ModifyAces();
	friend void PlayerHit(Player&, Game&);
	friend void Gameplay(Game&);
	friend void Stats(Game&);

public:
	Player(bool, Game*, const std::string&);
	void Hit();
	const std::string& Name() const;
	const std::vector<Card>& Cards() const;
	friend std::istream& getline(std::istream&, Player&);

};

#endif // !TEO_BLACKJACK_PLAYER_