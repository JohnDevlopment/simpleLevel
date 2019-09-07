# Tile Bit Flags

Each tile has a set of bit flags, 16 in all:

	0000 0000 0000 0000
	|||| |||| |||| ||||
	|||| |||| |||| |||=> This bit indicates whether the tile is on the back (off) or front (on) tilemap
	|||| |||| |||| ||==> This bit is on if the tile is a slope
	|||| |||| |||| |===> UNUSED
	|||| |||| |||| ====> This bit indicates that the slope is inverted
	|||| |||| ||||    |
	|||| |||| =========> These bits are the table lookup for the slope
	|||| ||||         |
	|||| ||||==========>
	|||| |||===========>
	|||| ||============>
	|||| |=============>
	||||              |
	||||===============>
	|||================>
	||=================>
	|==================>
