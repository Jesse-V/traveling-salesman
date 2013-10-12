
#define _GLIBCXX_USE_NANOSLEEP

#include "main.hpp"
#include <algorithm>
#include <thread>
#include <fstream>
#include <sstream>
#include <chrono>
#include <cfloat>

/*  simulated annealing
        hill climbing with heat
    hill climbing
        first random op that reduces tour length
        like simulated annealing but with temp of 0

    ops
        need three
            I choose:
                "swap 2"
                "reverse section"
                "scramble section"
        measure:
            solution quality
            speed of convergence

    test via experiments:
        performance as number of cities increases?
        given fixed amount of time on single CPU, most effective application?
            tradeoff between running longer or with another restart
            demonstrate on provided 256 city problem
        effect of initial temp and cooling strategy on the SA
        which op performs best and why

    brief report, results, answers to above, code, snapshot of running prog
*/

int main(int argc, char** argv)
{
    const int SIZE = 14;
    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    auto tour = getRandomTour(SIZE, seed);

    std::vector<int> bestSoFar(SIZE);
    float cheapestCostSoFar = FLT_MAX;
    std::mt19937 mersenneTwister;
    std::uniform_int_distribution<int> randomIndex(0, SIZE - 1);
    while (true)
    {
        float cost = getCost(tour);
        if (cost < cheapestCostSoFar)
        {
            cheapestCostSoFar = cost;
            std::copy(tour.begin(), tour.end(), bestSoFar.begin());
            print(tour);
            std::cout << tour[0] << ", cost of " << cost << std::endl;
        }

        std::size_t a = 0, b = 0;
        while (a == b)
        {
            a = (std::size_t)randomIndex(mersenneTwister);
            b = (std::size_t)randomIndex(mersenneTwister);
        }

        //super-fast in-place swap
        tour[a] ^= tour[b];
        tour[b] ^= tour[a];
        tour[a] ^= tour[b];
    }
}



std::vector<int> getRandomTour(std::size_t size, long seed)
{
    std::vector<int> tour(size);
    for (std::size_t j = 0; j < size; j++)
        tour[j] = (int)j;
    std::shuffle(tour.begin(), tour.end(), std::default_random_engine(seed));
    return tour;
}



float getCost(const std::vector<int>& tour)
{
    static Distances distances = getDistances(getCities());

    float cost = 0;
    std::size_t j;
    for (j = 0; j < tour.size() - 1; j++)
        cost += distances[(std::size_t)tour[j]][(std::size_t)tour[j + 1]];
    cost += distances[(std::size_t)tour[j]][(std::size_t)tour[0]];
    return cost;
}


void print(const std::vector<int>& tour)
{
    for (std::size_t j = 0; j < tour.size(); j++)
        std::cout << tour[j] << " ";
}


/******************************** OLDER SOLUTIONS *********************/

/*
int main(int argc, char** argv)
{
    distances_ = getDistances(getCities());

    test(std::make_pair(1, 12), std::make_pair(13, 13), std::make_pair(14, 14),
         std::make_pair(15, 15), std::make_pair(16, 16), doExhaustive);

    test(std::make_pair(1, 12), std::make_pair(13, 13), std::make_pair(14, 14),
         std::make_pair(15, 15), std::make_pair(16, 16), doBranchAndBound);
}


void exhaustive(std::vector<int>& stack, std::size_t depth, std::size_t maxD,
                float& bestSoFar, std::vector<bool>& visited
)
{
    if (depth == maxD)
    {
        float cost = 0;
        std::size_t j;
        for (j = 0; j < stack.size() - 1; j++)
            cost += distances_[stack[j]][stack[j + 1]];
        cost += distances_[stack[j]][stack[0]];

        if (cost < bestSoFar)
            bestSoFar = cost;

        return;
    }

    for (std::size_t j = 0; j < maxD; j++)
    {
        if (!visited[j])
        {
            visited[j] = true;
            stack.push_back((int)j);
            exhaustive(stack, depth + 1, maxD, bestSoFar, visited);
            stack.pop_back();
            visited[j] = false;
        }
    }
}



void branchAndBound(std::vector<int>& stack, std::size_t depth, std::size_t maxD,
                    float& bestSoFar, std::vector<bool>& visited,
                    const std::vector<std::pair<float, float>>& minCosts
)
{
    if (depth == maxD)
    {
        float cost = 0;
        std::size_t j;
        for (j = 0; j < stack.size() - 1; j++)
            cost += distances_[stack[j]][stack[j + 1]];
        cost += distances_[stack[j]][stack[0]];

        if (cost < bestSoFar)
            bestSoFar = cost;
        return;
    }

    float lowerBound = 0;
    for (std::size_t k = 0; k < maxD; k++)
        if (!visited[k])
            lowerBound += minCosts[k].first + minCosts[k].second;
    if (bestSoFar < 0.5f * lowerBound)
        return;

    for (std::size_t j = 0; j < maxD; j++)
    {
        if (!visited[j])
        {
            visited[j] = true;
            stack.push_back((int)j);
            branchAndBound(stack, depth + 1, maxD, bestSoFar, visited, minCosts);
            stack.pop_back();
            visited[j] = false;
        }
    }
}



std::pair<long, float> doExhaustive(std::size_t depth)
{
    using namespace std::chrono;

    float best = FLT_MAX;
    std::vector<int> stack;
    std::vector<bool> visited(depth, false);

    auto start = steady_clock::now();
    exhaustive(stack, 0, depth, best, visited);
    long us = duration_cast<microseconds>(steady_clock::now() - start).count();
    return std::make_pair(us, best);
}



std::pair<long, float> doBranchAndBound(std::size_t depth)
{
    using namespace std::chrono;

    float best = FLT_MAX;
    std::vector<int> stack;
    std::vector<bool> visited(depth, false);

    std::vector<std::pair<float, float>> minCosts;
    for (std::size_t j = 0; j < depth; j++)
    {
        float minA = FLT_MAX, minB = FLT_MAX;
        for (std::size_t k = 0; k < depth; k++)
        {
            float dist = distances_[j][k];
            if (dist < minB)
            {
                if (dist < minA)
                {
                    minB = minA;
                    minA = dist;
                }
                else
                    minB = dist;
            }
        }
        minCosts.push_back(std::make_pair(minA, minB));
    }

    auto start = steady_clock::now();
    branchAndBound(stack, 0, depth, best, visited, minCosts);
    long us = duration_cast<microseconds>(steady_clock::now() - start).count();
    return std::make_pair(us, best);
}



void test(std::pair<int, int> a, std::pair<int, int> b, std::pair<int, int> c,
          std::pair<int, int> d, std::pair<int, int> e,
          std::pair<long, float> (*func)(std::size_t depth))
{
    std::thread thread1([&]() {
        for (int j = a.first; j <= a.second; j++)
        {
            auto data = (*func)((std::size_t)j);
            std::cout << j << ": " << data.first << " " << data.second << std::endl;
        }

    });

    std::thread thread2([&]() {
        for (int j = b.first; j <= b.second; j++)
        {
            auto data = (*func)((std::size_t)j);
            std::cout << j << ": " << data.first << " " << data.second << std::endl;
        }
    });

    std::thread thread3([&]() {
        for (int j = c.first; j <= c.second; j++)
        {
            auto data = (*func)((std::size_t)j);
            std::cout << j << ": " << data.first << " " << data.second << std::endl;
        }
    });

    std::thread thread4([&]() {
        for (int j = d.first; j <= d.second; j++)
        {
            auto data = (*func)((std::size_t)j);
            std::cout << j << ": " << data.first << " " << data.second << std::endl;
        }
    });

    std::thread thread5([&]() {
        for (int j = e.first; j <= e.second; j++)
        {
            auto data = (*func)((std::size_t)j);
            std::cout << j << ": " << data.first << " " << data.second << std::endl;
        }
    });

    thread1.join();
    thread2.join();
    thread3.join();
    thread4.join();
    thread5.join();
}*/



Distances getDistances(const std::vector<City>& cities)
{
    std::cout << "Calculating distances... ";

    Distances distances(cities.size());
    std::generate(distances.begin(), distances.end(),
        [&]() { return std::vector<float>(cities.size()); }
    );

    for (std::size_t j = 0; j < cities.size(); j++)
    {
        for (std::size_t k = 0; k < cities.size(); k++)
        {
            float distance = j != k ? getDistance(cities[j], cities[k]) : FLT_MAX;
            distances[j][k] = distance;
            distances[k][j] = distance;
        }
    }

    std::cout << "done." << std::endl;
    return distances;
}



std::vector<City> getCities()
{
    std::cout << "Loading cities from file... ";

    std::string data;
    std::ifstream fin("tsp.txt", std::ios::in);
    fin.seekg(0, std::ios::end);
    data.resize((unsigned long)fin.tellg()); //allocate enough space
    fin.seekg(0, std::ios::beg);
    fin.read(&data[0], (long)data.size()); //read entire file
    fin.close();

    std::istringstream stream(data);
    std::string line;
    std::vector<City> cities;
    bool loading = false;
    while (std::getline(stream, line))
    {
        if (line == "EOF")
            loading = false;

        if (loading)
        {
            float index, x, y;
            std::istringstream lineStream(line);
            lineStream >> index >> x >> y;
            cities.push_back(City(x, y));
        }

        if (line == "NODE_COORD_SECTION")
            loading = true;
    }

    std::cout << "done." << std::endl;
    return cities;
}



float getDistance(const City& a, const City& b)
{
    float xDiff = b.x - a.x, yDiff = b.y - a.y;
    return std::sqrt(xDiff * xDiff + yDiff * yDiff);
}
