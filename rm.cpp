/*! Implementation of rm class methods

    Author:		Sven Duscha (sduscha@mpa-garching.mpg.de)
    Date:		18-12-2008
    Last change:	16-04-2009
*/


#include <iostream>				// C++/STL iostream
#include <math.h>					// mathematics library

#include <casa/Arrays.h>			// CASA library functions
#include <casa/Arrays/Array.h>
#include <casa/Utilities/DataType.h>
#include <tables/Tables/TiledFileAccess.h>
#include <lattices/Lattices/TiledShape.h>

#include "rm.h"					// rm class declarations

using namespace std;
using namespace casa;		// namespace for functions from casacore

//===============================================================================
//
//  Constructions / Destruction
//
//===============================================================================

/*!
  \brief Declare a new RM cube of size x (right ascension), y (declination), and depth and stepsize in Faraday depth

  \param x, y, depth, stepsize, os -- 
 
  \return none --
*/

rm::rm(int x, int y, int depth, double stepsize, ofstream &file)	// pass only reference to output stream!
{
	// declare RM cube of desired size and assign output stream
	
	
}

/*!
  \brief rm class destructor

*/

rm::~rm()
{ 
  // Free memory? Currently nothing to be done
  cout << "rm destructor called" << endl;
}

//===============================================================================
//
//  Methods
//
//===============================================================================

/*!
  \brief Convert from frequencies to lambda squared

  \param frequency -- 
  \param lambda_sq --

  \return success -- 
*/
//vector<double>rm::freqToLambdaSq(vector<double> &frequency);
bool rm::freqToLambdaSq(const vector<double> &frequency, vector<double> &lambda_sq)
{
  // constants
  double csq=89875517873681764.0;	// c^2
 
  // Check for data consistency
  if(frequency.size()==0)
  {
    cerr << "rm::freqToLambdaSq: frequency vector size 0" << endl;
    return false;
  }
  else if(frequency.size() != lambda_sq.size())
  {
    cerr << "rm::freqToLambdaSq: frequency and labda_sq vector sizes do not match" << endl;
    return false;
  }

  // loop over frequency vector and compute lambda squared values
  for(unsigned int i=0; i<frequency.size(); i++)
  {
     lambda_sq[i]=csq/(frequency[i]*frequency[i]);
     cout << "frequency = " << frequency[i] << "  lambda^2 = " << lambda_sq[i] << endl;
  }
  return true;
}


/*!
  \brief Convert from lambda squared to frequencies
  
  \param lambda_sq -- 
  \param frequency --
  
  \return succes -- 
*/
bool rm::lambdaSqToFreq(const vector<double> &lambda_sq, vector<double> &frequency)
{
    // constants
    double csq=89875517873681764.0;	// c^2

    // Check for data consistency
    if(lambda_sq.size()==0)
    {
      cerr << "rm::lambdaSqToFreq: lambda_sq vector size 0" << endl;
      return false;
    }
    else if(lambda_sq.size() != frequency.size())
    {
      cerr << "rm::freqToLambdaSq: frequency and labda_sq vector sizes do not match" << endl;
      return false;
    }

    // loop over frequency vector and compute lambda squared values
    for(unsigned int i=0; i<lambda_sq.size(); i++)
    {
      frequency[i]=sqrt(csq/(lambda_sq[i]));
      cout << "lambda^2 = " << lambda_sq[i] << "  frequency = " << frequency[i]  << endl;
    }
    return true;
}


/*!
    Unit conversion functions are needed:
    
    Jy/beam, total integrated flux, flux per channel
    need to know delta f of each channel for integration
    RM-Imager: uv data is V^2 (Voltage squared)
*/


/*! Calculate the total intensity integrated over all channels

  \param frequencies --
  \param totalIntensity --
  \param unit --
*/

bool rm:totalIntensity(vector<double> frequencies, double &totalIntensity, string unit)
{

  // integrate over all frequencies in channel
  
  
  // convert to desired unit (given by string unit)

  return true;
}



/*! Calculate the step size delta_lambda_sq for each lambda_squared channel
    
    \param freq_low --
    \param freq_high --
    \param freq
    
    \return delta_lambda_sq --
*/
vector<double> rm::deltaLambdaSq( //vector<double> delta_lambda_sq, 
			const vector<double> &freq_low, 
			const vector<double> &freq_high,
			bool freq)
{
  // vector containing the converted lambda squared bins
  vector<double> delta_lambda_sq;

  // lambda vectors for conversion
  vector<double> lambda_low(freq_high.size()), lambda_high(freq_low.size());


  // check for valid parameters
  if(freq_low.size()==0 || freq_high.size()==0)		// either of the limit vectors is zero
  {
    cerr << "rm::deltaLambdaSq: freq_low or freq_high vector 0" << endl;
    return false;
  }
  else if(freq_low.size()!=freq_high.size())		// freq_low and freq_high differ in size
  {
    throw "rm::deltaLambdaSq: freq_low and freq_high vector differ in size";
  }
  else
  {
    // if frequencies are given (i.e. bool freq=true, default)
    // convert lower and higher frequencies to lambda squared
    if(freq)
    {
      if(freqToLambdaSq(&lambda_low, freq_high)==false)
      {
	throw << "rm:deltaLambdaSq: conversion of freq_high to lambda_sq failed";
      }
      else if(freqToLambdaSq(&lambda_high, freq_low)==false)
      {
	throw "rm:deltaLambdaSq: conversion of freq_low to lambda_sq failed";
      }
    }
    else	// if limits were actually already given as lambda squareds just use these
    {
      lambda_low=freq_low;
      lambda_high=freq_high; 
    }

    // compute difference between higher lambda and lower lambda (other way around than frequencies)
    for(unsigned int i=0; i < freq_low.size(); i++)
    {
      delta_lambda_sq[i]=lambda_high[i]-lambda_low[i];	// compute difference
      
      #ifdef _debug
      cout << "delta_lambda_sq[" << i << "] = " << delta_lambda_sq[i] << endl;	// debug output
      #endif
    }

  }

  return delta_lambda_squared;		// return delta lambda squared vector
}


/*! \brief Inverse Fourier Method for calculating the Rotation Measure
    
    The inverse Fourier Transformation is the classical relation between the lambda squared space and Faraday
    space. Input images channel are not necessarily given in lambda squared wavelengths, but in frequency instead. If that is the case, a conversion function between lambda squared and frequency is used.
    
    Normal case (bool freq=true, default): the input vector is a range of complex polarized 
    intensities measured at different frequencies 
    Special case (bool freq=false): the input vector is already given as a range of polarized intensities given at different lambda squareds

    \param intensity --
    \param frequency --
    \param delta_freq --
    \param freq --
    
    \return rm --
*/
vector<double>rm::inverseFourier(vector<double> intensity,
				 vector<double> frequency,
				 vector<double> delta_freq,
				 bool freq)
{
	vector<double>rm;	// rm values along the line of sight


	if(freq)		// if given as frequency, convert to lambda squareds
	{
	  convertToLambdaSquared()	// convert frequency vector
	  
					// convert delta step vector
	
	}

	return rm;
}



/*!
  \brief Wavelet Transform Method for calculating the Rotation Measure

  Normal case (bool freq=true, default): the input vector is a range of complex
  polarized intensities measured at different frequencies. Parameters a_min/a_max
  and b_min/b_max define ranges for wavelet parameter space probing.

  \param intensity -- 
  \param frequencies --
  \param delta_freq --
  \param wavelet_parameters -- 
  \param freq -- 
  
  \return rm --
 */
vector<double> rm::wavelet(vector<double> intensity,
			   vector<double> frequencies,
			   vector<double> delta_freqs,
			   vector<double> wavelet_parameters,
			   bool freq)
{
  vector<double>rm;	// rm values along the line of sight
  
  
  return rm;
}


// High-Level access functions to compute RM tiles/cubes

// Wrapper function to perform RM synthesis on a tile of an image:
// A section (or whole) image is taken as input, using the casacore LatticeBase class,
// the algorithm used to compute the RM cube is specified via a function pointer
// also implement multithreading into that?
//

//Lattice<double> rm::rmOnLattice(Lattice<double> imagetile,  algorithm)
//{
//	LatticeBase *lattice;
//
//
//
//	return *lattice;
//}


// Error calculation algorithms

/*! \brief Calculate RM error using LSQ approximation
    
    Input parameters are the vector of polarized intensities, along with a vector describing
    their frequency distribution; if bool freq is set to false, these are assumed to be
    given in lambda squared values
    
    \param intensity --
    \param lambda_sqs --
    \param lambda_sq --
    
    \return rm_error --
*/

vector<double> rm::rmErrorLsq(vector<double> intensity, vector<double> lambda_sqs, bool lambda_sq)
{
	vector<double> rm_error;


	return rm_error;
}


/*! \brief Calculate RM error using Bayesian statistics

    Input parameters are the vector of polarized intensities, along with a vector describing
    their frequency distribution; if bool freq is set to false, these are assumed to be
    given in lambda squared values
    
    \param intensity --
    \param lambda_sqs --
    \param lambda_sq --
    
    \return rm_error --
*/

vector<double> rm::rmErrorBayes(vector<double> intensity, vector<double> lambda_sqs, bool lambda_sq)
{
	vector<double> rm_error;


	return rm_error;
}
