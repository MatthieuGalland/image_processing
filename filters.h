//
// Created by thoma on 25/05/2025.
//

#ifndef FILTERS_H
#define FILTERS_H


const float BOX_BLUR[3][3] = {
    {1.f/9, 1.f/9, 1.f/9},
    {1.f/9, 1.f/9, 1.f/9},
    {1.f/9, 1.f/9, 1.f/9}
};


const float GAUSSIAN_BLUR[3][3] = {
    {1.f/16*1, 1.f/16*2, 1.f/16*1},
    {1.f/16*2, 1.f/16*4, 1.f/16*2},
    {1.f/16*1, 1.f/16*2, 1.f/16*1}
};

const float GAUSSIAN_BLUR_SIMPLE[3][3] = {
    {1.f/16, 2.f/16, 1.f/16},
    {2.f/16, 4.f/16, 2.f/16},
    {1.f/16, 2.f/16, 1.f/16}
};


const float OUTLINE[3][3] = {
    {-1, -1, -1},
    {-1,  8, -1},
    {-1, -1, -1}
};


const float EMBOSS[3][3] = {
    {-2, -1,  0},
    {-1,  1,  1},
    { 0,  1,  2}
};

const float SHARPEN[3][3] = {
    { 0, -1,  0},
    {-1,  5, -1},
    { 0, -1,  0}
};

#endif //FILTERS_H
