
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

std::vector<int> simulatedAnnealing(std::vector<int> tour, long maxSeconds,
                                    float temperature, long seed, int op);
std::vector<int> getRandomTour(std::size_t size, long seed);
float getCost(const std::vector<int>& tour);
void print(const std::vector<int>& tour);

/*
void exhaustive(std::vector<int>& stack, std::size_t depth, std::size_t maxD,
                float& bestSoFar, std::vector<bool>& visited);
void branchAndBound(std::vector<int>& stack, std::size_t depth, std::size_t maxD,
                    float& bestSoFar, std::vector<bool>& visited,
                    const std::vector<std::pair<float, float>>& minCosts);

std::pair<long, float> doExhaustive(std::size_t depth);
std::pair<long, float> doBranchAndBound(std::size_t depth);

std::pair<long, float> test(void (*func)(std::vector<int>& stack,
                        std::size_t depth, std::size_t maxD, float& bestSoFar,
                        std::vector<bool>& visited), std::size_t depth);
void test(std::pair<int, int> a, std::pair<int, int> b, std::pair<int, int> c,
          std::pair<int, int> d, std::pair<int, int> e,
          std::pair<long, float> (*func)(std::size_t depth));
*/
std::vector<City> getCities();
Distances getDistances(const std::vector<City>& cities);
float getDistance(const City& a, const City& b);

#endif
