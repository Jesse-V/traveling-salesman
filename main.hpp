
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

void exhaustive(std::vector<int>& visited, std::size_t depth,
                float& bestSoFar, std::size_t maxCities);

std::vector<City> getCities();
Distances getDistances(const std::vector<City>& cities);
float getDistance(const City& a, const City& b);

#endif
