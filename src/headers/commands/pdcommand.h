#ifndef PDCOMMAND_H_INCLUDED
#define PDCOMMAND_H_INCLUDED

/* System headers */
#include <iostream>

/* User-defined headers */
#include "stdinc.h"

/* Command interface: derived classes will execute commands specified by the user. */
class PDCommand {
	public:
	
	PDCommand() {}
	PDCommand(const PDCommand&) = delete;
	PDCommand(PDCommand&&) = delete;
	virtual ~PDCommand() {}
	
	virtual void DoAction() = 0;
};

#endif /* PDCOMMAND_H_INCLUDED */
