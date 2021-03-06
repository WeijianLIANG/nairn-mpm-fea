/********************************************************************************
	SmoothStep3.cpp
	nairn-mpm-fea
 
	Softening law is linear f(delta) = 1 - S1[delta/deltamax]
 
    where S1[x] = -2 x^3 + 3 x^2 is the cubic smooth step function. This
    function has properties f(0)=1, f(deltamax) = 0, f'(0)=0, and
    f'(deltamax) = 0
 
	s = gscaling = Ac/(Vp rho sigma(sp))
 
	Created by John Nairn, Dec 25, 2016.
	Copyright (c) 2016 John A. Nairn, All rights reserved.
 *******************************************************************************/

#include "stdafx.h"
#include "Materials/SmoothStep3.hpp"

#pragma mark SmoothStep3::Methods

// This law is expnential such that f(delta) = exp(-k*delta)
double SmoothStep3::GetFFxn(double delta,double gScaling) const
{	double deltaMax = 2.*Gc*gScaling;
    double x = delta/deltaMax;
    return 1.+x*x*(2.*x-3.);
}

// This law is exponential such that such that f'(delta) = -k*exp(-k*delta)
double SmoothStep3::GetFpFxn(double delta,double gScaling) const
{	double deltaMax = 2.*Gc*gScaling;
    double x = delta/deltaMax;
    return -6.*x*(1.-x)/deltaMax;
}

// Get energy released (per unit volume per unit stress or Gbar/sigma) up to delta.
double SmoothStep3::GetGToDelta(double delta,double gScaling) const
{	double deltaMax = 2.*Gc*gScaling;
    double x = delta/deltaMax;
    return 0.5*delta*(1. + x*x*(1. - x));
}

// Get G/Gc up to delta or Gbar(delta)/Gbar(infinity)
double SmoothStep3::GetGoverGc(double delta,double gScaling) const
{	double deltaMax = 2.*Gc*gScaling;
    double x = delta/deltaMax;
    return x*(1. + x*x*(1. - x));
}

#ifdef SS3_ANALYTICAL

#define CUBEROOT3 1.442249570307408
#define CUBEROOT32 2.080083823051904

// Solve for increment in crack opening strain by solving de = ddelta + e0*(f(delta+ddelta)-f(delta))
// Return ddelta if not failed or -1 if failed
double SmoothStep3::GetDDelta(double de,double e0,double delta,double gScaling) const
{	double deltaMax = 2.*Gc*gScaling;
    double x = delta/deltaMax;
    double arg = (1.-2.*x)*deltaMax - 2.*de - (1.+4.*x*x*(x-1.5))*e0;
    double arg2 = 2.*deltaMax - 3.*e0;
    double denom = pow(9.*e0*e0*arg + e0*sqrt(3.*e0*(arg2*arg2*arg2 + 27.*e0*arg*arg)),1./3.);
    double ddelta = 0.5*deltaMax*(1.-2.*x+arg2/(CUBEROOT3*denom)-(denom/(CUBEROOT32*e0)));
    
    // has it failed
    if(delta+ddelta>deltaMax) ddelta = -1.;
    return ddelta;
}

#endif

// Get maximium decreasing slope (max(-f'(delta)) for this softening law
double SmoothStep3::GetMaxSlope(double gScaling) const
{	double deltaMax = 2.*Gc*gScaling;
    return 1.5/deltaMax;
}

#pragma mark SmoothStep3::Accessors

// maximum delta
double SmoothStep3::GetDeltaMax(double gScaling) const { return 2.*Gc*gScaling; }

// initiation law name
const char *SmoothStep3::GetSofteningLawName(void) const { return "Cubic step function softening"; }
