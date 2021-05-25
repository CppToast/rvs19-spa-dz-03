#ifndef _GAME_OF_LIFE_H_
#define _GAME_OF_LIFE_H_

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
using namespace std;

class game_of_life {

private:
	unsigned int STUPACA;
	unsigned int REDAKA;
	vector<vector<bool>> _generacija;
	vector<vector<bool>> _sljedeca_generacija;
	bool slucajna_vrijednost();
	bool hoce_li_zivjeti(int i, int j);

public:
	game_of_life(int redaka, int stupaca);
	game_of_life();
	bool getCell(int i, int j);
	void setCell(int i, int j, bool value);
	void sljedeca_generacija();
	void randomize();
	void clear();
};

#endif
