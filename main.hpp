
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

void test(std::pair<int, int> a, std::pair<int, int> b, std::pair<int, int> c,
          std::pair<int, int> d, std::pair<int, int> e,
          std::pair<long, float> (*func)(int depth));

std::pair<long, float> doExhaustive(int depth);
void exhaustive(std::vector<int>& visited, std::size_t depth,
                float& bestSoFar, std::size_t maxCities);

std::vector<City> getCities();
Distances getDistances(const std::vector<City>& cities);
float getDistance(const City& a, const City& b);

#endif
