
#define _GLIBCXX_USE_NANOSLEEP

#include "main.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cfloat>

Distances distances_;

int main(int argc, char** argv)
{
    distances_ = getDistances(getCities());

    float best = FLT_MAX;
    std::vector<int> stack;
    exhaustive(stack, 0, best, 3);
    std::cout << best << std::endl;
}



void exhaustive(std::vector<int>& visited, std::size_t depth,
                float& bestSoFar, std::size_t maxCities
)
{
    if (depth == maxCities)
    {
        std::cout << "Tour { ";

        float cost = 0;
        std::size_t j;
        for (j = 0; j < visited.size() - 1; j++)
        {
            std::cout << visited[j] << " ";
            cost += distances_[visited[j]][visited[j + 1]];
        }
        cost += distances_[visited[j]][visited[0]];

        std::cout << visited[j] << " " << visited[0] <<
            " } cost of " << cost << std::endl;
        //std::cout << distances_[visited[j]][visited[0]] << std::endl;

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
