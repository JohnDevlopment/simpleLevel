// slope bitmask tables
constexpr short int _slope_0_offsets[32] = {
	31, 31, 31, 30,
	30, 30, 29, 28,
	28, 27, 27, 27,
	26, 26, 26, 26,
	25, 25, 25, 25,
	25, 25, 25, 24,
	24, 24, 24, 24,
	24, 24, 24, 24
};

constexpr short int _slope_1_offsets[32] = {
	23, 23, 23, 23,
	23, 23, 22, 22,
	22, 22, 22, 21,
	21, 21, 21, 20,
	20, 20, 20, 19,
	19, 19, 19, 18,
	18, 18, 18, 17,
	17, 17, 16, 16
};

constexpr short int _slope_2_offsets[32] = {
	15, 15, 14, 14,
	14, 14, 13, 13,
	13, 12, 12, 12,
	11, 11, 11, 11,
	11, 11, 10, 10,
	10, 10,  9,  9,
	 9,  9,  8,  8,
	 8,  8,  8,  8
};

constexpr short int _slope_3_offsets[32] = {
	 7,  7,  7,  6,
	 6,  6,  6,  6,
	 6,  5,  5,  5,
	 5,  5,  4,  4,
	 4,  4,  4,  3,
	 3,  3,  3,  3,
	 2,  2,  2,  1,
	 1,  1,  0,  0
};

constexpr short int _slope_x_offsets[32] = {
	 0,  0,  0,  0,
	 0,  0,  0,  0,
	 0,  0,  0,  0,
	 0,  0,  0,  0,
	 0,  0,  0,  0,
	 0,  0,  0,  0,
	 0,  0,  0,  0,
	 0,  0,  0,  0
};

/*constexpr short int _slope_x_offsets[32] = {
	 0,  0,  0,  0,
	 0,  0,  0,  0,
	 0,  0,  0,  0,
	 0,  0,  0,  0,
	 0,  0,  0,  0,
	 0,  0,  0,  0,
	 0,  0,  0,  0,
	 0,  0,  0,  0
};*/

constexpr const short int* _slope_tables[7] = {
	_slope_0_offsets,
	_slope_1_offsets,
	_slope_2_offsets,
	_slope_3_offsets,
	_slope_x_offsets,
	_slope_x_offsets,
	_slope_x_offsets
};
