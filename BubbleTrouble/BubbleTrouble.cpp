// BubbleTrouble.cpp : définit le point d'entrée pour l'application.
//

#include "stdafx.h"
#include "BubbleTrouble.h"
#include "time.h"
#include "math.h"
#include "stdio.h"

#define MAX_LOADSTRING 100
#define LENGTH_NAME 50
#define INTERVAL_BUBBLE 3000
#define MAX_HARPOON 5
#define BUBBLE_INITIAL_RAY 30
#define CHILD_BUBBLE_DISTANCE 20
#define MIN_RAY 20

#define FRAME_ID 69
#define BUBBLE_ID 70
// Variables globales :
HINSTANCE hInst;                                // instance actuelle
WCHAR szTitle[MAX_LOADSTRING];                  // Le texte de la barre de titre
WCHAR szWindowClass[MAX_LOADSTRING];            // le nom de la classe de fenêtre principale
HWND g_hwdn;
RECT g_desktopWindow, g_updateRect;
POINT g_posHarpoon, g_tabHarpoonJ1[MAX_HARPOON], g_tabHarpoonJ2[MAX_HARPOON], g_characterPos;
Gameboard g_gameBoard;

HANDLE HarThread;
DWORD ThreadID;
HANDLE Anchor = NULL;
// Pré-déclarations des fonctions incluses dans ce module de code :
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

DWORD WINAPI HarpoonThread(LPVOID lParam);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: placez ici le code.
	srand((int)time(0));
	rand();

	g_characterPos.x = 800;
	g_characterPos.y = 900;

	g_updateRect.left = g_desktopWindow.left + 100;
	g_updateRect.right = g_desktopWindow.right - 100;
	g_updateRect.top = 0;
	g_updateRect.bottom = g_desktopWindow.bottom - 100;
    // Initialise les chaînes globales
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_BUBBLETROUBLE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Effectue l'initialisation de l'application :
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_BUBBLETROUBLE));

    MSG msg;

    // Boucle de messages principale :
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FONCTION : MyRegisterClass()
//
//  BUT : inscrit la classe de fenêtre.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BUBBLETROUBLE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_BUBBLETROUBLE);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	wcex.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));

    return RegisterClassExW(&wcex);
}

//
//   FONCTION : InitInstance(HINSTANCE, int)
//
//   BUT : enregistre le handle de l'instance et crée une fenêtre principale
//
//   COMMENTAIRES :
//
//        Dans cette fonction, nous enregistrons le handle de l'instance dans une variable globale, puis
//        créons et affichons la fenêtre principale du programme.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Stocke le handle d'instance dans la variable globale

   g_hwdn = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);


   if (!g_hwdn)
   {
      return FALSE;
   }

   DialogBox(hInst, MAKEINTRESOURCE(IDD_INSCRIPTION), g_hwdn, Inscription);

   SetTimer(g_hwdn, BUBBLE_ID, INTERVAL_BUBBLE, TimerProc);
   SetTimer(g_hwdn, FRAME_ID, 40, (TIMERPROC)NULL);
   ShowWindow(g_hwdn, nCmdShow);
   UpdateWindow(g_hwdn);

   //load texture
   
	   HDC hdc = GetDC(g_hwdn);

	   HBITMAP wall = (HBITMAP)LoadImageA(NULL, "brick.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	   HDC hdcMem = CreateCompatibleDC(hdc);
	   SelectObject(hdcMem, wall);

	   StretchBlt(hdc, -200, 0, 900, 1500, hdcMem, 000, 0, 900, 900, SRCCOPY);
	   StretchBlt(hdc, 1700, 0, 900, 1500, hdcMem, 000, 0, 900, 900, SRCCOPY);
	   //plancher
	   StretchBlt(hdc, 200, 900, 900, 1500, hdcMem, 000, 0, 900, 900, SRCCOPY);
	   StretchBlt(hdc, 500, 900, 900, 1500, hdcMem, 000, 0, 900, 900, SRCCOPY);
	   StretchBlt(hdc, 900, 900, 900, 1500, hdcMem, 000, 0, 900, 900, SRCCOPY);
	   StretchBlt(hdc, 1300, 900, 900, 1500, hdcMem, 000, 0, 900, 900, SRCCOPY);

	   ReleaseDC(g_hwdn, hdc);
   
   return TRUE;
}

//
//  FONCTION : WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  BUT :  traite les messages pour la fenêtre principale.
//
//  WM_COMMAND - traite le menu de l'application
//  WM_PAINT - dessine la fenêtre principale
//  WM_DESTROY - génère un message d'arrêt et retourne
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_QUIT:
		break;
	case WM_INITDIALOG:
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_LEFT:
			g_characterPos.x -= 20;
			break;
		case VK_RIGHT:
			g_characterPos.x += 20;
			break;
		case VK_UP:
		{
			if (Anchor == NULL)
			{
				Anchor = CreateThread(NULL, 0, HarpoonThread, &hWnd, 0, &ThreadID);
				if (Anchor == NULL)
				{
					ExitProcess(3);
				}
			}
			else
			{
				PostThreadMessage(ThreadID, WM_CHAR, NULL, NULL);
				if (Anchor == NULL)
				{
					ExitProcess(3);
				}
			}
			break;
		}
		default:
			break;
		}
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Analyse les sélections de menu :
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
	//Pour tester si les collisions marche
	case WM_LBUTTONDOWN:
		{
			g_tabHarpoonJ1[0].x = LOWORD(lParam);
			g_tabHarpoonJ1[0].y = HIWORD(lParam);
		}
		break;
	//Pour tester si les collisions marche
	case WM_LBUTTONUP:
		{
			g_tabHarpoonJ1[0].x = 0;
			g_tabHarpoonJ1[0].y = 0;
		}
		break;
	case WM_TIMER:
	{
		switch (wParam) {
		case FRAME_ID:
			InvalidateRect(g_hwdn, NULL, TRUE);
			return 0;
		}
	}
	break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: ajoutez le code de dessin qui utilise hdc ici...
			if (g_gameBoard.nbPlayer == 1)
			{
				int i = sprintf_s(g_gameBoard.scoreBoard, 200, "%s : %04d", g_gameBoard.tabPlayer[0].name, g_gameBoard.tabPlayer[0].score);
			}
			else
			{
				int i = sprintf_s(g_gameBoard.scoreBoard, 200, "%s : %04d vs. %s : %04d", g_gameBoard.tabPlayer[0].name, g_gameBoard.tabPlayer[0].score,
					g_gameBoard.tabPlayer[1].name, g_gameBoard.tabPlayer[1].score);
			}
		
			bool ret = TextOutA(hdc, 800, 10, g_gameBoard.scoreBoard, strlen(g_gameBoard.scoreBoard));

			HBITMAP character = (HBITMAP)LoadImageA(NULL, "front.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			HDC hdcMem = CreateCompatibleDC(hdc);
			SelectObject(hdcMem, character);
			StretchBlt(hdc, g_characterPos.x, g_characterPos.y, 250, 250, hdcMem, 000, 0, 900, 900, SRCCOPY);

        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Gestionnaire de messages pour la boîte de dialogue À propos de.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
		//load texture
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

// Gestionnaire de messages pour la boîte de dialogue Inscription.
INT_PTR CALLBACK Inscription(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		{
			HFONT hFont = CreateFont(25, 15, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, NULL);
			HWND hExplication = GetDlgItem(hDlg, IDC_EXPLICATION);
			SendMessage(hExplication, WM_SETFONT, (WPARAM)hFont, TRUE);
		}
		return (INT_PTR)TRUE;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			DestroyWindow(g_hwdn);
			return (INT_PTR)TRUE;
		}
		else if (LOWORD(wParam) == IDC_START)
		{
			char J1[LENGTH_NAME], J2[LENGTH_NAME];

			HWND hJ1 = GetDlgItem(hDlg, IDC_NAMEJ1);
			int lengthJ1 = GetWindowTextLengthW(hJ1);
			GetDlgItemTextA(hDlg, IDC_NAMEJ1, J1, lengthJ1 + 1);

			HWND hJ2 = GetDlgItem(hDlg, IDC_NAMEJ2);
			int lengthJ2 = GetWindowTextLengthW(hJ2);
			GetDlgItemTextA(hDlg, IDC_NAMEJ2, J2, lengthJ2 + 1);

			if (lengthJ1 <= 0 || (lengthJ1 <= 0 && lengthJ2 <= 0))
			{
				MessageBox(hDlg, L"Entrez un joueur", L"Erreur", MB_OK);
			}
			else
			{
				g_gameBoard.nbPlayer++;

				Player player1;
				strcpy_s(player1.name, J1);
				g_gameBoard.tabPlayer[0] = player1;
				g_gameBoard.tabPlayer[0].score = 0;
				if (lengthJ2 > 0)
				{
					g_gameBoard.nbPlayer++;

					Player player2;
					strcpy_s(player2.name, J2);
					g_gameBoard.tabPlayer[1] = player2;
					g_gameBoard.tabPlayer[1].score = 0;
				}

				GetWindowRect(GetDesktopWindow(), &g_desktopWindow);
				SetWindowPos(g_hwdn, HWND_TOPMOST, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), 0L);
				ShowWindow(g_hwdn, WS_MAXIMIZE);
				EndDialog(hDlg, LOWORD(wParam));			
				return (INT_PTR)TRUE;
			}
		}
		else if (LOWORD(wParam) == IDC_REGLE)
		{
			DialogBox(hInst, MAKEINTRESOURCE(IDD_REGLE), g_hwdn, Regle);
		}
		break;
	}
	return (INT_PTR)FALSE;
}

// Gestionnaire de messages pour la boîte de dialogue Regle.
INT_PTR CALLBACK Regle(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

// Timer des bulles.
VOID CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	Bubble *pBubble = new Bubble();
	pBubble->x = rand() % g_desktopWindow.right;
	pBubble->y = g_desktopWindow.top;
	pBubble->ray = BUBBLE_INITIAL_RAY;
	pBubble->xSpeed = 2;
	pBubble->ySpeed = 2;

	HANDLE hThread = CreateThread(NULL, 0, BubbleThread, pBubble, 0, 0);
	if (hThread == NULL)
	{
		ExitProcess(3);
	}
}

// Logique des bulles threads. 
DWORD WINAPI BubbleThread(LPVOID lParam)
{
	Bubble *pBubble = (Bubble*)lParam;

	float grav = 0.08;
	float res = 0.9999;

	while (true)
	{
		HDC hdc = GetDC(g_hwdn);

		//TODO répéter logique une fois pour les 2j
		//collision avec les harpons
		for (int i = 0; i < MAX_HARPOON; i++)
		{
			if (CollidedHarpoon(g_tabHarpoonJ1[i], *pBubble))
			{			
				//TODO add points to player1
				g_gameBoard.tabPlayer[0].score += 100;
				//TODO change position with velocity of harpoon hitting the main bubble
				if (pBubble->ray > MIN_RAY)
				{
					Bubble *pChildBubble1 = new Bubble();
					pChildBubble1->x = pBubble->x - CHILD_BUBBLE_DISTANCE;
					pChildBubble1->y = pBubble->y;
					pChildBubble1->ray = pBubble->ray / 2;
					HANDLE hThread = CreateThread(NULL, 0, BubbleThread, pChildBubble1, 0, 0);
					if (hThread == NULL)
					{
						ExitProcess(3);
					}

					Bubble *pChildBubble2 = new Bubble();
					pChildBubble2->x = pBubble->x + CHILD_BUBBLE_DISTANCE;
					pChildBubble2->y = pBubble->y;
					pChildBubble2->ray = pBubble->ray / 2;
					hThread = CreateThread(NULL, 0, BubbleThread, pChildBubble2, 0, 0);
					if (hThread == NULL)
					{
						ExitProcess(3);
					}
				}
	
				delete[] pBubble;
				InvalidateRect(g_hwdn, NULL, TRUE);
				ExitThread(0);
			}
			else if(g_gameBoard.nbPlayer == 2)
			{
				if (Collided(g_tabHarpoonJ2[i], *pBubble))
				{			
					//create sub bubble and verif if min is attain
					g_gameBoard.tabPlayer[1].score += 100;
					//add points player 2
					delete[] pBubble;
					InvalidateRect(g_hwdn, NULL, TRUE);
					ExitThread(0);
				}
			}
		}
		
		SelectObject(hdc, GetStockObject(DC_BRUSH));
		SetDCBrushColor(hdc, RGB(255, 20, 147));

		int lengthXY = sqrt(pow(pBubble->ray, 2) / 2);

		Ellipse(hdc, pBubble->x - lengthXY, pBubble->y - lengthXY, pBubble->x + lengthXY, pBubble->y + lengthXY);
		ReleaseDC(g_hwdn, hdc);
		Sleep(5);


		pBubble->ySpeed += grav;
		pBubble->ySpeed *= res;
		pBubble->xSpeed *= res;

		pBubble->x += pBubble->xSpeed;
		pBubble->y += pBubble->ySpeed;

		if (pBubble->y >= g_desktopWindow.bottom || pBubble->y < 0)
			pBubble->ySpeed *= -1;
		if (pBubble->x >= g_desktopWindow.right || pBubble->x < 0)
			pBubble->xSpeed *= -1;
	}
	return 0;
}

// Collision avec harpon.
BOOL Collided(POINT harpoon, Bubble bubble)
{
	bool collide = false;
	int distance = sqrt(pow(harpoon.x - bubble.x, 2) + pow(harpoon.y - bubble.y, 2));

	if (distance < bubble.ray)
		collide = true;
	return collide;
}

BOOL CollidedHarpoon(POINT harpoon, Bubble bubble)
{
	bool collide = false;
	int distance = sqrt(pow(harpoon.x - bubble.x, 2));

	if (distance < bubble.ray && harpoon.y <= bubble.y)
		collide = true;
	return collide;
}

BOOL CollidedCharacter(POINT character, Bubble bubble)
{
	bool collide = false;
	int distance = sqrt(pow(character.x - bubble.x, 2) + pow(character.y - bubble.y, 2));

	if (distance < bubble.ray)
		collide = true;
	return collide;
}

DWORD WINAPI HarpoonThread(LPVOID lParam)
{
	MSG msg = { 0 };
	HWND *hWnd;
	HDC hdc;
	int Pos = 0;
	POINT CharacterPosition;

	//
	int Left = 0;
	int Top = 0;
	int Right = 0;
	int Bottom = 0;
	hWnd = (HWND *)lParam;

	hdc = GetDC(*hWnd);

	//Outils de dessin

	Pos = g_characterPos.x + 25;
	HBRUSH brush = CreateSolidBrush(RGB(255,0,0));
	RECT rect;
	//Boucle du thread
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) == 0 && ((g_characterPos.y + Top) > 0))
	{
		rect = { Pos, g_characterPos.y + Top, Pos + 3 + Right, g_characterPos.y + Bottom };
		FillRect(hdc, &rect, brush);
		g_tabHarpoonJ1[0] = { Pos, g_characterPos.y + Top };
		Top = Top - 5; // Décrémenter pour augmenter la vitesse
		Sleep(3);
	}
	//Destruction du harpon précédent

	HBRUSH brosse = CreateSolidBrush(RGB(0, 0, 0));
	FillRect(hdc, &rect, brosse);
	DeleteObject(brosse);
	Anchor = NULL;
	ReleaseDC(*hWnd, hdc);
	DeleteObject(brush);
	ExitThread(ThreadID);

	return 0;
}

