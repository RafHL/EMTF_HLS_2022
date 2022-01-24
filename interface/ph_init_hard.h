    // taken from emtf-geometry-gen.cc, must match
    // start phi of each chamber in reduced precision, for zone hits, 
    // with negative offset to allow for possible chamber movement
    const int ph_init_hard[5][16] = {
        {39,  57,  76, 39,  58,  76, 41,  60,  79, 39,  57,  76, 21, 21, 23, 21},
        {95, 114, 132, 95, 114, 133, 98, 116, 135, 95, 114, 132, 0, 0, 0, 0},
        {38,  76, 113, 39,  58,  76, 95, 114, 132,  1,  21,   0, 0, 0, 0, 0},
        {38,  76, 113, 39,  58,  76, 95, 114, 132,  1,  21,   0, 0, 0, 0, 0},
        {38,  76, 113, 38,  57,  76, 95, 113, 132,  1,  20,   0, 0, 0, 0, 0}
    };

