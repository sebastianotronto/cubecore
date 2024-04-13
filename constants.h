#define NORMAL 0
#define INVERSE 1

#define _2p11  2048U
#define _2p12  4096U
#define _3p7   2187U
#define _3p8   6561U
#define _12c4  495U
#define _8c4   70U

#define _c_ufr      0U
#define _c_ubl      1U
#define _c_dfl      2U
#define _c_dbr      3U
#define _c_ufl      4U
#define _c_ubr      5U
#define _c_dfr      6U
#define _c_dbl      7U

#define _e_uf       0U
#define _e_ub       1U
#define _e_db       2U
#define _e_df       3U
#define _e_ur       4U
#define _e_ul       5U
#define _e_dl       6U
#define _e_dr       7U
#define _e_fr       8U
#define _e_fl       9U
#define _e_bl       10U
#define _e_br       11U

#define _eoshift    4U
#define _coshift    5U

#define _pbits      0xFU
#define _esepbit1   0x4U
#define _esepbit2   0x8U
#define _csepbit    0x4U
#define _eobit      0x10U
#define _cobits     0xF0U
#define _cobits2    0x60U
#define _ctwist_cw  0x20U
#define _ctwist_ccw 0x40U
#define _eflip      0x10U
#define _error      0xFFU

typedef enum {
	U, U2, U3, D, D2, D3,
	R, R2, R3, L, L2, L3,
	F, F2, F3, B, B2, B3
} move_t;

typedef enum {
	UFr, ULr, UBr, URr, DFr, DLr, DBr, DRr,
	RUr, RFr, RDr, RBr, LUr, LFr, LDr, LBr,
	FUr, FRr, FDr, FLr, BUr, BRr, BDr, BLr,

	UFm, ULm, UBm, URm, DFm, DLm, DBm, DRm,
	RUm, RFm, RDm, RBm, LUm, LFm, LDm, LBm,
	FUm, FRm, FDm, FLm, BUm, BRm, BDm, BLm
} trans_t;

_static cube_t zero = { .corner = {0}, .edge = {0} };
_static cube_t solved = {
	.corner = {0, 1, 2, 3, 4, 5, 6, 7},
	.edge = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}
};

_static cube_t move_table[] = {
	[U] = {
		.corner = {5, 4, 2, 3, 0, 1, 6, 7},
		.edge = {4, 5, 2, 3, 1, 0, 6, 7, 8, 9, 10, 11}
	},
	[U2] = {
		.corner = {1, 0, 2, 3, 5, 4, 6, 7},
		.edge = {1, 0, 2, 3, 5, 4, 6, 7, 8, 9, 10, 11}
	},
	[U3] = {
		.corner = {4, 5, 2, 3, 1, 0, 6, 7},
		.edge = {5, 4, 2, 3, 0, 1, 6, 7, 8, 9, 10, 11}
	},
	[D] = {
		.corner = {0, 1, 7, 6, 4, 5, 2, 3},
		.edge = {0, 1, 7, 6, 4, 5, 2, 3, 8, 9, 10, 11}
	},
	[D2] = {
		.corner = {0, 1, 3, 2, 4, 5, 7, 6},
		.edge = {0, 1, 3, 2, 4, 5, 7, 6, 8, 9, 10, 11}
	},
	[D3] = {
		.corner = {0, 1, 6, 7, 4, 5, 3, 2},
		.edge = {0, 1, 6, 7, 4, 5, 3, 2, 8, 9, 10, 11}
	},
	[R] = {
		.corner = {70, 1, 2, 69, 4, 32, 35, 7},
		.edge = {0, 1, 2, 3, 8, 5, 6, 11, 7, 9, 10, 4}
	},
	[R2] = {
		.corner = {3, 1, 2, 0, 4, 6, 5, 7},
		.edge = {0, 1, 2, 3, 7, 5, 6, 4, 11, 9, 10, 8}
	},
	[R3] = {
		.corner = {69, 1, 2, 70, 4, 35, 32, 7},
		.edge = {0, 1, 2, 3, 11, 5, 6, 8, 4, 9, 10, 7}
	},
	[L] = {
		.corner = {0, 71, 68, 3, 33, 5, 6, 34},
		.edge = {0, 1, 2, 3, 4, 10, 9, 7, 8, 5, 6, 11}
	},
	[L2] = {
		.corner = {0, 2, 1, 3, 7, 5, 6, 4},
		.edge = {0, 1, 2, 3, 4, 6, 5, 7, 8, 10, 9, 11}
	},
	[L3] = {
		.corner = {0, 68, 71, 3, 34, 5, 6, 33},
		.edge = {0, 1, 2, 3, 4, 9, 10, 7, 8, 6, 5, 11}
	},
	[F] = {
		.corner = {36, 1, 38, 3, 66, 5, 64, 7},
		.edge = {25, 1, 2, 24, 4, 5, 6, 7, 16, 19, 10, 11}
	},
	[F2] = {
		.corner = {2, 1, 0, 3, 6, 5, 4, 7},
		.edge = {3, 1, 2, 0, 4, 5, 6, 7, 9, 8, 10, 11}
	},
	[F3] = {
		.corner = {38, 1, 36, 3, 64, 5, 66, 7},
		.edge = {24, 1, 2, 25, 4, 5, 6, 7, 19, 16, 10, 11}
	},
	[B] = {
		.corner = {0, 37, 2, 39, 4, 67, 6, 65},
		.edge = {0, 27, 26, 3, 4, 5, 6, 7, 8, 9, 17, 18}
	},
	[B2] = {
		.corner = {0, 3, 2, 1, 4, 7, 6, 5},
		.edge = {0, 2, 1, 3, 4, 5, 6, 7, 8, 9, 11, 10}
	},
	[B3] = {
		.corner = {0, 39, 2, 37, 4, 65, 6, 67},
		.edge = {0, 26, 27, 3, 4, 5, 6, 7, 8, 9, 18, 17}
	},
};

_static cube_t trans_table[][2] = {
	[UFr] = {
		[NORMAL] = {
			.corner = {0, 1, 2, 3, 4, 5, 6, 7},
			.edge = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}
		},
		[INVERSE] = {
			.corner = {0, 1, 2, 3, 4, 5, 6, 7},
			.edge = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}
		}
	},
	[ULr] = {
		[NORMAL] = {
			.corner = {4, 5, 7, 6, 1, 0, 2, 3},
			.edge = {5, 4, 7, 6, 0, 1, 2, 3, 25, 26, 27, 24}
		},
		[INVERSE] = {
			.corner = {5, 4, 6, 7, 0, 1, 3, 2},
			.edge = {4, 5, 6, 7, 1, 0, 3, 2, 27, 24, 25, 26}
		}
	},
	[UBr] = {
		[NORMAL] = {
			.corner = {1, 0, 3, 2, 5, 4, 7, 6},
			.edge = {1, 0, 3, 2, 5, 4, 7, 6, 10, 11, 8, 9}
		},
		[INVERSE] = {
			.corner = {1, 0, 3, 2, 5, 4, 7, 6},
			.edge = {1, 0, 3, 2, 5, 4, 7, 6, 10, 11, 8, 9}
		}
	},
	[URr] = {
		[NORMAL] = {
			.corner = {5, 4, 6, 7, 0, 1, 3, 2},
			.edge = {4, 5, 6, 7, 1, 0, 3, 2, 27, 24, 25, 26}
		},
		[INVERSE] = {
			.corner = {4, 5, 7, 6, 1, 0, 2, 3},
			.edge = {5, 4, 7, 6, 0, 1, 2, 3, 25, 26, 27, 24}
		}
	},
	[DFr] = {
		[NORMAL] = {
			.corner = {2, 3, 0, 1, 6, 7, 4, 5},
			.edge = {3, 2, 1, 0, 6, 7, 4, 5, 9, 8, 11, 10}
		},
		[INVERSE] = {
			.corner = {2, 3, 0, 1, 6, 7, 4, 5},
			.edge = {3, 2, 1, 0, 6, 7, 4, 5, 9, 8, 11, 10}
		}
	},
	[DLr] = {
		[NORMAL] = {
			.corner = {7, 6, 4, 5, 2, 3, 1, 0},
			.edge = {6, 7, 4, 5, 2, 3, 0, 1, 26, 25, 24, 27}
		},
		[INVERSE] = {
			.corner = {7, 6, 4, 5, 2, 3, 1, 0},
			.edge = {6, 7, 4, 5, 2, 3, 0, 1, 26, 25, 24, 27}
		}
	},
	[DBr] = {
		[NORMAL] = {
			.corner = {3, 2, 1, 0, 7, 6, 5, 4},
			.edge = {2, 3, 0, 1, 7, 6, 5, 4, 11, 10, 9, 8}
		},
		[INVERSE] = {
			.corner = {3, 2, 1, 0, 7, 6, 5, 4},
			.edge = {2, 3, 0, 1, 7, 6, 5, 4, 11, 10, 9, 8}
		}
	},
	[DRr] = {
		[NORMAL] = {
			.corner = {6, 7, 5, 4, 3, 2, 0, 1},
			.edge = {7, 6, 5, 4, 3, 2, 1, 0, 24, 27, 26, 25}
		},
		[INVERSE] = {
			.corner = {6, 7, 5, 4, 3, 2, 0, 1},
			.edge = {7, 6, 5, 4, 3, 2, 1, 0, 24, 27, 26, 25}
		}
	},
	[RUr] = {
		[NORMAL] = {
			.corner = {64, 67, 65, 66, 37, 38, 36, 39},
			.edge = {20, 23, 22, 21, 24, 27, 26, 25, 0, 1, 2, 3}
		},
		[INVERSE] = {
			.corner = {32, 34, 35, 33, 70, 68, 69, 71},
			.edge = {8, 9, 10, 11, 16, 19, 18, 17, 20, 23, 22, 21}
		}
	},
	[RFr] = {
		[NORMAL] = {
			.corner = {38, 37, 36, 39, 64, 67, 66, 65},
			.edge = {24, 27, 26, 25, 23, 20, 21, 22, 19, 16, 17, 18}
		},
		[INVERSE] = {
			.corner = {36, 39, 38, 37, 66, 65, 64, 67},
			.edge = {25, 26, 27, 24, 21, 22, 23, 20, 16, 19, 18, 17}
		}
	},
	[RDr] = {
		[NORMAL] = {
			.corner = {67, 64, 66, 65, 38, 37, 39, 36},
			.edge = {23, 20, 21, 22, 27, 24, 25, 26, 2, 3, 0, 1}
		},
		[INVERSE] = {
			.corner = {33, 35, 34, 32, 71, 69, 68, 70},
			.edge = {10, 11, 8, 9, 17, 18, 19, 16, 21, 22, 23, 20}
		}
	},
	[RBr] = {
		[NORMAL] = {
			.corner = {37, 38, 39, 36, 67, 64, 65, 66},
			.edge = {27, 24, 25, 26, 20, 23, 22, 21, 17, 18, 19, 16}
		},
		[INVERSE] = {
			.corner = {37, 38, 39, 36, 67, 64, 65, 66},
			.edge = {27, 24, 25, 26, 20, 23, 22, 21, 17, 18, 19, 16}
		}
	},
	[LUr] = {
		[NORMAL] = {
			.corner = {65, 66, 64, 67, 36, 39, 37, 38},
			.edge = {21, 22, 23, 20, 26, 25, 24, 27, 1, 0, 3, 2}
		},
		[INVERSE] = {
			.corner = {34, 32, 33, 35, 68, 70, 71, 69},
			.edge = {9, 8, 11, 10, 19, 16, 17, 18, 22, 21, 20, 23}
		}
	},
	[LFr] = {
		[NORMAL] = {
			.corner = {36, 39, 38, 37, 66, 65, 64, 67},
			.edge = {25, 26, 27, 24, 21, 22, 23, 20, 16, 19, 18, 17}
		},
		[INVERSE] = {
			.corner = {38, 37, 36, 39, 64, 67, 66, 65},
			.edge = {24, 27, 26, 25, 23, 20, 21, 22, 19, 16, 17, 18}
		}
	},
	[LDr] = {
		[NORMAL] = {
			.corner = {66, 65, 67, 64, 39, 36, 38, 37},
			.edge = {22, 21, 20, 23, 25, 26, 27, 24, 3, 2, 1, 0}
		},
		[INVERSE] = {
			.corner = {35, 33, 32, 34, 69, 71, 70, 68},
			.edge = {11, 10, 9, 8, 18, 17, 16, 19, 23, 20, 21, 22}
		}
	},
	[LBr] = {
		[NORMAL] = {
			.corner = {39, 36, 37, 38, 65, 66, 67, 64},
			.edge = {26, 25, 24, 27, 22, 21, 20, 23, 18, 17, 16, 19}
		},
		[INVERSE] = {
			.corner = {39, 36, 37, 38, 65, 66, 67, 64},
			.edge = {26, 25, 24, 27, 22, 21, 20, 23, 18, 17, 16, 19}
		}
	},
	[FUr] = {
		[NORMAL] = {
			.corner = {68, 70, 69, 71, 32, 34, 33, 35},
			.edge = {16, 19, 18, 17, 9, 8, 11, 10, 5, 4, 7, 6}
		},
		[INVERSE] = {
			.corner = {68, 70, 69, 71, 32, 34, 33, 35},
			.edge = {16, 19, 18, 17, 9, 8, 11, 10, 5, 4, 7, 6}
		}
	},
	[FRr] = {
		[NORMAL] = {
			.corner = {32, 34, 35, 33, 70, 68, 69, 71},
			.edge = {8, 9, 10, 11, 16, 19, 18, 17, 20, 23, 22, 21}
		},
		[INVERSE] = {
			.corner = {64, 67, 65, 66, 37, 38, 36, 39},
			.edge = {20, 23, 22, 21, 24, 27, 26, 25, 0, 1, 2, 3}
		}
	},
	[FDr] = {
		[NORMAL] = {
			.corner = {70, 68, 71, 69, 34, 32, 35, 33},
			.edge = {19, 16, 17, 18, 8, 9, 10, 11, 7, 6, 5, 4}
		},
		[INVERSE] = {
			.corner = {69, 71, 68, 70, 33, 35, 32, 34},
			.edge = {17, 18, 19, 16, 11, 10, 9, 8, 4, 5, 6, 7}
		}
	},
	[FLr] = {
		[NORMAL] = {
			.corner = {34, 32, 33, 35, 68, 70, 71, 69},
			.edge = {9, 8, 11, 10, 19, 16, 17, 18, 22, 21, 20, 23}
		},
		[INVERSE] = {
			.corner = {65, 66, 64, 67, 36, 39, 37, 38},
			.edge = {21, 22, 23, 20, 26, 25, 24, 27, 1, 0, 3, 2}
		}
	},
	[BUr] = {
		[NORMAL] = {
			.corner = {69, 71, 68, 70, 33, 35, 32, 34},
			.edge = {17, 18, 19, 16, 11, 10, 9, 8, 4, 5, 6, 7}
		},
		[INVERSE] = {
			.corner = {70, 68, 71, 69, 34, 32, 35, 33},
			.edge = {19, 16, 17, 18, 8, 9, 10, 11, 7, 6, 5, 4}
		}
	},
	[BRr] = {
		[NORMAL] = {
			.corner = {35, 33, 32, 34, 69, 71, 70, 68},
			.edge = {11, 10, 9, 8, 18, 17, 16, 19, 23, 20, 21, 22}
		},
		[INVERSE] = {
			.corner = {66, 65, 67, 64, 39, 36, 38, 37},
			.edge = {22, 21, 20, 23, 25, 26, 27, 24, 3, 2, 1, 0}
		}
	},
	[BDr] = {
		[NORMAL] = {
			.corner = {71, 69, 70, 68, 35, 33, 34, 32},
			.edge = {18, 17, 16, 19, 10, 11, 8, 9, 6, 7, 4, 5}
		},
		[INVERSE] = {
			.corner = {71, 69, 70, 68, 35, 33, 34, 32},
			.edge = {18, 17, 16, 19, 10, 11, 8, 9, 6, 7, 4, 5}
		}
	},
	[BLr] = {
		[NORMAL] = {
			.corner = {33, 35, 34, 32, 71, 69, 68, 70},
			.edge = {10, 11, 8, 9, 17, 18, 19, 16, 21, 22, 23, 20}
		},
		[INVERSE] = {
			.corner = {67, 64, 66, 65, 38, 37, 39, 36},
			.edge = {23, 20, 21, 22, 27, 24, 25, 26, 2, 3, 0, 1}
		}
	},
	[UFm] = {
		[NORMAL] = {
			.corner = {4, 5, 6, 7, 0, 1, 2, 3},
			.edge = {0, 1, 2, 3, 5, 4, 7, 6, 9, 8, 11, 10}
		},
		[INVERSE] = {
			.corner = {4, 5, 6, 7, 0, 1, 2, 3},
			.edge = {0, 1, 2, 3, 5, 4, 7, 6, 9, 8, 11, 10}
		}
	},
	[ULm] = {
		[NORMAL] = {
			.corner = {0, 1, 3, 2, 5, 4, 6, 7},
			.edge = {4, 5, 6, 7, 0, 1, 2, 3, 24, 27, 26, 25}
		},
		[INVERSE] = {
			.corner = {0, 1, 3, 2, 5, 4, 6, 7},
			.edge = {4, 5, 6, 7, 0, 1, 2, 3, 24, 27, 26, 25}
		}
	},
	[UBm] = {
		[NORMAL] = {
			.corner = {5, 4, 7, 6, 1, 0, 3, 2},
			.edge = {1, 0, 3, 2, 4, 5, 6, 7, 11, 10, 9, 8}
		},
		[INVERSE] = {
			.corner = {5, 4, 7, 6, 1, 0, 3, 2},
			.edge = {1, 0, 3, 2, 4, 5, 6, 7, 11, 10, 9, 8}
		}
	},
	[URm] = {
		[NORMAL] = {
			.corner = {1, 0, 2, 3, 4, 5, 7, 6},
			.edge = {5, 4, 7, 6, 1, 0, 3, 2, 26, 25, 24, 27}
		},
		[INVERSE] = {
			.corner = {1, 0, 2, 3, 4, 5, 7, 6},
			.edge = {5, 4, 7, 6, 1, 0, 3, 2, 26, 25, 24, 27}
		}
	},
	[DFm] = {
		[NORMAL] = {
			.corner = {6, 7, 4, 5, 2, 3, 0, 1},
			.edge = {3, 2, 1, 0, 7, 6, 5, 4, 8, 9, 10, 11}
		},
		[INVERSE] = {
			.corner = {6, 7, 4, 5, 2, 3, 0, 1},
			.edge = {3, 2, 1, 0, 7, 6, 5, 4, 8, 9, 10, 11}
		}
	},
	[DLm] = {
		[NORMAL] = {
			.corner = {3, 2, 0, 1, 6, 7, 5, 4},
			.edge = {7, 6, 5, 4, 2, 3, 0, 1, 27, 24, 25, 26}
		},
		[INVERSE] = {
			.corner = {2, 3, 1, 0, 7, 6, 4, 5},
			.edge = {6, 7, 4, 5, 3, 2, 1, 0, 25, 26, 27, 24}
		}
	},
	[DBm] = {
		[NORMAL] = {
			.corner = {7, 6, 5, 4, 3, 2, 1, 0},
			.edge = {2, 3, 0, 1, 6, 7, 4, 5, 10, 11, 8, 9}
		},
		[INVERSE] = {
			.corner = {7, 6, 5, 4, 3, 2, 1, 0},
			.edge = {2, 3, 0, 1, 6, 7, 4, 5, 10, 11, 8, 9}
		}
	},
	[DRm] = {
		[NORMAL] = {
			.corner = {2, 3, 1, 0, 7, 6, 4, 5},
			.edge = {6, 7, 4, 5, 3, 2, 1, 0, 25, 26, 27, 24}
		},
		[INVERSE] = {
			.corner = {3, 2, 0, 1, 6, 7, 5, 4},
			.edge = {7, 6, 5, 4, 2, 3, 0, 1, 27, 24, 25, 26}
		}
	},
	[RUm] = {
		[NORMAL] = {
			.corner = {68, 71, 69, 70, 33, 34, 32, 35},
			.edge = {21, 22, 23, 20, 25, 26, 27, 24, 0, 1, 2, 3}
		},
		[INVERSE] = {
			.corner = {70, 68, 69, 71, 32, 34, 35, 33},
			.edge = {8, 9, 10, 11, 19, 16, 17, 18, 23, 20, 21, 22}
		}
	},
	[RFm] = {
		[NORMAL] = {
			.corner = {34, 33, 32, 35, 68, 71, 70, 69},
			.edge = {25, 26, 27, 24, 22, 21, 20, 23, 19, 16, 17, 18}
		},
		[INVERSE] = {
			.corner = {66, 65, 64, 67, 36, 39, 38, 37},
			.edge = {25, 26, 27, 24, 22, 21, 20, 23, 19, 16, 17, 18}
		}
	},
	[RDm] = {
		[NORMAL] = {
			.corner = {71, 68, 70, 69, 34, 33, 35, 32},
			.edge = {22, 21, 20, 23, 26, 25, 24, 27, 2, 3, 0, 1}
		},
		[INVERSE] = {
			.corner = {71, 69, 68, 70, 33, 35, 34, 32},
			.edge = {10, 11, 8, 9, 18, 17, 16, 19, 22, 21, 20, 23}
		}
	},
	[RBm] = {
		[NORMAL] = {
			.corner = {33, 34, 35, 32, 71, 68, 69, 70},
			.edge = {26, 25, 24, 27, 21, 22, 23, 20, 17, 18, 19, 16}
		},
		[INVERSE] = {
			.corner = {67, 64, 65, 66, 37, 38, 39, 36},
			.edge = {27, 24, 25, 26, 23, 20, 21, 22, 18, 17, 16, 19}
		}
	},
	[LUm] = {
		[NORMAL] = {
			.corner = {69, 70, 68, 71, 32, 35, 33, 34},
			.edge = {20, 23, 22, 21, 27, 24, 25, 26, 1, 0, 3, 2}
		},
		[INVERSE] = {
			.corner = {68, 70, 71, 69, 34, 32, 33, 35},
			.edge = {9, 8, 11, 10, 16, 19, 18, 17, 21, 22, 23, 20}
		}
	},
	[LFm] = {
		[NORMAL] = {
			.corner = {32, 35, 34, 33, 70, 69, 68, 71},
			.edge = {24, 27, 26, 25, 20, 23, 22, 21, 16, 19, 18, 17}
		},
		[INVERSE] = {
			.corner = {64, 67, 66, 65, 38, 37, 36, 39},
			.edge = {24, 27, 26, 25, 20, 23, 22, 21, 16, 19, 18, 17}
		}
	},
	[LDm] = {
		[NORMAL] = {
			.corner = {70, 69, 71, 68, 35, 32, 34, 33},
			.edge = {23, 20, 21, 22, 24, 27, 26, 25, 3, 2, 1, 0}
		},
		[INVERSE] = {
			.corner = {69, 71, 70, 68, 35, 33, 32, 34},
			.edge = {11, 10, 9, 8, 17, 18, 19, 16, 20, 23, 22, 21}
		}
	},
	[LBm] = {
		[NORMAL] = {
			.corner = {35, 32, 33, 34, 69, 70, 71, 68},
			.edge = {27, 24, 25, 26, 23, 20, 21, 22, 18, 17, 16, 19}
		},
		[INVERSE] = {
			.corner = {65, 66, 67, 64, 39, 36, 37, 38},
			.edge = {26, 25, 24, 27, 21, 22, 23, 20, 17, 18, 19, 16}
		}
	},
	[FUm] = {
		[NORMAL] = {
			.corner = {64, 66, 65, 67, 36, 38, 37, 39},
			.edge = {16, 19, 18, 17, 8, 9, 10, 11, 4, 5, 6, 7}
		},
		[INVERSE] = {
			.corner = {32, 34, 33, 35, 68, 70, 69, 71},
			.edge = {16, 19, 18, 17, 8, 9, 10, 11, 4, 5, 6, 7}
		}
	},
	[FRm] = {
		[NORMAL] = {
			.corner = {36, 38, 39, 37, 66, 64, 65, 67},
			.edge = {9, 8, 11, 10, 16, 19, 18, 17, 21, 22, 23, 20}
		},
		[INVERSE] = {
			.corner = {37, 38, 36, 39, 64, 67, 65, 66},
			.edge = {20, 23, 22, 21, 27, 24, 25, 26, 1, 0, 3, 2}
		}
	},
	[FDm] = {
		[NORMAL] = {
			.corner = {66, 64, 67, 65, 38, 36, 39, 37},
			.edge = {19, 16, 17, 18, 9, 8, 11, 10, 6, 7, 4, 5}
		},
		[INVERSE] = {
			.corner = {33, 35, 32, 34, 69, 71, 68, 70},
			.edge = {17, 18, 19, 16, 10, 11, 8, 9, 5, 4, 7, 6}
		}
	},
	[FLm] = {
		[NORMAL] = {
			.corner = {38, 36, 37, 39, 64, 66, 67, 65},
			.edge = {8, 9, 10, 11, 19, 16, 17, 18, 23, 20, 21, 22}
		},
		[INVERSE] = {
			.corner = {36, 39, 37, 38, 65, 66, 64, 67},
			.edge = {21, 22, 23, 20, 25, 26, 27, 24, 0, 1, 2, 3}
		}
	},
	[BUm] = {
		[NORMAL] = {
			.corner = {65, 67, 64, 66, 37, 39, 36, 38},
			.edge = {17, 18, 19, 16, 10, 11, 8, 9, 5, 4, 7, 6}
		},
		[INVERSE] = {
			.corner = {34, 32, 35, 33, 70, 68, 71, 69},
			.edge = {19, 16, 17, 18, 9, 8, 11, 10, 6, 7, 4, 5}
		}
	},
	[BRm] = {
		[NORMAL] = {
			.corner = {39, 37, 36, 38, 65, 67, 66, 64},
			.edge = {10, 11, 8, 9, 18, 17, 16, 19, 22, 21, 20, 23}
		},
		[INVERSE] = {
			.corner = {39, 36, 38, 37, 66, 65, 67, 64},
			.edge = {22, 21, 20, 23, 26, 25, 24, 27, 2, 3, 0, 1}
		}
	},
	[BDm] = {
		[NORMAL] = {
			.corner = {67, 65, 66, 64, 39, 37, 38, 36},
			.edge = {18, 17, 16, 19, 11, 10, 9, 8, 7, 6, 5, 4}
		},
		[INVERSE] = {
			.corner = {35, 33, 34, 32, 71, 69, 70, 68},
			.edge = {18, 17, 16, 19, 11, 10, 9, 8, 7, 6, 5, 4}
		}
	},
	[BLm] = {
		[NORMAL] = {
			.corner = {37, 39, 38, 36, 67, 65, 64, 66},
			.edge = {11, 10, 9, 8, 17, 18, 19, 16, 20, 23, 22, 21}
		},
		[INVERSE] = {
			.corner = {38, 37, 39, 36, 67, 64, 66, 65},
			.edge = {23, 20, 21, 22, 24, 27, 26, 25, 3, 2, 1, 0}
		}
	},
};

_static char *cornerstr[] = {
	[_c_ufr] = "UFR",
	[_c_ubl] = "UBL",
	[_c_dfl] = "DFL",
	[_c_dbr] = "DBR",
	[_c_ufl] = "UFL",
	[_c_ubr] = "UBR",
	[_c_dfr] = "DFR",
	[_c_dbl] = "DBL"
};

_static char *cornerstralt[] = {
	[_c_ufr] = "URF",
	[_c_ubl] = "ULB",
	[_c_dfl] = "DLF",
	[_c_dbr] = "DRB",
	[_c_ufl] = "ULF",
	[_c_ubr] = "URB",
	[_c_dfr] = "DRF",
	[_c_dbl] = "DLB"
};

_static char *edgestr[] = {
	[_e_uf] = "UF",
	[_e_ub] = "UB",
	[_e_db] = "DB",
	[_e_df] = "DF",
	[_e_ur] = "UR",
	[_e_ul] = "UL",
	[_e_dl] = "DL",
	[_e_dr] = "DR",
	[_e_fr] = "FR",
	[_e_fl] = "FL",
	[_e_bl] = "BL",
	[_e_br] = "BR"
};

_static char *movestr[] = {
	[U]  = "U",
	[U2] = "U2",
	[U3] = "U'",
	[D]  = "D",
	[D2] = "D2",
	[D3] = "D'",
	[R]  = "R",
	[R2] = "R2",
	[R3] = "R'",
	[L]  = "L",
	[L2] = "L2",
	[L3] = "L'",
	[F]  = "F",
	[F2] = "F2",
	[F3] = "F'",
	[B]  = "B",
	[B2] = "B2",
	[B3] = "B'",
};

_static char *transstr[] = {
	[UFr] = "rotation UF",
	[UFm] = "mirrored UF",
	[ULr] = "rotation UL",
	[ULm] = "mirrored UL",
	[UBr] = "rotation UB",
	[UBm] = "mirrored UB",
	[URr] = "rotation UR",
	[URm] = "mirrored UR",
	[DFr] = "rotation DF",
	[DFm] = "mirrored DF",
	[DLr] = "rotation DL",
	[DLm] = "mirrored DL",
	[DBr] = "rotation DB",
	[DBm] = "mirrored DB",
	[DRr] = "rotation DR",
	[DRm] = "mirrored DR",
	[RUr] = "rotation RU",
	[RUm] = "mirrored RU",
	[RFr] = "rotation RF",
	[RFm] = "mirrored RF",
	[RDr] = "rotation RD",
	[RDm] = "mirrored RD",
	[RBr] = "rotation RB",
	[RBm] = "mirrored RB",
	[LUr] = "rotation LU",
	[LUm] = "mirrored LU",
	[LFr] = "rotation LF",
	[LFm] = "mirrored LF",
	[LDr] = "rotation LD",
	[LDm] = "mirrored LD",
	[LBr] = "rotation LB",
	[LBm] = "mirrored LB",
	[FUr] = "rotation FU",
	[FUm] = "mirrored FU",
	[FRr] = "rotation FR",
	[FRm] = "mirrored FR",
	[FDr] = "rotation FD",
	[FDm] = "mirrored FD",
	[FLr] = "rotation FL",
	[FLm] = "mirrored FL",
	[BUr] = "rotation BU",
	[BUm] = "mirrored BU",
	[BRr] = "rotation BR",
	[BRm] = "mirrored BR",
	[BDr] = "rotation BD",
	[BDm] = "mirrored BD",
	[BLr] = "rotation BL",
	[BLm] = "mirrored BL",
};

static uint8_t inverse_trans_table[48] = {
	[UFr] = UFr,
	[UFm] = UFm,
	[ULr] = URr,
	[ULm] = ULm,
	[UBr] = UBr,
	[UBm] = UBm,
	[URr] = ULr,
	[URm] = URm,
	[DFr] = DFr,
	[DFm] = DFm,
	[DLr] = DLr,
	[DLm] = DRm,
	[DBr] = DBr,
	[DBm] = DBm,
	[DRr] = DRr,
	[DRm] = DLm,
	[RUr] = FRr,
	[RUm] = FLm,
	[RFr] = LFr,
	[RFm] = RFm,
	[RDr] = BLr,
	[RDm] = BRm,
	[RBr] = RBr,
	[RBm] = LBm,
	[LUr] = FLr,
	[LUm] = FRm,
	[LFr] = RFr,
	[LFm] = LFm,
	[LDr] = BRr,
	[LDm] = BLm,
	[LBr] = LBr,
	[LBm] = RBm,
	[FUr] = FUr,
	[FUm] = FUm,
	[FRr] = RUr,
	[FRm] = LUm,
	[FDr] = BUr,
	[FDm] = BUm,
	[FLr] = LUr,
	[FLm] = RUm,
	[BUr] = FDr,
	[BUm] = FDm,
	[BRr] = LDr,
	[BRm] = RDm,
	[BDr] = BDr,
	[BDm] = BDm,
	[BLr] = RDr,
	[BLm] = LDm,
};
