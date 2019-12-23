#include "pch.h"
#include "Square.h"

Square::Square()
{
	this->square_bitmap = al_create_bitmap(0, 0);
	this->chessman = new Chessman();
}

Square::Square(int x, int y, ALLEGRO_BITMAP * allegro_bitmap)
{
	Square();
	this->x = x;
	this->y = y;
	this->square_bitmap = allegro_bitmap;
	DrawSquare();
}

Square::~Square()
{
	
}

void Square::DrawSquare()
{
	al_draw_bitmap(this->square_bitmap, this->x, this->y, 0);
}

void Square::DrawSquare(ALLEGRO_BITMAP * allegro_bitmap)
{
	al_draw_bitmap(allegro_bitmap, this->x, this->y, 0);
}

void Square::SquareActivation(bool & whites_turn, Square * &active_square, Square * empty_square, Square *** ALL_SQUARES_HITBOXES, const int & INDEX_OF_ROW_1, const int & INDEX_OF_COLLUMN_1, Square * &black_king_square, Square * &white_king_square) {
	
	int this_index_x = this->x / 32;
	int this_index_y = this->y / 32;

	//Sprawdzam czy kliknieto w szachownice
	if (this_index_x >= INDEX_OF_COLLUMN_1 && this_index_x <= INDEX_OF_COLLUMN_1 + 7 && this_index_y >= INDEX_OF_ROW_1 && this_index_y <= INDEX_OF_ROW_1 + 7)
	{
		// Sprawdzam jest jakis aktywny kwadrat
		if (active_square != empty_square)
		{
			//jesli klikam zaznaczonego pionka to go odznaczam
			if (active_square->chessman == this->chessman || (this->chessman != empty_square->chessman && this->chessman->IsWhite() == active_square->chessman->IsWhite()))
			{
				DeactivateSquare(active_square, empty_square);
			}
			else
			{
				char active_name = active_square->chessman->GetName();
				
				//Sprawdzam czy krol nie bedzie zagrozony po ruchu
				//Musze sprawdzac to różnie jesli ruszam królem, a innym pionkiem
				// inaczej sie buguje
				if (active_name != 'R')
				{
					if (whites_turn)
					{
						if (!IsKingSafeAfterMove(active_square, empty_square, white_king_square, ALL_SQUARES_HITBOXES, INDEX_OF_ROW_1, INDEX_OF_COLLUMN_1))
						{
							DeactivateSquare(active_square, empty_square);
							return;
						}
					}
					if (!whites_turn)
					{
						if (!IsKingSafeAfterMove(active_square, empty_square, black_king_square, ALL_SQUARES_HITBOXES, INDEX_OF_ROW_1, INDEX_OF_COLLUMN_1))
						{
							DeactivateSquare(active_square, empty_square);
							return;
						}
					}
				}
				
				//Ruch pionka
				if (active_name == 'P')
				{
					const int move_dir = active_square->y / 32 - this->y / 32;
					const bool same_collumns = !(active_square->x / 32 - this->x / 32);
					const bool adjectant_collumns = abs(active_square->x / 32 - this->x / 32) == 1;

					int move_target = 1;
					int starting_row = INDEX_OF_ROW_1 + 6;

					if (!active_square->chessman->IsWhite())
					{
						move_target = -1;
						starting_row -= 5;
					}

					//Klikniecie pola bez pionka, w tej samej kolumnie 
					if (same_collumns && this->chessman == empty_square->chessman)
					{
						//Przy kliknieciu 2 pola dalej
						if (move_dir == move_target * 2)
						{
							//jesli pionek jest na starcie
							if (active_square->y / 32 == starting_row)
							{
								MoveChessman(active_square, empty_square);
								whites_turn = !whites_turn;
							}
						}
						//Przy kliknieciu 1 pole dalej
						else if (move_dir == move_target)
						{
							MoveChessman(active_square, empty_square);
							//Aktywacja promocji pionka przy dotarciu na koniec planszu
							if (this_index_y == INDEX_OF_ROW_1)
							{
								active_square = this;
								PromoteWhite(ALL_SQUARES_HITBOXES, INDEX_OF_ROW_1, INDEX_OF_COLLUMN_1);
								return;
							}
							else if (this_index_y == INDEX_OF_ROW_1 + 7)
							{
								active_square = this;
								PromoteBlack(ALL_SQUARES_HITBOXES, INDEX_OF_ROW_1, INDEX_OF_COLLUMN_1);
								return;
							}
							whites_turn = !whites_turn;
						}
						else
						{
							DeactivateSquare(active_square, empty_square);
						}
					}
					//bicie figury na polu do przodu po skosie
					else if (move_dir == move_target && adjectant_collumns && this->chessman != empty_square->chessman && this->chessman->IsWhite() != active_square->chessman->IsWhite())
					{
						MoveChessman(active_square, empty_square);
						//Aktywacja promocji pionka przy dotarciu na koniec planszu
						if (this_index_y == INDEX_OF_ROW_1)
						{
							active_square = this;
							PromoteWhite(ALL_SQUARES_HITBOXES, INDEX_OF_ROW_1, INDEX_OF_COLLUMN_1);
							return;
						}
						else if (this_index_y == INDEX_OF_ROW_1 + 7)
						{
							active_square = this;
							PromoteBlack(ALL_SQUARES_HITBOXES, INDEX_OF_ROW_1, INDEX_OF_COLLUMN_1);
							return;
						}
						whites_turn = !whites_turn;
					}
					else
					{
						//deaktywacja aktywnego pola
						DeactivateSquare(active_square, empty_square);
					}
				}
				else if (active_name == 'T')
				{
					const bool same_collumns = !(active_square->x / 32 - this->x / 32);
					const bool same_rows = !(active_square->y / 32 - this->y / 32);

					//Jesli kliknieto na pole w tej samej kolumnie
					if (same_collumns)
					{
						int move_dir;
						const int distance = active_square->y / 32 - this->y / 32;
						const int active_square_ind_y = active_square->y / 32;

						//Sprawdzam dystans miedzy aktywna wieza a kliknietym polem
						//i przypisuje odpowiednia wartosc kierunku ruchu
						move_dir = (distance > 0) ? -1 : 1;

						//Jesli sa po drodze pionki to deaktywuj wieze
						for (int i = active_square_ind_y + move_dir; i != active_square_ind_y - distance; i += move_dir)
						{
							if (ALL_SQUARES_HITBOXES[this->x / 32][i]->chessman != empty_square->chessman)
							{
								DeactivateSquare(active_square, empty_square);
								return;
							}
						}
						MoveChessman(active_square, empty_square);
						whites_turn = !whites_turn;
					}
					else if (same_rows)
					{
						int move_dir;
						const int distance = active_square->x / 32 - this->x / 32;
						const int active_square_ind_x = active_square->x / 32;

						//Sprawdzam dystans miedzy aktywna wieza a kliknietym polem
						//i przypisuje odpowiednia wartosc kierunku ruchu
						move_dir = (distance > 0) ? -1 : 1;

						//Jesli sa po drodze pionki to deaktywuj wieze
						for (int i = active_square_ind_x + move_dir; i != active_square_ind_x - distance; i += move_dir)
						{
							if (ALL_SQUARES_HITBOXES[i][this->y / 32]->chessman != empty_square->chessman)
							{
								DeactivateSquare(active_square, empty_square);
								return;
							}
						}

						//Jak wszystko gra to rzusz wieza
						MoveChessman(active_square, empty_square);
						whites_turn = !whites_turn;
					}
					else
					{
						DeactivateSquare(active_square, empty_square);
					}
				}
				else if (active_name == 'K')
				{
					const bool same_collumns = !(active_square->x / 32 - this->x / 32);
					const bool same_rows = !(active_square->y / 32 - this->y / 32);

					const int distance_x = active_square->x / 32 - this->x / 32;
					const int distance_y = active_square->y / 32 - this->y / 32;

					if (!same_collumns && !same_rows && abs(distance_x) + abs(distance_y) == 3)
					{
						MoveChessman(active_square, empty_square);
						whites_turn = !whites_turn;
					}
					else
					{
						DeactivateSquare(active_square, empty_square);
					}
				}
				else if (active_name == 'B')
				{
					const int distance_x = active_square->x / 32 - this->x / 32;
					const int distance_y = active_square->y / 32 - this->y / 32;

					if (abs(distance_x) == abs(distance_y))
					{
						int move_dir_x;
						int move_dir_y;

						const int active_square_ind_x = active_square->x / 32;
						const int active_square_ind_y = active_square->y / 32;

						move_dir_x = (distance_x > 0) ? -1 : 1;
						move_dir_y = (distance_y > 0) ? -1 : 1;

						for (int i = active_square_ind_x + move_dir_x, j = active_square_ind_y + move_dir_y; i != active_square_ind_x - distance_x && j != active_square_ind_y - distance_y; i += move_dir_x, j += move_dir_y)
						{
							if (ALL_SQUARES_HITBOXES[i][j]->chessman != empty_square->chessman)
							{
								DeactivateSquare(active_square, empty_square);
								return;
							}
						}

						MoveChessman(active_square, empty_square);
						whites_turn = !whites_turn;
					}
					else
					{
						DeactivateSquare(active_square, empty_square);
					}
				}
				else if (active_name == 'Q')
				{
					const bool same_collumns = !(active_square->x / 32 - this->x / 32);
					const bool same_rows = !(active_square->y / 32 - this->y / 32);

					const int distance_x = active_square->x / 32 - this->x / 32;
					const int distance_y = active_square->y / 32 - this->y / 32;

					//Jesli kliknieto na pole w tej samej kolumnie
					if (same_collumns)
					{
						int move_dir = 1;
						const int distance = active_square->y / 32 - this->y / 32;
						const int active_square_ind_y = active_square->y / 32;

						//Sprawdzam dystans miedzy aktywna krolowa a kliknietym polem
						//i przypisuje odpowiednia wartosc kierunku ruchu
						move_dir = (distance > 0) ? -1 : 1;

						//Jesli sa po drodze pionki to deaktywuj krolowa
						for (int i = active_square_ind_y + move_dir; i != active_square_ind_y - distance; i += move_dir)
						{
							if (ALL_SQUARES_HITBOXES[this->x / 32][i]->chessman != empty_square->chessman)
							{
								DeactivateSquare(active_square, empty_square);
								return;
							}
						}

						//Jak wszystko gra to rzusz krolowa
						MoveChessman(active_square, empty_square);
						whites_turn = !whites_turn;
					}
					else if (same_rows)
					{
						int move_dir;
						const int distance = active_square->x / 32 - this->x / 32;
						const int active_square_ind_x = active_square->x / 32;

						//Sprawdzam dystans miedzy aktywna krolowa a kliknietym polem 
						//i przypisuje odpowiednia wartosc kierunku ruchu
						move_dir = (distance > 0) ? -1 : 1;

						//Jesli sa po drodze pionki to deaktywuj krolowa
						for (int i = active_square_ind_x + move_dir; i != active_square_ind_x - distance; i += move_dir)
						{
							if (ALL_SQUARES_HITBOXES[i][this->y / 32]->chessman != empty_square->chessman)
							{
								DeactivateSquare(active_square, empty_square);
								return;
							}
						}
						//Jak wszystko gra to rzusz krolowa
						MoveChessman(active_square, empty_square);
						whites_turn = !whites_turn;
					}
					else if (abs(distance_x) == abs(distance_y))
					{
						int move_dir_x;
						int move_dir_y;

						const int active_square_ind_x = active_square->x / 32;
						const int active_square_ind_y = active_square->y / 32;

						move_dir_x = (distance_x > 0) ? -1 : 1;
						move_dir_y = (distance_y > 0) ? -1 : 1;

						for (int i = active_square_ind_x + move_dir_x, j = active_square_ind_y + move_dir_y; i != active_square_ind_x - distance_x && j != active_square_ind_y - distance_y; i += move_dir_x, j += move_dir_y)
						{
							if (ALL_SQUARES_HITBOXES[i][j]->chessman != empty_square->chessman)
							{
								DeactivateSquare(active_square, empty_square);
								return;
							}
						}

						MoveChessman(active_square, empty_square);
						whites_turn = !whites_turn;
					}
					else
					{
						DeactivateSquare(active_square, empty_square);
					}
				}
				else if (active_name == 'R')
				{
					const int distance_x = active_square->x / 32 - this->x / 32;
					const int distance_y = active_square->y / 32 - this->y / 32;

					int divider = 0;

					//Przy ruchu w poziomie albo pionie divider jest rowny 1 
					// a przy ruchu po skosie divider jest rowny 2
					divider += (distance_x) ? 1 : 0;
					divider += (distance_y) ? 1 : 0;

					//Jesli krol ma sie ruszyc o 1 pole w pionie, poziomie albo po skosie
					if (abs(distance_x) + abs(distance_y) / divider == 1)
					{
						//Symuluje ruch i sprawdzam czy krol jest zagrozony
						Chessman * temp = this->chessman;
						this->chessman = active_square->chessman;
						active_square->chessman = empty_square->chessman;
						if (!IsKingSafe(this, empty_square, ALL_SQUARES_HITBOXES, INDEX_OF_ROW_1, INDEX_OF_COLLUMN_1))
						{
							//Cofam symulowany ruch
							active_square->chessman = this->chessman;
							this->chessman = temp;
							DeactivateSquare(active_square, empty_square);
							return;
						}
						//Cofam symulowany ruch
						active_square->chessman = this->chessman;
						this->chessman = temp;

						// Jesli nie ma zadnych zagrozen to krol moze isc
						MoveChessman(active_square, empty_square);

						// Zmieniam wskaznik na krola
						if (whites_turn) white_king_square = this;
						else black_king_square = this;

						whites_turn = !whites_turn;
					}
					else
					{
						DeactivateSquare(active_square, empty_square);
					}
				}
			}
		}
		//Aktywuj odpowiedniego pionka 
		else if(chessman->IsWhite() && whites_turn || !chessman->IsWhite() && !whites_turn && this->chessman != empty_square->chessman)
		{
			active_square = this;
			DrawSquare(empty_square->square_bitmap);
			DrawSquare(this->chessman->GetBitmap());
		}
	}
	//Przy kliknieciu poza plansze gdy promuje sie pionka
	else if (active_square->chessman->GetName() == 'P')
	{
		if ((this->chessman->IsWhite() && active_square->chessman->IsWhite() && active_square->y / 32 == INDEX_OF_ROW_1) || (!this->chessman->IsWhite() && !active_square->chessman->IsWhite() && active_square->y / 32 == INDEX_OF_ROW_1 + 7))
		{
			active_square->ReceiveChessman(this->chessman);
			EndPromotion(ALL_SQUARES_HITBOXES, INDEX_OF_ROW_1, INDEX_OF_COLLUMN_1);
			DeactivateSquare(active_square, empty_square);
			whites_turn = !whites_turn;
		}
	}
	else
	{
		DeactivateSquare(active_square, empty_square);
	}
}

bool Square::IsKingSafe(Square * kings_square, Square * empty_square, Square *** ALL_SQUARES_HITBOXES, const int & INDEX_OF_ROW_1, const int & INDEX_OF_COLLUMN_1)
{
	int this_index_x = kings_square->x / 32;
	int this_index_y = kings_square->y / 32;

	//Sprawdzam czy krol nie bedzie w zasiegu przeciwnego krola
	if (ALL_SQUARES_HITBOXES[this_index_x][this_index_y - 1]->chessman->GetName() == 'R' && ALL_SQUARES_HITBOXES[this_index_x][this_index_y - 1]->chessman->IsWhite() != kings_square->chessman->IsWhite()) {
		return false;
	}
	if (ALL_SQUARES_HITBOXES[this_index_x][this_index_y + 1]->chessman->GetName() == 'R' && ALL_SQUARES_HITBOXES[this_index_x][this_index_y + 1]->chessman->IsWhite() != kings_square->chessman->IsWhite()) {
		return false;
	}
	if (ALL_SQUARES_HITBOXES[this_index_x + 1][this_index_y - 1]->chessman->GetName() == 'R' && ALL_SQUARES_HITBOXES[this_index_x + 1][this_index_y - 1]->chessman->IsWhite() != kings_square->chessman->IsWhite()) {
		return false;
	}
	if (ALL_SQUARES_HITBOXES[this_index_x + 1][this_index_y + 1]->chessman->GetName() == 'R' && ALL_SQUARES_HITBOXES[this_index_x + 1][this_index_y + 1]->chessman->IsWhite() != kings_square->chessman->IsWhite()) {
		return false;
	}
	if (ALL_SQUARES_HITBOXES[this_index_x - 1][this_index_y - 1]->chessman->GetName() == 'R' && ALL_SQUARES_HITBOXES[this_index_x - 1][this_index_y - 1]->chessman->IsWhite() != kings_square->chessman->IsWhite()) {
		return false;
	}
	if (ALL_SQUARES_HITBOXES[this_index_x - 1][this_index_y + 1]->chessman->GetName() == 'R' && ALL_SQUARES_HITBOXES[this_index_x - 1][this_index_y + 1]->chessman->IsWhite() != kings_square->chessman->IsWhite()) {
		return false;
	}
	if (ALL_SQUARES_HITBOXES[this_index_x - 1][this_index_y]->chessman->GetName() == 'R' && ALL_SQUARES_HITBOXES[this_index_x - 1][this_index_y]->chessman->IsWhite() != kings_square->chessman->IsWhite()) {
		return false;
	}
	if (ALL_SQUARES_HITBOXES[this_index_x + 1][this_index_y]->chessman->GetName() == 'R' && ALL_SQUARES_HITBOXES[this_index_x + 1][this_index_y]->chessman->IsWhite() != kings_square->chessman->IsWhite()) {
		return false;
	}

	//Przeciwnych pionkow
	if (kings_square->chessman->IsWhite())
	{
		if (ALL_SQUARES_HITBOXES[this_index_x + 1][this_index_y - 1]->chessman->GetName() == 'P' && !ALL_SQUARES_HITBOXES[this_index_x + 1][this_index_y - 1]->chessman->IsWhite()) {
			return false;
		}
		else if (ALL_SQUARES_HITBOXES[this_index_x - 1][this_index_y - 1]->chessman->GetName() == 'P' && !ALL_SQUARES_HITBOXES[this_index_x - 1][this_index_y - 1]->chessman->IsWhite()) {
			return false;
		}
	}
	else
	{
		if (ALL_SQUARES_HITBOXES[this_index_x + 1][this_index_y + 1]->chessman->GetName() == 'P' && ALL_SQUARES_HITBOXES[this_index_x + 1][this_index_y + 1]->chessman->IsWhite()) {
			return false;
		}
		else if (ALL_SQUARES_HITBOXES[this_index_x - 1][this_index_y + 1]->chessman->GetName() == 'P' && ALL_SQUARES_HITBOXES[this_index_x - 1][this_index_y + 1]->chessman->IsWhite()) {
			return false;
		}
	}

	//Sprawdzam czy pierwsza figura ktora krol zobaczy powyzej od niego nie jest krolowa albo wieza
	for (int i = this_index_y - 1; i >= INDEX_OF_ROW_1; i--)
	{
		if ((ALL_SQUARES_HITBOXES[this_index_x][i]->chessman->GetName() == 'T' || ALL_SQUARES_HITBOXES[this_index_x][i]->chessman->GetName() == 'Q') && ALL_SQUARES_HITBOXES[this_index_x][i]->chessman->IsWhite() != kings_square->chessman->IsWhite())
		{
		return false;
		}
		else if (ALL_SQUARES_HITBOXES[this_index_x][i]->chessman->GetName() != 'e')
		{
			//Jesli zobaczy inna figure to moze isc
			break;
		}
	}
	//To samo ponizej krola
	for (int i = this_index_y + 1; i <= INDEX_OF_ROW_1 + 7; i++)
	{
		if ((ALL_SQUARES_HITBOXES[this_index_x][i]->chessman->GetName() == 'T' || ALL_SQUARES_HITBOXES[this_index_x][i]->chessman->GetName() == 'Q') && ALL_SQUARES_HITBOXES[this_index_x][i]->chessman->IsWhite() != kings_square->chessman->IsWhite())
		{
			return false;
		}
		else if (ALL_SQUARES_HITBOXES[this_index_x][i]->chessman->GetName() != 'e')
		{
			break;
		}
	}
	//Po lewej
	for (int i = this_index_x - 1; i >= INDEX_OF_COLLUMN_1; i--)
	{
		if ((ALL_SQUARES_HITBOXES[i][this_index_y]->chessman->GetName() == 'T' || ALL_SQUARES_HITBOXES[i][this_index_y]->chessman->GetName() == 'Q') && ALL_SQUARES_HITBOXES[i][this_index_y]->chessman->IsWhite() != kings_square->chessman->IsWhite())
		{
			return false;
		}
		else if (ALL_SQUARES_HITBOXES[i][this_index_y]->chessman->GetName() != 'e')
		{
			break;
		}
	}
	// I prawej od niego
	for (int i = this_index_x + 1; i <= INDEX_OF_COLLUMN_1 + 7; i++)
	{
		if ((ALL_SQUARES_HITBOXES[i][this_index_y]->chessman->GetName() == 'T' || ALL_SQUARES_HITBOXES[i][this_index_y]->chessman->GetName() == 'Q') && ALL_SQUARES_HITBOXES[i][this_index_y]->chessman->IsWhite() != kings_square->chessman->IsWhite())
		{
			return false;
		}
		else if (ALL_SQUARES_HITBOXES[i][this_index_y]->chessman->GetName() != 'e')
		{
			break;
		}
	}

	//Podobnie po skosach dla gonca i krolowej
	//prawy dol
	for (int i = this_index_x + 1, j = this_index_y + 1; i <= INDEX_OF_COLLUMN_1 + 7 && j <= INDEX_OF_ROW_1 + 7; i++, j++)
	{
		if ((ALL_SQUARES_HITBOXES[i][j]->chessman->GetName() == 'B' || ALL_SQUARES_HITBOXES[i][j]->chessman->GetName() == 'Q') && ALL_SQUARES_HITBOXES[i][j]->chessman->IsWhite() != kings_square->chessman->IsWhite())
		{
			return false;
		}
		else if (ALL_SQUARES_HITBOXES[i][j]->chessman->GetName() != 'e')
		{
			break;
		}
	}
	//lewy dol
	for (int i = this_index_x - 1, j = this_index_y; i >= INDEX_OF_COLLUMN_1 && j <= INDEX_OF_ROW_1 + 7; i--, j++)
	{
		if ((ALL_SQUARES_HITBOXES[i][j]->chessman->GetName() == 'B' || ALL_SQUARES_HITBOXES[i][j]->chessman->GetName() == 'Q') && ALL_SQUARES_HITBOXES[i][j]->chessman->IsWhite() != kings_square->chessman->IsWhite())
		{
			return false;
		}
		else if (ALL_SQUARES_HITBOXES[i][j]->chessman->GetName() != 'e')
		{
			break;
		}
	}
	//lewa gora
	for (int i = this_index_x - 1, j = this_index_y - 1; i >= INDEX_OF_COLLUMN_1 && j >= INDEX_OF_ROW_1; i--, j--)
	{
		if ((ALL_SQUARES_HITBOXES[i][j]->chessman->GetName() == 'B' || ALL_SQUARES_HITBOXES[i][j]->chessman->GetName() == 'Q') && ALL_SQUARES_HITBOXES[i][j]->chessman->IsWhite() != kings_square->chessman->IsWhite())
		{
			return false;
		}
		else if (ALL_SQUARES_HITBOXES[i][j]->chessman->GetName() != 'e')
		{
			break;
		}
	}
	//prawa gora
	for (int i = this_index_x + 1, j = this_index_y - 1; i <= INDEX_OF_COLLUMN_1 + 7 && j >= INDEX_OF_ROW_1; i++, j--)
	{
		if ((ALL_SQUARES_HITBOXES[i][j]->chessman->GetName() == 'B' || ALL_SQUARES_HITBOXES[i][j]->chessman->GetName() == 'Q') && ALL_SQUARES_HITBOXES[i][j]->chessman->IsWhite() != kings_square->chessman->IsWhite())
		{
			return false;
		}
		else if (ALL_SQUARES_HITBOXES[i][j]->chessman->GetName() != 'e')
		{
			break;
		}
	}

	//Konie
	if (ALL_SQUARES_HITBOXES[this_index_x - 2][this_index_y - 1]->chessman->GetName() == 'K' && ALL_SQUARES_HITBOXES[this_index_x - 2][this_index_y - 1]->chessman->IsWhite() != kings_square->chessman->IsWhite()) {
		return false;
	}
	if (ALL_SQUARES_HITBOXES[this_index_x - 1][this_index_y - 2]->chessman->GetName() == 'K' && ALL_SQUARES_HITBOXES[this_index_x - 1][this_index_y - 2]->chessman->IsWhite() != kings_square->chessman->IsWhite()) {
		return false;
	}
	if (ALL_SQUARES_HITBOXES[this_index_x + 1][this_index_y - 2]->chessman->GetName() == 'K' && ALL_SQUARES_HITBOXES[this_index_x + 1][this_index_y - 2]->chessman->IsWhite() != kings_square->chessman->IsWhite()) {
		return false;
	}
	if (ALL_SQUARES_HITBOXES[this_index_x + 2][this_index_y - 1]->chessman->GetName() == 'K' && ALL_SQUARES_HITBOXES[this_index_x + 2][this_index_y - 1]->chessman->IsWhite() != kings_square->chessman->IsWhite()) {
		return false;
	}
	if (ALL_SQUARES_HITBOXES[this_index_x + 2][this_index_y + 1]->chessman->GetName() == 'K' && ALL_SQUARES_HITBOXES[this_index_x + 2][this_index_y + 1]->chessman->IsWhite() != kings_square->chessman->IsWhite()) {
		return false;
	}
	if (ALL_SQUARES_HITBOXES[this_index_x + 1][this_index_y + 2]->chessman->GetName() == 'K' && ALL_SQUARES_HITBOXES[this_index_x + 1][this_index_y + 2]->chessman->IsWhite() != kings_square->chessman->IsWhite()) {
		return false;
	}
	if (ALL_SQUARES_HITBOXES[this_index_x - 1][this_index_y + 2]->chessman->GetName() == 'K' && ALL_SQUARES_HITBOXES[this_index_x - 1][this_index_y + 2]->chessman->IsWhite() != kings_square->chessman->IsWhite()) {
		return false;
	}
	if (ALL_SQUARES_HITBOXES[this_index_x - 2][this_index_y + 1]->chessman->GetName() == 'K' && ALL_SQUARES_HITBOXES[this_index_x - 2][this_index_y + 1]->chessman->IsWhite() != kings_square->chessman->IsWhite()) {
		return false;
	}

	return true;
}

void Square::DrawAll()
{
	DrawSquare(this->square_bitmap);
	DrawSquare(this->chessman->GetBitmap());
}

void Square::ReceiveChessman(Chessman * chessman)
{
	this->chessman = chessman;
	al_draw_bitmap(this->chessman->GetBitmap(), this->x, this->y, 0);
}

void Square::SetAll(int x, int y, ALLEGRO_BITMAP * allegro_bitmap)
{
	this->x = x;
	this->y = y;
	this->square_bitmap = allegro_bitmap;
}

void Square::SetBitmap(ALLEGRO_BITMAP * allegro_bitmap)
{
	this->square_bitmap = allegro_bitmap;
}

void Square::MoveChessman(Square * &active_square, Square * empty_square)
{
	this->chessman = active_square->chessman;
	this->DrawAll();

	active_square->chessman = empty_square->chessman;
	active_square->DrawSquare();
	active_square = empty_square;
}

void Square::DeactivateSquare(Square * &active_square, Square * empty_square)
{
	active_square->DrawAll();
	active_square = empty_square;
}

bool Square::IsKingSafeAfterMove(Square * active_square, Square * empty_square, Square * king_square, Square *** ALL_SQUARES_HITBOXES, const int & INDEX_OF_ROW_1, const int & INDEX_OF_COLLUMN_1)
{
	//Symuluje ruch i sprawdzam czy krol jest zagrozony
	bool result = true;
	Chessman * temp = this->chessman;
	this->chessman = active_square->chessman;
	active_square->chessman = empty_square->chessman;
	if (!IsKingSafe(king_square, empty_square, ALL_SQUARES_HITBOXES, INDEX_OF_ROW_1, INDEX_OF_COLLUMN_1))
	{
		result = false;
	}
	//Cofam symulowany ruch
	active_square->chessman = this->chessman;
	this->chessman = temp;

	return result;
}

//Aktywuje kwadraty pokazujace biale pionki do promocji
void Square::PromoteWhite(Square *** ALL_SQUARES_HITBOXES, const int & INDEX_OF_ROW_1, const int & INDEX_OF_COLLUMN_1)
{
	ALL_SQUARES_HITBOXES[INDEX_OF_COLLUMN_1 + 2][INDEX_OF_ROW_1 + 11]->DrawAll();
	ALL_SQUARES_HITBOXES[INDEX_OF_COLLUMN_1 + 3][INDEX_OF_ROW_1 + 11]->DrawAll();
	ALL_SQUARES_HITBOXES[INDEX_OF_COLLUMN_1 + 4][INDEX_OF_ROW_1 + 11]->DrawAll();
	ALL_SQUARES_HITBOXES[INDEX_OF_COLLUMN_1 + 5][INDEX_OF_ROW_1 + 11]->DrawAll();
}

//Aktywuje kwadraty pokazujace czarne pionki do promocji
void Square::PromoteBlack(Square *** ALL_SQUARES_HITBOXES, const int & INDEX_OF_ROW_1, const int & INDEX_OF_COLLUMN_1)
{
	ALL_SQUARES_HITBOXES[INDEX_OF_COLLUMN_1 + 2][INDEX_OF_ROW_1 + 10]->DrawAll();
	ALL_SQUARES_HITBOXES[INDEX_OF_COLLUMN_1 + 3][INDEX_OF_ROW_1 + 10]->DrawAll();
	ALL_SQUARES_HITBOXES[INDEX_OF_COLLUMN_1 + 4][INDEX_OF_ROW_1 + 10]->DrawAll();
	ALL_SQUARES_HITBOXES[INDEX_OF_COLLUMN_1 + 5][INDEX_OF_ROW_1 + 10]->DrawAll();
}

void Square::EndPromotion(Square *** ALL_SQUARES_HITBOXES, const int & INDEX_OF_ROW_1, const int & INDEX_OF_COLLUMN_1)
{
	ALL_SQUARES_HITBOXES[INDEX_OF_COLLUMN_1 + 2][INDEX_OF_ROW_1 + 11]->DrawSquare();
	ALL_SQUARES_HITBOXES[INDEX_OF_COLLUMN_1 + 3][INDEX_OF_ROW_1 + 11]->DrawSquare();
	ALL_SQUARES_HITBOXES[INDEX_OF_COLLUMN_1 + 4][INDEX_OF_ROW_1 + 11]->DrawSquare();
	ALL_SQUARES_HITBOXES[INDEX_OF_COLLUMN_1 + 5][INDEX_OF_ROW_1 + 11]->DrawSquare();
	ALL_SQUARES_HITBOXES[INDEX_OF_COLLUMN_1 + 2][INDEX_OF_ROW_1 + 10]->DrawSquare();
	ALL_SQUARES_HITBOXES[INDEX_OF_COLLUMN_1 + 3][INDEX_OF_ROW_1 + 10]->DrawSquare();
	ALL_SQUARES_HITBOXES[INDEX_OF_COLLUMN_1 + 4][INDEX_OF_ROW_1 + 10]->DrawSquare();
	ALL_SQUARES_HITBOXES[INDEX_OF_COLLUMN_1 + 5][INDEX_OF_ROW_1 + 10]->DrawSquare();
}