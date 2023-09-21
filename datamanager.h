/**
 * @file datamanager.h
 * \author Raphael De Oliveira
 * @brief Définition de la classe DataManager et la structure HistoricalData pour la gestion des données des parkings.
 */
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
/**
 * @brief Structure représentant une entrée de données historiques pour un parking.
 */
struct HistoricalData {
    int dispo;
    int max;
    std::string date;
    /**
     * @brief Constructeur à partir d'un objet JSON.
     * @param jsonData Objet JSON contenant les données.
     */
    HistoricalData(const nlohmann::json& jsonData) {
        // Extraire les valeurs de l'objet JSON et initialiser les membres de la structure
        dispo = jsonData["dispo"].get<int>();
        max = jsonData["max"].get<int>();
        date = jsonData["date"].get<std::string>();
    }
};
/**
 * @brief Classe pour la gestion de la disponibilité des parkings.
 */
class DataManager {
private:
    const string JSON_URL = "https://opendata.lillemetropole.fr//explore/dataset/disponibilite-parkings/download?format=json&timezone=Europe/Berlin&use_labels_for_header=false";
    const string LOCAL_JSON_FILENAME = "disponibilite_parkings.json";
    const char* dataDirName = "Data_parking";
    const int MAX_HISTORY_SIZE = 13;
    map<string, vector<HistoricalData>> historique_disponibilites;
    /**
     * @brief Télécharge le fichier JSON depuis l'URL définie.
     */
    void download_json();
    /**
     * @brief Met à jour les données et stocke l'historique.
     */
    void update_data_and_store_history();

public:
    /**
     * @brief Constructeur par défaut.
     */
    DataManager();
    /**
     * @brief Collecte les données des parkings et met à jour l'historique.
     */
    void collectData();
    /**
     * @brief Sauvegarde les données de disponibilité d'un parking spécifique.
     * @param nom_parking Nom du parking.
     * @param dispo Nombre de places disponibles.
     * @param max Nombre total de places.
     * @param date Horodatage de l'entrée.
     */
    void sauvegarder_data_json(const string& nom_parking, int dispo, int max, const string& date);
    /**
     * @brief Génère un horodatage courant.
     * @return String représentant l'horodatage.
     */
    string generatedate();
    /**
     * @brief Renvoie les noms de tous les parkings disponibles.
     * @return Vector des noms des parkings.
     */
    vector<string> getNoms() const;
};

#endif // DATAMANAGER_H
