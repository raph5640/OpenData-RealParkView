#include <iostream>
#include <fstream>
#include <string>
#include <gd.h>
#include <nlohmann/json.hpp>
#include <vector>
#include <cmath>
#include <gdfonts.h>


using namespace std;

const string JSON_URL = "https://opendata.lillemetropole.fr//explore/dataset/disponibilite-parkings/download?format=json&timezone=Europe/Berlin&use_labels_for_header=false";
const string LOCAL_JSON_FILENAME = "disponibilite_parkings.json";

void download_json() {
    string command = "wget \"" + JSON_URL + "\" -O " + LOCAL_JSON_FILENAME;
    int result = system(command.c_str());

    if (result) {
        cerr << "Erreur lors du téléchargement du fichier avec wget. Code d'erreur: " << result << endl;
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


    create_histogram("histogram_pourcentage_disponibilité.png", noms, dispo, max);

    return 0;
}

