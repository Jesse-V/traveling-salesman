
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

    //test(std::make_pair(1, 11), std::make_pair(2, 1), std::make_pair(2, 1),
    //    std::make_pair(2, 1), std::make_pair(2, 1), doExhaustive);

    //test(std::make_pair(1, 11), std::make_pair(2, 1), std::make_pair(2, 1),
    //    std::make_pair(2, 1), std::make_pair(2, 1), doBranchAndBound);

    auto result1 = test(exhaustive, 5);
    std::cout << result1.first << " " << result1.second << std::endl;

    auto result2 = test(branchAndBound, 5);
    std::cout << result2.first << " " << result2.second << std::endl;

/*
    int j = 3;
    float minA = FLT_MAX, minB = FLT_MAX;
    for (int k = 0; k < 10; k++)
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
    std::cout << minA << " " << minB;
*/
}



void exhaustive(std::vector<int>& visited, std::size_t depth,
                float& bestSoFar, std::size_t maxCities
)
{
    if (depth == maxCities)
    {
        //std::cout << "Tour { ";

        float cost = 0;
        std::size_t j;
        for (j = 0; j < visited.size() - 1; j++)
        {
            //std::cout << visited[j] << " ";
            cost += distances_[visited[j]][visited[j + 1]];
        }
        cost += distances_[visited[j]][visited[0]];

        //std::cout << visited[j] << " " << visited[0] <<
        //    " } cost of " << cost << std::endl;

        if (cost < bestSoFar)
            bestSoFar = cost;

        return;
    }

    for (int j = 0; j < maxCities; j++)
    {
        if (std::find(visited.begin(), visited.end(), j) == visited.end())
        {
            visited.push_back(j);
            exhaustive(visited, depth + 1, bestSoFar, maxCities);
            visited.pop_back();
        }
    }
}



void branchAndBound(std::vector<int>& visited, std::size_t depth,
                    float& bestSoFar, std::size_t maxCities
)
{
    if (depth == maxCities)
    {
        //std::cout << "Tour { ";

        float cost = 0;
        std::size_t j;
        for (j = 0; j < visited.size() - 1; j++)
        {
            //std::cout << visited[j] << " ";
            cost += distances_[visited[j]][visited[j + 1]];
        }
        cost += distances_[visited[j]][visited[0]];

        //std::cout << visited[j] << " " << visited[0] <<
        //    " } cost of " << cost << std::endl;

        if (cost < bestSoFar)
            bestSoFar = cost;

        return;
    }

    static std::vector<std::pair<float, float>> minCosts;
    if (minCosts.empty())
    {
        for (int j = 0; j < maxCities; j++)
        {
            float minA = FLT_MAX, minB = FLT_MAX;
            for (int k = 0; k < maxCities; k++)
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

    for (int j = 0; j < maxCities; j++)
    {
        if (std::find(visited.begin(), visited.end(), j) == visited.end())
        {
            float lowerBound = 0;
            for (int k = 0; k < maxCities; k++)
                if (j == k ||
                    std::find(visited.begin(), visited.end(), k) == visited.end()
                )
                    lowerBound += minCosts[k].first + minCosts[k].second;

            std::cout << bestSoFar << " " << (0.5f * lowerBound) << std::endl;

            if (bestSoFar > 0.5f * lowerBound)
            {
                visited.push_back(j);
                exhaustive(visited, depth + 1, bestSoFar, maxCities);
                visited.pop_back();
            }
        }
    }
}



std::pair<long, float> test(void (*func)(std::vector<int>& visited, std::size_t depth,
                            float& bestSoFar, std::size_t maxCities), int depth)
{
    using namespace std::chrono;

    float best = FLT_MAX;
    std::vector<int> stack;

    auto start = steady_clock::now();
    (*func)(stack, 0, best, depth);
    long us = duration_cast<microseconds>(steady_clock::now() - start).count();
    return std::make_pair(us, best);
}



void test(std::pair<int, int> a, std::pair<int, int> b, std::pair<int, int> c,
          std::pair<int, int> d, std::pair<int, int> e,
          std::pair<long, float> (*func)(int depth))
{
    std::thread thread1([&]() {
        for (int j = a.first; j <= a.second; j++)
        {
            auto data = (*func)(j);
            std::cout << j << ": " << data.first << " " << data.second << std::endl;
        }

    });

    std::thread thread2([&]() {
        for (int j = b.first; j <= b.second; j++)
        {
            auto data = (*func)(j);
            std::cout << j << ": " << data.first << " " << data.second << std::endl;
        }
    });

    std::thread thread3([&]() {
        for (int j = c.first; j <= c.second; j++)
        {
            auto data = (*func)(j);
            std::cout << j << ": " << data.first << " " << data.second << std::endl;
        }
    });

    std::thread thread4([&]() {
        for (int j = d.first; j <= d.second; j++)
        {
            auto data = (*func)(j);
            std::cout << j << ": " << data.first << " " << data.second << std::endl;
        }
    });

    std::thread thread5([&]() {
        for (int j = e.first; j <= e.second; j++)
        {
            auto data = (*func)(j);
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
