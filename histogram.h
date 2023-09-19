/**
 * \file histogram.h
 * \author Raphael De Oliveira
 * \brief Classe pour la création d'histogrammes.
 */
#ifndef HISTOGRAM_H
#define HISTOGRAM_H
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
#include <dirent.h>
//Auteur : Raphael De Oliveira
using namespace std;
using json = nlohmann::json;
/**
 * \class Histogram
 * \brief Classe permettant de générer des graphiques sous forme d'histogrammes.
 */
class Histogram {
public:
    /**
     * \brief Crée un histogramme basé sur les données fournies.
     * \param filename Nom du fichier dans lequel l'image sera sauvegardée.
     * \param noms Noms à afficher sous les barres.
     * \param dispo Valeurs de disponibilité pour chaque barre.
     * \param max Valeurs maximales pour chaque barre.
     */
    static void createHistogram(const string& filename);
    /**
     * \brief Crée un histogramme d'évolution basé sur un fichier JSON.
     * \param filename Nom du fichier dans lequel l'image sera sauvegardée.
     * \param jsonFilePath Chemin vers le fichier JSON contenant les données.
     */
    static void createEvolutionHistogramFromJSON(const std::string& filename, const std::string& jsonFilePath);
    /**
     * \brief Affiche les images générées.
     * Cette fonction génère un fichier HTML contenant toutes les images d'histogrammes créées et l'ouvre dans un navigateur par défaut.
     */
    void showGeneratedImagesHTML() const;
    void showTerminalHistogram();

};

#endif // HISTOGRAM_H
