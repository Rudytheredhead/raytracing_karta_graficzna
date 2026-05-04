#version 130

#define szerokosc_czolowki  0.8
#define MAX_KUL 10
#define MAX_SWIATEL 5
//#define t_min 0.001

struct Uklad_wspolrzednych{
    vec3 W;
    vec3 U;
    vec3 V;

};

struct Promien{
    vec3 kierunek;
    vec3 poczatek;
};

struct Kula{

    vec3 srodek;
    float promien;
    vec3 kolor;
    float lustrzanosc;
    float przezroczystosc;
    float wspolczynnik_zalamania;
    float moc_emisji;
};


struct Zrodlo_swiatla{
    vec3 srodek;
    vec3 kolor;
    float moc_emisji;
    vec3 kierunek_swiecenia;
    float kat_swiecenia; 
};

struct Wyniki_zdarzenia{
    bool trafienie;
    float t;
    vec3 wektor_normalny;
    vec3 punkt_zderzenia;
    vec3 kolor;
    Promien promien_odbity;
    float lustrzanosc;
    float moc_emisji;

};

bool sprawdz_trafienie(Promien promien,Kula kula, inout Wyniki_zdarzenia wyniki, float t_min, float t_max){
    vec3 OC = promien.poczatek - kula.srodek;
    float a = dot(promien.kierunek,promien.kierunek);
    float b =  2.0*dot(OC,promien.kierunek);
    float c = dot(OC,OC) - kula.promien*kula.promien;
    float delta = b*b - 4.0*a*c;

    if(delta < 0 ) return false;

    float delta_sqrt = sqrt(delta);
    float t = (-b - delta_sqrt)/(2.0*a);
    if ( t<t_min || t>t_max){
        t =  (-b + delta_sqrt)/(2.0*a);
        if ( t<t_min ||t> t_max){
            return false;
        }
    }
    
    
    wyniki.t = t;
    wyniki.punkt_zderzenia = promien.poczatek + t*promien.kierunek;
    vec3 normalna = wyniki.punkt_zderzenia - kula.srodek;
    wyniki.wektor_normalny = normalize(normalna);
    wyniki.kolor = kula.kolor;
    wyniki.trafienie =true;

    

    Promien promien_odbity;
    promien_odbity.kierunek = normalize(reflect(promien.kierunek,wyniki.wektor_normalny));
    promien_odbity.poczatek = wyniki.punkt_zderzenia;

    wyniki.promien_odbity = promien_odbity;
    wyniki.lustrzanosc = kula.lustrzanosc;
    wyniki.moc_emisji = kula.moc_emisji;




    return true;
    
};

vec3 oblicz_kierunek_promienia(vec2 st, float odleglosc_od_ekranu, inout Uklad_wspolrzednych uklad){
    float grid_x = st.x*2.0 -1.0;
    float grid_y = st.y*2.0 -1.0;
    vec3 kierunek =normalize ((grid_x*uklad.U) - (grid_y*uklad.V) - (odleglosc_od_ekranu*uklad.W));
    return kierunek;

}




float oblicz_moc_czo(vec3 kierunek_promienia, vec3 przod){
    float kat_czolowki = dot(kierunek_promienia, przod);
    if (kat_czolowki>szerokosc_czolowki){
        float wynik = (kat_czolowki-szerokosc_czolowki)/(1.0-szerokosc_czolowki);
        return wynik;
    }
    else return 0.0;
}
float cieniownie_powieszchni(inout Wyniki_zdarzenia wyniki, vec3 kierunek_promienia){
    float natezenie = dot(wyniki.wektor_normalny,kierunek_promienia);
    natezenie = max(0.0, natezenie);
    return natezenie;
}

vec4 oblicz_oswietlenie(Wyniki_zdarzenia zderzenie,Zrodlo_swiatla swiatla[MAX_SWIATEL], int ilosc_swiatel, Kula kule[MAX_KUL],int ilosc_kul){
    vec3 calkowite_energia_swiatla = vec3(0.0,0.0,0.0);

    for (int i =0; i<ilosc_swiatel; i++){
        vec3 droga_swiatla = swiatla[i].srodek - zderzenie.punkt_zderzenia;
        float odleglosc_od_swiatla = length(droga_swiatla);
        vec3 kierunek_do_swiatla = normalize(droga_swiatla);
        float moc_swiatla_od_kata = 1.0;

        Promien promien_cienia;
        promien_cienia.poczatek = zderzenie.punkt_zderzenia;
        promien_cienia.kierunek = kierunek_do_swiatla;

        bool w_cieniu = false;
        Wyniki_zdarzenia wyniki_cien;
        float t_min = 0.001;

        for (int j =0; j<ilosc_kul;j++){
            if(sprawdz_trafienie(promien_cienia, kule[j], wyniki_cien, t_min, odleglosc_od_swiatla)){
                w_cieniu =true;
                break;
            }
        }
        if(!w_cieniu){
            if(swiatla[i].kat_swiecenia>-1.0){
                moc_swiatla_od_kata =0.0;
                float kat_swiatla = dot(-kierunek_do_swiatla, swiatla[i].kierunek_swiecenia);
                if (kat_swiatla>swiatla[i].kat_swiecenia){
                    moc_swiatla_od_kata = (kat_swiatla- swiatla[i].kat_swiecenia)/(1.0-swiatla[i].kat_swiecenia);
                }
            }

            float natezenie = max(0.0, dot(zderzenie.wektor_normalny, kierunek_do_swiatla));
            float tlumienie = 1.0/(odleglosc_od_swiatla*odleglosc_od_swiatla);
            calkowite_energia_swiatla += swiatla[i].kolor*swiatla[i].moc_emisji*natezenie*tlumienie*moc_swiatla_od_kata;
            
            
        }
   



    }
    return vec4(zderzenie.kolor*calkowite_energia_swiatla, calkowite_energia_swiatla*zderzenie.lustrzanosc);
   
    
    
}



uniform  int ilosc_kul;
uniform Kula kule[MAX_KUL];
uniform float odleglosc_od_ekranu;
uniform vec3 W;
uniform vec3 U;
uniform vec3 V;
uniform vec3 kamera;
uniform vec2 u_resolution; 

uniform int ilosc_swiatel;
uniform Zrodlo_swiatla swiatla[MAX_SWIATEL];

out vec4 outColor;





void main(){
    
    vec2 st = gl_FragCoord.xy / u_resolution.xy;
    Wyniki_zdarzenia wyniki;
    
    wyniki.t = 9999.0;
    Promien promien_swiatla;
    float t_min = 0.001;

    

    Uklad_wspolrzednych uklad;
    uklad.W = W;
    uklad.U = U;
    uklad.V = V;
    int idx_trafionej_kuli;

    vec3 kolor_powieszchni= vec3(0.0,0.0,0.0);
    vec3 wynikowy_kolor = vec3(0.0,0.0,0.0);

    

    bool czy_cos_zostalo_trafione = false;
    promien_swiatla.kierunek = oblicz_kierunek_promienia(st, odleglosc_od_ekranu,uklad );
    promien_swiatla.poczatek = kamera;
    
    Wyniki_zdarzenia wyniki_odbicia;
    bool czy_odbite = false;
    float maska_blasku =0.0;
    for (int i =0;i<ilosc_kul;i++ ){
        
        if(sprawdz_trafienie(promien_swiatla, kule[i], wyniki,t_min,wyniki.t))
        {   
            
            czy_cos_zostalo_trafione = true;  
            idx_trafionej_kuli =i;
        }
    }
    if (czy_cos_zostalo_trafione){
        vec4 matowy_wynik = oblicz_oswietlenie(wyniki,swiatla, ilosc_swiatel, kule, ilosc_kul);
        maska_blasku += matowy_wynik.a;
        vec3 kolor_matowy = matowy_wynik.rgb;

        vec3 kolor_odbicia = vec3(0.0);
        if (wyniki.lustrzanosc>0.0){
            
            wyniki_odbicia.t = 9999.0;
            czy_odbite = false;
            for (int i=0;i<ilosc_kul;i++){
                if(sprawdz_trafienie(wyniki.promien_odbity, kule[i], wyniki_odbicia, t_min, wyniki_odbicia.t)){
                    czy_odbite = true;
                }
            }

            if(czy_odbite){
                vec4 odbicie_wynik = oblicz_oswietlenie(wyniki_odbicia,swiatla, ilosc_swiatel, kule, ilosc_kul);
            
                kolor_odbicia = odbicie_wynik.rgb;
                maska_blasku += odbicie_wynik.a;
            }
           
        }

        kolor_powieszchni = mix(kolor_matowy, kolor_odbicia, wyniki.lustrzanosc);
        
        kolor_powieszchni += wyniki.kolor*wyniki.moc_emisji;
        

        kolor_powieszchni = kolor_powieszchni/(kolor_powieszchni + vec3(1.0));
        
        
    }
    else{
        kolor_powieszchni = vec3(0.0);
    }
    
    
    
    for (int i = 0; i<ilosc_swiatel;i++){
    vec3 kierunek_do_swiatla = normalize(swiatla[i].srodek-kamera);
    vec3 odchylenie = promien_swiatla.kierunek - kierunek_do_swiatla;

    float zgnicenie = 3.0;
    odchylenie += swiatla[i].kierunek_swiecenia*dot(odchylenie, swiatla[i].kierunek_swiecenia)*zgnicenie;


    float korelacja = max(0.0, 1.0 - length(odchylenie));
   
    float sila = pow(korelacja,20.0);
    float moc_swiatla_od_kata = 1.0;
    if(swiatla[i].kat_swiecenia>-1.0){
        moc_swiatla_od_kata =0.0;
        float kat_swiatla = dot(-kierunek_do_swiatla, swiatla[i].kierunek_swiecenia);
        if (kat_swiatla>swiatla[i].kat_swiecenia){
            moc_swiatla_od_kata = (kat_swiatla- swiatla[i].kat_swiecenia)/(1.0-swiatla[i].kat_swiecenia);
        }
    }
    maska_blasku += sila*swiatla[i].moc_emisji*moc_swiatla_od_kata;

 

    }
    outColor = vec4(kolor_powieszchni,maska_blasku);
    



}