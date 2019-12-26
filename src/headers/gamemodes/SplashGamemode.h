#ifndef SPLASHGAMEMODE_H_INCLUDED
#define SPLASHGAMEMODE_H_INCLUDED

#include "pdgamemode.h"

class PDCommand;

class PDSplashGamemode : public PDGamemode {
	int m_counter;
	int m_maxTime;
	PDCommand* m_command;
	
public:
	PDSplashGamemode();
	virtual ~PDSplashGamemode();
	
	virtual void Init();
	virtual void Update(float dt);
	virtual void Shutdown();
};

#endif /* SPLASHGAMEMODE_H_INCLUDED */
