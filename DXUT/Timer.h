#ifndef DXUT_TIMER_H
#define DXUT_TIMER_H

// -------------------------------------------------------------------------------

#include <windows.h>			// acesso ao contador de alta precis�o do Windows

// -------------------------------------------------------------------------------

class Timer
{
private:
	LARGE_INTEGER start, end;	// valores de in�cio e fim do contador
	LARGE_INTEGER freq;			// frequ�ncia do contador
	bool stoped;				// estado da contagem

public:
	Timer();					// construtor

	void  Start();				// inicia/retoma contagem do tempo
	void  Stop();				// p�ra contagem do tempo
	float Reset();				// reinicia contagem e retorna tempo transcorrido
	float Elapsed();			// retorna tempo transcorrido em segundos	
	bool  Elapsed(float secs);	// verifica se transcorreu "secs" segundos
};

// -------------------------------------------------------------------------------

// Fun��es Inline

inline bool Timer::Elapsed(float secs)
{
	return (Elapsed() >= secs ? true : false);
}

// -------------------------------------------------------------------------------

#endif