
#include "CSuperpixelSeg.h"

//int g_histdiffthreshold = 4096*10;

int g_frameIndex = 0;
int g_frameMaxNum = 0;
int g_frameInWidth = IMGWIDTHDEF;
int g_frameInHeight = IMGHEIGHTDEF;
int g_frameWidth = IMGWIDTHDEF;
int g_frameHeight = IMGHEIGHTDEF;
int g_frameBaseWidth = IMGWIDTHDEF;
int g_frameBaseHeight = IMGHEIGHTDEF;
int g_frameRenderRef = 0;
int g_frameResizeFlag = 0;

SFrameInfo *g_frames;

// Render Ref frame
void getrefframergb(SFrameJump data)
{
	int i;
	int size = g_frameWidth*g_frameHeight;

	char str[512];

	if(g_frameMaxNum!=0) // 表示已经读取文件了
	{		
		switch(data.jumpmode)
		{
			case JMPMODEFIRST:
				g_frameRenderRef = 0;
				break;
			case JMPMODELAST:
				g_frameRenderRef = g_frameMaxNum-1;
				break;
			case JMPMODECUR:
				break;
			case JMPMODEPREV:
				g_frameRenderRef--;
				if(g_frameRenderRef<0)
					g_frameRenderRef = 0;
				break;
			case JMPMODENEXT:
				g_frameRenderRef++;
				if(g_frameRenderRef>=g_frameMaxNum)
					g_frameRenderRef = g_frameMaxNum-1;
				break;
			case JMPMODEPREVK:
				break;
			case JMPMODENEXTK:
				break;
			default:
				break;
		}

		sprintf_s(str,"   Section %d of %d Time %ds to %ds Coordinate Position X: 0 Y: 0   /   Color R: 0 G: 0 B: 0   /   Layer: 0 Base: 0  /   Depth: 0  /  Render Ref: %3d  ", g_sectioncur, g_sectionnum, g_timestart+(g_sectioncur-1)*g_sectionduration, g_timestart+g_sectioncur*g_sectionduration, g_frameRenderRef+1);
		gtk_label_set_text((GtkLabel *)g_wdgInfo,str);

		if(g_inputfileformat==INPYUV||g_inputfileformat==INPVID)
		{
			#ifdef I64
			_fseeki64(g_fileInpYUV, (int64_t)g_frameRenderRef*IMAGE_SIZE(g_frameInWidth,g_frameInHeight), SEEK_SET);
			#else
			fseek(g_fileInpYUV, (unsigned long)g_frameRenderRef*IMAGE_SIZE(g_frameInWidth,g_frameInHeight), SEEK_SET);
			#endif
		
			if(g_frameResizeFlag==1)
			{
				fread(g_yuvorginputref.y,(g_frameInWidth*g_frameInHeight),1,g_fileInpYUV);
				fread(g_yuvorginputref.u,(g_frameInWidth*g_frameInHeight)>>2,1,g_fileInpYUV);
				fread(g_yuvorginputref.v,(g_frameInWidth*g_frameInHeight)>>2,1,g_fileInpYUV);
				yuvresize(g_yuvorginputref, g_yuvinputref, g_frameInWidth, g_frameInHeight, g_frameWidth, g_frameHeight);
				yuv2rgb(g_yuvorginputref.y, g_yuvorginputref.v, g_yuvorginputref.u, g_rgborginputref, g_frameInWidth, g_frameInHeight);
			}
			else
			{
				fread(g_yuvinputref.y,size,1,g_fileInpYUV);
				fread(g_yuvinputref.u,size>>2,1,g_fileInpYUV);
				fread(g_yuvinputref.v,size>>2,1,g_fileInpYUV);
			}
			
			yuv2rgb(g_yuvinputref.y, g_yuvinputref.v, g_yuvinputref.u, g_rgbinputref, g_frameWidth, g_frameHeight);
		}		

		for(i=0;i<3*g_frameWidth*g_frameHeight;i++)
			g_tmpdisplay3[i] = g_rgbinputref[i];

		// render参考标记清零
		g_renderrefflag = 0;
		
		// ③区显示参考帧
		gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
              IMGWIDTHMAX-g_frameWidth, IMGHEIGHTMAX, g_frameWidth, g_frameHeight,
              GDK_RGB_DITHER_MAX, g_tmpdisplay3, g_frameWidth * 3);
		

	}
	else
	{
		//messagebox("Please open a file first.");
	}
	return ;

}

/*Key Frame*/
SKeyFrameList g_keyframelist;
int g_keyframenum = 1;

void firstframe(GtkWidget * button,GtkWidget * data)
{
	SFrameJump jmp;
	
	jmp.framenum = 1;
	jmp.jumpmode = JMPMODEFIRST;
		
	if(g_rendereditflag==0)
	{
		jumpframe(NULL, &jmp);
	}
	else // g_renderflag = 1
	{
		getrefframergb(jmp);
	}

	return ;
}

void lastframe(GtkWidget * button,GtkWidget * data)
{
	SFrameJump jmp;
	
	jmp.framenum = g_frameMaxNum;
	jmp.jumpmode = JMPMODELAST;
		
	if(g_rendereditflag==0)
	{
		jumpframe(NULL, &jmp);
	}
	else // g_renderflag = 1
	{
		getrefframergb(jmp);
	}

	return ;
}


void previousframe(GtkWidget * button,GtkWidget * data)
{
	SFrameJump jmp;

	
	jmp.framenum = g_frameIndex;
	jmp.jumpmode = JMPMODEPREV;
	
	if(g_rendereditflag==0)
	{
		jumpframe(NULL, &jmp);
	}
	else // g_renderflag = 1
	{
		getrefframergb(jmp);
	}

	return ;
}

	
void nextframe(GtkWidget * button,GtkWidget * data)
{
	SFrameJump jmp;
	
	jmp.framenum = g_frameIndex;
	jmp.jumpmode = JMPMODENEXT;
		
	if(g_rendereditflag==0)
	{
		jumpframe(NULL, &jmp);
	}
	else // g_renderflag = 1
	{
		getrefframergb(jmp);
	}
	
	return ;
}

void previouskeyframe(GtkWidget * button,GtkWidget * data)
{
	SFrameJump jmp;
	
	jmp.framenum = g_frameIndex;
	if(g_keyframeflag)
		jmp.jumpmode = JMPMODEPREV;
	else
		jmp.jumpmode = JMPMODEPREVK;
		
	if(g_rendereditflag==0)
	{
		jumpframe(NULL, &jmp);
	}
	else // g_renderflag = 1
	{
		getrefframergb(jmp);
	}

	return ;
}

	
void nextkeyframe(GtkWidget * button,GtkWidget * data)
{
	SFrameJump jmp;
	
	jmp.framenum = g_frameIndex;
	if(g_keyframeflag)
		jmp.jumpmode = JMPMODENEXT;
	else
		jmp.jumpmode = JMPMODENEXTK;
		
	if(g_rendereditflag==0)
	{
		jumpframe(NULL, &jmp);
	}
	else // g_renderflag = 1
	{
		getrefframergb(jmp);
	}
	return ;
}

void gotoframe(GtkWidget * button,GtkWidget * data)
{
	SFrameJump jmp;
	
	jmp.framenum = g_frameIndex;
	jmp.jumpmode = JMPMODECUR;
		
	if(g_rendereditflag==0)
	{
		jumpframe(NULL, &jmp);
	}
	else // g_renderflag = 1
	{
		getrefframergb(jmp);
	}
	return ;
}

void histogram(uint8_t *y, int * hist, int width, int height)
{
	int i,j;
	for(i=0;i<256;i++)
		hist[i]=0;
	
	for(j=0;j<height;j++)
		for(i=0;i<width;i++)
		{
			hist[y[i+j*width]]++;
		}
	return ;
}

int histogramdiff(int *hist0, int *hist1)
{
	int i;
	int sum = 0;
	for(i=0;i<256;i++)
	{
		sum += abs(hist0[i]-hist1[i]);
	}

	return sum;
}

void histogramcopy(int *hist0, int *hist1)
{
	int i;
	for(i=0;i<256;i++)
	{
		hist0[i] = hist1[i];
	}

	return ;
}

void insertkeyframe(int index)
{
	SKeyFrameList *start = &g_keyframelist;
	SKeyFrameList *pkey;
	SKeyFrameList *insertkey;

	pkey = start;
	do
	{
		if(pkey->index<index&&pkey->next==NULL)
		{
			insertkey = (SKeyFrameList *)malloc(sizeof(SKeyFrameList));
			insertkey->index = index;
			insertkey->prev = pkey;
			insertkey->next = pkey->next;
			//pkey->next->prev = insertkey;
			pkey->next = insertkey;
			g_keyframenum++;
			break;
		}
		if(pkey->index<index&&pkey->next->index>index)
		{
			insertkey = (SKeyFrameList *)malloc(sizeof(SKeyFrameList));
			insertkey->index = index;
			insertkey->prev = pkey;
			insertkey->next = pkey->next;
			pkey->next->prev = insertkey;
			pkey->next = insertkey;
			g_keyframenum++;
			break;
		}
		pkey = pkey->next;
	}while(pkey!=NULL);
	//printf("keyframenum %d index %d - %d\n", g_keyframenum, insertkey->prev->index, insertkey->index);
	return ;
}

void deletekeyframe(int index)
{

	SKeyFrameList *start = &g_keyframelist;
	SKeyFrameList *pkey;
	
	pkey = start;
	if(g_keyframenum>1)
	{
		do
		{
			if(pkey->index==index&&pkey->next==NULL)
			{
				pkey->prev->next = NULL;
				free(pkey);
				pkey = NULL;
				g_keyframenum--;
				break;
			}
			if(pkey->index==index)
			{
				pkey->prev->next = pkey->next;
				pkey->next->prev = pkey->prev;
				free(pkey);
				pkey = NULL;
				g_keyframenum--;
				break;
			}

			pkey = pkey->next;
		}while(pkey!=NULL);
	}

	//printf("keyframenum %d\n", g_keyframenum);
	return ;
}

void freekeyframe()
{
	SKeyFrameList *start = &g_keyframelist;
	SKeyFrameList *pkey;
	
	pkey = start;

	while(1)
	{
		if(pkey->next==NULL||pkey->next->index<0||pkey->next->index>g_frameMaxNum) // 最后一个
		{
			break;
		}
		else
		{
			deletekeyframe(pkey->next->index);
		}
	}

	g_keyframelist.next = g_keyframelist.prev = NULL;
	return ;
}

SKeyFrameList * getcurkeyframe(int index)
{
	SKeyFrameList *start = &g_keyframelist;
	SKeyFrameList *pkey;
	
	pkey = start;
	do
	{
		if(pkey->index<=index&&pkey->next==NULL)
		{
			return pkey;
		}
		if(pkey->index<=index&&pkey->next->index>index)
		{
			return pkey;
		}
		pkey = pkey->next;
	}while(pkey!=NULL);

	return NULL;
}

void GetKeyFrames()
{
	int i;

	if(g_frameMaxNum>0&&(g_inputfileformat==INPYUV||g_inputfileformat==INPVID)&&g_keyframeflag==0)
	{
		g_keyframelist.index = 0;
		g_keyframelist.next = g_keyframelist.prev = NULL;

		for(i=1;i<g_frameMaxNum;i++)
		{
			if(g_frames[i].KeyFlag==1) // 已经由用户自动标记为关键帧了
			{
				insertkeyframe(i);
			}
		}
	}



	//printf("keyframenum %d\n", g_keyframenum);

	
}


void ShowAll()
{
	int i;

	int size = g_frameWidth*g_frameHeight;
	
	if(g_frameIndex < g_frameMaxNum && g_frameIndex >= 0)
	{
		if(g_frames[g_frameIndex].SegFlag)
		{
			if(g_renderflag==0)
			{
				if(g_enlargeflag==0)
				{
					/*原始区域*/
					gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
			              IMGWIDTHMAX-g_frameWidth, IMGHEIGHTMAX-g_frameHeight, g_frameWidth, g_frameHeight,
			              GDK_RGB_DITHER_MAX, g_frames[g_frameIndex].rgbinput, g_frameWidth * 3);
					/* 自动分割区*/
					gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
			              IMGWIDTHMAX, IMGHEIGHTMAX-g_frameHeight, g_frameWidth, g_frameHeight,
			              GDK_RGB_DITHER_MAX, g_autosegdisplay, g_frameWidth * 3);
					/* 手工分割区域和深度图区域显示*/ // TODO 和上面的合并
					ShowSegandDep(g_frames[g_frameIndex].SegLabelMask);
				}
				else
				{
					if(g_enlargeswitchflag==0)
					{
						gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
						              IMGWIDTHMAX-g_frameWidth, IMGHEIGHTMAX-g_frameHeight, g_frameWidth*2, g_frameHeight*2,
						              GDK_RGB_DITHER_MAX, g_enlargedisplaytmp0, g_frameWidth * 6);
					}
					else
					{
						gdk_draw_gray_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
							          IMGWIDTHMAX-g_frameWidth, IMGHEIGHTMAX-g_frameHeight, g_frameWidth*2, g_frameHeight*2,
							          GDK_RGB_DITHER_MAX, g_enlargedisplaytmp0, g_frameWidth*2);
					}
				}
			}
			else
			{

				//原始图像区
				gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
		              IMGWIDTHMAX-g_frameWidth, IMGHEIGHTMAX-g_frameHeight, g_frameWidth, g_frameHeight,
		              GDK_RGB_DITHER_MAX, g_tmpdisplay1, g_frameWidth * 3);
				// 自动分割区
				gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
		              IMGWIDTHMAX, IMGHEIGHTMAX-g_frameHeight, g_frameWidth, g_frameHeight,
		              GDK_RGB_DITHER_MAX, g_tmpdisplay2, g_frameWidth * 3);
				/* 深度图区域显示*/
				ShowDep(g_frames[g_frameIndex].SegLabelMask);

				// 手工分割区
				gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
              		IMGWIDTHMAX-g_frameWidth, IMGHEIGHTMAX, g_frameWidth, g_frameHeight,
		              GDK_RGB_DITHER_MAX, g_tmpdisplay3, g_frameWidth * 3);
			}
		}
		else
		{
			for(i=0;i<size;i++)
				g_depthdisplay[i] = 0;
			//原始图像区
			gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
	              IMGWIDTHMAX-g_frameWidth, IMGHEIGHTMAX-g_frameHeight, g_frameWidth, g_frameHeight,
	              GDK_RGB_DITHER_MAX, g_frames[g_frameIndex].rgbinput, g_frameWidth * 3);
			// 自动分割区
			gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
	              IMGWIDTHMAX, IMGHEIGHTMAX-g_frameHeight, g_frameWidth, g_frameHeight,
	              GDK_RGB_DITHER_MAX, g_frames[g_frameIndex].rgbinput, g_frameWidth * 3);
			// 手动分割区
			gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
	              IMGWIDTHMAX-g_frameWidth, IMGHEIGHTMAX, g_frameWidth, g_frameHeight,
	              GDK_RGB_DITHER_MAX, g_frames[g_frameIndex].rgbinput, g_frameWidth * 3);
			// 深度区
			gdk_draw_gray_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
	              IMGWIDTHMAX, IMGHEIGHTMAX, g_frameWidth, g_frameHeight,
	              GDK_RGB_DITHER_MAX, g_depthdisplay, g_frameWidth);
		}
		
	}
	else
	{
		// do nothing
	}
	return ;
}

void yuvresize(SYuvInput in, SYuvInput out, int inw, int inh, int outw, int outh)
{
	InterpYuvBilinear(in.y, out.y, inw, inh, outw, outh);
	InterpYuvBilinear(in.u, out.u, inw>>1, inh>>1, outw>>1, outh>>1);
	InterpYuvBilinear(in.v, out.v, inw>>1, inh>>1, outw>>1, outh>>1);
	return ;
}

void yuvresize1(uint8_t* in, uint8_t* out, int inw, int inh, int outw, int outh)
{
	uint8_t *iny, *inu, *inv;
	uint8_t *outy, *outu, *outv;

	iny = in;
	inu = in+inw*inh;
	inv = inu+(inw*inh>>2);
	outy = out;
	outu = out+outw*outh;
	outv = outu+(outw*outh>>2);

	InterpYuvBilinear(iny, outy, inw, inh, outw, outh);
	InterpYuvBilinear(inu, outu, inw>>1, inh>>1, outw>>1, outh>>1);
	InterpYuvBilinear(inv, outv, inw>>1, inh>>1, outw>>1, outh>>1);
	
	return ;
}

void yresize(uint8_t* in, uint8_t* out, int inw, int inh, int outw, int outh)
{
	InterpYuvBilinear(in, out, inw, inh, outw, outh);
	
	return ;
}


