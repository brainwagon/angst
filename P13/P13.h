//
// Plan13.cpp
//
// An implementation of Plan13 in C++ by Mark VandeWettering
//
// Plan13 is an algorithm for satellite orbit prediction first formulated
// by James Miller G3RUH.  I learned about it when I saw it was the basis 
// of the PIC based antenna rotator project designed by G6LVB.
//
// http://www.g6lvb.com/Articles/LVBTracker2/index.htm
//
// I ported the algorithm to Python, and it was my primary means of orbit
// prediction for a couple of years while I operated the "Easy Sats" with 
// a dual band hand held and an Arrow antenna.
//
// I've long wanted to redo the work in C++ so that I could port the code
// to smaller processors including the Atmel AVR chips.  Bruce Robertson,
// VE9QRP started the qrpTracker project to fufill many of the same goals,
// but I thought that the code could be made more compact and more modular,
// and could serve not just the embedded targets but could be of more
// use for more general applications.  And, I like the BSD License a bit
// better too.
//
// So, here it is!
//

//----------------------------------------------------------------------

#include "WProgram.h"

// here are a bunch of constants that will be used throughout the 
// code, but which will probably not be helpful outside.

static const double RE = 6378.137f ;
static const double FL = 1.f/298.257224f ;
static const double GM = 3.986E5f ;
static const double J2 = 1.08263E-3f ;
static const double YM = 365.25f ;
static const double YT = 365.2421874f ;
static const double WW = 2.f*M_PI/YT ;
static const double WE = 2.f*M_PI+ WW ;
static const double W0 = WE/86400.f ;
static const double YG = 2000.f ;
static const double G0 = 98.9821f ;
static const double MAS0 = 356.0507f ;
static const double MASD = 0.98560028f ;
static const double EQC1 = 0.03342 ;
static const double EQC2 = 0.00035 ;
static const double INS = radians(23.4393f) ;
static const double CNS = cos(INS) ;
static const double SNS = sin(INS) ;

//----------------------------------------------------------------------

// the original BASIC code used three variables (e.g. Ox, Oy, Oz) to
// represent a vector quantity.  I think that makes for slightly more
// obtuse code, so I going to collapse them into a single variable 
// which is an array of three elements

typedef double Vec3[3] ;

//----------------------------------------------------------------------

class DateTime {
public:
	long DN ;
 	double TN ;
   	DateTime(int year, int month, int day, int h, int m, int s) ;
	DateTime(const DateTime &) ;
	DateTime() ;
	~DateTime() { } 
	void add(double) ;
   	void settime(int year, int month, int day, int h, int m, int s) ;
        void gettime(int& year, int& mon, int& day, int& h, int& m, int& s) ;
        void ascii(char *) ;
	void roundup(double) ;
} ;

//----------------------------------------------------------------------

class Observer {
public:
    const char *name ;
    double LA ;
    double LO ;
    double HT ;
    Vec3 U, E, N, O, V  ;
    
    Observer(const char *, double, double, double) ;
    ~Observer() { } ;
} ;

//----------------------------------------------------------------------


class Satellite { 
  	long N ;
	long YE ;	
        long DE ;
	double TE ;
	double IN ;
	double RA ;
	double EC ;
	double WP ;
	double MA ;
	double MM ;
	double M2 ;
	double RV ;
	double ALON ;
	double ALAT ;


	// these values are stored, but could be calculated on the fly
	// during calls to predict() 
	// classic space/time tradeoff

        double N0, A_0, B_0 ;
        double PC ;
        double QD, WD, DC ;
        double RS ;

public:
        const char *name ;
	Vec3 SAT, VEL ;		// celestial coordinates
    	Vec3 S, V ; 		// geocentric coordinates
 
	Satellite() { } ;
	Satellite(const char *name, const char *l1, const char *l2) ;
	~Satellite() ;
        void tle(const char *name, const char *l1, const char *l2) ;
        void predict(const DateTime &dt) ;
 	void LL(double &lat, double &lng) ;
	void altaz(const Observer &obs, double &alt, double &az) ;
} ;

class Sun {
public:
	Vec3 SUN, H ;
	Sun() ;
	~Sun() { } ;
        void predict(const DateTime &dt) ;
 	void LL(double &lat, double &lng) ;
	void altaz(const Observer &obs, double &alt, double &az) ;
} ;
