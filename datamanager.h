#ifndef DATAMANAGER_H
#define DATAMANAGER_H
//Auteur : Raphael De Oliveira
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
#include <filesystem>
#include <filesystem>
#include <chrono>
#include <thread>
#include "histogram.h"

using namespace std;
using json = nlohmann::json;
namespace fs = std::filesystem;

struct HistoricalData {
    int dispo;
    int max;
    string timestamp;
};
class DataManager {
private:
    const string JSON_URL = "https://opendata.lillemetropole.fr//explore/dataset/disponibilite-parkings/download?format=json&timezone=Europe/Berlin&use_labels_for_header=false";
    const string LOCAL_JSON_FILENAME = "disponibilite_parkings.json";
    const char* dirName = "Images_PNG";
    const char* dataDirName = "Data_parking";
    const int MAX_HISTORY_SIZE;
    struct stat info; //pour la création du répertoire Image_PNG
    map<string, vector<HistoricalData>> historique_disponibilites;
    std::vector<std::string> timestamps;


    // Le reste de vos fonctions privées (ecrire_data, download_json, split_into_lines, etc.)
    void download_json();
    void update_data_and_store_history();
    void load_history(); // Charge les données historiques depuis les fichiers JSON
    void initializeHistoricalData();
public:
    DataManager();

    void collectData();
    void sauvegarder_data_json(const string& nom_parking, int dispo, int max, const string& timestamp);
    void saveParkingData(const string& parkingName, int availability, int maxCapacity, const string& timestamp);
    string generateTimestamp();
    vector<string> getNoms() const;
    vector<int> getDispo() const;
    vector<int> getMax() const;
    vector<string> getTimestamps(const string& parkingName) const;
    vector<int> getAvailabilityHistory(const string& parkingName) const;
    const std::vector<std::string>& getTimestamps() const {
        return timestamps;
    }

    void initializeAndCollectData() {
        initializeHistoricalData();
        update_data_and_store_history();
    }
};

#endif // DATAMANAGER_H
