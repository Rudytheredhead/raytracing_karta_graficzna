#pragma once
#include "Wektor3D.h"
#include <SFML/Graphics.hpp>
#include<string>

struct WynikZdarzenia{
    bool trafienie;
    float t ;
    Wektor3D wektor_normalny;
    Wektor3D punkt_udzenia; 
    Wektor3D kolor;

};

struct Kula_struct{

    sf::Vector3f srodek;
    float promien;
    sf::Vector3f kolor;
    float lustrzanosc;
    float przezroczystosc;
    float wspolczynnik_zalamania;
    float moc_emisji;
};
struct Zrodlo_swiatla{
    sf::Vector3f srodek;
    sf::Vector3f kolor;
    float moc_emisji;
};



class Obiekt3D{
protected:
    Wektor3D kolor_;
    std::string nazwa_;
public:
    Obiekt3D(Wektor3D kolor,std::string nazwa =""):kolor_(kolor),nazwa_(nazwa){}
    virtual ~Obiekt3D() = default;
    virtual bool sprawdz_trafienie(const Wektor3D & promien, const Wektor3D &kamera, WynikZdarzenia& wyniki,float t_min, float t_max) const = 0;
    std::string zwroc_string(){return nazwa_;}
    

};


class Kula: public Obiekt3D{
private:
    Wektor3D srodek_;
    float promien_; 
   

    
public:
    Kula (Wektor3D kolor, Wektor3D srodek, float promien): Obiekt3D(kolor,"kula"), srodek_(srodek), promien_(promien){};
    bool sprawdz_trafienie(const Wektor3D & kierunek,const Wektor3D &kamera,  WynikZdarzenia& wyniki,float t_min, float t_max) const;
    sf::Vector3f get_srodek(){return srodek_.zwroc_vector2f();}
    sf::Vector3f get_kolor(){return kolor_.zwroc_vector2f();}
    float get_promien(){return promien_;}


   
    ~Kula() =default;
    
};