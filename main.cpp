#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>


extern "C" {
#include"./sdl-2.0.7/include/SDL.h"
#include"./sdl-2.0.7/include/SDL_main.h"
}


#define SCREEN_WIDTH	700
#define SCREEN_HEIGHT	550
#define MAP_SIZE 4


void DrawString(SDL_Surface *screen, int x, int y, const char *text, SDL_Surface *charset);

void DrawSurface(SDL_Surface *screen, SDL_Surface *sprite, int x, int y);

void DrawPixel(SDL_Surface *surface, int x, int y, Uint32 color);

void DrawLine(SDL_Surface *screen, int x, int y, int l, int dx, int dy, Uint32 color);

void DrawRectangle(SDL_Surface *screen, int x, int y, int l, int k, Uint32 outlineColor, Uint32 fillColor);

bool checkGameStatus(int gra[][MAP_SIZE], char komunikat[], bool *koniec);

bool checkMap(int gra[][MAP_SIZE]);

void nowyKafelek(int gra[][MAP_SIZE], char komunikat[]);

void cofnijRuch(int gra[][MAP_SIZE], int gra2[][MAP_SIZE], char komunikat[], int *points, int *pointsSAVED);

void newgame(double *czas, int gra[][MAP_SIZE], int gra2[][MAP_SIZE], char komunikat[], int *points, int *pointsSAVED, int *error);

int kolor(int gra[][MAP_SIZE], int i, int n);

void RysujPlansze(SDL_Surface *screen, int lightgrey, int czarny, int info, SDL_Surface *charset, double worldTime, int gra[][MAP_SIZE], char komunikat[], int points);

void zapiszRuchCHECK(int gra[][MAP_SIZE], int gra3[][MAP_SIZE], int *points, int *punkty);

void zapiszRuch3(int gra2[][MAP_SIZE], int gra3[][MAP_SIZE], int *punkty, int *pointsSAVED);

void Ruch(int poziom, int pion, int gra[][MAP_SIZE], int gra2[][MAP_SIZE], char komunikat[], bool *warunek, int *points, int *pointsSAVED);


// main
#ifdef __cplusplus
extern "C"
#endif

int main(int argc, char **argv) {
	
	int t1, t2, quit, frames, rc, error, points, pointsSAVED;
	double delta, worldTime;
	int gra[MAP_SIZE][MAP_SIZE], gra2[MAP_SIZE][MAP_SIZE];
	bool warunek = true;
	bool koniec = true;
	SDL_Event event;
	SDL_Surface *screen, *charset;
	SDL_Texture *scrtex;
	SDL_Window *window;
	SDL_Renderer *renderer;

	if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		return 1;
		}

	// tryb pe³noekranowy / fullscreen mode
//	rc = SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP,
//	                                 &window, &renderer);
	rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0,
	                                 &window, &renderer);
	if(rc != 0) {
		SDL_Quit();
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		return 1;
		};
	
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	SDL_SetWindowTitle(window, "2048 Michal Wnuczynski 175739");		// Tytu³ aplikacji


	screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
	                              0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

	scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
	                           SDL_TEXTUREACCESS_STREAMING,
	                           SCREEN_WIDTH, SCREEN_HEIGHT);


	// wy³¹czenie widocznoœci kursora myszy
	SDL_ShowCursor(SDL_DISABLE);

	// wczytanie obrazka cs8x8.bmp
	charset = SDL_LoadBMP("./cs8x8.bmp");
	if(charset == NULL) {
		printf("SDL_LoadBMP(cs8x8.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
		};

	SDL_SetColorKey(charset, true, 0x000000);

	char komunikat[128];	// Komunikat do zakonczenia gry
	komunikat[0] = ' ';
	int czarny = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
	int grey = SDL_MapRGB(screen->format, 0x40, 0x40, 0x40);
	int info = SDL_MapRGB(screen->format, 0x44, 0x48, 0x8F);
	int lightgrey = SDL_MapRGB(screen->format, 0xC0, 0xC0, 0xC0);

	t1 = SDL_GetTicks();

	quit = 0;
	worldTime = 0;

	SDL_FillRect(screen, NULL, grey);		// Kolor tla

	newgame(&worldTime, gra, gra2, komunikat, &points, &pointsSAVED, &error);

	while(!quit) {											// Aktualne dzialanie aplikacji
		t2 = SDL_GetTicks();

		delta = (t2 - t1) * 0.001 * error;	// Obliczanie czasu
		t1 = t2;
		worldTime += delta;			// Czas zapis

		RysujPlansze(screen, lightgrey, czarny, info, charset, worldTime, gra, komunikat, points);		// Rysuje plansze

		if (koniec == false) {		// Sprawdzenie czy gra zostala zatrzymana z powodu przegranej lub wygranej
			DrawRectangle(screen, 4, 10, SCREEN_WIDTH - 8, 30, czarny, info);
			DrawString(screen, screen->w / 2 - strlen(komunikat) * 8 / 2, 21, komunikat, charset);
			error = 0;
			double CZAS = worldTime;
			while(SDL_PollEvent(&event))
				if (event.key.keysym.sym == 'n') {
					komunikat[0] = ' ';
					error = 1;
					koniec = true;
					newgame(&worldTime, gra, gra2, komunikat, &points, &pointsSAVED, &error);
				}
				else if (event.key.keysym.sym == 'u') {
					komunikat[0] = ' ';
					error = 1;
					koniec = true;
					worldTime = CZAS;
					cofnijRuch(gra, gra2, komunikat, &points, &pointsSAVED);
				}
		}

		SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
//		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, scrtex, NULL, NULL);
		SDL_RenderPresent(renderer);
		// obs³uga zdarzeñ (o ile jakieœ zasz³y) / handling of events (if there were any)
		while(SDL_PollEvent(&event)) {
			switch(event.type) {
				case SDL_KEYDOWN:			// Nacisniety klawisz
					if(event.key.keysym.sym == SDLK_ESCAPE) quit = 1;
					else if (event.key.keysym.sym == SDLK_UP) {
						if (koniec) {
							Ruch(0, 1, gra, gra2, komunikat, &warunek, &points, &pointsSAVED);
							checkGameStatus(gra, komunikat, &koniec);
						}
					}
					else if (event.key.keysym.sym == SDLK_DOWN) {
						if (koniec) {
							Ruch(0, -1, gra, gra2, komunikat, &warunek, &points, &pointsSAVED);
							checkGameStatus(gra, komunikat, &koniec);
						}
					}
					else if (event.key.keysym.sym == SDLK_LEFT) {
						if (koniec) {
							Ruch(-1, 0, gra, gra2, komunikat, &warunek, &points, &pointsSAVED);
							checkGameStatus(gra, komunikat, &koniec);
						}
					}
					else if (event.key.keysym.sym == SDLK_RIGHT) {
						if (koniec) {
							Ruch(1, 0, gra, gra2, komunikat, &warunek, &points, &pointsSAVED);
							checkGameStatus(gra, komunikat, &koniec);
						}

					}
					else if (event.key.keysym.sym == 'n') {
						newgame(&worldTime, gra, gra2, komunikat, &points, &pointsSAVED, &error);
					}
					else if (event.key.keysym.sym == 'u') {
						cofnijRuch(gra, gra2, komunikat, &points, &pointsSAVED);
					}
					break;
				case SDL_KEYUP:			// Puszczenie klawisza
					break;
				case SDL_QUIT:
					quit = 1;
					break;
				case 0x4E:
					worldTime = 0;
					break;
				};
			};

		};

	// zwolnienie powierzchni / freeing all surfaces
	SDL_FreeSurface(charset);
	SDL_FreeSurface(screen);
	SDL_DestroyTexture(scrtex);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();
	return 0;
};



void DrawString(SDL_Surface *screen, int x, int y, const char *text, SDL_Surface *charset) {
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8;
	d.h = 8;
	while (*text) {
		c = *text & 255;
		px = (c % 16) * 8;
		py = (c / 16) * 8;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		SDL_BlitSurface(charset, &s, screen, &d);
		x += 8;
		text++;
	};
};


void DrawSurface(SDL_Surface *screen, SDL_Surface *sprite, int x, int y) {
	SDL_Rect dest;
	dest.x = x - sprite->w / 2;
	dest.y = y - sprite->h / 2;
	dest.w = sprite->w;
	dest.h = sprite->h;
	SDL_BlitSurface(sprite, NULL, screen, &dest);
};


void DrawPixel(SDL_Surface *surface, int x, int y, Uint32 color) {
	int bpp = surface->format->BytesPerPixel;
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
	*(Uint32 *)p = color;
};


void DrawLine(SDL_Surface *screen, int x, int y, int l, int dx, int dy, Uint32 color) {
	for (int i = 0; i < l; i++) {
		DrawPixel(screen, x, y, color);
		x += dx;
		y += dy;
	};
};


void DrawRectangle(SDL_Surface *screen, int x, int y, int l, int k, Uint32 outlineColor, Uint32 fillColor) {
	int i;
	DrawLine(screen, x, y, k, 0, 1, outlineColor);
	DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
	DrawLine(screen, x, y, l, 1, 0, outlineColor);
	DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
	for (i = y + 1; i < y + k - 1; i++)
		DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
};


bool checkGameStatus(int gra[][MAP_SIZE], char komunikat[], bool *koniec) {

	bool wolnepola = false;
	bool wygrana = false;

	for (int a = 0; a < MAP_SIZE; a++) {
		for (int b = 0; b < MAP_SIZE; b++) {
			if (gra[a][b] == 0)
				wolnepola = true;
			if (gra[a][b] == 2048)
				wygrana = true;
		}
	}

	if (wygrana == true) {
		*koniec = false;
		sprintf(komunikat, "Gratulacje wygrales! Kliknij N, aby zagrac jeszcze raz.");
	}

	if (wolnepola == false) {
		for (int a = 0; a < MAP_SIZE; a++) {
			for (int b = 0; b < MAP_SIZE; b++) {
				if ((gra[a][b] == gra[a][b - 1] && b - 1 >= 0) || (gra[a][b] == gra[a][b + 1] && b + 1 < MAP_SIZE) || (gra[a][b] == gra[a - 1][b] && a - 1 >= 0) || (gra[a][b] == gra[a + 1][b] && a + 1 < MAP_SIZE))
					return false;
			}
		}
	}
	else {
		return false;
	}

	*koniec = false;
	sprintf(komunikat, "Przegrales. Kliknij N, aby zaczac od nowa");

	return true;
}


bool checkMap(int gra[][MAP_SIZE]) {

	for (int a = 0; a < MAP_SIZE; a++) {
		for (int b = 0; b < MAP_SIZE; b++) {
			if (gra[a][b] == 0)
				return false;
		}
	}

	return true;
}


void nowyKafelek(int gra[][MAP_SIZE], char komunikat[]) {

	int losX = rand() % 4;
	int losY = rand() % 4;

	if (checkMap(gra))
		return;

	while (gra[losY][losX] != 0)
	{
		if (gra[losY][losX] == 0)
		{
			gra[losY][losX] = 2;
			return;
		}
		else {
			losX = rand() % 4;
			losY = rand() % 4;
		}
	}

	gra[losY][losX] = 2;

	return;
}


void cofnijRuch(int gra[][MAP_SIZE], int gra2[][MAP_SIZE], char komunikat[], int *points, int *pointsSAVED) {

	for (int i = 0; i < MAP_SIZE; i++) {
		for (int n = 0; n < MAP_SIZE; n++) {
			gra[i][n] = gra2[i][n];
		}
	}

	*points = *pointsSAVED;
}


void newgame(double *czas, int gra[][MAP_SIZE], int gra2[][MAP_SIZE], char komunikat[], int *points, int *pointsSAVED, int *error) {

	*czas = 0;
	*pointsSAVED = 0;
	*error = 1;

	for (int i = 0; i < MAP_SIZE; i++) {
		for (int n = 0; n < MAP_SIZE; n++) {
			gra[i][n] = 0;
			gra2[i][n] = 0;
		}
	}

	nowyKafelek(gra2, komunikat);
	cofnijRuch(gra, gra2, komunikat, points, pointsSAVED);
}


int kolor(int gra[][MAP_SIZE], int i, int n) {

	int wynik = 0;
	int mnoznik = 2;

	if (gra[i][n] == 0)
		return 12;

	while ((gra[i][n] / mnoznik) != 1) {
		wynik++;
		mnoznik *= 2;
	}

	return wynik;
}


void RysujPlansze(SDL_Surface *screen, int lightgrey, int czarny, int info, SDL_Surface *charset, double worldTime, int gra[][MAP_SIZE], char komunikat[], int points) {

	char text[128];
	int orange = SDL_MapRGB(screen->format, 0xFF, 0x80, 0x00);
	int lightgray = SDL_MapRGB(screen->format, 0xC0, 0xC0, 0xC0);
	int tlo = SDL_MapRGB(screen->format, 0x82, 0x78, 0x64);
	int color[12];
	color[0] = SDL_MapRGB(screen->format, 0xed, 0xdf, 0xc9);
	color[1] = SDL_MapRGB(screen->format, 0xef, 0xd3, 0xa0);
	color[2] = SDL_MapRGB(screen->format, 0xf2, 0xba, 0x65);
	color[3] = SDL_MapRGB(screen->format, 0xff, 0x9e, 0x0c);
	color[4] = SDL_MapRGB(screen->format, 0xef, 0x26, 0x47);
	color[5] = SDL_MapRGB(screen->format, 0xc6, 0x1d, 0x23);
	color[6] = SDL_MapRGB(screen->format, 0xf6, 0xff, 0x4f);
	color[7] = SDL_MapRGB(screen->format, 0xff, 0xdd, 0x00);
	color[8] = SDL_MapRGB(screen->format, 0xdb, 0xa4, 0x00);
	color[9] = SDL_MapRGB(screen->format, 0x83, 0xdb, 0x00);
	color[10] = SDL_MapRGB(screen->format, 0xff, 0xae, 0x00);
	color[11] = SDL_MapRGB(screen->format, 0x66, 0x61, 0x57);

	// tekst informacyjny / info text
	DrawRectangle(screen, 4, 10, SCREEN_WIDTH - 8, 30, color[0], info);
	sprintf(text, "Esc - wyjscie, \030 - w gore, \031 - w dol, \032 - w lewo, \033 - w prawo");
	DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 21, text, charset);

	// Wypisywanie czasu
	DrawRectangle(screen, SCREEN_WIDTH - 160, 60, 150, 36, color[0], info);
	sprintf(text, "Czas = %.1lf s ", worldTime);
	DrawString(screen, screen->w - 15 - strlen(text) * 8, 75, text, charset);

	// Wyswietlanie punktow
	DrawRectangle(screen, 95, 60, 150, 45, tlo, tlo);
	sprintf(text, "Punkty");
	DrawString(screen, 172 - strlen(text) * 4, 65, text, charset);
	sprintf(text, "%d pkt", points);
	DrawString(screen, 172 - strlen(text) * 4, 85, text, charset);

	// Rysowanie planszy
	DrawRectangle(screen, 95, 120, MAP_SIZE * 100 + 10, MAP_SIZE * 100 + 10, tlo, tlo);	// T³o planszy

	for (int i = 0; i < MAP_SIZE; i++) {		// Rysowanie kafelków
		for (int n = 0; n < MAP_SIZE; n++) {
			int C = kolor(gra, i, n);

			DrawRectangle(screen, 105 + 100 * i, 130 + 100 * n, 90, 90, color[C], color[C]);
			if (gra[i][n] != 0) {
				sprintf(text, "%d ", gra[i][n]);
				DrawString(screen, 105 + 100 * i + (50 - strlen(text) * 8 / 2), 130 + 100 * n + (50 - strlen(text) * 6 / 2), text, charset);
			}
		}
	}

}


void zapiszRuchCHECK(int gra[][MAP_SIZE], int gra3[][MAP_SIZE], int *points, int *punkty) {

	for (int i = 0; i < MAP_SIZE; i++) {
		for (int n = 0; n < MAP_SIZE; n++) {
			gra3[i][n] = gra[i][n];
		}
	}

	*punkty += *points;
	*punkty *= 1;
}


void zapiszRuch3(int gra2[][MAP_SIZE], int gra3[][MAP_SIZE], int *punkty, int *pointsSAVED) {

	for (int i = 0; i < MAP_SIZE; i++) {
		for (int n = 0; n < MAP_SIZE; n++) {
			gra2[i][n] = gra3[i][n];
		}
	}

	*pointsSAVED = *punkty;
}


void Ruch(int poziom, int pion, int gra[][MAP_SIZE], int gra2[][MAP_SIZE], char komunikat[], bool *warunek, int *points, int *pointsSAVED) {

	int g_y;
	int g_x;
	int gra3[MAP_SIZE][MAP_SIZE];
	int punkty = 0;

	zapiszRuchCHECK(gra, gra3, points, &punkty);

	*warunek = false;

	if (pion < 0) {		// Pionowo w dó³
		for (int i = MAP_SIZE - 1; i >= 0; i--) {
			for (int n = MAP_SIZE - 1; n >= 0; n--) {
				if (gra[i][n] != 0 && n > 0) {
					int k = n - 1;
					while (k >= 0) {
						if (gra[i][k] != 0 && gra[i][k] != gra[i][n]) {
							break;
						}
						if (gra[i][k] == gra[i][n]) {
							gra[i][n] *= 2;
							gra[i][k] = 0;
							*points += gra[i][n];
							*warunek = true;
							break;
						}
						k--;
					}
				}
			}
		}

		for (g_y = MAP_SIZE - 1; g_y >= 0; g_y--) {
			for (g_x = MAP_SIZE - 1; g_x >= 0; g_x--) {
				if (gra[g_y][g_x] != 0 && g_x < MAP_SIZE) {
					int l = g_x;
					while (l < MAP_SIZE - 1) {
						if (gra[g_y][l + 1] == 0) {
							gra[g_y][l + 1] = gra[g_y][l];
							gra[g_y][l] = 0;
							*warunek = true;
						}
						l++;
					}
				}
			}
		}
	}

	if (pion > 0) {		// Pionowo w górê
		for (int i = 0; i < MAP_SIZE; i++) {
			for (int n = 0; n < MAP_SIZE; n++) {
				if (gra[i][n] != 0 && n < MAP_SIZE) {
					int k = n + 1;
					while (k < MAP_SIZE) {
						if (gra[i][k] != 0 && gra[i][k] != gra[i][n]) {
							break;
						}
						if (gra[i][k] == gra[i][n]) {
							gra[i][n] *= 2;
							gra[i][k] = 0;
							*points += gra[i][n];
							*warunek = true;
							break;
						}
						k++;
					}
				}
			}
		}

		for (g_y = 0; g_y < MAP_SIZE; g_y++) {
			for (g_x = 0; g_x < MAP_SIZE; g_x++) {
				if (gra[g_y][g_x] != 0 && g_x > 0) {
					int l = g_x;
					while (l > 0) {
						if (gra[g_y][l - 1] == 0) {
							gra[g_y][l - 1] = gra[g_y][l];
							gra[g_y][l] = 0;
							*warunek = true;
						}
						l--;
					}
				}
			}
		}
	}

	if (poziom < 0) {		// Poziomo w lewo
		for (int i = 0; i < MAP_SIZE; i++) {
			for (int n = 0; n < MAP_SIZE; n++) {
				if (gra[i][n] != 0 && i < MAP_SIZE) {
					int k = i + 1;
					while (k < MAP_SIZE) {
						if (gra[k][n] != 0 && gra[k][n] != gra[i][n]) {
							break;
						}
						if (gra[k][n] == gra[i][n]) {
							gra[i][n] *= 2;
							gra[k][n] = 0;
							*points += gra[i][n];
							*warunek = true;
							break;
						}
						k++;
					}
				}
			}
		}

		for (g_y = 0; g_y < MAP_SIZE; g_y++) {
			for (g_x = 0; g_x < MAP_SIZE; g_x++) {
				if (gra[g_y][g_x] != 0 && g_y > 0) {
					int l = g_y;
					while (l > 0) {
						if (gra[l - 1][g_x] == 0) {
							gra[l - 1][g_x] = gra[l][g_x];
							gra[l][g_x] = 0;
							*warunek = true;
						}
						l--;
					}
				}
			}
		}
	}

	if (poziom > 0) {		// Poziomo w prawo
		for (int i = MAP_SIZE - 1; i >= 0; i--) {
			for (int n = MAP_SIZE - 1; n >= 0; n--) {
				if (gra[i][n] != 0 && i > 0) {
					int k = i - 1;
					while (k >= 0) {
						if (gra[k][n] != 0 && gra[k][n] != gra[i][n]) {
							break;
						}
						if (gra[k][n] == gra[i][n]) {
							gra[i][n] *= 2;
							gra[k][n] = 0;
							*points += gra[i][n];
							*warunek = true;
							break;
						}
						k--;
					}
				}
			}
		}

		for (g_y = MAP_SIZE - 1; g_y >= 0; g_y--) {
			for (g_x = MAP_SIZE; g_x >= 0; g_x--) {
				if (gra[g_y][g_x] != 0 && g_y < MAP_SIZE) {
					int l = g_y;
					while (l < MAP_SIZE - 1) {
						if (gra[l + 1][g_x] == 0) {
							gra[l + 1][g_x] = gra[l][g_x];
							gra[l][g_x] = 0;
							*warunek = true;
						}
						l++;
					}
				}
			}
		}
	}

	if (*warunek == true) {
		zapiszRuch3(gra2, gra3, &punkty, pointsSAVED);		// Jezeli cos sie zmienilo to kopiuje poprzednia tablice
		nowyKafelek(gra, komunikat);
	}
}

