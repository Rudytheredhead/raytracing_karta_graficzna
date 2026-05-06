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

bool sprawdz_kolizje(const Wektor3D& nowa_pozycja, float promien_gracza, const std::vector<Kula_struct>& kule) {
    for (const auto kula:kule){
        Wektor3D srodek_kuli (kula.srodek.x,kula.srodek.y,kula.srodek.z);
        Wektor3D wektor_odleglosci = nowa_pozycja - srodek_kuli;
        float dystans_kwadrat = wektor_odleglosci.modul2();

        float min_dystans = promien_gracza + kula.promien;
        if(dystans_kwadrat< min_dystans*min_dystans){
            std::cout<<dystans_kwadrat<<std::endl;
            return true;
        }
    }
    return false;
}

// Zakładam, że w klasie Wektor3D masz zaimplementowane odejmowanie wektorów 
// oraz metodę Dlugosc() zwracającą wynik pierwiastkowania.

void Kolizje_z_kulami(Wektor3D& posPostaci, float promienPostaci,const std::vector<Kula_struct>& kule) {
    for (const auto kula:kule){
        Wektor3D srodek_kuli (kula.srodek.x,kula.srodek.y,kula.srodek.z);
        Wektor3D roznica = posPostaci - srodek_kuli;
        float sumaPromieni = promienPostaci + kula.promien;

        // Szybki test: odległość do kwadratu vs suma promieni do kwadratu
        float odlegloscKwadrat = roznica.modul2();
        float sumaPromieniKwadrat = sumaPromieni * sumaPromieni;

        if (odlegloscKwadrat < sumaPromieniKwadrat) {
            // Kule się przecinają! Teraz liczymy dokładny pierwiastek, 
            // aby zagwarantować bezbłędną i stabilną reakcję na kolizję.
            float odleglosc = roznica.modul();
            
            float glebokoscPenetracji = sumaPromieni - odleglosc;
            
            // Zabezpieczenie przed dzieleniem przez zero dla obiektów w dokładnie tym samym punkcie
            if (odleglosc > 0.0001f) {
                Wektor3D wektorNormalny;
                wektorNormalny.set_x( roznica.x() / odleglosc);
                wektorNormalny.set_y(  roznica.y() / odleglosc);
                wektorNormalny.set_z(roznica.z() / odleglosc);

       
                posPostaci = posPostaci + glebokoscPenetracji*wektorNormalny;
            }
        }
}   
}

