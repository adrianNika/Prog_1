// main1.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//
#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>

using namespace std;

int selectMot = 0;

string motUtilisateur;
string* motMelange(nullptr);

string tableMotMystere[10] = { "grammaire", "electronique", "avion", "ordinateur", "transistor", "condensateur", "resistance", "ecole", "atelier", "microcontrolleur" };

void melangerLettres(string* mot)
{
    string melange;
    int position(0);


    //Tant qu'on n'a pas extrait toutes les lettres du mot
    while (mot->size() != 0)
    {
        //On choisit un numéro de lettre au hasard dans le mot
        position = rand() % mot->size();
        //On ajoute la lettre dans le mot mélangé
        melange += mot->at(position);
        //On retire cette lettre du mot mystère
        //Pour ne pas la prendre une deuxième fois
        mot->erase(position, 1);
    }
    //On assigne le mot mélangé au pointeur
    *mot = melange;
}

int main()
{
    cout << "Saisissez un mot Mystere entre 0 et 9 : ";
    cin >> selectMot;

    motMelange = new string(tableMotMystere[selectMot]);

    melangerLettres(motMelange);

    while (motUtilisateur != tableMotMystere[selectMot])
    {
        cout << "Quel est ce mot ? " << *motMelange << endl;
        cin >> motUtilisateur;

        if (motUtilisateur == tableMotMystere[selectMot])
        {
            cout << "*****Bravo !*****" << endl;
        }
        else
        {
            cout << "Ce n'est pas le mot !\n" << endl;
        }
    }
   
    delete motMelange;

    return 0;
}
