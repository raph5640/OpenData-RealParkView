#include <iostream>
#include <fstream>
#include <string>
#include <gd.h>
#include <nlohmann/json.hpp>
#include <vector>
#include <cmath>
#include <gdfonts.h>
#include <curl/curl.h>

//Auteur : Raphael De Oliveira

using namespace std;

const string JSON_URL = "https://opendata.lillemetropole.fr//explore/dataset/disponibilite-parkings/download?format=json&timezone=Europe/Berlin&use_labels_for_header=false";
const string LOCAL_JSON_FILENAME = "disponibilite_parkings.json";


//La fonction ecrire_data est une fonction de rappel que l'on fourni à cURL dans ma fonction download_json pour traiter les données à mesure qu'elles sont reçues du serveur.
size_t ecrire_data(void* ptr_donnee_recu, size_t size, size_t nombre_elem, FILE* ptr_fichier_ecriture) {
    size_t written = fwrite(ptr_donnee_recu, size, nombre_elem, ptr_fichier_ecriture);
    return written;
}

//Fonction qui va récuperer le fichier .json en passant par curl
void download_json() {
    CURL* curl;
    FILE* fp;
    CURLcode res;

    curl = curl_easy_init();
    if (curl) {
        fp = fopen(LOCAL_JSON_FILENAME.c_str(), "wb");
        curl_easy_setopt(curl, CURLOPT_URL, JSON_URL.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ecrire_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); //Ignorer la vérification SSL (équivalent de l'option -k)
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            cerr << "Erreur lors du téléchargement avec la librairie cURL: " << curl_easy_strerror(res) << endl;
        }
        curl_easy_cleanup(curl);
        fclose(fp);
    }
}

//une fonction qui découpe une chaîne en plusieurs lignes
vector<string> split_into_lines(const string& chaine) {
    vector<string> result;
    stringstream chaine_stream(chaine);
    string word;

    while (getline(chaine_stream, word, ' ')) { //decoupe la chaine a chaque espace
        result.push_back(word);
    }

    return result;
}
// Fonction pour créer un histogramme
void create_histogram(const string& filename, const vector<string>& noms, const vector<int>& dispo, const vector<int>& max) {
    const int image_width = 1600;
    const int image_height = 600;
    const int bar_width = image_width / noms.size();
    const int MAX_BAR_HEIGHT = image_height - 20;

    gdImagePtr im = gdImageCreateTrueColor(image_width, image_height);
    int white = gdImageColorAllocate(im, 255, 255, 255);
    gdImageFill(im, 0, 0, white);

    int black = gdImageColorAllocate(im, 0, 0, 0);  // Couleur pour le texte

    //Dessin des barres de l'histogramme
    for (size_t i = 0; i < dispo.size(); i++) {
        int pourcentage = (100 * dispo[i]) / max[i];
        int bar_height = (image_height * pourcentage) / 100;
        const int RELIEF_OFFSET = 10;
        bar_height = min(bar_height, MAX_BAR_HEIGHT);
        int rouge = rand() % 255;
        int vert = rand() % 255;
        int bleu = rand() % 255;
        int color = gdImageColorAllocate(im, rouge, vert, bleu);
        int color_relief = gdImageColorAllocate(im, rouge-30, vert, bleu);
        // Dessine le rectangle principal
        gdImageFilledRectangle(im, i * bar_width, image_height - bar_height, (i + 1) * bar_width - RELIEF_OFFSET, image_height, color);
        //On dessine le reflief 3D
        for (int offset = 1; offset <= RELIEF_OFFSET; offset++) {
            gdImageFilledRectangle(im,(i + 1) * bar_width - offset,image_height - bar_height - offset + RELIEF_OFFSET,(i + 1) * bar_width - offset + 1,image_height - offset + RELIEF_OFFSET,color_relief);
        }

        string label = noms[i] + " (" + to_string(pourcentage) + "%)";
        if (pourcentage < 90) {     //Si le pourcentage du parking est inferieur a 90% on écrit son nom sur plusieurs ligne pour éviter les cheuvauchements du texte sur les autres barres suivantes
            auto lines = split_into_lines(label);
            int offsetY = (lines.size() - 1) * 12;
            for (const auto& line : lines) {
                gdImageString(im, gdFontGetSmall(), i * bar_width + 5, image_height - bar_height - 15 - offsetY, (unsigned char*)line.c_str(), black);
                offsetY -= 12;  //décrémente la valeur pour aller vers le haut
            }
        } else {  //sinon si le pourcentage est tres haut (>90) on considerer que si l'on affiche le nom entier du parking sur une ligne ca ne pose pas de probleme d'affichage et de lisibilité
            gdImageString(im, gdFontGetSmall(), i * bar_width + 5, image_height - bar_height - 15, (unsigned char*)label.c_str(), black);
        }
    }

    FILE* out = fopen(filename.c_str(), "wb");
    gdImagePng(im, out);
    fclose(out);

    gdImageDestroy(im);
}

using json = nlohmann::json;

int main() {
    download_json();
    cout << "Téléchargement terminé avec libcurl. Le fichier " << LOCAL_JSON_FILENAME << " a été enregistré localement." << endl;

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

    create_histogram("pourcentage_place_disponible.png", noms, dispo, max);

    return 0;
}

