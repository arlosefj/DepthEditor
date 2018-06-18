// CSuperpixelSeg.h: interface for the CSuperpixelSeg class.
//===========================================================================
// This code implements the superpixel segmentation method described in:
//
// Radhakrishna Achanta, Appu Shaji, Kevin Smith, Aurelien Lucchi, Pascal Fua, and Sabine Susstrunk,
// "SLIC Superpixels",
// EPFL Technical Report no. 149300, June 2010.
//===========================================================================
//	Copyright (c) 2010 Radhakrishna Achanta [EPFL].
//===========================================================================
// Email: firstname.lastname@epfl.ch
//===========================================================================
//	  This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//===========================================================================	

//////////////////////////////////////////////////////////////////////

#if !defined(_CSuperpixelSeg_H_INCLUDED_)
#define _CSuperpixelSeg_H_INCLUDED_

#include "common.h"

#include <vector>
#include <string>
#include <algorithm>
using namespace std;


class CSuperpixelSeg  
{
public:
	CSuperpixelSeg();
	virtual ~CSuperpixelSeg();
	//============================================================================
	// Superpixel segmentation for a given number of superpixels
	//============================================================================
	void DoSuperpixelSegmentation_ForGivenK(
		const unsigned char*			ubuff,
		const int					width,
		const int					height,
		int*						klabels,
		int&						numlabels,
		const int&					K,
		const double&				m);
	void DoSuperpixelSegmentation_ForGivenK(
		const unsigned char*			ubuff,
		const int					width,
		const int					height,
		int*						klabels,
		int&						numlabels,
		const int&					K,
		const double&				m,
		const int&					lb,
		int*						labelstmp);


	//============================================================================
	// Function to draw boundaries around superpixels of a given 'color'.
	// Can also be used to draw boundaries around supervoxels, i.e layer by layer.
	//============================================================================
	void DrawContoursAroundSegments(
		unsigned char*				segmentedImage,
		const int*					labels,
		const int&					width,
		const int&					height,
		const unsigned int&			color );
	void DrawContoursAroundSegmentsEnlarge(
		unsigned char*			ubuff,
		const int*				labels,
		const int&				width,
		const int&				height,
		const unsigned int& 	color );

private:
	//============================================================================
	// The main CSuperpixelSeg algorithm for generating superpixels
	//============================================================================
	void PerformSuperpixelSLIC(
		vector<double>&				kseedsl,
		vector<double>&				kseedsa,
		vector<double>&				kseedsb,
		vector<double>&				kseedsx,
		vector<double>&				kseedsy,
		int*						klabels,
		const int&					STEP,
		const vector<double>&		edgemag,
		const double&				m);
	void PerformSuperpixelSLIC(
		vector<double>& 			kseedsl,
		vector<double>& 			kseedsa,
		vector<double>& 			kseedsb,
		vector<double>& 			kseedsx,
		vector<double>& 			kseedsy,
		int*						klabels,
		const int&					STEP,
		const vector<double>&		edgemag,
		const double&				M,
		const int&					lb,
		int *						labelstmp);

	//============================================================================
	// Pick seeds for superpixels when number of superpixels is input.
	//============================================================================
	void GetLABXYSeeds_ForGivenK(
		vector<double>&				kseedsl,
		vector<double>&				kseedsa,
		vector<double>&				kseedsb,
		vector<double>&				kseedsx,
		vector<double>&				kseedsy,
		const int&					STEP,
		const bool&					perturbseeds,
		const vector<double>&		edges);

	//============================================================================
	// Move the seeds to low gradient positions to avoid putting seeds at region boundaries.
	//============================================================================
	void PerturbSeeds(
		vector<double>&				kseedsl,
		vector<double>&				kseedsa,
		vector<double>&				kseedsb,
		vector<double>&				kseedsx,
		vector<double>&				kseedsy,
		const vector<double>&		edges);
	//============================================================================
	// Detect color edges, to help PerturbSeeds()
	//============================================================================
	void DetectLabEdges(
		const double*				lvec,
		const double*				avec,
		const double*				bvec,
		const int&					width,
		const int&					height,
		vector<double>&				edges);
	//============================================================================
	// xRGB to XYZ conversion; helper for RGB2LAB()
	//============================================================================
	void RGB2XYZ(
		const int&					sR,
		const int&					sG,
		const int&					sB,
		double&						X,
		double&						Y,
		double&						Z);
	//============================================================================
	// sRGB to CIELAB conversion
	//============================================================================
	void RGB2LAB(
		const int&					sR,
		const int&					sG,
		const int&					sB,
		double&						lval,
		double&						aval,
		double&						bval);
	//============================================================================
	// sRGB to CIELAB conversion for 2-D images
	//============================================================================
	void DoRGBtoLABConversion(
		const unsigned char*&		ubuff,
		double*&					lvec,
		double*&					avec,
		double*&					bvec);

	//============================================================================
	// Post-processing of CSuperpixelSeg segmentation, to avoid stray labels.
	//============================================================================
	void EnforceLabelConnectivity(
		const int*					labels,
		const int&					width,
		const int&					height,
		int*						nlabels,//input labels that need to be corrected to remove stray labels
		int&						numlabels,//the number of labels changes in the end if segments are removed
		const int&					K); //the number of superpixels desired by the user
	void EnforceLabelConnectivity(
		const int*					labels,
		const int&					width,
		const int&					height,
		int*						nlabels,
		int&						numlabels,
		const int&					K,
		const int&					lb,
		int *						labelstmp);

	//============================================================================
	// Find next superpixel label; helper for EnforceLabelConnectivity()
	//============================================================================
	void FindNext(
		const int*					labels,
		int*						nlabels,
		const int&					height,
		const int&					width,
		const int&					h,
		const int&					w,
		const int&					lab,
		int*						xvec,
		int*						yvec,
		int&						count);

private:
	int										m_width;
	int										m_height;
	int										m_depth;

	double*									m_lvec;
	double*									m_avec;
	double*									m_bvec;

};

#endif // !defined(_CSuperpixelSeg_H_INCLUDED_)

