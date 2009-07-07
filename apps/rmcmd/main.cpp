/*!
  \file main.cpp

  \ingroup RM

  \brief RM-Synthesis test
 
  \author Sven Duscha
 
  \date 28.01.09.

  <h3>Synopsis</h3>
  RM-synthesis command line tool. Using algorithms provided in librm.a library. I/O of FITS format
  through casacore high-level functions.
*/

#include <iostream>									// STL iostream

#include "../../implement/rm.h"					// RM Synthesis class
#include "../../implement/rmCube.h"				// RM Cube class
#include "../../../DAL/implement/dalFITS.h"

// casa includes (from /usr/local/include/casacore)
/*
#include <casa/Arrays.h>
#include <casa/Arrays/Array.h>
#include <casa/Logging/LogIO.h>
#include <casa/Utilities/DataType.h>
#include <images/Images/FITSImage.h>				// high-level FITSImage interface
#include <tables/Tables/TiledFileAccess.h>
#include <lattices/Lattices/TiledShape.h>
#include <lattices/Lattices/LatticeBase.h>
#include <lattices/Lattices/LatticeIterator.h>			// Iterator over lattices
#include <images/Images/ImageOpener.h>				// wrapper class for Image type opening
#include <images/Images/ImageFFT.h>
#include <images/Images/ImageStatistics.h>
#include <images/Images/ImageInterface.h>
*/

#define _debug	

using namespace std;
//using namespace casa;					// casa(core) namespace
using namespace DAL;

int main (int argc, char * const argv[]) {

	try{

//	Bool status;					// status of casa calls

	unsigned int i=0;				// loop variable
	unsigned int N=100;			// length of test vectors


	if(argc<3)	// if no filenames was given, display usage/help (MUST change to 3!)
	{
		cout << "Usage: " << argv[0] << " <Q.fits> <U.fits>" << endl;
		cout << "Computes the RM cubes in Q,U (and Q+iU) from two polarized input" << endl;
		cout << "FITS images." << endl;		
		return(0);
	}

	const string filename_Q=argv[1];	// get filename for Q image from command line
	const string filename_U=argv[2];	// get filename for U image from command line
	
	#ifdef _debug
	cout << "Filename_Q: " << filename_Q << endl;		// Debug output
	cout << "Filename_U: " << filename_U << endl;
	#endif




	// TEST: inverseFourier RM-Synthesis
	// compute 300 pts along 0 to 299
	vector<double> phis(N);
	vector<complex<double> > intensities(N);
	vector<double> weights(N);	// weights
	vector<double> freq(N);
	vector<double> delta_freq(N);
	vector<double> lambda_squared(N);
	vector<double> delta_lambda_squared(N);
	
	vector<complex<double> > rmsf(N);
	vector<complex<double> > rmpol(N);		// polarized intensity at Faraday depth probed for

	// Need to test lambdaSquaredTopHat function with WSRT text file
	rmCube rm(1024, 1024, 100, 5);
	
	// Faradayd depths to probe for
	for(unsigned int i=0; i<N; i++)
	{
		phis[i]=i-150;							// from -150 to +150
	}
	
	// Load simulated Brentjens data from file
	rm.readSimDataFromFile("sim4Brentjens.dat", lambda_squared, delta_lambda_squared, intensities);
	
	// perform RM-Synthesis
	rmsf=rm.inverseFourier(phis, intensities, lambda_squared, weights, delta_lambda_squared, 0);
	rm.writeRMtoFile(phis, rmsf, "rmpolBrentjens.dat");
	
	// URGENT: need to verify with above deltaLambdas computation of RMSF!
	rmsf=rm.RMSF(phis, lambda_squared, weights, delta_lambda_squared);
	rm.writeRMtoFile(phis, rmsf, "rmsf.dat");		// debug write RMSF to file

	// Open Image and set parameters
	dalFITS fitsimage("Leiden_GEETEE_CS1_1.FITS", READONLY);
	fitsimage.moveAbsoluteHDU(1);

	
	int naxis=0;
	long naxes[3]={0,0,0};
	
	
	naxis=fitsimage.getImgDim();
	fitsimage.getImgSize(3 , naxes);
	
	double *buffer=new double[naxes[0]*naxes[1]];


	// create a rmCube object with an associated two dimensional buffer
	rmCube FaradayCube(1024, 1024, 100, 5.0);
	FaradayCube.setFaradayDepths(-100, 100, 10); // Set up Faraday depths to be probed
	// Test file read functions
	freq=FaradayCube.readFrequenciesAndDeltaFrequencies("frequenciesDelta.txt", delta_freq);


	// Generate Rectangular pulse in f (real)

	
	for(i=0; i<weights.size(); i++)
	{
	  weights[i]=1.0;	// weight them all equally
	}
	
	
	//fitsimage.readPlane(buffer, 1);
	
	
	// call inverseFourier with phi=5
	for(i=0; i<N; i++)
	{
	}
	
	#ifdef _debug
	cout << "Finished: " << filename_Q << "!\n";	// Debug output
	#endif
	
	return 0;
	
	}
	
	catch(const char *s)
	{
	  cout << s << endl;
	}
	
}
