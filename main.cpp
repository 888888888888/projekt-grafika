#include <windows.h>  //biblioteka do okienka
#include <windowsx.h> //tu tez
#include <string>
#ifdef __cplusplus
    #include <cstdlib>
#else
    #include <stdlib.h>
#endif
#include <fstream>
#include <SDL.h>
#include <math.h>
#include <iostream>
#include <vector>

/*DEFINICJE STALYCH I ID*/
#define SCREEN_WIDTH  800	//definiujemy szerokosc i wysokosc ekranu w trybie okna
#define SCREEN_HEIGHT 600
#define ID_Color 111	//id przycisków
#define ID_Convert 555	//id Przycisku konwertuj
/*************************/

using namespace std;

SDL_Surface *screen;
int width = 900;  //wielkosc okna graficznego
int height = 600;
char const* tytul = "Okno obrazka";

int w=0, h=0; //wielkosc pliku graficznego

LPSTR Bufor;
DWORD dlugosc;

HWND hWnd,view;			//uchwyty okna
HWND hCombo;
HWND hText;
fstream plik1;
int frame();
void Color(int TrybKoloru);
void newfile();
void setPixel(int x, int y, Uint8 R, Uint8 G, Uint8 B);
void byterun(SDL_Color a[], int length);
void czyscEkran(Uint8 R, Uint8 G, Uint8 B);
void ladujBMP(char const* nazwa, int x, int y);
SDL_Color getPixel (int x, int y);

void byterun(SDL_Color dane[], int length)
{
int i = 0;
    while (i < length)
    {
        if ((i < length-1) && (dane[i].r == dane[i+1].r) && (dane[i].g == dane[i+1].g) && (dane[i].b == dane[i+1].b))
        {
            int j = 0;
            while ((i+j < length-1) && (dane[i+j].r == dane[i+j+1].r) && (dane[i+j].g == dane[i+j+1].g) && (dane[i+j].b == dane[i+j+1].b) && (j < 127))  j++;
            plik1 << -j << dane[i+j].r <<dane[i+j].g <<dane[i+j].b ; //zapis skompresowanych danych do pliku
            i += (j+1);
        }
        else
        {
            int j=0;
            while ((i+j < length-1) && ((dane[i+j].r != dane[j+i+1].r) || (dane[i+j].g != dane[j+i+1].g) || (dane[i+j].b != dane[j+i+1].b)) && (j <128)) j++;
            //dodaj jeszcze koncowke
            if ((i+j == length-1) && (j < 128)) j++;
            plik1 << (j-1); //zapis do pliku danych których nie da się skompresować
            for (int k=0; k<j; k++)  plik1 << dane[i+k].r << dane[i+k].g  <<dane[i+k].b ;
            i += j;
        }
    }
}

void setPixel(int x, int y, Uint8 R, Uint8 G, Uint8 B)
{
  if ((x>=0) && (x<width) && (y>=0) && (y<height))
  {
    /* Zamieniamy poszczególne skladowe koloru na format koloru pixela */
    Uint32 pixel = SDL_MapRGB(screen->format, R, G, B);

    /* Pobieramy informacji ile bajtów zajmuje jeden pixel */
    int bpp = screen->format->BytesPerPixel;

    /* Obliczamy adres pixela */
    Uint8 *p = (Uint8 *)screen->pixels + y * screen->pitch + x * bpp;

    /* Ustawiamy wartosc pixela, w zaleznci od formatu powierzchni*/
    switch(bpp)
    {
        case 1: //8-bit
            *p = pixel;
            break;

        case 2: //16-bit
            *(Uint16 *)p = pixel;
            break;

        case 3: //24-bit
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                p[0] = (pixel >> 16) & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = pixel & 0xff;
            } else {
                p[0] = pixel & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = (pixel >> 16) & 0xff;
            }
            break;

        case 4: //32-bit
            *(Uint32 *)p = pixel;
            break;

    }
         /* update the screen (aka double buffering) */
  }
}

void newfile(int w, int h)
{
/*****************************TWORZENIE PLIKU**************************************/
    plik1.open( "converted.art", std::ios::in | std::ios::out );
    if( plik1.good() == true ) cout << "Uzyskano dostep do pliku!" << endl;
    else cout << "Dostep do pliku zostal zabroniony!" << endl;
/***********************************TWORZYENIE TABLICY PRZECHOWUJACEJ SUROWE DANE********************************/
    SDL_Color *pliknowy;
    SDL_Color  bufor;
    pliknowy=new SDL_Color  [w*h];
    int p=0;
    for(int x=0;x<w;x++)
        for(int y=0;y<h;y++)
        {
            bufor=getPixel(x,y);
            pliknowy[p++]=bufor;
        }
/*******************************KOMPRESJA DANYCH Z TABLICY ********************************/
byterun(pliknowy,w*h);
plik1.close();
SDL_Flip(screen);
}

void ladujBMP(char const* nazwa, int x, int y)
{
    SDL_Rect dstrect;
    SDL_Surface* bmp = SDL_LoadBMP(nazwa);
    if (!bmp)
    {
        printf("Unable to load bitmap: %s\n", SDL_GetError());
    }
    else
    {
        dstrect.x = x;
        dstrect.y = y;
        SDL_BlitSurface(bmp, 0, screen, &dstrect);
        w=bmp->w;  //ustalamy wielkosc wczytanego obrazka
        h=bmp->h;
        SDL_Flip(screen);
        SDL_FreeSurface(bmp);
    }
}

void czyscEkran(Uint8 R, Uint8 G, Uint8 B)
{
    SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, R, G, B));
    SDL_Flip(screen);
}

SDL_Color getPixel (int x, int y) {
    SDL_Color color ;
    Uint32 col = 0 ;
    if ((x>=0) && (x<width) && (y>=0) && (y<height)) {
        //determine position
        char* pPosition=(char*)screen->pixels ;
        //offset by y
        pPosition+=(screen->pitch*y) ;
        //offset by x
        pPosition+=(screen->format->BytesPerPixel*x);
        //copy pixel data
        memcpy(&col, pPosition, screen->format->BytesPerPixel);
        //convert color
        SDL_GetRGB(col, screen->format, &color.r, &color.g, &color.b);
    }
    return ( color ) ;
}

int frame()
{
    if ( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "Unable to init SDL: %s\n", SDL_GetError() );
        return 1;
    }

    // make sure SDL cleans up before exit
    atexit(SDL_Quit);

    // create a new window
    screen = SDL_SetVideoMode(width, height, 32,
                                           SDL_HWSURFACE|SDL_DOUBLEBUF);
    if ( !screen )
    {
        printf("Unable to set video: %s\n", SDL_GetError());
        return 1;
    }

    SDL_WM_SetCaption( tytul , NULL );
    ladujBMP(Bufor, 0, 0);
}

// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

HWND g_hPrzycisk;
// the entry point for any Windows program  //tu mamy funkcje ktora jest odpowiedznikiem funkcji main,
int WINAPI WinMain(HINSTANCE hInstance,		//1. HINSTANCE hInstance - uchwyt, który jest liczba za pomoca ktorej winows identyfikuje aplikacje i okna
	HINSTANCE hPrevInstance,					//2. HINSTANCE hPrevInstance - to jakis przestarzaly parametr informujacy o poprzedniej kopii
	LPSTR lpCmdLine,							//3. LPSTR lpCmdLine - ten parametr sprawia ze gdy dopiszemy w cmd przy run "nazwaaplikacji.exe" np. "runA" to bedzie mozna odpalic kilka okien o specjalnych parametrach (nie bardzo wiem jakich)
	int nCmdShow)  								//4. int nCmdShow - ten parametr odpowiada za to jak okno ma sie pojawic (zminimalizowane/zmaksymalizowane/w oknie	SW_SHOWNORMAL	SW_SHOW	SW_HIDE	SW_MAXIMIZE		SW_MINIMIZE	SW_RESTORE	SW_SHOWMAXIMIZED	SW_SHOWMINIMIZED	SW_SHOWMINNOACTIVE	SW_SHOWNA  SW_SHOWNOACTIVATE
{

	WNDCLASSEX wc;		//struktura majaca informacje o klasie okna

	ZeroMemory(&wc, sizeof(WNDCLASSEX)); //zerowanie prestrzeni pamieci dla struktury klasy okna (wc- window class)
	//wype³nianie strukturki
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = "WindowClass";
	//rejestrowanie klasy
	RegisterClassEx(&wc);

	RECT wr = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
	//tworzenie okna ************
	hWnd = CreateWindowEx(NULL,
		"WindowClass",
		"Program1",
		WS_OVERLAPPEDWINDOW,
		300,
		300,
		500,
		500,
		NULL,
		NULL,
		hInstance,
		NULL);

	ShowWindow(hWnd, nCmdShow); 	//pokaz okno

	MSG msg;  //ta struktura przechowuje info o wydarzeniach z nia zwiazanych
	//*************************************TU WSZYSTKIE KONTROLKI
    hCombo = CreateWindowEx(WS_EX_CLIENTEDGE, "COMBOBOX", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER |
		CBS_DROPDOWN, 5, 50, 150, 200, hWnd, (HMENU)ID_Color, hInstance, NULL);

	SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM) "RGB");
	SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM) "HSV");
	SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM) "HSL");
	SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM) "YUV");


    //sciezka dostepu do pliku
    hText = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER |
    WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL, 5, 5, 475, 40, hWnd, NULL, hInstance, NULL);
	SetWindowText(hText, "Wpisz tu sciezke do pliku");

       // dlugosc = GetWindowTextLength(hText);
        //Bufor = (LPSTR)GlobalAlloc(GPTR, dlugosc + 1);
       // GetWindowText(hText, Bufor, dlugosc + 1); // pobralem sciezke dostepu do zmiennej bufor


	g_hPrzycisk = CreateWindowEx(0, "BUTTON", "Konwertuj", WS_CHILD | WS_VISIBLE,
		250, 60, 150, 30, hWnd, (HMENU)ID_Convert, hInstance, NULL);

	//*************************************TU KONIEC KONTROLEK

	while (TRUE)
	{

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
            break;
		}


	}

//GlobalFree(Bufor);
	return msg.wParam;
}
/*************************************************************************************************************************************/

/*OBSLUGA KOMUNIKATOW*/
LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
/**************************/
        case WM_CLOSE:
            DestroyWindow(hWnd);
            break;
/**************************/
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
/**************************/
        case WM_COMMAND:
            switch (wParam)
            {
            /**************************/
            case ID_Convert:
                int TrybKoloru = ComboBox_GetCurSel( hCombo );
                dlugosc = GetWindowTextLength(hText);
                Bufor = (LPSTR)GlobalAlloc(GPTR, dlugosc + 1);
                GetWindowText(hText, Bufor, dlugosc + 1); // pobralem sciezke dostepu do zmiennej bufor
                frame();
                newfile(w,h);
                break;
            /**************************/
            }
         break;
/**************************/
        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return 0;
}
