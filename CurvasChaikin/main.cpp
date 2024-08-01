#include <Engine.h>
#include <Error.h>
#include "Resources.h"
#include "DXApp.h"

// ------------------------------------------------------------------------------
//                                  WinMain                                      
// ------------------------------------------------------------------------------

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	try
	{
		// cria motor
		Engine* engine = new Engine();

		// configura a janela
		engine->window->Mode(WINDOWED);
		engine->window->Size(1024, 600);
		engine->window->Color(0, 0, 0);
		engine->window->Title("DXGI DXUT");
		engine->window->Icon(IDI_ICON);
		engine->window->Cursor(IDC_CURSOR);

		// aplicação pausa/resume ao perder/ganhar o foco
		engine->window->LostFocus(Engine::Pause);
		engine->window->InFocus(Engine::Resume);

		// cria e executa a aplicação
		int exitCode = engine->Start(new DXApp());

		// finaliza execução
		delete engine;
		return exitCode;
	}
	catch (Error& e)
	{
		MessageBox(nullptr, e.ToString().data() , "DXGI DXUT", MB_OK);
		return 0;
	}
}

