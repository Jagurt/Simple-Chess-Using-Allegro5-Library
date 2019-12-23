#pragma once
#include <allegro5/allegro.h>
#include "Chessman.h"

class Square
{
public:
	Square();
	Square(int x, int y, ALLEGRO_BITMAP * allegro_bitmap);
	~Square();

	void DrawSquare();
	void DrawSquare(ALLEGRO_BITMAP * allegro_bitmap);
	void SquareActivation(bool & whites_turn, Square * &active_square, Square * empty_square, Square *** ALL_SQUARES_HITBOXES, const int & INDEX_OF_ROW_1, const int & INDEX_OF_COLLUMN_1, Square * &black_king_square, Square * &white_king_square);
	void DrawAll();
	void ReceiveChessman(Chessman * chessman);
	void MoveChessman(Square * &active_square, Square * empty_square);
	void SetAll(int x, int y, ALLEGRO_BITMAP * allegro_bitmap);
	void SetBitmap(ALLEGRO_BITMAP * allegro_bitmap);
	void DeactivateSquare(Square * &active_square, Square * empty_square);
	static bool IsKingSafe(Square * active_square, Square * empty_square, Square *** ALL_SQUARES_HITBOXES, const int & INDEX_OF_ROW_1, const int & INDEX_OF_COLLUMN_1);
	bool IsKingSafeAfterMove(Square * active_square, Square * empty_square, Square * king_square, Square *** ALL_SQUARES_HITBOXES, const int & INDEX_OF_ROW_1, const int & INDEX_OF_COLLUMN_1);
	void PromoteWhite( Square *** ALL_SQUARES_HITBOXES, const int & INDEX_OF_ROW_1, const int & INDEX_OF_COLLUMN_1);
	void PromoteBlack(Square *** ALL_SQUARES_HITBOXES, const int & INDEX_OF_ROW_1, const int & INDEX_OF_COLLUMN_1);
	void EndPromotion(Square *** ALL_SQUARES_HITBOXES, const int & INDEX_OF_ROW_1, const int & INDEX_OF_COLLUMN_1);
protected:
	int x;
	int y;
	ALLEGRO_BITMAP * square_bitmap;
	Chessman * chessman;
};

