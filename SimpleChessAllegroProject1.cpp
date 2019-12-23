// SimpleChessAllegroProject1.cpp : Ten plik zawiera funkcję „main”. W nim rozpoczyna się i kończy wykonywanie programu.
// https://wiki.allegro.cc/index.php?title=Main_Page wszystkie informacje o allegro można znaleźć tutaj.
// Nie ma warunku na Szach Mata booooo nie chcialo mi się go robic i trzeba sie domyslic

#include "pch.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <stdio.h>
#include "Chessman.h"
#include "Square.h"

ALLEGRO_BITMAP * load_bitmaps(int bitmap_number);

int main()
{
	const int SCREEN_W = 640;
	const int SCREEN_H = 480;
	const int SQUARE_SIZE = 32;
	const int NUM_OF_BITMAPS = 31;
	const int CHESS_BOARD_START_POINT_X = SCREEN_W / 2 - SQUARE_SIZE * 4;
	const int CHESS_BOARD_START_POINT_Y = SCREEN_H * 0.4f - SQUARE_SIZE * 4;

	const int ALL_SQUARES_W = SCREEN_W / SQUARE_SIZE;
	const int ALL_SQUARES_H = SCREEN_H / SQUARE_SIZE;

	const int INDEX_OF_ROW_1 = CHESS_BOARD_START_POINT_Y / SQUARE_SIZE;
	const int INDEX_OF_COLLUMN_1 = CHESS_BOARD_START_POINT_X / SQUARE_SIZE;
	bool whites_turn = true;

	//inicjalizacja Allegro
	al_init();
	al_init_image_addon();

	ALLEGRO_DISPLAY * display = NULL;
	ALLEGRO_EVENT_QUEUE * event_queue = NULL;

	//Robie tablice wskaznikow do bitmap, zeby moc je potem latwo skasowac
	ALLEGRO_BITMAP * bitmap_tab[NUM_OF_BITMAPS];

	if (!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}

	if (!al_install_mouse()) {
		fprintf(stderr, "failed to initialize the mouse!\n");
		return -1;
	}

	display = al_create_display(SCREEN_W, SCREEN_H);
	if (!display) {
		fprintf(stderr, "failed to create display!\n");
		return -1;
	}

	event_queue = al_create_event_queue();
	if (!event_queue) {
		fprintf(stderr, "failed to create event_queue!\n");
		for (int i = 0; i < NUM_OF_BITMAPS; i++)
		{
			al_destroy_bitmap(bitmap_tab[i]);
		}
		al_destroy_bitmap(*bitmap_tab);
		al_destroy_display(display);
		return -1;
	}

	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_mouse_event_source());

	//Laduje wszystkie bitmapy z podfolderu: "data/bitmaps/"
	for (int i = 0; i < NUM_OF_BITMAPS; i++)
	{
		bitmap_tab[i] = load_bitmaps(i);
		if (!bitmap_tab[i]){
			al_show_native_message_box(display, "Error", "Error", "Failed to load bitmaps! Exiting game.", NULL, ALLEGRO_MESSAGEBOX_ERROR);
			for (int j = 0; j < i; j++)
			{
				al_destroy_bitmap(bitmap_tab[j]);
			}
			al_destroy_display(display);
			return 0;
		}

		//nadaje przezroczystosc rozowym tlom bitmap pionkow
		if (i >= 19){
			al_convert_mask_to_alpha(bitmap_tab[i], al_map_rgb(255, 0, 255));
		}
	}

	//Robie zielona bitmape do wskazywania aktywnych pionkow
	ALLEGRO_BITMAP * active_bitmap = al_create_bitmap(SQUARE_SIZE, SQUARE_SIZE);
	al_set_target_bitmap(active_bitmap);
	al_clear_to_color(al_map_rgb(0, 255, 0));
	al_set_target_bitmap(al_get_backbuffer(display));

	//Ustawiam wzor kwadratu poza granicami ekranu
	Square * empty_square = new Square(-SQUARE_SIZE, -SQUARE_SIZE, active_bitmap);
	Chessman * empty_chessman = new Chessman(bitmap_tab[1], false, 'e');
	empty_square->ReceiveChessman(empty_chessman);

	//Tworze pionki
	Chessman * black_pawn = new Chessman(bitmap_tab[19], false, 'P');
	Chessman * white_pawn = new Chessman(bitmap_tab[25], true, 'P');
	Chessman * black_tower= new Chessman(bitmap_tab[20], false, 'T');
	Chessman * white_tower = new Chessman(bitmap_tab[26], true, 'T');
	Chessman * black_knight= new Chessman(bitmap_tab[21], false, 'K');
	Chessman * white_knight= new Chessman(bitmap_tab[27], true, 'K');
	Chessman * black_bishop= new Chessman(bitmap_tab[22], false, 'B');
	Chessman * white_bishop= new Chessman(bitmap_tab[28], true, 'B');
	Chessman * black_queen= new Chessman(bitmap_tab[23], false, 'Q');
	Chessman * white_queen= new Chessman(bitmap_tab[29], true, 'Q');
	Chessman * black_king = new Chessman(bitmap_tab[24], false, 'R');
	Chessman * white_king = new Chessman(bitmap_tab[30], true, 'R');

	Square * active_square = empty_square;

	//Tworze dwuwymiarowa tablice do wskaznikow na kwadraty 
	Square *** ALL_SQUARES_HITBOXES = new Square **[ALL_SQUARES_W];
	for (int i = 0; i < ALL_SQUARES_W; i++)
	{
		ALL_SQUARES_HITBOXES[i] = new Square *[ALL_SQUARES_H];
	}
	for (int i = 0; i < ALL_SQUARES_W; i++)
	{
		for (int j = 0; j < ALL_SQUARES_H; j++)
		{
			ALL_SQUARES_HITBOXES[i][j] = new Square(i * SQUARE_SIZE, j * SQUARE_SIZE, bitmap_tab[1]);
			ALL_SQUARES_HITBOXES[i][j]->ReceiveChessman(empty_chessman);
		}
	}

	//Rysuje plansze
	for (int i = INDEX_OF_COLLUMN_1, k = i + 8; i < k; i++)
	{
		for (int j = INDEX_OF_ROW_1, l = j + 8; j < l; j++)
		{
			int draw_position_x = SQUARE_SIZE * j;
			int draw_position_y = SQUARE_SIZE * i;

			if ((i + j + 1) % 2 == 0)
			{
				ALL_SQUARES_HITBOXES[i][j]->SetBitmap(bitmap_tab[1]);
				ALL_SQUARES_HITBOXES[i][j]->DrawSquare();
			}
			else
			{
				ALL_SQUARES_HITBOXES[i][j]->SetBitmap(bitmap_tab[0]);
				ALL_SQUARES_HITBOXES[i][j]->DrawSquare();
			}
		}
	}
		//litery po lewej
	for (int i = 0; i < 8; i++)
	{
		int draw_position_x = CHESS_BOARD_START_POINT_X + SQUARE_SIZE * i;
		int draw_position_y = CHESS_BOARD_START_POINT_Y - SQUARE_SIZE / 2;
		al_draw_bitmap(bitmap_tab[i + 10], draw_position_x, draw_position_y, 0);
	}
		//litery po prawej
	for (int i = 0; i < 8; i++)
	{
		int draw_position_x = CHESS_BOARD_START_POINT_X + SQUARE_SIZE * i;
		int draw_position_y = CHESS_BOARD_START_POINT_Y + SQUARE_SIZE * 8;
		al_draw_bitmap(bitmap_tab[i + 10], draw_position_x, draw_position_y, 0);
	}
		//liczby u gory
	for (int i = 0; i < 8; i++)
	{
		int draw_position_x = CHESS_BOARD_START_POINT_X - SQUARE_SIZE / 2;
		int draw_position_y = CHESS_BOARD_START_POINT_Y + SQUARE_SIZE * i;
		al_draw_bitmap(bitmap_tab[i + 2], draw_position_x, draw_position_y, 0);
	}
		//liczby na dole
	for (int i = 0; i < 8; i++)
	{
		int draw_position_x = CHESS_BOARD_START_POINT_X + SQUARE_SIZE * 8;
		int draw_position_y = CHESS_BOARD_START_POINT_Y + SQUARE_SIZE * i;
		al_draw_bitmap(bitmap_tab[i + 2], draw_position_x, draw_position_y, 0);
	}

		//Narozniki planszy
	al_draw_bitmap(bitmap_tab[18], CHESS_BOARD_START_POINT_X - SQUARE_SIZE / 2, CHESS_BOARD_START_POINT_Y - SQUARE_SIZE / 2, 0);
	al_draw_bitmap(bitmap_tab[18], CHESS_BOARD_START_POINT_X + SQUARE_SIZE * 8, CHESS_BOARD_START_POINT_Y - SQUARE_SIZE / 2, 0);
	al_draw_bitmap(bitmap_tab[18], CHESS_BOARD_START_POINT_X - SQUARE_SIZE / 2, CHESS_BOARD_START_POINT_Y + SQUARE_SIZE * 8, 0);
	al_draw_bitmap(bitmap_tab[18], CHESS_BOARD_START_POINT_X + SQUARE_SIZE * 8, CHESS_BOARD_START_POINT_Y + SQUARE_SIZE * 8, 0);

	//Koncze rysowac plansze
	//Ustawiam figury
	//Kwadraty wskazuja albo na szachowa figure albo na "pusta" figure

		//Czarne pionki
	for (int i = INDEX_OF_COLLUMN_1; i < INDEX_OF_COLLUMN_1 + 8; i++)
	{
		ALL_SQUARES_HITBOXES[i][INDEX_OF_ROW_1 + 1]->ReceiveChessman(black_pawn);
	}
		//Biale pionki
	for (int i = INDEX_OF_COLLUMN_1; i < INDEX_OF_COLLUMN_1 + 8; i++)
	{
		ALL_SQUARES_HITBOXES[i][INDEX_OF_ROW_1 + 6]->ReceiveChessman(white_pawn);
	}
		//Czarne wieze
	ALL_SQUARES_HITBOXES[INDEX_OF_COLLUMN_1][INDEX_OF_ROW_1]->ReceiveChessman(black_tower);
	ALL_SQUARES_HITBOXES[INDEX_OF_COLLUMN_1 + 7][INDEX_OF_ROW_1]->ReceiveChessman(black_tower);
		//Biale wieze
	ALL_SQUARES_HITBOXES[INDEX_OF_COLLUMN_1][INDEX_OF_ROW_1 + 7]->ReceiveChessman(white_tower);
	ALL_SQUARES_HITBOXES[INDEX_OF_COLLUMN_1 + 7][INDEX_OF_ROW_1 + 7]->ReceiveChessman(white_tower);
		//Czarne Konie
	ALL_SQUARES_HITBOXES[INDEX_OF_COLLUMN_1 + 1][INDEX_OF_ROW_1]->ReceiveChessman(black_knight);
	ALL_SQUARES_HITBOXES[INDEX_OF_COLLUMN_1 + 6][INDEX_OF_ROW_1]->ReceiveChessman(black_knight);
		//Biale Konie
	ALL_SQUARES_HITBOXES[INDEX_OF_COLLUMN_1 + 1][INDEX_OF_ROW_1 + 7]->ReceiveChessman(white_knight);
	ALL_SQUARES_HITBOXES[INDEX_OF_COLLUMN_1 + 6][INDEX_OF_ROW_1 + 7]->ReceiveChessman(white_knight);
		//Czarne Gonce
	ALL_SQUARES_HITBOXES[INDEX_OF_COLLUMN_1 + 2][INDEX_OF_ROW_1]->ReceiveChessman(black_bishop);
	ALL_SQUARES_HITBOXES[INDEX_OF_COLLUMN_1 + 5][INDEX_OF_ROW_1]->ReceiveChessman(black_bishop);
		//Biale Gonce
	ALL_SQUARES_HITBOXES[INDEX_OF_COLLUMN_1 + 2][INDEX_OF_ROW_1 + 7]->ReceiveChessman(white_bishop);
	ALL_SQUARES_HITBOXES[INDEX_OF_COLLUMN_1 + 5][INDEX_OF_ROW_1 + 7]->ReceiveChessman(white_bishop);
		//Czarna Krolowa
	ALL_SQUARES_HITBOXES[INDEX_OF_COLLUMN_1 + 4][INDEX_OF_ROW_1]->ReceiveChessman(black_queen);
		//Biala Krolowa
	ALL_SQUARES_HITBOXES[INDEX_OF_COLLUMN_1 + 3][INDEX_OF_ROW_1 + 7]->ReceiveChessman(white_queen);
		//Czarny Krol
	ALL_SQUARES_HITBOXES[INDEX_OF_COLLUMN_1 + 3][INDEX_OF_ROW_1]->ReceiveChessman(black_king);
	Square * black_king_square = ALL_SQUARES_HITBOXES[INDEX_OF_COLLUMN_1 + 3][INDEX_OF_ROW_1];
		//Bialy Krol
	ALL_SQUARES_HITBOXES[INDEX_OF_COLLUMN_1 + 4][INDEX_OF_ROW_1 + 7]->ReceiveChessman(white_king);
	Square * white_king_square = ALL_SQUARES_HITBOXES[INDEX_OF_COLLUMN_1 + 4][INDEX_OF_ROW_1 + 7];

	//Ustawiam miejsca do promocji pionkow, ustawione poza plansza
	//Musze wywolac DrawSquare() zeby zakryc pionki na tych polach
	//poniewaz DrawSquare() je rysuje
	ALL_SQUARES_HITBOXES[INDEX_OF_COLLUMN_1 + 2][INDEX_OF_ROW_1 + 11]->ReceiveChessman(white_tower);
	ALL_SQUARES_HITBOXES[INDEX_OF_COLLUMN_1 + 2][INDEX_OF_ROW_1 + 11]->DrawSquare();
	ALL_SQUARES_HITBOXES[INDEX_OF_COLLUMN_1 + 3][INDEX_OF_ROW_1 + 11]->ReceiveChessman(white_knight);
	ALL_SQUARES_HITBOXES[INDEX_OF_COLLUMN_1 + 3][INDEX_OF_ROW_1 + 11]->DrawSquare();
	ALL_SQUARES_HITBOXES[INDEX_OF_COLLUMN_1 + 4][INDEX_OF_ROW_1 + 11]->ReceiveChessman(white_bishop);
	ALL_SQUARES_HITBOXES[INDEX_OF_COLLUMN_1 + 4][INDEX_OF_ROW_1 + 11]->DrawSquare();
	ALL_SQUARES_HITBOXES[INDEX_OF_COLLUMN_1 + 5][INDEX_OF_ROW_1 + 11]->ReceiveChessman(white_queen);
	ALL_SQUARES_HITBOXES[INDEX_OF_COLLUMN_1 + 5][INDEX_OF_ROW_1 + 11]->DrawSquare();

	ALL_SQUARES_HITBOXES[INDEX_OF_COLLUMN_1 + 2][INDEX_OF_ROW_1 + 10]->ReceiveChessman(black_tower);
	ALL_SQUARES_HITBOXES[INDEX_OF_COLLUMN_1 + 2][INDEX_OF_ROW_1 + 10]->DrawSquare();
	ALL_SQUARES_HITBOXES[INDEX_OF_COLLUMN_1 + 3][INDEX_OF_ROW_1 + 10]->ReceiveChessman(black_knight);
	ALL_SQUARES_HITBOXES[INDEX_OF_COLLUMN_1 + 3][INDEX_OF_ROW_1 + 10]->DrawSquare();
	ALL_SQUARES_HITBOXES[INDEX_OF_COLLUMN_1 + 4][INDEX_OF_ROW_1 + 10]->ReceiveChessman(black_bishop);
	ALL_SQUARES_HITBOXES[INDEX_OF_COLLUMN_1 + 4][INDEX_OF_ROW_1 + 10]->DrawSquare();
	ALL_SQUARES_HITBOXES[INDEX_OF_COLLUMN_1 + 5][INDEX_OF_ROW_1 + 10]->ReceiveChessman(black_queen);
	ALL_SQUARES_HITBOXES[INDEX_OF_COLLUMN_1 + 5][INDEX_OF_ROW_1 + 10]->DrawSquare();

	//Rysuje wstępnie obraz
	al_flip_display();
	
	//Petla gry
	while (true)
	{
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);
		
		//Wylacza sie po zamknieciu okna
		if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
			break;
		}
		//Przy kliknieciu
		if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
			int active_square_x = ev.mouse.x / SQUARE_SIZE;
			int active_square_y = ev.mouse.y / SQUARE_SIZE;
			ALL_SQUARES_HITBOXES[active_square_x][active_square_y]->SquareActivation(whites_turn, active_square, empty_square, ALL_SQUARES_HITBOXES, INDEX_OF_ROW_1, INDEX_OF_COLLUMN_1, black_king_square, white_king_square);
			al_flip_display();
		}
	}

	//Zwalnianie pamieci
	for (int i = 0; i < ALL_SQUARES_W; i++)
	{
		for (int j = 0; j < ALL_SQUARES_H; j++)
		{
			if (ALL_SQUARES_HITBOXES[i][j])
			{
				delete ALL_SQUARES_HITBOXES[i][j];
			}
		}
	}

	for (int i = 0; i < ALL_SQUARES_W; i++)
	{
		if (ALL_SQUARES_HITBOXES[i])
		{
			delete ALL_SQUARES_HITBOXES[i];
		}
	}

	if (ALL_SQUARES_HITBOXES)
	{
		delete ALL_SQUARES_HITBOXES;
	}

	delete empty_chessman;
	delete empty_square;
	delete white_pawn;
	delete black_pawn;
	delete white_tower;
	delete black_tower;
	delete white_knight;
	delete black_knight;
	delete white_bishop;
	delete black_bishop;
	delete white_queen;
	delete black_queen;
	delete white_king;
	delete black_king;

	for (int i = 0; i < NUM_OF_BITMAPS; i++)
	{
		al_destroy_bitmap(bitmap_tab[i]);
	}
	al_destroy_bitmap(active_bitmap);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);

	return 0;
}

//Fukncja ladujaca odpowiednio nazwane bitmapy
ALLEGRO_BITMAP * load_bitmaps(int bitmap_number)
{
	char dir[20] = { 'd','a','t','a','/','b','i','t','m','a','p','s','/', '#', '#','.','p','n','g' };

	dir[13] = bitmap_number / 10 + 48;
	dir[14] = bitmap_number - (bitmap_number / 10) * 10 + 48;
	return al_load_bitmap(dir);
}

// Uruchomienie programu: Ctrl + F5 lub menu Debugowanie > Uruchom bez debugowania
// Debugowanie programu: F5 lub menu Debugowanie > Rozpocznij debugowanie

// Porady dotyczące rozpoczynania pracy:
//   1. Użyj okna Eksploratora rozwiązań, aby dodać pliki i zarządzać nimi
//   2. Użyj okna programu Team Explorer, aby nawiązać połączenie z kontrolą źródła
//   3. Użyj okna Dane wyjściowe, aby sprawdzić dane wyjściowe kompilacji i inne komunikaty
//   4. Użyj okna Lista błędów, aby zobaczyć błędy
//   5. Wybierz pozycję Projekt > Dodaj nowy element, aby utworzyć nowe pliki kodu, lub wybierz pozycję Projekt > Dodaj istniejący element, aby dodać istniejące pliku kodu do projektu
//   6. Aby w przyszłości ponownie otworzyć ten projekt, przejdź do pozycji Plik > Otwórz > Projekt i wybierz plik sln
