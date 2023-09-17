#ifndef HISTOGRAM_H
#define HISTOGRAM_H
#include <iostream>
#include <fstream>
#include <string>
#include <gd.h>
#include <nlohmann/json.hpp>
#include <vector>
#include <cmath>
#include <map>
#include <gdfonts.h>
#include <curl/curl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctime>
#include <iomanip>
//Auteur : Raphael De Oliveira
using namespace std;
using json = nlohmann::json;

class Histogram {
public:
    void createHistogram(const std::string& filename, const std::vector<std::string>& noms, const std::vector<int>& dispo, const std::vector<int>& max);
    static void createEvolutionHistogram(const std::string& filename, const std::vector<std::string>& timestamps, const std::vector<int>& data);

};

#endif // HISTOGRAM_H
