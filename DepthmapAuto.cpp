#include "common.h"
#include "project.h"
/* OPENCV */
#include "cv.h"   
#include "highgui.h"   

//#define SHOWAUTODEPGEN

#define MAX16 65535

const int OFFSET = 48;
const float K = 16;

int Flag_sc = 0;

#define HISTSZ 30
#define HISTTH 100

int HistCompute(IplImage *image1,IplImage *image2)
{

	CvHistogram *hist = 0;
	CvHistogram *hist2 = 0;

	CvSize sz;

	int hdims = HISTSZ; 
	float hranges_arr[] = {0,255};
	float* hranges = hranges_arr;
	float max_val,max_val2;
	double d;

	sz.width = image1->width;
	sz.height = image1->height;

	hist = cvCreateHist( 1, &hdims, CV_HIST_ARRAY, &hranges, 1 );  
	hist2 = cvCreateHist( 1, &hdims, CV_HIST_ARRAY, &hranges, 1 ); 

	cvCalcHist( &image1, hist, 0, 0 ); 
	cvCalcHist( &image2, hist2, 0, 0 );
	cvGetMinMaxHistValue( hist, 0, &max_val, 0, 0 ); 
	cvGetMinMaxHistValue( hist2, 0, &max_val2, 0, 0 );
	cvConvertScale( hist->bins, hist->bins, max_val ? 255. / max_val : 0., 0 ); 
	cvConvertScale( hist2->bins, hist2->bins, max_val2 ? 255. / max_val2 : 0., 0 ); 

	d = cvCompareHist(hist,hist2,1);
	
#if 0
	printf("Hist diff: %f\n",d);
#endif

	 if(d > HISTTH ){
	      return 1;
	 }
	 else{
	      return 0;
	 }

	 cvReleaseHist (&hist);
	 cvReleaseHist (&hist2);

}


/* 每层Depth Flaw 计算*/
void DepthflawCalc(IplImage * pImg, float * up, float * left, int width, int height)
{
	int i, j;
	int up_diff, left_diff;
	
	for(j=0; j<height; j++)
		for(i=0; i<width; i++)
		{
			up[j*width+i] = 0;
			left[j*width+i] = 0;
		}

	for(j=1; j<height-1; j++)
		for(i=1; i<width-1; i++)
		{
			up_diff = abs(pImg->imageData[j*pImg->widthStep+i*pImg->nChannels+0] - pImg->imageData[(j-1)*pImg->widthStep+i*pImg->nChannels+0]) +
				      abs(pImg->imageData[j*pImg->widthStep+i*pImg->nChannels+1] - pImg->imageData[(j-1)*pImg->widthStep+i*pImg->nChannels+1]) +
					  abs(pImg->imageData[j*pImg->widthStep+i*pImg->nChannels+2] - pImg->imageData[(j-1)*pImg->widthStep+i*pImg->nChannels+2]);
			if(up_diff<=OFFSET)
				up[j*width+i] = 1;
			else if(up_diff>(OFFSET+(K)))
				up[j*width+i] = 0;
			else
				up[j*width+i] = 1 - (up_diff-OFFSET)/K;

			left_diff = abs(pImg->imageData[j*pImg->widthStep+i*pImg->nChannels+0] - pImg->imageData[j*pImg->widthStep+(i-1)*pImg->nChannels+0]) +
				        abs(pImg->imageData[j*pImg->widthStep+i*pImg->nChannels+1] - pImg->imageData[j*pImg->widthStep+(i-1)*pImg->nChannels+1]) +
					    abs(pImg->imageData[j*pImg->widthStep+i*pImg->nChannels+2] - pImg->imageData[j*pImg->widthStep+(i-1)*pImg->nChannels+2]);
			if(left_diff<=OFFSET)
				left[j*width+i] = 1;
			else if(left_diff>(OFFSET+(K)))
				left[j*width+i] = 0;
			else
				left[j*width+i] = 1 - (left_diff-OFFSET)/K;
		}

	return ;
}


/* 最底层Depth Map 初始化 */
void DepthMapInit0(unsigned short * z, int width, int height)
{
	int i, j;
	//printf("DepthMapInit0: %dx%d", width, height);
	if(g_frames[g_frameIndex].DepBaseFlag==1)
	{
		for(j=0; j<height; j++)
			for(i=0; i<width; i++)
			{
				z[j*width+i] = ((unsigned short)g_BaseDepth[j*width+i])<<8;
			}
	}
	else
	{
		for(j=0; j<height; j++)
			for(i=0; i<width; i++)
			{
				z[j*width+i] = j*MAX16/height;
			}
	}

	return ;
}

/* 迭代计算每层Depth Map 值*/
void DepthMapRefine(unsigned short * z, float * up, float * left, int width, int height, int count)
{
#if 1
	int i,j,k;
	unsigned short* tmp;
	float t;
	float ztmp;

	tmp = (unsigned short*)malloc(width*height*sizeof(unsigned short)); 
	if(tmp==NULL)
	{
		printf("MALLOC  ERROR --- tmp malloc failed.\n");
		return ;
	}
	for(j=0; j<height; j++)
		for(i=0; i<width; i++)
		{
			tmp[j*width+i] = z[j*width+i];
		}

	for(k=0; k<count; k++)
	{
		for(j=1; j<height-1; j++)
			for(i=1; i<width-1; i++)
			{
				t = (tmp[(j-1)*width+i]-tmp[j*width+i])*up[j*width+i] + 
					(tmp[j*width+i-1]-tmp[j*width+i])*left[j*width+i] + 
					(tmp[j*width+i+1]-tmp[j*width+i])*left[j*width+i+1] + 
					(tmp[(j+1)*width+i]-tmp[j*width+i])*up[(j+1)*width+i];
				ztmp = (float)((0.25*t) + tmp[j*width+i]);
				if(ztmp>MAX16)
					z[j*width+i] = MAX16;
				else if(ztmp<0)
					z[j*width+i] = 0;
				else
					z[j*width+i] = (unsigned short)ztmp;
			}
			
		for(j=0; j<height; j++)
			for(i=0; i<width; i++)
			{
				tmp[j*width+i] = z[j*width+i];
			}
	}

	free(tmp);
#endif
	return ;
}

void InterpBilinear_RowFilter(unsigned short* src, long* dst, int len, int* leftIdx, int* rightIdx, long* weight, int shift)
{
    int i;
    for(i = 0; i < len - 4; i+=4){
        *dst++ = ((1<<shift) - weight[i])*src[leftIdx[i]] + weight[i]*src[rightIdx[i]];
        *dst++ = ((1<<shift) - weight[i+1])*src[leftIdx[i+1]] + weight[i+1]*src[rightIdx[i+1]];
        *dst++ = ((1<<shift) - weight[i+2])*src[leftIdx[i+2]] + weight[i+2]*src[rightIdx[i+2]];
        *dst++ = ((1<<shift) - weight[i+3])*src[leftIdx[i+3]] + weight[i+3]*src[rightIdx[i+3]];
 
    }
    for( ; i < len; ++i){
        *dst++ = ((1<<shift) - weight[i])*src[leftIdx[i]] + weight[i]*src[rightIdx[i]];
    }
}

void InterpBilinear(unsigned short* src, unsigned short* dst, int src_width, int src_height, int dst_width, int dst_height)
{
    int i, j;
    int sw, sh, sstep;
    int dw, dh, dstep;
    unsigned short *sdata, *ddata;
    float horScaleRatio, verScaleRatio;
    long *rowBuf1, *rowBuf2;
    long *upLinePtr, *downLinePtr, *tempPtr;
    long *horWeight;
    int *horLeftIdx, *horRightIdx;
    int preVerUpIdx, preVerDownIdx;
    int shift = 8;
 
    sw = src_width; sh = src_height; sstep = src_width; 
    dw = dst_width; dh = dst_height; dstep = dst_width;
 
    horScaleRatio = sw / (float)(dw);
    verScaleRatio = sh / (float)(dh);
 
    rowBuf1 = new long[dw];
    rowBuf2 = new long[dw];
    horWeight = new long[dw];
    horLeftIdx = new int[dw];
    horRightIdx = new int[dw];
	sdata = new unsigned short [sw*sh];
	ddata = new unsigned short [dw*dh];

	for(j=0;j<sh;j++)
	{
		for(i=0;i<sw;i++)
		{
			sdata[i+j*sw] = src[i+j*sw];
		}
	}

//col interpolation
    for(i = 0; i < dw; i++){
        float pos = (i + 0.5f) * horScaleRatio;
        horLeftIdx[i] = (int)(H3D_MAX(pos - 0.5f, 0));
        horRightIdx[i] = (int)(H3D_MIN(pos + 0.5f, sw-1));
        horWeight[i] = (long) (fabs(pos - 0.5f - horLeftIdx[i]) * (1<<shift));
    }
 
    preVerUpIdx = -1;
    preVerDownIdx = -1;
    upLinePtr = rowBuf1;
    downLinePtr = rowBuf2;
    for(j = 0; j < dh; j++){
        float pos = (j + 0.5f) * verScaleRatio;
        int verUpIdx = (int)(H3D_MAX(pos - 0.5f, 0));
        int verDownIdx = (int)(H3D_MIN(pos + 0.5f, sh-1));
        long verWeight = (long) (fabs(pos - 0.5f - verUpIdx) * (1<<shift));
 
        if(verUpIdx == preVerUpIdx && verDownIdx == preVerDownIdx){
            ;
//do nothing
        }
        else if(verUpIdx == preVerDownIdx){
            H3D_SWAP(upLinePtr, downLinePtr, tempPtr);
            InterpBilinear_RowFilter(sdata + sstep*verDownIdx, downLinePtr, dw, horLeftIdx, horRightIdx, horWeight, shift);
        }else{
            InterpBilinear_RowFilter(sdata + sstep*verUpIdx,   upLinePtr, dw, horLeftIdx, horRightIdx, horWeight, shift);
            InterpBilinear_RowFilter(sdata + sstep*verDownIdx, downLinePtr, dw, horLeftIdx, horRightIdx, horWeight, shift);
        }        
 
        unsigned short* _ptr = ddata + dstep*j;
        for(i = 0; i < dw-4; i+=4){
            *_ptr++ = (unsigned short) ( (((1<<shift) - verWeight)*upLinePtr[i] + verWeight*downLinePtr[i]) >> (2*shift) );
            *_ptr++ = (unsigned short) ( (((1<<shift) - verWeight)*upLinePtr[i+1] + verWeight*downLinePtr[i+1]) >> (2*shift) );
            *_ptr++ = (unsigned short) ( (((1<<shift) - verWeight)*upLinePtr[i+2] + verWeight*downLinePtr[i+2]) >> (2*shift) );
            *_ptr++ = (unsigned short) ( (((1<<shift) - verWeight)*upLinePtr[i+3] + verWeight*downLinePtr[i+3]) >> (2*shift) );
        }
        for(; i < dw; i++){
            *_ptr++ = (unsigned short) ( (((1<<shift) - verWeight)*upLinePtr[i] + verWeight*downLinePtr[i]) >> (2*shift) );
        }
        preVerUpIdx = verUpIdx;
        preVerDownIdx = verDownIdx;
    }

	for(j=0;j<dh;j++)
	{
		for(i=0;i<dw;i++)
		{
			dst[i+j*dw] = ddata[i+j*dw];
		}
	}
	
    delete []rowBuf1;
    delete []rowBuf2;
    delete []horWeight;
    delete []horLeftIdx;
    delete []horRightIdx;
	delete []sdata;
	delete []ddata;

	return ;
}


/* Upscale 作为上一层的输入*/
void DepthMapUpscaling(unsigned short * src, unsigned short * dst, int width, int height)
{
//	int i, j;
	int width2 = width>>1;
	int height2 = height>>1;
	//int tmp;
#if 1
	InterpBilinear(src, dst, width2, height2, width, height);
#else
	for(j=0; j<height2-1; j++)
		for(i=0; i<width2; i++)
		{
			dst[(j*2)*width+(i*2)] = src[j*width2+i];
			if(i!=width2-1)
				dst[(j*2)*width+(i*2+1)] = ((int)src[j*width2+i]+src[j*width2+i+1])>>1;
			else
				dst[(j*2)*width+(i*2+1)] = src[j*width2+i];
			tmp = MAX16/height*(!g_frames[g_frameIndex].DepBaseFlag) + src[j*width2+i];
			if(tmp>MAX16)
				dst[(j*2+1)*width+(i*2)] = MAX16;
			else
				dst[(j*2+1)*width+(i*2)] = (unsigned short)tmp;
			if(i!=width2-1)
				tmp = MAX16/height*(!g_frames[g_frameIndex].DepBaseFlag) + (((int)src[j*width2+i]+src[j*width2+i+1])>>1);
			else
				tmp = MAX16/height*(!g_frames[g_frameIndex].DepBaseFlag) + src[j*width2+i];
			if(tmp>MAX16)
				dst[(j*2+1)*width+(i*2+1)] = MAX16;
			else
				dst[(j*2+1)*width+(i*2+1)] = (unsigned short)tmp;
			
		}

	if(height%2==0)
	{
		j=height2-1;
		for(i=0; i<width2; i++)
		{
			dst[(j*2)*width+(i*2)] = src[j*width2+i];
			if(i!=width2-1)
				dst[(j*2)*width+(i*2+1)] = ((int)src[j*width2+i]+src[j*width2+i+1])>>1;
			else
				dst[(j*2)*width+(i*2+1)] = src[j*width2+i];
			tmp = MAX16/height*(!g_frames[g_frameIndex].DepBaseFlag) + src[j*width2+i];
			if(tmp>MAX16)
				dst[(j*2+1)*width+(i*2)] = MAX16;
			else
				dst[(j*2+1)*width+(i*2)] = (unsigned short)tmp;
			if(i!=width2-1)
				tmp = MAX16/height*(!g_frames[g_frameIndex].DepBaseFlag) + (((int)src[j*width2+i]+src[j*width2+i+1])>>1);
			else
				tmp = MAX16/height*(!g_frames[g_frameIndex].DepBaseFlag) + src[j*width2+i];
			if(tmp>MAX16)
				dst[(j*2+1)*width+(i*2+1)] = MAX16;
			else
				dst[(j*2+1)*width+(i*2+1)] = (unsigned short)tmp;
			
		}
	}
	else
	{
		j=height2-1;
		for(i=0; i<width2; i++)
		{
			dst[(j*2)*width+(i*2)] = src[j*width2+i];
			if(i!=width2-1)
				dst[(j*2)*width+(i*2+1)] = ((int)src[j*width2+i]+src[j*width2+i+1])>>1;
			else
				dst[(j*2)*width+(i*2+1)] = src[j*width2+i];
		}
	}
#endif
	return ;
}

/*加运动信息的深度图*/
/* 修改!原始深度图是16位无符号类型，而运动深度差为8位有符号类型*/
void DepthMapAddMotion(unsigned short * depth, short *depthfrommotion, int width, int height)
{
	int i,j;
	int odl, mdl, sdl;

	for(j=0;j<height;j++)
		for(i=0;i<width;i++)
		{
			odl = (int)depth[i+j*width];
			mdl = (int)(depthfrommotion[i+j*width]*256);
			sdl = odl + mdl;

			if(sdl<0)
				depth[i+j*width] = 0;
			else if(sdl>=MAX16)
				depth[i+j*width] = MAX16;
			else
				depth[i+j*width] = (unsigned short)(sdl);
		}
		
	return ;
}

void DepthMapFilter(unsigned short * depth, unsigned short * prev, int width, int height)
{
	int i,j;

	for(j=0;j<height;j++)
		for(i=0;i<width;i++)
		{
			depth[i+j*width] = (prev[i+j*width]>>3)*7+(depth[i+j*width]>>3);
		}
	return ;
}

H3D_RESULT CreateDepthMap(int frameNumber, const H3D_YUV input, H3D_DEPTH output, int delayed, const H3D_PARAM *pParameter)
{
	
	IplImage* pImg[STAGEMAX]; // 读入图像
	IplImage* pImg_cur; // 当前帧图像
	IplImage* pImg_prev; // 前一帧图像
	unsigned char *yuv_input = NULL;
	unsigned char *rgb_input = NULL;
	unsigned char *y_input = NULL;
	unsigned char *u_input = NULL;
	unsigned char *v_input = NULL;
	int w,h;
	int block_size, block_width, block_height;

	int i;
	int stage; // 金字塔层数
	int maxhw; // 图像宽高的最大值
	int hw;
	int count;
	CvSize sz;
	int scale = 1;
	//int m, n, minw, minh;

	float * depthflaw_up[STAGEMAX]; // depthflaw up
	float * depthflaw_left[STAGEMAX]; // depthflaw left

	short * depthfrommotion = NULL;

	w = input.width;
	h = input.height;
	sz.width = w;
	sz.height = h;

	if(frameNumber==0)
	{
		Flag_sc = 1;
		for(i=0;i<(w*h>>2);i++)
		{
			output.motionacc_data[i] = 0;
		}
	}
	else
	{
		pImg_cur = cvCreateImage(sz, IPL_DEPTH_8U, 1);
		pImg_cur->imageData = (char *)input.cur_data;
		
		pImg_prev = cvCreateImage(sz, IPL_DEPTH_8U, 1);
		pImg_prev->imageData = (char *)input.prev_data;

		//计算直方图差
		Flag_sc = HistCompute(pImg_prev, pImg_cur);

		// 运动矢量深度图空间分配
		if(pParameter->Method_MotionBlockSize==0) // 8x8
			{
				block_size = 8;
				block_width = w>>3;
				block_height = h>>3;
			}
		else if(pParameter->Method_MotionBlockSize==1) // 4x4
			{
				block_size = 4;
				block_width = w>>2;
				block_height = h>>2;
			}
		else //(pParameter->Method_MotionBlockSize==2) // 2x2
			{
				block_size = 2;
				block_width = w>>1;
				block_height = h>>1;
			}

		depthfrommotion = (short *)malloc(block_width*block_height*2);
		if(depthfrommotion==NULL)
		{
			printf("MALLOC  ERROR --- depthfrommotion malloc failed.\n");
			return H3D_RESULT_FAIL;
		}

		if(Flag_sc==0)
		{
			// 计算运动矢量
			MotionCompute(depthfrommotion, input.prev_data, input.cur_data, block_width, block_height, block_size);

			// 运动矢量累积
			MotionDepthAcc(output.motionacc_data, depthfrommotion, block_width, block_height, block_size);

			// 运动矢量相加前处理，此时depthfrommotion保存相加到原始深度图的值
			MotionDepthPre(depthfrommotion, output.motionacc_data, block_width, block_height, block_size);
		}
		else
		{
			for(i=0;i<block_width*block_height;i++)
				depthfrommotion[i] = 0;
		}
		
		cvReleaseImage( &pImg_cur );
		cvReleaseImage( &pImg_prev );
	}

#if 0
	if(Flag_sc==1)
		printf("\rDEPTH   INFO  --- Depthmap Auto Generation scene Cut Detect at frame %d", g_frameIndex+1);
#endif

	yuv_input = input.cur_data;

	rgb_input = (unsigned char *) malloc(IMAGERGB_SIZE(w,h));
	if(rgb_input==NULL)
	{
		printf("MALLOC  ERROR --- rgb_input malloc failed.\n");
		return H3D_RESULT_FAIL;
	}

	y_input = yuv_input;
	u_input = yuv_input+w*h;
	v_input = yuv_input+w*h*5/4;

	yuv2rgb(y_input,u_input,v_input,rgb_input,w,h);


	pImg[0] = cvCreateImage(sz, IPL_DEPTH_8U, 3);
	pImg[0]->imageData = (char *)rgb_input;

	/* 根据原始图像大小确定stage值*/
	maxhw = H3D_MAX(pImg[0]->width, pImg[0]->height);
	if(maxhw>MAXWIDTH)
	{
		printf("SIZE    ERROR --- Image size should be less than 2048.\n");
		return H3D_RESULT_FAIL;
	}
	if(maxhw<MINWIDTH)
	{
		printf("SIZE    ERROR --- Image size should be more than 192.\n");
		return H3D_RESULT_FAIL;
	}
	
	hw = maxhw;
	for(stage=2; ;stage++) // 最少有两层
	{
		hw = hw>>1;
		if(hw<=40)
			break;
	}

	/* 下采样并计算depthflaw */
	depthflaw_up[0] = (float*)malloc(pImg[0]->width*pImg[0]->height*sizeof(float)); 
	if(depthflaw_up[0]==NULL)
	{
		printf("MALLOC  ERROR --- depthflaw_up[0] malloc failed.\n");
		return H3D_RESULT_FAIL;
	}
	depthflaw_left[0] = (float*)malloc(pImg[0]->width*pImg[0]->height*sizeof(float)); 
	if(depthflaw_left[0]==NULL)
	{
		printf("MALLOC  ERROR --- depthflaw_left[0] malloc failed.\n");
		return H3D_RESULT_FAIL;
	}
	
	DepthflawCalc(pImg[0], depthflaw_up[0], depthflaw_left[0], pImg[0]->width, pImg[0]->height);
	for(i=1;i<stage;i++)
	{
		sz.width = (int)(pImg[i-1]->width>>scale);
		sz.height = (int)(pImg[i-1]->height>>scale);
		pImg[i] = cvCreateImage( sz, IPL_DEPTH_8U, 3);
		cvResize(pImg[i-1],pImg[i],CV_INTER_CUBIC);  // 目前最上层没有根据分辨率来设置下采样倍数(1920 -> 4)
		depthflaw_up[i] = (float*)malloc(pImg[i]->width*pImg[i]->height*sizeof(float)); 
		if(depthflaw_up[i]==NULL)
		{
			printf("MALLOC  ERROR --- depthflaw_up[%d] malloc failed.\n", i);
			return H3D_RESULT_FAIL;
		}
		depthflaw_left[i] = (float*)malloc(pImg[i]->width*pImg[i]->height*sizeof(float)); 
		if(depthflaw_left[i]==NULL)
		{
			printf("MALLOC  ERROR --- depthflaw_left[%d] malloc failed.\n", i);
			return H3D_RESULT_FAIL;
		}
		
		DepthflawCalc(pImg[i], depthflaw_up[i], depthflaw_left[i], pImg[i]->width, pImg[i]->height);
	}

	/* 最底层depthmap初始化和迭代计算*/
	DepthMapInit0(output.cur_data[stage-1], pImg[stage-1]->width, pImg[stage-1]->height);

#ifdef SHOWAUTODEPGEN
	if(g_enlargeflag==0)
	{
		for(m=0;m<pImg[stage-1]->height;m++)
			for(n=0;n<pImg[stage-1]->width;n++)
			{
				g_depthdisplay[n+m*g_frameWidth] = output.cur_data[stage-1][n+m*pImg[stage-1]->width];
			}
		gdk_draw_gray_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
		  IMGWIDTHMAX, IMGHEIGHTMAX, g_frameWidth, g_frameHeight,
		  GDK_RGB_DITHER_MAX, g_depthdisplay, g_frameWidth);
	}
#endif
	
	DepthMapRefine(output.cur_data[stage-1], depthflaw_up[stage-1], depthflaw_left[stage-1], pImg[stage-1]->width, pImg[stage-1]->height, g_depautorefineiter);
#ifdef SHOWAUTODEPGEN
	if(g_enlargeflag==0)
	{
		for(m=0;m<pImg[stage-1]->height;m++)
			for(n=0;n<pImg[stage-1]->width;n++)
			{
				g_depthdisplay[n+m*g_frameWidth] = output.cur_data[stage-1][n+m*pImg[stage-1]->width];
			}
		gdk_draw_gray_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
		  IMGWIDTHMAX, IMGHEIGHTMAX, g_frameWidth, g_frameHeight,
		  GDK_RGB_DITHER_MAX, g_depthdisplay, g_frameWidth);
	}
#endif

	/* 中间层depthmap 生成和迭代计算*/
	for(i=stage-2;i>0;i--)
	{
		if(i==stage-2)
			count = g_depautorefineiter;
		else
			count = g_depautorefineiter>>1;
		DepthMapUpscaling(output.cur_data[i+1], output.cur_data[i], pImg[i]->width, pImg[i]->height);
		
		#ifdef SHOWAUTODEPGEN
		minh = H3D_MIN(pImg[i]->height, g_frameHeight);
		minw = H3D_MIN(pImg[i]->width, g_frameWidth);
		if(g_enlargeflag==0)
		{
			for(m=0;m<minh;m++)
				for(n=0;n<minw;n++)
				{
					g_depthdisplay[n+m*g_frameWidth] = output.cur_data[i][n+m*pImg[i]->width];
				}
			gdk_draw_gray_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
			  IMGWIDTHMAX, IMGHEIGHTMAX, g_frameWidth, g_frameHeight,
			  GDK_RGB_DITHER_MAX, g_depthdisplay, g_frameWidth);
		}
		#endif
		/* 加上运动深度图信息*/
		if(i+pParameter->Method_MotionBlockSize==3)
			DepthMapAddMotion(output.cur_data[i], depthfrommotion, block_width, block_height);
		
		DepthMapRefine(output.cur_data[i], depthflaw_up[i], depthflaw_left[i], pImg[i]->width, pImg[i]->height, count);
		#ifdef SHOWAUTODEPGEN
		if(g_enlargeflag==0)
		{
			for(m=0;m<minh;m++)
				for(n=0;n<minw;n++)
				{
					g_depthdisplay[n+m*g_frameWidth] = output.cur_data[i][n+m*pImg[i]->width];
				}
			gdk_draw_gray_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
			  IMGWIDTHMAX, IMGHEIGHTMAX, g_frameWidth, g_frameHeight,
			  GDK_RGB_DITHER_MAX, g_depthdisplay, g_frameWidth);
		}
		#endif
	}
	/* 最上层depthmap 生成*/
	DepthMapUpscaling(output.cur_data[1], output.cur_data[0], pImg[0]->width, pImg[0]->height);

	if(Flag_sc==0)
	{
		DepthMapFilter(output.cur_data[0], output.prev_data, pImg[0]->width, pImg[0]->height);
	}

	if(rgb_input!=NULL)
		free(rgb_input);
	if(depthfrommotion!=NULL)
		free(depthfrommotion);
	for(i=0; i<stage; i++)
	{
		cvReleaseImage( &pImg[i] );
		free(depthflaw_up[i]);
		free(depthflaw_left[i]);
	}

	return H3D_RESULT_OK;
}

H3D_YUV yuv_in;
H3D_DEPTH depth_out;
H3D_PARAM Parameter;

static int XDIM = 0;
static int YDIM = 0;
static int   ARG_MEBLOCKSIZE = 0;
static int   ARG_OUTPUTFORMAT = 0;

static int filenr = 0;
int g_depoverwritten = 1;
int g_depautoaddmotion = 1;
int g_depautorefineiter = DEPAUTOPARAM_RIDEF;
int dddinitflag = 0;

int depthautogen(int first, int last)
{
	int i,j,k;

	char filename[256];
	
	int delayed = 0;
	unsigned char *in_buffer = NULL;
	unsigned char *in_prev_buffer = NULL;
	unsigned char *macc_buffer = NULL;
	int curidx = g_frameIndex;

	H3D_PARAM * pParameter = &Parameter;
	
	XDIM = g_frameInWidth;
	YDIM = g_frameInHeight;

	

	if(dddinitflag==0)
	{
		// 运动估计粒度0:8x8 1:4x4 2:2x2
		pParameter->Method_MotionBlockSize = ARG_MEBLOCKSIZE;
		// 图像下采样方法0:
		pParameter->Method_ImgDownscale = 0;
		// 深度图上采样方法0:
		pParameter->Method_DepthUpscale = 0;
		// 深度图后处理方法0:
		pParameter->Method_DepthPostprocess = 0;
		// Render方法0: PULL
		pParameter->Method_Render = 0;
		// 输出图格式0:leftright  1:anaglyph
		pParameter->Format_Out = ARG_OUTPUTFORMAT;
		// 初始化模板标号0:
		pParameter->InitialDepthTemplateIndex = 0;
		// 文字区域突出开关0: OFF 1: ON
		pParameter->Flag_TextRegionHighlight = 0;
		// 运动权重0-1 default: 0.6
		pParameter->MotionWeight = 0.6;
		// 人工权重0-1 default: 0.8
		pParameter->ManualWeight = 0.8;
		
		dddinitflag = 1;
	}

	in_buffer = (unsigned char *) malloc(IMAGE_SIZE(XDIM,YDIM));
	if(in_buffer==NULL)
	{
		printf("MALLOC  ERROR --- in_buffer av_malloc failed.\n");
		return H3D_RESULT_FAIL;
	}
	in_prev_buffer = (unsigned char *) malloc(IMAGE_SIZE(XDIM,YDIM));
	if(in_prev_buffer==NULL)
	{
		printf("MALLOC  ERROR --- in_prev_buffer av_malloc failed.\n");
		return H3D_RESULT_FAIL;
	}

	macc_buffer = (unsigned char *) malloc(XDIM*YDIM>>2);
	if(macc_buffer==NULL)
	{
		
		printf("MALLOC  ERROR --- macc_buffer av_malloc failed.\n");
		return H3D_RESULT_FAIL;
	}
	
	for(i=0;i<STAGEMAX;i++)
	{
		depth_out.cur_data[i] = (unsigned short *) malloc(2*XDIM*YDIM>>(i*2));
		if(depth_out.cur_data[i]==NULL)
		{
			printf("MALLOC  ERROR --- depth_out.cur_data[%d] av_malloc failed.\n", i);
			return H3D_RESULT_FAIL;
		}
	}
	depth_out.prev_data = (unsigned short *) malloc(2*XDIM*YDIM);
	if(depth_out.prev_data==NULL)
	{
		printf("MALLOC  ERROR --- depth_out.prev_data av_malloc failed.\n");
		return H3D_RESULT_FAIL;
	}
	
	if(g_frameMaxNum>0)
	{
		// 保存当前帧
		if(g_frames[g_frameIndex].SegFlag)// &&g_frames[g_frameIndex].ChangeFlag==1) // TODO 可优化，判断是否修改
		{
			if(g_keyframeflag)
				sprintf(filename,"./%s/info/%d_infosnap_%d.dat",cfgparams.projectname,g_sectioncur,g_keyframenumarr[g_frameIndex]+1);
			else
				sprintf(filename,"./%s/info/%d_infosnap_%d.dat",cfgparams.projectname,g_sectioncur,g_frameIndex+1);
			if((g_frames[g_frameIndex].finfo=fopen(filename,"wb"))!=NULL)
			{
				// save info file
				saveinfofile(g_frames[g_frameIndex].finfo, g_frameIndex);
				fclose(g_frames[g_frameIndex].finfo);
			}
		}
		filenr = 0;

		//printf("first %d last %d\n", first, last);
		for(k=first;k<=last;k++)
		{
			g_frameIndex = k;
			if(g_depautoaddmotion==0)
				filenr = 0; // 第一帧不采用运动累计
			
			// 载入处理帧
			if(g_keyframeflag)
				sprintf(filename,"./%s/info/%d_infosnap_%d.dat",cfgparams.projectname,g_sectioncur,g_keyframenumarr[g_frameIndex]+1);
			else
				sprintf(filename,"./%s/info/%d_infosnap_%d.dat",cfgparams.projectname,g_sectioncur,g_frameIndex+1);
			if((g_frames[k].finfo=fopen(filename,"rb"))!=NULL)
			{
				loadinfofile(g_frames[k].finfo, k, 0);
				fclose(g_frames[k].finfo);
				printf("\rDEPTH    INFO --- Depthmap Auto Generation load  information frame index %d from %d to %d %.2lf%% ", k+1, first+1, last+1,((double)(k-first+1))/(last-first+1)*100);
			}
			else
			{
				// 未分割
				if(g_frames[k].SegFlag == 0||g_frames[k].AutoSegLabel <= g_segparamnumregion/4)
				{
					if(g_inputfileformat!=INPIMG)
					{
						#ifdef I64
						_fseeki64(g_fileInpYUV, (int64_t)k*IMAGE_SIZE(g_frameInWidth,g_frameInHeight), SEEK_SET);
						#else
						fseek(g_fileInpYUV, (unsigned long)k*IMAGE_SIZE(g_frameInWidth,g_frameInHeight), SEEK_SET);
						#endif
						if(g_frameResizeFlag==1)
						{
							fread(g_yuvorginput.y,(g_frameInWidth*g_frameInHeight),1,g_fileInpYUV);
							fread(g_yuvorginput.u,(g_frameInWidth*g_frameInHeight)>>2,1,g_fileInpYUV);
							fread(g_yuvorginput.v,(g_frameInWidth*g_frameInHeight)>>2,1,g_fileInpYUV);
							yuvresize(g_yuvorginput, g_yuvinput, g_frameInWidth, g_frameInHeight, g_frameWidth, g_frameHeight);
							yuv2rgb(g_yuvorginput.y, g_yuvorginput.v, g_yuvorginput.u, g_rgborginput, g_frameInWidth, g_frameInHeight);
						}
						else
						{
							fread(g_yuvinput.y,(g_frameWidth*g_frameHeight),1,g_fileInpYUV);
							fread(g_yuvinput.u,(g_frameWidth*g_frameHeight)>>2,1,g_fileInpYUV);
							fread(g_yuvinput.v,(g_frameWidth*g_frameHeight)>>2,1,g_fileInpYUV);
						}

						yuv2rgb(g_yuvinput.y, g_yuvinput.v, g_yuvinput.u, g_frames[k].rgbinput, g_frameWidth, g_frameHeight);
					}
					SegmentImgNoShow(k);
					// 保存处理帧
					if(g_keyframeflag)
						sprintf(filename,"./%s/info/%d_infosnap_%d.dat",cfgparams.projectname,g_sectioncur,g_keyframenumarr[g_frameIndex]+1);
					else
						sprintf(filename,"./%s/info/%d_infosnap_%d.dat",cfgparams.projectname,g_sectioncur,g_frameIndex+1);
					if((g_frames[k].finfo=fopen(filename,"wb"))!=NULL)
					{
						// save info file
						saveinfofile(g_frames[k].finfo, k);
						fclose(g_frames[k].finfo);
					}
					printf("\rDEPTH    INFO --- Depthmap Auto Generation automatic segment frame index %d from %d to %d %.2lf%% ", k+1, first+1, last+1,((double)(k-first+1))/(last-first+1)*100);
				}
			}
			
			
			// 当前帧深度未被赋值时或强制覆盖标志置位时才可以进行深度自动生成
			if(g_frames[k].DepFlag==0||g_depoverwritten==1)
			{
				if(g_inputfileformat!=INPIMG)
				{
					#ifdef I64
					_fseeki64(g_fileInpYUV, (int64_t)k*IMAGE_SIZE(g_frameInWidth,g_frameInHeight), SEEK_SET);
					#else
					fseek(g_fileInpYUV, (unsigned long)k*IMAGE_SIZE(g_frameInWidth,g_frameInHeight), SEEK_SET);
					#endif
				}

				depth_out.motionacc_data = macc_buffer;
				yuv_in.width = XDIM;
				yuv_in.height = YDIM;
				depth_out.width = XDIM;
				depth_out.height = YDIM;
				yuv_in.prev_data = in_prev_buffer;

				if(g_frameResizeFlag==0)
				{
					fread(in_buffer,(g_frameWidth*g_frameHeight)*3/2,1,g_fileInpYUV);
					/*深度图计算*/
					yuv_in.cur_data = in_buffer;
					CreateDepthMap(filenr, yuv_in, depth_out, delayed, pParameter);
					/*当前深度图信息保存*/
					memcpy(depth_out.prev_data, depth_out.cur_data[0], (2*XDIM*YDIM));
					/*当前帧信息保存*/
					memcpy(yuv_in.prev_data, yuv_in.cur_data, IMAGE_SIZE(XDIM,YDIM));
					
					filenr++;
					/* 保存深度图*/
					for(j=0;j<g_frameHeight;j++)
						for(i=0;i<g_frameWidth;i++)
						{
							//g_depthdisplay[i+j*g_frameWidth] = 
							g_frames[k].SegDepths[i+j*g_frameWidth]=depth_out.cur_data[0][i+j*g_frameWidth]>>8;
						}
				}
				else
				{
					fread(in_buffer,(g_frameInWidth*g_frameInHeight)*3/2,1,g_fileInpYUV);
					/*深度图计算*/
					yuv_in.cur_data = in_buffer;
					CreateDepthMap(filenr, yuv_in, depth_out, delayed, pParameter);
					/*当前深度图信息保存*/
					memcpy(depth_out.prev_data, depth_out.cur_data[0], (2*XDIM*YDIM));
					/*当前帧信息保存*/
					memcpy(yuv_in.prev_data, yuv_in.cur_data, IMAGE_SIZE(XDIM,YDIM));
					
					filenr++;
					
					/* 保存深度图*/
					for(j=0;j<g_frameInHeight;j++)
						for(i=0;i<g_frameInWidth;i++)
						{
							g_frames[k].SegDepths0[i+j*g_frameInWidth]=depth_out.cur_data[0][i+j*g_frameInWidth]>>8;
						}
						
					yresize(g_frames[k].SegDepths0,g_frames[k].SegDepths,g_frameInWidth,g_frameInHeight,g_frameWidth,g_frameHeight);
					
					/* 显示滤波后深度图*/
					//for(j=0;j<g_frameHeight;j++)
						//for(i=0;i<g_frameWidth;i++)
						//{
							//g_depthdisplay[i+j*g_frameWidth]
								// = g_frames[k].SegDepths[i+j*g_frameWidth];
						//}
				}
				g_frames[k].DepFlag = 1;

				// 保存处理帧
				if(g_keyframeflag)
					sprintf(filename,"./%s/info/%d_infosnap_%d.dat",cfgparams.projectname,g_sectioncur,g_keyframenumarr[g_frameIndex]+1);
				else
					sprintf(filename,"./%s/info/%d_infosnap_%d.dat",cfgparams.projectname,g_sectioncur,g_frameIndex+1);
				if((g_frames[k].finfo=fopen(filename,"wb"))!=NULL)
				{
					// save info file
					saveinfofile(g_frames[k].finfo, k);
					fclose(g_frames[k].finfo);
				}

			}
		}
		printf("\n");

		g_frameIndex = curidx;
		// 载入当前帧
		if(g_keyframeflag)
			sprintf(filename,"./%s/info/%d_infosnap_%d.dat",cfgparams.projectname,g_sectioncur,g_keyframenumarr[g_frameIndex]+1);
		else
			sprintf(filename,"./%s/info/%d_infosnap_%d.dat",cfgparams.projectname,g_sectioncur,g_frameIndex+1);
		if((g_frames[g_frameIndex].finfo=fopen(filename,"rb"))!=NULL)
		{
			// load info file
			loadinfofile(g_frames[g_frameIndex].finfo, g_frameIndex, 0);
			fclose(g_frames[g_frameIndex].finfo);
		}

		SFrameJump stmp;
		stmp.framenum = g_frameIndex+1;
		stmp.jumpmode = JMPMODECUR;
		jumpframe(NULL,&stmp);

		#if 1
		ShowAll();
		#else
		if(g_enlargeflag==0)
		{
			gdk_draw_gray_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
	          IMGWIDTHMAX, IMGHEIGHTMAX, g_frameWidth, g_frameHeight,
	          GDK_RGB_DITHER_MAX, g_depthdisplay, g_frameWidth);
		}
		else if(g_enlargeswitchflag==1) // 放大模式并显示深度情况
		{
			for(j=0;j<g_frameHeight*2;j++)
				for(i=0;i<g_frameWidth*2;i++)
				{
					ii = i+g_enlargeorgx;
					jj = j+g_enlargeorgy;
					g_enlargedisplaytmp0[i+j*g_frameWidth*2] = g_frames[g_frameIndex].SegDepths0[ii+jj*g_frameInWidth];
				}
			gdk_draw_gray_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
		          IMGWIDTHMAX-g_frameWidth, IMGHEIGHTMAX-g_frameHeight, g_frameWidth*2, g_frameHeight*2,
		          GDK_RGB_DITHER_MAX, g_enlargedisplaytmp0, g_frameWidth*2);
		}
		#endif
	}
	else
	{
		//messagebox("Please open a file first.");
	}

	free(in_buffer);
	free(in_prev_buffer);
	free(macc_buffer);
	
	for(i=0;i<STAGEMAX;i++)
	{
		free(depth_out.cur_data[i]);
	}
	free(depth_out.prev_data);
	
	return H3D_RESULT_OK;
}


