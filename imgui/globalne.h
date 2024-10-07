#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

int brLinija = 0, brLinijaStari = 0;
int poceloCrtanje = 0;
int y = 0; 
int deltaX = 0;
int prvi = 0, drugi = 0, treci = 0;
float debljina = 2.0;
int mis = 0, misOtpusten = 0;
int GRID_STEP = 50;
int sredinaX = 0, sredinaY = 0;
bool pokretan = 0, nepokretan = 0, sila = 0;
int indikatorCrtanja = -1;
static int mernaJedinica = 0;
static int ugaoOslonca[] = { 0, 90, 180, 270 };
static int ugaoSile[] = { 0, -45, -90, -135, -180, -225, -270, -315 };
static int indexUgaoOslonca = 0;
static int indexUgaoSile = 0;
int inicijalizuj = 1;
int jednom = 1;
const int maxbrStapova = 20;
static int intenzitetSile = 100;
int smerMomenta = 0, intenzitetMomenta = 101;
int brCvorova = 0;

struct Stap
{
    int brStapa = 0;
    float A = 0.0;
    float E = 0.0;
    float I = 0.0;
    int i = 0;
    int j = 0;
    int osobina = 0;
};

Stap *niz = ( Stap * ) malloc( sizeof( Stap ) );

struct POslonac
{
    ImVec2 t0, t1, t2;    
    ImVec2 poc, kraj;
    int cvor = 0;
    int pomeranjeX = 0, pomeranjeY = 0;
};

struct NOslonac
{
    ImVec2 t0, t1, t2;    
    int cvor = 0;
};

struct Sila
{
    ImVec2 t0, t1, t2;
    ImVec2 poc,kraj;
    float intenzitetX, intenzitetY;
    float ugaoR;
    int ugaoS;
    int cvor = 0;
};

struct Moment
{
    ImVec2 centar;
    ImVec2 t1,t2,t3;
    int smer;
    int intenzitet;
    int cvor = 0;
};