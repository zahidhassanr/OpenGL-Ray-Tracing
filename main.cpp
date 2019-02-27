/*
 * GLUT Shapes Demo
 *
 * Written by Nigel Stewart November 2003
 *
 * This program is test harness for the sphere, cone
 * and torus shapes in GLUT.
 *
 * Spinning wireframe and smooth shaded shapes are
 * displayed until the ESC or q key is pressed.  The
 * number of geometry stacks and slices can be adjusted
 * using the + and - keys.
 */


#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<bits/stdc++.h>
#include <windows.h>
#include <glut.h>
#include<fstream>
#include "bitmap_image.hpp"


#define pi (2*acos(0.0))

#define INFINITY 9999999

using namespace std;

double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;
int up;
int numObj;

struct point
{
    double x,y,z;
};

struct point pos,bullet;
point Normal[100];
double Near;
double Faar;
double fovY;
double aspectRatio;
double levelRec;
int numPixel;
double fovX;
int numNor,numSpot;
point **textureBuffer;
int height, width;
bool texure=false;
void drawSphere(double radius,int slices,int stacks)
{
    struct point points[100][100];
    int i,j;
    double h,r;
    //generate points
    for(i=0; i<=stacks; i++)
    {
        h=radius*sin(((double)i/(double)stacks)*(pi/2));
        r=radius*cos(((double)i/(double)stacks)*(pi/2));
        for(j=0; j<=slices; j++)
        {


            points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
            points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
            points[i][j].z=h;
        }
    }
    //draw quads using generated points
    for(i=0; i<stacks; i++)
    {
        //glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
        for(j=0; j<slices; j++)
        {


            glBegin(GL_QUADS);
            {
                //upper hemisphere
                glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
                glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
                glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
                glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
                //lower hemisphere
                glVertex3f(points[i][j].x,points[i][j].y,-(points[i][j].z));
                glVertex3f(points[i][j+1].x,points[i][j+1].y,-(points[i][j+1].z));
                glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-(points[i+1][j+1].z));
                glVertex3f(points[i+1][j].x,points[i+1][j].y,-(points[i+1][j].z));
            }
            glEnd();
        }
    }

}






class Vector
{
public:
    double x,y,z;
    Vector() {};
    Vector(double i,double j,double k)
    {
        x=i;
        y=j;
        z=k;
    }

    void normalize()
    {
        double sum=sqrt(x*x+y*y+z*z);
        x=x/sum;
        y=y/sum;
        z=z/sum;
    }

    static Vector put(double i,double j,double k)
    {
        Vector v(i,j,k);
        return v;
    }

};

Vector u(0,0,1);
Vector l(-1.0/sqrt(2),-1.0/sqrt(2.0),0);
Vector r(-1.0/sqrt(2),1.0/sqrt(2.0),0);



class NormalLight
{
public:

    point pos;
    double fall;
    NormalLight() {};
    NormalLight(point pos,double fall)
    {
        this->pos.x=pos.x;
        this->pos.y=pos.y;
        this->pos.z=pos.z;
        this->fall=fall;

    }
    void print()
    {

        glPushMatrix();
        {
            glTranslatef(pos.x,pos.y,pos.z);
            glColor3f(1,1,1);
            drawSphere(10,50,40);
        }
        glPopMatrix();
    }

};

NormalLight normalLight[50];

class SpotLight
{
public:

    point pos;
    double fall;
    point look;
    double cutt;
    SpotLight() {};
    SpotLight(point pos,double fall,point look,double cutt)
    {
        this->pos.x=pos.x;
        this->pos.y=pos.y;
        this->pos.z=pos.z;
        this->fall=fall;

        this->look.x=look.x;
        this->look.y=look.y;
        this->look.z=look.z;

        this->cutt=cutt;

    }
    void print()
    {

        glPushMatrix();
        {
            glTranslatef(pos.x,pos.y,pos.z);
            glColor3f(1,1,1);
            drawSphere(10,50,40);
        }
        glPopMatrix();
    }

};

SpotLight spotLight[50];

class Sphere
{
public:
    point center;
    double radius;
    point color;
    double ambient,diffuse,specular,refCof,shininess;
    Sphere() {};
    void put(point center,double radius,point color,double ambient,double diffuse,double specular,double refCof,double shinines)
    {
        this->center.x=center.x;
        this->center.y=center.y;
        this->center.z=center.z;

        this->radius=radius;



        this->ambient=ambient;
        this->diffuse=diffuse;
        this->specular=specular;
        this->refCof=refCof;
        this->shininess=shinines;

        this->color.x=color.x;
        this->color.y=color.y;
        this->color.z=color.z;
    }

    void print()
    {

        glPushMatrix();
        {
            glTranslatef(center.x,center.y,center.z);
            glColor3f(color.x,color.y,color.z);
            drawSphere(radius,50,40);
        }
        glPopMatrix();
    }


};

class Pyramid
{

public:
    point low;
    double width,height;
    point color;
    double ambient,diffuse,specular,refCof,shininess;
    Pyramid() {};

    void put(point low,double width,double height,point color,double ambient,double diffuse,double specular,double refCof,double shinines)
    {
        this->low.x=low.x;
        this->low.y=low.y;
        this->low.z=low.z;

        this->width=width;
        this->height=height;

        this->color.x=color.x;
        this->color.y=color.y;
        this->color.z=color.z;

        this->ambient=ambient;
        this->diffuse=diffuse;
        this->specular=specular;
        this->refCof=refCof;
        this->shininess=shinines;

    }

    void print()
    {

        double x=low.x-(width/2);
        double y=low.y-(width/2);

        glPushMatrix();
        {
            glColor3f(color.x,color.y,color.z);
            glBegin(GL_TRIANGLES);
            {
                glVertex3f(x,y,low.z);
                glVertex3f(x,y+width,low.z);
                glVertex3f(low.x,low.y,low.z+height);

            }
            glEnd();
            glBegin(GL_TRIANGLES);
            {

                glVertex3f(x,y+width,low.z);
                glVertex3f(x+width, y+width,low.z);
                glVertex3f(low.x,low.y,low.z+height);

            }
            glEnd();
            glBegin(GL_TRIANGLES);
            {
                glVertex3f(x+width, y+width,low.z);
                glVertex3f( x+width,y,low.z);
                glVertex3f(low.x,low.y,low.z+height);

            }
            glEnd();
            glBegin(GL_TRIANGLES);
            {
                glVertex3f( x, y,low.z);
                glVertex3f( x+width,y,low.z);
                glVertex3f(low.x,low.y,low.z+height);

            }
            glEnd();


            glBegin(GL_QUADS);
            {
                glVertex3f( x, y,low.z);
                glVertex3f(x,y+width,low.z);
                glVertex3f(x+width, y+width,low.z);
                glVertex3f( x+width,y,low.z);
            }
            glEnd();
        }
        glPopMatrix();
    }

};


class CheckerBoard
{
public:
    double width;
    double ambient,diffuse,refCof;
    point color;
    CheckerBoard() {};

    void put(double width,double ambient,double diffuse,double refCof)
    {
        this->width=width;
        this->ambient=ambient;
        this->diffuse=diffuse;
        this->refCof=refCof;

    }

    void print(double x,double y,int i,int j)
    {
        if((i+j)% 2==0)
        {
            color.x=0;
            color.y=0;
            color.z=0;
        }
        else
        {
            color.x=1;
            color.y=1;
            color.z=1;
        }
        glPushMatrix();
        {
            glColor3f(color.x,color.y,color.z);
            glBegin(GL_QUADS);
            {
                glVertex3f( x, y,0);
                glVertex3f(x,y+width,0);
                glVertex3f(x+width, y+width,0);
                glVertex3f( x+width,y,0);


            }
            glEnd();
        }
        glPopMatrix();
    }
};

CheckerBoard board;
//Sphere sphere1,sphere2;
//Pyramid pyramid;



class Generic
{
public:

    string name;
    Sphere sphere;
    Pyramid pyramid;


    Generic() {};

};

Generic Allobj[20];

Vector Cross(Vector V1, Vector V2)
{
    Vector result(V1.y*V2.z-V1.z*V2.y , V1.z*V2.x-V1.x*V2.z , V1.x*V2.y-V1.y*V2.x);
    return result;

}

double cross(Vector V1, Vector V2)
{
    double result=sqrt(pow(V1.y*V2.z-V1.z*V2.y,2) +pow(V1.z*V2.x-V1.x*V2.z,2)+pow(V1.x*V2.y-V1.y*V2.x,2));
    return result;

}

Vector Rotate(Vector Vr, Vector Vt,double angle)
{
    Vector n=Cross(Vr,Vt);
    n.normalize();
    angle=angle*pi/180;
    Vector result;
    result.x=n.x*sin(angle)+Vt.x*cos(angle);
    result.y=n.y*sin(angle)+Vt.y*cos(angle);
    result.z=n.z*sin(angle)+Vt.z*cos(angle);
    result.normalize();
    return result;
}


Vector Rotate1(Vector Vr, Vector Vt,double angle)
{
    Vector n=Cross(Vr,Vt);
    n.normalize();
    angle=angle*pi/180;

    Vector result;
    result.x=Vt.x*cos(angle)-n.x*sin(angle);
    result.y=Vt.y*cos(angle)-n.y*sin(angle);
    result.z=Vt.z*cos(angle)-n.z*sin(angle);
    result.normalize();
    return result;
}


void drawAxes()
{
    if(drawaxes==1)
    {
        glColor3f(1.0, 1.0, 1.0);
        glBegin(GL_LINES);
        {
            glVertex3f( 100,0,0);
            glVertex3f(-100,0,0);

            glVertex3f(0,-100,0);
            glVertex3f(0, 100,0);

            glVertex3f(0,0, 100);
            glVertex3f(0,0,-100);
        }
        glEnd();
    }
}


void drawGrid()
{
    int i;
    if(drawgrid==1)
    {
        glColor3f(0.6, 0.6, 0.6);	//grey
        glBegin(GL_LINES);
        {
            for(i=-8; i<=8; i++)
            {

                if(i==0)
                    continue;	//SKIP the MAIN axes

                //lines parallel to Y-axis
                glVertex3f(i*10, -90, 0);
                glVertex3f(i*10,  90, 0);

                //lines parallel to X-axis
                glVertex3f(-90, i*10, 0);
                glVertex3f( 90, i*10, 0);
            }
        }
        glEnd();
    }
}

void drawSquare(double a)
{
    //glColor3f(1.0,0.0,0.0);
    glBegin(GL_QUADS);
    {
        glVertex3f( a, a,2);
        glVertex3f( a,-a,2);
        glVertex3f(-a,-a,2);
        glVertex3f(-a, a,2);
    }
    glEnd();
}


void drawCircle(double radius,int segments)
{
    int i;
    struct point points[100];
    //glColor3f(0.7,0.7,0.7);
    //generate points
    for(i=0; i<=segments; i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw segments using generated points
    for(i=0; i<segments; i++)
    {
        glBegin(GL_LINES);
        {
            glVertex3f(points[i].x,points[i].y,0);
            glVertex3f(points[i+1].x,points[i+1].y,0);
        }
        glEnd();
    }
}

point pointBuffer[1000][1000];


double dot(Vector a, Vector b)
{
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

class returnObj
{
public:
    point P;
    Vector normal;
    double t;
    point c;
    double shine;
    double spec,diffuse,ambient,reflection;

};

returnObj testSphere(point p,Vector v,int i)
{
    point newp;
    newp.x=p.x-Allobj[i].sphere.center.x;
    newp.y=p.y-Allobj[i].sphere.center.y;
    newp.z=p.z-Allobj[i].sphere.center.z;


    Vector vecP(newp.x,newp.y,newp.z);

    double p2=dot(vecP,vecP);

    double pv=dot(vecP,v);

    returnObj r;
    r.c=Allobj[i].sphere.color;
    r.shine=Allobj[i].sphere.shininess;
    r.t=INFINITY;
    r.spec=Allobj[i].sphere.specular;
    r.diffuse=Allobj[i].sphere.diffuse;
    r.ambient=Allobj[i].sphere.ambient;
    r.reflection=Allobj[i].sphere.refCof;
    double det=(2*pv)*(2*pv)-4*(p2-Allobj[i].sphere.radius*Allobj[i].sphere.radius);
    if(det>=0)
    {
        double t1=(-2*pv+sqrt(det))/2;
        double t2=(-2*pv-sqrt(det))/2;

        if(t1<0&&t2>0)
        {
            r.t=t2;
            r.P.x=p.x+v.x*t2;
            r.P.y=p.y+v.y*t2;
            r.P.z=p.z+v.z*t2;
            r.normal.x=r.P.x-Allobj[i].sphere.center.x;
            r.normal.y=r.P.y-Allobj[i].sphere.center.y;
            r.normal.z=r.P.z-Allobj[i].sphere.center.z;
            r.normal.normalize();

            return r;
        }

        if(t2<0&&t1>0)
        {

            r.t=t1;
            r.P.x=p.x+v.x*t1;
            r.P.y=p.y+v.y*t1;
            r.P.z=p.z+v.z*t1;
            r.normal.x=r.P.x-Allobj[i].sphere.center.x;
            r.normal.y=r.P.y-Allobj[i].sphere.center.y;
            r.normal.z=r.P.z-Allobj[i].sphere.center.z;
            r.normal.normalize();

            return r;

        }
        if(t1>0&&t2>0&&t1<t2)
        {
            r.t=t1;
            r.P.x=p.x+v.x*t1;
            r.P.y=p.y+v.y*t1;
            r.P.z=p.z+v.z*t1;
            r.normal.x=r.P.x-Allobj[i].sphere.center.x;
            r.normal.y=r.P.y-Allobj[i].sphere.center.y;
            r.normal.z=r.P.z-Allobj[i].sphere.center.z;
            r.normal.normalize();
            return r;
        }
        if(t1>0&&t2>0&&t1>t2)
        {
            r.t=t2;
            r.P.x=p.x+v.x*t2;
            r.P.y=p.y+v.y*t2;
            r.P.z=p.z+v.z*t2;
            r.normal.x=r.P.x-Allobj[i].sphere.center.x;
            r.normal.y=r.P.y-Allobj[i].sphere.center.y;
            r.normal.z=r.P.z-Allobj[i].sphere.center.z;
            r.normal.normalize();

            return r;
        }
        return r;
    }
    else
        return r;


}


//returnObj testSphere2(point p,Vector v)
//{
//    point newp;
//    newp.x=p.x-sphere2.center.x;
//    newp.y=p.y-sphere2.center.y;
//    newp.z=p.z-sphere2.center.z;
//
//
//}

double Distance(point a,point b)
{
    return sqrt(pow(a.x-b.x,2)+pow(a.y-b.y,2)+pow(a.z-b.z,2));
}


returnObj testPyramid(point p,Vector v,int i)
{
    returnObj r;
    r.c=Allobj[i].pyramid.color;
    r.shine=Allobj[i].pyramid.shininess;
    r.t=INFINITY;
    r.spec=Allobj[i].pyramid.specular;
    r.diffuse=Allobj[i].pyramid.diffuse;
    r.ambient=Allobj[i].pyramid.ambient;
    r.reflection=Allobj[i].pyramid.refCof;
    double x=Allobj[i].pyramid.low.x-(Allobj[i].pyramid.width/2);
    double y=Allobj[i].pyramid.low.y-(Allobj[i].pyramid.width/2);

    point A;
    A.x=x;
    A.y=y;
    A.z=Allobj[i].pyramid.low.z;
    Vector vecA(A.x,A.y,A.z);
    point B;
    B.x=x;
    B.y=y+Allobj[i].pyramid.width;
    B.z=Allobj[i].pyramid.low.z;
    point C;
    C.x=Allobj[i].pyramid.low.x;
    C.y=Allobj[i].pyramid.low.y;
    C.z=Allobj[i].pyramid.low.z+Allobj[i].pyramid.height;

    Vector AB(B.x-A.x,B.y-A.y,B.z-A.z);
    Vector AC(C.x-A.x,C.y-A.y,C.z-A.z);
    Vector BC(C.x-B.x,C.y-B.y,C.z-B.z);
    Vector CA(A.x-C.x,A.y-C.y,A.z-C.z);

    Vector n=Cross(AB,AC);
    n.normalize();
    if(dot(n,v)>0)
    {
       // cout<<"no"<<endl;
        n=Cross(AC,AB);
        n.normalize();
    }


    double d=-dot(n,vecA);

    Vector vecP(p.x,p.y,p.z);

    double t1=-(dot(n,vecP)+d)/dot(n,v);

    point intersect;
    intersect.x=p.x+v.x*t1;
    intersect.y=p.y+v.y*t1;
    intersect.z=p.z+v.z*t1;


    double ab=Distance(A,B);
    double bc=Distance(B,C);
    double ca=Distance(C,A);
    double ap=Distance(A,intersect);
    double bp=Distance(B,intersect);
    double cp=Distance(C,intersect);

    double Outers=(ab+bc+ca)/2;
    double totalArea=sqrt(Outers*(Outers-ab)*(Outers-bc)*(Outers-ca));

    double outerABP=(ab+ap+bp)/2;
    double totalAreaABP=sqrt(outerABP*(outerABP-ab)*(outerABP-ap)*(outerABP-bp));


    double outerBCP=(bc+bp+cp)/2;
    double totalAreaBCP=sqrt(outerBCP*(outerBCP-bc)*(outerBCP-bp)*(outerBCP-cp));

    double outerACP=(ca+ap+cp)/2;
    double totalAreaACP=sqrt(outerACP*(outerACP-ca)*(outerACP-ap)*(outerACP-cp));



    Vector Aint(intersect.x-A.x,intersect.y-A.y,intersect.z-A.z);
    Vector Bint(intersect.x-B.x,intersect.y-B.y,intersect.z-B.z);
    Vector Cint(intersect.x-C.x,intersect.y-C.y,intersect.z-C.z);


    if(abs((totalAreaABP+totalAreaBCP+totalAreaACP)-totalArea)< 0.000001 )
    {
        if(t1<0)
            t1= INFINITY;
        else
        {
            //cout<<"dfd"<<endl;
            r.normal=n;
            r.normal.normalize();
            r.t=t1;
            r.P.x=p.x+r.t*v.x;
            r.P.y=p.y+r.t*v.y;
            r.P.z=p.z+r.t*v.z;

        }
    }
    else
    {
        t1=INFINITY;
    }




    A.x=x;
    A.y=y+Allobj[i].pyramid.width;
    A.z=Allobj[i].pyramid.low.z;
    vecA=Vector::put(A.x,A.y,A.z);

    B.x=x+Allobj[i].pyramid.width;
    B.y=y+Allobj[i].pyramid.width;
    B.z=Allobj[i].pyramid.low.z;

    C.x=Allobj[i].pyramid.low.x;
    C.y=Allobj[i].pyramid.low.y;
    C.z=Allobj[i].pyramid.low.z+Allobj[i].pyramid.height;

    AB=Vector::put(B.x-A.x,B.y-A.y,B.z-A.z);
    AC=Vector::put(C.x-A.x,C.y-A.y,C.z-A.z);
    BC=Vector::put(C.x-B.x,C.y-B.y,C.z-B.z);
    CA=Vector::put(A.x-C.x,A.y-C.y,A.z-C.z);

    n=Cross(AB,AC);
    n.normalize();

    if(dot(n,v)>0)
    {
        //cout<<"no"<<endl;
        n=Cross(AC,AB);
        n.normalize();
    }

    d=-dot(n,vecA);

    vecP=Vector::put(p.x,p.y,p.z);

    double t2=-(dot(n,vecP)+d)/dot(n,v);


    intersect.x=p.x+v.x*t2;
    intersect.y=p.y+v.y*t2;
    intersect.z=p.z+v.z*t2;



    ab=Distance(A,B);
    bc=Distance(B,C);
    ca=Distance(C,A);
    ap=Distance(A,intersect);
    bp=Distance(B,intersect);
    cp=Distance(C,intersect);

    Outers=(ab+bc+ca)/2;
    totalArea=sqrt(Outers*(Outers-ab)*(Outers-bc)*(Outers-ca));

    outerABP=(ab+ap+bp)/2;
    totalAreaABP=sqrt(outerABP*(outerABP-ab)*(outerABP-ap)*(outerABP-bp));


    outerBCP=(bc+bp+cp)/2;
    totalAreaBCP=sqrt(outerBCP*(outerBCP-bc)*(outerBCP-bp)*(outerBCP-cp));

    outerACP=(ca+ap+cp)/2;
    totalAreaACP=sqrt(outerACP*(outerACP-ca)*(outerACP-ap)*(outerACP-cp));




    Aint=Vector::put(intersect.x-A.x,intersect.y-A.y,intersect.z-A.z);
    Bint=Vector::put(intersect.x-B.x,intersect.y-B.y,intersect.z-B.z);
    Cint=Vector::put(intersect.x-C.x,intersect.y-C.y,intersect.z-C.z);

    if(abs((totalAreaABP+totalAreaBCP+totalAreaACP)-totalArea)<0.0000001)
    {
        if(t2<0)
            t2= INFINITY;
        else if (r.t>t2)
        {
            //cout<<"dfd"<<endl;
            r.normal=n;
            r.normal.normalize();
            r.t=t2;
            r.P=intersect;

        }
    }
    else
    {
        // cout<<"no"<<endl;
        t2=INFINITY;
    }






    A.x=x+Allobj[i].pyramid.width;
    A.y=y+Allobj[i].pyramid.width;
    A.z=Allobj[i].pyramid.low.z;
    vecA=Vector::put(A.x,A.y,A.z);

    B.x=x+Allobj[i].pyramid.width;
    B.y=y;
    B.z=Allobj[i].pyramid.low.z;

    C.x=Allobj[i].pyramid.low.x;
    C.y=Allobj[i].pyramid.low.y;
    C.z=Allobj[i].pyramid.low.z+Allobj[i].pyramid.height;

    AB=Vector::put(B.x-A.x,B.y-A.y,B.z-A.z);
    AC=Vector::put(C.x-A.x,C.y-A.y,C.z-A.z);
    BC=Vector::put(C.x-B.x,C.y-B.y,C.z-B.z);
    CA=Vector::put(A.x-C.x,A.y-C.y,A.z-C.z);

    n=Cross(AB,AC);
    n.normalize();

if(dot(n,v)>0)
    {
        //cout<<"no"<<endl;
        n=Cross(AC,AB);
        n.normalize();
    }

    d=-dot(n,vecA);

    vecP=Vector::put(p.x,p.y,p.z);

    double t3=-(dot(n,vecP)+d)/dot(n,v);


    intersect.x=p.x+v.x*t3;
    intersect.y=p.y+v.y*t3;
    intersect.z=p.z+v.z*t3;




    ab=Distance(A,B);
    bc=Distance(B,C);
    ca=Distance(C,A);
    ap=Distance(A,intersect);
    bp=Distance(B,intersect);
    cp=Distance(C,intersect);

    Outers=(ab+bc+ca)/2;
    totalArea=sqrt(Outers*(Outers-ab)*(Outers-bc)*(Outers-ca));

    outerABP=(ab+ap+bp)/2;
    totalAreaABP=sqrt(outerABP*(outerABP-ab)*(outerABP-ap)*(outerABP-bp));


    outerBCP=(bc+bp+cp)/2;
    totalAreaBCP=sqrt(outerBCP*(outerBCP-bc)*(outerBCP-bp)*(outerBCP-cp));

    outerACP=(ca+ap+cp)/2;
    totalAreaACP=sqrt(outerACP*(outerACP-ca)*(outerACP-ap)*(outerACP-cp));





    Aint=Vector::put(intersect.x-A.x,intersect.y-A.y,intersect.z-A.z);
    Bint=Vector::put(intersect.x-B.x,intersect.y-B.y,intersect.z-B.z);
    Cint=Vector::put(intersect.x-C.x,intersect.y-C.y,intersect.z-C.z);

    if(abs((totalAreaABP+totalAreaBCP+totalAreaACP)-totalArea) <0.0000001)
    {
        if(t3<0)
            t3=INFINITY;
        else if (r.t>t3)
        {
            // cout<<"dfd"<<endl;
            r.normal=n;
            r.normal.normalize();
            r.t=t3;
            r.P=intersect;

        }
    }
    else
    {
        // cout<<"no"<<endl;
        t3=INFINITY;
    }





    A.x=x;
    A.y=y;
    A.z=Allobj[i].pyramid.low.z;
    vecA=Vector::put(A.x,A.y,A.z);

    B.x=x+Allobj[i].pyramid.width;
    B.y=y;
    B.z=Allobj[i].pyramid.low.z;

    C.x=Allobj[i].pyramid.low.x;
    C.y=Allobj[i].pyramid.low.y;
    C.z=Allobj[i].pyramid.low.z+Allobj[i].pyramid.height;

    AB=Vector::put(B.x-A.x,B.y-A.y,B.z-A.z);
    AC=Vector::put(C.x-A.x,C.y-A.y,C.z-A.z);
    BC=Vector::put(C.x-B.x,C.y-B.y,C.z-B.z);
    CA=Vector::put(A.x-C.x,A.y-C.y,A.z-C.z);

    n=Cross(AB,AC);
    n.normalize();
if(dot(n,v)>0)
    {
       // cout<<"no"<<endl;
        n=Cross(AC,AB);
        n.normalize();
    }

    d=-dot(n,vecA);

    vecP=Vector::put(p.x,p.y,p.z);

    double t4=-(dot(n,vecP)+d)/dot(n,v);


    intersect.x=p.x+v.x*t4;
    intersect.y=p.y+v.y*t4;
    intersect.z=p.z+v.z*t4;



    ab=Distance(A,B);
    bc=Distance(B,C);
    ca=Distance(C,A);
    ap=Distance(A,intersect);
    bp=Distance(B,intersect);
    cp=Distance(C,intersect);

    Outers=(ab+bc+ca)/2;
    totalArea=sqrt(Outers*(Outers-ab)*(Outers-bc)*(Outers-ca));

    outerABP=(ab+ap+bp)/2;
    totalAreaABP=sqrt(outerABP*(outerABP-ab)*(outerABP-ap)*(outerABP-bp));


    outerBCP=(bc+bp+cp)/2;
    totalAreaBCP=sqrt(outerBCP*(outerBCP-bc)*(outerBCP-bp)*(outerBCP-cp));

    outerACP=(ca+ap+cp)/2;
    totalAreaACP=sqrt(outerACP*(outerACP-ca)*(outerACP-ap)*(outerACP-cp));





    Aint=Vector::put(intersect.x-A.x,intersect.y-A.y,intersect.z-A.z);
    Bint=Vector::put(intersect.x-B.x,intersect.y-B.y,intersect.z-B.z);
    Cint=Vector::put(intersect.x-C.x,intersect.y-C.y,intersect.z-C.z);

    if(abs((totalAreaABP+totalAreaBCP+totalAreaACP)-totalArea)<0.000001)
    {
        if(t4<0)
            t4= INFINITY;
        else if (r.t>t4)
        {
            // cout<<"dfd"<<endl;
            r.normal=n;
            r.normal.normalize();
            r.t=t4;
            r.P=intersect;

        }
    }
    else
    {
        // cout<<"no"<<endl;
        t4=INFINITY;
    }

    double t5=(Allobj[i].pyramid.low.z-p.z)/v.z;
    intersect.x=p.x+v.x*t5;
    intersect.y=p.y+v.y*t5;
    intersect.z=p.z+v.z*t5;

    if((intersect.x<=(Allobj[i].pyramid.low.x+Allobj[i].pyramid.width/2)) && (intersect.x>=(Allobj[i].pyramid.low.x-Allobj[i].pyramid.width/2)) && (intersect.y<=(Allobj[i].pyramid.low.y+Allobj[i].pyramid.width/2)) && (intersect.y>=(Allobj[i].pyramid.low.y-Allobj[i].pyramid.width/2)) && (t5>0))

    {
        if (r.t>t5)
        {
            r.normal.x=0;
            r.normal.y=0;
            r.normal.z=-1;
            r.normal.normalize();
            r.t=t5;
            r.P=intersect;

        }
        else
        {
            t5=INFINITY;
        }

    }
    else
    {
        t5=INFINITY;
    }

    return r;



}
bitmap_image b_img("C:/Users/Zahid/Desktop/offline3/texture.bmp");

returnObj testBoard(point p,Vector v)
{

    returnObj r;
    r.t=INFINITY;
    r.diffuse=board.diffuse;
    r.spec=0;
    r.ambient=board.ambient;
    r.reflection=board.refCof;
    double t=-p.z/v.z;

    if(t<0) return r;

    point intersect;
    intersect.x=p.x+t*v.x;
    intersect.y=p.y+t*v.y;
    intersect.z=p.z+t*v.z;

    if((intersect.x>=-500 && intersect.x<=2050) && (intersect.y>=-500 && intersect.y<=2050))
    {

        double newX=intersect.x+500;
        double newY=intersect.y+500;

        newX=newX-int(newX/board.width)*board.width;
        newY=newY-int(newY/board.width)*board.width;

        int indX,indY;
        indX=int((width/board.width)*newX);
        indY=int((height/board.width)*newY);

        point color;
        int x=int((intersect.x+500)/board.width);
        int y=int((intersect.y+500)/board.width);
        if(texure){
//        f((x+y)%2 ==0)
//        {
            //cout<<"white"<<endl;
            color.x=textureBuffer[indX][indY].x;
            color.y=textureBuffer[indX][indY].y;
            color.z=textureBuffer[indX][indY].z;
}
else{
        if((x+y)%2 ==0)
        {
            color.x=0;
            color.y=0;
            color.z=0;
        }
        else
        {
            //cout<<"black";
            color.x=1;
            color.y=1;
            color.z=1;
        }
}
        r.c=color;
        r.t=t;
        r.normal.x=0;
        r.normal.y=0;
        r.normal.z=1;
        r.P.x=p.x+v.x*r.t;
        r.P.y=p.x+v.y*r.t;
        r.P.z=0;

    }
    return r;

}

bool illuminate(point P,Vector V,double dist)
{
    returnObj dummy;
    dummy.t=INFINITY;
    for(int i=0; i<numObj; i++)
    {
        if(Allobj[i].name=="sphere")
        {
            returnObj r=testSphere(P,V,i);
            if(dummy.t>r.t)
            {
                dummy=r;
//                dummy.c.x=r.c.x*Allobj[i].sphere.ambient;
//                dummy.c.y=r.c.y*Allobj[i].sphere.ambient;
//                dummy.c.z=r.c.z*Allobj[i].sphere.ambient;
            }

        }
        else if(Allobj[i].name=="pyramid")
        {
            returnObj r=testPyramid(P,V,i);
            if(dummy.t>r.t)
            {
                dummy=r;
            }

        }
    }
    returnObj r=testBoard(P,V);
    if(dummy.t>r.t)
    {
        dummy=r;

    }
    if(dist>dummy.t)
        return true;
    return false;

}


returnObj raytracing(point P,Vector V,int level)
{
    if(level==0)
    {

        returnObj dummy;
        dummy.t=INFINITY;
        return dummy;
    }

    returnObj dummy;
    dummy.t=INFINITY;
    for(int i=0; i<numObj; i++)
    {
        if(Allobj[i].name=="sphere")
        {
            returnObj r=testSphere(P,V,i);
            if(dummy.t>r.t)
            {
                dummy=r;
//                dummy.c.x=r.c.x*Allobj[i].sphere.ambient;
//                dummy.c.y=r.c.y*Allobj[i].sphere.ambient;
//                dummy.c.z=r.c.z*Allobj[i].sphere.ambient;
            }

        }
        else if(Allobj[i].name=="pyramid")
        {
            returnObj r=testPyramid(P,V,i);
            if(dummy.t>r.t)
            {
                dummy=r;

//                dummy.c.x=r.c.x*Allobj[i].pyramid.ambient;
//                dummy.c.y=r.c.y*Allobj[i].pyramid.ambient;
//                dummy.c.z=r.c.z*Allobj[i].pyramid.ambient;
            }

        }
    }
    double lambert=0;
    double phong=0;
    if(dummy.t<INFINITY)
    {
        dummy.P.x=P.x+dummy.t*V.x;
        dummy.P.y=P.y+dummy.t*V.y;
        dummy.P.z=P.z+dummy.t*V.z;


        for(int i=0; i<numNor; i++)
        {
            V.normalize();
            Vector Point_source=Vector(normalLight[i].pos.x-dummy.P.x,normalLight[i].pos.y-dummy.P.y,normalLight[i].pos.z-dummy.P.z);
            double distance=Distance(normalLight[i].pos,dummy.P);

            Point_source.normalize();
            double scaling_factor=exp(-distance*distance*normalLight[i].fall);

            point p;
            p.x=dummy.P.x+Point_source.x*.00001;
            p.y=dummy.P.y+Point_source.y*.00001;
            p.z=dummy.P.z+Point_source.z*.00001;


            if(illuminate(p,Point_source,distance)) continue;

            if(dot(Point_source,dummy.normal)>0)
                lambert+=dot(Point_source,dummy.normal)*scaling_factor;
            point x;
            x.x=2*dot(Point_source,dummy.normal)*dummy.normal.x-Point_source.x;
            x.y=2*dot(Point_source,dummy.normal)*dummy.normal.y-Point_source.y;
            x.z=2*dot(Point_source,dummy.normal)*dummy.normal.z-Point_source.z;
            Vector R=Vector(x.x,x.y,x.z);

            R.normalize();
            phong+=pow(dot(R,V),dummy.shine)*scaling_factor;


        }

    }
    //cout<<phong<<endl;




    returnObj r=testBoard(P,V);
    if(dummy.t>r.t)
    {
        dummy=r;
        dummy.P.x=P.x+dummy.t*V.x;
        dummy.P.y=P.y+dummy.t*V.y;
        dummy.P.z=P.z+dummy.t*V.z;


//        dummy.c.x=r.c.x*board.ambient;
//        dummy.c.y=r.c.y*board.ambient;
//        dummy.c.z=r.c.z*board.ambient;

        lambert=0;
        phong=0;
        for(int i=0; i<numNor; i++)
        {
            Vector Point_source=Vector::put(normalLight[i].pos.x-dummy.P.x,normalLight[i].pos.y-dummy.P.y,normalLight[i].pos.z-dummy.P.z);
            double distance=Distance(normalLight[i].pos,dummy.P);

            Point_source.normalize();
            double scaling_factor=exp(-distance*distance*normalLight[i].fall);

            point p;
            p.x=dummy.P.x+Point_source.x*.00001;
            p.y=dummy.P.y+Point_source.y*.00001;
            p.z=dummy.P.z+Point_source.z*.00001;



            if(illuminate(p,Point_source,distance)) continue;

            if(dot(Point_source,dummy.normal)>0)
                lambert+=dot(Point_source,dummy.normal)*scaling_factor;

        }

    }
    if(dummy.t==INFINITY)
    {
        dummy.c.x=dummy.c.y=dummy.c.z=0;
        return dummy;
    }
    Vector R=Vector(V.x-dummy.normal.x*2*dot(V,dummy.normal),V.y-dummy.normal.y*2*dot(V,dummy.normal),V.z-dummy.normal.z*2*dot(V,dummy.normal));

    point p;
    p.x=dummy.P.x+.000001*R.x;
    p.y=dummy.P.y+.000001*R.y;
    p.z=dummy.P.z+.000001*R.z;
    returnObj ret=raytracing(p,R,level-1);


    dummy.c.x=dummy.c.x*(dummy.ambient+dummy.diffuse*lambert+dummy.spec*phong)+ret.c.x*dummy.reflection;
    dummy.c.y=dummy.c.y*(dummy.ambient+dummy.diffuse*lambert+dummy.spec*phong)+ret.c.y*dummy.reflection;
    dummy.c.z=dummy.c.z*(dummy.ambient+dummy.diffuse*lambert+dummy.spec*phong)+ret.c.z*dummy.reflection;
    if(dummy.c.x>1)
        dummy.c.x=0;
    if(dummy.c.y>1)
        dummy.c.y=0;
    if(dummy.c.z>1)
        dummy.c.z=1;
    return dummy;
}




void PrintttImage()
{
    fovX=aspectRatio*fovY;

    double planeH=2*Near*tan(fovY*pi/(2*180));
    double planeW=2*Near*tan(fovX*pi/(2*180));




    point center;
    center.x=pos.x+l.x*Near;
    center.y=pos.y+l.y*Near;
    center.z=pos.z+l.z*Near;
    int indexX=int(numPixel/2);
    int indexY=int(numPixel/2);
    pointBuffer[indexX][indexY]=center;

    double Rx,Ry,Rz,Ux,Uy,Uz;
    Rx=(planeW/numPixel)*r.x;
    Ry=(planeW/numPixel)*r.y;
    Rz=(planeW/numPixel)*r.z;

    Ux=(planeH/numPixel)*u.x;
    Uy=(planeH/numPixel)*u.y;
    Uz=(planeH/numPixel)*u.z;

    double currentRX=0;
    double currentRY=0;
    double currentRZ=0;

    double currentUX=0;
    double currentUY=0;
    double currentUZ=0;

    for(int i=indexX; i<numPixel; i++)
    {
        for(int j=indexY; j<numPixel; j++)
        {
            pointBuffer[i][j].x=center.x+(i-indexX)*Rx+(j-indexY)*Ux;
            pointBuffer[i][j].y=center.y+(i-indexX)*Ry+(j-indexY)*Uy;
            pointBuffer[i][j].z=center.z+(i-indexX)*Rz+(j-indexY)*Uz;
        }

    }

    for(int i=indexX; i<numPixel; i++)
    {
        for(int j=indexY; j>=0; j--)
        {
            pointBuffer[i][j].x=center.x+(i-indexX)*Rx+(j-indexY)*Ux;
            pointBuffer[i][j].y=center.y+(i-indexX)*Ry+(j-indexY)*Uy;
            pointBuffer[i][j].z=center.z+(i-indexX)*Rz+(j-indexY)*Uz;
        }

    }

    for(int i=indexX; i>=0; i--)
    {
        for(int j=indexY; j<numPixel; j++)
        {
            pointBuffer[i][j].x=center.x+(i-indexX)*Rx+(j-indexY)*Ux;
            pointBuffer[i][j].y=center.y+(i-indexX)*Ry+(j-indexY)*Uy;
            pointBuffer[i][j].z=center.z+(i-indexX)*Rz+(j-indexY)*Uz;
        }

    }

    for(int i=indexX; i>=0; i--)
    {
        for(int j=indexY; j>=0; j--)
        {
            pointBuffer[i][j].x=center.x+(i-indexX)*Rx+(j-indexY)*Ux;
            pointBuffer[i][j].y=center.y+(i-indexX)*Ry+(j-indexY)*Uy;
            pointBuffer[i][j].z=center.z+(i-indexX)*Rz+(j-indexY)*Uz;
        }

    }





    bitmap_image image(numPixel, numPixel);
    for(int i=0; i<numPixel; i++)
    {


        for(int j=0; j<numPixel; j++)
        {
            point color;
            color.x=0;
            color.y=0;
            color.z=0;

            point p=pointBuffer[i][j];

            Vector v(p.x-pos.x,p.y-pos.y,p.z-pos.z);
            v.normalize();


            returnObj r=raytracing(p,v,int(levelRec));

            if(r.t<INFINITY)
            {
                color.x=r.c.x;
                color.y=r.c.y;
                color.z=r.c.z;
            }

            image.set_pixel(i, numPixel-j, color.x*255,color.y*255,color.z*255);


            /*double  t1=testSphere1(p,v);
            //cout<<t1<<endl;
            double  t2=testSphere2(p,v);
            double  t3=9999999;//testPyramid(p,v);
            //double  t4=testBoard(p,v);

            double arr[3];
            arr[0]=t1;
            arr[1]=t2;
            arr[2]=t3;
            //arr[3]=t4;

            int n = sizeof(arr)/sizeof(arr[0]);
            //
            sort(arr, arr+n);
            //cout<<"Result order"<<arr[0]<<arr[1]<<arr[2]<<arr[3]<<endl;

            point color;
            //cout<<"sds11";
            if(arr[0]==INFINITY)
            {

                else
                {
                    color.x=pyramid.color.x*255;
                    color.y=pyramid.color.y*255;
                    color.z=pyramid.color.z*255;
                }
                //color=sphere1.color;
                // cout<<sphere1.col<<sphere1.color.y<<sphere1.color.z;
            //                if(arr[0]==t1)
            //                {
            //                    cout<"sp";
            //                    color=sphere1.color;
            //                }
                /*else if(arr[0]==t2)
                {
                    color=sphere2.color;
                }*/
//                else if(arr[0]==t3)
//                {
//                    color=pyramid.color;
//                }
//                else{
//                     double t=-p.z/v.z;
//
//                    point intersect;
//                    intersect.x=p.x+t*v.x;
//                    intersect.y=p.y+t*v.y;
//                    intersect.z=p.z+t*v.z;
//
//                    int x=int((intersect.x+500)/board.width);
//                    int y=int((intersect.y+500)/board.width);
//                    if((x+y)/2 ==0)
//                    {
//                        color.x=0.4;
//                        color.y=0.4;
//                        color.z=0.4;
//                    }
//                    else{
//                        color.x=0;
//                        color.y=0;
//                        color.z=0;
//                    }
//                }
//           // }
        }


    }




    image.save_image("C:/Users/Zahid/Desktop/offline3/out.bmp");


}



void drawSS()
{
    double x;
    double y=-500;
    for(int i=0; i<=50; i++)
    {
        x=-500;
        y=y+board.width;
        for(int j=0; j<=50; j++)
        {
            x=x+board.width;
            board.print(x,y,i,j);
        }
    }

    for(int i=0; i<numObj; i++)
    {
        string name;
        name=Allobj[i].name;
        if(name=="sphere")
        {
            Allobj[i].sphere.print();
        }
        if(name=="pyramid")
        {
            Allobj[i].pyramid.print();
        }
    }

    for(int i=0; i<numNor; i++)
    {
        normalLight[i].print();
    }

    for(int i=0; i<numSpot; i++)
    {
        spotLight[i].print();
    }
//    sphere1.print();
//    sphere2.print();
//    pyramid.print();


}

void keyboardListener(unsigned char key, int x,int y)
{
    point newU, newR, newL;
    Vector newr,newu,newl;
    switch(key)
    {

    case '1':
        //l=Rotate(u,l,1);
        //r=Rotate1(u,r,1);
        newR.x = r.x * cos(pi / 180) + l.x * sin(pi / 180);
        newR.y = r.y * cos(pi / 180) + l.y * sin(pi / 180);
        newR.z = r.z * cos(pi / 180) + l.z * sin(pi / 180);

        newL.x = l.x * cos(pi / 180) + r.x * sin(pi / 180) * (-1);
        newL.y = l.y * cos(pi / 180) + r.y * sin(pi / 180) * (-1);
        newL.z = l.z * cos(pi / 180) + r.z * sin(pi / 180) * (-1);
        r=Vector::put(newR.x,newR.y,newR.z);
        l=Vector::put(newL.x,newL.y,newL.z);


        break;
    case '2':
        // l=Rotate(u,l,-1);
        // r=Rotate1(u,r,-1);

        newR.x = r.x * cos(pi / 180) + l.x * sin(pi / 180) * (-1);
        newR.y = r.y * cos(pi / 180) + l.y * sin(pi / 180) * (-1);
        newR.z = r.z * cos(pi / 180) + l.z * sin(pi / 180) * (-1);

        newL.x = l.x * cos(pi / 180) + r.x * sin(pi / 180);
        newL.y = l.y * cos(pi / 180) + r.y * sin(pi / 180);
        newL.z = l.z * cos(pi / 180) + r.z * sin(pi / 180);
        r=Vector::put(newR.x,newR.y,newR.z);
        l=Vector::put(newL.x,newL.y,newL.z);



        break;
    case '3':
        //  l=Rotate1(r,l,1);
        // u=Rotate(r,u,-1);

        newL.x = l.x * cos(pi / 180) + u.x * sin(pi / 180);
        newL.y = l.y * cos(pi / 180) + u.y * sin(pi / 180);
        newL.z = l.z * cos(pi / 180) + u.z * sin(pi / 180);

        newU.x = u.x * cos(pi / 180) + l.x * sin(pi / 180) * (-1);
        newU.y = u.y * cos(pi / 180) + l.y * sin(pi / 180) * (-1);
        newU.z = u.z * cos(pi / 180) + l.z * sin(pi / 180) * (-1);
        l= Vector::put(newL.x,newL.y,newL.z);
        u= Vector::put(newU.x,newU.y,newU.z);


        break;
    case '4':
        // l=Rotate1(r,l,1);
        //u=Rotate(r,u,-1);

        newU.x = u.x * cos(pi / 180) + l.x * sin(pi / 180);
        newU.y = u.y * cos(pi / 180) + l.y * sin(pi / 180);
        newU.z = u.z * cos(pi / 180) + l.z * sin(pi / 180);

        newL.x = l.x * cos(pi / 180) + u.x * sin(pi / 180) * (-1);
        newL.y = l.y * cos(pi / 180) + u.y * sin(pi / 180) * (-1);
        newL.z = l.z * cos(pi / 180) + u.z * sin(pi / 180) * (-1);

        l= Vector::put(newL.x,newL.y,newL.z);
        u= Vector::put(newU.x,newU.y,newU.z);


        break;
    case '5':
        //   u=Rotate(l,u,1);
        // r=Rotate1(l,r,1);
        newU.x = u.x * cos(pi / 180) + r.x * sin(pi / 180);
        newU.y = u.y * cos(pi / 180) + r.y * sin(pi / 180);
        newU.z = u.z * cos(pi / 180) + r.z * sin(pi / 180);

        newR.x = r.x * cos(pi / 180) + u.x * sin(pi / 180) * (-1);
        newR.y = r.y * cos(pi / 180) + u.y * sin(pi / 180) * (-1);
        newR.z = r.z * cos(pi / 180) + u.z * sin(pi / 180) * (-1);

        u= Vector::put(newU.x,newU.y,newU.z);
        r= Vector::put(newR.x,newR.y,newR.z);


        break;
    case '6':
        //     u=Rotate(l,u,-1);
        //  r=Rotate1(l,r,-1);
        newR.x = r.x * cos(pi / 180) + u.x * sin(pi / 180);
        newR.y = r.y * cos(pi / 180) + u.y * sin(pi / 180);
        newR.z = r.z * cos(pi / 180) + u.z * sin(pi / 180);

        newU.x = u.x * cos(pi / 180) + r.x * sin(pi / 180) * (-1);
        newU.y = u.y * cos(pi / 180) + r.y * sin(pi / 180) * (-1);
        newU.z = u.z * cos(pi / 180) + r.z * sin(pi / 180) * (-1);

        u= Vector::put(newU.x,newU.y,newU.z);
        r= Vector::put(newR.x,newR.y,newR.z);

        break;

    case '0':

        PrintttImage();
        break;
        case 32:

        texure=!texure;
        break;
    default:
        break;
    }
}


void specialKeyListener(int key, int x,int y)
{
    switch(key)
    {
    case GLUT_KEY_DOWN:		//down arrow key
        //cameraHeight -= 3.0;
        pos.x-=2*l.x;
        pos.y-=2*l.y;
        pos.z-=2*l.z;
        break;
    case GLUT_KEY_UP:		// up arrow key
        //cameraHeight += 3.0;
        pos.x+=2*l.x;
        pos.y+=2*l.y;
        pos.z+=2*l.z;
        break;

    case GLUT_KEY_RIGHT:
        //cameraAngle += 0.03;
        pos.x+=2*r.x;
        pos.y+=2*r.y;
        pos.z+=2*r.z;
        break;
    case GLUT_KEY_LEFT:
        //caeraAngle -= 0.03;
        pos.x-=2*r.x;
        pos.y-=2*r.y;
        pos.z-=2*r.z;
        break;

    case GLUT_KEY_PAGE_UP:
        pos.x+=2*u.x;
        pos.y+=2*u.y;
        pos.z+=2*u.z;

        break;
    case GLUT_KEY_PAGE_DOWN:
        pos.x-=2*u.x;
        pos.y-=2*u.y;
        pos.z-=2*u.z;

        break;

    case GLUT_KEY_INSERT:
        break;

    case GLUT_KEY_HOME:
        break;
    case GLUT_KEY_END:
        break;

    default:
        break;
    }
}


void mouseListener(int button, int state, int x, int y) 	//x, y is the x-y of the screen (2D)
{
    switch(button)
    {
    case GLUT_LEFT_BUTTON:
        if(state == GLUT_DOWN) 		// 2 times?? in ONE click? -- solution is checking DOWN or UP
        {
            drawaxes=1-drawaxes;
        }
        break;

    case GLUT_RIGHT_BUTTON:
        //........
        break;

    case GLUT_MIDDLE_BUTTON:
        //........
        break;

    default:
        break;
    }
}



void display()
{

    //clear the display
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0,0,0,0);	//color black
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /********************
    / set-up camera here
    ********************/
    //load the correct matrix -- MODEL-VIEW matrix
    glMatrixMode(GL_MODELVIEW);

    //initialize the matrix
    glLoadIdentity();



    gluLookAt(pos.x,pos.y,pos.z,		pos.x+l.x,pos.y+l.y,pos.z+l.z,		u.x,u.y,u.z);



    //again select MODEL-VIEW
    glMatrixMode(GL_MODELVIEW);


    /****************************
    / Add your objects from here
    ****************************/
    //add objects

    drawAxes();
    drawGrid();


    drawSS();

    //ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
    glutSwapBuffers();
}


void animate()
{
    angle+=0.05;
    up+=50;
    //codes for any changes in Models, Camera
    glutPostRedisplay();
}

void init()
{
    //codes for initialization
    drawgrid=0;
    drawaxes=0;
    cameraHeight=150.0;
    cameraAngle=1.0;
    angle=0;
    up=0;
    //clear the screen
    glClearColor(0,0,0,0);


    pos.x=50;
    pos.y=50;
    pos.z=50;

    ifstream input;
    input.open("C:/Users/Zahid/Desktop/offline3/input.txt");



    input>>Near>>Faar;
    cout<<"near"<<Near;
    input>>fovY>>aspectRatio;
    cout<<"fov"<<fovY;
    input>>levelRec;
    input>>numPixel;
    cout<<"numPixel"<<numPixel<<endl;
    fovX=aspectRatio*fovY;

    double  checkW,ambient,diffuse,refCof;
    input>>checkW>>ambient>>diffuse>>refCof;
    board.put(checkW,ambient,diffuse,refCof);
    input>>numObj;
    for(int i=0; i<numObj; i++)
    {

        string name;
        input>>name;

        if(name=="sphere")
        {
            Generic obj;
            obj.name=name;
            point center;
            input>>center.x>>center.y>>center.z;
            double rad;
            input>>rad;
            point color;
            input>>color.x>>color.y>>color.z;
            double spec,shine;
            input>>ambient>>diffuse>>spec>>refCof>>shine;
            obj.sphere.put(center,rad,color,ambient,diffuse,spec,refCof,shine);
            Allobj[i]=obj;
        }
        if(name=="pyramid")
        {
            Generic obj;
            obj.name=name;
            point low;
            point color;
            input>>low.x>>low.y>>low.z;
            double width,height;
            input>>width>>height;
            low.x+=width/2;
            low.y+=width/2;
            double spec,shine;

            input>>color.x>>color.y>>color.z;
            input>>ambient>>diffuse>>spec>>refCof>>shine;
            obj.pyramid.put(low,width,height,color,ambient,diffuse,spec,refCof,shine);
            Allobj[i]=obj;
        }
    }

    input>>numNor;
    for(int i=0; i<numNor; i++)
    {
        double x,y,z,fall;
        input>>x>>y>>z;
        input>>fall;
        point pos;
        pos.x=x;
        pos.y=y;
        pos.z=z;
        NormalLight n(pos,fall);
        normalLight[i]=n;
    }
    input>>numSpot;
    for(int i=0; i<numSpot; i++)
    {
        double x,y,z,fall,lx,ly,lz,cut;
        input>>x>>y>>z;
        input>>fall;
        input>>lx>>ly>>lz>>cut;
        point pos;
        pos.x=x;
        pos.y=y;
        pos.z=z;

        point look;
        look.x=lx;
        look.y=ly;
        look.z=lz;

        SpotLight n(pos,fall,look,cut);
        spotLight[i]=n;
    }


        height = b_img.height();
        width = b_img.width();
        textureBuffer = new point* [width];
        for (int i = 0; i < width; i++) {
            textureBuffer[i] = new point [height];
            for (int j = 0; j < height; j++) {
                unsigned char r, g, b;
                b_img.get_pixel(i, j, r, g, b);
                point c;
                c.x=r/255.0;
                c.y=g/255.0;
                c.z= b/255.0;
                textureBuffer[i][j] = c;
            }
        }

    input.close();
    /************************
    / set-up projection here
    ************************/
    //load the PROJECTION matrix
    glMatrixMode(GL_PROJECTION);

    //initialize the matrix
    glLoadIdentity();

    //give PERSPECTIVE parameters
    gluPerspective(fovY,	aspectRatio,	Near,	Faar);
    //field of view in the Y (vertically)
    //aspect ratio that determines the field of view in the X direction (horizontally)
    //near distance
    //far distance
}

int main(int argc, char **argv)
{
    glutInit(&argc,argv);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(0, 0);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

    glutCreateWindow("My OpenGL Program");

    init();

    glEnable(GL_DEPTH_TEST);	//enable Depth Testing

    glutDisplayFunc(display);	//display callback function
    glutIdleFunc(animate);		//what you want to do in the idle time (when no drawing is occuring)

    glutKeyboardFunc(keyboardListener);
    glutSpecialFunc(specialKeyListener);
    glutMouseFunc(mouseListener);

    glutMainLoop();		//The main loop of OpenGL

    return 0;
}

