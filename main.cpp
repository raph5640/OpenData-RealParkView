#include <iostream>
#include <fstream>
#include <string>
#include <curl/curl.h>
#include <gd.h>
#include <nlohmann/json.hpp>
#include <vector>
#include <cmath>
#include <gdfonts.h>


using namespace std;

const string JSON_URL = "https://opendata.lillemetropole.fr//explore/dataset/disponibilite-parkings/download?format=json&timezone=Europe/Berlin&use_labels_for_header=false";
const string LOCAL_JSON_FILENAME = "disponibilite_parkings.json";

// Fonction pour télécharger le fichier JSON
size_t write_data(void* ptr, size_t size, size_t nmemb, FILE* stream) {
    return fwrite(ptr, size, nmemb, stream);
}

void download_json() {
    CURL* curl;
    CURLcode res;
    FILE* fp;

    curl = curl_easy_init();
    if (curl) {
        fp = fopen(LOCAL_JSON_FILENAME.c_str(), "wb");
        if (!fp) {
            cerr << "Erreur d'ouverture du fichier " << LOCAL_JSON_FILENAME << endl;
            return;
        }

        curl_easy_setopt(curl, CURLOPT_URL, JSON_URL.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            cerr << "Erreur curl: " << curl_easy_strerror(res) << endl;
        }

        fclose(fp);
        curl_easy_cleanup(curl);
    }
}

// Fonction pour créer un histogramme
void create_histogram(const string& filename, const vector<string>& noms, const vector<int>& dispo, const vector<int>& max) {
    const int image_width = 1200;
    const int image_height = 400;
    const int bar_width = image_width / noms.size();

    gdImagePtr im = gdImageCreateTrueColor(image_width, image_height);
    int white = gdImageColorAllocate(im, 255, 255, 255);
    gdImageFill(im, 0, 0, white);

    int black = gdImageColorAllocate(im, 0, 0, 0);  // Couleur pour le texte et les axes

    //Dessin des barres de l'histogramme
    for (size_t i = 0; i < dispo.size(); i++) {
        int pourcentage = (100 * dispo[i]) / max[i];
        int bar_height = (image_height * pourcentage) / 100;
        int color = gdImageColorAllocate(im, rand() % 255, rand() % 255, rand() % 255);

        gdImageFilledRectangle(im, i * bar_width, image_height - bar_height, (i+1) * bar_width, image_height, color);

        //Ajout du texte pour le nom du parking au-dessus de chaque barre
        string label = noms[i] + " (" + to_string(pourcentage) + "%)";
        gdImageString(im, gdFontGetSmall(), i * bar_width + 5, image_height - bar_height - 15, (unsigned char*)label.c_str(), black);
    }

    //Dessin des axes
    gdImageLine(im, 0, image_height - 1, image_width, image_height - 1, black);  //Axe des x
    gdImageLine(im, 0, 0, 0, image_height, black);  //Axe des y

    FILE* out = fopen(filename.c_str(), "wb");
    gdImagePng(im, out);
    fclose(out);

    gdImageDestroy(im);
}

using json = nlohmann::json;

int main() {
    download_json();
    cout << "Téléchargement terminé. Le fichier " << LOCAL_JSON_FILENAME << " a été enregistré localement." << endl;

    ifstream jsonFile(LOCAL_JSON_FILENAME);
    json jsonObj;
    jsonFile >> jsonObj;

    vector<string> noms;
    vector<int> dispo, max;

    for (auto& record : jsonObj) {
        if (record["fields"].contains("libelle") && !record["fields"]["libelle"].is_null()) {
            noms.push_back(record["fields"]["libelle"].get<string>());
        }else {
            noms.push_back("Inconnu");
        }

        if (record["fields"].contains("dispo") && !record["fields"]["dispo"].is_null()) {
            dispo.push_back(record["fields"]["dispo"].get<int>());
        } else {
            dispo.push_back(0);
        }

        if (record["fields"].contains("max") && !record["fields"]["max"].is_null()) {
            max.push_back(record["fields"]["max"].get<int>());
        } else {
            max.push_back(1);
        }
    }


    create_histogram("histogram.png", noms, dispo, max);

    return 0;
}

