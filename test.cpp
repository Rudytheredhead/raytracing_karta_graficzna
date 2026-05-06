#include <SFML/Graphics.hpp>
#include "Matematyka.h"
#include "Bryly.h"
#include "Wektor3D.h"
#include <vector>
#include <memory>
#include<cmath>
#include <iostream>
#include <chrono>



int main() {
    // 1. Tworzenie okna: (rozdzielczość, tytuł)
    sf::RenderWindow window(sf::VideoMode(DLUGOSC,DLUGOSC), "Moje Okno SFML");
    window.setFramerateLimit(60); 
    window.setMouseCursorVisible(false);

    sf::RectangleShape ekran(sf::Vector2f(window.getSize()));


    std::vector<Kula_struct> kule;
    Kula_struct kula{sf::Vector3f(0.0f,0.0f,-5.0f), 
        2.0f,
        sf::Vector3f(0.0f,1.0f,0.0f),
        0.0f,
        0.0f,
        0.0f,
        0.0f
    };
    Kula_struct kula2{sf::Vector3f(6.0f,0.0f,-5.0f), 
        2.0f,
        sf::Vector3f(1.0f,0.0f,0.0f),
        0.95f,
        0.0f,
        0.0f,
        0.0f

    };
    // Kula_struct czolowka{sf::Vector3f(0.0f,-0.5f,0.0f), 
    //     0.2f,
    //     sf::Vector3f(1.0f,1.0f,1.0f),
    //     0.0f,
    //     0.0f,
    //     0.0f,
    //     1.0f
    // };
    kule.push_back(kula);
    kule.push_back(kula2);
    //kule.push_back(czolowka);

    std::vector<Zrodlo_swiatla> zrodla_swiatla;

    Zrodlo_swiatla czolowka_{sf::Vector3f(0.0f,-0.5f,-0.5f),
        sf::Vector3f(1.0f,1.0f,1.0f),
        1.0f
    };
    czolowka_.kat_swiecenia = 0.8f;

    Zrodlo_swiatla lampa{sf::Vector3f(0.0f,-10.0f,-5.0f),
        sf::Vector3f(1.0f,1.0f,1.0f),
        30.0f
    };
    lampa.kierunek_swiecena = sf::Vector3f(0.0f,1.0f,0.0f);
    lampa.kat_swiecenia = 0.5f;
    zrodla_swiatla.push_back(czolowka_);
    zrodla_swiatla.push_back(lampa);




    Wektor3D kamera(0.0f,0.0f,0.0f);
    Wektor3D cel(0.0f,0.0f,-1.0f);
    Wektor3D gora(0.0f,1.0f,0.0f);
    Wektor3D swiatlo(0.0f,1.0f,2.0f);
    Wektor3D predkosc_wektor (0.0f,0.0f,0.0f);
    bool czy_stoi_na_ziemi = true;
    float odleglosc_od_ekranu =1.0f;
    float predkosc_chodzenia = 5.0f;
    float sens_myszki = 0.005f;
    int dx=0;
    int dy=0;
    float obrot_na_boki=0.0f;
    float obrot_gora_dol = 0.0f;
    Wektor3D grawitacja(0.0f,9.81f,0.0f);
    sf::Mouse::setPosition(sf::Vector2i(DLUGOSC/2, DLUGOSC/2), window);
    czolowka_.kierunek_swiecena = cel.zwroc_vector2f();

    auto poprzedniCzas = std::chrono::high_resolution_clock::now();
    float deltaTime = 0.0f;


    
    




    

    if (!sf::Shader::isAvailable()) {
        std::cerr << "Shadery nie sa wspierane!" << std::endl;
        return -1;
    }
    sf::Shader shader_obliczenia;
    
    if (!shader_obliczenia.loadFromFile("fizyka.frag", sf::Shader::Fragment)) {
        std::cerr << "Nie udalo sie zaladowac shadera!" << std::endl;
        return -1;
    }

    sf::Shader shader_postprocess;
    if (!shader_postprocess.loadFromFile("post_process.frag", sf::Shader::Fragment)) {
        std::cerr << "Nie udalo sie zaladowac shadera postprocess.frag!" << std::endl;
        return -1;
    }

    shader_obliczenia.setUniform("ilosc_kul",static_cast<int>(kule.size()));
    shader_obliczenia.setUniform("ilosc_swiatel",static_cast<int>(zrodla_swiatla.size()));

    shader_obliczenia.setUniform("odleglosc_od_ekranu",odleglosc_od_ekranu);
    shader_obliczenia.setUniform("kamera",kamera.zwroc_vector2f());
    shader_obliczenia.setUniform("u_resolution", sf::Vector2f(window.getSize()));

    for(int i =0; i<kule.size();i++){
        
        std::string prefix = "kule["+std::to_string(i)+"].";
        shader_obliczenia.setUniform(prefix+"srodek", kule[i].srodek);
        shader_obliczenia.setUniform(prefix+"promien", kule[i].promien);
        shader_obliczenia.setUniform(prefix+"kolor", kule[i].kolor);
        shader_obliczenia.setUniform(prefix+"lustrzanosc", kule[i].lustrzanosc);
        shader_obliczenia.setUniform(prefix+"moc_emisji", kule[i].moc_emisji);
        shader_obliczenia.setUniform(prefix+"przezroczystosc", kule[i].przezroczystosc);
        shader_obliczenia.setUniform(prefix+"wspolczynnik_zalamania", kule[i].wspolczynnik_zalamania);

    }
    for(int i =0; i<zrodla_swiatla.size();i++){
        std::string prefix = "swiatla["+std::to_string(i)+"].";
        shader_obliczenia.setUniform(prefix+"srodek", zrodla_swiatla[i].srodek);
        shader_obliczenia.setUniform(prefix+"moc_emisji", zrodla_swiatla[i].moc_emisji);
        shader_obliczenia.setUniform(prefix+"kolor", zrodla_swiatla[i].kolor);
        shader_obliczenia.setUniform(prefix+"kierunek_swiecenia", zrodla_swiatla[i].kierunek_swiecena);
        shader_obliczenia.setUniform(prefix+"kat_swiecenia", zrodla_swiatla[i].kat_swiecenia);



    }



    Uklad_wspolrzednych uklad = obliczanie_ukladu_wspolrzednych(kamera,cel,gora);
    shader_obliczenia.setUniform("W", uklad.W.zwroc_vector2f());
    shader_obliczenia.setUniform("U", uklad.U.zwroc_vector2f());
    shader_obliczenia.setUniform("V", uklad.V.zwroc_vector2f());
  

    // Ustawiamy zmienne uniform dla drugiego shadera
    shader_postprocess.setUniform("u_tekstura", sf::Shader::CurrentTexture);
    shader_postprocess.setUniform("u_resolution", sf::Vector2f(window.getSize()));
    // Tworzymy niewidzialne płótno, na którym będzie "malował" pierwszy shader
    sf::RenderTexture plotno_robocze;
    plotno_robocze.create(DLUGOSC, DLUGOSC);
    sf::Sprite sprite_postprocess(plotno_robocze.getTexture());
    




    // 2. Główna pętla programu - działa tak długo, jak okno jest otwarte
    while (window.isOpen()) {
        sf::Event event;
    

        // 3. Sprawdzanie zdarzeń (np. kliknięcie krzyżyka)
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) window.close();

        }
        //obliczananie delta_time zeby predkosci nie zmienialy sie przy zmiuennej ilosci klatek

        auto obecnyCzas = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> roznica = obecnyCzas - poprzedniCzas;
        deltaTime = roznica.count();
        poprzedniCzas = obecnyCzas;


        Wektor3D przod(
            -std::sin(obrot_na_boki),
            0.0f,
            -std::cos(obrot_na_boki)
        );
        
        przod.normalizuj();
        Wektor3D prawo = przod%gora;
        prawo.normalizuj();
        Wektor3D przesuniecie(0.0f,0.0f,0.0f);
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            przesuniecie = przesuniecie + predkosc_chodzenia*deltaTime*prawo;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            przesuniecie = przesuniecie- predkosc_chodzenia*deltaTime*prawo;
        }        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            przesuniecie = przesuniecie + predkosc_chodzenia*deltaTime*przod;
        }        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            przesuniecie = przesuniecie - predkosc_chodzenia*deltaTime*przod;

        }
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && czy_stoi_na_ziemi){
            predkosc_wektor = predkosc_wektor + Wektor3D(0.0f,-10.0f,0.0f);
            czy_stoi_na_ziemi = false;
        }
        if (!czy_stoi_na_ziemi){
            predkosc_wektor = predkosc_wektor + (deltaTime*grawitacja);
            
            
        }
        czy_stoi_na_ziemi = false;
        

        przesuniecie = przesuniecie + (deltaTime*predkosc_wektor);
        kamera = kamera + przesuniecie;
        
        // Jeżeli gracz próbuje się poruszyć
        if (przesuniecie.modul2() > 0.0f) {
            float promien_postaci = 0.5f;
            Kolizje_z_kulami(kamera, promien_postaci, kule);
                
             
        }
        

        if (kamera.y()>0.0f){
            
            kamera.set_y(0.0f);
            predkosc_wektor.set_y(0.0f);
            czy_stoi_na_ziemi = true;
            
        }
      



        if (window.hasFocus()){
            sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
            dx = -(mouse_pos.x-DLUGOSC/2);
            dy = mouse_pos.y-DLUGOSC/2;
            sf::Mouse::setPosition(sf::Vector2i(DLUGOSC/2,DLUGOSC/2),window);
            
        }
        obrot_na_boki += dx*sens_myszki;
        obrot_gora_dol =std::clamp(obrot_gora_dol- dy*sens_myszki,-1.55f,1.55f);
        Wektor3D kierunek_kamery(
            -std::cos(obrot_gora_dol)*std::sin(obrot_na_boki),
            -std::sin(obrot_gora_dol),
            -std::cos(obrot_gora_dol)*std::cos(obrot_na_boki));
        cel = kamera +kierunek_kamery;
        
        dx=0;
        dy =0;









        


        



        uklad = obliczanie_ukladu_wspolrzednych(kamera,cel,gora);
        shader_obliczenia.setUniform("W", uklad.W.zwroc_vector2f());
        shader_obliczenia.setUniform("U", uklad.U.zwroc_vector2f());
        shader_obliczenia.setUniform("V", uklad.V.zwroc_vector2f());
        shader_obliczenia.setUniform("kamera",kamera.zwroc_vector2f());

        Wektor3D pozycja_czolowki = kamera + 0.5f*uklad.V;
        Wektor3D pozycja_czolowki_swiatla = pozycja_czolowki + 0.5f*uklad.W;
        // std::cout<<pozycja_czolowki<<std::endl<<uklad.V<<std::endl;



        std::string prefix = "swiatla[0].";
        shader_obliczenia.setUniform(prefix+"srodek", pozycja_czolowki_swiatla.zwroc_vector2f());
        shader_obliczenia.setUniform(prefix+"kierunek_swiecenia", -uklad.W.zwroc_vector2f());

        // prefix = "kule["+std::to_string(kule.size()-1)+"].";
        // shader_obliczenia.setUniform(prefix+"srodek", pozycja_czolowki.zwroc_vector2f());



        plotno_robocze.clear(sf::Color(0,0,0,0));
        sf::RenderStates stany_obliczen;
        stany_obliczen.shader = &shader_obliczenia;
        stany_obliczen.blendMode = sf::BlendNone;
        plotno_robocze.draw(ekran, stany_obliczen);
        plotno_robocze.display();
    

    
        // 4. Renderowanie (Czyszczenie -> Rysowanie -> Wyświetlanie)
        window.clear(sf::Color::Black); 
        
        // Tutaj miejsce na rysowanie obiektów (window.draw...)
        window.draw(sprite_postprocess ,&shader_postprocess);
      

        window.display();
    }

    return 0;
}