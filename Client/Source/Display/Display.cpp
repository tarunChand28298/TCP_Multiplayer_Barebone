#include "Display.h"

Display::Display()
{
}

Display::~Display()
{
}

void Display::Initialize()
{
	//Create window:
	windowClassInfo.style = CS_OWNDC;
	windowClassInfo.lpfnWndProc = MainWindowProcedure;
	windowClassInfo.cbClsExtra = 0;
	windowClassInfo.cbWndExtra = 0;
	windowClassInfo.hInstance = GetModuleHandle(0);
	windowClassInfo.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	windowClassInfo.hCursor = LoadCursor(nullptr, IDC_ARROW);
	windowClassInfo.hbrBackground = nullptr;
	windowClassInfo.lpszMenuName = 0;
	windowClassInfo.lpszClassName = "MainWindowClassName";
	RegisterClass(&windowClassInfo);

	mainWindowHandle = CreateWindow("MainWindowClassName", "Display Window", WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, width + 16, height + 39, nullptr, nullptr, GetModuleHandle(0), this);
	ShowWindow(mainWindowHandle, SW_SHOW);

	//Create pixelbuffer:
	pixelbufferInfo.bmiHeader.biSize = sizeof(pixelbufferInfo.bmiHeader);
	pixelbufferInfo.bmiHeader.biWidth = width;
	pixelbufferInfo.bmiHeader.biHeight = height;
	pixelbufferInfo.bmiHeader.biPlanes = 1;
	pixelbufferInfo.bmiHeader.biBitCount = sizeof(int) * 8;
	pixelbufferInfo.bmiHeader.biCompression = BI_RGB;

	pixelBuffer = new int[width*height];
	ClearPixelbuffer();

	Start();
}

void Display::Update() {
	{
		//Handle window messages:
		MSG message = {};
		while (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
	}
	{
		//render:
		DrawMaster();
	}
}

void Display::Shutdown()
{
	End();
	UnregisterClass("MainWindowClassName", GetModuleHandle(0));

	delete[] pixelBuffer;
	pixelBuffer = nullptr;
}

void Display::PutPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char p) {
	if (x < 0 || x >= width || y < 0 || y >= height) { return; }

	int index = 0;
	int colour = 0;

	//calculate colour:
	colour = p;
	colour <<= 8;
	colour |= r;
	colour <<= 8;
	colour |= g;
	colour <<= 8;
	colour |= b;

	//calculate index:
	index = (width*y) + x;

	//Write to pixelbuffer:
	pixelBuffer[index] = colour;
}

void Display::PutPixel(int x, int y, int colour)
{
	if (x < 0 || x >= width || y < 0 || y >= height) { return; }
	pixelBuffer[(width*y) + x] = colour;
}

void Display::DrawLine(int x1, int y1, int x2, int y2, int colour)
{
	int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
	dx = x2 - x1; dy = y2 - y1;

	// straight lines idea by gurkanctn
	if (dx == 0) // Line is vertical
	{
		if (y2 < y1) std::swap(y1, y2);
		for (y = y1; y <= y2; y++)
			PutPixel(x1, y, colour);
		return;
	}

	if (dy == 0) // Line is horizontal
	{
		if (x2 < x1) std::swap(x1, x2);
		for (x = x1; x <= x2; x++)
			PutPixel(x, y1, colour);
		return;
	}

	// Line is Funk-aye
	dx1 = abs(dx); dy1 = abs(dy);
	px = 2 * dy1 - dx1;	py = 2 * dx1 - dy1;
	if (dy1 <= dx1)
	{
		if (dx >= 0)
		{
			x = x1; y = y1; xe = x2;
		}
		else
		{
			x = x2; y = y2; xe = x1;
		}

		PutPixel(x, y, colour);

		for (i = 0; x < xe; i++)
		{
			x = x + 1;
			if (px < 0)
				px = px + 2 * dy1;
			else
			{
				if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) y = y + 1; else y = y - 1;
				px = px + 2 * (dy1 - dx1);
			}
			PutPixel(x, y, colour);
		}
	}
	else
	{
		if (dy >= 0)
		{
			x = x1; y = y1; ye = y2;
		}
		else
		{
			x = x2; y = y2; ye = y1;
		}

		PutPixel(x, y, colour);

		for (i = 0; y < ye; i++)
		{
			y = y + 1;
			if (py <= 0)
				py = py + 2 * dx1;
			else
			{
				if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) x = x + 1; else x = x - 1;
				py = py + 2 * (dx1 - dy1);
			}
			PutPixel(x, y, colour);
		}
	}
}

void Display::DrawCircle(int x, int y, int radius, int colour, DrawMode mode)
{
	switch (mode) {
		case Fill: {
			int x0 = 0;
			int y0 = radius;
			int d = 3 - 2 * radius;
			if (!radius) return;

			auto drawline = [&](int sx, int ex, int ny)
			{
				for (int i = sx; i <= ex; i++)
					PutPixel(i, ny, colour);
			};

			while (y0 >= x0)
			{
				// Modified to draw scan-lines instead of edges
				drawline(x - x0, x + x0, y - y0);
				drawline(x - y0, x + y0, y - x0);
				drawline(x - x0, x + x0, y + y0);
				drawline(x - y0, x + y0, y + x0);
				if (d < 0) d += 4 * x0++ + 6;
				else d += 4 * (x0++ - y0--) + 10;
			}
			break;
		}
		case Outline: {
			int x0 = 0;
			int y0 = radius;
			int d = 3 - 2 * radius;
			if (!radius) return;

			while (y0 >= x0) // only formulate 1/8 of circle
			{
				PutPixel(x + x0, y - y0, colour);
				PutPixel(x + y0, y - x0, colour);
				PutPixel(x + y0, y + x0, colour);
				PutPixel(x + x0, y + y0, colour);
				PutPixel(x - x0, y + y0, colour);
				PutPixel(x - y0, y + x0, colour);
				PutPixel(x - y0, y - x0, colour);
				PutPixel(x - x0, y - y0, colour);
				if (d < 0) d += 4 * x0++ + 6;
				else d += 4 * (x0++ - y0--) + 10;
			}
			break;
		}
		default: {
			break;
		}
	}
}

void Display::DrawRectangle(int x, int y, int width, int height, int colour, DrawMode mode) {
	switch (mode)
	{
		case Fill: {
			for (int i = x; i < x + width; i++) {
				for (int j = y; j < y + height; j++) {
					PutPixel(i, j, colour);
				}
			}
			break;
		}
		case Outline: {
			DrawLine(x, y, x + width, y, colour);
			DrawLine(x + width, y, x + width, y + height, colour);
			DrawLine(x + width, y + height, x, y + height, colour);
			DrawLine(x, y + height, x, y, colour);
			break;
		}
		default: {
			break;
		}
	}
}

void Display::DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, int colour, DrawMode mode)
{
	switch (mode)
	{
		case Fill: {
			auto SWAP = [](int &x, int &y) { int t = x; x = y; y = t; };
			auto drawline = [&](int sx, int ex, int ny) { for (int i = sx; i <= ex; i++) PutPixel(i, ny, colour); };

			int t1x, t2x, y, minx, maxx, t1xp, t2xp;
			bool changed1 = false;
			bool changed2 = false;
			int signx1, signx2, dx1, dy1, dx2, dy2;
			int e1, e2;
			// Sort vertices
			if (y1 > y2) { SWAP(y1, y2); SWAP(x1, x2); }
			if (y1 > y3) { SWAP(y1, y3); SWAP(x1, x3); }
			if (y2 > y3) { SWAP(y2, y3); SWAP(x2, x3); }

			t1x = t2x = x1; y = y1;   // Starting points
			dx1 = (int)(x2 - x1); if (dx1 < 0) { dx1 = -dx1; signx1 = -1; }
			else signx1 = 1;
			dy1 = (int)(y2 - y1);

			dx2 = (int)(x3 - x1); if (dx2 < 0) { dx2 = -dx2; signx2 = -1; }
			else signx2 = 1;
			dy2 = (int)(y3 - y1);

			if (dy1 > dx1) {   // swap values
				SWAP(dx1, dy1);
				changed1 = true;
			}
			if (dy2 > dx2) {   // swap values
				SWAP(dy2, dx2);
				changed2 = true;
			}

			e2 = (int)(dx2 >> 1);
			// Flat top, just process the second half
			if (y1 == y2) goto next;
			e1 = (int)(dx1 >> 1);

			for (int i = 0; i < dx1;) {
				t1xp = 0; t2xp = 0;
				if (t1x < t2x) { minx = t1x; maxx = t2x; }
				else { minx = t2x; maxx = t1x; }
				// process first line until y value is about to change
				while (i < dx1) {
					i++;
					e1 += dy1;
					while (e1 >= dx1) {
						e1 -= dx1;
						if (changed1) t1xp = signx1;//t1x += signx1;
						else          goto next1;
					}
					if (changed1) break;
					else t1x += signx1;
				}
				// Move line
			next1:
				// process second line until y value is about to change
				while (1) {
					e2 += dy2;
					while (e2 >= dx2) {
						e2 -= dx2;
						if (changed2) t2xp = signx2;//t2x += signx2;
						else          goto next2;
					}
					if (changed2)     break;
					else              t2x += signx2;
				}
			next2:
				if (minx > t1x) minx = t1x; if (minx > t2x) minx = t2x;
				if (maxx < t1x) maxx = t1x; if (maxx < t2x) maxx = t2x;
				drawline(minx, maxx, y);    // Draw line from min to max points found on the y
											// Now increase y
				if (!changed1) t1x += signx1;
				t1x += t1xp;
				if (!changed2) t2x += signx2;
				t2x += t2xp;
				y += 1;
				if (y == y2) break;

			}
		next:
			// Second half
			dx1 = (int)(x3 - x2); if (dx1 < 0) { dx1 = -dx1; signx1 = -1; }
			else signx1 = 1;
			dy1 = (int)(y3 - y2);
			t1x = x2;

			if (dy1 > dx1) {   // swap values
				SWAP(dy1, dx1);
				changed1 = true;
			}
			else changed1 = false;

			e1 = (int)(dx1 >> 1);

			for (int i = 0; i <= dx1; i++) {
				t1xp = 0; t2xp = 0;
				if (t1x < t2x) { minx = t1x; maxx = t2x; }
				else { minx = t2x; maxx = t1x; }
				// process first line until y value is about to change
				while (i < dx1) {
					e1 += dy1;
					while (e1 >= dx1) {
						e1 -= dx1;
						if (changed1) { t1xp = signx1; break; }//t1x += signx1;
						else          goto next3;
					}
					if (changed1) break;
					else   	   	  t1x += signx1;
					if (i < dx1) i++;
				}
			next3:
				// process second line until y value is about to change
				while (t2x != x3) {
					e2 += dy2;
					while (e2 >= dx2) {
						e2 -= dx2;
						if (changed2) t2xp = signx2;
						else          goto next4;
					}
					if (changed2)     break;
					else              t2x += signx2;
				}
			next4:

				if (minx > t1x) minx = t1x; if (minx > t2x) minx = t2x;
				if (maxx < t1x) maxx = t1x; if (maxx < t2x) maxx = t2x;
				drawline(minx, maxx, y);
				if (!changed1) t1x += signx1;
				t1x += t1xp;
				if (!changed2) t2x += signx2;
				t2x += t2xp;
				y += 1;
				if (y > y3) return;
			}

			break;
		}
		case Outline: {
			DrawLine(x1, y1, x2, y2, colour);
			DrawLine(x2, y2, x3, y3, colour);
			DrawLine(x3, y3, x1, y1, colour);

			break;
		}
		default: {
			break;
		}
	}
}

void Display::DrawBitmap(int x, int y, const Bitmap& bitmap)
{
	for (int i = 0; i < bitmap.width; i++) {
		for (int j = 0; j < bitmap.height; j++) {
			PutPixel(x + i, y + j, bitmap.surface[i + j * bitmap.width]);
		}
	}
}

void Display::ClearPixelbuffer() {
	for (int i = 0; i < height*width; i++) {
		pixelBuffer[i] = defaultClearColour;
	}
}

void Display::Start()
{
}

void Display::Draw()
{
}

void Display::End()
{
}

void Display::PresentPixelbufferToWindow() {
	HDC currentDeviceContext = GetDC(mainWindowHandle);
	StretchDIBits(currentDeviceContext, 0, 0, width, height, 0, 0, width, height, pixelBuffer, &pixelbufferInfo, DIB_RGB_COLORS, SRCCOPY);
	ReleaseDC(mainWindowHandle, currentDeviceContext);
}

void Display::DrawMaster() {
	if (pixelBuffer == nullptr) { return; }

	ClearPixelbuffer();
	Draw();
	PresentPixelbufferToWindow();
}

LRESULT Display::MainWindowProcedure(HWND windowHandle, UINT message, WPARAM wparam, LPARAM lparam) {
	static Display* displayInstance = nullptr;
	switch (message)
	{
		case WM_CREATE: {
			displayInstance = (Display*)((LPCREATESTRUCT)lparam)->lpCreateParams;
			displayInstance->running = true;
			break;
		}
		case WM_CLOSE: {
			DestroyWindow(windowHandle);
			break;
		}
		case WM_DESTROY: {
			displayInstance->running = false;
			displayInstance->Shutdown();
			break;
		}
		default: {
			break;
		}
	}

	return DefWindowProc(windowHandle, message, wparam, lparam);
}