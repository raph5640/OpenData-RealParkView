#include "datamanager.h"
#include "histogram.h"

using namespace std;
namespace fs = std::filesystem;
// Vous pouvez appeler cette fonction au début de votre programme
DataManager::DataManager() : MAX_HISTORY_SIZE(15) {
    if (!fs::exists(dataDirName) || !fs::is_directory(dataDirName)) {
        fs::create_directory(dataDirName);
    }
    initializeHistoricalData();
}
//La fonction ecrire_data est une fonction de rappel que l'on fourni à cURL dans ma fonction download_json pour traiter les données à mesure qu'elles sont reçues du serveur.
size_t ecrire_data(void* ptr_donnee_recu, size_t size, size_t nombre_elem, FILE* ptr_fichier_ecriture) {
    size_t written = fwrite(ptr_donnee_recu, size, nombre_elem, ptr_fichier_ecriture);
    return written;
}

void DataManager::download_json() {
    CURL* curl;
    FILE* fichier;
    CURLcode res;

    curl = curl_easy_init();
    if (curl) {
        fichier = fopen(LOCAL_JSON_FILENAME.c_str(), "wb");
        curl_easy_setopt(curl, CURLOPT_URL, JSON_URL.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ecrire_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fichier);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); //Ignorer la vérification SSL (équivalent de l'option -k)
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            cerr << "Erreur lors du téléchargement avec la librairie cURL: " << curl_easy_strerror(res) << endl;
        }
        curl_easy_cleanup(curl);
        fclose(fichier);
    }
}
void DataManager::collectData() {
    // Télécharger le fichier JSON
    download_json();

    cout<<"Collecte de données en cours..."<<endl;
    // Lire et traiter le fichier JSON
    json dataObj;
    ifstream jsonFile(LOCAL_JSON_FILENAME);
    jsonFile >> dataObj;
    jsonFile.close();

    // Obtenir le timestamp actuel
    string timestamp = generateTimestamp(); // une fonction pour générer le timestamp

    // Parcourir les données des parkings
    for (auto& element : dataObj) {
        if (element["fields"].contains("libelle") && !element["fields"]["libelle"].is_null()) {
            string parkingName = element["fields"]["libelle"].get<string>();
            int availability = 0;
            int maxCapacity = 0;

            if (element["fields"].contains("dispo")) {
                availability = element["fields"]["dispo"].get<int>();
            }

            if (element["fields"].contains("max")) {
                maxCapacity = element["fields"]["max"].get<int>();
            }

            // Ajouter les données actuelles à l'historique du parking
            saveParkingData(parkingName, availability, maxCapacity, timestamp);
        }
    }
}
void DataManager::saveParkingData(const string& parkingName, int availability, int maxCapacity, const string& timestamp) {
    // Charger l'historique actuel du parking depuis le fichier JSON
    json parkingHistory;

    if (!fs::exists(dataDirName) || !fs::is_directory(dataDirName)) {
        fs::create_directory(dataDirName);
    }
    string parkingHistoryFile = string(dataDirName) + "/" + parkingName + ".json";

    if (ifstream(parkingHistoryFile)) {
        ifstream inFile(parkingHistoryFile);
        inFile >> parkingHistory;
        inFile.close();
    }

    // Ajouter la nouvelle entrée à l'historique
    json newEntry;
    newEntry["dispo"] = availability;
    newEntry["max"] = maxCapacity;
    newEntry["timestamp"] = timestamp;

    parkingHistory.push_back(newEntry);

    // Limiter l'historique à MAX_HISTORY_SIZE entrées
    if (parkingHistory.size() > this->MAX_HISTORY_SIZE) {
        parkingHistory.erase(parkingHistory.begin());
    }

    // Sauvegarder l'historique mis à jour dans le fichier JSON
    ofstream outFile(parkingHistoryFile);
    outFile << parkingHistory.dump(4); // 4 est le nombre d'espaces pour l'indentation
    outFile.close();

    // Mettre à jour la map historique_disponibilites
    historique_disponibilites[parkingName].push_back({availability, maxCapacity, timestamp});
}



void DataManager::sauvegarder_data_json(const string& nom_parking, const int dispo, const int max, const string& timestamp) {
    string filename = string(dataDirName) + "/" + nom_parking + ".json";
    json j;

    // Si le fichier existe déjà, on lit ses données
    if (ifstream(filename)) {
        ifstream infile(filename);
        infile >> j;
        infile.close();
    }

    // Ajout ou mise à jour des informations
    j["dispo"] = dispo;
    j["max"] = max;
    j["timestamp"] = timestamp;

    // Sauvegarde des informations dans le fichier
    ofstream outfile(filename);
    outfile << j.dump(4);  // 4 est le nombre d'espaces pour l'indentation
    outfile.close();
}


void DataManager::update_data_and_store_history() {
    download_json();
    cout << "Téléchargement terminé avec libcurl. Le fichier " << LOCAL_JSON_FILENAME << " a été enregistré localement." << endl;

    ifstream jsonFile(LOCAL_JSON_FILENAME);
    json data_obj;
    jsonFile >> data_obj;

    vector<string> noms;
    vector<int> dispo, max;
    vector<string> timestamps; //vecteur pour stocker les horodatages actuels
    vector<int> disponibilites; // vecteur pour stocker les disponibilités actuelles

    // Obtenez la date et l'heure actuelles
    time_t temps = time(nullptr);
    tm* now = localtime(&temps);
    stringstream chaine_date, chaine_heure;

    // Formatage de la date
    chaine_date << setw(4) << setfill('0') << (1900 + now->tm_year) << "-"
               << setw(2) << setfill('0') << (now->tm_mon + 1) << "-"
               << setw(2) << setfill('0') << now->tm_mday;
    string dateStr = chaine_date.str();

    // Formatage de l'heure
    chaine_heure << setw(2) << setfill('0') << now->tm_hour << ":"
                << setw(2) << setfill('0') << now->tm_min << ":"
                << setw(2) << setfill('0') << now->tm_sec;
    string heureStr = chaine_heure.str();

    // Stock les horodatages actuels
    timestamps.push_back(dateStr + " " + heureStr);

    // Boucle for pour parcourir l'objet data_obj et collecter les données actuelles
    for (auto& element : data_obj) {
        if (element["fields"].contains("libelle") && !element["fields"]["libelle"].is_null()) {
            noms.push_back(element["fields"]["libelle"].get<string>());
        } else {
            noms.push_back("Inconnu");
        }

        if (element["fields"].contains("dispo") && !element["fields"]["dispo"].is_null()) {
            dispo.push_back(element["fields"]["dispo"].get<int>());
        } else {
            dispo.push_back(0);
        }

        if (element["fields"].contains("max") && !element["fields"]["max"].is_null()) {
            max.push_back(element["fields"]["max"].get<int>());
        } else {
            max.push_back(1);
        }

        if (element["fields"].contains("libelle") && element["fields"].contains("dispo") && element["fields"].contains("max")) {
            int dispo_actuelle = element["fields"]["dispo"].get<int>();
            int max_actuel = element["fields"]["max"].get<int>();
            int pourcentage = (100 * dispo_actuelle) / max_actuel;
            disponibilites.push_back(pourcentage);
        }
    }

    // Sauvegarde les données actuelles dans le répertoire "Data_parking"
    for (size_t i = 0; i < noms.size(); i++) {
        sauvegarder_data_json(noms[i], dispo[i], max[i], timestamps[i]);

    }

    // Crée des histogrammes pour les données actuelles
    //Histogram::createEvolutionHistogram(string(dirName) + "/global_evolution_taux_disponibilite.png", timestamps, disponibilites);
}


void DataManager::initializeHistoricalData() {
    // Parcourir tous les fichiers JSON dans le répertoire dataDirName
    const string dataDir = dataDirName;
    for (const auto& entry : fs::directory_iterator(dataDir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".json") {
            const string filename = entry.path().string();
            ifstream infile(filename);
            json j;
            infile >> j;
            infile.close();

            // Déclarez dispo et max en dehors de la boucle
            int dispo = 0;
            int max = 0;

            // Vérifiez d'abord si j est un objet JSON ou un tableau JSON
            if (j.is_array()) { // Modifiez ici pour charger tous les éléments du tableau
                for (const auto& item : j) {
                    // Accédez à "dispo" et "max" seulement si j est un objet JSON
                    if (item.contains("dispo") && !item["dispo"].is_null()) {
                        dispo = item["dispo"].get<int>();
                    }

                    if (item.contains("max") && !item["max"].is_null()) {
                        max = item["max"].get<int>();
                    }

                    const string timestamp = item["timestamp"];
                    const string parkingName = entry.path().stem().string();

                    // Ajoutez ces informations à votre historique_disponibilites
                    HistoricalData newData;
                    newData.dispo = dispo;
                    newData.max = max;
                    newData.timestamp = timestamp;
                    historique_disponibilites[parkingName].push_back(newData);
                }
            }
        }
    }
}


void DataManager::load_history() {
    // Parcoure tous les fichiers JSON dans le répertoire dataDirName
    const string dataDir = dataDirName;
    for (const auto& entry : fs::directory_iterator(dataDir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".json") {
            const string filename = entry.path().string();
            ifstream infile(filename);
            json j;
            infile >> j;
            infile.close();

            // Déclare dispo et max en dehors de la boucle
            int dispo = 0;
            int max = 0;

            // Vérifi d'abord si j est un objet JSON ou un tableau JSON
            if (j.is_object()) {
                const string nom_parking = entry.path().stem().string();

                // Accéde à "dispo" et "max" seulement si j est un objet JSON
                if (j.contains("dispo") && !j["dispo"].is_null()) {
                    dispo = j["dispo"].get<int>();
                }

                if (j.contains("max") && !j["max"].is_null()) {
                    max = j["max"].get<int>();
                }

                const string timestamp = j["timestamp"];
                // Ajoute ces informations à votre historique_disponibilites
                HistoricalData newData;
                newData.dispo = dispo;
                newData.max = max;
                newData.timestamp = timestamp;
                historique_disponibilites[nom_parking].push_back(newData);
            }
        }
    }
}


string DataManager::generateTimestamp() {
    time_t currentTime = time(nullptr);
    tm* currentTm = localtime(&currentTime);
    stringstream timestamp;
    timestamp << setw(4) << setfill('0') << (currentTm->tm_year + 1900) << "-"
              << setw(2) << setfill('0') << (currentTm->tm_mon + 1) << "-"
              << setw(2) << setfill('0') << currentTm->tm_mday << " "
              << setw(2) << setfill('0') << currentTm->tm_hour << ":"
              << setw(2) << setfill('0') << currentTm->tm_min << ":"
              << setw(2) << setfill('0') << currentTm->tm_sec;

    return timestamp.str();
}


vector<string> DataManager::getNoms() const {
    vector<string> noms;
    for (const auto& pair : historique_disponibilites) {
        noms.push_back(pair.first);
    }
    return noms;
}

vector<int> DataManager::getDispo() const {
    vector<int> dispo;
    for (const auto& pair : historique_disponibilites) {
        const vector<HistoricalData>& data = pair.second;
        if (!data.empty()) {
            dispo.push_back(data.back().dispo);
        } else {
            dispo.push_back(0);
        }
    }
    return dispo;
}

vector<int> DataManager::getMax() const {
    vector<int> maxValues;
    for (const auto& pair : historique_disponibilites) {
        const vector<HistoricalData>& data = pair.second;
        if (!data.empty()) {
            maxValues.push_back(data.back().max);
        } else {
            maxValues.push_back(0);
        }
    }
    return maxValues;
}

vector<string> DataManager::getTimestamps(const string& parkingName) const {
    vector<string> parkingTimestamps;

    // Recherche le parking spécifique dans l'historique
    auto it = historique_disponibilites.find(parkingName);
    if (it != historique_disponibilites.end()) {
        const vector<HistoricalData>& historicalData = it->second;

        // Ajoute les horodatages de l'historique à la liste
        for (const HistoricalData& data : historicalData) {
            parkingTimestamps.push_back(data.timestamp);
        }
    }

    return parkingTimestamps;
}

vector<int> DataManager::getAvailabilityHistory(const string& parkingName) const {
    vector<int> availabilityHistory;

    // Recherche le parking spécifique dans l'historique
    auto it = historique_disponibilites.find(parkingName);
    if (it != historique_disponibilites.end()) {
        const vector<HistoricalData>& historicalData = it->second;

        // Ajoute les disponibilités de l'historique à la liste
        for (const HistoricalData& data : historicalData) {
            availabilityHistory.push_back(data.dispo);
        }
    }

    return availabilityHistory;
}
