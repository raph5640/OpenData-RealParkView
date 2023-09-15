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
const string JSON_URL = "https://opendata.lillemetropole.fr//explore/dataset/disponibilite-parkings/download?format=json&timezone=Europe/Berlin&use_labels_for_header=false";
const string LOCAL_JSON_FILENAME = "disponibilite_parkings.json";
const char* dirName = "Images_PNG";
const char* dataDirName = "Data_parking";
struct stat info; //pour la création du repertoire Image_PNG
//map<string, vector<pair<time_t, int>>> historique_disponibilites;
map<string, vector<int>> historique_disponibilites;

//La fonction ecrire_data est une fonction de rappel que l'on fourni à cURL dans ma fonction download_json pour traiter les données à mesure qu'elles sont reçues du serveur.
size_t ecrire_data(void* ptr_donnee_recu, size_t size, size_t nombre_elem, FILE* ptr_fichier_ecriture) {
    size_t written = fwrite(ptr_donnee_recu, size, nombre_elem, ptr_fichier_ecriture);
    return written;
}

/**
 * @brief Télécharge un fichier JSON via cURL.
 *
 * La fonction utilise la bibliothèque cURL pour télécharger un
 * fichier JSON à partir d'une URL spécifiée et le sauvegarde localement.
 */
void download_json() {
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
/*
 * Le parking aléatoire ou l'on va suivre l'evolution de disponibilité est choisis au lancement du programme et toutes les 5 minutes il fait la collecte de donnée et met a jour l'image png et rajoute une barre a l'histogramme toute les 5 minutes indiquant en pourcentage le pourcentage de disponibilité du parking selectionnée
 * */
void create_evolution_histogram(const string &filename, const vector<int> &data) {
    const int image_width = 1600;
    const int image_height = 600;
    const int bar_width = image_width / data.size();
    const int MAX_BAR_HEIGHT = image_height - 20;

    gdImagePtr im = gdImageCreateTrueColor(image_width, image_height);
    int white = gdImageColorAllocate(im, 255, 255, 255);
    gdImageFill(im, 0, 0, white);

    int black = gdImageColorAllocate(im, 0, 0, 0);  // Couleur pour le texte

    for (size_t i = 0; i < data.size(); i++) {
        int pourcentage = data[i];
        int bar_height = (image_height * pourcentage) / 100;
        bar_height = min(bar_height, MAX_BAR_HEIGHT);
        int rouge = rand() % 255;
        int vert = rand() % 255;
        int bleu = rand() % 255;
        int color = gdImageColorAllocate(im, rouge, vert, bleu);

        gdImageFilledRectangle(im, i * bar_width, image_height - bar_height, (i + 1) * bar_width, image_height, color);

        //Ecris un label en haut a gauche de chaque barre indiquant le pourcentage
        string label = to_string(pourcentage) + "%";
        gdImageString(im, gdFontGetSmall(), i * bar_width + 5, image_height - bar_height - 15, (unsigned char*)label.c_str(), black);

        // Modifiez ici pour tenir compte de l'écart de temps
        time_t temps = time(nullptr) - (data.size() - 1 - i) * 20 * 60;  //Soustraire 20 minutes pour chaque position en arrière
        tm* now = localtime(&temps);
        stringstream chaine_date, chaine_heure;

        // Formatage de la date
        chaine_date << setw(2) << setfill('0') << now->tm_mday << "/"
               << setw(2) << setfill('0') << (now->tm_mon + 1) << "/"
               << (1900 + now->tm_year);
        string dateStr = chaine_date.str();

        // Formatage de l'heure
        chaine_heure << setw(2) << setfill('0') << now->tm_hour << ":"
               << setw(2) << setfill('0') << now->tm_min;
        string heureStr = chaine_heure.str();

        int textOffset = (bar_width - gdFontGetSmall()->w * (dateStr.length() + heureStr.length())) / 2;
        gdImageString(im, gdFontGetSmall(), i * bar_width + textOffset, image_height - 30, (unsigned char*)dateStr.c_str(), black);
        gdImageString(im, gdFontGetSmall(), i * bar_width + textOffset, image_height - 15, (unsigned char*)heureStr.c_str(), black);
    }

    FILE* out = fopen(filename.c_str(), "wb");
    gdImagePng(im, out);
    fclose(out);

    gdImageDestroy(im);
}



void sauvegarder_data_json(const string& nom_parking, const int dispo, const int max) {
    string filename = string(dataDirName) + "/" + nom_parking + ".json";
    json j;

    //Si le fichier existe déjà, on lit ses données
    if (ifstream(filename)) {
        ifstream infile(filename);
        infile >> j;
        infile.close();
    }

    //Ajout ou mise à jour des informations
    j["dispo"] = dispo;
    j["max"] = max;

    //Sauvegarde des informations dans le fichier
    ofstream outfile(filename);
    outfile << j.dump(4);  //4 est le nombre d'espaces pour l'indentation
    outfile.close();
}
/**
 * @brief Collecte et traite les données de disponibilité des parkings à partir du fichier JSON.
 *
 * Cette fonction réalise les opérations suivantes :
 * 1. Télécharge le fichier JSON à l'aide de libcurl.
 * 2. Lit et parse le fichier JSON téléchargé pour extraire les informations pertinentes.
 * 3. Remplit les vecteurs et les maps avec les données extraites.
 * 4. Sauvegarde les informations pertinentes dans des fichiers JSON locaux.
 * 5. Crée des histogrammes pour visualiser les données de disponibilité des parkings.
 *
 * @note Les vecteurs et les maps utilisés pour stocker les données sont globaux.
 *
 * @see download_json()
 * @see sauvegarder_data_json()
 * @see create_histogram()
 * @see create_evolution_histogram()
 */
void collecte_donnees() {
    download_json();
    cout << "Téléchargement terminé avec libcurl. Le fichier " << LOCAL_JSON_FILENAME << " a été enregistré localement." << endl;

    ifstream jsonFile(LOCAL_JSON_FILENAME);
    json data_obj;
    jsonFile >> data_obj;

    vector<string> noms;
    vector<int> dispo, max;
    //Boucle for pour parcourir l'objet data_obj (On parse les données ici on récupére les donnée et on les stocks dans mes vectors et maps)
    for (auto& element : data_obj) {
        if (element["fields"].contains("libelle") && !element["fields"]["libelle"].is_null()) {
            noms.push_back(element["fields"]["libelle"].get<string>());
        }else {
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
            string libelle = element["fields"]["libelle"].get<string>();
            int dispo_actuelle = element["fields"]["dispo"].get<int>();
            int max_actuel = element["fields"]["max"].get<int>();
            int pourcentage = (100 * dispo_actuelle) / max_actuel;
            historique_disponibilites[libelle].push_back(pourcentage);
        }
    }
    //Ici on sauvegarde le noms les dispos de places et la taille max de chaque parking dans le fichier data
    for (size_t i = 0; i < noms.size(); i++) {
        sauvegarder_data_json(noms[i], dispo[i], max[i]);
    }
    //On crée un histogramme classique qui affiche le pourcentage de disponibilité de tout les parkings
    create_histogram(dirName + string("/pourcentage_place_disponible.png"), noms, dispo, max);
    //Maintenant on va parsez les donnée pour crée l'histogramme evolutif du parking selectionné lors du lancement du programme
    for (auto& pair : historique_disponibilites) {
        const string& nom_parking = pair.first;
        const vector<int>& data = pair.second;
        create_evolution_histogram(string(dirName) + "/" + nom_parking + "_evolution_taux_disponibilite.png", data);
    }

}


int main() {
    //Création du repertoire Image_PNG pour stocker les images générés par le programme
    if (stat(dirName, &info) != 0) {
        cout << "Création du répertoire : " << dirName << endl;
        mkdir(dirName, 0755);  //0755 est le mode d'accès standard pour un répertoire
    } else if (info.st_mode & S_IFDIR) {  // Vérifie que c'est bien un répertoire
        cout << "Le répertoire " << dirName << " existe déjà." << endl;
    } else {
        cerr << "Erreur: " << dirName << " n'est pas un répertoire." << endl;
    }
    //Création du repertoire data_parking ou stocker le vecteur evolution_data
    if (stat(dataDirName, &info) != 0) {
        cout << "Création du répertoire : " << dataDirName << endl;
        mkdir(dataDirName, 0755);  //0755 est le mode d'accès standard pour un répertoire
    } else if (info.st_mode & S_IFDIR) {  //Vérifie que c'est bien un répertoire
        cout << "Le répertoire " << dataDirName << " existe déjà." << endl;
    } else {
        cerr << "Erreur: " << dataDirName << " n'est pas un répertoire." << endl;
    }
    while (true) {
        collecte_donnees();
        cout << "Données collectées."<< endl;
        sleep(1200);  // Pause de 20 minutes
    }
    return 0;
}


