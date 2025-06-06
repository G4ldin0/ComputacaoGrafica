/*

#define WIN32_LEAN_AND_MEAN
#include "DXUT.h"
#include <sstream>
using std::stringstream;

// Prot�tipo de fun��es e declara��es globais
Window* window;
void Welcome();
void Bye();

// ------------------------------------------------------------------------------
//                                  WinMain                                      
// ------------------------------------------------------------------------------

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	// configura e cria a janela
	window = new Window();
	window->Mode(WINDOWED);
	window->Size(1024, 600);
	window->Color(0, 122, 204);
	window->Title("Input DXUT");
	window->Icon(IDI_ICON);
	window->Cursor(IDC_CURSOR);
	window->InFocus(Welcome);
	window->LostFocus(Bye);
	window->Create();

	// leitura da entrada
	Input* input = new Input();

	MSG msg = { 0 };
	stringstream text;

	int x = window->CenterX();
	int y = window->CenterY();

	HDC hdc = GetDC(window->Id());


	// trata as mensagens destinadas a janela
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (input->KeyPress(VK_ESCAPE))
			window->Close();

		// CTRL, ENTER, ESPA�O
		if (input->KeyDown(VK_CONTROL))
			window->Print("CTRL", x - 125, y + 150, RGB(255, 255, 255));
		if (input->KeyDown(VK_RETURN))
			window->Print("ENTER", x - 25, y + 150, RGB(255, 255, 255));
		if (input->KeyDown(VK_SPACE))
			window->Print("SPACE", x + 75, y + 150, RGB(255, 255, 255));

		// setas de dire��o
		if (input->KeyDown(VK_LEFT))
			window->Print("LEFT", x - 45, y, RGB(255, 255, 255));
		if (input->KeyDown(VK_RIGHT))
			window->Print("RIGHT", x, y, RGB(255, 255, 255));
		if (input->KeyDown(VK_UP))
			window->Print("UP", x - 10, y - 20, RGB(255, 255, 255));
		if (input->KeyDown(VK_DOWN))
			window->Print("DOWN", x - 25, y + 20, RGB(255, 255, 255));

		// teclar R
		if (input->KeyPress('R'))
			window->Clear();

		// posi��o do mouse
		text.str("");
		text << input->MouseX() << " x " << input->MouseY();

		// bot�es do mouse
		if (input->KeyDown(VK_LBUTTON))
		{
			window->Clear();
			window->Print("BOT�O ESQ", x - 45, y - 230, RGB(255, 255, 255));
			window->Print(text.str().c_str(), x - 32, y - 210, RGB(255, 255, 255));
		}
		if (input->KeyDown(VK_MBUTTON))
		{
			window->Clear();
			window->Print("BOT�O MEIO", x - 50, y - 230, RGB(255, 255, 255));
			window->Print(text.str().c_str(), x - 32, y - 210, RGB(255, 255, 255));
		}
		if (input->KeyDown(VK_RBUTTON))
		{
			window->Clear();
			window->Print("BOT�O DIR", x - 45, y - 230, RGB(255, 255, 255));
			window->Print(text.str().c_str(), x - 32, y - 210, RGB(255, 255, 255));
		}

		// roda do mouse
		if (short rotation = input->MouseWheel(); rotation != 0)
		{
			window->Clear();
			window->Print("ROLAGEM", x - 35, y - 250, RGB(255, 255, 255));

			if (rotation > 0)
				window->Print("PARA FRENTE", x - 52, y - 230, RGB(255, 255, 255));
			else
				window->Print("PARA TR�S", x - 40, y - 230, RGB(255, 255, 255));

			text.str("");
			text << rotation;
			window->Print(text.str().c_str(), x - 8, y - 210, RGB(255, 255, 255));
		}

		// inicia entrada de texto
		if (input->KeyPress(VK_INSERT))
			input->Read();

		window->Print(Input::Text(), x - 120, y - 120, RGB(255, 255, 255));


		SetPixel(hdc, input->MouseX(), input->MouseY(), RGB(255,255,255));

	}
	ReleaseDC(window->Id(), hdc);

	// fim do programa
	delete input;
	delete window;
	return 0;
}

// ----------------------------------------------------------------------------

void Welcome()
{
	window->Clear();
	window->Print("De volta?", 485, 250, RGB(255, 255, 255));
}

void Bye()
{
	window->Clear();
	window->Print("At� Logo!", 485, 250, RGB(255, 255, 255));
}

// ----------------------------------------------------------------------------

*/