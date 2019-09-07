# included by main.tcl

set lvlhash [CallProc GetHash $CurrentLevel 5000 200]
switch $lvlhash {
	default {
	  # default
	}
	
	114 {
	  # "rt1-1.dat"
	  CallProc level background color 84 251 245
	}
	
	319 {
	  # "rt1-2.dat"
	}
}
