
// include the basic windows header files and the Direct3D header files
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

#define SCREEN_WIDTH  800	//definiujemy szerokosc i wysokosc ekranu w trybie okna
#define SCREEN_HEIGHT 600
#define ID_Color 111	//id przycisków
#define ID_Convert 222	//id Przycisku konwertuj
using namespace std;
SDL_Surface *screen;
int width = 900;
int height = 600;
char const* tytul = "Okno obrazka";
LPSTR Bufor;
DWORD dlugosc;
 int w=0, h=0; //wielkosc pliku

fstream plik1;
int frame();
void newfile();
void setPixel(int x, int y, Uint8 R, Uint8 G, Uint8 B);
void byterun(SDL_Color a[], int length);
void czyscEkran(Uint8 R, Uint8 G, Uint8 B);
void ladujBMP(char const* nazwa, int x, int y);
SDL_Color getPixel (int x, int y);
//void byterun(signed char a[], int length)
void byterun(SDL_Color a[], int length)
{
int i = 0;

//dopoki wszystkie bajty nie sa skompresowane
while (i < length)
{
//sekwencja powtarzajacych sie bajtow
if ((i < length-1) &&
(a[i].r == a[i+1].r)&&
(a[i].g == a[i+1].g)&&
(a[i].b == a[i+1].b))
{
//zmierz dlugosc sekwencji
int j = 0;
while ((i+j < length-1) &&
(a[i+j].r == a[i+j+1].r) &&
    (a[i+j].g == a[i+j+1].g) &&
    (a[i+j].b == a[i+j+1].b) &&
(j < 127))
{
j++;
}
//wypisz spakowana sekwencje
plik1 << -j << a[i+j].r <<a[i+j].g <<a[i+j].b ;
//przesun wskaznik o dlugosc sekwencji
i += (j+1);
}
//sekwencja roznych bajtow
else
{
//zmierz dlugosc sekwencji
int j=0;
while ((i+j < length-1) &&
((a[i+j].r != a[j+i+1].r) || (a[i+j].g != a[j+i+1].g) || (a[i+j].b != a[j+i+1].b)) &&
(j <128))
{
j++;
}
//dodaj jeszcze koncowke
if ((i+j == length-1) &&
(j < 128))
{
j++;
}
//wypisz spakowana sekwencje
plik1 << (j-1);
for (int k=0; k<j; k++)
{
plik1 << a[i+k].r << a[i+k].g  <<a[i+k].b ;
}
//przesun wskaznik o dlugosc sekwencji
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
    //plik1 = fopen( "converted.art", "w+" );

    plik1.open( "converted.art", std::ios::in | std::ios::out );
    if( plik1.good() == true )
{
    cout << "Uzyskano dostep do pliku!" << endl;
    //tu operacje na pliku
} else cout << "Dostep do pliku zostal zabroniony!" << endl;
/***********************************TWORZYENIE TABLICY PRZECHOWUJACEJ SUROWE DANE********************************/
   // SDL_Color **pliknowy=new SDL_Color * [w];
   // for(int i=0;i<w;i++) pliknowy[i]=new SDL_Color[h];
    SDL_Color *pliknowy;
    SDL_Color  bufor;
    pliknowy=new SDL_Color  [w*h];
    int p=0;
    for(int x=0;x<w;x++)
    for(int y=0;y<h;y++)
    {
    bufor=getPixel(x,y);

    pliknowy[p++]=bufor;
   // plik1<<bufor.r;
   // plik1<<bufor.g;
   // plik1<<bufor.b;
    }
    //for(int i=0;i<20;i++) cout<<pliknowy[i];
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
        w=bmp->w;
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
	HWND hWnd,view;			//uchwyt okna, uzupe³niany przez funkcjê
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

	//pokaz okno
	ShowWindow(hWnd, nCmdShow);


	// enter the main loop:

	MSG msg;  //ta struktura przechowuje info o wydarzeniach z nia zwiazanych

	//*************************************

	HWND hCombo = CreateWindowEx(WS_EX_CLIENTEDGE, "COMBOBOX", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER |
		CBS_DROPDOWN, 5, 50, 150, 200, hWnd, (HMENU)ID_Color, hInstance, NULL);

	SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM) "RGB");
	SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM) "HSV");
	SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM) "HSL");
	SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM) "YUV");

	//*************************************

	HWND hText = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER |   //sciezka dostepu do pliku
		WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL, 5, 5, 475, 40, hWnd, NULL, hInstance, NULL);
	SetWindowText(hText, "C:\\Users\\siusiak\\Desktop\\matki na studia\\rok2\\grafika\\projekt\\main\\projekt\\obrazek1.bmp");



            dlugosc = GetWindowTextLength(hText);
            Bufor = (LPSTR)GlobalAlloc(GPTR, dlugosc + 1);
            GetWindowText(hText, Bufor, dlugosc + 1); // pobralem sciezke dostepu do zmiennej bufor

	//*************************************

	g_hPrzycisk = CreateWindowEx(0, "BUTTON", "Konwertuj", WS_CHILD | WS_VISIBLE,
		250, 60, 150, 30, hWnd, (HMENU)ID_Convert, hInstance, NULL);

	//*************************************

	//CreateFile(sciezka, GENERIC_READ, dwShareMode,
	//	lpSecurityAttributes, dwCreationDistribution, dwFlagsAndAttributes, hTemplateFile)

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
// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_COMMAND:
		switch (wParam)
		{
		case ID_Color:
			MessageBox(hWnd, "Wcisnoles przycisk 1", "Test", MB_ICONINFORMATION);
			break;
		case ID_Convert:

			frame();
			newfile(w,h);

			break;
		}

	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return 0;
}



/*
int main ( int argc, char** argv )
{
    // console output
    freopen( "CON", "wt", stdout );
    freopen( "CON", "wt", stderr );

    // initialize SDL video
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
    // program main loop
    bool done = false;
    while (!done)
    {
        // message processing loop
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            // check for messages
            switch (event.type)
            {
                // exit if the window is closed
            case SDL_QUIT:
                done = true;
                break;

                // check for keypresses
            case SDL_KEYDOWN:
                {
                    // exit if ESCAPE is pressed
                    if (event.key.keysym.sym == SDLK_ESCAPE)
                        done = true;
                    if (event.key.keysym.sym == SDLK_a)
                        ladujBMP("obrazek1.bmp", 0, 0);
                    if (event.key.keysym.sym == SDLK_s)
                        ladujBMP("obrazek4.bmp", 0, 0);

                    if (event.key.keysym.sym == SDLK_b)
                        czyscEkran(10, 0, 0);          break;
                     }
            } // end switch
        } // end of message processing

    } // end main loop


    // all is well ;)
    printf("Exited cleanly\n");
    return 0;
}
*/

