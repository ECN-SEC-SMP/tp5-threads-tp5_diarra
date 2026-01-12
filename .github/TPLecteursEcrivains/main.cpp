#include <iostream>           
#include <thread>             
#include "Semaphore.hpp"

int ressourcePartagee;
int nombreLecteurs=0;

//création des sémaphores
Semaphore mutex_m(1); //mutex pour l'accès à la ressource partagée
Semaphore ecrit(1); //sémaphore pour l'accès des écrivains

void lecteur(int numLecteur){
  for (int i = 0; i < 4; i++){
    std::cout << "Lecteur n° " << numLecteur << " en cours " << endl;
    this_thread::sleep_for(chrono::milliseconds(rand() % 20000) );

    mutex_m.P(nombreLecteurs);
    nombreLecteurs++; //un lecteur de plus
    if(nombreLecteurs == 1){
      ecrit.P(numLecteur);
    } //le premier lecteur bloque les écrivains
    mutex_m.V(nombreLecteurs); //Permet aux écrivains d'accéder au mutex
    //section de lecture
    std::cout << "        Valeur lue = " << ressourcePartagee << "  " << endl;
    mutex_m.P(nombreLecteurs);
    nombreLecteurs--;
    if(nombreLecteurs == 0){
      ecrit.V(numLecteur);} //le dernier lecteur débloque les écrivains
    mutex_m.V(nombreLecteurs);
  }
}

void ecrivain(int numEcrivain){
  int x;
  for (int i = 0; i < 4; i++){
    std::cout << "Ecrivain n° " << numEcrivain << " en cours " << endl;
    x = ressourcePartagee;
    this_thread::sleep_for(chrono::milliseconds(rand() % 20000) );
    std::cout << "valeur à incrémenter de la ressourcePartagee = " << x << "  " << endl;

    ecrit.P(numEcrivain);
    ressourcePartagee = x+1 ;
    ecrit.V(numEcrivain);

  }
}

int main() {
  const int nbr = 8;
  std::thread r[nbr];
  std::thread w[nbr];
  int ressourcePartagee = 0;

  for (int i = 1; i < nbr; i++){
    r[i] = std::thread(lecteur, -i);
    w[i] = std::thread(ecrivain, i);
  }
// Join des threads 
  for (int i = 1; i < nbr; i++) {
  r[i].join(); 
  w[i].join();   
  }
  return 0;
}
