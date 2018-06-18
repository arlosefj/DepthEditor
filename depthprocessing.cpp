/* OPENCV */
#include "cv.h"   
#include "highgui.h"   


#include "common.h"
#include "project.h"

/* 深度图后处理*/
/*
  输入
  in_buffer			当前帧图像YUV or RGB
  in_prev_depth_buffer	前一帧深度图 Y（灰度图）
  in_depth_buffer		当前帧深度图 Y（灰度图）
  width				图像宽
  height				图像高
  isKey				关键帧标志
  param				待加，输入参数(如方法选择)
  输出
  out_buffer			后处理后当前帧深度图Y（灰度图）
  */

/**********************************************************************************
算法说明
深度图处理方法：
滤波（中值，平均，时域平滑，高斯，非对称双向滤波）采用
其中一种或多种组合(设置开关)。

深度图后处理in common.h
1,2一组，3,4,5,6,7一组，8一组，最多三个滤波，每组任选一个组合而成。
共3x6x2=36种组合方式。每一种滤波参数也不相同，所以变化比较多。
#define MIDFILTER 		0x01 // 中值滤波
#define MEANFILTER 		0x02 // 平均滤波

#define GAUSEFILTER 		0x04 // 高斯滤波
#define UNGAUSEFILTER	0x08 // 非对称高斯滤波
#define EDGEFILTER		0x10 // Edge空域滤波
#define JBFILTER			0x20 // 联合滤波
#define UNJBFILTER		0x40 // 非对称联合滤波

#define TEMFILTER			0x80 // 时域滤波

其他深度图处理方案，如增强立体感，参考专利CN200810112454.2 
三维影像显示深度调整方法。这个其实就是调整alpha，暂不用。

**********************************************************************************/

/* 参数调整TODO */
// 高斯滤波所用
//#define SIGMA 30//0.4f 
//#define SIZE 3
static const double PI = 3.14159265358979323846;

// Edge滤波所用
static const int EdgeRefineCnt = 6;
static const int OFFSET = 48;
static const float K = 16.0;

int g_depthprocessparam = 0;

void setnone0()
{
	g_depthprocessparam &= ~DEPPROCESSMASK1;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgDepProcnone0), TRUE);
}

void setmedian()
{
	g_depthprocessparam &= ~DEPPROCESSMASK1;
	g_depthprocessparam |= MIDFILTER;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgDepProcmedian), TRUE);
}

void setmean()
{
	g_depthprocessparam &= ~DEPPROCESSMASK1;
	g_depthprocessparam |= MEANFILTER;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgDepProcmean), TRUE);
}

void setnone1()
{
	g_depthprocessparam &= ~DEPPROCESSMASK2;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgDepProcnone1), TRUE);
}

void setgaussion()
{
	g_depthprocessparam &= ~DEPPROCESSMASK2;
	g_depthprocessparam |= GAUSEFILTER;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgDepProcgaussion), TRUE);
}

void setugaussion()
{
	g_depthprocessparam &= ~DEPPROCESSMASK2;
	g_depthprocessparam |= UNGAUSEFILTER;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgDepProcugaussion), TRUE);
}

void setedge()
{
	g_depthprocessparam &= ~DEPPROCESSMASK2;
	g_depthprocessparam |= EDGEFILTER;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgDepProcedge), TRUE);
}

void setjbf()
{
	g_depthprocessparam &= ~DEPPROCESSMASK2;
	g_depthprocessparam |= JBFILTER;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgDepProcjbf), TRUE);
}

void setujbf()
{
	g_depthprocessparam &= ~DEPPROCESSMASK2;
	g_depthprocessparam |= UNJBFILTER;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgDepProcujbf), TRUE);
}


void setnone2()
{
	g_depthprocessparam &= ~DEPPROCESSMASK3;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgDepProcnone2), TRUE);
}

void settimef()
{
	g_depthprocessparam &= ~DEPPROCESSMASK3;
	g_depthprocessparam |= TEMFILTER;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgDepProctimef), TRUE);
}

//  中值滤波
// 1x3中值
void DepthMidFilter(uint8_t * in_depth, uint8_t * out_depth, int width, int height)
{
	int i,j;
	
	memcpy(out_depth, in_depth, width*height);
	
	for(j=1;j<height-1;j++)
		for(i=1;i<width-1;i++)
		{
			out_depth[i+j*width] = (uint8_t)H3D_MEDIAN(in_depth[i-1+j*width],in_depth[i+j*width],in_depth[i+1+j*width]);
			out_depth[i+j*width] = (uint8_t)H3D_MEDIAN(in_depth[i+(j-1)*width],out_depth[i+j*width],in_depth[i+(j+1)*width]);
		}
		
	return ;
}

// 平均滤波
// 1x3均值or 加权均值
void DepthMeanFilter(uint8_t * in_depth, uint8_t * out_depth, int width, int height)
{
	int i,j;
		
	memcpy(out_depth, in_depth, width*height);
	
	for(j=1;j<height-1;j++)
		for(i=1;i<width-1;i++)
		{
			//out_depth[i+j*width] = (uint8_t)(((int)in_depth[i-1+j*width]+in_depth[i+j*width]+in_depth[i+1+j*width])/3);
			out_depth[i+j*width] = (uint8_t)(((int)in_depth[i-1+j*width]+2*in_depth[i+j*width]+in_depth[i+1+j*width])>>2);
			out_depth[i+j*width] = (uint8_t)(((int)in_depth[i+(j-1)*width]+2*out_depth[i+j*width]+in_depth[i+(j+1)*width])>>2);
		}

	return ;
}

// 高斯滤波
// x,y方向均滤波
void DepthGauseFilter(uint8_t * in_depth, uint8_t * out_depth, int width, int height)
{
	int i,x,y;
	double sigmax = g_depfiltgausesigmax;
	double sigmay = g_depfiltgausesigmay;
	int kernelsize;
	int center, halfLen;
	double *kernelx = NULL;
	double *kernely = NULL;
	double * tmpImage = NULL;
	double sum = 0;
	double dis, value;
	double dotMul = 0; // 高斯系数与图像数据的点乘
	double weightSum = 0; // 高斯滤波系数的总和

	// 数组长度，根据概率论的知识，选取[-3 * sigma, 3 * sigma]以内的数据。
	// 这些数据会覆盖绝大部分的滤波系数
	kernelsize = 1 + 2 * (int)ceil(3 * sigmax);
	center = (int)((kernelsize) / 2); // 数组的中心点

	kernelx = (double *)malloc(kernelsize*sizeof(double));
	tmpImage = (double *)malloc(width * height*sizeof(double));
	if(kernelx==NULL||tmpImage==NULL)
	{
		printf("MALLOC  ERROR --- kernel tmpImage malloc failed.\n");
		return ;
	}

	// 生成高斯数据
	for (i = 0; i < kernelsize; i++) {
		dis = (double)(i - center);
		value = exp(-(1/2) * dis * dis /(sigmax * sigmax)) / (sqrt(2 * PI) * sigmax);
		kernelx[i] = value;
		sum += value;
	}

	// 归一化
	for (i = 0; i < kernelsize; i++) {
		kernelx[i] = kernelx[i] / sum;
	}

	// x 方向进行滤波
	halfLen = kernelsize / 2;
	
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			dotMul = 0; // 高斯系数与图像数据的点乘
			weightSum = 0; // 高斯滤波系数的总和
			for (i = -halfLen; i <= halfLen; i++) {
				// 判断是否在图像内部
				if ((i+x) >= 0 && (i+x) < width ) {
					// 利用高斯系数对图像进行数字滤波
					dotMul += (double)in_depth[y * width + (i+x)] * kernelx[halfLen + i];
					weightSum += kernelx[halfLen + i];
				}
			}
			tmpImage[y * width + x] = dotMul / weightSum;
		}
	}

	kernelsize = 1 + 2 * (int)ceil(3 * sigmay);
	center = (int)((kernelsize) / 2); // 数组的中心点
	kernely = (double *)malloc(kernelsize*sizeof(double));
	if(kernely==NULL)
	{
		printf("MALLOC  ERROR --- kernely malloc failed.\n");
		return ;
	}

	// 生成高斯数据
	for (i = 0; i < kernelsize; i++) {
		dis = (double)(i - center);
		value = exp(-(1/2) * dis * dis /(sigmay * sigmay)) / (sqrt(2 * PI) * sigmay);
		kernely[i] = value;
		sum += value;
	}

	// 归一化
	for (i = 0; i < kernelsize; i++) {
		kernely[i] = kernely[i] / sum;
	}

	// y 方向进行滤波
	halfLen = kernelsize / 2;
	
	for (x = 0; x < width; x++) {
		for(y = 0; y < height; y++) {
			dotMul = 0;
			weightSum = 0;
			for(i = -halfLen; i <= halfLen; i++) {
				// 判断是否在图像内部
				if ((i+y) >= 0 && (i+y) < height) {
					dotMul += (double)tmpImage[(y+i) * width + x] * kernely[halfLen + i];
					weightSum += kernely[halfLen + i];
				}
			}
			out_depth[y * width + x] = (unsigned char)(int)(dotMul / weightSum);
		}
	}

	if(kernelx!=NULL)
		free(kernelx);
	if(kernely!=NULL)
		free(kernely);
	if(tmpImage!=NULL)
		free(tmpImage);

	return ;
}

// 非对称高斯滤波
// 仅在x方向均滤波
void DepthUnbalGauseFilter(uint8_t * in_depth, uint8_t * out_depth, int width, int height)
{
	int i,x,y;
	double sigma = g_depfiltgausesigmax;
	int kernelsize;
	int center, halfLen;
	double *kernel = NULL;
	double * tmpImage = NULL;
	double sum = 0;
	double dis, value;
	double dotMul = 0; // 高斯系数与图像数据的点乘
	double weightSum = 0; // 高斯滤波系数的总和

	// 数组长度，根据概率论的知识，选取[-3 * sigma, 3 * sigma]以内的数据。
	// 这些数据会覆盖绝大部分的滤波系数
	kernelsize = 1 + 2 * (int)ceil(3 * sigma);
	center = (int)((kernelsize) / 2); // 数组的中心点

	kernel = (double *)malloc(kernelsize*sizeof(double));
	tmpImage = (double *)malloc(width * height*sizeof(double));
	if(kernel==NULL||tmpImage==NULL)
	{
		printf("MALLOC  ERROR --- kernel tmpImage malloc failed.\n");
		return ;
	}

	// 生成高斯数据
	for (i = 0; i < kernelsize; i++) {
		dis = (double)(i - center);
		value = exp(-(1/2) * dis * dis /(sigma * sigma)) / (sqrt(2 * PI) * sigma);
		kernel[i] = value;
		sum += value;
	}

	// 归一化
	for (i = 0; i < kernelsize; i++) {
		kernel[i] = kernel[i] / sum;
	}

	// x 方向进行滤波
	halfLen = kernelsize / 2;
	
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			dotMul = 0; // 高斯系数与图像数据的点乘
			weightSum = 0; // 高斯滤波系数的总和
			for (i = -halfLen; i <= halfLen; i++) {
				// 判断是否在图像内部
				if ((i+x) >= 0 && (i+x) < width ) {
					// 利用高斯系数对图像进行数字滤波
					dotMul += (double)in_depth[y * width + (i+x)] * kernel[halfLen + i];
					weightSum += kernel[halfLen + i];
				}
			}
			if(weightSum!=0)
				tmpImage[y * width + x] = dotMul / weightSum;
			else
				tmpImage[y * width + x] = (double)in_depth[y * width + (i+x)];
		}
	}

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			out_depth[y * width + x] = (uint8_t)tmpImage[y * width + x];
		}
	}

	if(kernel!=NULL)
		free(kernel);
	if(tmpImage!=NULL)
		free(tmpImage);

	return ;

}

/* Depth Flaw 计算*/
void DepthflawCalc(unsigned char *rgb_input, float * up, float * left, int width, int height)
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
			up_diff = abs(rgb_input[j*width*3+i*3+0] - rgb_input[(j-1)*width*3+i*3+0]) +
				      abs(rgb_input[j*width*3+i*3+1] - rgb_input[(j-1)*width*3+i*3+1]) +
					  abs(rgb_input[j*width*3+i*3+2] - rgb_input[(j-1)*width*3+i*3+2]);
			if(up_diff<=OFFSET)
				up[j*width+i] = 1;
			else if(up_diff>(OFFSET+(K)))
				up[j*width+i] = 0;
			else
				up[j*width+i] = 1 - (up_diff-OFFSET)/K;
			
			left_diff = abs(rgb_input[j*width*3+i*3+0] - rgb_input[j*width*3+(i-1)*3+0]) +
				        abs(rgb_input[j*width*3+i*3+1] - rgb_input[j*width*3+(i-1)*3+1]) +
					    abs(rgb_input[j*width*3+i*3+2] - rgb_input[j*width*3+(i-1)*3+2]);
			if(left_diff<=OFFSET)
				left[j*width+i] = 1;
			else if(left_diff>(OFFSET+(K)))
				left[j*width+i] = 0;
			else
				left[j*width+i] = 1 - (left_diff-OFFSET)/K;
		}

	return ;
}


// Edge空域滤波
void DepthEdgeFilter(uint8_t * in_color, uint8_t * in_depth, uint8_t * out_depth, int width, int height)
{
	int i,j,k;
	uint8_t * tmp = NULL;
	float t;
	int count = EdgeRefineCnt;
	float * up = NULL;
	float * left = NULL;

	tmp = (uint8_t*)malloc(width*height*sizeof(uint8_t)); 
	if(tmp==NULL)
	{
		printf("MALLOC  ERROR --- tmp malloc failed.\n");
		return ;
	}

	up = (float*)malloc(width*height*sizeof(float)); 
	if(up==NULL)
	{
		printf("MALLOC  ERROR --- up malloc failed.\n");
		return ;
	}
	left = (float*)malloc(width*height*sizeof(float)); 
	if(left==NULL)
	{
		printf("MALLOC  ERROR --- left malloc failed.\n");
		return ;
	}	

	if(g_frameResizeFlag==1)
		DepthflawCalc(g_rgborginput, up, left, width, height);
	else
		DepthflawCalc(g_rgbinput, up, left, width, height);
	
	for(j=0; j<height; j++)
		for(i=0; i<width; i++)
		{
			tmp[j*width+i] = in_depth[j*width+i];
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
				out_depth[j*width+i] = tmp[j*width+i] + (uint8_t)(0.25*t);
			}
			
		for(j=0; j<height; j++)
			for(i=0; i<width; i++)
			{
				tmp[j*width+i] = out_depth[j*width+i];
			}
	}
	
	if(tmp!=NULL)
		free(tmp);
	if(up!=NULL)
		free(up);
	if(left!=NULL)
		free(left);
	
	return ;
}


// 联合滤波
// x,y方向滤波
void DepthJBFilter(uint8_t * in_color, uint8_t * in_depth, uint8_t * out_depth, int width, int height)
{
	int i,x,y,n;
	double sigmax = g_depfiltgausesigmax;
	double sigmay = g_depfiltgausesigmax;
	int kernelsize;
	int kernelsize2;
	int center, halfLen;
	double *kernelx = NULL;
	double *kernely = NULL;
	double weight;
	double * tmpImage = NULL;
	double sum = 0;
	double dis, value;
	double dotMul = 0; // 高斯系数与图像数据的点乘
	double weightSum = 0; // 高斯滤波系数的总和

	// 数组长度，根据概率论的知识，选取[-3 * sigma, 3 * sigma]以内的数据。
	// 这些数据会覆盖绝大部分的滤波系数
	kernelsize = 1 + 2 * (int)ceil(3 * sigmax);
	center = (int)((kernelsize) / 2); // 数组的中心点

	kernelx = (double *)malloc(kernelsize*sizeof(double));
	tmpImage = (double *)malloc(width * height*sizeof(double));
	if(kernelx==NULL||tmpImage==NULL)
	{
		printf("MALLOC  ERROR --- kernel tmpImage malloc failed.\n");
		return ;
	}

	// 生成高斯数据
	for (i = 0; i < kernelsize; i++) {
		dis = (double)(i - center);
		value = exp(-(1/2) * dis * dis /(sigmax * sigmax)) / (sqrt(2 * PI) * sigmax);
		kernelx[i] = value;
		sum += value;
	}

	// 归一化
	for (i = 0; i < kernelsize; i++) {
		kernelx[i] = kernelx[i] / sum;
	}

	
	halfLen = kernelsize / 2;
	kernelsize2 = kernelsize/2;

	// x 方向进行滤波
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			dotMul = 0; // 高斯系数与图像数据的点乘
			weightSum = 0; // 高斯滤波系数的总和
			for (i = -halfLen; i <= halfLen; i++) {
				// 判断是否在图像内部
				if ((i+x) >= 0 && (i+x) < width ) {
					// 利用高斯系数对图像进行数字滤波
					n = (int)in_color[y * width + (i+x)] - in_color[y * width + x];
					if(n>=-kernelsize2&&n<kernelsize2+1)
						weight = kernelx[kernelsize2 + n];
					else
						weight = 0;
					dotMul += (double)in_depth[y * width + (i+x)] * weight;
					weightSum += weight;
				}
			}
			tmpImage[y * width + x] = dotMul / weightSum;
		}
	}

	kernelsize = 1 + 2 * (int)ceil(3 * sigmay);
	center = (int)((kernelsize) / 2); // 数组的中心点

	kernely = (double *)malloc(kernelsize*sizeof(double));
	if(kernely==NULL)
	{
		printf("MALLOC  ERROR --- kernely malloc failed.\n");
		return ;
	}

	// 生成高斯数据
	for (i = 0; i < kernelsize; i++) {
		dis = (double)(i - center);
		value = exp(-(1/2) * dis * dis /(sigmay * sigmay)) / (sqrt(2 * PI) * sigmay);
		kernely[i] = value;
		sum += value;
	}

	// 归一化
	for (i = 0; i < kernelsize; i++) {
		kernely[i] = kernely[i] / sum;
	}

	
	halfLen = kernelsize / 2;
	kernelsize2 = kernelsize/2;

	// y 方向进行滤波
	for (x = 0; x < width; x++) {
		for(y = 0; y < height; y++) {
			dotMul = 0;
			weightSum = 0;
			for(i = -halfLen; i <= halfLen; i++) {
				// 判断是否在图像内部
				if ((i+y) >= 0 && (i+y) < height) {
					n = (int)in_color[(y+i) * width + x] - in_color[y * width + x];
					if(n>=-kernelsize2&&n<kernelsize2+1)
						weight = kernely[kernelsize2 + n];
					else
						weight = 0;
					dotMul += (double)tmpImage[(y+i) * width + x] * weight;
					weightSum += weight;
				}
			}
			out_depth[y * width + x] = (unsigned char)(int)(dotMul / weightSum);
		}
	}

	if(kernelx!=NULL)
		free(kernelx);
	if(kernely!=NULL)
		free(kernely);
	if(tmpImage!=NULL)
		free(tmpImage);

	return ;
}

// 非对称联合滤波
// 仅对x方向滤波
void DepthUJBFilter(uint8_t * in_color, uint8_t * in_depth, uint8_t * out_depth, int width, int height)
{
	int i,j,x,y,n;
	double sigma = g_depfiltgausesigmax;
	int kernelsize;
	int kernelsize2;
	int center, halfLen;
	double *kernel = NULL;
	double weight;
	double * tmpImage = NULL;
	double sum = 0;
	double dis, value;
	double dotMul = 0; // 高斯系数与图像数据的点乘
	double weightSum = 0; // 高斯滤波系数的总和

	// 数组长度，根据概率论的知识，选取[-3 * sigma, 3 * sigma]以内的数据。
	// 这些数据会覆盖绝大部分的滤波系数
	kernelsize = 1 + 2 * (int)ceil(3 * sigma);
	center = (int)((kernelsize) / 2); // 数组的中心点

	kernel = (double *)malloc(kernelsize*sizeof(double));
	tmpImage = (double *)malloc(width * height*sizeof(double));
	if(kernel==NULL||tmpImage==NULL)
	{
		printf("MALLOC  ERROR --- kernel tmpImage malloc failed.\n");
		return ;
	}

	// 生成高斯数据
	for (i = 0; i < kernelsize; i++) {
		dis = (double)(i - center);
		value = exp(-(1/2) * dis * dis /(sigma * sigma)) / (sqrt(2 * PI) * sigma);
		kernel[i] = value;
		sum += value;
	}

	// 归一化
	for (i = 0; i < kernelsize; i++) {
		kernel[i] = kernel[i] / sum;
	}

	
	halfLen = kernelsize / 2;
	kernelsize2 = kernelsize/2;

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			dotMul = 0; // 高斯系数与图像数据的点乘
			weightSum = 0; // 高斯滤波系数的总和
			for (i = -halfLen; i <= halfLen; i++) 
				for(j = -halfLen; j <= halfLen; j++){
					// 判断是否在图像内部
					if ((i+x) >= 0 && (i+x) < width && (j+y) >= 0 && (j+y) < height) {
						// 利用高斯系数对图像进行数字滤波
						n = (int)in_color[(y+j) * width + (i+x)] - in_color[y * width + x];
						if(n>=-kernelsize2&&n<kernelsize2+1)
							weight = kernel[kernelsize2 + n];
						else
							weight = 0;
						dotMul += (double)in_depth[(y+j) * width + (i+x)] * weight;
						weightSum += weight;
				}
			}
			tmpImage[y * width + x] = dotMul / weightSum;
		}
	}
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			out_depth[y * width + x] = (uint8_t)tmpImage[y * width + x];
		}
	}

	if(kernel!=NULL)
		free(kernel);
	if(tmpImage!=NULL)
		free(tmpImage);


	return ;
}

// 时域滤波
void DepthTimeFilter(uint8_t * in_depth, uint8_t * in_prev_depth, uint8_t * out_depth, int width, int height)
{
	int i,j;

	for(j=0;j<height;j++)
		for(i=0;i<width;i++)
		{
			//out_depth[i+j*width] = (uint8_t)((int)(in_prev_depth[i+j*width]>>3)*7+(in_depth[i+j*width]>>3));
			out_depth[i+j*width] = (uint8_t)((int)(in_prev_depth[i+j*width]>>3)+7*(in_depth[i+j*width]>>3));
		}

	return ;
}

void GauseFilter(char * in_depth, uint8_t * out_depth, int width, int height)
{
	int i,x,y;
	double sigma = g_depfiltgausesigmax;
	int kernelsize;
	int center, halfLen;
	double *kernel = NULL;
	double * tmpImage = NULL;
	double sum = 0;
	double dis, value;
	double dotMul = 0; // 高斯系数与图像数据的点乘
	double weightSum = 0; // 高斯滤波系数的总和

	// 数组长度，根据概率论的知识，选取[-3 * sigma, 3 * sigma]以内的数据。
	// 这些数据会覆盖绝大部分的滤波系数
	kernelsize = 1 + 2 * (int)ceil(3 * sigma);
	center = (int)((kernelsize) / 2); // 数组的中心点

	kernel = (double *)malloc(kernelsize*sizeof(double));
	tmpImage = (double *)malloc(width * height*sizeof(double));
	if(kernel==NULL||tmpImage==NULL)
	{
		printf("MALLOC  ERROR --- kernel tmpImage malloc failed.\n");
		return ;
	}

	// 生成高斯数据
	for (i = 0; i < kernelsize; i++) {
		dis = (double)(i - center);
		value = exp(-(1/2) * dis * dis /(sigma * sigma)) / (sqrt(2 * PI) * sigma);
		kernel[i] = value;
		sum += value;
	}

	// 归一化
	for (i = 0; i < kernelsize; i++) {
		kernel[i] = kernel[i] / sum;
	}

	// x 方向进行滤波
	halfLen = kernelsize / 2;
	
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			dotMul = 0; // 高斯系数与图像数据的点乘
			weightSum = 0; // 高斯滤波系数的总和
			for (i = -halfLen; i <= halfLen; i++) {
				// 判断是否在图像内部
				if ((i+x) >= 0 && (i+x) < width ) {
					// 利用高斯系数对图像进行数字滤波
					dotMul += (double)(uint8_t)in_depth[y * width + (i+x)] * kernel[halfLen + i];
					weightSum += kernel[halfLen + i];
				}
			}
			tmpImage[y * width + x] = dotMul / weightSum;
		}
	}

	// y 方向进行滤波
	for (x = 0; x < width; x++) {
		for(y = 0; y < height; y++) {
			dotMul = 0;
			weightSum = 0;
			for(i = -halfLen; i <= halfLen; i++) {
				// 判断是否在图像内部
				if ((i+y) >= 0 && (i+y) < height) {
					dotMul += (double)tmpImage[(y+i) * width + x] * kernel[halfLen + i];
					weightSum += kernel[halfLen + i];
				}
			}
			out_depth[y * width + x] = (unsigned char)(int)(dotMul / weightSum);
		}
	}

	if(kernel!=NULL)
		free(kernel);
	if(tmpImage!=NULL)
		free(tmpImage);

	return ;
}

// 非对称高斯滤波
// 仅在x方向均滤波
void UnbalGauseFilter(char * in_depth, uint8_t * out_depth, int width, int height)
{
	int i,x,y;
	double sigma = g_depfiltgausesigmax;
	int kernelsize;
	int center, halfLen;
	double *kernel = NULL;
	double * tmpImage = NULL;
	double sum = 0;
	double dis, value;
	double dotMul = 0; // 高斯系数与图像数据的点乘
	double weightSum = 0; // 高斯滤波系数的总和

	// 数组长度，根据概率论的知识，选取[-3 * sigma, 3 * sigma]以内的数据。
	// 这些数据会覆盖绝大部分的滤波系数
	kernelsize = 1 + 2 * (int)ceil(3 * sigma);
	center = (int)((kernelsize) / 2); // 数组的中心点

	kernel = (double *)malloc(kernelsize*sizeof(double));
	tmpImage = (double *)malloc(width * height*sizeof(double));
	if(kernel==NULL||tmpImage==NULL)
	{
		printf("MALLOC  ERROR --- kernel tmpImage malloc failed.\n");
		return ;
	}

	// 生成高斯数据
	for (i = 0; i < kernelsize; i++) {
		dis = (double)(i - center);
		value = exp(-(1/2) * dis * dis /(sigma * sigma)) / (sqrt(2 * PI) * sigma);
		kernel[i] = value;
		sum += value;
	}

	// 归一化
	for (i = 0; i < kernelsize; i++) {
		kernel[i] = kernel[i] / sum;
	}

	// x 方向进行滤波
	halfLen = kernelsize / 2;
	
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			dotMul = 0; // 高斯系数与图像数据的点乘
			weightSum = 0; // 高斯滤波系数的总和
			for (i = -halfLen; i <= halfLen; i++) {
				// 判断是否在图像内部
				if ((i+x) >= 0 && (i+x) < width ) {
					// 利用高斯系数对图像进行数字滤波
					dotMul += (double)(uint8_t)in_depth[y * width + (i+x)] * kernel[halfLen + i];
					weightSum += kernel[halfLen + i];
				}
			}
			if(weightSum!=0)
				tmpImage[y * width + x] = dotMul / weightSum;
			else
				tmpImage[y * width + x] = (double)(uint8_t)in_depth[y * width + (i+x)];
		}
	}

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			out_depth[y * width + x] = (uint8_t)tmpImage[y * width + x];
		}
	}

	if(kernel!=NULL)
		free(kernel);
	if(tmpImage!=NULL)
		free(tmpImage);

	return ;

}


void JointBilateralFilter(char * data, char * gray, int width, int height)
{
	int i,x,y,n;
	double sigma = g_depfiltgausesigmax;
	int kernelsize;
	int kernelsize2;
	int center, halfLen;
	double *kernel = NULL;
	double weight;
	double * tmpImage = NULL;
	double sum = 0;
	double dis, value;
	double dotMul = 0; // 高斯系数与图像数据的点乘
	double weightSum = 0; // 高斯滤波系数的总和

	// 数组长度，根据概率论的知识，选取[-3 * sigma, 3 * sigma]以内的数据。
	// 这些数据会覆盖绝大部分的滤波系数
	kernelsize = 1 + 2 * (int)ceil(3 * sigma);
	center = (int)((kernelsize) / 2); // 数组的中心点

	kernel = (double *)malloc(kernelsize*sizeof(double));
	tmpImage = (double *)malloc(width * height*sizeof(double));
	if(kernel==NULL||tmpImage==NULL)
	{
		printf("MALLOC  ERROR --- kernel tmpImage malloc failed.\n");
		return ;
	}

	// 生成高斯数据
	for (i = 0; i < kernelsize; i++) {
		dis = (double)(i - center);
		value = exp(-(1/2) * dis * dis /(sigma * sigma)) / (sqrt(2 * PI) * sigma);
		kernel[i] = value;
		sum += value;
	}

	// 归一化
	for (i = 0; i < kernelsize; i++) {
		kernel[i] = kernel[i] / sum;
	}

	
	halfLen = kernelsize / 2;
	kernelsize2 = kernelsize/2;

	// x 方向进行滤波
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			dotMul = 0; // 高斯系数与图像数据的点乘
			weightSum = 0; // 高斯滤波系数的总和
			for (i = -halfLen; i <= halfLen; i++) {
				// 判断是否在图像内部
				if ((i+x) >= 0 && (i+x) < width ) {
					// 利用高斯系数对图像进行数字滤波
					n = (int)(uint8_t)gray[y * width + (i+x)] - (int)(uint8_t)gray[y * width + x];
					if(n>=-kernelsize2&&n<kernelsize2+1)
						weight = kernel[kernelsize2 + n];
					else
						weight = 0;
					dotMul += (double)(uint8_t)data[y * width + (i+x)] * weight;
					weightSum += weight;
				}
			}
			tmpImage[y * width + x] = dotMul / weightSum;
		}
	}

	// y 方向进行滤波
	for (x = 0; x < width; x++) {
		for(y = 0; y < height; y++) {
			dotMul = 0;
			weightSum = 0;
			for(i = -halfLen; i <= halfLen; i++) {
				// 判断是否在图像内部
				if ((i+y) >= 0 && (i+y) < height) {
					n = (int)(uint8_t)gray[(y+i) * width + x] - (int)(uint8_t)gray[y * width + x];
					if(n>=-kernelsize2&&n<kernelsize2+1)
						weight = kernel[kernelsize2 + n];
					else
						weight = 0;
					dotMul += (double)tmpImage[(y+i) * width + x] * weight;
					weightSum += weight;
				}
			}
			data[y * width + x] = (unsigned char)(int)(dotMul / weightSum);
		}
	}

	free(kernel);
	free(tmpImage);

	
	return ;
}

void JointUBilateralFilter(char * data, char * gray, int width, int height)
{
	int i,j,x,y,n;
	double sigma = g_depfiltgausesigmax;
	int kernelsize;
	int kernelsize2;
	int center, halfLen;
	double *kernel = NULL;
	double weight;
	double * tmpImage = NULL;
	double sum = 0;
	double dis, value;
	double dotMul = 0; // 高斯系数与图像数据的点乘
	double weightSum = 0; // 高斯滤波系数的总和
	
	// 数组长度，根据概率论的知识，选取[-3 * sigma, 3 * sigma]以内的数据。
	// 这些数据会覆盖绝大部分的滤波系数
	kernelsize = 1 + 2 * (int)ceil(3 * sigma);
	center = (int)((kernelsize) / 2); // 数组的中心点

	kernel = (double *)malloc(kernelsize*sizeof(double));
	tmpImage = (double *)malloc(width * height*sizeof(double));
	if(kernel==NULL||tmpImage==NULL)
	{
		printf("MALLOC  ERROR --- kernel tmpImage malloc failed.\n");
		return ;
	}

	// 生成高斯数据
	for (i = 0; i < kernelsize; i++) {
		dis = (double)(i - center);
		value = exp(-(1/2) * dis * dis /(sigma * sigma)) / (sqrt(2 * PI) * sigma);
		kernel[i] = value;
		sum += value;
	}

	// 归一化
	for (i = 0; i < kernelsize; i++) {
		kernel[i] = kernel[i] / sum;
	}

	
	halfLen = kernelsize / 2;
	kernelsize2 = kernelsize/2;

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			dotMul = 0; // 高斯系数与图像数据的点乘
			weightSum = 0; // 高斯滤波系数的总和
			for (i = -halfLen; i <= halfLen; i++) 
				for(j = -halfLen; j <= halfLen; j++){
					// 判断是否在图像内部
					if ((i+x) >= 0 && (i+x) < width && (j+y) >= 0 && (j+y) < height) {
						// 利用高斯系数对图像进行数字滤波
						n = (int)(uint8_t)gray[(y+j) * width + (i+x)] - (int)(uint8_t)gray[y * width + x];
						if(n>=-kernelsize2&&n<kernelsize2+1)
							weight = kernel[kernelsize2 + n];
						else
							weight = 0;
						dotMul += (double)(uint8_t)data[(y+j) * width + (i+x)] * weight;
						weightSum += weight;
				}
			}
			tmpImage[y * width + x] = dotMul / weightSum;
		}
	}
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			data[y * width + x] = (char)tmpImage[y * width + x];
		}
	}

	free(kernel);
	free(tmpImage);

	
	return ;
}

/* 膨胀*/
void DilateFilter(char * src, char * dst, int iter, int width, int height)
{

	int i, j, m, n, k;
	int max;

	short * tmp0 = (short *) malloc(width*height*2);
	short * tmp1 = (short *) malloc(width*height*2);

	for(j=0;j<height;j++)
		for(i=0;i<width;i++)
		{
			tmp0[i+j*width] = tmp1[i+j*width] = (short)(uint8_t)src[i+j*width];
		}

	for(k=0;k<iter;k++)
	{
		for(j=1;j<height-1;j++)
			for(i=1;i<width-1;i++)
			{
				max = 0;
				for(m=-1;m<=1;m++)
					for(n=-1;n<=1;n++)
					{
						if(tmp0[i+n+(j+m)*width]>(short)max)
							max = tmp0[i+n+(j+m)*width];
					}
				tmp1[i+j*width] = max;
			}
		for(j=0;j<height;j++)
			for(i=0;i<width;i++)
			{
				tmp0[i+j*width] = tmp1[i+j*width];
			}
	}

	for(j=0;j<height;j++)
		for(i=0;i<width;i++)
		{
			dst[i+j*width] = (char)(uint8_t)tmp1[i+j*width];
		}

	if(tmp0!=NULL)
		free(tmp0);
	if(tmp1!=NULL)
		free(tmp1);
	
	return ;
}

/* 腐蚀*/
void ErodeFilter(char * src, char * dst, int iter, int width, int height)
{

	int i, j, m, n, k;
	int min;

	short * tmp0 = (short *) malloc(width*height*2);
	short * tmp1 = (short *) malloc(width*height*2);

	for(j=0;j<height;j++)
		for(i=0;i<width;i++)
		{
			tmp0[i+j*width] = tmp1[i+j*width] = (short)(uint8_t)src[i+j*width];
		}

	for(k=0;k<iter;k++)
	{
		for(j=1;j<height-1;j++)
			for(i=1;i<width-1;i++)
			{
				min = 255;
				for(m=-1;m<=1;m++)
					for(n=-1;n<=1;n++)
					{
						if(tmp0[i+n+(j+m)*width]<(short)min)
							min = tmp0[i+n+(j+m)*width];
					}
				tmp1[i+j*width] = min;
			}
		for(j=0;j<height;j++)
			for(i=0;i<width;i++)
			{
				tmp0[i+j*width] = tmp1[i+j*width];
			}
	}

	for(j=0;j<height;j++)
		for(i=0;i<width;i++)
		{
			dst[i+j*width] = (char)(uint8_t)tmp1[i+j*width];
		}

	if(tmp0!=NULL)
		free(tmp0);
	if(tmp1!=NULL)
		free(tmp1);
	
	return ;
}

void OpenFilter(char * src, char * dst, int iter, int width, int height)
{

	int i,j,k;
	char * tmp = (char *) malloc(width*height);

	for(j=0;j<height;j++)
		for(i=0;i<width;i++)
		{
			tmp[i+j*width] = src[i+j*width];
		}

	for(k=0;k<iter;k++)
	{
		ErodeFilter(tmp, tmp, 1, width, height);
		DilateFilter(tmp, tmp, 1, width, height);
	}

	for(j=0;j<height;j++)
		for(i=0;i<width;i++)
		{
			dst[i+j*width] = tmp[i+j*width];
		}

	if(tmp!=NULL)
		free(tmp);
	
	return ;
}

void CloseFilter(char * src, char * dst, int iter, int width, int height)
{

	int i,j,k;
	char * tmp = (char *) malloc(width*height);

	for(j=0;j<height;j++)
		for(i=0;i<width;i++)
		{
			tmp[i+j*width] = src[i+j*width];
		}

	for(k=0;k<iter;k++)
	{
		DilateFilter(tmp, tmp, 1, width, height);
		ErodeFilter(tmp, tmp, 1, width, height);
	}

	for(j=0;j<height;j++)
		for(i=0;i<width;i++)
		{
			dst[i+j*width] = tmp[i+j*width];
		}

	if(tmp!=NULL)
		free(tmp);
	
	return ;
}


void DepthProcess(uint8_t * in_buffer, uint8_t * in_prev_depth_buffer,
                      uint8_t * in_depth_buffer, uint8_t * out_buffer, 
                      int width, int height, int isKey, uint8_t param)
{

	int cnt1 = 0;
	int cnt2 = 0;
	uint8_t * depthbuf_tmp0 = NULL;
	uint8_t * depthbuf_tmp1 = NULL;
	uint8_t * depthbuf_tmp2 = NULL;

	memcpy(out_buffer, in_depth_buffer, width*height);

	depthbuf_tmp0 = (unsigned char *) malloc(width*height);
	depthbuf_tmp1 = (unsigned char *) malloc(width*height);
	if(depthbuf_tmp0==NULL||depthbuf_tmp1==NULL)
	{
		printf("MALLOC  ERROR --- depthbuf_tmp malloc failed.\n");
		return ;
	}

	// 检查参数
	if(param==0&&g_depfiltdilateiter==0)
	{
		printf("DEPTH    INFO --- Depth Processing No depthmap postprocess.\n");
		if(depthbuf_tmp0!=NULL)
			free(depthbuf_tmp0);
		if(depthbuf_tmp1!=NULL)
			free(depthbuf_tmp1);
		return ;
	}
	else
	{
		if((param&MIDFILTER)!=0) 		cnt1++;
		if((param&MEANFILTER)!=0) 	cnt1++;
		if((param&GAUSEFILTER)!=0)	cnt2++;
		if((param&UNGAUSEFILTER)!=0)	cnt2++;
		if((param&JBFILTER)!=0)		cnt2++;
		if((param&UNJBFILTER)!=0)		cnt2++;
		if((param&EDGEFILTER)!=0)		cnt2++;
	}
	#if 0
	if(g_frameResizeFlag==1) // 高分辨率模式
	{
		int i;
		char filename[256];
		IplImage* pImg_depth; // 输入深度图
		IplImage* pImg_updepth; // 深度图上采样

		IplImage* pImg_gray; // 输入图像
		IplImage* pImg_downgray; // 输入图像下采样

		CvSize sz;
	 
		sz.width = width;
		sz.height = height;
		pImg_downgray = cvCreateImage(sz, IPL_DEPTH_8U, 1);
		pImg_depth = cvCreateImage(sz, IPL_DEPTH_8U, 1);
		
		sz.width = g_frameInWidth;
		sz.height = g_frameInHeight;
		pImg_gray = cvCreateImage(sz, IPL_DEPTH_8U, 1);
		pImg_updepth = cvCreateImage(sz, IPL_DEPTH_8U, 1);

		memcpy(pImg_gray->imageData, g_yuvorginput.y, g_frameInWidth*g_frameInHeight);
		
		//cvResize(pImg_gray, pImg_downgray,CV_INTER_CUBIC);
		InterpYuvBilinear((uint8_t *)(pImg_gray->imageData), (uint8_t *)(pImg_downgray->imageData), g_frameInWidth, g_frameInHeight, width, height);
		
		memcpy(pImg_depth->imageData, in_depth_buffer, width*height);

		InterpYuvBilinear((uint8_t *)(pImg_depth->imageData), (uint8_t *)(pImg_updepth->imageData), width, height, g_frameInWidth, g_frameInHeight);
		
		cvResize(pImg_depth, pImg_updepth,CV_INTER_CUBIC);
		//sprintf(filename,"./%s/updepth0.png",cfgparams.projectname);
		//cvSaveImage(filename, pImg_updepth);

		for(i=0;i<g_frameInWidth*g_frameInHeight;i++)
		{
			g_orgdepth[i] = (uint8_t)pImg_updepth->imageData[i];
		}
		
		#if 1
		if((param&GAUSEFILTER)!=0)
		{
			GauseFilter(pImg_updepth->imageData, g_orgdepth, g_frameInWidth, g_frameInHeight);
		}
		if((param&UNGAUSEFILTER)!=0)
		{
			UnbalGauseFilter(pImg_updepth->imageData, g_orgdepth, g_frameInWidth, g_frameInHeight);
		}
		if((param&JBFILTER)!=0)
		{
			JointBilateralFilter(pImg_updepth->imageData, pImg_gray->imageData, g_frameInWidth, g_frameInHeight);
			for(i=0;i<g_frameInWidth*g_frameInHeight;i++)
			{
				g_orgdepth[i] = (uint8_t)pImg_updepth->imageData[i];
			}
		}
		if((param&UNJBFILTER)!=0)
		{
			JointUBilateralFilter(pImg_updepth->imageData, pImg_gray->imageData, g_frameInWidth, g_frameInHeight);
			for(i=0;i<g_frameInWidth*g_frameInHeight;i++)
			{
				g_orgdepth[i] = (uint8_t)pImg_updepth->imageData[i];
			}
		}

		DilateFilter((char*)g_orgdepth, pImg_updepth->imageData, g_depfiltdilateiter, g_frameInWidth, g_frameInHeight);

		for(i=0;i<g_frameInWidth*g_frameInHeight;i++)
		{
			g_orgdepth[i] = (uint8_t)pImg_updepth->imageData[i];
		}
		sprintf(filename,"./%s/depth/depthsnap_%d.png",cfgparams.projectname, g_frameIndex+1);
		cvSaveImage(filename, pImg_updepth);
		#else
		JointBilateralFilter(pImg_gray->imageData, pImg_updepth->imageData, g_frameInWidth, g_frameInHeight);
		#endif

		//cvResize(pImg_updepth, pImg_depth,CV_INTER_CUBIC);
		InterpYuvBilinear((uint8_t *)(pImg_updepth->imageData), (uint8_t *)(pImg_depth->imageData), g_frameInWidth, g_frameInHeight, width, height);
		#if 1
		for(i=0;i<width*height;i++)
		{
			out_buffer[i] = pImg_depth->imageData[i];
		}
		#else
		for(j=0;j<height;j++)
			for(i=0;i<width;i++)
				out_buffer[i+j*width] = pImg_updepth->imageData[i+600+(j+60)*g_frameInWidth];
		#endif

		cvReleaseImage( &pImg_gray );
		cvReleaseImage( &pImg_updepth );
		cvReleaseImage( &pImg_downgray );
		cvReleaseImage( &pImg_depth );
	}
	else
	#endif
	{
		int i,j;
		char filename[256];
		IplImage* pImg_depth; // 输入深度图
		CvSize sz;
		sz.width = width;
		sz.height = height;
		pImg_depth = cvCreateImage(sz, IPL_DEPTH_8U, 1);
		/***************************************************************************/
		// 中值滤波
		if((param&MIDFILTER)!=0)
		{
			DepthMidFilter(in_depth_buffer, depthbuf_tmp0, width, height);
		}

		// 平均滤波
		if((param&MEANFILTER)!=0)
		{
			DepthMeanFilter(in_depth_buffer, depthbuf_tmp0, width, height);
		}
		
		if(cnt1==0)
			memcpy(depthbuf_tmp0, in_depth_buffer, (width*height));

		/***************************************************************************/
		// 高斯滤波
		if((param&GAUSEFILTER)!=0)
		{
			DepthGauseFilter(depthbuf_tmp0, depthbuf_tmp1, width, height);
		}

		// 非对称高斯滤波
		if((param&UNGAUSEFILTER)!=0)
		{
			DepthUnbalGauseFilter(depthbuf_tmp0, depthbuf_tmp1, width, height);
		}

		// 联合滤波
		if((param&JBFILTER)!=0)
		{
			DepthJBFilter(in_buffer, depthbuf_tmp0, depthbuf_tmp1, width, height);
		}

		// 非对称联合滤波
		if((param&UNJBFILTER)!=0)
		{
			DepthUJBFilter(in_buffer, depthbuf_tmp0, depthbuf_tmp1, width, height);
		}

		// Edge空域滤波
		if((param&EDGEFILTER)!=0)
		{
			DepthEdgeFilter(in_buffer, depthbuf_tmp0, depthbuf_tmp1, width, height);
		}
		
		if(cnt2==0)
			memcpy(depthbuf_tmp1, depthbuf_tmp0, (width*height));
		
		/***************************************************************************/
		// 时域滤波
		if((param&TEMFILTER)!=0&&isKey==0)
		{
			DepthTimeFilter(depthbuf_tmp1, in_prev_depth_buffer, out_buffer, width, height);
		}
		else
		{
			memcpy(out_buffer, depthbuf_tmp1, (width*height));
		}

		/***************************************************************************/

		// 膨胀操作
		DilateFilter((char *)out_buffer, (char *)out_buffer, g_depfiltdilateiter, width, height);
		
		//memcpy(pImg_depth->imageData, out_buffer, width*height);
		for(j=0;j<height;j++)
			for(i=0;i<width;i++)
			{
				pImg_depth->imageData[i+j*pImg_depth->widthStep] = out_buffer[i+j*width];
			}
		if(g_keyframeflag)
			sprintf(filename,"./%s/depth/%d_depthsnap_%d.png",cfgparams.projectname, g_sectioncur, g_keyframenumarr[g_frameIndex]+1);
		else
			sprintf(filename,"./%s/depth/%d_depthsnap_%d.png",cfgparams.projectname, g_sectioncur, g_frameIndex+1);
		cvSaveImage(filename, pImg_depth);
		cvReleaseImage( &pImg_depth );
		
	}
	
	if(depthbuf_tmp0!=NULL)
		free(depthbuf_tmp0);
	if(depthbuf_tmp1!=NULL)
		free(depthbuf_tmp1);
	
	return ;
}

void depthimgproc(int first, int last)
{
	//int i,j,ii,jj;
	int k;
	int prevframenum;
	int isfirst;
	uint8_t * indepth;
	uint8_t * inprevdepth;
	//uint8_t * outdepth0;
	//uint8_t * outdepth;
	char filename[256];

	

	//printf("g_depthprocessparam: 0x%x \n", g_depthprocessparam);

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

		//printf("first %d last %d\n", first, last);
		for(k=first;k<=last;k++)
		{
			isfirst = k==0?1:0;
			prevframenum = k - 1;
			
			if(g_frames[k].DepFlag==1)
			{
				/*获取当前深度图*/
				#if 0
				if(g_depeditflag==0) // 不在深度图编辑模式时深度图重新生成
				{
					for(j=0;j<g_frames[g_frameIndex].LayerVisibleNum;j++)
					{
						int lb = g_frameWidth*g_frameHeight + (1<<j);
						if(g_frames[g_frameIndex].layerdepth[j].setflag)
						{
							if(g_frameResizeFlag==1)
							{
								set_depth_value0(g_frameIndex, lb, g_frames[g_frameIndex].layerdepth[j].depthmin, g_frames[g_frameIndex].layerdepth[j].depthmax, g_frames[g_frameIndex].layerdepth[j].depthmode);
								yresize(g_SegDepths0,g_SegDepths,g_frameInWidth,g_frameInHeight,g_frameWidth,g_frameHeight);
							}
							else
								set_depth_value(g_frameIndex, lb, g_frames[g_frameIndex].layerdepth[j].depthmin, g_frames[g_frameIndex].layerdepth[j].depthmax, g_frames[g_frameIndex].layerdepth[j].depthmode);
						}

					}
				}
				#endif
				// 载入处理帧
				if(g_keyframeflag)
					sprintf(filename,"./%s/info/%d_infosnap_%d.dat",cfgparams.projectname,g_sectioncur,g_keyframenumarr[k]+1);
				else
					sprintf(filename,"./%s/info/%d_infosnap_%d.dat",cfgparams.projectname,g_sectioncur,k+1);
				if((g_frames[k].finfo=fopen(filename,"rb"))!=NULL)
				{
					loadinfofile(g_frames[k].finfo, k, 0);
					fclose(g_frames[k].finfo);
				}
				if((g_depthprocessparam&TEMFILTER)!=0&&isfirst==0)
				{
					if(g_keyframeflag)
						sprintf(filename,"./%s/info/%d_infosnap_%d.dat",cfgparams.projectname,g_sectioncur,g_keyframenumarr[prevframenum]+1);
					else
						sprintf(filename,"./%s/info/%d_infosnap_%d.dat",cfgparams.projectname,g_sectioncur,prevframenum+1);
					if((g_frames[prevframenum].finfo=fopen(filename,"rb"))!=NULL)
					{
						loadinfofile(g_frames[prevframenum].finfo, prevframenum, 1);
						fclose(g_frames[prevframenum].finfo);
					}
				}
				
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
					}
					else
					{
						fread(g_yuvinput.y,(g_frameWidth*g_frameHeight),1,g_fileInpYUV);
						fread(g_yuvinput.u,(g_frameWidth*g_frameHeight)>>2,1,g_fileInpYUV);
						fread(g_yuvinput.v,(g_frameWidth*g_frameHeight)>>2,1,g_fileInpYUV);
					}
				
				}

				if(g_frameResizeFlag==0)
				{
					indepth = g_frames[k].SegDepths;
					if(prevframenum>0&&g_frames[prevframenum].SegFlag==1)
						inprevdepth = g_frames[prevframenum].SegDepths;
					else
						inprevdepth = g_frames[k].SegDepths;

					
				
					/* 深度处理*/
					DepthProcess(g_yuvinput.y, inprevdepth, indepth, g_frames[k].SegDepths, 
		                      g_frameWidth, g_frameHeight, isfirst, g_depthprocessparam); // TODO 输入仍为yuv

					/* 显示滤波后深度图*/
					//for(j=0;j<g_frameHeight;j++)
						//for(i=0;i<g_frameWidth;i++)
						//{
							//g_depthdisplay[i+j*g_frameWidth]
								// = g_frames[k].SegDepths[i+j*g_frameWidth];
						//}
				}
				else
				{
					indepth = g_frames[k].SegDepths0;
					if(prevframenum>0&&g_frames[prevframenum].SegFlag==1)
						inprevdepth = g_frames[prevframenum].SegDepths0;
					else
						inprevdepth = g_frames[k].SegDepths0;

					//outdepth0 = new uint8_t [g_frameInWidth*g_frameInHeight];
					//outdepth = new uint8_t [g_frameWidth*g_frameHeight];
				
					/* 深度处理*/
					DepthProcess(g_yuvorginput.y, inprevdepth, indepth, g_frames[k].SegDepths0, 
		                      g_frameInWidth, g_frameInHeight, isfirst, g_depthprocessparam); // TODO 输入仍为yuv
					yresize(g_frames[k].SegDepths0,g_frames[k].SegDepths,g_frameInWidth,g_frameInHeight,g_frameWidth,g_frameHeight);
					
					/* 显示滤波后深度图*/
					//for(j=0;j<g_frameHeight;j++)
						//for(i=0;i<g_frameWidth;i++)
						//{
							//g_depthdisplay[i+j*g_frameWidth]
								// = g_frames[k].SegDepths[i+j*g_frameWidth];
						//}
				}
				printf("\rDEPTH    INFO --- Depth Processing Processing frame index %d from %d to %d %.2lf%% ", k+1, first+1, last+1,((double)(k-first+1))/(last-first+1)*100);

				// 保存处理帧
				if(g_keyframeflag)
					sprintf(filename,"./%s/info/%d_infosnap_%d.dat",cfgparams.projectname,g_sectioncur,g_keyframenumarr[k]+1);
				else
					sprintf(filename,"./%s/info/%d_infosnap_%d.dat",cfgparams.projectname,g_sectioncur,k+1);
				if((g_frames[k].finfo=fopen(filename,"wb"))!=NULL)
				{
					// save info file
					saveinfofile(g_frames[k].finfo, k);
					fclose(g_frames[k].finfo);
				}
			}
		}
		printf("\n");


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

		#if 0
		if(g_inputfileformat!=INPIMG)
		{
			#ifdef I64
			_fseeki64(g_fileInpYUV, (int64_t)g_frameIndex*IMAGE_SIZE(g_frameInWidth,g_frameInHeight), SEEK_SET);
			#else
			fseek(g_fileInpYUV, (unsigned long)g_frameIndex*IMAGE_SIZE(g_frameInWidth,g_frameInHeight), SEEK_SET);
			#endif
			if(g_frameResizeFlag==1)
			{
				fread(g_yuvorginput.y,(g_frameInWidth*g_frameInHeight),1,g_fileInpYUV);
				fread(g_yuvorginput.u,(g_frameInWidth*g_frameInHeight)>>2,1,g_fileInpYUV);
				fread(g_yuvorginput.v,(g_frameInWidth*g_frameInHeight)>>2,1,g_fileInpYUV);
				yuvresize(g_yuvorginput, g_yuvinput, g_frameInWidth, g_frameInHeight, g_frameWidth, g_frameHeight);
			}
			else
			{
				fread(g_yuvinput.y,(g_frameWidth*g_frameHeight),1,g_fileInpYUV);
				fread(g_yuvinput.u,(g_frameWidth*g_frameHeight)>>2,1,g_fileInpYUV);
				fread(g_yuvinput.v,(g_frameWidth*g_frameHeight)>>2,1,g_fileInpYUV);
			}
		
		}
		#endif

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
		else if(g_enlargeswitchflag==1)
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
	
	return ;
}

void DepthEditIncFlatSquare(uint8_t * Depths, int * labels, int lb, int x, int y, int rad, int width, int height)
{
	int i, j, ii, jj;
	int tmp, step;

	for(j=-rad;j<=rad;j++)
		for(i=-rad;i<=rad;i++)
		{
			ii = x+i;
			jj = y+j;
			if(ii>=0&&ii<width&&jj>=0&&jj<height&&labels[ii+jj*width]==lb)
			{	
				step = rad;
				tmp = H3D_MIN(step+Depths[ii+jj*width],255);
				Depths[ii+jj*width] = tmp;
			}
		}
	
	return ;
}

void DepthEditIncFlatCircle(uint8_t * Depths, int * labels, int lb, int x, int y, int rad, int width, int height)
{
	int i, j, ii, jj, dis;
	int tmp, step;

	for(j=-rad;j<=rad;j++)
		for(i=-rad;i<=rad;i++)
		{
			ii = x+i;
			jj = y+j;
			dis = (int)sqrt((double)(i*i+j*j));
			if(ii>=0&&ii<width&&jj>=0&&jj<height&&dis<=rad&&labels[ii+jj*width]==lb)
			{	
				step = rad;
				tmp = H3D_MIN(step+Depths[ii+jj*width],255);
				Depths[ii+jj*width] = tmp;
			}
		}
	
	return ;
}

void DepthEditIncGradSquare(uint8_t * Depths, int * labels, int lb, int x, int y, int rad, int width, int height)
{
	int i, j, ii, jj;
	int tmp, step;

	for(j=-rad;j<=rad;j++)
		for(i=-rad;i<=rad;i++)
		{
			ii = x+i;
			jj = y+j;
			if(ii>=0&&ii<width&&jj>=0&&jj<height&&labels[ii+jj*width]==lb)
			{	
				step = rad+1-(H3D_MAX(abs(i),abs(j)));
				tmp = H3D_MIN(step+Depths[ii+jj*width],255);
				Depths[ii+jj*width] = tmp;
			}
		}
	
	return ;
}

void DepthEditIncGradCircle(uint8_t * Depths, int * labels, int lb, int x, int y, int rad, int width, int height)
{
	int i, j, ii, jj, dis;
	int tmp, step;

	for(j=-rad;j<=rad;j++)
		for(i=-rad;i<=rad;i++)
		{
			ii = x+i;
			jj = y+j;
			dis = (int)sqrt((double)(i*i+j*j));
			if(ii>=0&&ii<width&&jj>=0&&jj<height&&dis<=rad&&labels[ii+jj*width]==lb)
			{	
				step = rad+1-dis;
				tmp = H3D_MIN(step+Depths[ii+jj*width],255);
				Depths[ii+jj*width] = tmp;
			}
		}
	
	return ;
}

void DepthEditDecFlatSquare(uint8_t * Depths, int * labels, int lb, int x, int y, int rad, int width, int height)
{
	int i, j, ii, jj;
	int tmp, step;

	for(j=-rad;j<=rad;j++)
		for(i=-rad;i<=rad;i++)
		{
			ii = x+i;
			jj = y+j;
			if(ii>=0&&ii<width&&jj>=0&&jj<height&&labels[ii+jj*width]==lb)
			{	
				step = rad;
				tmp = H3D_MAX(Depths[ii+jj*width]-step,0);
				Depths[ii+jj*width] = tmp;
			}
		}
	
	return ;
}

void DepthEditDecFlatCircle(uint8_t * Depths, int * labels, int lb, int x, int y, int rad, int width, int height)
{
	int i, j, ii, jj, dis;
	int tmp, step;

	for(j=-rad;j<=rad;j++)
		for(i=-rad;i<=rad;i++)
		{
			ii = x+i;
			jj = y+j;
			dis = (int)sqrt((double)(i*i+j*j));
			if(ii>=0&&ii<width&&jj>=0&&jj<height&&dis<=rad&&labels[ii+jj*width]==lb)
			{	
				step = rad;
				tmp = H3D_MAX(Depths[ii+jj*width]-step,0);
				Depths[ii+jj*width] = tmp;
			}
		}
	
	return ;
}

void DepthEditDecGradSquare(uint8_t * Depths, int * labels, int lb, int x, int y, int rad, int width, int height)
{
	int i, j, ii, jj;
	int tmp, step;

	for(j=-rad;j<=rad;j++)
		for(i=-rad;i<=rad;i++)
		{
			ii = x+i;
			jj = y+j;
			if(ii>=0&&ii<width&&jj>=0&&jj<height&&labels[ii+jj*width]==lb)
			{	
				step = rad+1-(H3D_MAX(abs(i),abs(j)));
				tmp = H3D_MAX(Depths[ii+jj*width]-step,0);
				Depths[ii+jj*width] = tmp;
			}
		}
	
	return ;
}

void DepthEditDecGradCircle(uint8_t * Depths, int * labels, int lb, int x, int y, int rad, int width, int height)
{
	int i, j, ii, jj, dis;
	int tmp, step;

	for(j=-rad;j<=rad;j++)
		for(i=-rad;i<=rad;i++)
		{
			ii = x+i;
			jj = y+j;
			dis = (int)sqrt((double)(i*i+j*j));
			if(ii>=0&&ii<width&&jj>=0&&jj<height&&dis<=rad&&labels[ii+jj*width]==lb)
			{	
				step = rad+1-dis;
				tmp = H3D_MAX(Depths[ii+jj*width]-step,0);
				Depths[ii+jj*width] = tmp;
			}
		}
	
	return ;
}

void DepthFineEdit(uint8_t * Depths, int * labels, int lb, int rad, int x, int y, int width, int height)
{
		
	if(g_depeditmode==DEPEDITMODEINCFLAT)
	{
		if(g_depeditmodule==DEPEDITMODULESQU)
		{
			DepthEditIncFlatSquare(Depths, labels, lb, x, y, rad, width, height);
		}
		else
		{
			DepthEditIncFlatCircle(Depths, labels, lb, x, y, rad, width, height);
		}		
	}
	else if(g_depeditmode==DEPEDITMODEINCGRAD)
	{
		if(g_depeditmodule==DEPEDITMODULESQU)
		{
			DepthEditIncGradSquare(Depths, labels, lb, x, y, rad, width, height);
		}
		else
		{
			DepthEditIncGradCircle(Depths, labels, lb, x, y, rad, width, height);
		}
	}
	else if(g_depeditmode==DEPEDITMODEDECFLAT)
	{
		if(g_depeditmodule==DEPEDITMODULESQU)
		{
			DepthEditDecFlatSquare(Depths, labels, lb, x, y, rad, width, height);
		}
		else
		{
			DepthEditDecFlatCircle(Depths, labels, lb, x, y, rad, width, height);
		}
	}
	else // (g_depeditmode==DEPEDITMODEDECGRAD)
	{
		if(g_depeditmodule==DEPEDITMODULESQU)
		{
			DepthEditDecGradSquare(Depths, labels, lb, x, y, rad, width, height);
		}
		else
		{
			DepthEditDecGradCircle(Depths, labels, lb, x, y, rad, width, height);
		}
	}
	return ;

}

