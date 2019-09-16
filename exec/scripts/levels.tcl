# included by main.tcl

set lvlhash [CallProc GetHash $CurrentLevel 5000 200]
switch $lvlhash {
	114 {
	  # "rt1-1.dat"
	  CallProc level background color 84 251 245
	  CallProc level background offsety 1 352
	}
	
	319 {
	  # "rt1-2.dat"
	  puts "rt1-2.dat"
	}
	
	default {
	  # default
	  error "unknown level: $CurrentLevel"
	}
}
