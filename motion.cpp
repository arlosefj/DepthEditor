#include "common.h"

#define FASTSEARCH // 快速运动估计算法
#define MVMEDIANFILTER // 对得到的运动矢量进行中值滤波
#define SELMEDIAN   // 计算运动矢量水平和垂直方向的均值

#define USEIMGWIDTH // 采用图像宽度作为判断运动矢量大小的参数

#define MAXCOST 0x7fff
#define SEARCHRENGE 24

#ifdef FASTSEARCH
#define PMVSEARCHTH 2
#define ZMVSEARCHTH 1
#else
#define LAMDACOEFF 8  // 可再参考编码器设定
#endif

#ifdef USEIMGWIDTH
#define PERCENTWIDTH 0.03
#endif

#define MOTIONTOTAL 80

/*
H3D_RESULT WriteMotionDepth(FILE * outfile, short * depthfrommotion, int width, int height)
{
	int i;
	unsigned char * tmp;
	unsigned char * tmpd;

	tmp = (unsigned char*)malloc(width*height); 
	tmpd = (unsigned char*)malloc(width*height>>1); 
	
	if(tmp==NULL||tmpd==NULL)
	{
		printf("MALLOC  ERROR --- tmp tmpd malloc failed.\n");
		return H3D_RESULT_FAIL;
	}

	for(i=0; i<(width*height); i++)
	{
		tmp[i] = (unsigned char)(depthfrommotion[i]);
	}
	for(i=0; i<(width*height>>1); i++)
	{
		tmpd[i] = 128;
	}
	
	if (fwrite(tmp, 1, (width*height), outfile) != (unsigned int)(width*height)) 
	{
		printf("FILE ERROR --- write motiondepthmap 0 Y error!");
		return H3D_RESULT_FAIL;
	}

	if (fwrite(tmpd, 1, (width*height>>1), outfile) != (unsigned int)(width*height>>1)) 
	{
		printf("write motiondepthmap 0 UV error!");
		return H3D_RESULT_FAIL;
	}

	free(tmp);
	free(tmpd);

	return H3D_RESULT_OK;
}
*/

int SadCalc(unsigned char * cur_block, unsigned char * prev_block, int stride, int block_size)
{
	int i, j;
	int cur, prev;
	int cost = 0;
	
	for(j=0;j<block_size;j++)
		for(i=0;i<block_size;i++)
		{
			cur = *(cur_block+i+j*stride);
			prev = *(prev_block+i+j*stride);
			cost = cost+abs(cur-prev);
		}
	return cost;
}

#ifdef FASTSEARCH
#define COPY2_IF_LT(x,y,a,b)\
if((y)<(x))\
{\
    (x)=(y);\
    (a)=(b);\
}

#define COPY3_IF_LT(x,y,a,b,c,d)\
if((y)<(x))\
{\
    (x)=(y);\
    (a)=(b);\
    (c)=(d);\
}

#define CHECK_MVRANGE(mx,my) ( mx >= mvxmin && mx <= mvxmax && my >= mvymin && my <= mvymax )

#define COST_MV( mx, my )\
{\
    int cost = SadCalc( cur_block, &prev_block[(my)*width+(mx)], width, block_size );\
    COPY3_IF_LT( bcost, cost, bmx, mx, bmy, my );\
}

#define COST_MV_X3_DIR( m0x, m0y, m1x, m1y, m2x, m2y, costs )\
{\
    unsigned char *pix_base = prev_block + bmx + bmy*width;\
    if(CHECK_MVRANGE((bmx+m0x),(bmy+m0y)))\
    (costs)[0] = SadCalc( cur_block, &pix_base[(m0y)*width+(m0x)], width, block_size );\
    if(CHECK_MVRANGE((bmx+m1x),(bmy+m1y)))\
    (costs)[1] = SadCalc( cur_block, &pix_base[(m1y)*width+(m1x)], width, block_size );\
    if(CHECK_MVRANGE((bmx+m2x),(bmy+m2y)))\
    (costs)[2] = SadCalc( cur_block, &pix_base[(m2y)*width+(m2x)], width, block_size );\
}

#define COST_MV_X4( m0x, m0y, m1x, m1y, m2x, m2y, m3x, m3y )\
{\
    unsigned char *pix_base = prev_block + omx + omy*width;\
    if(CHECK_MVRANGE((omx+m0x),(omy+m0y)))\
    (costs)[0] = SadCalc( cur_block, &pix_base[(m0y)*width+(m0x)], width, block_size );\
    if(CHECK_MVRANGE((omx+m1x),(omy+m1y)))\
    (costs)[1] = SadCalc( cur_block, &pix_base[(m1y)*width+(m1x)], width, block_size );\
    if(CHECK_MVRANGE((omx+m2x),(omy+m2y)))\
    (costs)[2] = SadCalc( cur_block, &pix_base[(m2y)*width+(m2x)], width, block_size );\
    if(CHECK_MVRANGE((omx+m3x),(omy+m3y)))\
    (costs)[3] = SadCalc( cur_block, &pix_base[(m3y)*width+(m3x)], width, block_size );\
    COPY3_IF_LT( bcost, costs[0], bmx, omx+(m0x), bmy, omy+(m0y) );\
    COPY3_IF_LT( bcost, costs[1], bmx, omx+(m1x), bmy, omy+(m1y) );\
    COPY3_IF_LT( bcost, costs[2], bmx, omx+(m2x), bmy, omy+(m2y) );\
    COPY3_IF_LT( bcost, costs[3], bmx, omx+(m3x), bmy, omy+(m3y) );\
}

#endif

int FindMedian(int * in, int n)
{
	//冒泡排序
	int i,j;
	int * temp=NULL;
	int* ptemp;
	int ntemp = 0;
	int median;

	temp = (int *)malloc((n+1)*sizeof(int));  // 
	if(temp==NULL)
	{
		printf("MALLOC  ERROR --- temp malloc failed\n");
		return 65535;
	}

	for(i=0;i<n;i++)
		temp[i] = in[i];
	
	ptemp = temp;
	
	for (i = 0;i < n;i++)
	{
		ptemp = temp;

		for (j = 0;j < n-i;j++)
		{
			if(*ptemp > *(ptemp+1))
			{
				ntemp = *ptemp;
				*ptemp = *(ptemp+1);
				*(ptemp+1) = ntemp; 
				ptemp++;
			}else
				ptemp++;

		}
	}

	median = *(temp+(n-1)/2);

	free(temp);
	
	return median;

}

#ifdef MVMEDIANFILTER
void MvMidian(H3D_MOTIONVECTOR * fmv, H3D_MOTIONVECTOR * mv, int width, int height)
{
	int i,j;
	int tmp[9];
	//边界点
	i=0;
	for(j=0;j<width;j++)
	{
		fmv[i*width+j] = mv[i*width+j];
	}
	i=height-1;
	for(j=0;j<width;j++)
	{
		fmv[i*width+j] = mv[i*width+j];
	}
	for(i=1;i<height-1;i++)
	{
		j = 0;
		fmv[i*width+j] = mv[i*width+j];
		j = width-1;
		fmv[i*width+j] = mv[i*width+j];
	}
	//内部点
	for(i=1;i<height-1;i++)
		for(j=1;j<width-1;j++)
		{
			#if 0 // 9个点
			tmp[0] = mv[(i-1)*width+j-1].mvx;
			tmp[1] = mv[(i-1)*width+j].mvx;
			tmp[2] = mv[(i-1)*width+j+1].mvx;
			tmp[3] = mv[(i)*width+j-1].mvx;
			tmp[4] = mv[(i)*width+j].mvx;
			tmp[5] = mv[(i)*width+j+1].mvx;
			tmp[6] = mv[(i+1)*width+j-1].mvx;
			tmp[7] = mv[(i+1)*width+j].mvx;
			tmp[8] = mv[(i+1)*width+j+1].mvx;
			fmv[i*width+j].mvx = FindMedian(tmp,9);
			tmp[0] = mv[(i-1)*width+j-1].mvy;
			tmp[1] = mv[(i-1)*width+j].mvy;
			tmp[2] = mv[(i-1)*width+j+1].mvy;
			tmp[3] = mv[(i)*width+j-1].mvy;
			tmp[4] = mv[(i)*width+j].mvy;
			tmp[5] = mv[(i)*width+j+1].mvy;
			tmp[6] = mv[(i+1)*width+j-1].mvy;
			tmp[7] = mv[(i+1)*width+j].mvy;
			tmp[8] = mv[(i+1)*width+j+1].mvy;
			fmv[i*width+j].mvy = FindMedian(tmp,9);
			#else // 5个点
			tmp[0] = mv[(i-1)*width+j].mvx;
			tmp[1] = mv[(i)*width+j-1].mvx;
			tmp[2] = mv[(i)*width+j].mvx;
			tmp[3] = mv[(i)*width+j+1].mvx;
			tmp[4] = mv[(i+1)*width+j+1].mvx;
			fmv[i*width+j].mvx = FindMedian(tmp,5);
			tmp[0] = mv[(i-1)*width+j].mvy;
			tmp[1] = mv[(i)*width+j-1].mvy;
			tmp[2] = mv[(i)*width+j].mvy;
			tmp[3] = mv[(i)*width+j+1].mvy;
			tmp[4] = mv[(i+1)*width+j].mvy;
			fmv[i*width+j].mvy = FindMedian(tmp,5);
			#endif
		}
		
	return ;
}
#endif

/*目前在(0, 0)点周围全搜索，搜索范围SEARCHRENGE，不加边*/
void BlcokSearch(unsigned char * cur_block, unsigned char * prev_data, H3D_MOTIONVECTOR * mv, 
                    int idx_i, int idx_j, int width, int height, int block_size)
{
	int i;
	int zerox, zeroy; // (0,0)点的坐标

	int mincost = MAXCOST;

	unsigned char * prev_block;

#ifdef FASTSEARCH
	H3D_MOTIONVECTOR pmv, mv_left, mv_up, mv_upright, mv_zero;
	int block_width;
	int bmx, bmy, bcost, mx, my, omx, omy, mvxmin, mvxmax, mvymin, mvymax;
	int dir;
	int costs[6];

	block_width = width/block_size;
	mv_zero.mvx = mv_zero.mvy = 0;
	bcost = MAXCOST;
	for(i=0; i<6; i++)
	{
		costs[i] = MAXCOST;
	}

	/* 快速搜索算法 参考x264 X264_ME_HEX*/
	// 选择搜索中心:pmv, left, up, upright, zero
	if(idx_i-1>=0)
		mv_left = *(mv-1);
	else
		mv_left = mv_zero;
	if(idx_j-1>=0)
		mv_up = *(mv-block_width);
	else
		mv_up = mv_zero;
	if(idx_j-1>=0&&idx_i<block_width-1)
		mv_upright = *(mv+1-block_width);
	else
		mv_upright = mv_zero;
	bmx = pmv.mvx = H3D_MEDIAN(mv_left.mvx, mv_up.mvx, mv_upright.mvx);
	bmy = pmv.mvy = H3D_MEDIAN(mv_left.mvy, mv_up.mvy, mv_upright.mvy);

	zerox = idx_i*block_size;
	zeroy = idx_j*block_size;

	mvxmin = 0 - zerox;
	mvxmax = width-1-block_size - zerox;
	mvymin = 0 - zeroy;
	mvymax = height-1-block_size - zeroy;
	
	prev_block = prev_data+zerox+zeroy*width;

	mx = H3D_CLIP3(pmv.mvx,mvxmin,mvxmax);
	my = H3D_CLIP3(pmv.mvy,mvymin,mvymax);
	COST_MV( mx, my );
	// added by arlose for pmv bais
	if(bcost<PMVSEARCHTH*block_size*block_size)
	{
		mv->mvx = bmx;
		mv->mvy = bmy;
		return ;
	}
	if( 0 != bmx || 0 != bmy )
	{
		COST_MV( 0, 0 );
		// added by arlose for zmv bais
		if(bcost<ZMVSEARCHTH*block_size*block_size)
		{
			mv->mvx = 0;
			mv->mvy = 0;
			return ;
		}
	}
	mx = H3D_CLIP3(mv_left.mvx,mvxmin,mvxmax);
	my = H3D_CLIP3(mv_left.mvy,mvymin,mvymax);
	if( mx != bmx || my != bmy )
		COST_MV( mx, my );
	mx = H3D_CLIP3(mv_up.mvx,mvxmin,mvxmax);
	my = H3D_CLIP3(mv_up.mvy,mvymin,mvymax);
	if( mx != bmx || my != bmy )
		COST_MV( mx, my );
	mx = H3D_CLIP3(mv_upright.mvx,mvxmin,mvxmax);
	my = H3D_CLIP3(mv_upright.mvy,mvymin,mvymax);
	if( mx != bmx || my != bmy )
		COST_MV( mx, my );
	
	// 连续六边形搜索，直到选择中心或达到搜索范围
	/* hexagon search, radius 2 */
	#if 0
	for( i = 0; i < i_me_range/2; i++ )
	{
		omx = bmx; omy = bmy;
		COST_MV( omx-2, omy   );
		COST_MV( omx-1, omy+2 );
		COST_MV( omx+1, omy+2 );
		COST_MV( omx+2, omy   );
		COST_MV( omx+1, omy-2 );
		COST_MV( omx-1, omy-2 );
		if( bmx == omx && bmy == omy )
			break;
		if( !CHECK_MVRANGE(bmx, bmy) )
			break;
	}
	#else
	/* equivalent to the above, but eliminates duplicate candidates */
	dir = -2;

	/* hexagon */
	COST_MV_X3_DIR( -2,0, -1, 2,  1, 2, costs	);
	COST_MV_X3_DIR(  2,0,  1,-2, -1,-2, costs+3 );
	COPY2_IF_LT( bcost, costs[0], dir, 0 );
	COPY2_IF_LT( bcost, costs[1], dir, 1 );
	COPY2_IF_LT( bcost, costs[2], dir, 2 );
	COPY2_IF_LT( bcost, costs[3], dir, 3 );
	COPY2_IF_LT( bcost, costs[4], dir, 4 );
	COPY2_IF_LT( bcost, costs[5], dir, 5 );

	if( dir != -2 )
	{
		static const int hex2[8][2] = {{-1,-2}, {-2,0}, {-1,2}, {1,2}, {2,0}, {1,-2}, {-1,-2}, {-2,0}};
		bmx += hex2[dir+1][0];
		bmy += hex2[dir+1][1];
		/* half hexagon, not overlapping the previous iteration */
		for( i = 1; i < SEARCHRENGE/2 && CHECK_MVRANGE(bmx, bmy); i++ )
		{
			static const int mod6[8] = {5,0,1,2,3,4,5,0};
			const int odir = mod6[dir+1];
			COST_MV_X3_DIR( hex2[odir+0][0], hex2[odir+0][1],
							hex2[odir+1][0], hex2[odir+1][1],
							hex2[odir+2][0], hex2[odir+2][1],
							costs );
			dir = -2;
			COPY2_IF_LT( bcost, costs[0], dir, odir-1 );
			COPY2_IF_LT( bcost, costs[1], dir, odir   );
			COPY2_IF_LT( bcost, costs[2], dir, odir+1 );
			if( dir == -2 )
				break;
			bmx += hex2[dir+1][0];
			bmy += hex2[dir+1][1];
		}
	}
	#endif
	// 最终方形求精
	omx = bmx; omy = bmy;
	COST_MV_X4(  0,-1,	0,1, -1,0, 1,0 );
	COST_MV_X4( -1,-1, -1,1, 1,-1, 1,1 );

	mv->mvx = bmx;
	mv->mvy = bmy;
#else //FASTSEARCH
	int j;
	int cost;
	int xmin, xmax, ymin, ymax;
	int lamda;

	/* 根据搜索范围计算起止搜索点*/
	xmin = H3D_MAX(idx_i*block_size-SEARCHRENGE,0);
	xmax = H3D_MIN(idx_i*block_size+SEARCHRENGE,width-1-block_size);
	ymin = H3D_MAX(idx_j*block_size-SEARCHRENGE,0);
	ymax = H3D_MIN(idx_j*block_size+SEARCHRENGE,height-1-block_size);

	zerox = idx_i*block_size;
	zeroy = idx_j*block_size;
	
	/* 在(0, 0)点周围全搜索*/
	for(j=ymin;j<ymax;j++)
		for(i=xmin;i<xmax;i++)
		{
			prev_block = prev_data+i+j*width;
			cost = SadCalc(cur_block, prev_block, width, block_size);
			/* 越靠近中心越得到越小的lamda */
			lamda = (abs(i-zerox)+abs(j-zeroy))*LAMDACOEFF;
			if(mincost>cost+lamda)
			{
				mincost = cost+lamda;
				mv->mvx = i-zerox;
				mv->mvy = j-zeroy;
			}
		}
#endif
	return ;
}

H3D_RESULT MotionDepthCalc(short * depthfrommotion, H3D_MOTIONVECTOR * mv, int width, int height, int img_width)
{
	int i,j;
	int mvvalue = 0;
	int maxmvvalue = 0;
	#ifndef SELMEDIAN
	int summvx = 0;
	int summvy = 0;
	#endif
	int avgmvx = 0;
	int avgmvy = 0;
	int * mvx = NULL;
	int * mvy = NULL;
	int * ED = NULL;

	mvx = (int *)malloc(width*height*sizeof(int));
	mvy = (int *)malloc(width*height*sizeof(int));
	ED = (int *)malloc(width*height*sizeof(int));
	if(mvx==NULL||mvy==NULL||ED==NULL)
	{
		printf("MALLOC  ERROR --- malloc mvx mvy ED failed\n");
		return H3D_RESULT_FAIL;
	}
	

#ifdef MVMEDIANFILTER
	H3D_MOTIONVECTOR * fmv = NULL;
	fmv = (H3D_MOTIONVECTOR *)malloc(width*height*sizeof(H3D_MOTIONVECTOR));
	if(fmv==NULL)
	{
		printf("MALLOC  ERROR --- malloc fmv failed\n");
		return H3D_RESULT_FAIL;
	}
	
	MvMidian(fmv, mv, width, height);
	
	#ifdef SELMEDIAN // 中值
	
		for(j=0;j<height;j++)
			for(i=0;i<width;i++)
			{
				mvx[i+j*width] = fmv[i+j*width].mvx;
				mvy[i+j*width] = fmv[i+j*width].mvy;
			}
		avgmvx = FindMedian(mvx,width*height);
		avgmvy = FindMedian(mvy,width*height);
		
		#ifdef H3D_DEBUG
		printf(" median mv %d %d \n", avgmvx, avgmvy);
		#endif
	
	#else // 均值
	
		for(j=0;j<height;j++)
			for(i=0;i<width;i++)
			{
				summvx += fmv[i+j*width].mvx;
				summvy += fmv[i+j*width].mvy;
			}

		avgmvx = summvx/(height*width);
		avgmvy = summvy/(height*width);
	
		#ifdef H3D_DEBUG
		printf(" avgmv %d %d \n", avgmvx, avgmvy);
		#endif
		
	#endif

	for(j=0;j<height;j++)
		for(i=0;i<width;i++)
		{
			mvvalue = ED[i+j*width] = (int)sqrt((double)((fmv[i+j*width].mvx-avgmvx)*(fmv[i+j*width].mvx-avgmvx)+(fmv[i+j*width].mvy-avgmvy)*(fmv[i+j*width].mvy-avgmvy))/2);
			#ifndef USEIMGWIDTH
			if(mvvalue>maxmvvalue)
				maxmvvalue = mvvalue;
			#endif
		}
		
	free(fmv);

#else
	#ifdef SELMEDIAN // 中值
	
		for(j=0;j<height;j++)
			for(i=0;i<width;i++)
			{
				mvx[i+j*width] = mv[i+j*width].mvx;
				mvy[i+j*width] = mv[i+j*width].mvy;
			}
		avgmvx = FindMedian(mvx,width*height);
		avgmvy = FindMedian(mvy,width*height);
		
		#ifdef H3D_DEBUG
		printf(" median mv %d %d \n", avgmvx, avgmvy);
		#endif
	
	#else // 均值
	
		for(j=0;j<height;j++)
			for(i=0;i<width;i++)
			{
				summvx += mv[i+j*width].mvx;
				summvy += mv[i+j*width].mvy;
			}

		avgmvx = summvx/(height*width);
		avgmvy = summvy/(height*width);
		
		#ifdef H3D_DEBUG
		printf(" avgmv %d %d \n", avgmvx, avgmvy);
		#endif
		
	#endif
	
	for(j=0;j<height;j++)
		for(i=0;i<width;i++)
		{
			mvvalue = ED[i+j*width] = (int)sqrt((double)((mv[i+j*width].mvx-avgmvx)*(mv[i+j*width].mvx-avgmvx)+(mv[i+j*width].mvy-avgmvy)*(mv[i+j*width].mvy-avgmvy))/2);
			#ifndef USEIMGWIDTH
			if(mvvalue>maxmvvalue)
				maxmvvalue = mvvalue;
			#endif
		}

#endif
	#ifdef USEIMGWIDTH
	maxmvvalue = (int)(img_width*PERCENTWIDTH);
	#endif

	for(j=0;j<height;j++)
		for(i=0;i<width;i++)
		{
			depthfrommotion[i+j*width] = (short)(unsigned char)(ED[i+j*width]*255/maxmvvalue);
		}

	free(mvx);
	free(mvy);
	free(ED);

	// wet paint
	for(j=1;j<height;j++)
		for(i=0;i<width;i++)
		{
			if(depthfrommotion[i+j*width] < depthfrommotion[i+(j-1)*width])
				depthfrommotion[i+j*width] = depthfrommotion[i+(j-1)*width];
		}

	//WriteMotionDepth(out_motiondepth, depthfrommotion, width, height);
	
	return H3D_RESULT_OK;
}


H3D_RESULT MotionCompute(short * depthfrommotion, unsigned char *prev_data, unsigned char *cur_data, int block_width, int block_height, int block_size)
{
	int i,j;
	int width, height;
	H3D_MOTIONVECTOR *mv;
	unsigned char * cur_block;
	H3D_MOTIONVECTOR *cur_mv;

	width = block_width*block_size;
	height = block_height*block_size;

	mv = (H3D_MOTIONVECTOR *)malloc(block_width*block_height*sizeof(H3D_MOTIONVECTOR));
	if(mv==NULL)
	{
		printf("MALLOC  ERROR --- mv malloc failed.\n");
		return H3D_RESULT_FAIL;
	}

	for(j=0;j<block_height;j++)
		for(i=0;i<block_width;i++)
		{
			cur_block = cur_data+i*block_size+j*width*block_size;
			cur_mv = mv+i+j*block_width;
			BlcokSearch(cur_block, prev_data, cur_mv, i, j, width, height, block_size);
		}
			
	MotionDepthCalc(depthfrommotion, mv, block_width, block_height, width);
	
	if(mv!=NULL)
		free(mv);
	
	return H3D_RESULT_OK;
}

H3D_RESULT MotionDepthAcc(unsigned char *macc, short * depthfrommotion, int block_width, int block_height, int block_size)
{
	int i,j;
	int macc_total = 0; 
	int new_total = 0;
	int over_total;
	double f;

	over_total = MOTIONTOTAL*block_width*block_height;

	for(j=0;j<block_height;j++)
		for(i=0;i<block_width;i++)
		{
			macc_total += macc[i+j*block_width];
			new_total += depthfrommotion[i+j*block_width];
		}

	if(new_total+macc_total<over_total)
	{
		for(j=0;j<block_height;j++)
			for(i=0;i<block_width;i++)
			{
				macc[i+j*block_width] += depthfrommotion[i+j*block_width];
			}
	}
	else
	{
		f = ((double)(over_total-new_total))/macc_total;
		for(j=0;j<block_height;j++)
			for(i=0;i<block_width;i++)
			{
				macc[i+j*block_width] += (unsigned char)(macc[i+j*block_width]*f + depthfrommotion[i+j*block_width]);
			}
	}
	
	return H3D_RESULT_OK;
}

H3D_RESULT MotionDepthPre(short * depthfrommotion, unsigned char *macc, int block_width, int block_height, int block_size)
{
	int i, j;
	int avg, total = 0;

	for(j=0;j<block_height;j++)
		for(i=0;i<block_width;i++)
		{
			total += macc[i+j*block_width];
		}

	avg = total/(block_width*block_height);

	for(j=0;j<block_height;j++)
		for(i=0;i<block_width;i++)
		{
			depthfrommotion[i+j*block_width] = macc[i+j*block_width] - avg;
		}
	
	return H3D_RESULT_OK;
}



