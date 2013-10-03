
#ifndef MAIN
#define MAIN

#include <iostream>
#include <vector>

typedef std::vector<std::vector<float>> Distances;

struct City
{
    float x, y;

    City(float initialX, float initialY) :
        x(initialX), y(initialY)
    {}
};

std::vector<City> getCities();
Distances getDistances(const std::vector<City>& cities);
float getDistance(const City& a, const City& b);

#endif
