#include <iostream>
#include <fstream>
#include <string>
#include "Libraries/json.hpp" // Bibliothèque JSON pour C++ (https://github.com/nlohmann/json)
#include <cstdlib> // Pour system() ou CreateProcess sur Windows

using json = nlohmann::json;

// Fonction pour lire et afficher les infos du projet Unreal
void ShowProjectInfo(const std::string& uprojectPath) {
    std::ifstream file(uprojectPath);
    if (!file) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier .uproject !" << std::endl;
        return;
    }
    
    json projectJson;
    file >> projectJson;

    if (projectJson.contains("Modules")) {
        for (const auto& module : projectJson["Modules"]) {
            if (module.contains("Name")) {
                const std::string nomProjet=module["Name"];
                std::cout << "Nom du jeu " << module["Name"] << std::endl;
            }
        }
    }
    
    std::string engineVersion = projectJson["EngineAssociation"];
    if (engineVersion.length() > 3) {
        engineVersion = "Source";
    }
    std::cout << "Version Unreal Engine : " << engineVersion << std::endl;
    
    if (projectJson.contains("Plugins")) {
        std::cout << "Plugins utilises :" << std::endl;
        for (const auto& plugin : projectJson["Plugins"]) {
            if (plugin.contains("Name")) {
                std::cout << " - " << plugin["Name"] << std::endl;
            }
        }
    }
}

// Fonction pour compiler un projet Unreal
void BuildProject(const std::string& uprojectPath) {
    std::string projectName = uprojectPath.substr(uprojectPath.find_last_of("/\\") + 1);
    projectName = projectName.substr(0, projectName.find_last_of("."));
    std::string target= projectName + "Editor";
    std::string command = "./Engine/Build/BatchFiles/Build.bat";
    command += " " + target + " Win64 Development -Project=\"" + uprojectPath + "\"" + "- waitmutex";
    
    int result = system(command.c_str());
    if (result != 0) {
        std::cerr << "Erreur lors de la compilation." << std::endl;
    }
}

// Fonction pour packager un projet Unreal
void PackageProject(const std::string& uprojectPath, const std::string& outputPath) {
    std::string command = ".\\Engine\\Build\\BatchFiles\\RunUAT.bat";
    command += " -ScriptsForProject=" + uprojectPath + " BuildCookRun";
    command += " -project=" + uprojectPath;
    command += " -noP4 -clientconfig=Development -serverconfig=Development";
    command += " -nocompileeditor -unrealexe=C:/UnrealEngine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe -utf8output -platform=Win64 -build -cook -map=ThirdPersonMap+StarterMap -CookCultures=fr -unversionedcookedcontent -stage -package -cmdline=' -Messaging' -addcmdline='-SessionId=12D2F7F146879741318F75817C660C32 -SessionOwner='alexa' -SessionName='ProfileTP' ";
    command += " -archivedirectory=" + outputPath;
    
    int result = system(command.c_str());
    if (result != 0) {
        std::cerr << "Erreur lors du packaging." << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage : PackagingTool [CHEMIN_DU_UPROJECT] [show-infos | build | package CHEMIN_DU_PACKAGE]" << std::endl;
        return 1;
    }
    
    std::string uprojectPath = argv[1];
    std::string command = argv[2];
    
    if (command == "show-infos") {
        ShowProjectInfo(uprojectPath);
    } else if (command == "build") {
        BuildProject(uprojectPath);
    } else if (command == "package" && argc == 4) {
        std::string outputPath = argv[3];
        PackageProject(uprojectPath, outputPath);
    } else {
        std::cerr << "Commande inconnue ou paramètres incorrects." << std::endl;
        return 1;
    }
    
    return 0;
}
