
#ifndef MAIN
#define MAIN

#include <iostream>
#include <vector>

struct City
{
    float x, y;

    City(float initialX, float initialY) :
        x(initialX), y(initialY)
    {}
};

std::vector<City> getCities();
float getDistance(const City& a, const City& b);

#endif
