// CSuperpixelSeg.cpp: implementation of the SLIC class.
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
//////////////////////////////////////////////////////////////////////
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

#include <cfloat>
#include <cmath>
#include <iostream>
#include <fstream>
#include "CSuperpixelSeg.h"

// For superpixels
const int dx4[4] = {-1,  0,  1,  0};
const int dy4[4] = { 0, -1,  0,  1};
//const int dx8[8] = {-1, -1,  0,  1, 1, 1, 0, -1};
//const int dy8[8] = { 0, -1, -1, -1, 0, 1, 1,  1};

// For supervoxels
const int dx10[10] = {-1,  0,  1,  0, -1,  1,  1, -1,  0, 0};
const int dy10[10] = { 0, -1,  0,  1, -1, -1,  1,  1,  0, 0};
const int dz10[10] = { 0,  0,  0,  0,  0,  0,  0,  0, -1, 1};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSuperpixelSeg::CSuperpixelSeg()
{
	m_lvec = NULL;
	m_avec = NULL;
	m_bvec = NULL;
}

CSuperpixelSeg::~CSuperpixelSeg()
{
	if(m_lvec) delete [] m_lvec;
	if(m_avec) delete [] m_avec;
	if(m_bvec) delete [] m_bvec;

}

//==============================================================================
///	RGB2XYZ
///
/// sRGB (D65 illuninant assumption) to XYZ conversion
//==============================================================================
void CSuperpixelSeg::RGB2XYZ(
	const int&		sR,
	const int&		sG,
	const int&		sB,
	double&			X,
	double&			Y,
	double&			Z)
{
	double R = sR/255.0;
	double G = sG/255.0;
	double B = sB/255.0;

	double r, g, b;

	if(R <= 0.04045)	r = R/12.92;
	else				r = pow((R+0.055)/1.055,2.4);
	if(G <= 0.04045)	g = G/12.92;
	else				g = pow((G+0.055)/1.055,2.4);
	if(B <= 0.04045)	b = B/12.92;
	else				b = pow((B+0.055)/1.055,2.4);

	X = r*0.4124564 + g*0.3575761 + b*0.1804375;
	Y = r*0.2126729 + g*0.7151522 + b*0.0721750;
	Z = r*0.0193339 + g*0.1191920 + b*0.9503041;
}

//===========================================================================
///	RGB2LAB
//===========================================================================
void CSuperpixelSeg::RGB2LAB(const int& sR, const int& sG, const int& sB, double& lval, double& aval, double& bval)
{
	//------------------------
	// sRGB to XYZ conversion
	//------------------------
	double X, Y, Z;
	RGB2XYZ(sR, sG, sB, X, Y, Z);

	//------------------------
	// XYZ to LAB conversion
	//------------------------
	double epsilon = 0.008856;	//actual CIE standard
	double kappa   = 903.3;		//actual CIE standard

	double Xr = 0.950456;	//reference white
	double Yr = 1.0;		//reference white
	double Zr = 1.088754;	//reference white

	double xr = X/Xr;
	double yr = Y/Yr;
	double zr = Z/Zr;

	double fx, fy, fz;
	if(xr > epsilon)	fx = pow(xr, 1.0/3.0);
	else				fx = (kappa*xr + 16.0)/116.0;
	if(yr > epsilon)	fy = pow(yr, 1.0/3.0);
	else				fy = (kappa*yr + 16.0)/116.0;
	if(zr > epsilon)	fz = pow(zr, 1.0/3.0);
	else				fz = (kappa*zr + 16.0)/116.0;

	lval = 116.0*fy-16.0;
	aval = 500.0*(fx-fy);
	bval = 200.0*(fy-fz);
}

//===========================================================================
///	DoRGBtoLABConversion
///
///	For whole image: overlaoded floating point version
//===========================================================================
void CSuperpixelSeg::DoRGBtoLABConversion(
	const unsigned char*&		ubuff,
	double*&					lvec,
	double*&					avec,
	double*&					bvec)
{
	int sz = m_width*m_height;
	lvec = new double[sz];
	avec = new double[sz];
	bvec = new double[sz];

	for( int j = 0; j < sz; j++ )
	{
		int r = (int)ubuff[j*3] ;
		int g = (int)ubuff[j*3+1] ;
		int b = (int)ubuff[j*3+2] ;

		RGB2LAB( r, g, b, lvec[j], avec[j], bvec[j] );
	}
}

//=================================================================================
/// DrawContoursAroundSegments
///
/// Internal contour drawing option exists. One only needs to comment the if
/// statement inside the loop that looks at neighbourhood.
//=================================================================================
void CSuperpixelSeg::DrawContoursAroundSegments(
	unsigned char*			ubuff,
	const int*				labels,
	const int&				width,
	const int&				height,
	const unsigned int&		color )
{
	const int dx8[8] = {-1, -1,  0,  1, 1, 1, 0, -1};
	const int dy8[8] = { 0, -1, -1, -1, 0, 1, 1,  1};

	int sz = width*height;

	vector<bool> istaken(sz, false);

	int mainindex(0);
	for( int j = 0; j < height; j++ )
	{
		for( int k = 0; k < width; k++ )
		{
			int np(0);
			for( int i = 0; i < 8; i++ )
			{
				int x = k + dx8[i];
				int y = j + dy8[i];

				if( (x >= 0 && x < width) && (y >= 0 && y < height) )
				{
					int index = y*width + x;

					if( false == istaken[index] )//comment this to obtain internal contours
					{
						if( labels[mainindex] != labels[index] ) np++;
					}
				}
			}
			if( np > 1 )//change to 2 or 3 for thinner lines
			{
				ubuff[mainindex*3] = (color>>16)&0xFF;
				ubuff[mainindex*3+1] = (color>>8)&0xFF;
				ubuff[mainindex*3+2] = (color>>0)&0xFF;
				istaken[mainindex] = true;
			}
			mainindex++;
		}
	}
}

void CSuperpixelSeg::DrawContoursAroundSegmentsEnlarge(
	unsigned char*			ubuff,
	const int*				labels,
	const int&				width,
	const int&				height,
	const unsigned int&		color )
{
	int i, j, k;
	const int dx8[8] = {-1, -1,  0,  1, 1, 1, 0, -1};
	const int dy8[8] = { 0, -1, -1, -1, 0, 1, 1,  1};

	int sz = width*height*4;

	int * labelstmp = new int[sz];

	for(j=0;j<height;j++)
		for(i=0;i<width;i++)
		{
			labelstmp[2*i+2*j*width*2] = labelstmp[2*i+1+2*j*width*2] = 
			labelstmp[2*i+2*j*width*2+width*2] = labelstmp[2*i+1+2*j*width*2+width*2] = labels[i+j*width];
		}

	vector<bool> istaken(sz, false);

	int mainindex(0);
	for( j = 0; j < height*2; j++ )
	{
		for( k = 0; k < width*2; k++ )
		{
			int np(0);
			for( i = 0; i < 8; i++ )
			{
				int x = k + dx8[i];
				int y = j + dy8[i];

				if( (x >= 0 && x < width*2) && (y >= 0 && y < height*2) )
				{
					int index = y*width*2 + x;

					if( false == istaken[index] )//comment this to obtain internal contours
					{
						if( labelstmp[mainindex] != labelstmp[index] ) np++;
					}
				}
			}
			if( np > 1 )//change to 2 or 3 for thinner lines
			{
				ubuff[mainindex*3] = (color>>16)&0xFF;
				ubuff[mainindex*3+1] = (color>>8)&0xFF;
				ubuff[mainindex*3+2] = (color>>0)&0xFF;
				istaken[mainindex] = true;
			}
			mainindex++;
		}
	}

	delete [] labelstmp;
}


//==============================================================================
///	DetectLabEdges
//==============================================================================
void CSuperpixelSeg::DetectLabEdges(
	const double*				lvec,
	const double*				avec,
	const double*				bvec,
	const int&					width,
	const int&					height,
	vector<double>&				edges)
{
	int sz = width*height;

	edges.resize(sz,0);
	for( int j = 1; j < height-1; j++ )
	{
		for( int k = 1; k < width-1; k++ )
		{
			int i = j*width+k;

			double dx = (lvec[i-1]-lvec[i+1])*(lvec[i-1]-lvec[i+1]) +
						(avec[i-1]-avec[i+1])*(avec[i-1]-avec[i+1]) +
						(bvec[i-1]-bvec[i+1])*(bvec[i-1]-bvec[i+1]);

			double dy = (lvec[i-width]-lvec[i+width])*(lvec[i-width]-lvec[i+width]) +
						(avec[i-width]-avec[i+width])*(avec[i-width]-avec[i+width]) +
						(bvec[i-width]-bvec[i+width])*(bvec[i-width]-bvec[i+width]);

			//edges[i] = (sqrt(dx) + sqrt(dy));
			edges[i] = (dx + dy);
		}
	}
}

//===========================================================================
///	PerturbSeeds
//===========================================================================
void CSuperpixelSeg::PerturbSeeds(
	vector<double>&				kseedsl,
	vector<double>&				kseedsa,
	vector<double>&				kseedsb,
	vector<double>&				kseedsx,
	vector<double>&				kseedsy,
	const vector<double>&		edges)
{
	const int dx8[8] = {-1, -1,  0,  1, 1, 1, 0, -1};
	const int dy8[8] = { 0, -1, -1, -1, 0, 1, 1,  1};
	
	int numseeds = kseedsl.size();

	for( int n = 0; n < numseeds; n++ )
	{
		int ox = (int)kseedsx[n];//original x
		int oy = (int)kseedsy[n];//original y
		int oind = oy*m_width + ox;

		int storeind = oind;
		for( int i = 0; i < 8; i++ )
		{
			int nx = ox+dx8[i];//new x
			int ny = oy+dy8[i];//new y

			if( nx >= 0 && nx < m_width && ny >= 0 && ny < m_height)
			{
				int nind = ny*m_width + nx;
				if( edges[nind] < edges[storeind])
				{
					storeind = nind;
				}
			}
		}
		if(storeind != oind)
		{
			kseedsx[n] = storeind%m_width;
			kseedsy[n] = storeind/m_width;
			kseedsl[n] = m_lvec[storeind];
			kseedsa[n] = m_avec[storeind];
			kseedsb[n] = m_bvec[storeind];
		}
	}
}


//===========================================================================
///	GetLABXYSeeds_ForGivenK
///
/// The k seed values are taken as uniform spatial pixel samples.
//===========================================================================
void CSuperpixelSeg::GetLABXYSeeds_ForGivenK(
	vector<double>&				kseedsl,
	vector<double>&				kseedsa,
	vector<double>&				kseedsb,
	vector<double>&				kseedsx,
	vector<double>&				kseedsy,
	const int&					K,
	const bool&					perturbseeds,
	const vector<double>&		edgemag)
{
	int sz = m_width*m_height;
	double step = sqrt(double(sz)/double(K));
	int T = (int)step;
	int xoff = (int)step/2;
	int yoff = (int)step/2;
	
	int n(0);
	for( int y = 0; y < m_height; y++ )
	{
		int Y = (int)(y*step + yoff);
		if( Y > m_height-1 ) break;

		for( int x = 0; x < m_width; x++ )
		{
			int X = (int)(x*step + xoff);
			if(X > m_width-1) break;

			int i = Y*m_width + X;

			//_ASSERT(n < K);
			
			//kseedsl[n] = m_lvec[i];
			//kseedsa[n] = m_avec[i];
			//kseedsb[n] = m_bvec[i];
			//kseedsx[n] = X;
			//kseedsy[n] = Y;
			kseedsl.push_back(m_lvec[i]);
			kseedsa.push_back(m_avec[i]);
			kseedsb.push_back(m_bvec[i]);
			kseedsx.push_back(X);
			kseedsy.push_back(Y);
			n++;
		}
	}

	if(perturbseeds)
	{
		PerturbSeeds(kseedsl, kseedsa, kseedsb, kseedsx, kseedsy, edgemag);
	}
}

//===========================================================================
///	PerformSuperpixelSLIC
///
///	Performs k mean segmentation. It is fast because it looks locally, not
/// over the entire image.
//===========================================================================
void CSuperpixelSeg::PerformSuperpixelSLIC(
	vector<double>&				kseedsl,
	vector<double>&				kseedsa,
	vector<double>&				kseedsb,
	vector<double>&				kseedsx,
	vector<double>&				kseedsy,
	int*						klabels,
	const int&					STEP,
	const vector<double>&		edgemag,
	const double&				M)
{
	int sz = m_width*m_height;
	const int numk = kseedsl.size();
	//----------------
	int offset = STEP;
	//----------------
	
	vector<double> clustersize(numk, 0);
	vector<double> inv(numk, 0);//to store 1/clustersize[k] values

	vector<double> sigmal(numk, 0);
	vector<double> sigmaa(numk, 0);
	vector<double> sigmab(numk, 0);
	vector<double> sigmax(numk, 0);
	vector<double> sigmay(numk, 0);
	vector<double> distvec(sz, DBL_MAX);

	double invwt = 1.0/((STEP/M)*(STEP/M));

	int x1, y1, x2, y2;
	double l, a, b;
	double dist;
	double distxy;
	for( int itr = 0; itr < g_segparamiterate; itr++ )
	{
		distvec.assign(sz, DBL_MAX);
		for( int n = 0; n < numk; n++ )
		{
			y1 = (int)H3D_MAX(0,			kseedsy[n]-offset);
			y2 = (int)H3D_MIN(m_height,	kseedsy[n]+offset);
			x1 = (int)H3D_MAX(0,			kseedsx[n]-offset);
			x2 = (int)H3D_MIN(m_width,	kseedsx[n]+offset);


			for( int y = y1; y < y2; y++ )
			{
				for( int x = x1; x < x2; x++ )
				{
					int i = y*m_width + x;

					l = m_lvec[i];
					a = m_avec[i];
					b = m_bvec[i];

					dist =			(l - kseedsl[n])*(l - kseedsl[n]) +
									(a - kseedsa[n])*(a - kseedsa[n]) +
									(b - kseedsb[n])*(b - kseedsb[n]);

					distxy =		(x - kseedsx[n])*(x - kseedsx[n]) +
									(y - kseedsy[n])*(y - kseedsy[n]);
					
					//------------------------------------------------------------------------
					dist += distxy*invwt;//dist = sqrt(dist) + sqrt(distxy*invwt);//this is more exact
					//------------------------------------------------------------------------
					if( dist < distvec[i] )
					{
						distvec[i] = dist;
						klabels[i]  = n;
					}
				}
			}
		}
		//-----------------------------------------------------------------
		// Recalculate the centroid and store in the seed values
		//-----------------------------------------------------------------
		//instead of reassigning memory on each iteration, just reset.
	
		sigmal.assign(numk, 0);
		sigmaa.assign(numk, 0);
		sigmab.assign(numk, 0);
		sigmax.assign(numk, 0);
		sigmay.assign(numk, 0);
		clustersize.assign(numk, 0);
		//------------------------------------
		//edgesum.assign(numk, 0);
		//------------------------------------

		{int ind(0);
		for( int r = 0; r < m_height; r++ )
		{
			for( int c = 0; c < m_width; c++ )
			{
				sigmal[klabels[ind]] += m_lvec[ind];
				sigmaa[klabels[ind]] += m_avec[ind];
				sigmab[klabels[ind]] += m_bvec[ind];
				sigmax[klabels[ind]] += c;
				sigmay[klabels[ind]] += r;
				//------------------------------------
				//edgesum[klabels[ind]] += edgemag[ind];
				//------------------------------------
				clustersize[klabels[ind]] += 1.0;
				ind++;
			}
		}}

		{for( int k = 0; k < numk; k++ )
		{
			if( clustersize[k] <= 0 ) clustersize[k] = 1;
			inv[k] = 1.0/clustersize[k];//computing inverse now to multiply, than divide later
		}}
		
		{for( int k = 0; k < numk; k++ )
		{
			kseedsl[k] = sigmal[k]*inv[k];
			kseedsa[k] = sigmaa[k]*inv[k];
			kseedsb[k] = sigmab[k]*inv[k];
			kseedsx[k] = sigmax[k]*inv[k];
			kseedsy[k] = sigmay[k]*inv[k];
			//------------------------------------
			//edgesum[k] *= inv[k];
			//------------------------------------
		}}
	}
}


void CSuperpixelSeg::PerformSuperpixelSLIC(
	vector<double>&				kseedsl,
	vector<double>&				kseedsa,
	vector<double>&				kseedsb,
	vector<double>&				kseedsx,
	vector<double>&				kseedsy,
	int*						klabels,
	const int&					STEP,
	const vector<double>&		edgemag,
	const double&				M,
	const int&					lb,
	int *						labelstmp)
{
	int sz = m_width*m_height;
	const int numk = kseedsl.size();
	//----------------
	int offset = STEP;
	//----------------
	
	vector<double> clustersize(numk, 0);
	vector<double> inv(numk, 0);//to store 1/clustersize[k] values

	vector<double> sigmal(numk, 0);
	vector<double> sigmaa(numk, 0);
	vector<double> sigmab(numk, 0);
	vector<double> sigmax(numk, 0);
	vector<double> sigmay(numk, 0);
	vector<double> distvec(sz, DBL_MAX);

	double invwt = 1.0/((STEP/M)*(STEP/M));

	int x1, y1, x2, y2;
	double l, a, b;
	double dist;
	double distxy;
	for( int itr = 0; itr < 10; itr++ ) // 为什么是10次迭代??????????????
	{
		distvec.assign(sz, DBL_MAX);
		for( int n = 0; n < numk; n++ )
		{
			y1 = (int)H3D_MAX(0,			kseedsy[n]-offset);
			y2 = (int)H3D_MIN(m_height,	kseedsy[n]+offset);
			x1 = (int)H3D_MAX(0,			kseedsx[n]-offset);
			x2 = (int)H3D_MIN(m_width,	kseedsx[n]+offset);


			for( int y = y1; y < y2; y++ )
			{
				for( int x = x1; x < x2; x++ )
				{
					int i = y*m_width + x;

					l = m_lvec[i];
					a = m_avec[i];
					b = m_bvec[i];

					dist =			(l - kseedsl[n])*(l - kseedsl[n]) +
									(a - kseedsa[n])*(a - kseedsa[n]) +
									(b - kseedsb[n])*(b - kseedsb[n]);

					distxy =		(x - kseedsx[n])*(x - kseedsx[n]) +
									(y - kseedsy[n])*(y - kseedsy[n]);
					
					//------------------------------------------------------------------------
					dist += distxy*invwt;//dist = sqrt(dist) + sqrt(distxy*invwt);//this is more exact
					//------------------------------------------------------------------------
					if( dist < distvec[i] )
					{
						distvec[i] = dist;
						klabels[i]  = n;
					}
				}
			}
		}
		//-----------------------------------------------------------------
		// Recalculate the centroid and store in the seed values
		//-----------------------------------------------------------------
		//instead of reassigning memory on each iteration, just reset.
	
		sigmal.assign(numk, 0);
		sigmaa.assign(numk, 0);
		sigmab.assign(numk, 0);
		sigmax.assign(numk, 0);
		sigmay.assign(numk, 0);
		clustersize.assign(numk, 0);
		//------------------------------------
		//edgesum.assign(numk, 0);
		//------------------------------------

		{int ind(0);
		for( int r = 0; r < m_height; r++ )
		{
			for( int c = 0; c < m_width; c++ )
			{
				if(labelstmp[c+r*m_width]==lb)
				{
					sigmal[klabels[ind]] += m_lvec[ind];
					sigmaa[klabels[ind]] += m_avec[ind];
					sigmab[klabels[ind]] += m_bvec[ind];
					sigmax[klabels[ind]] += c;
					sigmay[klabels[ind]] += r;
					//------------------------------------
					//edgesum[klabels[ind]] += edgemag[ind];
					//------------------------------------
					clustersize[klabels[ind]] += 1.0;
					ind++;
				}
			}
		}}

		{for( int k = 0; k < numk; k++ )
		{
			if( clustersize[k] <= 0 ) clustersize[k] = 1;
			inv[k] = 1.0/clustersize[k];//computing inverse now to multiply, than divide later
		}}
		
		{for( int k = 0; k < numk; k++ )
		{
			kseedsl[k] = sigmal[k]*inv[k];
			kseedsa[k] = sigmaa[k]*inv[k];
			kseedsb[k] = sigmab[k]*inv[k];
			kseedsx[k] = sigmax[k]*inv[k];
			kseedsy[k] = sigmay[k]*inv[k];
			//------------------------------------
			//edgesum[k] *= inv[k];
			//------------------------------------
		}}
	}
}

//===========================================================================
///	FindNext
///
///	Helper function for EnforceLabelConnectivity. Called recursively.
//===========================================================================
void CSuperpixelSeg::FindNext(
	const int*					labels,
	int*						nlabels,
	const int&					height,
	const int&					width,
	const int&					h,
	const int&					w,
	const int&					lab,
	int*						xvec,
	int*						yvec,
	int&						count)
{
	int oldlab = labels[h*width+w];
	for( int i = 0; i < 4; i++ )
	{
		int y = h+dy4[i];int x = w+dx4[i];
		if((y < height && y >= 0) && (x < width && x >= 0) )
		{
			int ind = y*width+x;
			if(nlabels[ind] < 0 && labels[ind] == oldlab )
			{
				xvec[count] = x;
				yvec[count] = y;
				count++;
				nlabels[ind] = lab;
				FindNext(labels, nlabels, height, width, y, x, lab, xvec, yvec, count);
			}
		}
	}
}

//===========================================================================
///	EnforceLabelConnectivity
///
///	Some superpixels may be unconnected, Relabel them. Recursive algorithm
/// used here, can crash if stack overflows. This will only happen if the
/// superpixels are very large, otherwise safe.
///		STEPS:
///		1. finding an adjacent label for each new component at the start
///		2. if a certain component is too small, assigning the previously found
///		    adjacent label to this component, and not incrementing the label.
//===========================================================================
void CSuperpixelSeg::EnforceLabelConnectivity(
	const int*					labels,
	const int&					width,
	const int&					height,
	int*						nlabels,
	int&						numlabels,
	const int&					K)
{
	int sz = width*height;		
	{for( int i = 0; i < sz; i++ ) nlabels[i] = -1;}

	const int SUPSZ = sz/K;
	//------------------
	// labeling
	//------------------
	int lab = 1;//g_frames[g_frameIndex].AutoSegLabel;
	int i(0);
	int adjlabel(0);//adjacent label
	int* xvec = new int[sz];//worst case size
	int* yvec = new int[sz];//worst case size
	{for( int h = 0; h < height; h++ )
	{
		for( int w = 0; w < width; w++ )
		{
			if(nlabels[i] < 0)
			{
				nlabels[i] = lab;
				//-------------------------------------------------------
				// Quickly find an adjacent label for use later if needed
				//-------------------------------------------------------
				{for( int n = 0; n < 4; n++ )
				{
					int x = w + dx4[n];
					int y = h + dy4[n];
					if( (x >= 0 && x < width) && (y >= 0 && y < height) )
					{
						int nindex = y*width + x;
						if(nlabels[nindex] >= 0) adjlabel = nlabels[nindex];
					}
				}}
				xvec[0] = w; yvec[0] = h;
				int count(1);
				FindNext(labels, nlabels, height, width, h, w, lab, xvec, yvec, count);
				//-------------------------------------------------------
				// If segment size is less then a limit, assign an
				// adjacent label found before, and decrement label count.
				//-------------------------------------------------------
				if(count <= (SUPSZ >> 2))
				{
					for( int c = 0; c < count; c++ )
					{
						int ind = yvec[c]*width+xvec[c];
						nlabels[ind] = adjlabel;
					}
					lab--;
				}
				lab++;
			}
			i++;
		}
	}}
	//------------------
	numlabels = lab;
	//------------------
	if(xvec) delete [] xvec;
	if(yvec) delete [] yvec;
}

void CSuperpixelSeg::EnforceLabelConnectivity(
	const int*					labels,
	const int&					width,
	const int&					height,
	int*						nlabels,
	int&						numlabels,
	const int&					K,
	const int&					lb,
	int *						labelstmp)
{
	int sz = width*height;		
	int cnt = 0;
	for( int j = 0; j < sz; j++ ) 
	{
		if(labelstmp[j]==lb)
		{
			nlabels[j] = -1;
			cnt ++;
		}
		else
			nlabels[j] = labelstmp[j];
	}
	//printf("cnt: %d\n", cnt);

	const int SUPSZ = sz/K;
	//------------------
	// labeling
	//------------------
	int lab = 1;//g_frames[g_frameIndex].AutoSegLabel;
	int i(0);
	int adjlabel(0);//adjacent label
	int* xvec = new int[sz];//worst case size
	int* yvec = new int[sz];//worst case size
	{for( int h = 0; h < height; h++ )
	{
		for( int w = 0; w < width; w++ )
		{
			if(nlabels[i] < 0)
			{
				nlabels[i] = lab;
				//-------------------------------------------------------
				// Quickly find an adjacent label for use later if needed
				//-------------------------------------------------------
				{for( int n = 0; n < 4; n++ )
				{
					int x = w + dx4[n];
					int y = h + dy4[n];
					if( (x >= 0 && x < width) && (y >= 0 && y < height) )
					{
						int nindex = y*width + x;
						if(nlabels[nindex] >= 0) adjlabel = nlabels[nindex];
					}
				}}
				xvec[0] = w; yvec[0] = h;
				int count(1);
				FindNext(labels, nlabels, height, width, h, w, lab, xvec, yvec, count);
				//-------------------------------------------------------
				// If segment size is less then a limit, assign an
				// adjacent label found before, and decrement label count.
				//-------------------------------------------------------
				if(count <= (SUPSZ >> 2))
				{
					for( int c = 0; c < count; c++ )
					{
						int ind = yvec[c]*width+xvec[c];
						nlabels[ind] = adjlabel;
					}
					lab--;
				}
				lab++;
			}
			i++;
		}
	}}
	//------------------
	numlabels = lab - 1;//g_frames[g_frameIndex].AutoSegLabel;
	//printf(" numblaels: %d\n", numlabels);
	//------------------
	if(xvec) delete [] xvec;
	if(yvec) delete [] yvec;
}


//===========================================================================
///	DoSuperpixelSegmentation_ForGivenStepSize
///
/// Originally called DoSegmentation_LABXY
/// There is option to save the labels if needed. However the filename and
/// path need to be provided.
//===========================================================================
void CSuperpixelSeg::DoSuperpixelSegmentation_ForGivenK(
	const unsigned char*		ubuff,
	const int					width,
	const int					height,
	int*						klabels,
	int&						numlabels,
	const int&					K,//required number of superpixels
	const double&				m)//weight given to spatial distance
{
	vector<double> kseedsl(0);
	vector<double> kseedsa(0);
	vector<double> kseedsb(0);
	vector<double> kseedsx(0);
	vector<double> kseedsy(0);

	//--------------------------------------------------
	m_width  = width;
	m_height = height;
	int sz = m_width*m_height;
	//--------------------------------------------------
	//if(0 == klabels) klabels = new int[sz];
	for( int s = 0; s < sz; s++ ) klabels[s] = -1;
	//--------------------------------------------------

	DoRGBtoLABConversion(ubuff, m_lvec, m_avec, m_bvec);

	//--------------------------------------------------

	bool perturbseeds(true);
	vector<double> edgemag(0);
	if(perturbseeds) DetectLabEdges(m_lvec, m_avec, m_bvec, m_width, m_height, edgemag);
	GetLABXYSeeds_ForGivenK(kseedsl, kseedsa, kseedsb, kseedsx, kseedsy, K, perturbseeds, edgemag);

	int STEP = (int)(sqrt(double(sz)/double(K)) + 2.0);//adding a small value in the even the STEP size is too small.
	PerformSuperpixelSLIC(kseedsl, kseedsa, kseedsb, kseedsx, kseedsy, klabels, STEP, edgemag, m);
	numlabels = kseedsl.size();

	int* nlabels = new int[sz];
	EnforceLabelConnectivity(klabels, m_width, m_height, nlabels, numlabels, K);
	{for(int i = 0; i < sz; i++ ) klabels[i] = nlabels[i];}
	if(nlabels) delete [] nlabels;
}

void CSuperpixelSeg::DoSuperpixelSegmentation_ForGivenK(
	const unsigned char*		ubuff,
	const int					width,
	const int					height,
	int*						klabels,
	int&						numlabels,
	const int&					K,//required number of superpixels
	const double&				m,//weight given to spatial distance
	const int&					lb,
	int*						labelstmp)
{
	vector<double> kseedsl(0);
	vector<double> kseedsa(0);
	vector<double> kseedsb(0);
	vector<double> kseedsx(0);
	vector<double> kseedsy(0);

	//--------------------------------------------------
	m_width  = width;
	m_height = height;
	int sz = m_width*m_height;
	//--------------------------------------------------
	//if(0 == klabels) klabels = new int[sz];
	for( int s = 0; s < sz; s++ ) labelstmp[s] = klabels[s];
	//--------------------------------------------------

	DoRGBtoLABConversion(ubuff, m_lvec, m_avec, m_bvec);

	//--------------------------------------------------

	bool perturbseeds(true);
	vector<double> edgemag(0);
	if(perturbseeds) DetectLabEdges(m_lvec, m_avec, m_bvec, m_width, m_height, edgemag);
	GetLABXYSeeds_ForGivenK(kseedsl, kseedsa, kseedsb, kseedsx, kseedsy, K, perturbseeds, edgemag);

	int STEP = (int)(sqrt(double(sz)/double(K)) + 2.0);//adding a small value in the even the STEP size is too small.
	/*仅对标号为lb的区域进行分割*/
	PerformSuperpixelSLIC(kseedsl, kseedsa, kseedsb, kseedsx, kseedsy, klabels, STEP, edgemag, m, lb, labelstmp);
	numlabels = kseedsl.size();

	int* nlabels = new int[sz];
	// 此时numlabels代表某个区域内的分割个数，nlabels也只是对该区域内的标号进行重排
	EnforceLabelConnectivity(klabels, m_width, m_height, nlabels, numlabels, K, lb, labelstmp); 
	{for(int i = 0; i < sz; i++ ) klabels[i] = nlabels[i];}
	if(nlabels) delete [] nlabels;
}



