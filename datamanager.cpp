#include "datamanager.h"
//author Raphael De Oliveira
using namespace std;
namespace fs = std::filesystem;

DataManager::DataManager() : MAX_HISTORY_SIZE(13) {
    if (!fs::exists(dataDirName) || !fs::is_directory(dataDirName)) {
        fs::create_directory(dataDirName);
    }
}
//ecris les donnée au fur et a mesure que le serveur envois les données
size_t ecrire_data(void* ptr_donnee_recu, size_t size, size_t nombre_elem, FILE* ptr_fichier_ecriture) {
    size_t written = fwrite(ptr_donnee_recu, size, nombre_elem, ptr_fichier_ecriture);
    return written;
}

void DataManager::download_json() {
    CURL* curl;
    FILE* fichier;
    CURLcode resultat;
    curl = curl_easy_init();
    if (curl) {
        fichier = fopen(LOCAL_JSON_FILENAME.c_str(), "wb");
        curl_easy_setopt(curl, CURLOPT_URL, JSON_URL.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ecrire_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fichier);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); //On ignore la vérification SSL (équivalent de l'option -k)
        resultat = curl_easy_perform(curl);
        if (resultat != CURLE_OK) {
            cerr << "Erreur lors du téléchargement avec la librairie cURL: " << curl_easy_strerror(resultat) << endl;
        }
        curl_easy_cleanup(curl);
        fclose(fichier);
    }
}
void DataManager::update_data_and_store_history() {
    download_json();
    cout << "Téléchargement terminé avec libcurl. Le fichier " << LOCAL_JSON_FILENAME << " a été enregistré localement." << endl;

    ifstream jsonFile(LOCAL_JSON_FILENAME);
    json data_obj;
    jsonFile >> data_obj;

    // Obtention de la date et de l'heure actuelles
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

    // Stocke l'horodatage actuel dans le format souhaité
    string timestamp = dateStr + " " + heureStr;

    // Boucle for pour parcourir l'objet data_obj et collecter les données actuelles
    for (auto& element : data_obj) {
        if (element.contains("fields")) {
            json parkingData; // Crée un objet JSON pour stocker les données du parking

            // Vous pouvez spécifier les clés de l'objet "fields" pour obtenir les données souhaitées
            if (element["fields"].contains("dispo")) {
                parkingData["dispo"] = element["fields"]["dispo"].get<int>();
            } else {
                parkingData["dispo"] = 0;
            }

            if (element["fields"].contains("max")) {
                parkingData["max"] = element["fields"]["max"].get<int>();
            } else {
                parkingData["max"] = 1;
            }

            parkingData["timestamp"] = timestamp;

            // Ajout des données du parking à l'historique
            historique_disponibilites[element["fields"]["libelle"].get<string>()].emplace_back(parkingData);

            // Si le nombre d'entrées dépasse MAX_HISTORY_SIZE, on supprime la plus ancienne
            if (historique_disponibilites[element["fields"]["libelle"].get<string>()].size() > MAX_HISTORY_SIZE) {
                historique_disponibilites[element["fields"]["libelle"].get<string>()].erase(
                    historique_disponibilites[element["fields"]["libelle"].get<string>()].begin());
            }

            // Maintenant on limite également la taille de l'historique dans le fichier JSON
            string filename = string(dataDirName) + "/" + element["fields"]["libelle"].get<string>() + ".json";
            json j;

            // Chargement du fichier JSON existant s'il existe
            if (ifstream(filename)) {
                ifstream infile(filename);
                infile >> j;
                infile.close();
            }

            // Ajoute les données actuelles à l'historique existant
            for (const HistoricalData& data : historique_disponibilites[element["fields"]["libelle"].get<string>()]) {
                json historicalDataJSON;

                historicalDataJSON["dispo"] = data.dispo;
                historicalDataJSON["max"] = data.max;
                historicalDataJSON["timestamp"] = data.timestamp;

                j.push_back(historicalDataJSON); // Ajoute l'objet JSON à l'objet JSON principal
            }

            // Si le nombre d'entrées dépasse MAX_HISTORY_SIZE, on supprime les plus anciennes
            while (j.size() > MAX_HISTORY_SIZE) {
                j.erase(j.begin());
            }

            // Sauvegarde des données dans le fichier
            ofstream outfile(filename);
            outfile << j.dump(4);
            outfile.close();
        }
    }
}


void DataManager::collectData() {
    static int increment=1;
    cout<<"Collecte de données numero "<<increment<<" en cours..."<<endl;
    download_json();
    update_data_and_store_history();
    increment++;
}

void DataManager::sauvegarder_data_json(const string& nom_parking, const int dispo, const int max, const string& timestamp) {
    string filename = string(dataDirName) + "/" + nom_parking + ".json";
    json j;

    if (ifstream(filename)) {
        ifstream infile(filename);
        infile >> j;
        infile.close();
    }

    j["dispo"] = dispo;
    j["max"] = max;
    j["timestamp"] = timestamp;

    ofstream outfile(filename);
    outfile << j.dump(4);
    outfile.close();
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
