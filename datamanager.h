#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <string>
#include <nlohmann/json.hpp>
#include <vector>
#include <map>
#include <curl/curl.h>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <filesystem>

using namespace std;
using json = nlohmann::json;
namespace fs = std::filesystem;

struct HistoricalData {
    int dispo;
    int max;
    std::string timestamp;


    HistoricalData(const nlohmann::json& jsonData) {
        // Extraire les valeurs de l'objet JSON et initialiser les membres de la structure
        dispo = jsonData["dispo"].get<int>();
        max = jsonData["max"].get<int>();
        timestamp = jsonData["timestamp"].get<std::string>();
    }
};

class DataManager {
private:
    const string JSON_URL = "https://opendata.lillemetropole.fr//explore/dataset/disponibilite-parkings/download?format=json&timezone=Europe/Berlin&use_labels_for_header=false";
    const string LOCAL_JSON_FILENAME = "disponibilite_parkings.json";
    const char* dataDirName = "Data_parking";
    const int MAX_HISTORY_SIZE = 13;
    map<string, vector<HistoricalData>> historique_disponibilites;

    void download_json();
    void update_data_and_store_history();

public:
    DataManager();
    void collectData();
    void sauvegarder_data_json(const string& nom_parking, int dispo, int max, const string& timestamp);
    string generateTimestamp();
    vector<string> getNoms() const;
};

#endif // DATAMANAGER_H
