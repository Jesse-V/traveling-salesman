
#define _GLIBCXX_USE_NANOSLEEP

#include "main.hpp"
#include <algorithm>
#include <thread>
#include <fstream>
#include <sstream>
#include <chrono>
#include <cfloat>

Distances distances_;

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

    static std::vector<std::pair<float, float>> minCosts;
    if (minCosts.empty())
    {
        for (std::size_t j = 0; j < maxD; j++)
        {
            float minA = FLT_MAX, minB = FLT_MAX;
            for (std::size_t k = 0; k < maxD; k++)
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
            branchAndBound(stack, depth + 1, maxD, bestSoFar, visited);
            stack.pop_back();
            visited[j] = false;
        }
    }
}



std::pair<long, float> doExhaustive(std::size_t depth)
{ //almost identical code to doBranchAndBound

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
{ //almost identical code to doExhaustive

    using namespace std::chrono;

    float best = FLT_MAX;
    std::vector<int> stack;
    std::vector<bool> visited(depth, false);

    auto start = steady_clock::now();
    branchAndBound(stack, 0, depth, best, visited);
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
}



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
