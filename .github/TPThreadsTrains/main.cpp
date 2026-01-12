#include <iostream>           
#include <thread>             
#include <mutex>              
#include <condition_variable> 
#include "Controleur.hpp" 

std::mutex mtx;
std::condition_variable cv;
Controleur ctrl(0);   // LE PROCESSUS CONTROLEUR

//il faut gérer la synchronisation pour qu'il n'y ait pas de collision
//et que des trains puissent circuler simultanément dans le même sens
//pour cela on utilise un mutex et une variable de condition
//le controleur gère les autorisations de circuler
//il faut aussi gérer l'exclusion mutuelle pour la circulation dans les deux sens
//pour cela on utilise un mutex mtx;
//on place le mutex mtx lors de la circulation des trains dans un sens comme cela on est sûr qu'aucun train ne circule dans l'autre sens
//Cette section correspond à la section critique ligne 23 à 29 et ligne 54 à 60

// TrainAB 
void circuleAB(int numTrain){
  std::cout << "Train n° " << numTrain << " arrive en A vers B " << endl;

  //mtx.lock();
  this_thread::sleep_for(chrono::milliseconds(rand() % 1000) );
  std::unique_lock<std::mutex> lck(mtx); //verrouillage du mutex c'est-à-dire demande d'accès au tronçon
    cv.wait(lck,[numTrain]{return ctrl.controlinEnA(numTrain);}); // ATTENTE D'AUTORISATION DE CIRCULER 
    lck.unlock();
  //mtx.unlock();

  // DEBUT DU PARCOURS A->B
  std::cout << "Train n° "<< numTrain << " circule de A vers B  >>>>>> " << endl;
  this_thread::sleep_for(chrono::milliseconds(rand() % 100) );

  // FIN DU PARCOURS A->B
  std::cout << "Train n° " << numTrain << " quitte le tronçon de voie unique " << endl;
  ctrl.controloutEnB(numTrain);  // SIGNAL DE SORTIE AU CONTROLEUR
  cv.notify_all();
}


// TrainBA 
void circuleBA(int numTrain){
  std::cout << "Train n° " << numTrain << " arrive en B vers A " << endl;
  // mtx.lock();
  this_thread::sleep_for(chrono::milliseconds(rand() % 1000) );
  std::unique_lock<std::mutex> lck(mtx);
  cv.wait(lck,[numTrain]{return ctrl.controlinEnB(numTrain);}); // ATTENTE D'AUTORISATION DE CIRCULER
  lck.unlock();
  // mtx.unlock();

  // DEBUT DU PARCOURSs B->A
  std::cout << "Train n° "<< numTrain << " circule de B vers A  <<<<<<<<" << endl;
  this_thread::sleep_for(chrono::milliseconds(rand() % 100) );
  // FIN DU PARCOURS B->A
  std::cout << "Train n° " << numTrain << " quitte le tronçon de voie unique " << endl;
   lck.lock();
  ctrl.controloutEnA(numTrain);   // SIGNAL DE SORTIE AU CONTROLEUR
   lck.unlock();
  cv.notify_all();

}

int main(){
  const int nbr = 9;
  std::srand(std::time(nullptr));
  std::thread trainsAB[nbr];
  std::thread trainsBA[nbr];

  // INITALISE TRAINS ET CONTROLEUR
  for (int i = 1; i < nbr; i++){
    trainsAB[i] = std::thread(circuleAB, i);
    trainsBA[i] = std::thread(circuleBA, -i);
  }
// JOIN DES THREADS 
  for (int i = 1; i < nbr; i++) {
    if (trainsAB[i].joinable()) trainsAB[i].join(); 
    if (trainsBA[i].joinable()) trainsBA[i].join();   
  }
  return 0;
}

