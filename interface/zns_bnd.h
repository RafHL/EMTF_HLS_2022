// Zone boundary array: [type][station][zone][0-low ind|1-high ind]
constexpr int zns_bnd[5][5][3][2] = {
    { // Ty: 0
        { // St: 0
            {  0,   0}, // Ty: 0 St: 0 Zn: 0
            {  0,   0}, // Ty: 0 St: 0 Zn: 1
            {  0,   0}  // Ty: 0 St: 0 Zn: 2
        },
        { // St: 1
            {  0,   0}, // Ty: 0 St: 1 Zn: 0
            {  0,   0}, // Ty: 0 St: 1 Zn: 1
            {  0,   0}  // Ty: 0 St: 1 Zn: 2
        },
        { // St: 2
            {  0,   0}, // Ty: 0 St: 2 Zn: 0
            {  0,   0}, // Ty: 0 St: 2 Zn: 1
            {  0,   0}  // Ty: 0 St: 2 Zn: 2
        },
        { // St: 3
            {  0,   0}, // Ty: 0 St: 3 Zn: 0
            {  0,   0}, // Ty: 0 St: 3 Zn: 1
            {  0,   0}  // Ty: 0 St: 3 Zn: 2
        },
        { // St: 4
            {  0,   0}, // Ty: 0 St: 4 Zn: 0
            {  0,   0}, // Ty: 0 St: 4 Zn: 1
            {  0,   0}  // Ty: 0 St: 4 Zn: 2
        }
    },
    { // Ty: 1
        { // St: 0
            {  4,  26}, // Ty: 1 St: 0 Zn: 0
            { 24,  54}, // Ty: 1 St: 0 Zn: 1
            { 52,  88}  // Ty: 1 St: 0 Zn: 2
        },
        { // St: 1
            {  4,  26}, // Ty: 1 St: 1 Zn: 0
            { 24,  54}, // Ty: 1 St: 1 Zn: 1
            { 52,  88}  // Ty: 1 St: 1 Zn: 2
        },
        { // St: 2
            {  4,  25}, // Ty: 1 St: 2 Zn: 0
            { 23,  49}, // Ty: 1 St: 2 Zn: 1
            { 52,  88}  // Ty: 1 St: 2 Zn: 2
        },
        { // St: 3
            {  4,  25}, // Ty: 1 St: 3 Zn: 0
            { 23,  54}, // Ty: 1 St: 3 Zn: 1
            { 50,  88}  // Ty: 1 St: 3 Zn: 2
        },
        { // St: 4
            {  4,  25}, // Ty: 1 St: 4 Zn: 0
            { 23,  54}, // Ty: 1 St: 4 Zn: 1
            { 50,  88}  // Ty: 1 St: 4 Zn: 2
        }
    },
    { // Ty: 2
        { // St: 0
            {  0,   0}, // Ty: 2 St: 0 Zn: 0
            { 52,  56}, // Ty: 2 St: 0 Zn: 1
            { 52,  84}  // Ty: 2 St: 0 Zn: 2
        },
        { // St: 1
            {  0,   0}, // Ty: 2 St: 1 Zn: 0
            { 52,  56}, // Ty: 2 St: 1 Zn: 1
            { 52,  84}  // Ty: 2 St: 1 Zn: 2
        },
        { // St: 2
            {  0,   0}, // Ty: 2 St: 2 Zn: 0
            {  0,   0}, // Ty: 2 St: 2 Zn: 1
            { 56,  88}  // Ty: 2 St: 2 Zn: 2
        },
        { // St: 3
            {  4,  25}, // Ty: 2 St: 3 Zn: 0
            { 23,  52}, // Ty: 2 St: 3 Zn: 1
            { 48,  84}  // Ty: 2 St: 3 Zn: 2
        },
        { // St: 4
            {  4,  25}, // Ty: 2 St: 4 Zn: 0
            { 23,  52}, // Ty: 2 St: 4 Zn: 1
            { 52,  84}  // Ty: 2 St: 4 Zn: 2
        }
    },
    { // Ty: 3
        { // St: 0
            { 17,  26}, // Ty: 3 St: 0 Zn: 0
            { 24,  52}, // Ty: 3 St: 0 Zn: 1
            {  0,   0}  // Ty: 3 St: 0 Zn: 2
        },
        { // St: 1
            { 17,  26}, // Ty: 3 St: 1 Zn: 0
            { 24,  52}, // Ty: 3 St: 1 Zn: 1
            {  0,   0}  // Ty: 3 St: 1 Zn: 2
        },
        { // St: 2
            {  7,  25}, // Ty: 3 St: 2 Zn: 0
            { 23,  46}, // Ty: 3 St: 2 Zn: 1
            {  0,   0}  // Ty: 3 St: 2 Zn: 2
        },
        { // St: 3
            {  0,   0}, // Ty: 3 St: 3 Zn: 0
            {  0,   0}, // Ty: 3 St: 3 Zn: 1
            {  0,   0}  // Ty: 3 St: 3 Zn: 2
        },
        { // St: 4
            {  0,   0}, // Ty: 3 St: 4 Zn: 0
            {  0,   0}, // Ty: 3 St: 4 Zn: 1
            {  0,   0}  // Ty: 3 St: 4 Zn: 2
        }
    },
    { // Ty: 4
        { // St: 0
            {  4,  23}, // Ty: 4 St: 0 Zn: 0
            {  0,   0}, // Ty: 4 St: 0 Zn: 1
            {  0,   0}  // Ty: 4 St: 0 Zn: 2
        },
        { // St: 1
            {  4,  23}, // Ty: 4 St: 1 Zn: 0
            {  0,   0}, // Ty: 4 St: 1 Zn: 1
            {  0,   0}  // Ty: 4 St: 1 Zn: 2
        },
        { // St: 2
            {  0,   0}, // Ty: 4 St: 2 Zn: 0
            {  0,   0}, // Ty: 4 St: 2 Zn: 1
            {  0,   0}  // Ty: 4 St: 2 Zn: 2
        },
        { // St: 3
            {  0,   0}, // Ty: 4 St: 3 Zn: 0
            {  0,   0}, // Ty: 4 St: 3 Zn: 1
            {  0,   0}  // Ty: 4 St: 3 Zn: 2
        },
        { // St: 4
            {  0,   0}, // Ty: 4 St: 4 Zn: 0
            {  0,   0}, // Ty: 4 St: 4 Zn: 1
            {  0,   0}  // Ty: 4 St: 4 Zn: 2
        }
    }
};
