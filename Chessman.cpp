#include "pch.h"
#include "Chessman.h"

Chessman::Chessman()
{
	this->chessman_bitmap = al_create_bitmap(0, 0);
}

Chessman::Chessman(ALLEGRO_BITMAP * chessman_bitmap, bool white, char name)
{
	this->chessman_bitmap = chessman_bitmap;
	this->white = white;
	this->name = name;
}

Chessman::~Chessman()
{
	
}

ALLEGRO_BITMAP * Chessman::GetBitmap()
{
	return chessman_bitmap;
}

bool Chessman::IsWhite()
{
	return this->white;
}

char Chessman::GetName()
{
	return this->name;
}