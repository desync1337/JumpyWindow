#include "Windows.h"
#include <iostream>

bool grabbed = false;
bool grabbable = false; // true если хотите покидать его туда сюда

float deltay;
float deltax;

float deltaxnonz = 0;
float deltaynonz = 0;
POINT curr;
POINT last;

RECT windowRect;

DWORD WINAPI Calc(LPVOID lParam)
{
	GetWindowRect((HWND)lParam, &windowRect);
	float posY = static_cast<float>(windowRect.top);
	float posX = static_cast<float>(windowRect.left);
	float vely = rand() * 5;
	float velx = rand() * 0.01;

	const float gravity = 9.8f;
	const float dampening = 0.9f;
	
	int windowHeight = windowRect.bottom - windowRect.top;
	int windowWidth = windowRect.left - windowRect.right;

	DWORD lastTime = GetTickCount();


	while (true)
	{
		DWORD currentTime = GetTickCount();
		float deltaTime = (currentTime - lastTime) / 100.0f;
		lastTime = currentTime;

		if (!grabbed) 
		{
			

			vely += gravity * deltaTime;

			posY += vely * deltaTime;
			posX += velx * deltaTime;

			//top
			if (posY < 0) 
			{
				posY = 0;
				vely = -vely * dampening;
			}

			// bottom
			if (posY + windowHeight > 1080)
			{
				posY = 1080 - windowHeight;
				vely = -vely * dampening;

				if (std::abs(vely) < 0.5f)
				vely = 0.0f;
			}

			//right
			if (posX - windowWidth > 1920) 
			{
				posX = 1920 + windowWidth;
				velx = -velx * dampening;
			}

			//left
			if (posX < 0)
			{
				posX = 0;
				velx = -velx * dampening;
			}

			SetWindowPos(
				(HWND)lParam,
				HWND_TOPMOST,
				static_cast<int>(posX),
				static_cast<int>(posY),
				0,
				0,
				SWP_NOSIZE | SWP_SHOWWINDOW
			);
		}
		else
		{

			GetWindowRect((HWND)lParam, &windowRect);
			posY = static_cast<float>(windowRect.top);
			posX = static_cast<float>(windowRect.left);
			windowHeight = windowRect.bottom - windowRect.top;
			windowWidth = windowRect.left - windowRect.right;

			GetCursorPos(&curr);
			deltay = (curr.y - last.y);
			deltax = (curr.x - last.x);
			last = curr;

			if (std::abs(deltay) > 0)
				deltaynonz = deltay;


			if (std::abs(deltax) > 0) 
				deltaxnonz = deltax;

			vely = deltaynonz * 10;
			velx = deltaxnonz * 10;
			
		}
		
	}

	
	return 0;
}

int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR szCmdLine, int ncmdShow) 
{
	

	MSG msg{};
	HWND hwnd{};
	WNDCLASSEX wc{sizeof(WNDCLASSEX)};
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hIconSm = LoadCursor(nullptr, IDI_APPLICATION);
	wc.hIcon = LoadCursor(nullptr, IDI_APPLICATION);
	wc.hInstance = hInstance;
	wc.lpfnWndProc = [](HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)->LRESULT
	{
			switch (uMsg)
			{
			case WM_CREATE:
			{
				
				CreateThread(NULL, 0, Calc, hWnd, 0, NULL);
				ShowWindow(GetConsoleWindow(), SW_HIDE);
				
			}
			return 0;


			case WM_PAINT:
			{

				grabbed = false;
				if ((GetKeyState(VK_LBUTTON) & 0x80) != 0 && grabbable)
					grabbed = true;

			}
			//grabbed = false;
			return 0;

			case WM_DESTROY:
			{
				PostQuitMessage(1337);
			}
				
				return 0;
			}
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
	};
	wc.lpszClassName = L"Jumpy Window";
	wc.lpszMenuName = nullptr;
	wc.style = CS_VREDRAW | CS_HREDRAW;

	if (!RegisterClassEx(&wc))
		return EXIT_FAILURE;

	hwnd = CreateWindow(wc.lpszClassName, L"окно", WS_SYSMENU , 0, 1080-250, 250, 250, nullptr, nullptr, wc.hInstance, nullptr);

	if (hwnd == INVALID_HANDLE_VALUE)
		return EXIT_FAILURE;
	ShowWindow(hwnd, ncmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return static_cast<int>(msg.wParam);

}