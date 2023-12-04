#include <numbers>

/*
   Various generators for randomly distributed 3D vectors.
   
   For 2D objects we set z to zero.

*/

// Random point in a spherical shell Ri to Ro
void RandomSolidShell(double& Ri, double&Ro, double &x, double &y, double &z, auto& dist)
{
    double R;
    do
      {
        x= Ro*dist(re);
        y= Ro*dist(re);
        z= Ro*dist(re);
        
        R=sqrt(x*x+y*y+z*z);
      }
    while (R>Ro || R<Ri);
}

// Random point inside sphere
void RandomSphere(double R, double &x, double &y, double &z, auto& dist)
{
    double d2;
    do
      {
        x= R*dist(re);
        y= R*dist(re);
        z= R*dist(re);
        d2=x*x+y*y+z*z;
      }
    while (d2>R*R);
}

// Random point on surface of sphere with R
void RandomSurface(double R, double &x, double &y, double &z, auto& dist)
{
    double d2;
    do
      {
        x= R*dist(re);
        y= R*dist(re);
        z= R*dist(re);
        d2=x*x+y*y+z*z;
      }
    while (d2>R*R || d2<1e-5);
    
    x*=R/sqrt(d2);
    y*=R/sqrt(d2);
    z*=R/sqrt(d2);
}

// Random point on a circular disk (x,y) with radius R    z=0
void RandomDisk(double R, double &x, double &y, double &z, auto& dist)
{
    z= 0;
    double d2;
    do
      {
        x= R*dist(re);
        y= R*dist(re);
        d2=x*x+y*y;
      }
    while (d2>R*R || d2<1e-5);
}



// Random point on a circle with radius R
void RandomCircle(double R, double &x, double &y, double &z, auto& dist)
{
    double theta=std::numbers::pi*dist(re);

    x=R*cos(theta);
    y=R*sin(theta);
    z=0;
}


// Random point inside cyllinder.
void RandomSolidCylinder(double L, double R, double &x, double &y, double &z, auto& dist)
{
    double d2;
    do
      {
        x= R*dist(re);
        y= R*dist(re);
        d2=x*x+y*y;
      }
    while (d2>R*R);
    z= L/2*dist(re);
}

// Random point on a circle with radius R
void RandomCylinderHull(double L, double R, double &x, double &y, double &z, auto& dist)
{
    double theta=std::numbers::pi*dist(re);

    x=R*cos(theta);
    y=R*sin(theta);
    z=L/2*dist(re);
}

