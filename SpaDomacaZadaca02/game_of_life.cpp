#include "game_of_life.h"
#include <ctime>

bool game_of_life::slucajna_vrijednost() {
	return rand() % 4 == 0; // jer je sansa 25%
}

bool game_of_life::hoce_li_zivjeti(int i, int j) {
	// koristim '%' kao implementaciju wrap-arounda

	// Zato jer '%' nije direktna implementacija funkcije
	// modulo, nego je samo ostatak pri dijeljenju,
	// ne ponasa se isto sa negativnim brojevima.
	// Iz tog razloga prije provjere povecavam koordinate.

	int ti = i + REDAKA;
	int tj = j + STUPACA;

	int susjeda =
		_generacija [(ti - 1) % REDAKA] [ tj      % STUPACA] +
		_generacija [(ti + 1) % REDAKA] [ tj      % STUPACA] +
		_generacija [ ti      % REDAKA] [(tj - 1) % STUPACA] +
		_generacija [ ti      % REDAKA] [(tj + 1) % STUPACA] +
		_generacija [(ti - 1) % REDAKA] [(tj - 1) % STUPACA] +
		_generacija [(ti + 1) % REDAKA] [(tj - 1) % STUPACA] +
		_generacija [(ti - 1) % REDAKA] [(tj + 1) % STUPACA] +
		_generacija [(ti + 1) % REDAKA] [(tj + 1) % STUPACA];

	bool zivi;

	switch (susjeda) {
	case 2:
		zivi = _generacija[i][j];
		break;
	case 3:
		zivi = true;
		break;
	default:
		zivi = false;
		break;
	}

	return zivi;
}

game_of_life::game_of_life(int redaka, int stupaca) {
	REDAKA = redaka;
	STUPACA = stupaca;

	_generacija.resize(REDAKA);
	_sljedeca_generacija.resize(REDAKA);
	for (size_t i = 0; i < REDAKA; i++) {
		_generacija[i].resize(STUPACA);
		_sljedeca_generacija[i].resize(STUPACA);
	}
}

void game_of_life::randomize() {
	for (size_t i = 0; i < REDAKA; i++) {
		for (size_t j = 0; j < STUPACA; j++) {
			_generacija[i][j] = slucajna_vrijednost();
		}
	}
}

void game_of_life::clear() {
	for (size_t i = 0; i < REDAKA; i++) {
		for (size_t j = 0; j < STUPACA; j++) {
			_generacija[i][j] = false;
		}
	}
}

void game_of_life::sljedeca_generacija() {
	for (size_t i = 0; i < REDAKA; i++) {
		for (size_t j = 0; j < STUPACA; j++) {
			_sljedeca_generacija[i][j] = hoce_li_zivjeti(i, j);
		}
	}
	for (size_t i = 0; i < REDAKA; i++) {
		for (size_t j = 0; j < STUPACA; j++) {
			_generacija[i][j] = _sljedeca_generacija[i][j];
		}
	}
}

bool game_of_life::getCell(int i, int j) {
	return _generacija[i][j];
}

void game_of_life::setCell(int i, int j, bool value) {
	_generacija[i][j] = value;
}

game_of_life::game_of_life() {
	STUPACA = 0;
	REDAKA = 0;
	_generacija = {};
	_sljedeca_generacija = {};
}

