#pragma once
#include <allegro5/allegro.h>

class Chessman
{
public:
	Chessman();
	Chessman(ALLEGRO_BITMAP * chessman_bitmap, bool white, char name);
	~Chessman();
	ALLEGRO_BITMAP * GetBitmap();
	char GetName();
	bool IsWhite();
private:
	ALLEGRO_BITMAP * chessman_bitmap;
	bool white;
	char name;
};