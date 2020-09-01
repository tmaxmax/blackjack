#pragma once
#include "Card.h"
#include "Player.h"
#include <random>

class Game
{
	std::mt19937 m_dev;
	std::vector<Card> m_stack;
	Player m_player, m_dealer;

	void StackInit();
	friend void PlayerHit(Player&, Game&);
	friend void ::Gameplay(Game&);
	friend void ::Stats(Game&);
	const std::string& Menu(std::string&);
	void Gameplay();
	void Stats();

public:
	Game();
	void Play();
};

