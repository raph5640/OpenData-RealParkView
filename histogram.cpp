#include "histogram.h"
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
using namespace std;
//Auteur : Raphael De Oliveira

vector<string> split_into_lines(const string& s) {
    istringstream iss(s);
    vector<string> lines;
    string line;
    while (getline(iss, line)) {
        lines.push_back(line);
    }
    return lines;
}

// Fonction pour créer un histogramme
void Histogram::createHistogram(const string& filename) {

    vector<string> noms;
    vector<int> dispo;
    vector<int> max;

    // Étape 1: Lire le fichier 'disponibilite_parkings.json'
    ifstream file("disponibilite_parkings.json");
    if (file.is_open()) {
        json j;
        file >> j;

        for (const auto& parking : j) {
            if(parking.find("fields") != parking.end()) {
                auto fields = parking["fields"];
                if(fields.find("libelle") != fields.end() &&
                   fields.find("dispo") != fields.end() &&
                   fields.find("max") != fields.end()) {

                    string nom = fields["libelle"].get<string>();
                    int dispoValue = fields["dispo"].get<int>();
                    int maxValue = fields["max"].get<int>();

                    noms.push_back(nom);
                    dispo.push_back(dispoValue);
                    max.push_back(maxValue);

                } else {
                    // Message d'erreur
                    cerr << "Objet parking malformé: " << parking << endl;
                }
            }
        }

        file.close();
    }
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


void Histogram::createEvolutionHistogramFromJSON(const std::string& filename, const std::string& jsonFilePath) {
    json data;
    std::ifstream jsonFile(jsonFilePath);
    jsonFile >> data;
    jsonFile.close();

    std::vector<std::string> timestamps;
    std::vector<int> availabilityHistory;
    std::vector<int> maxValues;

    for (const auto& item : data) {
        if (item.contains("dispo") && item.contains("max") && item.contains("timestamp")) {
            int availability = item["dispo"].get<int>();
            int maxCapacity = item["max"].get<int>();
            std::string timestamp = item["timestamp"].get<std::string>();

            timestamps.push_back(timestamp);
            availabilityHistory.push_back(availability);
            maxValues.push_back(maxCapacity);
        }
    }

    const int image_width = 1600;
    const int image_height = 600;
    const int bar_width = image_width / availabilityHistory.size();
    const int MAX_BAR_HEIGHT = image_height - 20;

    gdImagePtr im = gdImageCreateTrueColor(image_width, image_height);
    int white = gdImageColorAllocate(im, 255, 255, 255);
    gdImageFill(im, 0, 0, white);

    int black = gdImageColorAllocate(im, 0, 0, 0);  // Couleur pour le texte
    gdImageString(im, gdFontGetSmall(), 10, 10, (unsigned char*)filename.c_str(), black);

    for (size_t i = 0; i < availabilityHistory.size(); i++) {
        int availability = availabilityHistory[i];
        int maxCapacity = maxValues[i];
        int pourcentage = (100 * availability) / maxCapacity; // Calcul du pourcentage

        int bar_height = (image_height * pourcentage) / 100;
        bar_height = min(bar_height, MAX_BAR_HEIGHT);
        int rouge = rand() % 255;
        int vert = rand() % 255;
        int bleu = rand() % 255;
        int color = gdImageColorAllocate(im, rouge, vert, bleu);

        gdImageFilledRectangle(im, i * bar_width, image_height - bar_height, (i + 1) * bar_width, image_height, color);

        string label = to_string(pourcentage) + "%";
        gdImageString(im, gdFontGetSmall(), i * bar_width + 5, image_height - bar_height - 15, (unsigned char*)label.c_str(), black);

        string timestamp = timestamps[i];
        size_t pos = timestamp.find(' ');
        if (pos != string::npos) {
            string dateStr = timestamp.substr(0, pos);
            string heureStr = timestamp.substr(pos + 1);

            int textOffset = (bar_width - gdFontGetSmall()->w * (dateStr.length() + heureStr.length())) / 2;
            gdImageString(im, gdFontGetSmall(), i * bar_width + textOffset, image_height - 30, (unsigned char*)dateStr.c_str(), black);
            gdImageString(im, gdFontGetSmall(), i * bar_width + textOffset, image_height - 15, (unsigned char*)heureStr.c_str(), black);
        }
    }

    FILE* out = fopen(filename.c_str(), "wb");
    gdImagePng(im, out);
    fclose(out);

    gdImageDestroy(im);
}

void Histogram::showGeneratedImagesHTML() const {
    // Chemin du répertoire contenant les images .png générées
    const std::string path = "Images_PNG/";

    // Nom du fichier HTML à créer
    const std::string htmlFilename = "Images_histograms.html";

    // Ouvre le fichier pour écrire le HTML
    ofstream htmlFile(htmlFilename);
    if (!htmlFile.is_open()) {
        cerr << "Erreur lors de la création du fichier HTML." << endl;
        return;
    }

    // Écri l'en-tête du fichier HTML
    htmlFile << "<!DOCTYPE html>\n<html>\n<head>\n";
    htmlFile << "<title>Histogrammes des parkings</title>\n";
    htmlFile << "</head>\n<body>\n";

    // Ouvre le dossier et lisez chaque fichier
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(path.c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            std::string filename = ent->d_name;

            // Vérifi si le fichier est une image .png
            if (filename.size() > 4 && filename.substr(filename.size() - 4) == ".png") {
                // Ajoute chaque image au fichier HTML
                htmlFile << "<img src=\"" << path << filename << "\" alt=\"" << filename << "\" style=\"width:100%;margin-top:20px;\">\n";
            }
        }
        closedir(dir);
    } else {
        // Impossible d'ouvrir le répertoire
        perror("");
        htmlFile.close();
        return;
    }

    // Ferme la balise body et html
    htmlFile << "</body>\n</html>\n";
    htmlFile.close();

    // Ouvre le fichier HTML avec le navigateur par défaut
    std::string commandXdgOpen = "xdg-open " + htmlFilename;
    system(commandXdgOpen.c_str());
}

