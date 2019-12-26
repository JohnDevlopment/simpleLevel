#ifndef PDEXITCMD_H_INCLUDED
#define PDEXITCMD_H_INCLUDED

/* System headers */

/* User-defined headers */
#include "pdcommand.h"

/* Command to break the main loop
and start the default shutdown procedure. */
class ExitCmd : public PDCommand {
	public:
	
	ExitCmd();
	virtual ~ExitCmd();
	
	virtual void DoAction();
};

#endif /* PDEXITCMD_H_INCLUDED */
