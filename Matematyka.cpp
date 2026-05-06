#include"Wektor3D.h"
#include"Matematyka.h"
#include <SFML/Graphics.hpp>


Uklad_wspolrzednych obliczanie_ukladu_wspolrzednych( const Wektor3D& kamera,const  Wektor3D &cel, const Wektor3D & gora){
    Wektor3D W = kamera-cel;
    W.normalizuj();//przod tyl
    Wektor3D U = gora%W;
    U.normalizuj();//prawo lewo
    Wektor3D V = W%U;//gora _dol
    V.normalizuj();
    Uklad_wspolrzednych uklad{W,U,V};
    return uklad;
}

Wektor3D oblicz_kierunek_promienia(float grid_x, float grid_y,float odleglosc_od_ekranu, Uklad_wspolrzednych &uklad ){
    grid_x/=DLUGOSC;
    grid_y/=DLUGOSC;
    grid_x = grid_x*2.0f- 1.0f;
    grid_y = grid_y*2.0f- 1.0f;

    
    //tworzymy nowy uklad wspolrzednych (ten z ktorego patrzy kamera)
    Wektor3D kierunek = (grid_x*uklad.U)  - (grid_y*uklad.V) - (odleglosc_od_ekranu*uklad.W);
    kierunek.normalizuj();
    return kierunek;
    
    
}



