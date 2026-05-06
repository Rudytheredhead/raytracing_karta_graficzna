#ifndef A_h
#define A_h
#include <iostream>
#include <SFML/Graphics.hpp>

class Wektor3D {
private:
    float x_, y_, z_;
public:
    Wektor3D(float x = 0, float y = 0, float z = 0);
    bool operator == (const Wektor3D& wektor) const;
    float operator * (const Wektor3D& wektor) const; 
    Wektor3D operator % (const Wektor3D& wektor) const;
    Wektor3D operator +(const Wektor3D& wektor) const;
    Wektor3D operator -(const Wektor3D& wektor) const;
    float modul() const;
    float modul2() const;
    float x() const;
    float y() const;
    float z() const;
    friend std::ostream& operator << (std::ostream& os, const Wektor3D& wektor);
    friend std::istream& operator >> (std::istream& is, Wektor3D& wektor);
    friend Wektor3D operator * (float scalar, const Wektor3D& wektor);
    void normalizuj();
    sf::Vector3f zwroc_vector2f(){return sf::Vector3f(x_,y_,z_);}
    void set_x(float x);
    void set_y(float y);
    void set_z(float z);

    
};

#endif
