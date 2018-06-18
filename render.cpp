
/* OPENCV */
#include "cv.h"   
#include "highgui.h"   

#include "common.h"
#include "project.h"
#include <io.h>

//#define YTORGB
//#define BASEDEPTH 127 // 焦平面位置，大于该值为出屏，小于该值为内屏，可调

// 空洞时填充值
#define HOLER 0
#define HOLEG 0
#define HOLEB 255

int g_renderoutputformat = RENDEROUTLR;
int g_renderalpha = RENPARAM_ALPHADEF;
int g_renderforcalplane = RENPARAM_FPDEF;
int g_renderprocmethod = RENDERPROCNONE;

int g_renderoutputantype = RENDEROUTAN_RE_BL;

int g_renderedgesoftenmethod = RENDEREDGESOFTENMEAN;
int g_renderedgesoftenth = RENPARAM_ESTHDEF;

// 是否进入render模式标志
int g_renderflag = 0;
// render参考标志
int g_renderrefflag = 0;
int g_rendereditrefxcenter = 0;
int g_rendereditrefycenter = 0;
int g_rendereditrefxleft = 0;
int g_rendereditrefyup = 0;
int g_rendereditrefxright = 0;
int g_rendereditrefydown = 0;

int g_renderdisplayflag = 0;

int g_rendermethod = RENDERPUSH;

#define SOURCE_INTERP_MASK 0xC000
#define SOURCE_LEFT_JITTER 0x8000

void setleftright()
{
	g_renderoutputformat = RENDEROUTLR;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgRenderOutLR), TRUE);
	return ;
}

void setanaglyph()
{
	g_renderoutputformat = RENDEROUTAN;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgRenderOutAnag), TRUE);
	return ;
}

void setredblue()
{
	g_renderoutputantype = RENDEROUTAN_RE_BL;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgRenderOutAnRedBlue), TRUE);
	return ;
}

void setredgreen()
{
	g_renderoutputantype = RENDEROUTAN_RE_GR;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgRenderOutAnRedGreen), TRUE);
	return ;
}

void setgreenred()
{
	g_renderoutputantype = RENDEROUTAN_GR_RE;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgRenderOutAnGreenRed), TRUE);
	return ;
}

void setbrownblue()
{
	g_renderoutputantype = RENDEROUTAN_BR_BL;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgRenderOutAnBrownBlue), TRUE);
	return ;
}

void setwarp()
{
	g_rendermethod = RENDERWARP;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgRenderMethodWarp), TRUE);
	return ;
}

void sets2d()
{
	g_rendermethod = RENDERPULL;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgRenderMethodS2D), TRUE);
	return ;
}

void setd2s()
{
	g_rendermethod = RENDERPUSH;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgRenderMethodD2S), TRUE);
	return ;
}

void setnone()
{
	g_renderprocmethod = RENDERPROCNONE;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgRenderProcNone), TRUE);
	return ;
}

void sethollfill()
{
	g_renderprocmethod = RENDERPROCHOLLFILL;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgRenderProcHollFill), TRUE);
	return ;
}

void setinpainting()
{
	g_renderprocmethod = RENDERPROCINPAINTING;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgRenderProcInpainting), TRUE);
	return ;
}

void sethhf()
{
	g_renderprocmethod = RENDERPROCHHF;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgRenderProcHHF), TRUE);
	return ;
}


void setresnone()
{
	g_renderedgesoftenmethod = RENDEREDGESOFTENNON;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgRenderEdgeSoftenNone), TRUE);
	return ;
}

void setresmean()
{
	g_renderedgesoftenmethod = RENDEREDGESOFTENMEAN;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgRenderEdgeSoftenMean), TRUE);
	return ;
}

void setresmedian()
{
	g_renderedgesoftenmethod = RENDEREDGESOFTENMID;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgRenderEdgeSoftenMedian), TRUE);
	return ;
}

// 视点Render
void RenderView(uint8_t *rgb_output, uint8_t * rgb_input, uint8_t * in_depths, int inwidth, int inheight, int lrflag, int bgflag)
{
	int i,j;
	unsigned char *leftlabel_output = NULL;
	unsigned char *rightlabel_output = NULL;
	int *leftmatch = NULL;
	int *rightmatch = NULL;
	double disparity, alpha;
	double pos_left, pos_right;
	int pos;

	/* 标记值初始化，初始值设定*/
	if(lrflag&0x1) // 有左视点
	{
		leftlabel_output = new uint8_t[inwidth*inheight];
		memset(leftlabel_output, 0, inwidth*inheight);
		leftmatch = new int[inwidth*inheight];
		for(j=0; j<inheight; j++)
			for(i=0; i<inwidth; i++)
			{
				rgb_output[j*inwidth*6+(i)*3+0] = HOLER;
				rgb_output[j*inwidth*6+(i)*3+1] = HOLEG;
				rgb_output[j*inwidth*6+(i)*3+2] = HOLEB;
				leftmatch[i+j*inwidth] = 0;
			}
	}
	if(lrflag&0x2) // 有右视点
	{
		rightlabel_output = new uint8_t[inwidth*inheight];
		memset(rightlabel_output, 0, inwidth*inheight);
		rightmatch = new int[inwidth*inheight];
		for(j=0; j<inheight; j++)
			for(i=0; i<inwidth; i++)
			{
				rgb_output[j*inwidth*6+inwidth*3+(i)*3+0] = HOLER;
				rgb_output[j*inwidth*6+inwidth*3+(i)*3+1] = HOLEG;
				rgb_output[j*inwidth*6+inwidth*3+(i)*3+2] = HOLEB;
				rightmatch[i+j*inwidth] = 0;
			}
	}

	/* 像素移位*/
	alpha = 0.01*inwidth*0.20*g_renderalpha;

	if(lrflag&0x1) // 左视点
	{
		for(j=0; j<inheight; j++)
			for(i=0; i<inwidth; i++)
			{
				if(!bgflag||rgb_input[j*inwidth*3+i*3+0]!=HOLER||rgb_input[j*inwidth*3+i*3+1]!=HOLEG||rgb_input[j*inwidth*3+i*3+2]!=HOLEB)
				{
					disparity = alpha*((int)in_depths[j*inwidth+i]-g_renderforcalplane)/255.0; // 
					pos_left = i + disparity;
					pos = (int)floor(pos_left+0.5);
					if(pos<=0) pos=0;
					if(pos>=inwidth-1) pos=inwidth-1;
			
					if(leftlabel_output[j*inwidth+pos]==0)
					{
						leftlabel_output[j*inwidth+pos]=1;
						leftmatch[j*inwidth+pos] = i;
						rgb_output[j*inwidth*6+pos*3+0] = rgb_input[j*inwidth*3+i*3+0];
						rgb_output[j*inwidth*6+pos*3+1] = rgb_input[j*inwidth*3+i*3+1];
						rgb_output[j*inwidth*6+pos*3+2] = rgb_input[j*inwidth*3+i*3+2];
					}
				}
			}
		/* 去除量化噪声*/
		// 水平方向
		for(j=0; j<inheight; j++)
			for(i=inwidth-1; i>=2; i--)
			{
				if(leftlabel_output[j*inwidth+i]==1&&leftlabel_output[j*inwidth+i-1]==0&&leftlabel_output[j*inwidth+i-2]==1)
				{
					leftlabel_output[j*inwidth+i+1]=1;
					leftmatch[j*inwidth+i-1] = leftmatch[j*inwidth+i-2];
					rgb_output[j*inwidth*6+(i-1)*3+0] = rgb_output[j*inwidth*6+(i)*3+0];
					rgb_output[j*inwidth*6+(i-1)*3+1] = rgb_output[j*inwidth*6+(i)*3+1];
					rgb_output[j*inwidth*6+(i-1)*3+2] = rgb_output[j*inwidth*6+(i)*3+2];
				}
			}
		// 垂直方向
		for(j=0; j<inheight-2; j++)
			for(i=inwidth-1; i>=0; i--)
			{
				if(leftlabel_output[j*inwidth+i]==1&&leftlabel_output[(j+1)*inwidth+i]==0&&leftlabel_output[(j+2)*inwidth+i]==1)
				{
					leftlabel_output[(j+1)*inwidth+i]=1;
					leftmatch[(j+1)*inwidth+i] = leftmatch[j*inwidth+i];
					rgb_output[(j+1)*inwidth*6+(i)*3+0] = rgb_output[j*inwidth*6+i*3+0];
					rgb_output[(j+1)*inwidth*6+(i)*3+1] = rgb_output[j*inwidth*6+i*3+1];
					rgb_output[(j+1)*inwidth*6+(i)*3+2] = rgb_output[j*inwidth*6+i*3+2];
				}
			}		
		/* 去除明显错误*/
		for(j=0; j<inheight; j++)
			for(i=1; i<inwidth; i++)
			{
				if(leftmatch[j*inwidth+i]<leftmatch[j*inwidth+i-1]&&leftlabel_output[j*inwidth+i-1]==1&&leftlabel_output[j*inwidth+i]==1)
				{
					leftmatch[j*inwidth+i-1]=leftmatch[j*inwidth+i];
					rgb_output[j*inwidth*6+(i-1)*3+0] = rgb_output[j*inwidth*6+i*3+0];
					rgb_output[j*inwidth*6+(i-1)*3+1] = rgb_output[j*inwidth*6+i*3+1];
					rgb_output[j*inwidth*6+(i-1)*3+2] = rgb_output[j*inwidth*6+i*3+2];
				}
			}
	}

	if(lrflag&0x2) // 右视点
	{
		for(j=0; j<inheight; j++)
			for(i=inwidth-1; i>=0; i--)//for(i=0; i<width; i++)
			{
				if(!bgflag||rgb_input[j*inwidth*3+i*3+0]!=HOLER||rgb_input[j*inwidth*3+i*3+1]!=HOLEG||rgb_input[j*inwidth*3+i*3+2]!=HOLEB)
				{
					disparity = alpha*((int)in_depths[j*inwidth+i]-g_renderforcalplane)/255.0; // 
					pos_right = i - disparity;
					pos = (int)floor(pos_right+0.5);
					if(pos<=0) pos=0;
					if(pos>=inwidth-1) pos=inwidth-1;
			
					if(rightlabel_output[j*inwidth+pos]==0)
					{
						rightlabel_output[j*inwidth+pos]=1;
						rightmatch[j*inwidth+pos] = i;
						rgb_output[j*inwidth*6+inwidth*3+pos*3+0] = rgb_input[j*inwidth*3+i*3+0];
						rgb_output[j*inwidth*6+inwidth*3+pos*3+1] = rgb_input[j*inwidth*3+i*3+1];
						rgb_output[j*inwidth*6+inwidth*3+pos*3+2] = rgb_input[j*inwidth*3+i*3+2];
					}
				}
			}
		/* 去除量化噪声*/
		// 水平方向
		for(j=0; j<inheight; j++)
			for(i=0; i<inwidth-2; i++)
			{
				if(rightlabel_output[j*inwidth+i]==1&&rightlabel_output[j*inwidth+i+1]==0&&rightlabel_output[j*inwidth+i+2]==1)
				{
					rightlabel_output[j*inwidth+i+1]=1;
					rightmatch[j*inwidth+i+1] = rightmatch[j*inwidth+i+2];
					rgb_output[j*inwidth*6+inwidth*3+(i+1)*3+0] = rgb_output[j*inwidth*6+inwidth*3+(i+2)*3+0];
					rgb_output[j*inwidth*6+inwidth*3+(i+1)*3+1] = rgb_output[j*inwidth*6+inwidth*3+(i+2)*3+1];
					rgb_output[j*inwidth*6+inwidth*3+(i+1)*3+2] = rgb_output[j*inwidth*6+inwidth*3+(i+2)*3+2];
				}
			}
		// 垂直方向
		for(j=0; j<inheight-2; j++)
			for(i=0; i<inwidth; i++)
			{
				if(rightlabel_output[j*inwidth+i]==1&&rightlabel_output[(j+1)*inwidth+i]==0&&rightlabel_output[(j+2)*inwidth+i]==1)
				{
					rightlabel_output[(j+1)*inwidth+i]=1;
					rightmatch[(j+1)*inwidth+i] = rightmatch[j*inwidth+i];
					rgb_output[(j+1)*inwidth*6+inwidth*3+(i)*3+0] = rgb_output[j*inwidth*6+inwidth*3+i*3+0];
					rgb_output[(j+1)*inwidth*6+inwidth*3+(i)*3+1] = rgb_output[j*inwidth*6+inwidth*3+i*3+1];
					rgb_output[(j+1)*inwidth*6+inwidth*3+(i)*3+2] = rgb_output[j*inwidth*6+inwidth*3+i*3+2];
				}
			}		
		/* 去除明显错误*/
		for(j=0; j<inheight; j++)
			for(i=inwidth-2; i>=0; i--)//for(i=0; i<width-1; i++)
			{
				if(rightmatch[j*inwidth+i]>rightmatch[j*inwidth+i+1]&&rightlabel_output[j*inwidth+i+1]==1&&rightlabel_output[j*inwidth+i]==1)
				{
					rightmatch[j*inwidth+i+1]=rightmatch[j*inwidth+i];
					rgb_output[j*inwidth*6+inwidth*3+(i+1)*3+0] = rgb_output[j*inwidth*6+inwidth*3+i*3+0];
					rgb_output[j*inwidth*6+inwidth*3+(i+1)*3+1] = rgb_output[j*inwidth*6+inwidth*3+i*3+1];
					rgb_output[j*inwidth*6+inwidth*3+(i+1)*3+2] = rgb_output[j*inwidth*6+inwidth*3+i*3+2];
				}
			}
	}

	if(lrflag&0x1)
	{
		delete []leftlabel_output;
		delete []leftmatch;
	}
	if(lrflag&0x2)
	{
		delete []rightlabel_output;
		delete []rightmatch;
	}
	
	return ;
}

// 视点Render
void RenderViewLayer(uint8_t *rgb_output, uint8_t * rgb_input, uint8_t * in_depths, int inwidth, int inheight, int lrflag, uint8_t * flag)
{
	int i,j;
	unsigned char *leftlabel_output = NULL;
	unsigned char *rightlabel_output = NULL;
	int *leftmatch = NULL;
	int *rightmatch = NULL;
	double disparity, alpha;
	double pos_left, pos_right;
	int pos;

	/* 标记值初始化，初始值设定*/
	if(lrflag&0x1) // 有左视点
	{
		leftlabel_output = new uint8_t[inwidth*inheight];
		memset(leftlabel_output, 0, inwidth*inheight);
		leftmatch = new int[inwidth*inheight];
		for(j=0; j<inheight; j++)
			for(i=0; i<inwidth; i++)
			{
				leftmatch[i+j*inwidth] = 0;
			}
	}
	if(lrflag&0x2) // 有右视点
	{
		rightlabel_output = new uint8_t[inwidth*inheight];
		memset(rightlabel_output, 0, inwidth*inheight);
		rightmatch = new int[inwidth*inheight];
		for(j=0; j<inheight; j++)
			for(i=0; i<inwidth; i++)
			{
				rightmatch[i+j*inwidth] = 0;
			}
	}
	
	/* 像素移位*/
	alpha = 0.01*inwidth*0.20*g_renderalpha;

	if(lrflag&0x1) // 左视点
	{
		for(j=0; j<inheight; j++)
			for(i=0; i<inwidth; i++)
			{
				if(flag[i+j*inwidth])
				{
					disparity = alpha*((int)in_depths[j*inwidth+i]-g_renderforcalplane)/255.0; // 
					pos_left = i + disparity;
					pos = (int)floor(pos_left+0.5);
					if(pos<=0) pos=0;
					if(pos>=inwidth-1) pos=inwidth-1;
			
					if(leftlabel_output[j*inwidth+pos]==0)
					{
						leftlabel_output[j*inwidth+pos]=1;
						leftmatch[j*inwidth+pos] = i;
						rgb_output[j*inwidth*6+pos*3+0] = rgb_input[j*inwidth*3+i*3+0];
						rgb_output[j*inwidth*6+pos*3+1] = rgb_input[j*inwidth*3+i*3+1];
						rgb_output[j*inwidth*6+pos*3+2] = rgb_input[j*inwidth*3+i*3+2];
					}
				}
			}
		/* 去除量化噪声*/
		// 水平方向
		for(j=0; j<inheight; j++)
			for(i=inwidth-1; i>=2; i--)
			{
				if(leftlabel_output[j*inwidth+i]==1&&leftlabel_output[j*inwidth+i-1]==0&&leftlabel_output[j*inwidth+i-2]==1)
				{
					leftlabel_output[j*inwidth+i+1]=1;
					leftmatch[j*inwidth+i-1] = leftmatch[j*inwidth+i-2];
					rgb_output[j*inwidth*6+(i-1)*3+0] = rgb_output[j*inwidth*6+(i)*3+0];
					rgb_output[j*inwidth*6+(i-1)*3+1] = rgb_output[j*inwidth*6+(i)*3+1];
					rgb_output[j*inwidth*6+(i-1)*3+2] = rgb_output[j*inwidth*6+(i)*3+2];
				}
			}
		// 垂直方向
		for(j=0; j<inheight-2; j++)
			for(i=inwidth-1; i>=0; i--)
			{
				if(leftlabel_output[j*inwidth+i]==1&&leftlabel_output[(j+1)*inwidth+i]==0&&leftlabel_output[(j+2)*inwidth+i]==1)
				{
					leftlabel_output[(j+1)*inwidth+i]=1;
					leftmatch[(j+1)*inwidth+i] = leftmatch[j*inwidth+i];
					rgb_output[(j+1)*inwidth*6+(i)*3+0] = rgb_output[j*inwidth*6+i*3+0];
					rgb_output[(j+1)*inwidth*6+(i)*3+1] = rgb_output[j*inwidth*6+i*3+1];
					rgb_output[(j+1)*inwidth*6+(i)*3+2] = rgb_output[j*inwidth*6+i*3+2];
				}
			}		
		/* 去除明显错误*/
		for(j=0; j<inheight; j++)
			for(i=1; i<inwidth; i++)
			{
				if(leftmatch[j*inwidth+i]<leftmatch[j*inwidth+i-1]&&leftlabel_output[j*inwidth+i-1]==1&&leftlabel_output[j*inwidth+i]==1)
				{
					leftmatch[j*inwidth+i-1]=leftmatch[j*inwidth+i];
					rgb_output[j*inwidth*6+(i-1)*3+0] = rgb_output[j*inwidth*6+i*3+0];
					rgb_output[j*inwidth*6+(i-1)*3+1] = rgb_output[j*inwidth*6+i*3+1];
					rgb_output[j*inwidth*6+(i-1)*3+2] = rgb_output[j*inwidth*6+i*3+2];
				}
			}
	}

	if(lrflag&0x2) // 右视点
	{
		for(j=0; j<inheight; j++)
			for(i=inwidth-1; i>=0; i--)//for(i=0; i<width; i++)
			{
				if(flag[i+j*inwidth])
				{
					disparity = alpha*((int)in_depths[j*inwidth+i]-g_renderforcalplane)/255.0; // 
					pos_right = i - disparity;
					pos = (int)floor(pos_right+0.5);
					if(pos<=0) pos=0;
					if(pos>=inwidth-1) pos=inwidth-1;
			
					if(rightlabel_output[j*inwidth+pos]==0)
					{
						rightlabel_output[j*inwidth+pos]=1;
						rightmatch[j*inwidth+pos] = i;
						rgb_output[j*inwidth*6+inwidth*3+pos*3+0] = rgb_input[j*inwidth*3+i*3+0];
						rgb_output[j*inwidth*6+inwidth*3+pos*3+1] = rgb_input[j*inwidth*3+i*3+1];
						rgb_output[j*inwidth*6+inwidth*3+pos*3+2] = rgb_input[j*inwidth*3+i*3+2];
					}
				}
			}
		/* 去除量化噪声*/
		// 水平方向
		for(j=0; j<inheight; j++)
			for(i=0; i<inwidth-2; i++)
			{
				if(rightlabel_output[j*inwidth+i]==1&&rightlabel_output[j*inwidth+i+1]==0&&rightlabel_output[j*inwidth+i+2]==1)
				{
					rightlabel_output[j*inwidth+i+1]=1;
					rightmatch[j*inwidth+i+1] = rightmatch[j*inwidth+i+2];
					rgb_output[j*inwidth*6+inwidth*3+(i+1)*3+0] = rgb_output[j*inwidth*6+inwidth*3+(i+2)*3+0];
					rgb_output[j*inwidth*6+inwidth*3+(i+1)*3+1] = rgb_output[j*inwidth*6+inwidth*3+(i+2)*3+1];
					rgb_output[j*inwidth*6+inwidth*3+(i+1)*3+2] = rgb_output[j*inwidth*6+inwidth*3+(i+2)*3+2];
				}
			}
		// 垂直方向
		for(j=0; j<inheight-2; j++)
			for(i=0; i<inwidth; i++)
			{
				if(rightlabel_output[j*inwidth+i]==1&&rightlabel_output[(j+1)*inwidth+i]==0&&rightlabel_output[(j+2)*inwidth+i]==1)
				{
					rightlabel_output[(j+1)*inwidth+i]=1;
					rightmatch[(j+1)*inwidth+i] = rightmatch[j*inwidth+i];
					rgb_output[(j+1)*inwidth*6+inwidth*3+(i)*3+0] = rgb_output[j*inwidth*6+inwidth*3+i*3+0];
					rgb_output[(j+1)*inwidth*6+inwidth*3+(i)*3+1] = rgb_output[j*inwidth*6+inwidth*3+i*3+1];
					rgb_output[(j+1)*inwidth*6+inwidth*3+(i)*3+2] = rgb_output[j*inwidth*6+inwidth*3+i*3+2];
				}
			}		
		/* 去除明显错误*/
		for(j=0; j<inheight; j++)
			for(i=inwidth-2; i>=0; i--)//for(i=0; i<width-1; i++)
			{
				if(rightmatch[j*inwidth+i]>rightmatch[j*inwidth+i+1]&&rightlabel_output[j*inwidth+i+1]==1&&rightlabel_output[j*inwidth+i]==1)
				{
					rightmatch[j*inwidth+i+1]=rightmatch[j*inwidth+i];
					rgb_output[j*inwidth*6+inwidth*3+(i+1)*3+0] = rgb_output[j*inwidth*6+inwidth*3+i*3+0];
					rgb_output[j*inwidth*6+inwidth*3+(i+1)*3+1] = rgb_output[j*inwidth*6+inwidth*3+i*3+1];
					rgb_output[j*inwidth*6+inwidth*3+(i+1)*3+2] = rgb_output[j*inwidth*6+inwidth*3+i*3+2];
				}
			}
	}

	if(lrflag&0x1)
	{
		delete []leftlabel_output;
		delete []leftmatch;
	}
	if(lrflag&0x2)
	{
		delete []rightlabel_output;
		delete []rightmatch;
	}
	
	return ;
}


/* 其他层次render */
void LayerRender(uint8_t * rgb_output)
{
	int i,j;
	int base = g_frames[g_frameIndex].BackgroundFrame;
	uint8_t * layerflag = NULL;

	if(base<=0)
	{
		RenderView(rgb_output, g_rgborginput, g_SegDepths0, g_frameInWidth, g_frameInHeight, 3, 0);
		return ;
	}

	layerflag = new uint8_t[g_frameInWidth*g_frameInHeight];
	//memset(layerflag, 0, g_frameInWidth*g_frameInHeight);

	for(j=0;j<g_frameInHeight;j++)
		for(i=0;i<g_frameInWidth;i++)
		{
			if((g_frames[g_frameIndex].SegLabels0[i+j*g_frameInWidth]>g_frameWidth*g_frameHeight)&&((1<<g_frames[g_frameIndex].LayerBaseNum)&(g_frames[g_frameIndex].SegLabels0[i+j*g_frameInWidth]-g_frameWidth*g_frameHeight)))
			{
				layerflag[i+j*g_frameInWidth] = 0;
			}
			else
			{
				layerflag[i+j*g_frameInWidth] = 1;
			}
		}

	RenderViewLayer(rgb_output, g_rgborginput, g_SegDepths0, g_frameInWidth, g_frameInHeight, 3, layerflag);
	
	delete []layerflag;
	
	return ;
}

/* 背景层次render */
void BackgroundRender(uint8_t * rgb_output)
{
	int i,j;
	char filename[256];
	char filenamed[256];
	IplImage* pImg, *pImgd;
	int base = g_frames[g_frameIndex].BackgroundFrame;

	if(base<=0)
		return ;

	// 背景信息载入
	sprintf(filename,"./%s/background/%d_backgroundsnap_%d.png",cfgparams.projectname,g_sectioncur,base);
	sprintf(filenamed,"./%s/background/%d_backgrounddepthsnap_%d.png",cfgparams.projectname,g_sectioncur,base);

	if(_access(filename,0))
	{
		printf("RENDER   INFO --- Depth Rendering frame index %d no background %d\n", g_frameIndex+1, base);
		return ;
	}
	else
	{
		pImg = cvLoadImage(filename);
	}
	
	if(_access(filenamed,0))
	{
		printf("RENDER   INFO --- Depth Rendering frame index %d no background depth %d\n", g_frameIndex+1, base);
		return ;
	}
	else
	{
		pImgd = cvLoadImage(filenamed);
	}
	
	if(pImg->width!=g_frameInWidth||pImgd->width!=g_frameInWidth||pImg->height!=g_frameInHeight||pImgd->height!=g_frameInHeight)
	{
		printf("RENDER   INFO --- Depth Rendering frame index %d is not match with background %d\n", g_frameIndex+1, base);
		return ;
	}
	
	for(j=0;j<g_frameInHeight;j++)
		for(i=0;i<g_frameInWidth;i++)
		{
			g_rgborginputref[i*3+0+j*g_frameInWidth*3] = pImg->imageData[i*3+2+j*pImg->widthStep];
			g_rgborginputref[i*3+1+j*g_frameInWidth*3] = pImg->imageData[i*3+1+j*pImg->widthStep];
			g_rgborginputref[i*3+2+j*g_frameInWidth*3] = pImg->imageData[i*3+0+j*pImg->widthStep];
			g_RefSegDepths0[i+j*g_frameInWidth] = pImgd->imageData[i+j*pImgd->widthStep];
		}
	
	cvReleaseImage( &pImg );
	cvReleaseImage( &pImgd );

	// 背景render
	RenderView(rgb_output, g_rgborginputref, g_RefSegDepths0, g_frameInWidth, g_frameInHeight, 3, 1);
	
	return ;
}

void InterFillRender(uint8_t * rgb_output, uint8_t * rgb_input)
{

	int i,j,k;
	int inwidth = g_frameInWidth;
	int inheight = g_frameInHeight;
	uint8_t * layerflag = NULL;
	int rightoffset = g_frameInWidth*3;
	
	layerflag = new uint8_t[g_frameInWidth*g_frameInHeight];
	
	//for(k=0;k<=1;k++)
	{
		k = 0;
		// 获取标志
		for(j=0;j<g_frameInHeight;j++)
			for(i=0;i<g_frameInWidth;i++)
			{
				if(rgb_output[i*3+0+k*rightoffset+j*g_frameInWidth*6]==HOLER&&rgb_output[i*3+1+k*rightoffset+j*g_frameInWidth*6]==HOLEG&&rgb_output[i*3+2+k*rightoffset+j*g_frameInWidth*6]==HOLEB)
				{
					layerflag[i+j*g_frameInWidth] = 0;
				}
				else
				{
					layerflag[i+j*g_frameInWidth] = 1;
				}
					
			}
		for(j=0; j<inheight; j++)
		{
			if(layerflag[j*inwidth+0]==0)
			{
				layerflag[j*inwidth+0]=1;
				rgb_output[j*inwidth*6+k*rightoffset+0+0]
				   = rgb_input[j*inwidth*3+0+0];
				rgb_output[j*inwidth*6+k*rightoffset+0+1] 
				   = rgb_input[j*inwidth*3+0+1];
				rgb_output[j*inwidth*6+k*rightoffset+0+2] 
				   = rgb_input[j*inwidth*3+0+2];
			}
			if(layerflag[j*inwidth+1]==0)
			{
				layerflag[j*inwidth+1]=1;
				rgb_output[j*inwidth*6+k*rightoffset+(1)*3+0]
				   = rgb_output[j*inwidth*6+k*rightoffset+(0)*3+0];
				rgb_output[j*inwidth*6+k*rightoffset+(1)*3+1] 
				   = rgb_output[j*inwidth*6+k*rightoffset+(0)*3+1];
				rgb_output[j*inwidth*6+k*rightoffset+(1)*3+2] 
				   = rgb_output[j*inwidth*6+k*rightoffset+(0)*3+2];
			}
			if(layerflag[j*inwidth+2]==0)
			{
				layerflag[j*inwidth+2]=1;
				rgb_output[j*inwidth*6+k*rightoffset+(2)*3+0]
				   = rgb_output[j*inwidth*6+k*rightoffset+(1)*3+0];
				rgb_output[j*inwidth*6+k*rightoffset+(2)*3+1] 
				   = rgb_output[j*inwidth*6+k*rightoffset+(1)*3+1];
				rgb_output[j*inwidth*6+k*rightoffset+(2)*3+2] 
				   = rgb_output[j*inwidth*6+k*rightoffset+(1)*3+2];
			}
		}
		for(j=0; j<inheight; j++)
			for(i=2; i<inwidth-1; i++)
			{
				if(layerflag[j*inwidth+i+1]==0&&layerflag[j*inwidth+i]==1&&layerflag[j*inwidth+i-1]==1&&layerflag[j*inwidth+i-2]==1)
				{
					layerflag[j*inwidth+i+1]=1;
					rgb_output[j*inwidth*6+k*rightoffset+(i+1)*3+0]
					   = rgb_output[j*inwidth*6+k*rightoffset+(i)*3+0]
					   = rgb_output[j*inwidth*6+k*rightoffset+(i-1)*3+0]
					   = rgb_output[j*inwidth*6+k*rightoffset+(i-2)*3+0];
					rgb_output[j*inwidth*6+k*rightoffset+(i+1)*3+1]
					   = rgb_output[j*inwidth*6+k*rightoffset+(i)*3+1]
					   = rgb_output[j*inwidth*6+k*rightoffset+(i-1)*3+1]
					   = rgb_output[j*inwidth*6+k*rightoffset+(i-2)*3+1];
					rgb_output[j*inwidth*6+k*rightoffset+(i+1)*3+2]
					   = rgb_output[j*inwidth*6+k*rightoffset+(i)*3+2]
					   = rgb_output[j*inwidth*6+k*rightoffset+(i-1)*3+2]
					   = rgb_output[j*inwidth*6+k*rightoffset+(i-2)*3+2];
				}
			}

		k = 1;
		// 获取标志
		for(j=0;j<g_frameInHeight;j++)
			for(i=0;i<g_frameInWidth;i++)
			{
				if(rgb_output[i*3+0+k*rightoffset+j*g_frameInWidth*6]==HOLER&&rgb_output[i*3+1+k*rightoffset+j*g_frameInWidth*6]==HOLEG&&rgb_output[i*3+2+k*rightoffset+j*g_frameInWidth*6]==HOLEB)
				{
					layerflag[i+j*g_frameInWidth] = 0;
				}
				else
				{
					layerflag[i+j*g_frameInWidth] = 1;
				}
					
			}
		for(j=0; j<inheight; j++)
		{
			if(layerflag[j*inwidth+inwidth-1]==0)
			{
				layerflag[j*inwidth+inwidth-1]=1;
				rgb_output[j*inwidth*6+k*rightoffset+(inwidth-1)*3+0]
				   = rgb_input[j*inwidth*3+(inwidth-1)*3+0];
				rgb_output[j*inwidth*6+k*rightoffset+(inwidth-1)*3+1] 
				   = rgb_input[j*inwidth*3+(inwidth-1)*3+1];
				rgb_output[j*inwidth*6+k*rightoffset+(inwidth-1)*3+2] 
				   = rgb_input[j*inwidth*3+(inwidth-1)*3+2];
			}
			if(layerflag[j*inwidth+inwidth-2]==0)
			{
				layerflag[j*inwidth+inwidth-2]=1;
				rgb_output[j*inwidth*6+k*rightoffset+(inwidth-2)*3+0]
				   = rgb_output[j*inwidth*6+k*rightoffset+(inwidth-1)*3+0];
				rgb_output[j*inwidth*6+k*rightoffset+(inwidth-2)*3+1] 
				   = rgb_output[j*inwidth*6+k*rightoffset+(inwidth-1)*3+1];
				rgb_output[j*inwidth*6+k*rightoffset+(inwidth-2)*3+2] 
				   = rgb_output[j*inwidth*6+k*rightoffset+(inwidth-1)*3+2];
			}
			if(layerflag[j*inwidth+inwidth-3]==0)
			{
				layerflag[j*inwidth+inwidth-3]=1;
				rgb_output[j*inwidth*6+k*rightoffset+(inwidth-3)*3+0]
				   = rgb_output[j*inwidth*6+k*rightoffset+(inwidth-2)*3+0];
				rgb_output[j*inwidth*6+k*rightoffset+(inwidth-3)*3+1] 
				   = rgb_output[j*inwidth*6+k*rightoffset+(inwidth-2)*3+1];
				rgb_output[j*inwidth*6+k*rightoffset+(inwidth-3)*3+2] 
				   = rgb_output[j*inwidth*6+k*rightoffset+(inwidth-2)*3+2];
			}
		}
		for(j=0; j<inheight; j++)
			for(i=inwidth-3; i>0; i--)
			{
				if(layerflag[j*inwidth+i-1]==0&&layerflag[j*inwidth+i]==1&&layerflag[j*inwidth+i+1]==1&&layerflag[j*inwidth+i+2]==1)
				{
					layerflag[j*inwidth+i-1]=1;
					rgb_output[j*inwidth*6+k*rightoffset+(i-1)*3+0]
					   = rgb_output[j*inwidth*6+k*rightoffset+(i)*3+0]
					   = rgb_output[j*inwidth*6+k*rightoffset+(i+1)*3+0]
					   = rgb_output[j*inwidth*6+k*rightoffset+(i+2)*3+0];
					rgb_output[j*inwidth*6+k*rightoffset+(i-1)*3+1]
					   = rgb_output[j*inwidth*6+k*rightoffset+(i)*3+1]
					   = rgb_output[j*inwidth*6+k*rightoffset+(i+1)*3+1]
					   = rgb_output[j*inwidth*6+k*rightoffset+(i+2)*3+1];
					rgb_output[j*inwidth*6+k*rightoffset+(i-1)*3+2]
					   = rgb_output[j*inwidth*6+k*rightoffset+(i)*3+2]
					   = rgb_output[j*inwidth*6+k*rightoffset+(i+1)*3+2]
					   = rgb_output[j*inwidth*6+k*rightoffset+(i+2)*3+2];
				}
			}
	
	}

	
	return ;
}


void InpaintFillRender(uint8_t * rgb_output, uint8_t * rgb_input)
{

	int i,j,k;
	int inwidth = g_frameInWidth;
	int inheight = g_frameInHeight;
	uint8_t * layerflag = NULL;
	int rightoffset = g_frameInWidth*3;
	
	layerflag = new uint8_t[g_frameInWidth*g_frameInHeight];

	int inpaintRadius = 3;
	IplImage* pImgin;
	IplImage* pImgmask;
	IplImage* pImgout;
	CvSize sz;
	int flags = CV_INPAINT_TELEA;
	sz.width = inwidth;
	sz.height = inheight;
	pImgin = cvCreateImage(sz, IPL_DEPTH_8U, 3);
	pImgmask = cvCreateImage(sz, IPL_DEPTH_8U, 1);
	pImgout = cvCreateImage(sz, IPL_DEPTH_8U, 3);

	//for(k=0;k<=1;k++)
	{
		k = 0;
		// 获取标志
		for(j=0;j<g_frameInHeight;j++)
			for(i=0;i<g_frameInWidth;i++)
			{
				if(rgb_output[i*3+0+k*rightoffset+j*g_frameInWidth*6]==HOLER&&rgb_output[i*3+1+k*rightoffset+j*g_frameInWidth*6]==HOLEG&&rgb_output[i*3+2+k*rightoffset+j*g_frameInWidth*6]==HOLEB)
				{
					layerflag[i+j*g_frameInWidth] = 0;
				}
				else
				{
					layerflag[i+j*g_frameInWidth] = 1;
				}
					
			}
		for(j=0; j<inheight; j++)
		{
			if(layerflag[j*inwidth+0]==0)
			{
				layerflag[j*inwidth+0]=1;
				rgb_output[j*inwidth*6+k*rightoffset+0+0]
				   = rgb_input[j*inwidth*3+0+0];
				rgb_output[j*inwidth*6+k*rightoffset+0+1] 
				   = rgb_input[j*inwidth*3+0+1];
				rgb_output[j*inwidth*6+k*rightoffset+0+2] 
				   = rgb_input[j*inwidth*3+0+2];
			}
			if(layerflag[j*inwidth+1]==0)
			{
				layerflag[j*inwidth+1]=1;
				rgb_output[j*inwidth*6+k*rightoffset+(1)*3+0]
				   = rgb_output[j*inwidth*6+k*rightoffset+(0)*3+0];
				rgb_output[j*inwidth*6+k*rightoffset+(1)*3+1] 
				   = rgb_output[j*inwidth*6+k*rightoffset+(0)*3+1];
				rgb_output[j*inwidth*6+k*rightoffset+(1)*3+2] 
				   = rgb_output[j*inwidth*6+k*rightoffset+(0)*3+2];
			}
			if(layerflag[j*inwidth+2]==0)
			{
				layerflag[j*inwidth+2]=1;
				rgb_output[j*inwidth*6+k*rightoffset+(2)*3+0]
				   = rgb_output[j*inwidth*6+k*rightoffset+(1)*3+0];
				rgb_output[j*inwidth*6+k*rightoffset+(2)*3+1] 
				   = rgb_output[j*inwidth*6+k*rightoffset+(1)*3+1];
				rgb_output[j*inwidth*6+k*rightoffset+(2)*3+2] 
				   = rgb_output[j*inwidth*6+k*rightoffset+(1)*3+2];
			}
		}
		for(j=0; j<inheight; j++)
			for(i=2; i<inwidth-1; i++)
			{
				if(layerflag[j*inwidth+i+1]==0&&layerflag[j*inwidth+i]==1&&layerflag[j*inwidth+i-1]==1&&layerflag[j*inwidth+i-2]==1)
				{
					layerflag[j*inwidth+i+1]=1;
					rgb_output[j*inwidth*6+k*rightoffset+(i+1)*3+0]
					   = rgb_output[j*inwidth*6+k*rightoffset+(i)*3+0]
					   = rgb_output[j*inwidth*6+k*rightoffset+(i-1)*3+0]
					   = rgb_output[j*inwidth*6+k*rightoffset+(i-2)*3+0];
					rgb_output[j*inwidth*6+k*rightoffset+(i+1)*3+1]
					   = rgb_output[j*inwidth*6+k*rightoffset+(i)*3+1]
					   = rgb_output[j*inwidth*6+k*rightoffset+(i-1)*3+1]
					   = rgb_output[j*inwidth*6+k*rightoffset+(i-2)*3+1];
					rgb_output[j*inwidth*6+k*rightoffset+(i+1)*3+2]
					   = rgb_output[j*inwidth*6+k*rightoffset+(i)*3+2]
					   = rgb_output[j*inwidth*6+k*rightoffset+(i-1)*3+2]
					   = rgb_output[j*inwidth*6+k*rightoffset+(i-2)*3+2];
				}
			}

		for(j=0;j<inheight;j++)
			for(i=0;i<inwidth;i++)
			{
				pImgin->imageData[j*pImgin->widthStep+i*3+2] = rgb_output[j*inwidth*6+k*rightoffset+(i)*3+0]; 
				pImgin->imageData[j*pImgin->widthStep+i*3+1] = rgb_output[j*inwidth*6+k*rightoffset+(i)*3+1]; 
				pImgin->imageData[j*pImgin->widthStep+i*3+0] = rgb_output[j*inwidth*6+k*rightoffset+(i)*3+2]; 
			}
			
		cvZero(pImgmask);
		cvZero(pImgout);
		for(j=0;j<inheight;j++)
			for(i=0;i<inwidth;i++)
			{
				if(layerflag[j*inwidth+i]==0)
					pImgmask->imageData[j*pImgmask->widthStep+i]=(char)255;
			}
		
		cvInpaint(pImgin, pImgmask, pImgout, inpaintRadius, flags);
		
		for(j=0;j<inheight;j++)
			for(i=0;i<inwidth;i++)
			{
				rgb_output[j*inwidth*6+k*rightoffset+(i)*3+0] = pImgout->imageData[j*pImgout->widthStep+i*3+2]; 
				rgb_output[j*inwidth*6+k*rightoffset+(i)*3+1] = pImgout->imageData[j*pImgout->widthStep+i*3+1]; 
				rgb_output[j*inwidth*6+k*rightoffset+(i)*3+2] = pImgout->imageData[j*pImgout->widthStep+i*3+0]; 
			}

		k = 1;
		// 获取标志
		for(j=0;j<g_frameInHeight;j++)
			for(i=0;i<g_frameInWidth;i++)
			{
				if(rgb_output[i*3+0+k*rightoffset+j*g_frameInWidth*6]==HOLER&&rgb_output[i*3+1+k*rightoffset+j*g_frameInWidth*6]==HOLEG&&rgb_output[i*3+2+k*rightoffset+j*g_frameInWidth*6]==HOLEB)
				{
					layerflag[i+j*g_frameInWidth] = 0;
				}
				else
				{
					layerflag[i+j*g_frameInWidth] = 1;
				}
					
			}
		for(j=0; j<inheight; j++)
		{
			if(layerflag[j*inwidth+inwidth-1]==0)
			{
				layerflag[j*inwidth+inwidth-1]=1;
				rgb_output[j*inwidth*6+k*rightoffset+(inwidth-1)*3+0]
				   = rgb_input[j*inwidth*3+(inwidth-1)*3+0];
				rgb_output[j*inwidth*6+k*rightoffset+(inwidth-1)*3+1] 
				   = rgb_input[j*inwidth*3+(inwidth-1)*3+1];
				rgb_output[j*inwidth*6+k*rightoffset+(inwidth-1)*3+2] 
				   = rgb_input[j*inwidth*3+(inwidth-1)*3+2];
			}
			if(layerflag[j*inwidth+inwidth-2]==0)
			{
				layerflag[j*inwidth+inwidth-2]=1;
				rgb_output[j*inwidth*6+k*rightoffset+(inwidth-2)*3+0]
				   = rgb_output[j*inwidth*6+k*rightoffset+(inwidth-1)*3+0];
				rgb_output[j*inwidth*6+k*rightoffset+(inwidth-2)*3+1] 
				   = rgb_output[j*inwidth*6+k*rightoffset+(inwidth-1)*3+1];
				rgb_output[j*inwidth*6+k*rightoffset+(inwidth-2)*3+2] 
				   = rgb_output[j*inwidth*6+k*rightoffset+(inwidth-1)*3+2];
			}
			if(layerflag[j*inwidth+inwidth-3]==0)
			{
				layerflag[j*inwidth+inwidth-3]=1;
				rgb_output[j*inwidth*6+k*rightoffset+(inwidth-3)*3+0]
				   = rgb_output[j*inwidth*6+k*rightoffset+(inwidth-2)*3+0];
				rgb_output[j*inwidth*6+k*rightoffset+(inwidth-3)*3+1] 
				   = rgb_output[j*inwidth*6+k*rightoffset+(inwidth-2)*3+1];
				rgb_output[j*inwidth*6+k*rightoffset+(inwidth-3)*3+2] 
				   = rgb_output[j*inwidth*6+k*rightoffset+(inwidth-2)*3+2];
			}
		}
		for(j=0; j<inheight; j++)
			for(i=inwidth-3; i>0; i--)
			{
				if(layerflag[j*inwidth+i-1]==0&&layerflag[j*inwidth+i]==1&&layerflag[j*inwidth+i+1]==1&&layerflag[j*inwidth+i+2]==1)
				{
					layerflag[j*inwidth+i-1]=1;
					rgb_output[j*inwidth*6+k*rightoffset+(i-1)*3+0]
					   = rgb_output[j*inwidth*6+k*rightoffset+(i)*3+0]
					   = rgb_output[j*inwidth*6+k*rightoffset+(i+1)*3+0]
					   = rgb_output[j*inwidth*6+k*rightoffset+(i+2)*3+0];
					rgb_output[j*inwidth*6+k*rightoffset+(i-1)*3+1]
					   = rgb_output[j*inwidth*6+k*rightoffset+(i)*3+1]
					   = rgb_output[j*inwidth*6+k*rightoffset+(i+1)*3+1]
					   = rgb_output[j*inwidth*6+k*rightoffset+(i+2)*3+1];
					rgb_output[j*inwidth*6+k*rightoffset+(i-1)*3+2]
					   = rgb_output[j*inwidth*6+k*rightoffset+(i)*3+2]
					   = rgb_output[j*inwidth*6+k*rightoffset+(i+1)*3+2]
					   = rgb_output[j*inwidth*6+k*rightoffset+(i+2)*3+2];
				}
			}

		for(j=0;j<inheight;j++)
			for(i=0;i<inwidth;i++)
			{
				pImgin->imageData[j*pImgin->widthStep+i*3+2] = rgb_output[j*inwidth*6+k*rightoffset+(i)*3+0]; 
				pImgin->imageData[j*pImgin->widthStep+i*3+1] = rgb_output[j*inwidth*6+k*rightoffset+(i)*3+1]; 
				pImgin->imageData[j*pImgin->widthStep+i*3+0] = rgb_output[j*inwidth*6+k*rightoffset+(i)*3+2]; 
			}
			
		cvZero(pImgmask);
		cvZero(pImgout);
		for(j=0;j<inheight;j++)
			for(i=0;i<inwidth;i++)
			{
				if(layerflag[j*inwidth+i]==0)
					pImgmask->imageData[j*pImgmask->widthStep+i]=(char)255;
			}
		
		cvInpaint(pImgin, pImgmask, pImgout, inpaintRadius, flags);
		
		for(j=0;j<inheight;j++)
			for(i=0;i<inwidth;i++)
			{
				rgb_output[j*inwidth*6+k*rightoffset+(i)*3+0] = pImgout->imageData[j*pImgout->widthStep+i*3+2]; 
				rgb_output[j*inwidth*6+k*rightoffset+(i)*3+1] = pImgout->imageData[j*pImgout->widthStep+i*3+1]; 
				rgb_output[j*inwidth*6+k*rightoffset+(i)*3+2] = pImgout->imageData[j*pImgout->widthStep+i*3+0]; 
			}
	
	}

	cvReleaseImage( &pImgin );
	cvReleaseImage( &pImgmask );
	cvReleaseImage( &pImgout );
	
	return ;
}

#define HHFEDGE 2 // 加两个像素宽度的边，方便进行高斯滤波

int HHFAddEdge(uint8_t * rgb_output, int inwidth, uint8_t * orgrgbedge, uint8_t * holeflag, int width, int wedge, int height, int hedge)
{
	int i,j;
	int havehole = 0;

	// 左上角
	for(j=0;j<hedge;j++)
		for(i=0;i<wedge;i++)
		{
			orgrgbedge[i*3+0+j*(width+2*wedge)*3] = rgb_output[0];
			orgrgbedge[i*3+1+j*(width+2*wedge)*3] = rgb_output[1];
			orgrgbedge[i*3+2+j*(width+2*wedge)*3] = rgb_output[2];
			if(rgb_output[0]==HOLER&&rgb_output[1]==HOLEG&&rgb_output[2]==HOLEB)
			{
				holeflag[i+j*(width+2*wedge)] = 0;
			}
			else
			{
				holeflag[i+j*(width+2*wedge)] = 1;
			}
		}
	// 右上角
	for(j=0;j<hedge;j++)
		for(i=0;i<wedge;i++)
		{
			orgrgbedge[(wedge+width+i)*3+0+j*(width+2*wedge)*3] = rgb_output[(width-1)*3+0];
			orgrgbedge[(wedge+width+i)*3+1+j*(width+2*wedge)*3] = rgb_output[(width-1)*3+1];
			orgrgbedge[(wedge+width+i)*3+2+j*(width+2*wedge)*3] = rgb_output[(width-1)*3+2];
			if(rgb_output[(width-1)*3+0]==HOLER&&rgb_output[(width-1)*3+1]==HOLEG&&rgb_output[(width-1)*3+2]==HOLEB)
			{
				holeflag[wedge+width+i+j*(width+2*wedge)] = 0;
			}
			else
			{
				holeflag[wedge+width+i+j*(width+2*wedge)] = 1;
			}
		}
	// 左下角
	for(j=0;j<hedge;j++)
		for(i=0;i<wedge;i++)
		{
			orgrgbedge[i*3+0+(j+height+hedge)*(width+2*wedge)*3] = rgb_output[(height-1)*inwidth+0];
			orgrgbedge[i*3+1+(j+height+hedge)*(width+2*wedge)*3] = rgb_output[(height-1)*inwidth+1];
			orgrgbedge[i*3+2+(j+height+hedge)*(width+2*wedge)*3] = rgb_output[(height-1)*inwidth+2];
			if(rgb_output[(height-1)*inwidth+0]==HOLER&&rgb_output[(height-1)*inwidth+1]==HOLEG&&rgb_output[(height-1)*inwidth+2]==HOLEB)
			{
				holeflag[i+(j+height+hedge)*(width+2*wedge)] = 0;
			}
			else
			{
				holeflag[i+(j+height+hedge)*(width+2*wedge)] = 1;
			}
		}
	// 右下角
	for(j=0;j<hedge;j++)
		for(i=0;i<wedge;i++)
		{
			orgrgbedge[(wedge+width+i)*3+0+(j+height+hedge)*(width+2*wedge)*3] = rgb_output[(width-1)*3+(height-1)*inwidth+0];
			orgrgbedge[(wedge+width+i)*3+1+(j+height+hedge)*(width+2*wedge)*3] = rgb_output[(width-1)*3+(height-1)*inwidth+1];
			orgrgbedge[(wedge+width+i)*3+2+(j+height+hedge)*(width+2*wedge)*3] = rgb_output[(width-1)*3+(height-1)*inwidth+2];
			if(rgb_output[(width-1)*3+(height-1)*inwidth+0]==HOLER&&rgb_output[(width-1)*3+(height-1)*inwidth+1]==HOLEG&&rgb_output[(width-1)*3+(height-1)*inwidth+2]==HOLEB)
			{
				holeflag[wedge+width+i+(j+height+hedge)*(width+2*wedge)] = 0;
			}
			else
			{
				holeflag[wedge+width+i+(j+height+hedge)*(width+2*wedge)] = 1;
			}
		}

	// 上边
	for(j=0;j<hedge;j++)
		for(i=0;i<width;i++)
		{
			orgrgbedge[(wedge+i)*3+0+j*(width+2*wedge)*3] = rgb_output[i*3+0];
			orgrgbedge[(wedge+i)*3+1+j*(width+2*wedge)*3] = rgb_output[i*3+1];
			orgrgbedge[(wedge+i)*3+2+j*(width+2*wedge)*3] = rgb_output[i*3+2];
			if(rgb_output[i*3+0]==HOLER&&rgb_output[i*3+1]==HOLEG&&rgb_output[i*3+2]==HOLEB)
			{
				holeflag[i+wedge+j*(width+2*wedge)] = 0;
			}
			else
			{
				holeflag[i+wedge+j*(width+2*wedge)] = 1;
			}
		}
	// 下边
	for(j=0;j<hedge;j++)
		for(i=0;i<width;i++)
		{
			orgrgbedge[(wedge+i)*3+0+(j+height+hedge)*(width+2*wedge)*3] = rgb_output[i*3+(height-1)*inwidth+0];
			orgrgbedge[(wedge+i)*3+1+(j+height+hedge)*(width+2*wedge)*3] = rgb_output[i*3+(height-1)*inwidth+1];
			orgrgbedge[(wedge+i)*3+2+(j+height+hedge)*(width+2*wedge)*3] = rgb_output[i*3+(height-1)*inwidth+2];
			if(rgb_output[i*3+(height-1)*inwidth+0]==HOLER&&rgb_output[i*3+(height-1)*inwidth+1]==HOLEG&&rgb_output[i*3+(height-1)*inwidth+2]==HOLEB)
			{
				holeflag[i+wedge+(j+height+hedge)*(width+2*wedge)] = 0;
			}
			else
			{
				holeflag[i+wedge+(j+height+hedge)*(width+2*wedge)] = 1;
			}
		}
	// 左边
	for(j=0;j<height;j++)
		for(i=0;i<wedge;i++)
		{
			orgrgbedge[i*3+0+(j+hedge)*(width+2*wedge)*3] = rgb_output[j*inwidth+0];
			orgrgbedge[i*3+1+(j+hedge)*(width+2*wedge)*3] = rgb_output[j*inwidth+1];
			orgrgbedge[i*3+2+(j+hedge)*(width+2*wedge)*3] = rgb_output[j*inwidth+2];
			if(rgb_output[j*inwidth+0]==HOLER&&rgb_output[j*inwidth+1]==HOLEG&&rgb_output[j*inwidth+2]==HOLEB)
			{
				holeflag[i+(j+hedge)*(width+2*wedge)] = 0;
			}
			else
			{
				holeflag[i+(j+hedge)*(width+2*wedge)] = 1;
			}
		}
	// 右边
	for(j=0;j<height;j++)
		for(i=0;i<wedge;i++)
		{
			orgrgbedge[(i+width+wedge)*3+0+(j+hedge)*(width+2*wedge)*3] = rgb_output[(width-1)*3+j*inwidth+0];
			orgrgbedge[(i+width+wedge)*3+1+(j+hedge)*(width+2*wedge)*3] = rgb_output[(width-1)*3+j*inwidth+1];
			orgrgbedge[(i+width+wedge)*3+2+(j+hedge)*(width+2*wedge)*3] = rgb_output[(width-1)*3+j*inwidth+2];
			if(rgb_output[(width-1)*3+j*inwidth+0]==HOLER&&rgb_output[(width-1)*3+j*inwidth+1]==HOLEG&&rgb_output[(width-1)*3+j*inwidth+2]==HOLEB)
			{
				holeflag[(i+width+wedge)+(j+hedge)*(width+2*wedge)] = 0;
			}
			else
			{
				holeflag[(i+width+wedge)+(j+hedge)*(width+2*wedge)] = 1;
			}
		}

	// 中间
	for(j=0;j<height;j++)
		for(i=0;i<width;i++)
		{
			orgrgbedge[(i+wedge)*3+0+(j+hedge)*(width+2*wedge)*3] = rgb_output[i*3+j*inwidth+0];
			orgrgbedge[(i+wedge)*3+1+(j+hedge)*(width+2*wedge)*3] = rgb_output[i*3+j*inwidth+1];
			orgrgbedge[(i+wedge)*3+2+(j+hedge)*(width+2*wedge)*3] = rgb_output[i*3+j*inwidth+2];
			if(rgb_output[i*3+j*inwidth+0]==HOLER&&rgb_output[i*3+j*inwidth+1]==HOLEG&&rgb_output[i*3+j*inwidth+2]==HOLEB)
			{
				holeflag[(i+wedge)+(j+hedge)*(width+2*wedge)] = 0;
				havehole = 1;
			}
			else
			{
				holeflag[(i+wedge)+(j+hedge)*(width+2*wedge)] = 1;
			}
		}

	return havehole;
}


// 5x5 Gausefilter&downsample
int HHFGauseDownSample(uint8_t * orgrgbedge, uint8_t * holeflag, uint8_t * orgrgbgausedown, int width, int wedge, int height, int hedge)
{
	int i,j, m,n;
	int havehole=0;
	int width2 = width>>1;
	int height2 = height>>1;
	int waddedge = width + 2*wedge;
	uint8_t * orgrgbedgeoffset = orgrgbedge+wedge*3+hedge*(width+2*wedge)*3;
	uint8_t * holeflagoffset = holeflag+wedge+hedge*(width+2*wedge);
	
	int templates[25] = { 1, 4, 7, 4, 1,   
                           4, 16, 26, 16, 4,   
                           7, 26, 41, 26, 7,  
                           4, 16, 26, 16, 4,   
                           1, 4, 7, 4, 1 }; 
	

	for(j=0;j<height2;j++)
		for(i=0;i<width2;i++)
		{
			int sumr = 0; 
			int sumg = 0; 
			int sumb = 0; 
			int sumw = 0;
            int index = 0; 
			for(m=2*j-2;m<2*j+3;m++)
				for(n=2*i-2;n<2*i+3;n++)
				{
					sumr += orgrgbedgeoffset[ m*waddedge*3 + n*3 +0 ] * holeflagoffset[ m*waddedge + n] * templates[index] ; 
					sumg += orgrgbedgeoffset[ m*waddedge*3 + n*3 +1 ] * holeflagoffset[ m*waddedge + n] * templates[index] ; 
					sumb += orgrgbedgeoffset[ m*waddedge*3 + n*3 +2 ] * holeflagoffset[ m*waddedge + n] * templates[index] ; 
					sumw += holeflagoffset[ m*waddedge + n] * templates[index] ;
				}
				if(sumw==0)
				{
					havehole = 1;
					orgrgbgausedown[i*3+0+j*width2*3] = HOLER;
					orgrgbgausedown[i*3+1+j*width2*3] = HOLEG;
					orgrgbgausedown[i*3+2+j*width2*3] = HOLEB;
				}
				else
				{
					sumr = sumr/sumw;
					sumg = sumg/sumw;
					sumb = sumb/sumw;
					if (sumr > 255)  sumr = 255;
					if (sumg > 255)  sumg = 255;
					if (sumb > 255)  sumb = 255;
					orgrgbgausedown[i*3+0+j*width2*3] = sumr;
					orgrgbgausedown[i*3+1+j*width2*3] = sumg;
					orgrgbgausedown[i*3+2+j*width2*3] = sumb;
				}
		}
	
	return havehole;
}

/* 线性扩展并填充*/
void HHFLinearExpandFill(uint8_t * rgbdown, uint8_t * rgbup, int srcwidth, int srcheight, int dstwidth, int dstheight, int dststride)
{
	int i,j;
	uint8_t *rgbuptmp = new uint8_t[dstwidth*dstheight*3];
	
	InterpImageBilinear(rgbdown, rgbuptmp, srcwidth, srcheight, dstwidth, dstheight);

	for(j=0;j<dstheight;j++)
		for(i=0;i<dstwidth;i++)
		{
			if(rgbup[i*3+0+j*dststride]==HOLER&&rgbup[i*3+1+j*dststride]==HOLEG&&rgbup[i*3+2+j*dststride]==HOLEB)
			{
				rgbup[i*3+0+j*dststride] = rgbuptmp[i*3+0+j*dstwidth*3];
				rgbup[i*3+1+j*dststride] = rgbuptmp[i*3+1+j*dstwidth*3];
				rgbup[i*3+2+j*dststride] = rgbuptmp[i*3+2+j*dstwidth*3];
			}
		}

	delete [] rgbuptmp;
	return ;
}

void HHFBaseHoleFill(uint8_t *rgb, int width, int height)
{
	int i,j;
	short lpos, rpos;

	short * leftpos = new short[width*height];
	short * rightpos = new short[width*height];

	
	for(j=0;j<height;j++)
	{
		// 空洞左侧位置
		lpos = -1;
		for(i=0;i<width;i++)
		{
			if(rgb[i*3+0+j*width*3]==HOLER&&rgb[i*3+1+j*width*3]==HOLEG&&rgb[i*3+2+j*width*3]==HOLEB)
			{
				leftpos[i+j*width] = lpos;
			}
			else
			{
				lpos = i;
				leftpos[i+j*width] = lpos;
			}
		}
		
		// 空洞右侧位置
		rpos = width;
		for(i=width-1;i>=0;i--)
		{
			if(rgb[i*3+0+j*width*3]==HOLER&&rgb[i*3+1+j*width*3]==HOLEG&&rgb[i*3+2+j*width*3]==HOLEB)
			{
				rightpos[i+j*width] = rpos;
			}
			else
			{
				rpos = i;
				rightpos[i+j*width] = rpos;
			}
		}

		// 填充
		for(i=0;i<width;i++)
		{
			if(rgb[i*3+0+j*width*3]==HOLER&&rgb[i*3+1+j*width*3]==HOLEG&&rgb[i*3+2+j*width*3]==HOLEB)
			{
				if(leftpos[i+j*width]==-1)
				{
					rpos = rightpos[i+j*width];
					rgb[i*3+0+j*width*3]=rgb[rpos*3+0+j*width*3];
					rgb[i*3+1+j*width*3]=rgb[rpos*3+1+j*width*3];
					rgb[i*3+2+j*width*3]=rgb[rpos*3+2+j*width*3];
				}
				else if(rightpos[i+j*width]==width)
				{
					lpos = leftpos[i+j*width];
					rgb[i*3+0+j*width*3]=rgb[lpos*3+0+j*width*3];
					rgb[i*3+1+j*width*3]=rgb[lpos*3+1+j*width*3];
					rgb[i*3+2+j*width*3]=rgb[lpos*3+2+j*width*3];
				}
				else
				{
					lpos = leftpos[i+j*width];
					rpos = rightpos[i+j*width];
					rgb[i*3+0+j*width*3]=((rgb[lpos*3+0+j*width*3]*(i-lpos)+rgb[rpos*3+0+j*width*3]*(rpos-i)))/(rpos-lpos);
					rgb[i*3+1+j*width*3]=((rgb[lpos*3+1+j*width*3]*(i-lpos)+rgb[rpos*3+1+j*width*3]*(rpos-i)))/(rpos-lpos);
					rgb[i*3+2+j*width*3]=((rgb[lpos*3+2+j*width*3]*(i-lpos)+rgb[rpos*3+2+j*width*3]*(rpos-i)))/(rpos-lpos);
				}
			}
		}
	}
	delete [] leftpos;
	delete [] rightpos;
	
	return ;
}

// 2012_Hierarchical Hole-Filling For Depth-Based View Synthesis in FTV and 3D Video
// 区别: nz(Am,n)<25时，还是按照加权来计算
//              expand采用线性插值
void HHFFillRender(uint8_t * rgb_output)
{
	int k;
	
	int inwidth = g_frameInWidth;
	int inheight = g_frameInHeight;
	int inwidth2 = inwidth>>1;
	int inheight2 = inheight>>1;
	int inwidth4 = inwidth2>>1;
	int inheight4 = inheight2>>1;
	int inwidth8 = inwidth4>>1;
	int inheight8 = inheight4>>1;
	int inwidth16 = inwidth8>>1;
	int inheight16 = inheight8>>1;
	int havehole;

	int rightoffset = g_frameInWidth*3;

	#if 0
	int i,j;
	CvSize sz;
	IplImage* ptmpimg;
	#endif

	// 空洞标记
	uint8_t * holeflag = NULL;
	uint8_t * holeflag2 = NULL;
	uint8_t * holeflag4 = NULL;
	uint8_t * holeflag8 = NULL;
	uint8_t * holeflag16 = NULL;

	// 加边后的render 后rgb
	uint8_t * orgrgbedge = NULL;
	uint8_t * orgrgbedge2 = NULL;
	uint8_t * orgrgbedge4 = NULL;
	uint8_t * orgrgbedge8 = NULL;
	uint8_t * orgrgbedge16 = NULL;

	// 高斯下采样后的rgb
	uint8_t * orgrgbgause2 = NULL;
	uint8_t * orgrgbgause4 = NULL;
	uint8_t * orgrgbgause8 = NULL;
	uint8_t * orgrgbgause16 = NULL;

	holeflag = new uint8_t[(g_frameInWidth+2*HHFEDGE)*(g_frameInHeight+2*HHFEDGE)];
	orgrgbedge = new uint8_t[(g_frameInWidth+2*HHFEDGE)*(g_frameInHeight+2*HHFEDGE)*3];
	
	for(k=0;k<2;k++)
	{
		// 加边

		havehole = HHFAddEdge(rgb_output+k*rightoffset, g_frameInWidth*6, orgrgbedge, holeflag, g_frameInWidth, HHFEDGE, g_frameInHeight, HHFEDGE);
	
		if(havehole==1)
		{
			orgrgbgause2 = new uint8_t[inwidth2*inheight2*3];
			havehole = HHFGauseDownSample(orgrgbedge, holeflag, orgrgbgause2, g_frameInWidth, HHFEDGE, g_frameInHeight, HHFEDGE);

			if(havehole==1)
			{
				holeflag2 = new uint8_t[(inwidth2+2*HHFEDGE)*(inheight2+2*HHFEDGE)];
				orgrgbedge2 = new uint8_t[(inwidth2+2*HHFEDGE)*(inheight2+2*HHFEDGE)*3];
				HHFAddEdge(orgrgbgause2, inwidth2*3, orgrgbedge2, holeflag2, inwidth2, HHFEDGE, inheight2, HHFEDGE);

				orgrgbgause4 = new uint8_t[inwidth4*inheight4*3];
				havehole = HHFGauseDownSample(orgrgbedge2, holeflag2, orgrgbgause4, inwidth2, HHFEDGE, inheight2, HHFEDGE);

				if(havehole==1)
				{
					holeflag4 = new uint8_t[(inwidth4+2*HHFEDGE)*(inheight4+2*HHFEDGE)];
					orgrgbedge4 = new uint8_t[(inwidth4+2*HHFEDGE)*(inheight4+2*HHFEDGE)*3];
					HHFAddEdge(orgrgbgause4, inwidth4*3, orgrgbedge4, holeflag4, inwidth4, HHFEDGE, inheight4, HHFEDGE);
					
					orgrgbgause8 = new uint8_t[inwidth8*inheight8*3];
					havehole = HHFGauseDownSample(orgrgbedge4, holeflag4, orgrgbgause8, inwidth4, HHFEDGE, inheight4, HHFEDGE);
					if(havehole)
					{
						holeflag8 = new uint8_t[(inwidth8+2*HHFEDGE)*(inheight8+2*HHFEDGE)];
						orgrgbedge8 = new uint8_t[(inwidth8+2*HHFEDGE)*(inheight8+2*HHFEDGE)*3];
						HHFAddEdge(orgrgbgause8, inwidth8*3, orgrgbedge8, holeflag8, inwidth8, HHFEDGE, inheight8, HHFEDGE);
						
						orgrgbgause16 = new uint8_t[inwidth16*inheight16*3];
						havehole = HHFGauseDownSample(orgrgbedge8, holeflag8, orgrgbgause16, inwidth8, HHFEDGE, inheight8, HHFEDGE);
						#if 0
						sz.width = inwidth16;
						sz.height = inheight16;
						ptmpimg = cvCreateImage(sz, IPL_DEPTH_8U, 3);
						for(j=0;j<sz.height;j++)
							for(i=0;i<sz.width;i++)
							{
								ptmpimg->imageData[i*3+2+j*ptmpimg->widthStep] = orgrgbgause16[i*3+j*3*sz.width]; 
								ptmpimg->imageData[i*3+1+j*ptmpimg->widthStep] = orgrgbgause16[i*3+1+j*3*sz.width];
								ptmpimg->imageData[i*3+0+j*ptmpimg->widthStep] = orgrgbgause16[i*3+2+j*3*sz.width];
							}
						cvSaveImage("testdbg.bmp", ptmpimg);
						cvReleaseImage( &ptmpimg );
						#endif
						if(havehole)
						{
							// 应该没有空洞了
							// printf("\nDEBUG always have holes\n");
							// 还有空洞，则直接填充
							HHFBaseHoleFill(orgrgbgause16, inwidth16, inheight16);
						}
						
						HHFLinearExpandFill(orgrgbgause16, orgrgbgause8, inwidth16, inheight16, inwidth8, inheight8, inwidth8*3);
						delete [] orgrgbgause16;
						delete [] holeflag8;
						delete [] orgrgbedge8;
						
					}

					HHFLinearExpandFill(orgrgbgause8, orgrgbgause4, inwidth8, inheight8, inwidth4, inheight4, inwidth4*3);
					delete [] orgrgbgause8;
					delete [] holeflag4;
					delete [] orgrgbedge4;
				}
				HHFLinearExpandFill(orgrgbgause4, orgrgbgause2, inwidth4, inheight4, inwidth2, inheight2, inwidth2*3);
				delete [] orgrgbgause4;
				delete [] holeflag2;
				delete [] orgrgbedge2;
			}
			HHFLinearExpandFill(orgrgbgause2, rgb_output+k*rightoffset, inwidth2, inheight2, inwidth, inheight, inwidth*6);
			delete [] orgrgbgause2;
		}
	}

	delete [] holeflag;
	delete [] orgrgbedge;
	
	return ;
}



/* 由深度图进行视角合成算法*/
/*
  输入
  in_buffer		当前帧图像RGB
  in_depth_buffer	当前帧深度图 Y（灰度图）
  width			图像宽
  height			图像高
  format			输出格式
  param			待加，输入参数(如方法选择)
  输出
  out_buffer		当前帧多视角图像YUV
  */

/**********************************************************************************
算法说明

参考专利CN200810105346.2 一种基于深度图的快速图像绘制方法
步骤1）像素移位。
步骤2）去除量化噪声。
步骤3）去除明显错误。
步骤4）空洞填充。

**********************************************************************************/
void DepthRenderingDisplay(uint8_t * in_buffer, uint8_t * out_buffer, int inwidth, int inheight, int format)
{
	int i,j;
	char name[FILE_NAME_SIZE];
	
	uint8_t *rgb_input = g_rgborginput;
	uint8_t *in_depth_buffer = NULL;
	uint8_t *rgb_output = NULL;
	uint8_t *y_output, *u_output, *v_output;

	// use opencv to inpainting 
	//IplImage* pImgin;
	//IplImage* pImgmask;
	//IplImage* pImgout;
	CvSize sz;
	// for save stereo image
	IplImage* stereoimg0, *stereoimg1;
	// for left right image display
	IplImage* leftimg0, *leftimg1;
	IplImage* rightimg0, *rightimg1;
	IplImage* animg0, *animg1;
	IplImage* pdepthin;

	in_depth_buffer = new uint8_t[inwidth*inheight];
	rgb_output = new uint8_t[inwidth*inheight*6];

	// load depth
	if(g_keyframeflag)
		sprintf(name,"./%s/depth/%d_depthsnap_%d.png",cfgparams.projectname, g_sectioncur, g_keyframenumarr[g_frameIndex]+1);
	else
		sprintf(name,"./%s/depth/%d_depthsnap_%d.png",cfgparams.projectname, g_sectioncur, g_frameIndex+1);
	if(_access(name,0))
	{
		// no depth image
		//printf("\rDepth Rendering --- No depth image of frame index %d", g_frameIndex+1);
		//yresize(g_frames[g_frameIndex].SegDepths, in_depth_buffer, g_frameWidth, g_frameHeight, inwidth, inheight);
		if(g_frameResizeFlag==0)
			DepthProcess(g_yuvinput.y, NULL, g_frames[g_frameIndex].SegDepths, in_depth_buffer, inwidth, inheight, 0, g_depthprocessparam);
		else
			DepthProcess(g_yuvorginput.y, NULL, g_frames[g_frameIndex].SegDepths0, in_depth_buffer, inwidth, inheight, 0, g_depthprocessparam);
	}
	else
	{
		// load depth image
		pdepthin = cvLoadImage(name);

		for(j=0;j<inheight;j++)
		{
			for(i=0;i<inwidth;i++)
			{
				in_depth_buffer[i+j*inwidth] = (uint8_t)(pdepthin->imageData[3*i+j*pdepthin->widthStep]);
			}
		}
		cvReleaseImage(&pdepthin);
		//printf("\rDepth Rendering --- Load depth image of frame index %d", g_frameIndex+1);
	}

	if(g_rendermethod==RENDERWARP)
	{
		// 背景层次render
		BackgroundRender(rgb_output);
		// 其他层次render
		LayerRender(rgb_output);
		if(g_renderprocmethod==RENDERPROCHOLLFILL) // 插值填补并进行羽化处理
			InterFillRender(rgb_output, rgb_input);
		else if(g_renderprocmethod==RENDERPROCINPAINTING) // Inpaint填补
			InpaintFillRender(rgb_output, rgb_input);
		else if(g_renderprocmethod==RENDERPROCHHF)
			HHFFillRender(rgb_output);
	}
	else
	{
		int depthtmp;
		for(j=0;j<g_frameInHeight;j++)
			for(i=0;i<g_frameInWidth;i++)
			{
				depthtmp = in_depth_buffer[i+j*g_frameInWidth];
				if(depthtmp>g_renderforcalplane)
				{
					depthtmp = g_renderforcalplane + (depthtmp-g_renderforcalplane)*(depthtmp-g_renderforcalplane);
					if(depthtmp>255)
						depthtmp = 255;
				}
				else
				{
					depthtmp = g_renderforcalplane - (depthtmp-g_renderforcalplane)*(depthtmp-g_renderforcalplane);
					if(depthtmp<0)
						depthtmp = 0;
				}
				in_depth_buffer[i+j*g_frameInWidth] = depthtmp;
				
			}
		Render3DImage(rgb_input, in_depth_buffer, rgb_output, g_renderalpha, g_renderforcalplane, g_frameInWidth, g_frameInHeight);
	}

	if(format==RENDEROUTLR)
	{
		// left right display
		if(g_frameResizeFlag==1)
		{
			sz.width = inwidth;
			sz.height = inheight;
			leftimg0 = cvCreateImage(sz, IPL_DEPTH_8U, 3);
			for(j=0;j<inheight;j++)
				for(i=0;i<inwidth;i++)
				{
					leftimg0->imageData[i*3+2+j*leftimg0->widthStep] = rgb_output[i*3+j*6*inwidth]; 
					leftimg0->imageData[i*3+1+j*leftimg0->widthStep] = rgb_output[i*3+1+j*6*inwidth];
					leftimg0->imageData[i*3+0+j*leftimg0->widthStep] = rgb_output[i*3+2+j*6*inwidth];
				}
			rightimg0 = cvCreateImage(sz, IPL_DEPTH_8U, 3);
			for(j=0;j<inheight;j++)
				for(i=0;i<inwidth;i++)
				{
					rightimg0->imageData[i*3+2+j*rightimg0->widthStep] = rgb_output[i*3+j*6*inwidth+3*inwidth]; 
					rightimg0->imageData[i*3+1+j*rightimg0->widthStep] = rgb_output[i*3+1+j*6*inwidth+3*inwidth];
					rightimg0->imageData[i*3+0+j*rightimg0->widthStep] = rgb_output[i*3+2+j*6*inwidth+3*inwidth];
				}
			
			sz.width = g_frameWidth;
			sz.height = g_frameHeight;
			leftimg1 = cvCreateImage(sz, IPL_DEPTH_8U, 3);
			rightimg1 = cvCreateImage(sz, IPL_DEPTH_8U, 3);
			cvResize(leftimg0, leftimg1,CV_INTER_CUBIC);
			cvResize(rightimg0, rightimg1,CV_INTER_CUBIC);
			switch(g_renderalpha)
			{
				case 40:
					for(j=0;j<g_frameHeight;j++)
						for(i=0;i<g_frameWidth;i++)
						{
							g_renderdisplay1[i*3+j*3*g_frameWidth] = leftimg1->imageData[i*3+2+j*leftimg1->widthStep]; 
							g_renderdisplay1[i*3+1+j*3*g_frameWidth] = leftimg1->imageData[i*3+1+j*leftimg1->widthStep];
							g_renderdisplay1[i*3+2+j*3*g_frameWidth] = leftimg1->imageData[i*3+0+j*leftimg1->widthStep];
							g_renderdisplay8[i*3+j*3*g_frameWidth] = rightimg1->imageData[i*3+2+j*rightimg1->widthStep]; 
							g_renderdisplay8[i*3+1+j*3*g_frameWidth] = rightimg1->imageData[i*3+1+j*rightimg1->widthStep];
							g_renderdisplay8[i*3+2+j*3*g_frameWidth] = rightimg1->imageData[i*3+0+j*rightimg1->widthStep];
						}
					break;
				case 30:
					for(j=0;j<g_frameHeight;j++)
						for(i=0;i<g_frameWidth;i++)
						{
							g_renderdisplay2[i*3+j*3*g_frameWidth] = leftimg1->imageData[i*3+2+j*leftimg1->widthStep]; 
							g_renderdisplay2[i*3+1+j*3*g_frameWidth] = leftimg1->imageData[i*3+1+j*leftimg1->widthStep];
							g_renderdisplay2[i*3+2+j*3*g_frameWidth] = leftimg1->imageData[i*3+0+j*leftimg1->widthStep];
							g_renderdisplay7[i*3+j*3*g_frameWidth] = rightimg1->imageData[i*3+2+j*rightimg1->widthStep]; 
							g_renderdisplay7[i*3+1+j*3*g_frameWidth] = rightimg1->imageData[i*3+1+j*rightimg1->widthStep];
							g_renderdisplay7[i*3+2+j*3*g_frameWidth] = rightimg1->imageData[i*3+0+j*rightimg1->widthStep];
						}
					break;
				case 20:
					for(j=0;j<g_frameHeight;j++)
						for(i=0;i<g_frameWidth;i++)
						{
							g_renderdisplay3[i*3+j*3*g_frameWidth] = leftimg1->imageData[i*3+2+j*leftimg1->widthStep]; 
							g_renderdisplay3[i*3+1+j*3*g_frameWidth] = leftimg1->imageData[i*3+1+j*leftimg1->widthStep];
							g_renderdisplay3[i*3+2+j*3*g_frameWidth] = leftimg1->imageData[i*3+0+j*leftimg1->widthStep];
							g_renderdisplay6[i*3+j*3*g_frameWidth] = rightimg1->imageData[i*3+2+j*rightimg1->widthStep]; 
							g_renderdisplay6[i*3+1+j*3*g_frameWidth] = rightimg1->imageData[i*3+1+j*rightimg1->widthStep];
							g_renderdisplay6[i*3+2+j*3*g_frameWidth] = rightimg1->imageData[i*3+0+j*rightimg1->widthStep];
						}
					break;
				case 10:
					for(j=0;j<g_frameHeight;j++)
						for(i=0;i<g_frameWidth;i++)
						{
							g_renderdisplay4[i*3+j*3*g_frameWidth] = leftimg1->imageData[i*3+2+j*leftimg1->widthStep]; 
							g_renderdisplay4[i*3+1+j*3*g_frameWidth] = leftimg1->imageData[i*3+1+j*leftimg1->widthStep];
							g_renderdisplay4[i*3+2+j*3*g_frameWidth] = leftimg1->imageData[i*3+0+j*leftimg1->widthStep];
							g_renderdisplay5[i*3+j*3*g_frameWidth] = rightimg1->imageData[i*3+2+j*rightimg1->widthStep]; 
							g_renderdisplay5[i*3+1+j*3*g_frameWidth] = rightimg1->imageData[i*3+1+j*rightimg1->widthStep];
							g_renderdisplay5[i*3+2+j*3*g_frameWidth] = rightimg1->imageData[i*3+0+j*rightimg1->widthStep];
						}
					break;
				default:
					for(j=0;j<g_frameHeight;j++)
						for(i=0;i<g_frameWidth;i++)
						{
							g_tmpdisplay1[i*3+j*3*g_frameWidth] = leftimg1->imageData[i*3+2+j*leftimg1->widthStep]; 
							g_tmpdisplay1[i*3+1+j*3*g_frameWidth] = leftimg1->imageData[i*3+1+j*leftimg1->widthStep];
							g_tmpdisplay1[i*3+2+j*3*g_frameWidth] = leftimg1->imageData[i*3+0+j*leftimg1->widthStep];
							g_tmpdisplay2[i*3+j*3*g_frameWidth] = rightimg1->imageData[i*3+2+j*rightimg1->widthStep]; 
							g_tmpdisplay2[i*3+1+j*3*g_frameWidth] = rightimg1->imageData[i*3+1+j*rightimg1->widthStep];
							g_tmpdisplay2[i*3+2+j*3*g_frameWidth] = rightimg1->imageData[i*3+0+j*rightimg1->widthStep];
						}
					break;
			}
			for(j=0;j<g_frameHeight;j++)
				for(i=0;i<g_frameWidth;i++)
				{
					g_tmpdisplay1[i*3+j*3*g_frameWidth] = leftimg1->imageData[i*3+2+j*leftimg1->widthStep]; 
					g_tmpdisplay1[i*3+1+j*3*g_frameWidth] = leftimg1->imageData[i*3+1+j*leftimg1->widthStep];
					g_tmpdisplay1[i*3+2+j*3*g_frameWidth] = leftimg1->imageData[i*3+0+j*leftimg1->widthStep];
					g_tmpdisplay2[i*3+j*3*g_frameWidth] = rightimg1->imageData[i*3+2+j*rightimg1->widthStep]; 
					g_tmpdisplay2[i*3+1+j*3*g_frameWidth] = rightimg1->imageData[i*3+1+j*rightimg1->widthStep];
					g_tmpdisplay2[i*3+2+j*3*g_frameWidth] = rightimg1->imageData[i*3+0+j*rightimg1->widthStep];
				}
				
			cvReleaseImage( &leftimg0 );
			cvReleaseImage( &rightimg0 );
			cvReleaseImage( &leftimg1 );
			cvReleaseImage( &rightimg1 );

		}
		else
		{
			switch(g_renderalpha)
			{
				case 40:
					for(j=0;j<g_frameHeight;j++)
						for(i=0;i<g_frameWidth;i++)
						{
							g_renderdisplay1[i*3+j*3*g_frameWidth] = rgb_output[i*3+j*6*inwidth]; 
							g_renderdisplay1[i*3+1+j*3*g_frameWidth] = rgb_output[i*3+1+j*6*inwidth];
							g_renderdisplay1[i*3+2+j*3*g_frameWidth] = rgb_output[i*3+2+j*6*inwidth];
							g_renderdisplay8[i*3+j*3*g_frameWidth] = rgb_output[i*3+j*6*inwidth+3*inwidth];
							g_renderdisplay8[i*3+1+j*3*g_frameWidth] = rgb_output[i*3+1+j*6*inwidth+3*inwidth];
							g_renderdisplay8[i*3+2+j*3*g_frameWidth] = rgb_output[i*3+2+j*6*inwidth+3*inwidth];
						}
					break;
				case 30:
					for(j=0;j<g_frameHeight;j++)
						for(i=0;i<g_frameWidth;i++)
						{
							g_renderdisplay2[i*3+j*3*g_frameWidth] = rgb_output[i*3+j*6*inwidth]; 
							g_renderdisplay2[i*3+1+j*3*g_frameWidth] = rgb_output[i*3+1+j*6*inwidth];
							g_renderdisplay2[i*3+2+j*3*g_frameWidth] = rgb_output[i*3+2+j*6*inwidth];
							g_renderdisplay7[i*3+j*3*g_frameWidth] = rgb_output[i*3+j*6*inwidth+3*inwidth];
							g_renderdisplay7[i*3+1+j*3*g_frameWidth] = rgb_output[i*3+1+j*6*inwidth+3*inwidth];
							g_renderdisplay7[i*3+2+j*3*g_frameWidth] = rgb_output[i*3+2+j*6*inwidth+3*inwidth];
						}
					break;
				case 20:
					for(j=0;j<g_frameHeight;j++)
						for(i=0;i<g_frameWidth;i++)
						{
							g_renderdisplay3[i*3+j*3*g_frameWidth] = rgb_output[i*3+j*6*inwidth]; 
							g_renderdisplay3[i*3+1+j*3*g_frameWidth] = rgb_output[i*3+1+j*6*inwidth];
							g_renderdisplay3[i*3+2+j*3*g_frameWidth] = rgb_output[i*3+2+j*6*inwidth];
							g_renderdisplay6[i*3+j*3*g_frameWidth] = rgb_output[i*3+j*6*inwidth+3*inwidth];
							g_renderdisplay6[i*3+1+j*3*g_frameWidth] = rgb_output[i*3+1+j*6*inwidth+3*inwidth];
							g_renderdisplay6[i*3+2+j*3*g_frameWidth] = rgb_output[i*3+2+j*6*inwidth+3*inwidth];
						}
					break;
				case 10:
					for(j=0;j<g_frameHeight;j++)
						for(i=0;i<g_frameWidth;i++)
						{
							g_renderdisplay4[i*3+j*3*g_frameWidth] = rgb_output[i*3+j*6*inwidth]; 
							g_renderdisplay4[i*3+1+j*3*g_frameWidth] = rgb_output[i*3+1+j*6*inwidth];
							g_renderdisplay4[i*3+2+j*3*g_frameWidth] = rgb_output[i*3+2+j*6*inwidth];
							g_renderdisplay5[i*3+j*3*g_frameWidth] = rgb_output[i*3+j*6*inwidth+3*inwidth];
							g_renderdisplay5[i*3+1+j*3*g_frameWidth] = rgb_output[i*3+1+j*6*inwidth+3*inwidth];
							g_renderdisplay5[i*3+2+j*3*g_frameWidth] = rgb_output[i*3+2+j*6*inwidth+3*inwidth];
						}
					break;
				default:
					for(j=0;j<g_frameHeight;j++)
						for(i=0;i<g_frameWidth;i++)
						{
							g_tmpdisplay1[i*3+j*3*g_frameWidth] = rgb_output[i*3+j*6*inwidth]; 
							g_tmpdisplay1[i*3+1+j*3*g_frameWidth] = rgb_output[i*3+1+j*6*inwidth];
							g_tmpdisplay1[i*3+2+j*3*g_frameWidth] = rgb_output[i*3+2+j*6*inwidth];
							g_tmpdisplay2[i*3+j*3*g_frameWidth] = rgb_output[i*3+j*6*inwidth+3*inwidth];
							g_tmpdisplay2[i*3+1+j*3*g_frameWidth] = rgb_output[i*3+1+j*6*inwidth+3*inwidth];
							g_tmpdisplay2[i*3+2+j*3*g_frameWidth] = rgb_output[i*3+2+j*6*inwidth+3*inwidth];
						}
					break;
					
			}
			for(j=0;j<g_frameHeight;j++)
				for(i=0;i<g_frameWidth;i++)
				{
					g_tmpdisplay1[i*3+j*3*g_frameWidth] = rgb_output[i*3+j*6*inwidth]; 
					g_tmpdisplay1[i*3+1+j*3*g_frameWidth] = rgb_output[i*3+1+j*6*inwidth];
					g_tmpdisplay1[i*3+2+j*3*g_frameWidth] = rgb_output[i*3+2+j*6*inwidth];
					g_tmpdisplay2[i*3+j*3*g_frameWidth] = rgb_output[i*3+j*6*inwidth+3*inwidth];
					g_tmpdisplay2[i*3+1+j*3*g_frameWidth] = rgb_output[i*3+1+j*6*inwidth+3*inwidth];
					g_tmpdisplay2[i*3+2+j*3*g_frameWidth] = rgb_output[i*3+2+j*6*inwidth+3*inwidth];
				}
		}

		// stereo save
		sz.width = inwidth*2;
		sz.height = inheight;
		stereoimg0 = cvCreateImage(sz, IPL_DEPTH_8U, 3);
		for(j=0;j<inheight;j++)
			for(i=0;i<inwidth*2;i++)
			{
				stereoimg0->imageData[i*3+2+j*stereoimg0->widthStep] = rgb_output[i*3+j*6*inwidth]; 
				stereoimg0->imageData[i*3+1+j*stereoimg0->widthStep] = rgb_output[i*3+1+j*6*inwidth];
				stereoimg0->imageData[i*3+0+j*stereoimg0->widthStep] = rgb_output[i*3+2+j*6*inwidth];
			}
		sz.width = g_frameWidth*2;
		sz.height = g_frameHeight;
		stereoimg1 = cvCreateImage(sz, IPL_DEPTH_8U, 3);
		cvResize(stereoimg0, stereoimg1,CV_INTER_CUBIC);

		if(g_keyframeflag)
			sprintf(name,"./%s/render/%d_rendersnap_%d.png",cfgparams.projectname,g_sectioncur,g_keyframenumarr[g_frameIndex]+1);
		else
			sprintf(name,"./%s/render/%d_rendersnap_%d.png",cfgparams.projectname,g_sectioncur,g_frameIndex+1);
		cvSaveImage(name, stereoimg0);

		if(g_frameResizeFlag==1)
		{
			for(j=0;j<g_frameInHeight;j++)
				for(i=0;i<g_frameInWidth*2;i++)
				{
					g_renderorgtmp1[i*3+0+j*g_frameInWidth*6] = g_renderorgtmp0[i*3+0+j*g_frameInWidth*6] = (uint8_t)(stereoimg0->imageData[i*3+2+j*stereoimg0->widthStep]);
					g_renderorgtmp1[i*3+1+j*g_frameInWidth*6] = g_renderorgtmp0[i*3+1+j*g_frameInWidth*6] = (uint8_t)(stereoimg0->imageData[i*3+1+j*stereoimg0->widthStep]);
					g_renderorgtmp1[i*3+2+j*g_frameInWidth*6] = g_renderorgtmp0[i*3+2+j*g_frameInWidth*6] = (uint8_t)(stereoimg0->imageData[i*3+0+j*stereoimg0->widthStep]);
				}
			for(j=0;j<g_frameHeight;j++)
				for(i=0;i<g_frameWidth*2;i++)
				{
					g_rendertmp1[i*3+0+j*g_frameWidth*6] = g_rendertmp0[i*3+0+j*g_frameWidth*6] = (uint8_t)(stereoimg1->imageData[i*3+2+j*stereoimg1->widthStep]);
					g_rendertmp1[i*3+1+j*g_frameWidth*6] = g_rendertmp0[i*3+1+j*g_frameWidth*6] = (uint8_t)(stereoimg1->imageData[i*3+1+j*stereoimg1->widthStep]);
					g_rendertmp1[i*3+2+j*g_frameWidth*6] = g_rendertmp0[i*3+2+j*g_frameWidth*6] = (uint8_t)(stereoimg1->imageData[i*3+0+j*stereoimg1->widthStep]);
				}
		}
		else
		{
			for(j=0;j<g_frameHeight;j++)
				for(i=0;i<g_frameWidth*2;i++)
				{
					g_rendertmp1[i*3+0+j*g_frameWidth*6] = g_rendertmp0[i*3+0+j*g_frameWidth*6] = (uint8_t)(stereoimg1->imageData[i*3+2+j*stereoimg1->widthStep]);
					g_rendertmp1[i*3+1+j*g_frameWidth*6] = g_rendertmp0[i*3+1+j*g_frameWidth*6] = (uint8_t)(stereoimg1->imageData[i*3+1+j*stereoimg1->widthStep]);
					g_rendertmp1[i*3+2+j*g_frameWidth*6] = g_rendertmp0[i*3+2+j*g_frameWidth*6] = (uint8_t)(stereoimg1->imageData[i*3+0+j*stereoimg1->widthStep]);
				}
		}
		
		cvReleaseImage( &stereoimg0 );
		cvReleaseImage( &stereoimg1 );
		
		y_output = out_buffer;
		u_output = out_buffer+inwidth*2*inheight;
		v_output = out_buffer+inwidth*2*inheight*5/4;
		rgb2yuv(rgb_output, y_output, v_output, u_output, inwidth*2, inheight);
		
	}
	else
	{
		
		sz.width = inwidth;
		sz.height = inheight;
		animg0 = cvCreateImage(sz, IPL_DEPTH_8U, 3);

		y_output = out_buffer;
		u_output = out_buffer+inwidth*2*inheight;
		v_output = out_buffer+inwidth*2*inheight*5/4;
		rgb2yuv(rgb_output, y_output, v_output, u_output, inwidth*2, inheight);
		
		switch(g_renderoutputantype)
		{
			case RENDEROUTAN_RE_GR:
				for(j=0;j<inheight;j++)
					for(i=0;i<inwidth;i++)
					{
						#ifdef YTORGB
						animg0->imageData[i*3+2+j*animg0->widthStep] = out_buffer[i+j*2*inwidth]; 
						animg0->imageData[i*3+1+j*animg0->widthStep] = out_buffer[i+j*2*inwidth+inwidth];
						animg0->imageData[i*3+0+j*animg0->widthStep] = out_buffer[i+j*2*inwidth];
						#else
						animg0->imageData[i*3+2+j*animg0->widthStep] = rgb_output[i*3+j*6*inwidth]; 
						animg0->imageData[i*3+1+j*animg0->widthStep] = rgb_output[i*3+1+j*6*inwidth+3*inwidth];
						animg0->imageData[i*3+0+j*animg0->widthStep] = rgb_output[i*3+2+j*6*inwidth];
						#endif
					}
				break;
			case RENDEROUTAN_GR_RE:
				for(j=0;j<inheight;j++)
					for(i=0;i<inwidth;i++)
					{
						#ifdef YTORGB
						animg0->imageData[i*3+2+j*animg0->widthStep] = out_buffer[i+j*2*inwidth+inwidth]; 
						animg0->imageData[i*3+1+j*animg0->widthStep] = out_buffer[i+j*2*inwidth];
						animg0->imageData[i*3+0+j*animg0->widthStep] = out_buffer[i+j*2*inwidth+inwidth];
						#else
						animg0->imageData[i*3+2+j*animg0->widthStep] = rgb_output[i*3+j*6*inwidth+3*inwidth]; 
						animg0->imageData[i*3+1+j*animg0->widthStep] = rgb_output[i*3+1+j*6*inwidth];
						animg0->imageData[i*3+0+j*animg0->widthStep] = rgb_output[i*3+2+j*6*inwidth+3*inwidth];
						#endif
					}
				break;
			case RENDEROUTAN_BR_BL:
				for(j=0;j<inheight;j++)
					for(i=0;i<inwidth;i++)
					{
						#ifdef YTORGB
						animg0->imageData[i*3+2+j*animg0->widthStep] = out_buffer[i+j*2*inwidth]; 
						animg0->imageData[i*3+1+j*animg0->widthStep] = out_buffer[i+j*2*inwidth];
						animg0->imageData[i*3+0+j*animg0->widthStep] = out_buffer[i+j*2*inwidth+inwidth];
						#else
						animg0->imageData[i*3+2+j*animg0->widthStep] = rgb_output[i*3+j*6*inwidth]; 
						animg0->imageData[i*3+1+j*animg0->widthStep] = rgb_output[i*3+1+j*6*inwidth];
						animg0->imageData[i*3+0+j*animg0->widthStep] = rgb_output[i*3+2+j*6*inwidth+3*inwidth];
						#endif
					}
				break;
			case RENDEROUTAN_RE_BL:
			default: 
				for(j=0;j<inheight;j++)
					for(i=0;i<inwidth;i++)
					{
						#ifdef YTORGB
						animg0->imageData[i*3+2+j*animg0->widthStep] = out_buffer[i+j*2*inwidth]; 
						animg0->imageData[i*3+1+j*animg0->widthStep] = out_buffer[i+j*2*inwidth+inwidth];
						animg0->imageData[i*3+0+j*animg0->widthStep] = out_buffer[i+j*2*inwidth+inwidth];
						#else
						animg0->imageData[i*3+2+j*animg0->widthStep] = rgb_output[i*3+j*6*inwidth]; 
						animg0->imageData[i*3+1+j*animg0->widthStep] = rgb_output[i*3+1+j*6*inwidth+3*inwidth];
						animg0->imageData[i*3+0+j*animg0->widthStep] = rgb_output[i*3+2+j*6*inwidth+3*inwidth];
						#endif
					}
				break;
		}
		
		if(g_keyframeflag)
			sprintf(name,"./%s/render/%d_rendersnap_%d.png",cfgparams.projectname,g_sectioncur,g_keyframenumarr[g_frameIndex]+1);
		else
			sprintf(name,"./%s/render/%d_rendersnap_%d.png",cfgparams.projectname,g_sectioncur,g_frameIndex+1);
		cvSaveImage(name, animg0);
		if(g_frameResizeFlag==1)
		{
			sz.width = g_frameWidth;
			sz.height = g_frameHeight;
			animg1 = cvCreateImage(sz, IPL_DEPTH_8U, 3);
			cvResize(animg0, animg1,CV_INTER_CUBIC);
			for(j=0;j<g_frameHeight;j++)
				for(i=0;i<g_frameWidth;i++)
				{
					g_tmpdisplay1[i*3+j*3*g_frameWidth] = animg1->imageData[i*3+2+j*animg1->widthStep]; 
					g_tmpdisplay1[i*3+1+j*3*g_frameWidth] = animg1->imageData[i*3+1+j*animg1->widthStep];
					g_tmpdisplay1[i*3+2+j*3*g_frameWidth] = animg1->imageData[i*3+0+j*animg1->widthStep];
					g_tmpdisplay2[i*3+j*3*g_frameWidth] = animg1->imageData[i*3+2+j*animg1->widthStep]; 
					g_tmpdisplay2[i*3+1+j*3*g_frameWidth] = animg1->imageData[i*3+1+j*animg1->widthStep];
					g_tmpdisplay2[i*3+2+j*3*g_frameWidth] = animg1->imageData[i*3+0+j*animg1->widthStep];
				}
			cvReleaseImage( &animg1 );
		}

		for(j=0;j<inheight;j++)
			for(i=0;i<inwidth;i++)
			{
				rgb_output[i*3+j*3*inwidth] = animg0->imageData[i*3+2+j*animg0->widthStep]; 
				rgb_output[i*3+1+j*3*inwidth] = animg0->imageData[i*3+1+j*animg0->widthStep];
				rgb_output[i*3+2+j*3*inwidth] = animg0->imageData[i*3+0+j*animg0->widthStep];
			}
		
		y_output = out_buffer;
		u_output = out_buffer+inwidth*inheight;
		v_output = out_buffer+inwidth*inheight*5/4;
		rgb2yuv(rgb_output, y_output, v_output, u_output, inwidth, inheight);
		cvReleaseImage( &animg0 );
	}
	
	if(in_depth_buffer) delete []in_depth_buffer;
	if(rgb_output)      delete []rgb_output;
	
	return ;
}


/* 由深度图进行视角合成算法*/
/*
  输入
  in_buffer		当前帧图像RGB
  in_depth_buffer	当前帧深度图 Y（灰度图）
  width			图像宽
  height			图像高
  format			输出格式
  param			待加，输入参数(如方法选择)
  输出
  out_buffer		当前帧多视角图像YUV
  */

/**********************************************************************************
算法说明

参考专利CN200810105346.2 一种基于深度图的快速图像绘制方法
步骤1）像素移位。
步骤2）去除量化噪声。
步骤3）去除明显错误。
步骤4）空洞填充。

**********************************************************************************/
void DepthRendering(uint8_t * in_buffer, uint8_t * out_buffer, int inwidth, int inheight, int format)
{
	int i,j;
	char name[FILE_NAME_SIZE];
	
	uint8_t *rgb_input = in_buffer;
	uint8_t *in_depth_buffer = NULL;
	uint8_t *rgb_output = NULL;
	uint8_t *y_output, *u_output, *v_output;

	// use opencv to inpainting 
	//IplImage* pImgin;
	//IplImage* pImgmask;
	//IplImage* pImgout;
	CvSize sz;
	// for save stereo image
	IplImage* stereoimg0, *stereoimg1;
	// for left right image display
	IplImage* leftimg0, *leftimg1;
	IplImage* rightimg0, *rightimg1;
	IplImage* animg0, *animg1;
	IplImage* pdepthin;

	in_depth_buffer = new uint8_t[inwidth*inheight];
	rgb_output = new uint8_t[inwidth*inheight*6];

	// load depth
	if(g_keyframeflag)
		sprintf(name,"./%s/depth/%d_depthsnap_%d.png",cfgparams.projectname, g_sectioncur, g_keyframenumarr[g_frameIndex]+1);
	else
		sprintf(name,"./%s/depth/%d_depthsnap_%d.png",cfgparams.projectname, g_sectioncur, g_frameIndex+1);
	if(_access(name,0))
	{
		// no depth image
		//printf("\rDepth Rendering --- No depth image of frame index %d", g_frameIndex+1);
		//yresize(g_frames[g_frameIndex].SegDepths, in_depth_buffer, g_frameWidth, g_frameHeight, inwidth, inheight);
		if(g_frameResizeFlag==0)
			DepthProcess(g_yuvinput.y, NULL, g_frames[g_frameIndex].SegDepths, in_depth_buffer, inwidth, inheight, 0, g_depthprocessparam);
		else
			DepthProcess(g_yuvorginput.y, NULL, g_frames[g_frameIndex].SegDepths0, in_depth_buffer, inwidth, inheight, 0, g_depthprocessparam);
	}
	else
	{
		// load depth image
		pdepthin = cvLoadImage(name);

		for(j=0;j<inheight;j++)
		{
			for(i=0;i<inwidth;i++)
			{
				in_depth_buffer[i+j*inwidth] = (uint8_t)(pdepthin->imageData[3*i+j*pdepthin->widthStep]);
			}
		}
		cvReleaseImage(&pdepthin);
		//printf("\rDepth Rendering --- Load depth image of frame index %d", g_frameIndex+1);
	}

	if(g_rendermethod==RENDERWARP)
	{
		// 背景层次render
		BackgroundRender(rgb_output);
		// 其他层次render
		LayerRender(rgb_output);
		if(g_renderprocmethod==RENDERPROCHOLLFILL) // 插值填补并进行羽化处理
			InterFillRender(rgb_output, rgb_input);
		else if(g_renderprocmethod==RENDERPROCINPAINTING) // Inpaint填补
			InpaintFillRender(rgb_output, rgb_input);
		else if(g_renderprocmethod==RENDERPROCHHF)
			HHFFillRender(rgb_output);
	}
	else
	{
		int depthtmp;
		for(j=0;j<g_frameInHeight;j++)
			for(i=0;i<g_frameInWidth;i++)
			{
				depthtmp = in_depth_buffer[i+j*g_frameInWidth];
				if(depthtmp>g_renderforcalplane)
				{
					depthtmp = g_renderforcalplane + (depthtmp-g_renderforcalplane)*(depthtmp-g_renderforcalplane);
					if(depthtmp>255)
						depthtmp = 255;
				}
				else
				{
					depthtmp = g_renderforcalplane - (depthtmp-g_renderforcalplane)*(depthtmp-g_renderforcalplane);
					if(depthtmp<0)
						depthtmp = 0;
				}
				in_depth_buffer[i+j*g_frameInWidth] = depthtmp;
				
			}
		Render3DImage(rgb_input, in_depth_buffer, rgb_output, g_renderalpha, g_renderforcalplane, g_frameInWidth, g_frameInHeight);
	}

	if(format==RENDEROUTLR)
	{
		// left right display
		if(g_frameResizeFlag==1)
		{
			sz.width = inwidth;
			sz.height = inheight;
			leftimg0 = cvCreateImage(sz, IPL_DEPTH_8U, 3);
			for(j=0;j<inheight;j++)
				for(i=0;i<inwidth;i++)
				{
					leftimg0->imageData[i*3+2+j*leftimg0->widthStep] = rgb_output[i*3+j*6*inwidth]; 
					leftimg0->imageData[i*3+1+j*leftimg0->widthStep] = rgb_output[i*3+1+j*6*inwidth];
					leftimg0->imageData[i*3+0+j*leftimg0->widthStep] = rgb_output[i*3+2+j*6*inwidth];
				}
			rightimg0 = cvCreateImage(sz, IPL_DEPTH_8U, 3);
			for(j=0;j<inheight;j++)
				for(i=0;i<inwidth;i++)
				{
					rightimg0->imageData[i*3+2+j*rightimg0->widthStep] = rgb_output[i*3+j*6*inwidth+3*inwidth]; 
					rightimg0->imageData[i*3+1+j*rightimg0->widthStep] = rgb_output[i*3+1+j*6*inwidth+3*inwidth];
					rightimg0->imageData[i*3+0+j*rightimg0->widthStep] = rgb_output[i*3+2+j*6*inwidth+3*inwidth];
				}
			
			sz.width = g_frameWidth;
			sz.height = g_frameHeight;
			leftimg1 = cvCreateImage(sz, IPL_DEPTH_8U, 3);
			rightimg1 = cvCreateImage(sz, IPL_DEPTH_8U, 3);
			cvResize(leftimg0, leftimg1,CV_INTER_CUBIC);
			cvResize(rightimg0, rightimg1,CV_INTER_CUBIC);
			for(j=0;j<g_frameHeight;j++)
				for(i=0;i<g_frameWidth;i++)
				{
					g_tmpdisplay1[i*3+j*3*g_frameWidth] = leftimg1->imageData[i*3+2+j*leftimg1->widthStep]; 
					g_tmpdisplay1[i*3+1+j*3*g_frameWidth] = leftimg1->imageData[i*3+1+j*leftimg1->widthStep];
					g_tmpdisplay1[i*3+2+j*3*g_frameWidth] = leftimg1->imageData[i*3+0+j*leftimg1->widthStep];
					g_tmpdisplay2[i*3+j*3*g_frameWidth] = rightimg1->imageData[i*3+2+j*rightimg1->widthStep]; 
					g_tmpdisplay2[i*3+1+j*3*g_frameWidth] = rightimg1->imageData[i*3+1+j*rightimg1->widthStep];
					g_tmpdisplay2[i*3+2+j*3*g_frameWidth] = rightimg1->imageData[i*3+0+j*rightimg1->widthStep];
				}
				
			cvReleaseImage( &leftimg0 );
			cvReleaseImage( &rightimg0 );
			cvReleaseImage( &leftimg1 );
			cvReleaseImage( &rightimg1 );

		}
		else
		{
			for(j=0;j<g_frameHeight;j++)
				for(i=0;i<g_frameWidth;i++)
				{
					g_tmpdisplay1[i*3+j*3*g_frameWidth] = rgb_output[i*3+j*6*inwidth]; 
					g_tmpdisplay1[i*3+1+j*3*g_frameWidth] = rgb_output[i*3+1+j*6*inwidth];
					g_tmpdisplay1[i*3+2+j*3*g_frameWidth] = rgb_output[i*3+2+j*6*inwidth];
					g_tmpdisplay2[i*3+j*3*g_frameWidth] = rgb_output[i*3+j*6*inwidth+3*inwidth];
					g_tmpdisplay2[i*3+1+j*3*g_frameWidth] = rgb_output[i*3+1+j*6*inwidth+3*inwidth];
					g_tmpdisplay2[i*3+2+j*3*g_frameWidth] = rgb_output[i*3+2+j*6*inwidth+3*inwidth];
				}
		}

		// stereo save
		sz.width = inwidth*2;
		sz.height = inheight;
		stereoimg0 = cvCreateImage(sz, IPL_DEPTH_8U, 3);
		for(j=0;j<inheight;j++)
			for(i=0;i<inwidth*2;i++)
			{
				stereoimg0->imageData[i*3+2+j*stereoimg0->widthStep] = rgb_output[i*3+j*6*inwidth]; 
				stereoimg0->imageData[i*3+1+j*stereoimg0->widthStep] = rgb_output[i*3+1+j*6*inwidth];
				stereoimg0->imageData[i*3+0+j*stereoimg0->widthStep] = rgb_output[i*3+2+j*6*inwidth];
			}
		sz.width = g_frameWidth*2;
		sz.height = g_frameHeight;
		stereoimg1 = cvCreateImage(sz, IPL_DEPTH_8U, 3);
		cvResize(stereoimg0, stereoimg1,CV_INTER_CUBIC);
		
		if(g_keyframeflag)
			sprintf(name,"./%s/render/%d_rendersnap_%d.png",cfgparams.projectname,g_sectioncur,g_keyframenumarr[g_frameIndex]+1);
		else
			sprintf(name,"./%s/render/%d_rendersnap_%d.png",cfgparams.projectname,g_sectioncur,g_frameIndex+1);
		cvSaveImage(name, stereoimg0);

		if(g_frameResizeFlag==1)
		{
			for(j=0;j<g_frameInHeight;j++)
				for(i=0;i<g_frameInWidth*2;i++)
				{
					g_renderorgtmp1[i*3+0+j*g_frameInWidth*6] = g_renderorgtmp0[i*3+0+j*g_frameInWidth*6] = (uint8_t)(stereoimg0->imageData[i*3+2+j*stereoimg0->widthStep]);
					g_renderorgtmp1[i*3+1+j*g_frameInWidth*6] = g_renderorgtmp0[i*3+1+j*g_frameInWidth*6] = (uint8_t)(stereoimg0->imageData[i*3+1+j*stereoimg0->widthStep]);
					g_renderorgtmp1[i*3+2+j*g_frameInWidth*6] = g_renderorgtmp0[i*3+2+j*g_frameInWidth*6] = (uint8_t)(stereoimg0->imageData[i*3+0+j*stereoimg0->widthStep]);
				}
			for(j=0;j<g_frameHeight;j++)
				for(i=0;i<g_frameWidth*2;i++)
				{
					g_rendertmp1[i*3+0+j*g_frameWidth*6] = g_rendertmp0[i*3+0+j*g_frameWidth*6] = (uint8_t)(stereoimg1->imageData[i*3+2+j*stereoimg1->widthStep]);
					g_rendertmp1[i*3+1+j*g_frameWidth*6] = g_rendertmp0[i*3+1+j*g_frameWidth*6] = (uint8_t)(stereoimg1->imageData[i*3+1+j*stereoimg1->widthStep]);
					g_rendertmp1[i*3+2+j*g_frameWidth*6] = g_rendertmp0[i*3+2+j*g_frameWidth*6] = (uint8_t)(stereoimg1->imageData[i*3+0+j*stereoimg1->widthStep]);
				}
		}
		else
		{
			for(j=0;j<g_frameHeight;j++)
				for(i=0;i<g_frameWidth*2;i++)
				{
					g_rendertmp1[i*3+0+j*g_frameWidth*6] = g_rendertmp0[i*3+0+j*g_frameWidth*6] = (uint8_t)(stereoimg1->imageData[i*3+2+j*stereoimg1->widthStep]);
					g_rendertmp1[i*3+1+j*g_frameWidth*6] = g_rendertmp0[i*3+1+j*g_frameWidth*6] = (uint8_t)(stereoimg1->imageData[i*3+1+j*stereoimg1->widthStep]);
					g_rendertmp1[i*3+2+j*g_frameWidth*6] = g_rendertmp0[i*3+2+j*g_frameWidth*6] = (uint8_t)(stereoimg1->imageData[i*3+0+j*stereoimg1->widthStep]);
				}
		}
		
		cvReleaseImage( &stereoimg0 );
		cvReleaseImage( &stereoimg1 );
		
		y_output = out_buffer;
		u_output = out_buffer+inwidth*2*inheight;
		v_output = out_buffer+inwidth*2*inheight*5/4;
		rgb2yuv(rgb_output, y_output, v_output, u_output, inwidth*2, inheight);
		
	}
	else
	{
		
		sz.width = inwidth;
		sz.height = inheight;
		animg0 = cvCreateImage(sz, IPL_DEPTH_8U, 3);

		y_output = out_buffer;
		u_output = out_buffer+inwidth*2*inheight;
		v_output = out_buffer+inwidth*2*inheight*5/4;
		rgb2yuv(rgb_output, y_output, v_output, u_output, inwidth*2, inheight);
		
		switch(g_renderoutputantype)
		{
			case RENDEROUTAN_RE_GR:
				for(j=0;j<inheight;j++)
					for(i=0;i<inwidth;i++)
					{
						#ifdef YTORGB
						animg0->imageData[i*3+2+j*animg0->widthStep] = out_buffer[i+j*2*inwidth]; 
						animg0->imageData[i*3+1+j*animg0->widthStep] = out_buffer[i+j*2*inwidth+inwidth];
						animg0->imageData[i*3+0+j*animg0->widthStep] = out_buffer[i+j*2*inwidth];
						#else
						animg0->imageData[i*3+2+j*animg0->widthStep] = rgb_output[i*3+j*6*inwidth]; 
						animg0->imageData[i*3+1+j*animg0->widthStep] = rgb_output[i*3+1+j*6*inwidth+3*inwidth];
						animg0->imageData[i*3+0+j*animg0->widthStep] = rgb_output[i*3+2+j*6*inwidth];
						#endif
					}
				break;
			case RENDEROUTAN_GR_RE:
				for(j=0;j<inheight;j++)
					for(i=0;i<inwidth;i++)
					{
						#ifdef YTORGB
						animg0->imageData[i*3+2+j*animg0->widthStep] = out_buffer[i+j*2*inwidth+inwidth]; 
						animg0->imageData[i*3+1+j*animg0->widthStep] = out_buffer[i+j*2*inwidth];
						animg0->imageData[i*3+0+j*animg0->widthStep] = out_buffer[i+j*2*inwidth+inwidth];
						#else
						animg0->imageData[i*3+2+j*animg0->widthStep] = rgb_output[i*3+j*6*inwidth+3*inwidth]; 
						animg0->imageData[i*3+1+j*animg0->widthStep] = rgb_output[i*3+1+j*6*inwidth];
						animg0->imageData[i*3+0+j*animg0->widthStep] = rgb_output[i*3+2+j*6*inwidth+3*inwidth];
						#endif
					}
				break;
			case RENDEROUTAN_BR_BL:
				for(j=0;j<inheight;j++)
					for(i=0;i<inwidth;i++)
					{
						#ifdef YTORGB
						animg0->imageData[i*3+2+j*animg0->widthStep] = out_buffer[i+j*2*inwidth]; 
						animg0->imageData[i*3+1+j*animg0->widthStep] = out_buffer[i+j*2*inwidth];
						animg0->imageData[i*3+0+j*animg0->widthStep] = out_buffer[i+j*2*inwidth+inwidth];
						#else
						animg0->imageData[i*3+2+j*animg0->widthStep] = rgb_output[i*3+j*6*inwidth]; 
						animg0->imageData[i*3+1+j*animg0->widthStep] = rgb_output[i*3+1+j*6*inwidth];
						animg0->imageData[i*3+0+j*animg0->widthStep] = rgb_output[i*3+2+j*6*inwidth+3*inwidth];
						#endif
					}
				break;
			case RENDEROUTAN_RE_BL:
			default: 
				for(j=0;j<inheight;j++)
					for(i=0;i<inwidth;i++)
					{
						#ifdef YTORGB
						animg0->imageData[i*3+2+j*animg0->widthStep] = out_buffer[i+j*2*inwidth]; 
						animg0->imageData[i*3+1+j*animg0->widthStep] = out_buffer[i+j*2*inwidth+inwidth];
						animg0->imageData[i*3+0+j*animg0->widthStep] = out_buffer[i+j*2*inwidth+inwidth];
						#else
						animg0->imageData[i*3+2+j*animg0->widthStep] = rgb_output[i*3+j*6*inwidth]; 
						animg0->imageData[i*3+1+j*animg0->widthStep] = rgb_output[i*3+1+j*6*inwidth+3*inwidth];
						animg0->imageData[i*3+0+j*animg0->widthStep] = rgb_output[i*3+2+j*6*inwidth+3*inwidth];
						#endif
					}
				break;
		}
		
		if(g_keyframeflag)
			sprintf(name,"./%s/render/%d_rendersnap_%d.png",cfgparams.projectname,g_sectioncur,g_keyframenumarr[g_frameIndex]+1);
		else
			sprintf(name,"./%s/render/%d_rendersnap_%d.png",cfgparams.projectname,g_sectioncur,g_frameIndex+1);
		cvSaveImage(name, animg0);
		if(g_frameResizeFlag==1)
		{
			sz.width = g_frameWidth;
			sz.height = g_frameHeight;
			animg1 = cvCreateImage(sz, IPL_DEPTH_8U, 3);
			cvResize(animg0, animg1,CV_INTER_CUBIC);
			for(j=0;j<g_frameHeight;j++)
				for(i=0;i<g_frameWidth;i++)
				{
					g_tmpdisplay1[i*3+j*3*g_frameWidth] = animg1->imageData[i*3+2+j*animg1->widthStep]; 
					g_tmpdisplay1[i*3+1+j*3*g_frameWidth] = animg1->imageData[i*3+1+j*animg1->widthStep];
					g_tmpdisplay1[i*3+2+j*3*g_frameWidth] = animg1->imageData[i*3+0+j*animg1->widthStep];
					g_tmpdisplay2[i*3+j*3*g_frameWidth] = animg1->imageData[i*3+2+j*animg1->widthStep]; 
					g_tmpdisplay2[i*3+1+j*3*g_frameWidth] = animg1->imageData[i*3+1+j*animg1->widthStep];
					g_tmpdisplay2[i*3+2+j*3*g_frameWidth] = animg1->imageData[i*3+0+j*animg1->widthStep];
				}
			cvReleaseImage( &animg1 );
		}

		for(j=0;j<inheight;j++)
			for(i=0;i<inwidth;i++)
			{
				rgb_output[i*3+j*3*inwidth] = animg0->imageData[i*3+2+j*animg0->widthStep]; 
				rgb_output[i*3+1+j*3*inwidth] = animg0->imageData[i*3+1+j*animg0->widthStep];
				rgb_output[i*3+2+j*3*inwidth] = animg0->imageData[i*3+0+j*animg0->widthStep];
			}
		
		y_output = out_buffer;
		u_output = out_buffer+inwidth*inheight;
		v_output = out_buffer+inwidth*inheight*5/4;
		rgb2yuv(rgb_output, y_output, v_output, u_output, inwidth, inheight);
		cvReleaseImage( &animg0 );
	}
	
	if(in_depth_buffer) delete []in_depth_buffer;
	if(rgb_output)      delete []rgb_output;
	
	return ;
}
/*
void renderframes(int idx, uint8_t * out_buffer)
{
	int sizergb = IMAGERGB_SIZE(g_frameInWidth,g_frameInHeight);
	uint8_t * in_rgb = new uint8_t [sizergb];
	int sizeyuv = IMAGE_SIZE(g_frameInWidth,g_frameInHeight);
	uint8_t * in_yuv = new uint8_t [sizeyuv];
	SYuvInput inyuv;

	if(g_inputfileformat==INPYUV||g_inputfileformat==INPVID)
	{
		inyuv.y = in_yuv;
		inyuv.u = in_yuv + g_frameInWidth*g_frameInHeight;
		inyuv.v = in_yuv + g_frameInWidth*g_frameInHeight*5/4;
		#ifdef I64
		_fseeki64(g_fileInpYUV, (int64_t)idx*sizeyuv, SEEK_SET);
		#else
		fseek(g_fileInpYUV, (unsigned long)idx*sizeyuv, SEEK_SET);
		#endif
		fread(in_yuv,sizeyuv,1,g_fileInpYUV);
		yuv2rgb(inyuv.y, inyuv.v, inyuv.u, in_rgb, g_frameInWidth, g_frameInHeight);
		DepthRendering(in_rgb, out_buffer, g_frameInWidth, g_frameInHeight, g_renderoutputformat);
	}
	else if(g_inputfileformat==INPIMG) //
	{
		DepthRendering(g_rgborginput, out_buffer, g_frameInWidth, g_frameInHeight, g_renderoutputformat);
	}
	else
	{
		// TODO INPVID
	}

	delete [] in_yuv;
	delete [] in_rgb;
	return ;
}
*/

/* 只render当前帧，为了显示 */
void rendercurframe()
{
	int i,j;
	int width = g_frameInWidth;
	uint8_t * out_yuv = NULL;
	int bakalpha = g_renderalpha;

	if(g_frameMaxNum>0)
	{
		g_renderflag = 1;
		g_enlargeflag = 0;
		g_segeditflag = 0;
		g_mateditflag = 0;
		g_matselflag = 0;
		g_depeditflag = 0;
		g_rendereditflag = 0;
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgRenderMode), TRUE);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgSegmentMode), FALSE);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgNonEdit), TRUE);
		//printf("render: %d\n",g_renderprocmethod);
		
		if(g_renderoutputformat==RENDEROUTLR)
				width = width * 2;
		out_yuv = new uint8_t [IMAGE_SIZE(g_frameInWidth*2,g_frameInHeight)];

		if(g_inputfileformat==INPYUV||g_inputfileformat==INPVID)
		{
			g_renderalpha = 40;
			DepthRenderingDisplay(NULL, out_yuv, g_frameInWidth, g_frameInHeight, g_renderoutputformat);
			g_renderalpha = 30;
			DepthRenderingDisplay(NULL, out_yuv, g_frameInWidth, g_frameInHeight, g_renderoutputformat);
			g_renderalpha = 20;
			DepthRenderingDisplay(NULL, out_yuv, g_frameInWidth, g_frameInHeight, g_renderoutputformat);
			g_renderalpha = 10;
			DepthRenderingDisplay(NULL, out_yuv, g_frameInWidth, g_frameInHeight, g_renderoutputformat);
			g_renderdisplayflag = 1;
			g_renderalpha = bakalpha;
			DepthRenderingDisplay(NULL, out_yuv, g_frameInWidth, g_frameInHeight, g_renderoutputformat);
		}
		else if(g_inputfileformat==INPIMG) //
		{
			g_renderalpha = 40;
			DepthRenderingDisplay(NULL, out_yuv, g_frameInWidth, g_frameInHeight, g_renderoutputformat);
			g_renderalpha = 30;
			DepthRenderingDisplay(NULL, out_yuv, g_frameInWidth, g_frameInHeight, g_renderoutputformat);
			g_renderalpha = 20;
			DepthRenderingDisplay(NULL, out_yuv, g_frameInWidth, g_frameInHeight, g_renderoutputformat);
			g_renderalpha = 10;
			DepthRenderingDisplay(NULL, out_yuv, g_frameInWidth, g_frameInHeight, g_renderoutputformat);
			g_renderdisplayflag = 1;
			g_renderalpha = bakalpha;
			DepthRenderingDisplay(NULL, out_yuv, g_frameInWidth, g_frameInHeight, g_renderoutputformat);
		}
		else
		{
			// do nothing
		}

		for(j=0;j<g_frameHeight;j++)
			for(i=0;i<g_frameWidth;i++)
			{
				g_tmpdisplay3[3*i+0+j*g_frameWidth*3] = g_rgbinput[3*i+0+j*g_frameWidth*3];
				g_tmpdisplay3[3*i+1+j*g_frameWidth*3] = g_rgbinput[3*i+1+j*g_frameWidth*3];
				//g_tmpdisplay3[3*i+2+j*g_frameWidth*3] = g_rgbinput[3*i+2+j*g_frameWidth*3];
			}

		gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
        		IMGWIDTHMAX-g_frameWidth, IMGHEIGHTMAX-g_frameHeight, g_frameWidth, g_frameHeight,
                GDK_RGB_DITHER_MAX, g_tmpdisplay1, g_frameWidth * 3);
		gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
        		IMGWIDTHMAX, IMGHEIGHTMAX-g_frameHeight, g_frameWidth, g_frameHeight,
                GDK_RGB_DITHER_MAX, g_tmpdisplay2, g_frameWidth * 3);
		//gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
        	//	IMGWIDTHMAX-g_frameWidth, IMGHEIGHTMAX, g_frameWidth, g_frameHeight,
              //  GDK_RGB_DITHER_MAX, g_tmpdisplay3, g_frameWidth * 3);
		gdk_draw_gray_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
		          IMGWIDTHMAX, IMGHEIGHTMAX, g_frameWidth, g_frameHeight,
		          GDK_RGB_DITHER_MAX, g_depthdisplay, g_frameWidth);

		delete [] out_yuv;
	}
	else
	{
		//messagebox("Please open a file first.");
	}
	
	return ;
}

void renedit(GtkWidget * button,GtkWidget * data)
{
	char str[32];
	
	if(g_frameMaxNum>0)
	{
		if(g_renderflag==1)
		{
			g_segeditflag = 0;
			g_mateditflag = 0;
			g_matselflag = 0;
			g_depeditflag = 0;
			g_rendereditflag = 1;
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgRenEdit), TRUE);
			g_renderrefflag = 0;

			if(g_rendereditselmodule == RENEDITSELMODECIR)
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgEditCircle), TRUE);
			else
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgEditSquare), TRUE);
			
			sprintf_s(str,"%d", g_rendereditselmodulerad);
			gtk_entry_set_text(GTK_ENTRY(g_wdgEditRadius), str);
		}
		else
		{
			g_rendereditflag = 0;
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgSegEdit), g_segeditflag);
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgMatEdit), g_mateditflag);
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgDepEdit), g_depeditflag);
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgNonEdit), (1-g_segeditflag-g_depeditflag-g_mateditflag));
		}
	}
	else
	{
		//messagebox("Please open a file first.");
	}
	
	return ;
}


void RenderEditSelRegion(int x, int y, int region)
{
	int i, j, ii, jj;
	int dis;
	if(g_renderflag==1)
	{
		//printf("RenderEditSelRegion\n");
		#if 0
		if(g_renderrefflag == 0)
		{
			g_rendereditrefxcenter = x;
			g_rendereditrefycenter = y;
			g_rendereditrefxleft = H3D_MAX(g_rendereditrefxcenter-g_rendereditrefregionrad, 0);
			g_rendereditrefxright = H3D_MIN(g_rendereditrefxcenter+g_rendereditrefregionrad, g_frameWidth-1);
			g_rendereditrefyup = H3D_MAX(g_rendereditrefycenter-g_rendereditrefregionrad, 0);
			g_rendereditrefydown = H3D_MIN(g_rendereditrefycenter+g_rendereditrefregionrad, g_frameHeight-1);
		}
		#endif
		switch(g_rendereditselmodule)
		{
			case RENEDITSELMODESQU:
				for(j=-g_rendereditselmodulerad;j<=g_rendereditselmodulerad;j++)
					for(i=-g_rendereditselmodulerad;i<=g_rendereditselmodulerad;i++)
					{
						ii=x+i;
						jj=y+j;
						ii = H3D_CLIP3(ii,0,g_frameWidth-1);
						jj = H3D_CLIP3(jj,0,g_frameHeight-1);
						#if 1
						if(region==REGIONORG)
						{
							g_renderlabel1[ii+jj*g_frameWidth] = 1;
							g_tmpdisplay1[ii*3+jj*3*g_frameWidth] = 0;
							g_tmpdisplay1[ii*3+1+jj*3*g_frameWidth] = 255;
							g_tmpdisplay1[ii*3+2+jj*3*g_frameWidth] = 0;
							//printf("RenderEditSelRegion1\n");
							gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
					              IMGWIDTHMAX-g_frameWidth, IMGHEIGHTMAX-g_frameHeight, g_frameWidth, g_frameHeight,
					              GDK_RGB_DITHER_MAX, g_tmpdisplay1, g_frameWidth * 3);
						}
						#endif
						if(region==REGIONSEG)
						{
							g_renderlabel2[ii+jj*g_frameWidth] = 1;
							g_tmpdisplay2[ii*3+jj*3*g_frameWidth] = 0;
							g_tmpdisplay2[ii*3+1+jj*3*g_frameWidth] = 255;
							g_tmpdisplay2[ii*3+2+jj*3*g_frameWidth] = 0;
							//printf("RenderEditSelRegion2\n");
							gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
								  IMGWIDTHMAX, IMGHEIGHTMAX-g_frameHeight, g_frameWidth, g_frameHeight,
								  GDK_RGB_DITHER_MAX, g_tmpdisplay2, g_frameWidth * 3);
						}
					}
				break;
			
			case RENEDITSELMODECIR:
			default:
				for(j=-g_rendereditselmodulerad;j<=g_rendereditselmodulerad;j++)
					for(i=-g_rendereditselmodulerad;i<=g_rendereditselmodulerad;i++)
					{
						ii=x+i;
						jj=y+j;
						dis = (int)sqrt((double)(i*i+j*j));
						if(ii>=0&&ii<g_frameWidth&&jj>=0&&jj<g_frameHeight&&dis<=g_rendereditselmodulerad)
						{
							#if 1
							if(region==REGIONORG)
							{
								g_renderlabel1[ii+jj*g_frameWidth] = 1;
								g_tmpdisplay1[ii*3+jj*3*g_frameWidth] = 0;
								g_tmpdisplay1[ii*3+1+jj*3*g_frameWidth] = 255;
								g_tmpdisplay1[ii*3+2+jj*3*g_frameWidth] = 0;
								gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
					              IMGWIDTHMAX-g_frameWidth, IMGHEIGHTMAX-g_frameHeight, g_frameWidth, g_frameHeight,
					              GDK_RGB_DITHER_MAX, g_tmpdisplay1, g_frameWidth * 3);
							}
							#endif
							if(region==REGIONSEG)
							{
								g_renderlabel2[ii+jj*g_frameWidth] = 1;
								g_tmpdisplay2[ii*3+jj*3*g_frameWidth] = 0;
								g_tmpdisplay2[ii*3+1+jj*3*g_frameWidth] = 255;
								g_tmpdisplay2[ii*3+2+jj*3*g_frameWidth] = 0;
								gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
								  IMGWIDTHMAX, IMGHEIGHTMAX-g_frameHeight, g_frameWidth, g_frameHeight,
								  GDK_RGB_DITHER_MAX, g_tmpdisplay2, g_frameWidth * 3);
							}
						}
					}		
				break;
		}
		if(region==REGIONMAN)
		{
			g_renderrefflag = 1; // TODO
			g_rendereditrefxcenter = x;
			g_rendereditrefycenter = y;
			g_rendereditrefxleft = H3D_MAX(g_rendereditrefxcenter-g_rendereditrefregionrad, 0);
			g_rendereditrefxright = H3D_MIN(g_rendereditrefxcenter+g_rendereditrefregionrad, g_frameWidth-1);
			g_rendereditrefyup = H3D_MAX(g_rendereditrefycenter-g_rendereditrefregionrad, 0);
			g_rendereditrefydown = H3D_MIN(g_rendereditrefycenter+g_rendereditrefregionrad, g_frameHeight-1);
			
			for(j=0;j<g_frameHeight;j++)
				for(i=0;i<g_frameWidth;i++)
				{
					if((abs(i-x)==g_rendereditrefregionrad&&abs(j-y)<=g_rendereditrefregionrad)||(abs(j-y)==g_rendereditrefregionrad&&abs(i-x)<=g_rendereditrefregionrad))
					{
						g_tmpdisplay3[i*3+j*g_frameWidth*3] = 255;
						g_tmpdisplay3[i*3+1+j*g_frameWidth*3] = 255;
						g_tmpdisplay3[i*3+2+j*g_frameWidth*3] = 255;
					}
					else
					{
						g_tmpdisplay3[i*3+j*g_frameWidth*3] = g_rgbinputref[i*3+j*g_frameWidth*3];
						g_tmpdisplay3[i*3+1+j*g_frameWidth*3] = g_rgbinputref[i*3+1+j*g_frameWidth*3];
						g_tmpdisplay3[i*3+2+j*g_frameWidth*3] = g_rgbinputref[i*3+2+j*g_frameWidth*3];
					}
				}
				
			gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
	          IMGWIDTHMAX-g_frameWidth, IMGHEIGHTMAX, g_frameWidth, g_frameHeight,
	          GDK_RGB_DITHER_MAX, g_tmpdisplay3, g_frameWidth * 3);
		}
	}
	else
	{
		messagebox(MSGBOX_ERM);
	}
	
	return ;
}



void DrawRenEditContour(int x, int y, int region)
{
	int i, j, ii, jj;
	int dis;

	if(g_renderflag==1&&g_rendereditflag==1)
	{
		switch(g_rendereditselmodule)
		{
			case RENEDITSELMODESQU:
				if(region==REGIONORG)
				{
					for(j=0;j<g_frameHeight;j++)
						for(i=0;i<g_frameWidth;i++)
						{
							if((abs(i-x)==g_rendereditselmodulerad&&abs(j-y)<=g_rendereditselmodulerad)||(abs(j-y)==g_rendereditselmodulerad&&abs(i-x)<=g_rendereditselmodulerad))
							{
								g_tmpdisplay[i*3+j*g_frameWidth*3] = 255;
								g_tmpdisplay[i*3+1+j*g_frameWidth*3] = 0;
								g_tmpdisplay[i*3+2+j*g_frameWidth*3] = 0;
							}
							else
							{
								g_tmpdisplay[i*3+j*g_frameWidth*3] = g_tmpdisplay1[i*3+j*g_frameWidth*3];
								g_tmpdisplay[i*3+1+j*g_frameWidth*3] = g_tmpdisplay1[i*3+1+j*g_frameWidth*3];
								g_tmpdisplay[i*3+2+j*g_frameWidth*3] = g_tmpdisplay1[i*3+2+j*g_frameWidth*3];
							}
						}
						
					gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
			          IMGWIDTHMAX-g_frameWidth, IMGHEIGHTMAX-g_frameHeight, g_frameWidth, g_frameHeight,
			          GDK_RGB_DITHER_MAX, g_tmpdisplay, g_frameWidth * 3);
				}
				if(region==REGIONSEG)
				{
					for(j=0;j<g_frameHeight;j++)
						for(i=0;i<g_frameWidth;i++)
						{
							if((abs(i-x)==g_rendereditselmodulerad&&abs(j-y)<=g_rendereditselmodulerad)||(abs(j-y)==g_rendereditselmodulerad&&abs(i-x)<=g_rendereditselmodulerad))
							{
								g_tmpdisplay[i*3+j*g_frameWidth*3] = 0;
								g_tmpdisplay[i*3+1+j*g_frameWidth*3] = 0;
								g_tmpdisplay[i*3+2+j*g_frameWidth*3] = 255;
							}
							else
							{
								g_tmpdisplay[i*3+j*g_frameWidth*3] = g_tmpdisplay2[i*3+j*g_frameWidth*3];
								g_tmpdisplay[i*3+1+j*g_frameWidth*3] = g_tmpdisplay2[i*3+1+j*g_frameWidth*3];
								g_tmpdisplay[i*3+2+j*g_frameWidth*3] = g_tmpdisplay2[i*3+2+j*g_frameWidth*3];
							}
						}					
					
					gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
							  IMGWIDTHMAX, IMGHEIGHTMAX-g_frameHeight, g_frameWidth, g_frameHeight,
							  GDK_RGB_DITHER_MAX, g_tmpdisplay, g_frameWidth * 3);
				}
				break;
			
			case RENEDITSELMODECIR:
			default:
				if(region==REGIONORG)
				{
					for(j=0;j<g_frameHeight;j++)
						for(i=0;i<g_frameWidth;i++)
						{
							ii = i-x;
							jj = j-y;
							dis = (int)sqrt((double)(ii*ii+jj*jj));
							if(dis==g_rendereditselmodulerad)
							{
								g_tmpdisplay[i*3+j*g_frameWidth*3] = 255;
								g_tmpdisplay[i*3+1+j*g_frameWidth*3] = 0;
								g_tmpdisplay[i*3+2+j*g_frameWidth*3] = 0;
							}
							else
							{
								g_tmpdisplay[i*3+j*g_frameWidth*3] = g_tmpdisplay1[i*3+j*g_frameWidth*3];
								g_tmpdisplay[i*3+1+j*g_frameWidth*3] = g_tmpdisplay1[i*3+1+j*g_frameWidth*3];
								g_tmpdisplay[i*3+2+j*g_frameWidth*3] = g_tmpdisplay1[i*3+2+j*g_frameWidth*3];
							}
						}
						
					gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
			          IMGWIDTHMAX-g_frameWidth, IMGHEIGHTMAX-g_frameHeight, g_frameWidth, g_frameHeight,
			          GDK_RGB_DITHER_MAX, g_tmpdisplay, g_frameWidth * 3);
				}	
				if(region==REGIONSEG)
				{
					for(j=0;j<g_frameHeight;j++)
						for(i=0;i<g_frameWidth;i++)
						{
							ii = i-x;
							jj = j-y;
							dis = (int)sqrt((double)(ii*ii+jj*jj));
							if(dis==g_rendereditselmodulerad)
							{
								g_tmpdisplay[i*3+j*g_frameWidth*3] = 0;
								g_tmpdisplay[i*3+1+j*g_frameWidth*3] = 0;
								g_tmpdisplay[i*3+2+j*g_frameWidth*3] = 255;
							}
							else
							{
								g_tmpdisplay[i*3+j*g_frameWidth*3] = g_tmpdisplay2[i*3+j*g_frameWidth*3];
								g_tmpdisplay[i*3+1+j*g_frameWidth*3] = g_tmpdisplay2[i*3+1+j*g_frameWidth*3];
								g_tmpdisplay[i*3+2+j*g_frameWidth*3] = g_tmpdisplay2[i*3+2+j*g_frameWidth*3];
							}
						}
					
					gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
				          IMGWIDTHMAX, IMGHEIGHTMAX-g_frameHeight, g_frameWidth, g_frameHeight,
				          GDK_RGB_DITHER_MAX, g_tmpdisplay, g_frameWidth * 3);
				}	
		}
	}
	return ;
}

// 对指定点进行区域增长，赋给相邻标记点同一标记值
void RenderRegionGrow(int x, int y, uint8_t * label)
{

	label[x+y*g_frameWidth] = RENDERLABEL;

	// up
	if(y-1>=0&&label[x+(y-1)*g_frameWidth]==1)
	{
		RenderRegionGrow(x, y-1, label);
	}
	// down
	if(y+1<g_frameHeight&&label[x+(y+1)*g_frameWidth]==1)
	{
		RenderRegionGrow(x, y+1, label);
	}
	// left
	if(x-1>=0&&label[x-1+y*g_frameWidth]==1)
	{
		RenderRegionGrow(x-1, y, label);
	}
	// right
	if(x+1<g_frameWidth&&label[x+1+y*g_frameWidth]==1)
	{
		RenderRegionGrow(x+1, y, label);
	}
	
	return ;
}

// 统计标记点数目
int RenderRegionCount(uint8_t * label)
{
	int i;
	int cnt = 0;
	for(i=0;i<g_frameWidth*g_frameHeight;i++)
	{
		if(label[i]==RENDERLABEL)
			cnt ++;
	}
	return cnt;
}

// 选择匹配点
void RenderStartPoint(uint8_t * label, int *px, int *py)
{
	int i, j;
	int mincost = 1000000;
	int cost;
	int cnt;
	int r = g_rendereditmatchmodule+3;

	for(j=0;j<g_frameHeight;j++)
		for(i=0;i<g_frameWidth;i++)
		{
			cnt = 0;
			if(label[i+j*g_frameWidth]==RENDERLABEL)
			{
				if(i-1>=0&&label[i-1+j*g_frameWidth]==0)
					cnt ++;
				if(i+1<g_frameWidth&&label[i+1+j*g_frameWidth]==0)
					cnt ++;
				if(j-1>=0&&label[i+(j-1)*g_frameWidth]==0)
					cnt ++;
				if(j+1<g_frameHeight&&label[i+(j+1)*g_frameWidth]==0)
					cnt ++;
				cost = (4-cnt)*(g_frameWidth+g_frameHeight) + abs(i-g_rendereditrefxcenter) + abs(j-g_rendereditrefycenter);
				if(cost<mincost)
				{
					mincost = cost;
					*px = i;
					*py = j;
				}
			}
		}

	// 保证块匹配时不越界
	if((*px)-r<0) *px = r;
	if((*px)+r>=g_frameWidth) *px = g_frameWidth-r-1;
	if((*py)-r<0) *py = r;
	if((*py)+r>=g_frameHeight) *py = g_frameHeight-r-1;
	
	//printf("startpoint: %d %d\n", *px, *py);
	
	return ;
}

// 进行匹配并赋值
void RenderPointMatching(int px, int py, uint8_t * inp, uint8_t * inorg, uint8_t * ref, uint8_t * label, int region)
{
	int i, j, ri, rj;
	int ii, jj, iii, jjj, iiii, jjjj;
	int r = g_rendereditmatchmodule+3;
	int mincost =  0xFFFFFF;
	int cost, minx, miny, rminx,rminy,rr, rpx, rpy;
	int pos;
	double disparity, alpha;
	double offset;
	
	if(g_renderrefflag == 0)
	{
		alpha = 0.01*g_frameWidth*0.20*g_renderalpha;
		disparity = alpha*((int)g_frames[g_frameIndex].SegDepths[py*g_frameWidth+px]-g_renderforcalplane)/255.0;
		
		if(region==REGIONORG) // left
		{
			offset = px - disparity;
		}
		if(region==REGIONSEG) // right
		{
			offset = px + disparity;
		}
		pos = (int)floor(offset+0.5);
		if(pos<=0) pos=0;
		if(pos>=g_frameWidth-1) pos=g_frameWidth-1;
		g_rendereditrefxcenter = pos;
		g_rendereditrefycenter = py;
		g_rendereditrefxleft = H3D_MAX(g_rendereditrefxcenter-g_rendereditrefregionrad, 0);
		g_rendereditrefxright = H3D_MIN(g_rendereditrefxcenter+g_rendereditrefregionrad, g_frameWidth-1);
		g_rendereditrefyup = H3D_MAX(g_rendereditrefycenter-g_rendereditrefregionrad, 0);
		g_rendereditrefydown = H3D_MIN(g_rendereditrefycenter+g_rendereditrefregionrad, g_frameHeight-1);
	}

	// 块匹配
	for(j=g_rendereditrefyup+r;j<=g_rendereditrefydown-r;j++)
		for(i=g_rendereditrefxleft+r;i<=g_rendereditrefxright-r;i++)
		{
			cost = abs(i-g_rendereditrefxcenter)+abs(j-g_rendereditrefycenter);
			for(jj=0;jj<=2*r;jj++)
				for(ii=0;ii<=2*r;ii++)
				{
					iii = i-r+ii;
					jjj = j-r+jj;
					iiii = px-r+ii;
					jjjj = py-r+jj;
					if(label[iiii+jjjj*g_frameWidth]==0)
						cost += (abs((int)inp[iiii*3+jjjj*g_frameWidth*3]-(int)ref[iii*3+jjj*g_frameWidth*3])+
								abs((int)inp[iiii*3+1+jjjj*g_frameWidth*3]-(int)ref[iii*3+1+jjj*g_frameWidth*3])+
								abs((int)inp[iiii*3+2+jjjj*g_frameWidth*3]-(int)ref[iii*3+2+jjj*g_frameWidth*3]));
				}
			if(cost<mincost)
			{
				mincost = cost;
				minx = i;
				miny = j;
			}
		}

	//printf("mincost: %d, minx: %d ,miny: %d\n", mincost, minx, miny);

	// 修改大图相应块的值
	if(g_frameResizeFlag==1)
	{
		//sprintf(name,"./%s/render/rendersnap_%d.png",cfgparams.projectname,g_frameIndex+1);
		//imgrenderout = cvLoadImage(name);
		rr = (r+1)*g_frameInWidth/g_frameWidth+1;
		rminx = minx*g_frameInWidth/g_frameWidth;
		rminy = miny*g_frameInWidth/g_frameWidth;
		rpx = px*g_frameInWidth/g_frameWidth;
		rpy = py*g_frameInWidth/g_frameWidth;
		for(jj=0;jj<=2*rr;jj++)
			for(ii=0;ii<=2*rr;ii++)
			{
				iii = H3D_MEDIAN(rminx-rr+ii,0,g_frameInWidth-1);
				jjj = H3D_MEDIAN(rminy-rr+jj,0,g_frameInHeight-1);
				i = H3D_MEDIAN(rpx-rr+ii,0,g_frameInWidth-1);
				j = H3D_MEDIAN(rpy-rr+jj,0,g_frameInWidth-1);
				ri = i*g_frameWidth/g_frameInWidth;
				rj = j*g_frameWidth/g_frameInWidth;
				if(label[ri+rj*g_frameWidth]==RENDERLABEL)
				{
					if(region==REGIONORG)
					{
						inorg[i*3+0+j*g_frameInWidth*6] = g_rgborginputref[iii*3+jjj*g_frameInWidth*3];
						inorg[i*3+1+j*g_frameInWidth*6] = g_rgborginputref[iii*3+1+jjj*g_frameInWidth*3];
						inorg[i*3+2+j*g_frameInWidth*6] = g_rgborginputref[iii*3+2+jjj*g_frameInWidth*3];
					}
					if(region==REGIONSEG)
					{
						inorg[i*3+0+g_frameInWidth*3+j*g_frameInWidth*6] = g_rgborginputref[iii*3+jjj*g_frameInWidth*3];
						inorg[i*3+1+g_frameInWidth*3+j*g_frameInWidth*6] = g_rgborginputref[iii*3+1+jjj*g_frameInWidth*3];
						inorg[i*3+2+g_frameInWidth*3+j*g_frameInWidth*6] = g_rgborginputref[iii*3+2+jjj*g_frameInWidth*3];
					}
				}
			}
		//cvSaveImage(name, imgrenderout);
		//cvReleaseImage( &imgrenderout );
	}

	// 用最匹配的块进行赋值
	
	for(jj=0;jj<=2*r;jj++)
		for(ii=0;ii<=2*r;ii++)
		{
			iii = minx-r+ii;
			jjj = miny-r+jj;
			i = px-r+ii;
			j = py-r+jj;
			if(label[i+j*g_frameWidth]==RENDERLABEL)
			{
				inp[i*3+j*g_frameWidth*3] = ref[iii*3+jjj*g_frameWidth*3];
				inp[i*3+1+j*g_frameWidth*3] = ref[iii*3+1+jjj*g_frameWidth*3];
				inp[i*3+2+j*g_frameWidth*3] = ref[iii*3+2+jjj*g_frameWidth*3];
				label[i+j*g_frameWidth] = 0;
			}
		}

	
		
	return ;
}

void RenderBlockMatching(int x, int y, int region)
{
	int cnt = 0;
	int px = 0;
	int py = 0;
	uint8_t * label;
	uint8_t * inp;
	uint8_t * ref = g_rgbinputref;
	uint8_t * inorg = NULL;
	IplImage* imgrenderout;
	char name[FILE_NAME_SIZE];
	int i,j;

	#if 1
	if(region==REGIONORG)
	{
		label = g_renderlabel1;
		inp = g_tmpdisplay1;
	}
	else
	#endif
	if(region==REGIONSEG)
	{
		label = g_renderlabel2;
		inp = g_tmpdisplay2;
	}
	else
	{
		return ;
	}

	if(label[x+y*g_frameWidth]==0)
		return;
	else
	{
		if(g_rendereditstartpoint==RENEDITSELSPMENU)
		{
			// 对指定点进行区域增长，赋给相邻标记点同一标记值
			RenderRegionGrow(x, y, label);
			// 统计标记点数目
			cnt = RenderRegionCount(label);
			//printf("count: %d\n", cnt);
			if(g_frameResizeFlag==1)
			{
				inorg = new uint8_t[g_frameInWidth*g_frameInHeight*6];
				for(j=0;j<g_frameInHeight;j++)
					for(i=0;i<g_frameInWidth*2;i++)
					{
						inorg[i*3+0+j*g_frameInWidth*6] = g_renderorgtmp0[i*3+0+j*g_frameInWidth*6] = g_renderorgtmp1[i*3+0+j*g_frameInWidth*6];
						inorg[i*3+1+j*g_frameInWidth*6] = g_renderorgtmp0[i*3+1+j*g_frameInWidth*6] = g_renderorgtmp1[i*3+1+j*g_frameInWidth*6];
						inorg[i*3+2+j*g_frameInWidth*6] = g_renderorgtmp0[i*3+2+j*g_frameInWidth*6] = g_renderorgtmp1[i*3+2+j*g_frameInWidth*6];
					}
			}
			// 循环，直到标记点数目为零
			while(cnt!=0)
			{
				// 选择匹配顺序
				RenderStartPoint(label, &px, &py);
				// 进行匹配并赋值
				RenderPointMatching(px, py, inp, inorg, ref, label, region);
				// 统计标记点数目
				cnt = RenderRegionCount(label);
				//printf("count1: %d\n", cnt);
				// 显示每步结果
				#if 1
				if(region==REGIONORG) // TODO 删除
				{
					gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
						              IMGWIDTHMAX-g_frameWidth, IMGHEIGHTMAX-g_frameHeight, g_frameWidth, g_frameHeight,
						              GDK_RGB_DITHER_MAX, g_tmpdisplay1, g_frameWidth * 3);
				}
				#endif
				if(region==REGIONSEG)
				{
					gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
									  IMGWIDTHMAX, IMGHEIGHTMAX-g_frameHeight, g_frameWidth, g_frameHeight,
									  GDK_RGB_DITHER_MAX, g_tmpdisplay2, g_frameWidth * 3);
				}
				//printf("cnt :%d\n", cnt);
			}
			if(g_keyframeflag)
				sprintf(name,"./%s/render/%d_rendersnap_%d.png",cfgparams.projectname,g_sectioncur,g_keyframenumarr[g_frameIndex]+1);
			else
				sprintf(name,"./%s/render/%d_rendersnap_%d.png",cfgparams.projectname,g_sectioncur,g_frameIndex+1);
			imgrenderout = cvLoadImage(name);
			if(g_frameResizeFlag==1)
			{
				
				for(j=0;j<g_frameInHeight;j++)
					for(i=0;i<g_frameInWidth*2;i++)
					{
						imgrenderout->imageData[i*3+2+j*imgrenderout->widthStep] = g_renderorgtmp1[i*3+0+j*g_frameInWidth*6] = inorg[i*3+0+j*g_frameInWidth*6];
						imgrenderout->imageData[i*3+1+j*imgrenderout->widthStep] = g_renderorgtmp1[i*3+1+j*g_frameInWidth*6] = inorg[i*3+1+j*g_frameInWidth*6];
						imgrenderout->imageData[i*3+0+j*imgrenderout->widthStep] = g_renderorgtmp1[i*3+2+j*g_frameInWidth*6] = inorg[i*3+2+j*g_frameInWidth*6];
					}
				
				delete []inorg;
			}
			else
			{
				for(j=0;j<g_frameHeight;j++)
					for(i=0;i<g_frameWidth*2;i++)
					{
						imgrenderout->imageData[i*3+2+j*imgrenderout->widthStep] = g_rendertmp1[i*3+0+j*g_frameWidth*6];
						imgrenderout->imageData[i*3+1+j*imgrenderout->widthStep] = g_rendertmp1[i*3+1+j*g_frameWidth*6];
						imgrenderout->imageData[i*3+0+j*imgrenderout->widthStep] = g_rendertmp1[i*3+2+j*g_frameWidth*6];
					}
			}
			cvSaveImage(name, imgrenderout);
			cvReleaseImage( &imgrenderout );
		}
		else // g_rendereditstartpoint==RENEDITSELSPSTRU
		{
			int i, j;
			int m_width; // image width
			int m_height; // image height

			double * m_r;
			double * m_g;
			double * m_b;
			
			int m_top, m_bottom, m_left, m_right; // the rectangle of inpaint area

			int * m_mark;// mark it as source(0) or to-be-inpainted area(-1) or bondary(-2).  区域标记
			double * m_confid;// record the confidence for every pixel  置信度
			double * m_pri; // record the priority for pixels. only boudary pixels will be used
			double * m_gray; // the gray image
			bool * m_source; // whether this pixel can be used as an example texture center

			// 初始化
			m_width = g_frameWidth;
			m_height = g_frameHeight;

			m_mark = new int[m_width*m_height];
			m_confid = new double[m_width*m_height];
			memset( m_confid, 0, m_width*m_height*sizeof(double) );
			m_pri = new double[m_width*m_height];
			m_gray  = new double[m_width*m_height];
			m_source = new bool[m_width*m_height];
			m_r = new double[m_width*m_height];
			m_g = new double[m_width*m_height];
			m_b = new double[m_width*m_height];

			m_top = m_height;  // initialize the rectangle area
		    m_bottom = 0; 
			m_left = m_width;
			m_right = 0;
			
			for(j = 0; j<m_height; j++)
				for(i = 0; i<m_width; i++)
				{
					m_r[j*m_width+i] = inp[i*3+j*g_frameWidth*3];
					m_g[j*m_width+i] = inp[i*3+1+j*g_frameWidth*3];
					m_b[j*m_width+i] = inp[i*3+2+j*g_frameWidth*3];
				}

			
			// 对指定点进行区域增长，赋给相邻标记点同一标记值
			RenderRegionGrow(x, y, label);
			// 修复
			Convert2Gray(m_r, m_g, m_b, m_gray, m_width, m_height);  // convert it to gray image
			DrawBoundary(label, m_mark, m_confid, &m_left, &m_right, &m_bottom, &m_top, m_width, m_height);  // first time draw boundary
			draw_source(m_source, m_mark, m_width, m_height);   // find the patches that can be used as sample texture
			memset(m_pri, 0, m_width*m_height*sizeof(double));
			for(j= m_top; j<=m_bottom; j++)
			    for(i = m_left; i<= m_right; i++)if(m_mark[j*m_width+i] == BOUNDARY)m_pri[j*m_width+i] = priority(i,j,m_confid, m_mark, m_gray, m_width, m_height);//if it is boundary, calculate the priority
			int count=0;
			while(TargetExist(m_mark, m_top, m_bottom, m_left, m_right, m_width)) 
			{
				count++;
				double max_pri = 0;
				int pri_x,pri_y;
				for(j= m_top; j<=m_bottom; j++)
			    for(i = m_left; i<= m_right; i++)
				{
					if(m_mark[j*m_width+i] == BOUNDARY&&m_pri[j*m_width+i]>max_pri)// find the boundary pixel with highest priority
					{
						pri_x = i;
						pri_y = j;
						max_pri = m_pri[j*m_width+i];
					}
				}
				//printf("pri_x is %d, pri_y is %d, amount is %f\n", pri_x, pri_y, max_pri);
				int patch_x, patch_y;
				PatchTexture(pri_x, pri_y, &patch_x, &patch_y, m_mark, m_source, m_r, m_g, m_b, m_width, m_height);  // find the most similar source patch
				update(pri_x, pri_y, patch_x,patch_y, ComputeConfidence(pri_x,pri_y, m_confid, m_width, m_height), label, m_mark, m_gray, m_confid, m_r, m_g, m_b, m_width, m_height);// inpaint this area and update confidence
				UpdateBoundary(pri_x, pri_y, label, m_mark, m_width, m_height); // update boundary near the changed area
				UpdatePri(pri_x, pri_y, m_mark, m_pri, m_confid, m_gray, m_width, m_height);  //  update priority near the changed area

				for(j=0;j<m_height;j++)
					for(i=0;i<m_width;i++)
					{
						inp[i*3+j*g_frameWidth*3] = (uint8_t)m_r[i+j*m_width];
						inp[i*3+1+j*g_frameWidth*3] = (uint8_t)m_g[j*m_width+i];
						inp[i*3+2+j*g_frameWidth*3] = (uint8_t)m_b[j*m_width+i];
					}
				if(region==REGIONSEG)
				{
					gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
									  IMGWIDTHMAX, IMGHEIGHTMAX-g_frameHeight, g_frameWidth, g_frameHeight,
									  GDK_RGB_DITHER_MAX, g_tmpdisplay2, g_frameWidth * 3);
				}
			
			}

			// 结束
			if(m_mark)delete m_mark;
			if(m_source)delete m_source;
			if(m_r)delete m_r;
			if(m_g)delete m_g;
			if(m_b)delete m_b;
		}
	}

	for(j=0;j<g_frameHeight;j++)
		for(i=0;i<g_frameWidth*2;i++)
		{
			g_rendertmp0[i*3+0+j*g_frameWidth*6] = g_rendertmp1[i*3+0+j*g_frameWidth*6];
			g_rendertmp0[i*3+1+j*g_frameWidth*6] = g_rendertmp1[i*3+1+j*g_frameWidth*6];
			g_rendertmp0[i*3+2+j*g_frameWidth*6] = g_rendertmp1[i*3+2+j*g_frameWidth*6];
		}

	// 保存当前-> tmp1
	for(j=0;j<g_frameHeight;j++)
		for(i=0;i<g_frameWidth;i++)
		{
			g_rendertmp1[i*3+0+j*g_frameWidth*6] = g_tmpdisplay1[i*3+0+j*3*g_frameWidth];
			g_rendertmp1[i*3+1+j*g_frameWidth*6] = g_tmpdisplay1[i*3+1+j*3*g_frameWidth];
			g_rendertmp1[i*3+2+j*g_frameWidth*6] = g_tmpdisplay1[i*3+2+j*3*g_frameWidth];
			g_rendertmp1[i*3+0+g_frameWidth*3+j*g_frameWidth*6] = g_tmpdisplay2[i*3+0+j*3*g_frameWidth];
			g_rendertmp1[i*3+1+g_frameWidth*3+j*g_frameWidth*6] = g_tmpdisplay2[i*3+1+j*3*g_frameWidth];
			g_rendertmp1[i*3+2+g_frameWidth*3+j*g_frameWidth*6] = g_tmpdisplay2[i*3+2+j*3*g_frameWidth];
		}
	
	return ;
}

void RenderRowPull(RGB *outDest, const RGB *inSource, int inLastSrcX, int inSrcToZ16,
	const uint8_t *inDepth, int inLastDepthX,
	int inX0, int inX1,
	int inDisparityScale, int inFocalZ, uint16_t *Sx)
{
	// Z16 is the sample co-ordinate doe samplig the depth
	// x loops from inX0, but we will notionally add on a half-pixel, going from
	//  inX0+0.5 ... inX1+0.5.  We can take this into account with the (inSrcToZ16>>1)
	// When sampling the depth, we will take the least-significant bits as fractional,
	// however, this would mean that a value of 0 would be all pixel 0 and none of pixel 1,
	//  where we actually want half of pixel -1 and half of pixel 0.
	//  We subtracr 0x8000 to compensate.
	int z16  = inX0 * inSrcToZ16 + (inSrcToZ16>>1) - 0x8000;

	int focal_z6 = inFocalZ << 6;
	int jitter = inDisparityScale > 0 ? SOURCE_LEFT_JITTER : 0;

	// Loop through columns.
	for (int x = inX0; x < inX1; x++)
	{
		int disparity_16;

		int z0 = (z16>>16);
		if (z0<0)
		{
			disparity_16 = (inDepth[0] - inFocalZ)*inDisparityScale;
		}
		else if (z0>=inLastDepthX)
		{
			disparity_16 = (inDepth[inLastDepthX] - inFocalZ)*inDisparityScale;
		}
		else
		{
			int w1 = z16 & 0xffff;
			int depth_16 = (inDepth[z0]<<16) + w1*(inDepth[z0+1]-inDepth[z0]);
			// Perform this calcualtion with 6 extra bits of precision, and then remove
			// them at the end.  This is so we don't overflow the intermediate calculation.
			disparity_16 = ( ( (depth_16>>10) - focal_z6)*inDisparityScale ) >> 6;
		}

		int src_16 = (x << 16) + jitter - disparity_16;
		// Reduce to integer from fixed 16.
		int sx0 = src_16 >> 16;
		if (sx0 >= inLastSrcX)
		{
			// If sx0 can't be interpolated as it is the last pixel in the row then
			// just use it directly.
			outDest[x] = inSource[sx0];
			int sx_interp = sx0<<2; // 2 fractional bits
			Sx[x] = sx_interp&0xff;

		}
		else
		{
			// The normal case requires interpolation:
			const RGB &s0 = inSource[sx0];
			const RGB &s1 = inSource[sx0+1];

			// Remember the index of the pixel to the right
			int sx1 = sx0 + 1;
			// Fixed 16 pixel weighting - SOURCE_INTERP_MASK may reduce this to 2 bits...
			int weight1 = src_16 & SOURCE_INTERP_MASK;

			int sx_interp = (sx0<<2) + (weight1>>14); // with 2 fractional bits
			Sx[x] = sx_interp&0xff;

			// Straight forward linear interpolation.
			outDest[x].r = s0.r + (( (s1.r-s0.r) * weight1) >> 16);
			outDest[x].g = s0.g + (( (s1.g-s0.g) * weight1) >> 16);
			outDest[x].b = s0.b + (( (s1.b-s0.b) * weight1) >> 16);
		}

		z16 += inSrcToZ16;
	}
}

/**
* inSX0, inSX1, inDX0 and inDX1 are all in fixed 16
* inLastSX is the last valid source image pixel to read from for this row.
* inX0 and inX1 specify the valid column range to write to (we don't write
* where there are black bars).
*/
void RenderStrip(const RGB * inSrc, int inSX0, int inSX1, int inLastSX,
	RGB *outDest, int inDX0, int inDX1, int inX0, int inX1)
{
	// Convert to integer pixels (with fair rounding).
	int dx0 = (inDX0 + 0x8000) >> 16;
	int dx1 = (inDX1 + 0x8000) >> 16;

	// Simple abort cases.
	if (dx1<=dx0 || dx1<=inX0 || dx0>=inX1)
	{
		return;
	}

	// Change in source, per change in destination pixel
	// This equation will overflow, so write it a bit differently...
	//int ds_dd = ((inSX1-inSX0)<<16)/(inDX1-inDX0);
	int denom = (inDX1 - inDX0) >> 6;
	int ds_dd = denom==0 ? 0 : ((inSX1 - inSX0) << 10) / denom;

	// Simple clamp to allowable pixel columns.
	if (dx1 > inX1) dx1 = inX1;
	if (dx0 < inX0) dx0 = inX0;

	// Shift the source to the centre of the first pixel.
	// This equation may overflow, so re-write it...
	// int sx_16 = inSX0 + (((dx0 << 16) + 0x8000 - inDX0) * ds_dd) >> 16;
	// This calculates the fractional pixel distance between the center
	// of the pixel we are going to write to and the fractional pixel position
	// we actually wanted to write to. It then multiplies this by the change in
	// source x per destination pixel to give us the correct source x for the
	// center of the pixel.
	int sx_16 = inSX0 + (((dx0 << 16) + 0x8000 - inDX0) >> 8) * (ds_dd >> 8);

	// Loop through each destination pixel.
	for (int x = dx0; x < dx1; x++)
	{
		int s_int = sx_16 >> 16;
		if (s_int >= inLastSX)
		{
			// Special case of trying to interpolate with a source pixel that doesn't exist.
			// Note with careful maths this case may not be required.
			outDest[x] = inSrc[inLastSX];
		}
		else
		{
			// Fixed 16 pixel weighting - SOURCE_INTERP_MASK may reduce this to 2 bits...
			int w1 = sx_16 & SOURCE_INTERP_MASK;

			const RGB &s0 = inSrc[s_int];
			const RGB &s1 = inSrc[s_int+1];

			// Straight forward interpolation.
			outDest[x].r = s0.r + (( (s1.r-s0.r) * w1) >> 16);
			outDest[x].g = s0.g + (( (s1.g-s0.g) * w1) >> 16);
			outDest[x].b = s0.b + (( (s1.b-s0.b) * w1) >> 16);
		}
		// Advance the source pixel coordinate ready for the next destination pixel.
		sx_16 += ds_dd;
	}
}

/**
* inSX0, inSX1, inDX0 and inDX1 are all in fixed 16
* inLastSX is the last valid source image pixel to read from for this row.
* inX0 and inX1 specify the valid column range to write to (we don't write
* where there are black bars).
*/
void RenderStrip1(const RGB * inSrc, int inSX0, int inSX1, int inLastSX,
	RGB *outDest, int inDX0, int inDX1, int inX0, int inX1, int flag, int * flags)
{
	// Convert to integer pixels (with fair rounding).
	int dx0 = (inDX0 + 0x8000) >> 16;
	int dx1 = (inDX1 + 0x8000) >> 16;

	// Simple abort cases.
	if (dx1<=dx0 || dx1<=inX0 || dx0>=inX1)
	{
		return;
	}

	// Change in source, per change in destination pixel
	// This equation will overflow, so write it a bit differently...
	//int ds_dd = ((inSX1-inSX0)<<16)/(inDX1-inDX0);
	int denom = (inDX1 - inDX0) >> 6;
	int ds_dd = denom==0 ? 0 : ((inSX1 - inSX0) << 10) / denom;

	// Simple clamp to allowable pixel columns.
	if (dx1 > inX1) dx1 = inX1;
	if (dx0 < inX0) dx0 = inX0;

	// Shift the source to the centre of the first pixel.
	// This equation may overflow, so re-write it...
	// int sx_16 = inSX0 + (((dx0 << 16) + 0x8000 - inDX0) * ds_dd) >> 16;
	// This calculates the fractional pixel distance between the center
	// of the pixel we are going to write to and the fractional pixel position
	// we actually wanted to write to. It then multiplies this by the change in
	// source x per destination pixel to give us the correct source x for the
	// center of the pixel.
	int sx_16 = inSX0 + (((dx0 << 16) + 0x8000 - inDX0) >> 8) * (ds_dd >> 8);

	// Loop through each destination pixel.
	for (int x = dx0; x < dx1; x++)
	{
		int s_int = sx_16 >> 16;
		if (s_int >= inLastSX)
		{
			// Special case of trying to interpolate with a source pixel that doesn't exist.
			// Note with careful maths this case may not be required.
			outDest[x] = inSrc[inLastSX];
		}
		else
		{
			// Fixed 16 pixel weighting - SOURCE_INTERP_MASK may reduce this to 2 bits...
			int w1 = sx_16 & SOURCE_INTERP_MASK;

			const RGB &s0 = inSrc[s_int];
			const RGB &s1 = inSrc[s_int+1];

			// Straight forward interpolation.
			outDest[x].r = s0.r + (( (s1.r-s0.r) * w1) >> 16);
			outDest[x].g = s0.g + (( (s1.g-s0.g) * w1) >> 16);
			outDest[x].b = s0.b + (( (s1.b-s0.b) * w1) >> 16);

			if(flag==0)
			{
				flags[x] = 0;
			}
			else
			{
				flags[x] = 1;
			}
		}
		// Advance the source pixel coordinate ready for the next destination pixel.
		sx_16 += ds_dd;
	}
}

void RenderSoften(RGB *outDest, int * flags, int width)
{
	int i;

	RGB * tmp = new RGB[width];
	
	for(i=0;i<width;i++)
	{
		tmp[i].r = outDest[i].r;
		tmp[i].g = outDest[i].g;
		tmp[i].b = outDest[i].b;
	}

	for(i=3;i<width-3;i++)
	{
		if(flags[i]==1)
		{
			if(g_renderedgesoftenmethod==RENDEREDGESOFTENMID)
			{
				tmp[i-2].r = H3D_MEDIAN(outDest[i-3].r, outDest[i-2].r, outDest[i-1].r);
				tmp[i-2].g = H3D_MEDIAN(outDest[i-3].g, outDest[i-2].g, outDest[i-1].g);
				tmp[i-2].b = H3D_MEDIAN(outDest[i-3].b, outDest[i-2].b, outDest[i-1].b);
				tmp[i-1].r = H3D_MEDIAN(outDest[i-2].r, outDest[i-1].r, outDest[i].r);
				tmp[i-1].g = H3D_MEDIAN(outDest[i-2].g, outDest[i-1].g, outDest[i].g);
				tmp[i-1].b = H3D_MEDIAN(outDest[i-2].b, outDest[i-1].b, outDest[i].b);
				tmp[i].r = H3D_MEDIAN(outDest[i-1].r, outDest[i].r, outDest[i+1].r);
				tmp[i].g = H3D_MEDIAN(outDest[i-1].g, outDest[i].g, outDest[i+1].g);
				tmp[i].b = H3D_MEDIAN(outDest[i-1].b, outDest[i].b, outDest[i+1].b);
				tmp[i+1].r = H3D_MEDIAN(outDest[i].r, outDest[i+1].r, outDest[i+2].r);
				tmp[i+1].g = H3D_MEDIAN(outDest[i].g, outDest[i+1].g, outDest[i+2].g);
				tmp[i+1].b = H3D_MEDIAN(outDest[i].b, outDest[i+1].b, outDest[i+2].b);
				tmp[i+2].r = H3D_MEDIAN(outDest[i+1].r, outDest[i+2].r, outDest[i+3].r);
				tmp[i+2].g = H3D_MEDIAN(outDest[i+1].g, outDest[i+2].g, outDest[i+3].g);
				tmp[i+2].b = H3D_MEDIAN(outDest[i+1].b, outDest[i+2].b, outDest[i+3].b);
			}
			else
			{
				tmp[i-2].r = ((int)outDest[i-3].r+outDest[i-2].r+outDest[i-1].r)/3;
				tmp[i-2].g = ((int)outDest[i-3].g+outDest[i-2].g+outDest[i-1].g)/3;
				tmp[i-2].b = ((int)outDest[i-3].b+outDest[i-2].b+outDest[i-1].b)/3;
				tmp[i-1].r = ((int)outDest[i-2].r+outDest[i-1].r+outDest[i].r)/3;
				tmp[i-1].g = ((int)outDest[i-2].g+outDest[i-1].g+outDest[i].g)/3;
				tmp[i-1].b = ((int)outDest[i-2].b+outDest[i-1].b+outDest[i].b)/3;
				tmp[i].r = ((int)outDest[i-1].r+outDest[i].r+outDest[i+1].r)/3;
				tmp[i].g = ((int)outDest[i-1].g+outDest[i].g+outDest[i+1].g)/3;
				tmp[i].b = ((int)outDest[i-1].b+outDest[i].b+outDest[i+1].b)/3;
				tmp[i+1].r = ((int)outDest[i].r+outDest[i+1].r+outDest[i+2].r)/3;
				tmp[i+1].g = ((int)outDest[i].g+outDest[i+1].g+outDest[i+2].g)/3;
				tmp[i+1].b = ((int)outDest[i].b+outDest[i+1].b+outDest[i+2].b)/3;
				tmp[i+2].r = ((int)outDest[i+1].r+outDest[i+2].r+outDest[i+3].r)/3;
				tmp[i+2].g = ((int)outDest[i+1].g+outDest[i+2].g+outDest[i+3].g)/3;
				tmp[i+2].b = ((int)outDest[i+1].b+outDest[i+2].b+outDest[i+3].b)/3;
			}
		}
	}

	for(i=0;i<width;i++)
	{
		outDest[i].r = tmp[i].r;
		outDest[i].g = tmp[i].g;
		outDest[i].b = tmp[i].b;
	}

	delete []tmp;
	
	return ;
}

void RenderRowPush(RGB *outDest, const RGB *inSource, int inLastSrcX,
	const uint8_t *inDepth, int inZWidth, int inZtoSrc16,
	int inX0, int inX1,
	int inDisparityScale, int inFocalZ)
{
	int *softenflags = new int[inZWidth];
	// Create a virtual z-grid.
	// The source texture matches the z texture at the left edge and
	// the right edge. So Sx(z) = z * inZtoSrc16 (16-bit fixes)

	// Dest pixels are sampled at their centres (ie, +0.5), and at 0 disparity
	// the left edges match the Z.
	// double z_to_dest = (double)1/mDepth->Width();

	// Change direction to allow closer pixels to overwrite further pixels.
	// NOTE: If applying gradient limiting with a strength equal to or
	// greater than the gradient of the push then we can render left and
	// right view the same way as there will never be overlap.
	if (inDisparityScale < 0)
	{
		// Push a small strip of source pixels for each pixel of depth,
		// working from the left to the right.

		// Because we offset the z grid by half a z pixel we need to render the
		// first few pixels with with constant depth.
		int src_x0 = 0;
		int src_x1 = inZtoSrc16 / 2;
		int dx0_16 = src_x0 + (inDepth[0] - inFocalZ) * inDisparityScale;
		int dx1_16 = src_x1 + (inDepth[0] - inFocalZ) * inDisparityScale;
		int flag = 0;

		// Render the left most strip.
		RenderStrip(inSource, src_x0, src_x1, inLastSrcX,
			outDest, dx0_16, dx1_16, inX0, inX1);

		// Middle pixels have different depths...
		for (int z = 1; z < inZWidth; z++)
		{
			// Advance to the next depth map pixel.
			dx0_16 = dx1_16;
			src_x0 = src_x1;
			// Advance the source image coordinate by the distance of one depth map pixel.
			src_x1 += inZtoSrc16;
			// Calculate which destination x coordinate this depth map coordinate maps to. 
			// z * inZtoSrc16 + z_scale16 / 2 gives the base x coordinate which we then
			// add the pixel shift due to depth.
			dx1_16 = src_x1 + (inDepth[z] - inFocalZ) * inDisparityScale;

			if(g_renderedgesoftenmethod)
			{
				if(abs(inDepth[z]-inDepth[z-1])>g_renderedgesoftenth)
					flag = 1;
				else
					flag = 0;
			}

			RenderStrip1(inSource, src_x0, src_x1, inLastSrcX,
				outDest, dx0_16, dx1_16, inX0, inX1, flag, softenflags);

			
		}

		// Last half depth pixel has constant depth 
		{
			// Advance to the next depth map pixel. 
			dx0_16 = dx1_16;
			src_x0 = src_x1;
			// Advance the source image coordinate by the distance of one depth map pixel.
			src_x1 += inZtoSrc16 / 2;
			dx1_16 = inZWidth * inZtoSrc16 + (inDepth[inZWidth - 1] - inFocalZ) * inDisparityScale;

			RenderStrip(inSource, src_x0, src_x1, inLastSrcX,
				outDest, dx0_16, dx1_16, inX0, inX1);
		}

		if(g_renderedgesoftenmethod)
		{
			RenderSoften(outDest, softenflags, inZWidth);
		}
		
	}
	else
	{
		int flag = 0;
		int dx0_16 = inZtoSrc16*inZWidth - inZtoSrc16/2 +
			(inDepth[inZWidth-1]-inFocalZ)*inDisparityScale;
		int dx1_16 = inZtoSrc16*inZWidth +
			(inDepth[inZWidth-1]-inFocalZ)*inDisparityScale;

		// Add jitter for left view
		int src_x0 = inZWidth*inZtoSrc16 - inZtoSrc16/2 + SOURCE_LEFT_JITTER;
		int src_x1 = inZtoSrc16*inZWidth + SOURCE_LEFT_JITTER;

		RenderStrip(inSource,src_x0,src_x1, inLastSrcX,
			outDest,dx0_16,dx1_16, inX0,inX1 );

		// Middle pixels have different depths...
		for (int z = inZWidth - 2; z >= 0; z--)
		{
			dx1_16 = dx0_16;
			src_x1 = src_x0;

			src_x0 -= inZtoSrc16;
			dx0_16 = z*inZtoSrc16 + inZtoSrc16/2 +
				(inDepth[z]-inFocalZ)*inDisparityScale;

			if(g_renderedgesoftenmethod)
			{
				if(abs(inDepth[z]-inDepth[z+1])>g_renderedgesoftenth)
					flag = 1;
				else
					flag = 0;
			}

			RenderStrip1(inSource,src_x0,src_x1, inLastSrcX,
				outDest,dx0_16,dx1_16, inX0,inX1, flag, softenflags);

		}

		// Last half depth pixel has constant depth 
		dx1_16 = dx0_16;
		src_x1 = src_x0;

		src_x0 = 0;
		dx0_16 = 0 + (inDepth[0]-inFocalZ)*inDisparityScale;

		RenderStrip(inSource,src_x0,src_x1, inLastSrcX,
			outDest,dx0_16,dx1_16, inX0,inX1 );

		if(g_renderedgesoftenmethod)
		{
			RenderSoften(outDest, softenflags, inZWidth);
		}
	}

	delete []softenflags;
	
}


/**
* Render view is the main rendering function. It:
* - Calculates the disparity to use.
* - Determines the width of the black bars and renders them
* - Calls RenderRowPush or RenderRowPull for each row of the
*   destination view to actually render the image.
*/
//Side By Side version
void Render3DImage( const uint8_t * inSource, const uint8_t * inDepth,
	uint8_t * outRender, int inSceneDepth_8bit, int inFocalZ, int w, int h)
{
	// Cache resolution.
	static uint16_t *SxLeft=NULL;
	static uint16_t *SxRight=NULL;

	//int i;
	// Set output image size.
	//outRender.SetSize(w, h);

	//if (inDepth.Height()!=inSource.Height())
		//Error("Render3DImage: Source and depth should have the same height\n");

	if (SxLeft == NULL) {
		SxLeft = (uint16_t *)malloc(sizeof(uint16_t)*w*h);
	}

	if (SxRight == NULL) {
		SxRight = (uint16_t *)malloc(sizeof(uint16_t)*w*h);
	}

	// Row width ...
	int depth_w = w;
	// Add extra "1" to not go out-of-bounds when doing bi-linear sampling
	//std::vector<uint8_t> depth(depth_w + 1);


	// Disparity, fixed-8
	// inSign denotes left vs right view
	// Gives the maximal pixel shift (in dest pixels).
	//int disparity_scale = inSceneDepth_8bit * w / 20;
	int disparity_scale = inSceneDepth_8bit * w / 5;

	// Remember the first and last destination pixel to render.
	int x0 = 0;
	int x1 = w;


	// Maximum extent of disparity ...
	int d0 = (disparity_scale * (0-inFocalZ)) >> 16 ;
	int d1 = (disparity_scale * (255-inFocalZ) + SOURCE_LEFT_JITTER) >> 16 ;

	// Symetrical black bars:
	x0 = H3D_MAX(abs(d0), abs(d1));
	x1 = w - x0;

	// For push renderer ...
	int depth_to_src16 = (w<<16)/w;

	// For pull renderer ...
	int src_to_depth16 = (w<<16)/w;

	RGB *left_dest = new RGB[w];
	RGB *right_dest = new RGB[w];
	
	// Loop through each row of the destination view.
	for (int y = 0; y < h; y++)
	{
		const uint8_t *depth = (uint8_t *)(inDepth+y*w);
		//RGB *dest = (RGB *)(outRender+y*w*3);

		// Render black in the black bars.
		// Left black bar.
		memset(left_dest, 0, x0 * sizeof(RGB));
		// Right black bar.
		memset(&left_dest[x1], 0, (w-x1) * sizeof(RGB));

		// Render black in the black bars.
		// Left black bar.
		memset(right_dest, 0, x0 * sizeof(RGB));
		// Right black bar.
		memset(&right_dest[x1], 0, (w-x1) * sizeof(RGB));

		int factor;

		// Render the row.
		if (g_rendermethod==RENDERPULL)
		{
			factor = disparity_scale;
			RenderRowPull(left_dest, (RGB*)(inSource+y*w*3), w-1, src_to_depth16,
				depth, w-1,
				x0, x1,
				factor, inFocalZ, &SxLeft[y*w] );

			factor = -disparity_scale;
			RenderRowPull(right_dest, (RGB*)(inSource+y*w*3), w-1, src_to_depth16,
				depth, w-1,
				x0, x1,
				factor, inFocalZ, &SxRight[y*w] );
		}
		else
		{
			factor = disparity_scale;
			RenderRowPush(left_dest, (RGB*)(inSource+y*w*3), w-1,
				depth, w, depth_to_src16,
				x0, x1,
				factor, inFocalZ );

			factor = -disparity_scale;
			RenderRowPush(right_dest, (RGB*)(inSource+y*w*3), w-1,
				depth, w, depth_to_src16,
				x0, x1,
				factor, inFocalZ );

		}
		//Combing left and right into a side by side viewer
		#if 0
		for(int x = 0; x < w/2; x++)
		{
			outRender[y*w*3+x*3] = (left_dest[x*2].r + left_dest[x*2+1].r)/2;	
			outRender[y*w*3+x*3+1] = (left_dest[x*2].g + left_dest[x*2+1].g)/2;	
			outRender[y*w*3+x*3+2] = (left_dest[x*2].b + left_dest[x*2+1].b)/2;	

			outRender[y*w*3+(x+w/2)*3] = (right_dest[x*2].r + right_dest[x*2+1].r)/2;		
			outRender[y*w*3+(x+w/2)*3+1] = (right_dest[x*2].g + right_dest[x*2+1].g)/2;	
			outRender[y*w*3+(x+w/2)*3+2] = (right_dest[x*2].b + right_dest[x*2+1].b)/2;	
		}
		#endif
		for(int x = 0; x < w; x++)
		{
			outRender[y*w*6+x*3] = left_dest[x].r;	
			outRender[y*w*6+x*3+1] = left_dest[x].g;	
			outRender[y*w*6+x*3+2] = left_dest[x].b;	

			outRender[y*w*6+(x+w)*3] = right_dest[x].r;		
			outRender[y*w*6+(x+w)*3+1] = right_dest[x].g;	
			outRender[y*w*6+(x+w)*3+2] = right_dest[x].b;	
		}

	}

	if(left_dest) delete []left_dest;
	if(right_dest) delete []right_dest;

	//printArray("Sx_left_1920x1080.yuv", SxLeft, inSource.Width(), inSource.Height(), 1);
	//printArray("Sx_right_1920x1080.yuv", SxRight, inSource.Width(), inSource.Height(), 1);
}

