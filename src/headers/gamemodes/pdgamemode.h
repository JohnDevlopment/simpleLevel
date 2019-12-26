#ifndef PDGAMEMODE_H_INCLUDED
#define PDGAMEMODE_H_INCLUDED

class PDGamemode {
public:
	
	PDGamemode() {}
	virtual ~PDGamemode() {}
	
	virtual void Init() = 0;
	virtual void Update(float dt) = 0;
	virtual void Shutdown() = 0;
};

#endif /* PDGAMEMODE_H_INCLUDED */
