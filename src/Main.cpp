#include <windows.h>
#include <string_view>
#include <vector>
#include <sstream>
#include "../res/resource.h"

#define CURRENT_HINSTANCE GetModuleHandle(NULL)

LRESULT CALLBACK wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HBITMAP hBmp = NULL;
	static HCURSOR hCur = NULL;
	switch (uMsg)
	{
		case WM_CREATE:
		{
			hBmp = (HBITMAP)LoadImage(CURRENT_HINSTANCE, MAKEINTRESOURCE(IDB_TEST_BITMAP), IMAGE_BITMAP, 0, 0, LR_SHARED);
			hCur = (HCURSOR)LoadCursor(CURRENT_HINSTANCE, MAKEINTRESOURCE(IDC_TEST_CURSOR));

			/* If cursor is set in WNDCLASSEX than SetCursor does not work */
			// SetCursor(hCur);
			SetClassLongPtr(hWnd, GCLP_HCURSOR, (LONG)hCur);

			auto hResInfo = FindResource(CURRENT_HINSTANCE, MAKEINTRESOURCE(IDU_TEST_USER), MAKEINTRESOURCE(SOME));
			auto hRes = LoadResource(CURRENT_HINSTANCE, hResInfo);
			auto size = ::SizeofResource(NULL, hResInfo);
			auto hResData = LockResource(hRes); // MSDN says that there is no need to unlock this resource

			std::vector<unsigned char> binData(static_cast<unsigned char*>(hResData), static_cast<unsigned char*>(hResData) + size);
			std::wstringstream ss;

			ss << L"Size of binary: " << size << L" \nData: ";
			ss << std::hex;

			for (const auto& byte : binData)
				ss << byte << " ";

			MessageBox(hWnd, ss.str().c_str(), L"User defined resource", MB_OK);

			return 0;
		}
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);

			HDC memDC = CreateCompatibleDC(hdc);
			SelectObject(memDC, hBmp);

			BitBlt(hdc, 0, 0, 1280, 720, memDC, 0, 0, SRCCOPY);

			EndPaint(hWnd, &ps);
			return 0;
		}
		case WM_LBUTTONDOWN:
		{
			const wchar_t* hello;
			int size = LoadString(CURRENT_HINSTANCE, IDS_HELLO, (LPWSTR)&hello, 0);
			std::wstring h(hello, size);

			MessageBox(NULL, h.c_str(), L"Hello string from resources", MB_OK);
			return 0;
		}
		case WM_CLOSE:
		{
			DeleteObject(hBmp);
			DestroyCursor(hCur);
			DestroyWindow(hWnd);
			return 0;
		}
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	HINSTANCE hInst = CURRENT_HINSTANCE;

	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(wc);
	wc.lpszClassName = L"ResourceWindow";
	wc.hInstance = hInst;
	wc.lpfnWndProc = wndProc;
	wc.hbrBackground = HBRUSH(COLOR_WINDOW + 1);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);

	RegisterClassEx(&wc);

	HWND hWnd = CreateWindow(L"ResourceWindow", L"Resource Window", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 
		200, 200, 1280, 720, NULL, NULL, CURRENT_HINSTANCE, NULL);

	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		DispatchMessage(&msg);
	}

	return 0;
}