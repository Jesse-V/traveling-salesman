
#define _GLIBCXX_USE_NANOSLEEP

#include "main.hpp"
#include <fstream>
#include <sstream>


int main(int argc, char** argv)
{
    std::vector<City> cities = getCities();

}



std::vector<City> getCities()
{
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

    return cities;
}



float getDistance(const City& a, const City& b)
{
    float xDiff = b.x - a.x, yDiff = b.y - a.y;
    return xDiff * xDiff + yDiff * yDiff;
}
