/* OPENCV */
#include "cv.h"   
#include "highgui.h"   

#include "CSuperpixelSeg.h"
#include "project.h"

int g_segparamnumregion = SEGPARAM_NUMREGIONDEF;
int g_segparamweightspadis = SEGPARAM_WEIGHTSPADISDEF;
int g_segparamiterate = SEGPARAM_ITERATEDEF;

int g_segparamautoedgeth = SEGPARAM_AUTOEDGETHDEF;

int g_segparamedgeenlargeiter = SEGPARAM_EDGEENLARGEITERDEF;
int g_segparamedgeenlargesize = SEGPARAM_EDGEENLARGESIZEDEF;
int g_segparamedgeenlargeth = SEGPARAM_EDGEENLARGETHDEF;

// 分割用变量
int g_regionr[SEGPARAM_NUMREGIONMAX+15];
int g_regiong[SEGPARAM_NUMREGIONMAX+15];
int g_regionb[SEGPARAM_NUMREGIONMAX+15];
int g_regionc[SEGPARAM_NUMREGIONMAX+15];

int g_showalldepflag = 1;
int g_showautosegflag = 0;

int g_setsegframesbarflag = 0;
double g_setsegframesbarval = 0.0;

int g_autosegcolor = SEGEDITCOLORBLACK;
//#define RGBDIFFTH 16

//int g_refparamstrength = REFPARAM_STRENGTHDEF;
//int g_refparamweightspadis = REFPARAM_WEIGHTSPADISDEF;
//int g_refparamiterate = REFPARAM_ITERATEDEF;

/* 仅用于由高到低分辨率映射，低分辨率仅用于四画面显示*/
void LabelResize(int * srclabel, int * dstlabel, uint8_t *srcinp, uint8_t *dstinp, int srcw, int srch, int dstw, int dsth)
{
#if 1
	int i,j,ii,jj;

	for(j=0;j<dsth;j++)
		for(i=0;i<dstw;i++)
		{
			ii = i*srcw/dstw;
			jj = j*srch/dsth;
			dstlabel[i+j*dstw] = srclabel[ii+jj*srcw];
		}
#else
	int i,j,ii,jj,iii,jjj;
	int diff, src, dst;
	int min;

	for(j=0;j<dsth;j++)
		for(i=0;i<dstw;i++)
		{
			min = (int)10000;
			ii = i*srcw/dstw;
			jj = j*srch/dsth;

			src = (int)srcinp[ii+jj*srcw];
			dst = (int)dstinp[i+j*dstw];
			diff = abs(dst-src);
			if(diff<min)
			{
				dstlabel[i+j*dstw] = srclabel[ii+jj*srcw];
				min = diff;
				if(diff<g_segparamautoedgeth)
				{
					//if(diff!=0) printf("%d@", diff);
					continue ;
				}
			}

			// 判断周围label TODO 顺序
			for(jjj=-1;jjj<=1;jjj++)
				for(iii=-1;iii<=1;iii++)
				{
					if(ii+iii>=0&&jj+jjj>=0&&ii+iii<srcw&&jj+jjj<srch&&srclabel[ii+jj*srcw]!=srclabel[ii+iii+(jj+jjj)*srcw])
					{
						src = (int)srcinp[(ii+iii)+(jj+jjj)*srcw];
						dst = (int)dstinp[i+j*dstw];
						diff = abs(dst-src)+(abs(iii)+abs(jjj))*3; // 距离因子设定TODO
						if(diff<min)
						{
							dstlabel[i+j*dstw] = srclabel[ii+iii+(jj+jjj)*srcw];
							min = diff;
							if(diff<g_segparamautoedgeth)
								continue ;
							//printf("%d ", diff);
						}
						if(diff<g_segparamautoedgeth)
						{
							dstlabel[i+j*dstw] = srclabel[ii+iii+(jj+jjj)*srcw];
							continue ;
						}
					}
				}
			
		}

#endif
	return ;
}

void CalcRegionAvgRgb(int * label, uint8_t *inp,int w, int h, int number)
{
	int i,j;
	
	for(i=0;i<number+1;i++)
	{
		g_regionr[i] = g_regiong[i] = g_regionb[i] = g_regionc[i] = 0;
	}

	for(j=0;j<h;j++)
		for(i=0;i<w;i++)
		{
			g_regionr[label[i+j*w]] +=  inp[i*3+j*w*3];
			g_regiong[label[i+j*w]] +=  inp[i*3+1+j*w*3];
			g_regionb[label[i+j*w]] +=  inp[i*3+2+j*w*3];
			g_regionc[label[i+j*w]]++;
		}

	for(i=0;i<number+1;i++)
	{
		if(g_regionc[i]!=0)
		{
			g_regionr[i] = g_regionr[i]/g_regionc[i];
			g_regiong[i] = g_regiong[i]/g_regionc[i];
			g_regionb[i] = g_regionb[i]/g_regionc[i];
		}
	}

	return ;
}

/* 仅用于高分辨率情况下，第一次分割后低分辨率分割映射到高分辨率分割情况*/
void LabelResize0(int * srclabel, int * dstlabel, uint8_t *srcinp, uint8_t *dstinp, int srcw, int srch, int dstw, int dsth, int number)
{
	int i,j,ii,jj,iii,jjj;
	int diff, src, dst;
	int min;
	int * tmplabel = new int[dstw*dsth];

	for(j=0;j<dsth;j++)
		for(i=0;i<dstw;i++)
		{
			min = (int)10000;
			ii = i*srcw/dstw;
			jj = j*srch/dsth;

			// 判断周围label TODO 顺序
			for(jjj=-2;jjj<=2;jjj++)
				for(iii=-2;iii<=2;iii++)
				{
					if(ii+iii>=0&&jj+jjj>=0&&ii+iii<srcw&&jj+jjj<srch)//&&srclabel[ii+jj*srcw]!=srclabel[ii+iii+(jj+jjj)*srcw])
					{
						//src = (int)srcinp[(ii+iii)+(jj+jjj)*srcw];
						src = (int)g_regionr[srclabel[ii+iii+(jj+jjj)*srcw]];
						dst = (int)dstinp[i*3+j*dstw*3];
						diff = abs(dst-src); 
						src = (int)g_regiong[srclabel[ii+iii+(jj+jjj)*srcw]];
						dst = (int)dstinp[i*3+1+j*dstw*3];
						diff += abs(dst-src); 
						src = (int)g_regionb[srclabel[ii+iii+(jj+jjj)*srcw]];
						dst = (int)dstinp[i*3+2+j*dstw*3];
						diff += abs(dst-src); 
						if(diff<min)
						{
							dstlabel[i+j*dstw] = srclabel[ii+iii+(jj+jjj)*srcw];
							min = diff;
							if(diff<g_segparamautoedgeth)
								continue ;
							//printf("%d ", diff);
						}
						if(diff<g_segparamautoedgeth)
						{
							dstlabel[i+j*dstw] = srclabel[ii+iii+(jj+jjj)*srcw];
							continue ;
						}
					}
				}
			
		}

	// 中值滤波，先水平，后垂直
	for(j=1;j<dsth-1;j++)
		for(i=1;i<dstw-1;i++)
		{
			tmplabel[i+j*dstw] = H3D_MEDIAN(dstlabel[i-1+j*dstw],dstlabel[i+j*dstw],dstlabel[i+1+j*dstw]);
			tmplabel[i+j*dstw] = H3D_MEDIAN(dstlabel[i+(j-1)*dstw],tmplabel[i+j*dstw],dstlabel[i+(j+1)*dstw]);
		}
		
	for(j=1;j<dsth-1;j++)
		for(i=1;i<dstw-1;i++)
		{
			dstlabel[i+j*dstw] = tmplabel[i+j*dstw];
		}		

	delete [] tmplabel;
	
	return ;
}


void SegmentImgNoShow(int frameidx)
{
	int i;
	/*SLIC分割图像*/
	int sz = g_frameWidth*g_frameHeight;
	int szorg = g_frameInWidth*g_frameInHeight;
	int m_spcount = g_segparamnumregion;
	double m_compactness = (double)g_segparamweightspadis;
	
	if(g_frames[frameidx].SegFlag == 0) 
	{
		g_frames[frameidx].SegFlag = 1;

		if(g_frameResizeFlag==1)
			for(i=0;i<szorg;i++)
			{
				g_OrgLabels0[i] = g_SegLabels0[i] = 0;
				g_SegDepths0[i] = 0;
			}
			
		for(i=0;i<sz;i++)
		{
			g_OrgLabels[i] = g_SegLabels[i] = 0;
			g_SegDepths[i] = 0;
		}

	}
	
	for(i=0;i<szorg;i++)
	{
		g_frames[frameidx].OrgLabels0[i] = g_frames[frameidx].SegLabels0[i] = 0;
		g_frames[frameidx].SegDepths0[i] = 255;
	}
	
	for(i=0;i<sz;i++)
	{
		g_frames[frameidx].OrgLabels[i] = g_frames[frameidx].SegLabels[i] = 0;
		g_frames[frameidx].SegDepths[i] = 255;
	}
	
	/*slic分割*/	
	int numlabels(0);
	CSuperpixelSeg superpixelseg;
	
	if(g_frameResizeFlag==1)
	{
		#if 1 // 低分辨率分割速度快
		/*采用低分辨率进行分割，速度快，再映射到高分辨率图像中
                 为保证load高分辨率分割结果与低分辨率一致，此时要再进行一次映射
		  */
		superpixelseg.DoSuperpixelSegmentation_ForGivenK(g_frames[frameidx].rgbinput, g_frameWidth, g_frameHeight, g_frames[frameidx].OrgLabels, numlabels, m_spcount, m_compactness);
		CalcRegionAvgRgb(g_frames[frameidx].OrgLabels, g_rgbinput, g_frameWidth, g_frameHeight, numlabels);
		LabelResize0(g_frames[frameidx].OrgLabels,  g_frames[frameidx].OrgLabels0, g_rgbinput, g_rgborginput, g_frameWidth, g_frameHeight, g_frameInWidth, g_frameInHeight, g_frames[frameidx].AutoSegLabel);
		LabelResize(g_frames[frameidx].OrgLabels0,g_frames[frameidx].OrgLabels,g_yuvorginput.y, g_yuvinput.y,  g_frameInWidth, g_frameInHeight,g_frameWidth,g_frameHeight);
		//superpixelseg.DrawContoursAroundSegments(g_autosegdisplay, g_frames[g_frameIndex].OrgLabels, g_frameWidth, g_frameHeight, 0);
		CalcRegionAvgRgb(g_frames[frameidx].OrgLabels, g_rgbinput, g_frameWidth, g_frameHeight, numlabels);
		for(i=0;i<szorg;i++)
			g_frames[frameidx].SegLabels0[i] = g_frames[frameidx].OrgLabels0[i];
		#else
		superpixelseg.DoSuperpixelSegmentation_ForGivenK(g_rgborginput, g_frameInWidth, g_frameInHeight, g_frames[g_frameIndex].OrgLabels0, numlabels, m_spcount, m_compactness);
		for(i=0;i<szorg;i++)
			g_frames[g_frameIndex].SegLabels0[i] = g_frames[g_frameIndex].OrgLabels0[i];
		LabelResize(g_frames[g_frameIndex].OrgLabels0, g_frames[g_frameIndex].OrgLabels, g_yuvinput, g_yuvorginput, g_frameInWidth, g_frameInHeight, g_frameWidth, g_frameHeight);
		superpixelseg.DrawContoursAroundSegments(g_autosegdisplay, g_frames[g_frameIndex].OrgLabels, g_frameWidth, g_frameHeight, 255);
		#endif
	}
	else
	{
		superpixelseg.DoSuperpixelSegmentation_ForGivenK(g_frames[frameidx].rgbinput, g_frameWidth, g_frameHeight, g_frames[frameidx].OrgLabels, numlabels, m_spcount, m_compactness);
		//superpixelseg.DrawContoursAroundSegments(g_autosegdisplay, g_frames[g_frameIndex].OrgLabels, g_frameWidth, g_frameHeight, 0);
		CalcRegionAvgRgb(g_frames[frameidx].OrgLabels, g_rgbinput, g_frameWidth, g_frameHeight, numlabels);
	}

	//printf("g_numlabels: %d\n", g_numlabels);
	g_frames[frameidx].AutoSegLabel = numlabels;
	
	for(i=0;i<sz;i++)
		g_frames[frameidx].SegLabels[i] = g_frames[frameidx].OrgLabels[i];


	#if 0
	/*结果显示*/
	//原始图像区
	gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
		              IMGWIDTHMAX-g_frameWidth, IMGHEIGHTMAX-g_frameHeight, g_frameWidth, g_frameHeight,
		              GDK_RGB_DITHER_MAX, g_frames[g_frameIndex].rgbinput, g_frameWidth * 3);
	// 自动分割区
	gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
          IMGWIDTHMAX, IMGHEIGHTMAX-g_frameHeight, g_frameWidth, g_frameHeight,
          GDK_RGB_DITHER_MAX, g_autosegdisplay, g_frameWidth * 3);
	// 手动分割区
	gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
          IMGWIDTHMAX-g_frameWidth, IMGHEIGHTMAX, g_frameWidth, g_frameHeight,
          GDK_RGB_DITHER_MAX, g_frames[g_frameIndex].rgbinput, g_frameWidth * 3);
	// 深度区
	gdk_draw_gray_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
          IMGWIDTHMAX, IMGHEIGHTMAX, g_frameWidth, g_frameHeight,
          GDK_RGB_DITHER_MAX, g_depthdisplay, g_frameWidth);
	#endif
		
	return ;
}

void SegmentImg(GtkWidget *button,GtkWidget *window)
{
	int i;
	/*SLIC分割图像*/
	int sz = g_frameWidth*g_frameHeight;
	int szorg = g_frameInWidth*g_frameInHeight;
	int m_spcount = g_segparamnumregion;
	double m_compactness = (double)g_segparamweightspadis;
	
	if(g_frames[g_frameIndex].SegFlag == 0) 
	{
		g_frames[g_frameIndex].SegFlag = 1;

		if(g_frameResizeFlag==1)
			for(i=0;i<szorg;i++)
			{
				g_OrgLabels0[i] = g_SegLabels0[i] = 0;
				g_SegDepths0[i] = 0;
			}
			
		for(i=0;i<sz;i++)
		{
			g_OrgLabels[i] = g_SegLabels[i] = 0;
			g_SegDepths[i] = 0;
		}

	}
	
	for(i=0;i<szorg;i++)
	{
		g_frames[g_frameIndex].OrgLabels0[i] = g_frames[g_frameIndex].SegLabels0[i] = 0;
		g_frames[g_frameIndex].SegDepths0[i] = 255;
	}
	
	for(i=0;i<sz;i++)
	{
		g_frames[g_frameIndex].OrgLabels[i] = g_frames[g_frameIndex].SegLabels[i] = 0;
		g_frames[g_frameIndex].SegDepths[i] = 255;
	}
		
	for(i=0;i<sz*3;i++)
		g_autosegdisplay[i] = g_frames[g_frameIndex].rgbinput[i];
	
	/*slic分割*/	
	int numlabels(0);
	CSuperpixelSeg superpixelseg;
	
	if(g_frameResizeFlag==1)
	{
		#if 1 // 低分辨率分割速度快
		/*采用低分辨率进行分割，速度快，再映射到高分辨率图像中
                 为保证load高分辨率分割结果与低分辨率一致，此时要再进行一次映射
		  */
		superpixelseg.DoSuperpixelSegmentation_ForGivenK(g_frames[g_frameIndex].rgbinput, g_frameWidth, g_frameHeight, g_frames[g_frameIndex].OrgLabels, numlabels, m_spcount, m_compactness);
		CalcRegionAvgRgb(g_frames[g_frameIndex].OrgLabels, g_rgbinput, g_frameWidth, g_frameHeight, numlabels);
		LabelResize0(g_frames[g_frameIndex].OrgLabels,  g_frames[g_frameIndex].OrgLabels0, g_rgbinput, g_rgborginput, g_frameWidth, g_frameHeight, g_frameInWidth, g_frameInHeight, g_frames[g_frameIndex].AutoSegLabel);
		LabelResize(g_frames[g_frameIndex].OrgLabels0,g_frames[g_frameIndex].OrgLabels,g_yuvorginput.y, g_yuvinput.y,  g_frameInWidth, g_frameInHeight,g_frameWidth,g_frameHeight);
		superpixelseg.DrawContoursAroundSegments(g_autosegdisplay, g_frames[g_frameIndex].OrgLabels, g_frameWidth, g_frameHeight, g_autosegcolor);
		CalcRegionAvgRgb(g_frames[g_frameIndex].OrgLabels, g_rgbinput, g_frameWidth, g_frameHeight, numlabels);
		for(i=0;i<szorg;i++)
			g_frames[g_frameIndex].SegLabels0[i] = g_frames[g_frameIndex].OrgLabels0[i];
		#else
		superpixelseg.DoSuperpixelSegmentation_ForGivenK(g_rgborginput, g_frameInWidth, g_frameInHeight, g_frames[g_frameIndex].OrgLabels0, numlabels, m_spcount, m_compactness);
		for(i=0;i<szorg;i++)
			g_frames[g_frameIndex].SegLabels0[i] = g_frames[g_frameIndex].OrgLabels0[i];
		LabelResize(g_frames[g_frameIndex].OrgLabels0, g_frames[g_frameIndex].OrgLabels, g_yuvinput, g_yuvorginput, g_frameInWidth, g_frameInHeight, g_frameWidth, g_frameHeight);
		superpixelseg.DrawContoursAroundSegments(g_autosegdisplay, g_frames[g_frameIndex].OrgLabels, g_frameWidth, g_frameHeight, 255);
		#endif
	}
	else
	{
		superpixelseg.DoSuperpixelSegmentation_ForGivenK(g_frames[g_frameIndex].rgbinput, g_frameWidth, g_frameHeight, g_frames[g_frameIndex].OrgLabels, numlabels, m_spcount, m_compactness);
		superpixelseg.DrawContoursAroundSegments(g_autosegdisplay, g_frames[g_frameIndex].OrgLabels, g_frameWidth, g_frameHeight, g_autosegcolor);
		CalcRegionAvgRgb(g_frames[g_frameIndex].OrgLabels, g_rgbinput, g_frameWidth, g_frameHeight, numlabels);
	}

	//printf("g_numlabels: %d\n", g_numlabels);
	g_frames[g_frameIndex].AutoSegLabel = numlabels;
	
	for(i=0;i<sz;i++)
		g_frames[g_frameIndex].SegLabels[i] = g_frames[g_frameIndex].OrgLabels[i];

	if(window!=NULL)
		gtk_widget_hide(window);
	
	/*结果显示*/
	//原始图像区
	gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
		              IMGWIDTHMAX-g_frameWidth, IMGHEIGHTMAX-g_frameHeight, g_frameWidth, g_frameHeight,
		              GDK_RGB_DITHER_MAX, g_frames[g_frameIndex].rgbinput, g_frameWidth * 3);
	// 自动分割区
	gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
          IMGWIDTHMAX, IMGHEIGHTMAX-g_frameHeight, g_frameWidth, g_frameHeight,
          GDK_RGB_DITHER_MAX, g_autosegdisplay, g_frameWidth * 3);
	// 手动分割区
	gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
          IMGWIDTHMAX-g_frameWidth, IMGHEIGHTMAX, g_frameWidth, g_frameHeight,
          GDK_RGB_DITHER_MAX, g_frames[g_frameIndex].rgbinput, g_frameWidth * 3);
	// 深度区
	gdk_draw_gray_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
          IMGWIDTHMAX, IMGHEIGHTMAX, g_frameWidth, g_frameHeight,
          GDK_RGB_DITHER_MAX, g_depthdisplay, g_frameWidth);
		
	return ;
}


int segmentframes(int first, int last)
{
	int k;

	char filename[256];
	char buff[512];
	
	int curidx = g_frameIndex;

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

		g_setsegframesbarflag = 1;
		g_setsegframesbarval = 0.0;
		//printf("first %d last %d\n", first, last);
		for(k=first;k<=last;k++)
		{
			g_frameIndex = k;
			
			// 载入处理帧
			if(g_keyframeflag)
				sprintf(filename,"./%s/info/%d_infosnap_%d.dat",cfgparams.projectname,g_sectioncur,g_keyframenumarr[g_frameIndex]+1);
			else
				sprintf(filename,"./%s/info/%d_infosnap_%d.dat",cfgparams.projectname,g_sectioncur,g_frameIndex+1);
			if((g_frames[k].finfo=fopen(filename,"rb"))!=NULL)  //如果存在，必然已分割
			{
				loadinfofile(g_frames[k].finfo, k, 0);
				fclose(g_frames[k].finfo);
				g_snprintf (buff, 512, "SEGMENT  INFO --- Load  information frame index %d from %d to %d %.2lf%% ", k+1, first+1, last+1,((double)(k-first+1))/(last-first+1)*100);
				printf("\r%s", buff);
				//g_usleep(100);
				//g_thread_create((GThreadFunc)statusbar_thread,buff,FALSE,NULL); 
				//g_idle_add((GSourceFunc)statusbar_thread,buff);
				gtk_statusbar_push (GTK_STATUSBAR (g_status_bar), GPOINTER_TO_INT(g_context_id), buff);
				
			}
			else
			{
				// 未分割
				//if(g_frames[k].SegFlag == 0||g_frames[k].AutoSegLabel <= g_segparamnumregion/4)
				//{
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
				// 保存分割信息
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
				g_snprintf (buff, 512, "SEGMENT  INFO --- Automatic segment frame index %d from %d to %d %.2lf%% ", k+1, first+1, last+1,((double)(k-first+1))/(last-first+1)*100);
				printf("\r%s", buff);
				//g_usleep(100); 
				//g_thread_create((GThreadFunc)statusbar_thread,buff,FALSE,NULL); 
				//g_idle_add((GSourceFunc)statusbar_thread,buff);
				gtk_statusbar_push (GTK_STATUSBAR (g_status_bar), GPOINTER_TO_INT(g_context_id), buff);
				g_setsegframesbarval = ((double)(k-first+1))/(last-first+1);
				//g_usleep(1000); 
					
				//}
			}
		}
		printf("\n");
		
		g_setsegframesbarflag = 0;

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
	
	return H3D_RESULT_OK;
}


void CancelRefine()
{	
	return ;
}

#define NEWAUTOSEG

#define AUTHSTEP 5
#define REGIONSTEP 10 

void AutoRegionSeg(int *labels, int *oldlabels, uint8_t * rgbinp, int width, int height, int number)
{
	int i,j,k;
	int diff;
	int laberth = g_frameWidth*g_frameHeight;
	int min;
	int count, count1, count2;

	int step = (int)(sqrt(float(g_frameInWidth*g_frameInHeight/g_segparamnumregion))/2);

	int * regionr = new int[number+1];
	int * regiong = new int[number+1];
	int * regionb = new int[number+1];
	int * regionx = new int[number+1];
	int * regiony = new int[number+1];
	int * regionl = new int[number+1];
	int * regionc = new int[number+1];

	for(i=0;i<=number;i++)
		regionr[i] = regiong[i] = regionb[i] = regionx[i] = regiony[i] = regionl[i] = regionc[i] = 0;

	for(j=0;j<height;j++)
		for(i=0;i<width;i++)
		{
			if(labels[i+j*width]<=number)
			{
				regionr[labels[i+j*width]] += rgbinp[i*3+j*width*3];
				regiong[labels[i+j*width]] += rgbinp[i*3+1+j*width*3];
				regionb[labels[i+j*width]] += rgbinp[i*3+2+j*width*3];
				regionx[labels[i+j*width]] += i;
				regiony[labels[i+j*width]] += j;
				regionc[labels[i+j*width]] ++;
			}
		}

	//do{
	count = 0;
	count1 = 0;
	count2 = 0;
	for(i=0;i<=number;i++)
	{
		if(regionc[i]!=0)
		{
			min = g_segparamautoedgeth*AUTHSTEP; //
			count1 ++;
			
			regionr[i] = regionr[i]/regionc[i];
			regiong[i] = regiong[i]/regionc[i];
			regionb[i] = regionb[i]/regionc[i];
			regionx[i] = regionx[i]/regionc[i];
			regiony[i] = regiony[i]/regionc[i];
			regionl[i] = labels[regionx[i]+regiony[i]*width];

			// left
			if(regionx[i]-step>0&&labels[regionx[i]-step+regiony[i]*width]>laberth)
			{
				diff = abs(regionr[i]-g_regionr[oldlabels[regionx[i]-step+regiony[i]*width]]) + 
					   abs(regiong[i]-g_regiong[oldlabels[regionx[i]-step+regiony[i]*width]]) + 
					   abs(regionb[i]-g_regionb[oldlabels[regionx[i]-step+regiony[i]*width]]);
				if(diff<min)
				{
					min = diff;
					regionl[i] = labels[regionx[i]-step+regiony[i]*width];
				}
			}

			// right
			if(regionx[i]+step<width&&labels[regionx[i]+step+regiony[i]*width]>laberth)
			{
				diff = abs(regionr[i]-g_regionr[oldlabels[regionx[i]+step+regiony[i]*width]]) + 
					   abs(regiong[i]-g_regiong[oldlabels[regionx[i]+step+regiony[i]*width]]) + 
					   abs(regionb[i]-g_regionb[oldlabels[regionx[i]+step+regiony[i]*width]]);
				if(diff<min)
				{
					min = diff;
					regionl[i] = labels[regionx[i]+step+regiony[i]*width];
				}
			}

			// up
			if(regiony[i]-step>0&&labels[regionx[i]+(regiony[i]-step)*width]>laberth)
			{
				diff = abs(regionr[i]-g_regionr[oldlabels[regionx[i]+(regiony[i]-step)*width]]) + 
					   abs(regiong[i]-g_regiong[oldlabels[regionx[i]+(regiony[i]-step)*width]]) + 
					   abs(regionb[i]-g_regionb[oldlabels[regionx[i]+(regiony[i]-step)*width]]);
				if(diff<min)
				{
					min = diff;
					regionl[i] = labels[regionx[i]+(regiony[i]-step)*width];
				}
			}

			// down
			if(regiony[i]+step<height&&labels[regionx[i]+(regiony[i]+step)*width]>laberth)
			{
				diff = abs(regionr[i]-g_regionr[oldlabels[regionx[i]+(regiony[i]+step)*width]]) + 
					   abs(regiong[i]-g_regiong[oldlabels[regionx[i]+(regiony[i]+step)*width]]) + 
					   abs(regionb[i]-g_regionb[oldlabels[regionx[i]+(regiony[i]+step)*width]]);
				if(diff<min)
				{
					min = diff;
					regionl[i] = labels[regionx[i]+(regiony[i]+step)*width];
				}
			}

			// leftup
			if(regionx[i]-step>0&&regiony[i]-step>0&&labels[regionx[i]-step+(regiony[i]-step)*width]>laberth)
			{
				diff = abs(regionr[i]-g_regionr[oldlabels[regionx[i]-step+(regiony[i]-step)*width]]) + 
					   abs(regiong[i]-g_regiong[oldlabels[regionx[i]-step+(regiony[i]-step)*width]]) + 
					   abs(regionb[i]-g_regionb[oldlabels[regionx[i]-step+(regiony[i]-step)*width]]);
				if(diff<min)
				{
					min = diff;
					regionl[i] = labels[regionx[i]-step+(regiony[i]-step)*width];
				}
			}

			// leftdown
			if(regionx[i]-step>0&&regiony[i]+step<height&&labels[regionx[i]-step+(regiony[i]+step)*width]>laberth)
			{
				diff = abs(regionr[i]-g_regionr[oldlabels[regionx[i]-step+(regiony[i]+step)*width]]) + 
					   abs(regiong[i]-g_regiong[oldlabels[regionx[i]-step+(regiony[i]+step)*width]]) + 
					   abs(regionb[i]-g_regionb[oldlabels[regionx[i]-step+(regiony[i]+step)*width]]);
				if(diff<min)
				{
					min = diff;
					regionl[i] = labels[regionx[i]-step+(regiony[i]+step)*width];
				}
			}

			// rightup
			if(regionx[i]+step<width&&regiony[i]-step>0&&labels[regionx[i]+step+(regiony[i]-step)*width]>laberth)
			{
				diff = abs(regionr[i]-g_regionr[oldlabels[regionx[i]+step+(regiony[i]-step)*width]]) + 
					   abs(regiong[i]-g_regiong[oldlabels[regionx[i]+step+(regiony[i]-step)*width]]) + 
					   abs(regionb[i]-g_regionb[oldlabels[regionx[i]+step+(regiony[i]-step)*width]]);
				if(diff<min)
				{
					min = diff;
					regionl[i] = labels[regionx[i]+step+(regiony[i]-step)*width];
				}
			}

			// rightdown
			if(regionx[i]+step<width&&regiony[i]+step<height&&labels[regionx[i]+step+(regiony[i]+step)*width]>laberth)
			{
				diff = abs(regionr[i]-g_regionr[oldlabels[regionx[i]+step+(regiony[i]+step)*width]]) + 
					   abs(regiong[i]-g_regiong[oldlabels[regionx[i]+step+(regiony[i]+step)*width]]) + 
					   abs(regionb[i]-g_regionb[oldlabels[regionx[i]+step+(regiony[i]+step)*width]]);
				if(diff<min)
				{
					min = diff;
					regionl[i] = labels[regionx[i]+step+(regiony[i]+step)*width];
				}
			}
		}
	}

	for(j=0;j<height;j++)
		for(i=0;i<width;i++)
		{
			if(labels[i+j*width]<=number)
			{
				k=labels[i+j*width];
				labels[i+j*width] = regionl[k];
				if(regionl[k]>laberth)
					count++;
				count2++;
			}
		}	
	//}while(count!=0);

	//printf("count: %d %d %d\n", count, count1, count2);

	delete [] regionr;
	delete [] regiong;
	delete [] regionb;
	delete [] regionx;
	delete [] regiony;
	delete [] regionl;
	delete [] regionc;
	
	return ;
}

void AutoPixelSeg()
{
	int i,j,k;

	uint8_t r0, g0,b0, r1, g1, b1;
	int segcnt, mincost, cost, minlabel, cnt;
	int labels[8];
	int flag;

	if(g_frameMaxNum>0&&g_frames[g_frameIndex].SegFlag==1)
	{
		if(g_frameResizeFlag==0)
		{
		for(j=g_frameHeight-1;j>=0;j--)
			for(i=0;i<g_frameWidth;i++)
			{
				if(g_frames[g_frameIndex].SegLabels[i+j*g_frameWidth]<g_frameWidth*g_frameHeight)
				{
					for(k=0;k<8;k++)
						labels[k] = 0;
					r0 = g_frames[g_frameIndex].rgbinput[i*3+j*g_frameWidth*3];
					g0 = g_frames[g_frameIndex].rgbinput[i*3+1+j*g_frameWidth*3];
					b0 = g_frames[g_frameIndex].rgbinput[i*3+2+j*g_frameWidth*3];
					segcnt = 0;
					cnt = 0;
					mincost = 0x7FFF;
					
					/* 检查该点8邻域的区域分割值*/
					// down right
					if((i+1<g_frameWidth)&&(j+1<g_frameHeight)&&g_frames[g_frameIndex].SegLabels[i+1+(j+1)*g_frameWidth]>g_frameWidth*g_frameHeight)
					{
						r1 = g_frames[g_frameIndex].rgbinput[(i+1)*3+(j+1)*g_frameWidth*3];
						g1 = g_frames[g_frameIndex].rgbinput[(i+1)*3+1+(j+1)*g_frameWidth*3];
						b1 = g_frames[g_frameIndex].rgbinput[(i+1)*3+2+(j+1)*g_frameWidth*3];
						cost = abs((int)r0-(int)r1)+abs((int)g0-(int)g1)+abs((int)b0-(int)b1);
						if(cost<mincost)
						{
							mincost = cost;
							minlabel = g_frames[g_frameIndex].SegLabels[i+1+(j+1)*g_frameWidth];
						}
						cnt ++;
						flag = 0;
						for(k=0;k<=segcnt;k++)
						{
							if(labels[k]==g_frames[g_frameIndex].SegLabels[i+1+(j+1)*g_frameWidth])
							{
								flag = 1;
								break;
							}
						}
						if(flag!=1)
						{
							labels[segcnt]=g_frames[g_frameIndex].SegLabels[i+1+(j+1)*g_frameWidth];
							segcnt++;
						}
					}

					// down
					if((j+1<g_frameHeight)&&g_frames[g_frameIndex].SegLabels[i+(j+1)*g_frameWidth]>g_frameWidth*g_frameHeight)
					{
						r1 = g_frames[g_frameIndex].rgbinput[(i)*3+(j+1)*g_frameWidth*3];
						g1 = g_frames[g_frameIndex].rgbinput[(i)*3+1+(j+1)*g_frameWidth*3];
						b1 = g_frames[g_frameIndex].rgbinput[(i)*3+2+(j+1)*g_frameWidth*3];
						cost = abs((int)r0-(int)r1)+abs((int)g0-(int)g1)+abs((int)b0-(int)b1);
						if(cost<mincost)
						{
							mincost = cost;
							minlabel = g_frames[g_frameIndex].SegLabels[i+(j+1)*g_frameWidth];
						}
						cnt ++;
						flag = 0;
						for(k=0;k<=segcnt;k++)
						{
							if(labels[k]==g_frames[g_frameIndex].SegLabels[i+(j+1)*g_frameWidth])
							{
								flag = 1;
								break;
							}
						}
						if(flag!=1)
						{
							labels[segcnt]=g_frames[g_frameIndex].SegLabels[i+(j+1)*g_frameWidth];
							segcnt++;
						}
					}
					
					// down left
					if((i-1>=0)&&(j+1<g_frameHeight)&&g_frames[g_frameIndex].SegLabels[i-1+(j+1)*g_frameWidth]>g_frameWidth*g_frameHeight)
					{
						r1 = g_frames[g_frameIndex].rgbinput[(i-1)*3+(j+1)*g_frameWidth*3];
						g1 = g_frames[g_frameIndex].rgbinput[(i-1)*3+1+(j+1)*g_frameWidth*3];
						b1 = g_frames[g_frameIndex].rgbinput[(i-1)*3+2+(j+1)*g_frameWidth*3];
						cost = abs((int)r0-(int)r1)+abs((int)g0-(int)g1)+abs((int)b0-(int)b1);
						if(cost<mincost)
						{
							mincost = cost;
							minlabel = g_frames[g_frameIndex].SegLabels[i-1+(j+1)*g_frameWidth];
						}
						cnt ++;
						flag = 0;
						for(k=0;k<=segcnt;k++)
						{
							if(labels[k]==g_frames[g_frameIndex].SegLabels[i-1+(j+1)*g_frameWidth])
							{
								flag = 1;
								break;
							}
						}
						if(flag!=1)
						{
							labels[segcnt]=g_frames[g_frameIndex].SegLabels[i-1+(j+1)*g_frameWidth];
							segcnt++;
						}
					}

					// right
					if((i+1<g_frameWidth)&&g_frames[g_frameIndex].SegLabels[i+1+(j)*g_frameWidth]>g_frameWidth*g_frameHeight)
					{
						r1 = g_frames[g_frameIndex].rgbinput[(i+1)*3+(j)*g_frameWidth*3];
						g1 = g_frames[g_frameIndex].rgbinput[(i+1)*3+1+(j)*g_frameWidth*3];
						b1 = g_frames[g_frameIndex].rgbinput[(i+1)*3+2+(j)*g_frameWidth*3];
						cost = abs((int)r0-(int)r1)+abs((int)g0-(int)g1)+abs((int)b0-(int)b1);
						if(cost<mincost)
						{
							mincost = cost;
							minlabel = g_frames[g_frameIndex].SegLabels[i+1+(j)*g_frameWidth];
						}
						cnt ++;
						flag = 0;
						for(k=0;k<=segcnt;k++)
						{
							if(labels[k]==g_frames[g_frameIndex].SegLabels[i+1+(j)*g_frameWidth])
							{
								flag = 1;
								break;
							}
						}
						if(flag!=1)
						{
							labels[segcnt]=g_frames[g_frameIndex].SegLabels[i+1+(j)*g_frameWidth];
							segcnt++;
						}
					}

					// left
					if((i-1>=0)&&g_frames[g_frameIndex].SegLabels[i-1+(j)*g_frameWidth]>g_frameWidth*g_frameHeight)
					{
						r1 = g_frames[g_frameIndex].rgbinput[(i-1)*3+(j)*g_frameWidth*3];
						g1 = g_frames[g_frameIndex].rgbinput[(i-1)*3+1+(j)*g_frameWidth*3];
						b1 = g_frames[g_frameIndex].rgbinput[(i-1)*3+2+(j)*g_frameWidth*3];
						cost = abs((int)r0-(int)r1)+abs((int)g0-(int)g1)+abs((int)b0-(int)b1);
						if(cost<mincost)
						{
							mincost = cost;
							minlabel = g_frames[g_frameIndex].SegLabels[i-1+(j)*g_frameWidth];
						}
						cnt ++;
						flag = 0;
						for(k=0;k<=segcnt;k++)
						{
							if(labels[k]==g_frames[g_frameIndex].SegLabels[i-1+(j)*g_frameWidth])
							{
								flag = 1;
								break;
							}
						}
						if(flag!=1)
						{
							labels[segcnt]=g_frames[g_frameIndex].SegLabels[i-1+(j)*g_frameWidth];
							segcnt++;
						}
					}

					// top right
					if((i+1<g_frameWidth)&&(j-1>=0)&&g_frames[g_frameIndex].SegLabels[i+1+(j-1)*g_frameWidth]>g_frameWidth*g_frameHeight)
					{
						r1 = g_frames[g_frameIndex].rgbinput[(i+1)*3+(j-1)*g_frameWidth*3];
						g1 = g_frames[g_frameIndex].rgbinput[(i+1)*3+1+(j-1)*g_frameWidth*3];
						b1 = g_frames[g_frameIndex].rgbinput[(i+1)*3+2+(j-1)*g_frameWidth*3];
						cost = abs((int)r0-(int)r1)+abs((int)g0-(int)g1)+abs((int)b0-(int)b1);
						if(cost<mincost)
						{
							mincost = cost;
							minlabel = g_frames[g_frameIndex].SegLabels[i+1+(j-1)*g_frameWidth];
						}
						cnt ++;
						flag = 0;
						for(k=0;k<=segcnt;k++)
						{
							if(labels[k]==g_frames[g_frameIndex].SegLabels[i+1+(j-1)*g_frameWidth])
							{
								flag = 1;
								break;
							}
						}
						if(flag!=1)
						{
							labels[segcnt]=g_frames[g_frameIndex].SegLabels[i+1+(j-1)*g_frameWidth];
							segcnt++;
						}
					}
					
					// top
					if((j-1>=0)&&g_frames[g_frameIndex].SegLabels[i+(j-1)*g_frameWidth]>g_frameWidth*g_frameHeight)
					{
						r1 = g_frames[g_frameIndex].rgbinput[(i)*3+(j-1)*g_frameWidth*3];
						g1 = g_frames[g_frameIndex].rgbinput[(i)*3+1+(j-1)*g_frameWidth*3];
						b1 = g_frames[g_frameIndex].rgbinput[(i)*3+2+(j-1)*g_frameWidth*3];
						cost = abs((int)r0-(int)r1)+abs((int)g0-(int)g1)+abs((int)b0-(int)b1);
						if(cost<mincost)
						{
							mincost = cost;
							minlabel = g_frames[g_frameIndex].SegLabels[i+(j-1)*g_frameWidth];
						}
						cnt ++;
						flag = 0;
						for(k=0;k<=segcnt;k++)
						{
							if(labels[k]==g_frames[g_frameIndex].SegLabels[i+(j-1)*g_frameWidth])
							{
								flag = 1;
								break;
							}
						}
						if(flag!=1)
						{
							labels[segcnt]=g_frames[g_frameIndex].SegLabels[i+(j-1)*g_frameWidth];
							segcnt++;
						}
					}
					
					// top left
					if((i-1>=0)&&(j-1>=0)&&g_frames[g_frameIndex].SegLabels[i-1+(j-1)*g_frameWidth]>g_frameWidth*g_frameHeight)
					{
						r1 = g_frames[g_frameIndex].rgbinput[(i-1)*3+(j-1)*g_frameWidth*3];
						g1 = g_frames[g_frameIndex].rgbinput[(i-1)*3+1+(j-1)*g_frameWidth*3];
						b1 = g_frames[g_frameIndex].rgbinput[(i-1)*3+2+(j-1)*g_frameWidth*3];
						cost = abs((int)r0-(int)r1)+abs((int)g0-(int)g1)+abs((int)b0-(int)b1);
						if(cost<mincost)
						{
							mincost = cost;
							minlabel = g_frames[g_frameIndex].SegLabels[i-1+(j-1)*g_frameWidth];
						}
						cnt ++;
						flag = 0;
						for(k=0;k<=segcnt;k++)
						{
							if(labels[k]==g_frames[g_frameIndex].SegLabels[i-1+(j-1)*g_frameWidth])
							{
								flag = 1;
								break;
							}
						}
						if(flag!=1)
						{
							labels[segcnt]=g_frames[g_frameIndex].SegLabels[i-1+(j-1)*g_frameWidth];
							segcnt++;
						}
					}
					
					if(segcnt==0)
					{
						// do nothing
					}
					else if(segcnt==1)
					{
						if(cnt>=5||mincost<g_segparamautoedgeth)
						{
							g_frames[g_frameIndex].SegLabels[i+j*g_frameWidth] = minlabel;
						}
					}
					else // segcnt >1
					{
						g_frames[g_frameIndex].SegLabels[i+j*g_frameWidth] = minlabel;
					}
				}
			}
		}
		else // high resolusion
		{
		for(j=g_frameInHeight-1;j>=0;j--)
			for(i=0;i<g_frameInWidth;i++)
			{
				if(g_frames[g_frameIndex].SegLabels0[i+j*g_frameInWidth]<g_frameWidth*g_frameHeight)
				{
					for(k=0;k<8;k++)
						labels[k] = 0;
					r0 = g_rgborginput[i*3+j*g_frameInWidth*3];
					g0 = g_rgborginput[i*3+1+j*g_frameInWidth*3];
					b0 = g_rgborginput[i*3+2+j*g_frameInWidth*3];
					segcnt = 0;
					cnt = 0;
					mincost = 0x7FFF;
					
					/* 检查该点8邻域的区域分割值*/
					// down right
					if((i+1<g_frameInWidth)&&(j+1<g_frameInHeight)&&g_frames[g_frameIndex].SegLabels0[i+1+(j+1)*g_frameInWidth]>g_frameWidth*g_frameHeight)
					{
						#ifdef NEWAUTOSEG
						r1 = g_regionr[g_frames[g_frameIndex].OrgLabels0[i+1+(j+1)*g_frameInWidth]];
						g1 = g_regiong[g_frames[g_frameIndex].OrgLabels0[i+1+(j+1)*g_frameInWidth]];
						b1 = g_regionb[g_frames[g_frameIndex].OrgLabels0[i+1+(j+1)*g_frameInWidth]];
						#else
						r1 = g_rgborginput[(i+1)*3+(j+1)*g_frameInWidth*3];
						g1 = g_rgborginput[(i+1)*3+1+(j+1)*g_frameInWidth*3];
						b1 = g_rgborginput[(i+1)*3+2+(j+1)*g_frameInWidth*3];
						#endif
						cost = abs((int)r0-(int)r1)+abs((int)g0-(int)g1)+abs((int)b0-(int)b1);
						if(cost<mincost)
						{
							mincost = cost;
							minlabel = g_frames[g_frameIndex].SegLabels0[i+1+(j+1)*g_frameInWidth];
						}
						cnt ++;
						flag = 0;
						for(k=0;k<=segcnt;k++)
						{
							if(labels[k]==g_frames[g_frameIndex].SegLabels0[i+1+(j+1)*g_frameInWidth])
							{
								flag = 1;
								break;
							}
						}
						if(flag!=1)
						{
							labels[segcnt]=g_frames[g_frameIndex].SegLabels0[i+1+(j+1)*g_frameInWidth];
							segcnt++;
						}
					}

					// down
					if((j+1<g_frameInHeight)&&g_frames[g_frameIndex].SegLabels0[i+(j+1)*g_frameInWidth]>g_frameWidth*g_frameHeight)
					{
						#ifdef NEWAUTOSEG
						r1 = g_regionr[g_frames[g_frameIndex].OrgLabels0[i+(j+1)*g_frameInWidth]];
						g1 = g_regiong[g_frames[g_frameIndex].OrgLabels0[i+(j+1)*g_frameInWidth]];
						b1 = g_regionb[g_frames[g_frameIndex].OrgLabels0[i+(j+1)*g_frameInWidth]];
						#else
						r1 = g_rgborginput[(i)*3+(j+1)*g_frameInWidth*3];
						g1 = g_rgborginput[(i)*3+1+(j+1)*g_frameInWidth*3];
						b1 = g_rgborginput[(i)*3+2+(j+1)*g_frameInWidth*3];
						#endif
						cost = abs((int)r0-(int)r1)+abs((int)g0-(int)g1)+abs((int)b0-(int)b1);
						if(cost<mincost)
						{
							mincost = cost;
							minlabel = g_frames[g_frameIndex].SegLabels0[i+(j+1)*g_frameInWidth];
						}
						cnt ++;
						flag = 0;
						for(k=0;k<=segcnt;k++)
						{
							if(labels[k]==g_frames[g_frameIndex].SegLabels0[i+(j+1)*g_frameInWidth])
							{
								flag = 1;
								break;
							}
						}
						if(flag!=1)
						{
							labels[segcnt]=g_frames[g_frameIndex].SegLabels0[i+(j+1)*g_frameInWidth];
							segcnt++;
						}
					}
					
					// down left
					if((i-1>=0)&&(j+1<g_frameInHeight)&&g_frames[g_frameIndex].SegLabels0[i-1+(j+1)*g_frameInWidth]>g_frameWidth*g_frameHeight)
					{
						#ifdef NEWAUTOSEG
						r1 = g_regionr[g_frames[g_frameIndex].OrgLabels0[i-1+(j+1)*g_frameInWidth]];
						g1 = g_regiong[g_frames[g_frameIndex].OrgLabels0[i-1+(j+1)*g_frameInWidth]];
						b1 = g_regionb[g_frames[g_frameIndex].OrgLabels0[i-1+(j+1)*g_frameInWidth]];
						#else
						r1 = g_rgborginput[(i-1)*3+(j+1)*g_frameInWidth*3];
						g1 = g_rgborginput[(i-1)*3+1+(j+1)*g_frameInWidth*3];
						b1 = g_rgborginput[(i-1)*3+2+(j+1)*g_frameInWidth*3];
						#endif
						cost = abs((int)r0-(int)r1)+abs((int)g0-(int)g1)+abs((int)b0-(int)b1);
						if(cost<mincost)
						{
							mincost = cost;
							minlabel = g_frames[g_frameIndex].SegLabels0[i-1+(j+1)*g_frameInWidth];
						}
						cnt ++;
						flag = 0;
						for(k=0;k<=segcnt;k++)
						{
							if(labels[k]==g_frames[g_frameIndex].SegLabels0[i-1+(j+1)*g_frameInWidth])
							{
								flag = 1;
								break;
							}
						}
						if(flag!=1)
						{
							labels[segcnt]=g_frames[g_frameIndex].SegLabels0[i-1+(j+1)*g_frameInWidth];
							segcnt++;
						}
					}

					// right
					if((i+1<g_frameInWidth)&&g_frames[g_frameIndex].SegLabels0[i+1+(j)*g_frameInWidth]>g_frameWidth*g_frameHeight)
					{
						#ifdef NEWAUTOSEG
						r1 = g_regionr[g_frames[g_frameIndex].OrgLabels0[i+1+(j)*g_frameInWidth]];
						g1 = g_regiong[g_frames[g_frameIndex].OrgLabels0[i+1+(j)*g_frameInWidth]];
						b1 = g_regionb[g_frames[g_frameIndex].OrgLabels0[i+1+(j)*g_frameInWidth]];
						#else
						r1 = g_rgborginput[(i+1)*3+(j)*g_frameInWidth*3];
						g1 = g_rgborginput[(i+1)*3+1+(j)*g_frameInWidth*3];
						b1 = g_rgborginput[(i+1)*3+2+(j)*g_frameInWidth*3];
						#endif
						cost = abs((int)r0-(int)r1)+abs((int)g0-(int)g1)+abs((int)b0-(int)b1);
						if(cost<mincost)
						{
							mincost = cost;
							minlabel = g_frames[g_frameIndex].SegLabels0[i+1+(j)*g_frameInWidth];
						}
						cnt ++;
						flag = 0;
						for(k=0;k<=segcnt;k++)
						{
							if(labels[k]==g_frames[g_frameIndex].SegLabels0[i+1+(j)*g_frameInWidth])
							{
								flag = 1;
								break;
							}
						}
						if(flag!=1)
						{
							labels[segcnt]=g_frames[g_frameIndex].SegLabels0[i+1+(j)*g_frameInWidth];
							segcnt++;
						}
					}

					// left
					if((i-1>=0)&&g_frames[g_frameIndex].SegLabels0[i-1+(j)*g_frameInWidth]>g_frameWidth*g_frameHeight)
					{
						#ifdef NEWAUTOSEG
						r1 = g_regionr[g_frames[g_frameIndex].OrgLabels0[i-1+(j)*g_frameInWidth]];
						g1 = g_regiong[g_frames[g_frameIndex].OrgLabels0[i-1+(j)*g_frameInWidth]];
						b1 = g_regionb[g_frames[g_frameIndex].OrgLabels0[i-1+(j)*g_frameInWidth]];
						#else
						r1 = g_rgborginput[(i-1)*3+(j)*g_frameInWidth*3];
						g1 = g_rgborginput[(i-1)*3+1+(j)*g_frameInWidth*3];
						b1 = g_rgborginput[(i-1)*3+2+(j)*g_frameInWidth*3];
						#endif
						cost = abs((int)r0-(int)r1)+abs((int)g0-(int)g1)+abs((int)b0-(int)b1);
						if(cost<mincost)
						{
							mincost = cost;
							minlabel = g_frames[g_frameIndex].SegLabels0[i-1+(j)*g_frameInWidth];
						}
						cnt ++;
						flag = 0;
						for(k=0;k<=segcnt;k++)
						{
							if(labels[k]==g_frames[g_frameIndex].SegLabels0[i-1+(j)*g_frameInWidth])
							{
								flag = 1;
								break;
							}
						}
						if(flag!=1)
						{
							labels[segcnt]=g_frames[g_frameIndex].SegLabels0[i-1+(j)*g_frameInWidth];
							segcnt++;
						}
					}

					// top right
					if((i+1<g_frameInWidth)&&(j-1>=0)&&g_frames[g_frameIndex].SegLabels0[i+1+(j-1)*g_frameInWidth]>g_frameWidth*g_frameHeight)
					{
						#ifdef NEWAUTOSEG
						r1 = g_regionr[g_frames[g_frameIndex].OrgLabels0[i+1+(j-1)*g_frameInWidth]];
						g1 = g_regiong[g_frames[g_frameIndex].OrgLabels0[i+1+(j-1)*g_frameInWidth]];
						b1 = g_regionb[g_frames[g_frameIndex].OrgLabels0[i+1+(j-1)*g_frameInWidth]];
						#else
						r1 = g_rgborginput[(i+1)*3+(j-1)*g_frameInWidth*3];
						g1 = g_rgborginput[(i+1)*3+1+(j-1)*g_frameInWidth*3];
						b1 = g_rgborginput[(i+1)*3+2+(j-1)*g_frameInWidth*3];
						#endif
						cost = abs((int)r0-(int)r1)+abs((int)g0-(int)g1)+abs((int)b0-(int)b1);
						if(cost<mincost)
						{
							mincost = cost;
							minlabel = g_frames[g_frameIndex].SegLabels0[i+1+(j-1)*g_frameInWidth];
						}
						cnt ++;
						flag = 0;
						for(k=0;k<=segcnt;k++)
						{
							if(labels[k]==g_frames[g_frameIndex].SegLabels0[i+1+(j-1)*g_frameInWidth])
							{
								flag = 1;
								break;
							}
						}
						if(flag!=1)
						{
							labels[segcnt]=g_frames[g_frameIndex].SegLabels0[i+1+(j-1)*g_frameInWidth];
							segcnt++;
						}
					}
					
					// top
					if((j-1>=0)&&g_frames[g_frameIndex].SegLabels0[i+(j-1)*g_frameInWidth]>g_frameWidth*g_frameHeight)
					{
						#ifdef NEWAUTOSEG
						r1 = g_regionr[g_frames[g_frameIndex].OrgLabels0[i+(j-1)*g_frameInWidth]];
						g1 = g_regiong[g_frames[g_frameIndex].OrgLabels0[i+(j-1)*g_frameInWidth]];
						b1 = g_regionb[g_frames[g_frameIndex].OrgLabels0[i+(j-1)*g_frameInWidth]];
						#else
						r1 = g_rgborginput[(i)*3+(j-1)*g_frameInWidth*3];
						g1 = g_rgborginput[(i)*3+1+(j-1)*g_frameInWidth*3];
						b1 = g_rgborginput[(i)*3+2+(j-1)*g_frameInWidth*3];
						#endif
						cost = abs((int)r0-(int)r1)+abs((int)g0-(int)g1)+abs((int)b0-(int)b1);
						if(cost<mincost)
						{
							mincost = cost;
							minlabel = g_frames[g_frameIndex].SegLabels0[i+(j-1)*g_frameInWidth];
						}
						cnt ++;
						flag = 0;
						for(k=0;k<=segcnt;k++)
						{
							if(labels[k]==g_frames[g_frameIndex].SegLabels0[i+(j-1)*g_frameInWidth])
							{
								flag = 1;
								break;
							}
						}
						if(flag!=1)
						{
							labels[segcnt]=g_frames[g_frameIndex].SegLabels0[i+(j-1)*g_frameInWidth];
							segcnt++;
						}
					}
					
					// top left
					if((i-1>=0)&&(j-1>=0)&&g_frames[g_frameIndex].SegLabels0[i-1+(j-1)*g_frameInWidth]>g_frameWidth*g_frameHeight)
					{
						#ifdef NEWAUTOSEG
						r1 = g_regionr[g_frames[g_frameIndex].OrgLabels0[i-1+(j-1)*g_frameInWidth]];
						g1 = g_regiong[g_frames[g_frameIndex].OrgLabels0[i-1+(j-1)*g_frameInWidth]];
						b1 = g_regionb[g_frames[g_frameIndex].OrgLabels0[i-1+(j-1)*g_frameInWidth]];
						#else
						r1 = g_rgborginput[(i-1)*3+(j-1)*g_frameInWidth*3];
						g1 = g_rgborginput[(i-1)*3+1+(j-1)*g_frameInWidth*3];
						b1 = g_rgborginput[(i-1)*3+2+(j-1)*g_frameInWidth*3];
						#endif
						cost = abs((int)r0-(int)r1)+abs((int)g0-(int)g1)+abs((int)b0-(int)b1);
						if(cost<mincost)
						{
							mincost = cost;
							minlabel = g_frames[g_frameIndex].SegLabels0[i-1+(j-1)*g_frameInWidth];
						}
						cnt ++;
						flag = 0;
						for(k=0;k<=segcnt;k++)
						{
							if(labels[k]==g_frames[g_frameIndex].SegLabels0[i-1+(j-1)*g_frameInWidth])
							{
								flag = 1;
								break;
							}
						}
						if(flag!=1)
						{
							labels[segcnt]=g_frames[g_frameIndex].SegLabels0[i-1+(j-1)*g_frameInWidth];
							segcnt++;
						}
					}
					
					if(segcnt==0)
					{
						// do nothing
					}
					else if(segcnt==1)
					{
						if(cnt>=5||mincost<g_segparamautoedgeth)
						{
							g_frames[g_frameIndex].SegLabels0[i+j*g_frameInWidth] = minlabel;
						}
					}
					else // segcnt >1
					{
						g_frames[g_frameIndex].SegLabels0[i+j*g_frameInWidth] = minlabel;
					}
				}
			}

			LabelResize(g_frames[g_frameIndex].SegLabels0, g_frames[g_frameIndex].SegLabels, g_yuvorginput.y, g_yuvinput.y, g_frameInWidth, g_frameInHeight, g_frameWidth, g_frameHeight);
		}
		
		

		if(g_enlargeflag == 1)
		{
			SegEnlargedisplay(g_frames[g_frameIndex].SegLabelMask);
		}
		else
		{
			ShowSegandDep(g_frames[g_frameIndex].SegLabelMask);
		}
	}
}

int SegmentCheck()
{
	int i;
	int cnt = 0;
	char str[128];

	if(g_frameMaxNum>0&&g_frames[g_frameIndex].SegFlag==1)
	{
		if(g_frameResizeFlag==1)
		{
			for(i=0;i<g_frameInWidth*g_frameInHeight;i++)
			{
				if(g_frames[g_frameIndex].SegLabels0[i]<g_frameWidth*g_frameHeight)
					cnt ++;
			}
		}
		else
		{
			for(i=0;i<g_frameWidth*g_frameHeight;i++)
			{
				if(g_frames[g_frameIndex].SegLabels[i]<g_frameWidth*g_frameHeight)
					cnt ++;
			}
		}
		#ifdef USE_CHINESE
		sprintf_s(str, "%d个像素没有被手工分割",cnt);
		#else
		sprintf_s(str, "%d pixels didn't hand segmented",cnt);
		#endif

		messagebox(INFO_STRINGS);

		//savesegmentframe();
	}

	return cnt;
}

int SegmentCheckSilent()
{
	int i;
	int cnt = 0;

	if(g_frameMaxNum>0&&g_frames[g_frameIndex].SegFlag==1)
	{
		if(g_frameResizeFlag==1)
		{
			for(i=0;i<g_frameInWidth*g_frameInHeight;i++)
			{
				if(g_frames[g_frameIndex].SegLabels0[i]<g_frameWidth*g_frameHeight)
					cnt ++;
			}
		}
		else
		{
			for(i=0;i<g_frameWidth*g_frameHeight;i++)
			{
				if(g_frames[g_frameIndex].SegLabels[i]<g_frameWidth*g_frameHeight)
					cnt ++;
			}
		}
	}

	return cnt;
}

void Automenuseg()
{

	int cnt = 0;
	
	if(g_frameMaxNum>0&&g_frames[g_frameIndex].SegFlag==1)
	{
		cnt = SegmentCheckSilent();
		if(cnt<100)
		{
			AutoPixelSeg();
		}
		else
		{
			if(g_frameResizeFlag==1)
			{
				#if 0
				AutoRegionSeg(g_frames[g_frameIndex].SegLabels0, g_frames[g_frameIndex].OrgLabels0, g_rgborginput, g_frameInWidth, g_frameInHeight, g_frames[g_frameIndex].AutoSegLabel);
				LabelResize(g_frames[g_frameIndex].SegLabels0,g_frames[g_frameIndex].SegLabels,g_yuvorginput.y, g_yuvinput.y, g_frameInWidth,g_frameInHeight,g_frameWidth,g_frameHeight);
				#else // 20120322
				//LabelResize(g_frames[g_frameIndex].SegLabels0,g_frames[g_frameIndex].SegLabels,g_yuvorginput.y, g_yuvinput.y, g_frameInWidth,g_frameInHeight,g_frameWidth,g_frameHeight);
				AutoRegionSeg(g_frames[g_frameIndex].SegLabels0, g_frames[g_frameIndex].OrgLabels0, g_rgborginput, g_frameInWidth, g_frameInHeight, g_segparamnumregion+10);
				LabelResize(g_frames[g_frameIndex].SegLabels0,g_frames[g_frameIndex].SegLabels,g_yuvorginput.y, g_yuvinput.y, g_frameInWidth,g_frameInHeight,g_frameWidth,g_frameHeight);
				//LabelResize(g_frames[g_frameIndex].SegLabels0,g_frames[g_frameIndex].SegLabels,g_yuvorginput.y, g_yuvinput.y, g_frameInWidth,g_frameInHeight,g_frameWidth,g_frameHeight);
				#endif
			}
			else
			{
				AutoRegionSeg(g_frames[g_frameIndex].SegLabels, g_frames[g_frameIndex].OrgLabels, g_rgbinput, g_frameWidth, g_frameHeight, g_segparamnumregion+10);
			}

			if(g_enlargeflag == 1)
			{
				SegEnlargedisplay(g_frames[g_frameIndex].SegLabelMask);
			}
			else
			{
				ShowSegandDep(g_frames[g_frameIndex].SegLabelMask);
			}
		}
	}

	return ;
}



void savesegmenttmp()
{
	int i;

	//printf("save segment....\n");
	
	if(g_frames[g_frameIndex].SegFlag==1)
	{
		if(g_frameResizeFlag==1)
		{
			for(i=0;i<g_frameInWidth*g_frameInHeight;i++)
			{
				g_tmpOrgLabels0[i] = g_frames[g_frameIndex].OrgLabels0[i];
				g_tmpSegLabels0[i] = g_frames[g_frameIndex].SegLabels0[i];
				g_tmpSegDepths0[i] = g_frames[g_frameIndex].SegDepths0[i];
			}
		}
		
		for(i=0;i<g_frameWidth*g_frameHeight;i++)
		{
			g_tmpOrgLabels[i] = g_frames[g_frameIndex].OrgLabels[i];
			g_tmpSegLabels[i] = g_frames[g_frameIndex].SegLabels[i];
			g_tmpSegDepths[i] = g_frames[g_frameIndex].SegDepths[i];
		}

		g_tmpframeIndex = g_frameIndex;
	}
	
	return ;
}

void loadsegmenttmp()
{
	int i;

	if(g_frameMaxNum>0)
	{
		if(g_frames[g_frameIndex].SegFlag==1&&g_tmpframeIndex==g_frameIndex)
		{
			if(g_frameResizeFlag==1)
			{
				for(i=0;i<g_frameInWidth*g_frameInHeight;i++)
				{
					g_frames[g_frameIndex].OrgLabels0[i] = g_tmpOrgLabels0[i];
					g_frames[g_frameIndex].SegLabels0[i] = g_tmpSegLabels0[i];
					g_frames[g_frameIndex].SegDepths0[i] = g_tmpSegDepths0[i];
				}
			}
			
			for(i=0;i<g_frameWidth*g_frameHeight;i++)
			{
				g_frames[g_frameIndex].OrgLabels[i] = g_tmpOrgLabels[i];
				g_frames[g_frameIndex].SegLabels[i] = g_tmpSegLabels[i];
				g_frames[g_frameIndex].SegDepths[i] = g_tmpSegDepths[i];
			}
		}
		
		if(g_enlargeflag == 1)
		{
			SegEnlargedisplay(g_frames[g_frameIndex].SegLabelMask);
		}
		else
		{
			ShowSegandDep(g_frames[g_frameIndex].SegLabelMask);
		}
	}
	
	return ;
}

void loadsegmenttmp1()
{	
	return ;
}



/* 清除边缘部分标记,注意保留单个标记*/
void ClearEdgeLabels(int * Labels, int * oldlabels, int width, int height, int edge)
{
	int i,j,k;
	int cnt;
	int labelth = g_frameWidth*g_frameHeight;

	int * tmplabels = new int[width*height];

	for(j=0;j<height;j++)
		for(i=0;i<width;i++)
		{
			tmplabels[i+j*width] = Labels[i+j*width];
		}

	for(k=0;k<edge;k++)
	{
		for(j=k+1;j<height-k-1;j++)
			for(i=k+1;i<width-k-1;i++)
			{
				if(Labels[i+j*width]>labelth)
				{
					cnt = 0;
					if(Labels[i+j*width]!=Labels[i-1+j*width])
						cnt ++;
					if(Labels[i+j*width]!=Labels[i+1+j*width])
						cnt ++;
					if(Labels[i+j*width]!=Labels[i+(j-1)*width])
						cnt ++;
					if(Labels[i+j*width]!=Labels[i+(j+1)*width])
						cnt ++;

					if(cnt>0&&cnt<4)
						tmplabels[i+j*width] = oldlabels[i+j*width]; // TODO 设为原来值
				}
			}

		for(j=0;j<height;j++)
			for(i=0;i<width;i++)
			{
				Labels[i+j*width] = tmplabels[i+j*width];
			}

	}

	delete [] tmplabels;
	
	return ;
}

void Setlabel(int * labels, int oldlabel, int newlabel, int sz)
{
	int i;

	for(i=0;i<sz;i++)
	{
		if(labels[i]==oldlabel)
			labels[i] = newlabel;
	}
	
	return ;
}

void SetOldlabel(int * labels, int * oldlabels, int oldlabel, int newlabel, int sz)
{
	int i;

	for(i=0;i<sz;i++)
	{
		if(labels[i]==oldlabel&&oldlabels[i]==newlabel)
			labels[i] = newlabel;
	}
	
	return ;
}

void SetOldlabelMask(int * labels, int * oldlabels, int mask, int x, int y, int width, int height)
{
	int i, j;
	int sz = g_frameWidth*g_frameHeight;
	int lb = labels[x+y*width];
	int olb = oldlabels[x+y*width];
	

	for(j=0;j<height;j++)
	for(i=0;i<width;i++)
	{
		if(lb==labels[i+j*width]&&(((labels[i+j*width]-sz)&mask)!=0)&&olb==oldlabels[i+j*width])
			labels[i+j*width] = olb;
	}
	
	return ;
}

void SetOldlabelMask0(int * labels0, int * oldlabels0, int * labels, int * oldlabels, int mask, int x, int y)
{
	int i, j;
	int sz = g_frameWidth*g_frameHeight;
	int lb, olb;

	if(g_enlargeflag==0)
	{
		lb = labels[x+y*g_frameWidth];
		olb = oldlabels[x+y*g_frameWidth];
	}
	else
	{
		lb = labels0[x+y*g_frameInWidth];
		olb = oldlabels0[x+y*g_frameInWidth];
	}
	

	for(j=0;j<g_frameHeight;j++)
	for(i=0;i<g_frameWidth;i++)
	{
		if(lb==labels[i+j*g_frameWidth]&&(((labels[i+j*g_frameWidth]-sz)&mask)!=0)&&olb==oldlabels[i+j*g_frameWidth])
			labels[i+j*g_frameWidth] = olb;
	}

	for(j=0;j<g_frameInHeight;j++)
	for(i=0;i<g_frameInWidth;i++)
	{
		if(lb==labels0[i+j*g_frameInWidth]&&(((labels0[i+j*g_frameInWidth]-sz)&mask)!=0)&&olb==oldlabels0[i+j*g_frameInWidth])
			labels0[i+j*g_frameInWidth] = olb;
	}
	
	return ;
}



void SetOldlabelRegion(int * labels, int * oldlabels, int mask, int x, int y)
{
	int i, j, ii, jj;
	int dis;
	int sz = g_frameWidth*g_frameHeight;
	int * orglabels = g_frames[g_frameIndex].SegLabels0;
	int * orgoldlabels = g_frames[g_frameIndex].OrgLabels0;

	switch(g_segeditmodule)
	{
		case SEGEDITMODULESQU:
			for(j=-g_segeditmodulerad;j<=g_segeditmodulerad;j++)
				for(i=-g_segeditmodulerad;i<=g_segeditmodulerad;i++)
				{
					ii=x+i;
					jj=y+j;
					ii = H3D_CLIP3(ii,0,g_frameWidth-1);
					jj = H3D_CLIP3(jj,0,g_frameHeight-1);
					if((labels[ii+jj*g_frameWidth]>sz)&&(((labels[ii+jj*g_frameWidth]-sz)&mask)!=0))
					{
						labels[ii+jj*g_frameWidth] = oldlabels[ii+jj*g_frameWidth];
					}
				}
			break;
		
		case SEGEDITMODULECIR:
		default:
			for(j=-g_segeditmodulerad;j<=g_segeditmodulerad;j++)
				for(i=-g_segeditmodulerad;i<=g_segeditmodulerad;i++)
				{
					ii=x+i;
					jj=y+j;
					dis = (int)sqrt((double)(i*i+j*j));
					if(ii>=0&&ii<g_frameWidth&&jj>=0&&jj<g_frameHeight&&dis<=g_segeditmodulerad&&(labels[ii+jj*g_frameWidth]>sz)&&(((labels[ii+jj*g_frameWidth]-sz)&mask)!=0))
					{
						labels[ii+jj*g_frameWidth] = oldlabels[ii+jj*g_frameWidth];
					}
				}		
			break;
	}
	
	return ;
}

void SetOldlabelRegion0(int * labels, int * oldlabels, int mask, int x, int y)
{
	int i, j, ii, jj, iii,jjj;
	int dis;
	int sz = g_frameWidth*g_frameHeight;
	int * dislabels = g_frames[g_frameIndex].SegLabels;
	int rad = g_enlargeflag==1?g_segeditmodulerad:(g_segeditmodulerad*g_frameInWidth/g_frameWidth);

	switch(g_segeditmodule)
	{
		case SEGEDITMODULESQU:
			for(j=-rad;j<=rad;j++)
				for(i=-rad;i<=rad;i++)
				{
					ii=x+i;
					jj=y+j;
					ii = H3D_CLIP3(ii,0,g_frameInWidth-1);
					jj = H3D_CLIP3(jj,0,g_frameInHeight-1);
					if((labels[ii+jj*g_frameInWidth]>sz)&&(((labels[ii+jj*g_frameInWidth]-sz)&mask)!=0))
					{
						labels[ii+jj*g_frameInWidth] = oldlabels[ii+jj*g_frameInWidth];
						jjj = jj*g_frameHeight/g_frameInHeight;
						iii = ii*g_frameWidth/g_frameInWidth;
						dislabels[iii+jjj*g_frameWidth] = oldlabels[ii+jj*g_frameInWidth];
					}
				}
			break;
		
		case SEGEDITMODULECIR:
		default:
			for(j=-rad;j<=rad;j++)
				for(i=-rad;i<=rad;i++)
				{
					ii=x+i;
					jj=y+j;
					dis = (int)sqrt((double)(i*i+j*j));
					if(ii>=0&&ii<g_frameInWidth&&jj>=0&&jj<g_frameInHeight&&dis<=rad&&(labels[ii+jj*g_frameInWidth]>sz)&&(((labels[ii+jj*g_frameInWidth]-sz)&mask)!=0))
					{
						labels[ii+jj*g_frameInWidth] = oldlabels[ii+jj*g_frameInWidth];
						jjj = jj*g_frameHeight/g_frameInHeight;
						iii = ii*g_frameWidth/g_frameInWidth;
						dislabels[iii+jjj*g_frameWidth] = oldlabels[ii+jj*g_frameInWidth];
					}
				}		
			break;
	}
	
	return ;
}


void SegEditLabel(int * labels, int newlabel, int x, int y)
{
	int i, j, ii, jj;
	int dis;
	int * orglabels = g_frames[g_frameIndex].SegLabels0;
	
	switch(g_segeditmodule)
	{
		case SEGEDITMODULESQU:
			for(j=-g_segeditmodulerad;j<=g_segeditmodulerad;j++)
				for(i=-g_segeditmodulerad;i<=g_segeditmodulerad;i++)
				{
					ii=x+i;
					jj=y+j;
					ii = H3D_CLIP3(ii,0,g_frameWidth-1);
					jj = H3D_CLIP3(jj,0,g_frameHeight-1);
					labels[ii+jj*g_frameWidth] = newlabel;
				}
			break;
		
		case SEGEDITMODULECIR:
		default:
			for(j=-g_segeditmodulerad;j<=g_segeditmodulerad;j++)
				for(i=-g_segeditmodulerad;i<=g_segeditmodulerad;i++)
				{
					ii=x+i;
					jj=y+j;
					dis = (int)sqrt((double)(i*i+j*j));
					if(ii>=0&&ii<g_frameWidth&&jj>=0&&jj<g_frameHeight&&dis<=g_segeditmodulerad)
					{
						labels[ii+jj*g_frameWidth] = newlabel;
					}
				}		
			break;
	}
	return ;
}

void SegEditLabel0(int * labels, int newlabel, int x, int y)
{
	int i, j, ii, jj, iii, jjj;
	int dis;
	int * dislabels = g_frames[g_frameIndex].SegLabels;
	int rad = g_enlargeflag==1?g_segeditmodulerad:(g_segeditmodulerad*g_frameInWidth/g_frameWidth);

	switch(g_segeditmodule)
	{
		case SEGEDITMODULESQU:
			for(j=-rad;j<=rad;j++)
				for(i=-rad;i<=rad;i++)
				{
					ii=x+i;
					jj=y+j;
					ii = H3D_CLIP3(ii,0,g_frameInWidth-1);
					jj = H3D_CLIP3(jj,0,g_frameInHeight-1);
					labels[ii+jj*g_frameInWidth] = newlabel;
					jjj = jj*g_frameHeight/g_frameInHeight;
					iii = ii*g_frameWidth/g_frameInWidth;
					dislabels[iii+jjj*g_frameWidth] = newlabel;
				}
			break;
		
		case SEGEDITMODULECIR:
		default:
			for(j=-rad;j<=rad;j++)
				for(i=-rad;i<=rad;i++)
				{
					ii=x+i;
					jj=y+j;
					dis = (int)sqrt((double)(i*i+j*j));
					if(ii>=0&&ii<g_frameInWidth&&jj>=0&&jj<g_frameInHeight&&dis<=rad)
					{
						labels[ii+jj*g_frameInWidth] = newlabel;
						jjj = jj*g_frameHeight/g_frameInHeight;
						iii = ii*g_frameWidth/g_frameInWidth;
						dislabels[iii+jjj*g_frameWidth] = newlabel;
					}
				}		
			break;
	}

	return ;
}

void SetMatSelxy(int x, int y)
{
	g_matselflag = 1;
	g_mateditx = x;
	g_matedity = y;
	//printf("x: %d, y: %d r: %d g: %d, b: %d\n", x, y, g_rgborginput[g_mateditx*3+g_matedity*g_frameInWidth*3], g_rgborginput[g_mateditx*3+1+g_matedity*g_frameInWidth*3], g_rgborginput[g_mateditx*3+2+g_matedity*g_frameInWidth*3]);
}

void SegMattingLabel(int * labels, int * oldlabels, int x, int y, int width, int height)
{
	int i, j, ii, jj;
	int dis;
	int rad = g_segeditmodulerad;
	int diff;
	int r,g,b, rs, gs, bs, lb, maxr, maxg, maxb, minr, ming, minb, th;

	if(g_matselflag==1)
	{
		rs = g_rgbinput[g_mateditx*3+g_matedity*g_frameWidth*3];
		gs = g_rgbinput[g_mateditx*3+1+g_matedity*g_frameWidth*3];
		bs = g_rgbinput[g_mateditx*3+2+g_matedity*g_frameWidth*3];
		lb = labels[g_mateditx+g_matedity*width];
	}
	else
	{
		rs = g_rgbinput[x*3+y*g_frameWidth*3];
		gs = g_rgbinput[x*3+1+y*g_frameWidth*3];
		bs = g_rgbinput[x*3+2+y*g_frameWidth*3];
		lb = labels[x+y*width];
	}
	
	switch(g_segeditmodule)
	{
		case SEGEDITMODULESQU:
			maxr = maxg = maxb = 0;
			minr = ming = minb = 255;
			for(j=-rad;j<=rad;j++)
				for(i=-rad;i<=rad;i++)
				{
					ii=x+i;
					jj=y+j;
					ii = H3D_CLIP3(ii,0,g_frameWidth-1);
					jj = H3D_CLIP3(jj,0,g_frameHeight-1);

					r = g_rgbinput[ii*3+jj*g_frameWidth*3];
					g = g_rgbinput[ii*3+1+jj*g_frameWidth*3];
					b = g_rgbinput[ii*3+2+jj*g_frameWidth*3];

					if(r>maxr) maxr = r;
					if(g>maxg) maxg = g;
					if(b>maxb) maxb = b;
					if(r<minr) minr = r;
					if(g<ming) ming = g;
					if(b<minb) minb = b;
				}
				
			th = abs(maxr-minr) + abs(maxg-ming) + abs(maxb-minb);
			
			for(j=-rad;j<=rad;j++)
				for(i=-rad;i<=rad;i++)
				{
					ii=x+i;
					jj=y+j;
					ii = H3D_CLIP3(ii,0,g_frameWidth-1);
					jj = H3D_CLIP3(jj,0,g_frameHeight-1);

					r = g_rgbinput[ii*3+jj*g_frameWidth*3];
					g = g_rgbinput[ii*3+1+jj*g_frameWidth*3];
					b = g_rgbinput[ii*3+2+jj*g_frameWidth*3];

					diff = abs(r-rs) + abs(g-gs) + abs(b-bs);
					
					if(diff<g_segparamautoedgeth*AUTHSTEP||diff<th/3)
					{
						labels[ii+jj*width] = lb;
					}
				}
			break;
		
		case SEGEDITMODULECIR:
		default:
			maxr = maxg = maxb = 0;
			minr = ming = minb = 255;
			for(j=-rad;j<=rad;j++)
				for(i=-rad;i<=rad;i++)
				{
					ii=x+i;
					jj=y+j;
					dis = (int)sqrt((double)(i*i+j*j));
					if(ii>=0&&ii<g_frameWidth&&jj>=0&&jj<g_frameHeight&&dis<=rad)
					{
						r = g_rgbinput[ii*3+jj*g_frameWidth*3];
						g = g_rgbinput[ii*3+1+jj*g_frameWidth*3];
						b = g_rgbinput[ii*3+2+jj*g_frameWidth*3];

						if(r>maxr) maxr = r;
						if(g>maxg) maxg = g;
						if(b>maxb) maxb = b;
						if(r<minr) minr = r;
						if(g<ming) ming = g;
						if(b<minb) minb = b;
					}
				}
			th = abs(maxr-minr) + abs(maxg-ming) + abs(maxb-minb);
			for(j=-rad;j<=rad;j++)
				for(i=-rad;i<=rad;i++)
				{
					ii=x+i;
					jj=y+j;
					dis = (int)sqrt((double)(i*i+j*j));
					if(ii>=0&&ii<g_frameWidth&&jj>=0&&jj<g_frameHeight&&dis<=rad)
					{
						r = g_rgbinput[ii*3+jj*g_frameWidth*3];
						g = g_rgbinput[ii*3+1+jj*g_frameWidth*3];
						b = g_rgbinput[ii*3+2+jj*g_frameWidth*3];

						diff = abs(r-rs) + abs(g-gs) + abs(b-bs);
					
						if(diff<g_segparamautoedgeth*AUTHSTEP||diff<th/3)
						{
							labels[ii+jj*width] = lb;
						}
					}
				}		
			break;
	}
	return ;
}


void SegMattingLabel0(int * labels, int * oldlabels, int x, int y, int width, int height)
{
	int i, j, ii, jj, iii, jjj;
	int dis;
	int * dislabels = g_frames[g_frameIndex].SegLabels;
	int rad = g_enlargeflag==1?g_segeditmodulerad:(g_segeditmodulerad*g_frameInWidth/g_frameWidth);
	int diff;
	int r,g,b, rs, gs, bs, lb, maxr, maxg, maxb, minr, ming, minb, th;

	if(g_matselflag==1)
	{
		rs = g_rgborginput[g_mateditx*3+g_matedity*g_frameInWidth*3];
		gs = g_rgborginput[g_mateditx*3+1+g_matedity*g_frameInWidth*3];
		bs = g_rgborginput[g_mateditx*3+2+g_matedity*g_frameInWidth*3];
		lb = labels[g_mateditx+g_matedity*width];
	}
	else
	{
		rs = g_rgborginput[x*3+y*g_frameInWidth*3];
		gs = g_rgborginput[x*3+1+y*g_frameInWidth*3];
		bs = g_rgborginput[x*3+2+y*g_frameInWidth*3];
		lb = labels[x+y*width];
	}
	//printf("g_matselflag: %d  r: %d, g: %d, b: %d l: %x\n", g_matselflag, rs, gs, bs, lb);
	
	switch(g_segeditmodule)
	{
		case SEGEDITMODULESQU:
			maxr = maxg = maxb = 0;
			minr = ming = minb = 255;
			for(j=-rad;j<=rad;j++)
				for(i=-rad;i<=rad;i++)
				{
					ii=x+i;
					jj=y+j;
					ii = H3D_CLIP3(ii,0,g_frameInWidth-1);
					jj = H3D_CLIP3(jj,0,g_frameInHeight-1);

					r = g_rgborginput[ii*3+jj*g_frameInWidth*3];
					g = g_rgborginput[ii*3+1+jj*g_frameInWidth*3];
					b = g_rgborginput[ii*3+2+jj*g_frameInWidth*3];

					if(r>maxr) maxr = r;
					if(g>maxg) maxg = g;
					if(b>maxb) maxb = b;
					if(r<minr) minr = r;
					if(g<ming) ming = g;
					if(b<minb) minb = b;
				}
				
			th = abs(maxr-minr) + abs(maxg-ming) + abs(maxb-minb);
			for(j=-rad;j<=rad;j++)
				for(i=-rad;i<=rad;i++)
				{
					ii=x+i;
					jj=y+j;
					ii = H3D_CLIP3(ii,0,g_frameInWidth-1);
					jj = H3D_CLIP3(jj,0,g_frameInHeight-1);

					r = g_rgborginput[ii*3+jj*g_frameInWidth*3];
					g = g_rgborginput[ii*3+1+jj*g_frameInWidth*3];
					b = g_rgborginput[ii*3+2+jj*g_frameInWidth*3];

					diff = abs(r-rs) + abs(g-gs) + abs(b-bs);
					
					if(diff<g_segparamautoedgeth*AUTHSTEP||diff<th/3)
					{
						labels[ii+jj*width] = lb;
						jjj = jj*g_frameHeight/g_frameInHeight;
						iii = ii*g_frameWidth/g_frameInWidth;
						dislabels[iii+jjj*g_frameWidth] = lb;
					}
				}
			break;
		
		case SEGEDITMODULECIR:
		default:
			maxr = maxg = maxb = 0;
			minr = ming = minb = 255;
			for(j=-rad;j<=rad;j++)
				for(i=-rad;i<=rad;i++)
				{
					ii=x+i;
					jj=y+j;
					dis = (int)sqrt((double)(i*i+j*j));
					if(ii>=0&&ii<g_frameInWidth&&jj>=0&&jj<g_frameInHeight&&dis<=rad)
					{
						r = g_rgborginput[ii*3+jj*g_frameInWidth*3];
						g = g_rgborginput[ii*3+1+jj*g_frameInWidth*3];
						b = g_rgborginput[ii*3+2+jj*g_frameInWidth*3];

						if(r>maxr) maxr = r;
						if(g>maxg) maxg = g;
						if(b>maxb) maxb = b;
						if(r<minr) minr = r;
						if(g<ming) ming = g;
						if(b<minb) minb = b;
					}
				}
			th = abs(maxr-minr) + abs(maxg-ming) + abs(maxb-minb);
			//printf("th: %d\n", th);
			
			for(j=-rad;j<=rad;j++)
				for(i=-rad;i<=rad;i++)
				{
					ii=x+i;
					jj=y+j;
					dis = (int)sqrt((double)(i*i+j*j));
					if(ii>=0&&ii<g_frameInWidth&&jj>=0&&jj<g_frameInHeight&&dis<=rad)
					{
						r = g_rgborginput[ii*3+jj*g_frameInWidth*3];
						g = g_rgborginput[ii*3+1+jj*g_frameInWidth*3];
						b = g_rgborginput[ii*3+2+jj*g_frameInWidth*3];

						diff = abs(r-rs) + abs(g-gs) + abs(b-bs);
						//printf("diff: %d  r: %d, g: %d, b: %d \n", diff, r, g, b);
					
						if(diff<g_segparamautoedgeth*AUTHSTEP||diff<th/3)
						{
							labels[ii+jj*width] = lb;
							jjj = jj*g_frameHeight/g_frameInHeight;
							iii = ii*g_frameWidth/g_frameInWidth;
							dislabels[iii+jjj*g_frameWidth] = lb;
							//printf("#");
						}
					}
				}		
			break;
	}
	return ;
}



void DrawSegEditContour(int x, int y, int ex, int ey, int region)
{
	int i, j, ii, jj;
	int dis;
	int Segoffset = g_frameWidth*3;
	int Manoffset = g_frameHeight*g_frameWidth*6;
	int Depoffset = g_frameHeight*g_frameWidth*6+g_frameWidth*3;

	if(g_enlargeflag==0)
	{
		switch(g_segeditmodule)
		{
			case SEGEDITMODULESQU:
				#if 1
				for(j=0;j<g_frameHeight;j++)
					for(i=0;i<g_frameWidth;i++)
					{
						if((abs(i-x)==g_segeditmodulerad&&abs(j-y)<=g_segeditmodulerad)||(abs(j-y)==g_segeditmodulerad&&abs(i-x)<=g_segeditmodulerad))
						{
							// REGIONORG
							g_tmpdisplay[i*3+j*g_frameWidth*6] = 255;
							g_tmpdisplay[i*3+1+j*g_frameWidth*6] = 0;
							g_tmpdisplay[i*3+2+j*g_frameWidth*6] = 0;
							// REGIONSEG
							g_tmpdisplay[i*3+j*g_frameWidth*6+Segoffset] = 0;
							g_tmpdisplay[i*3+1+j*g_frameWidth*6+Segoffset] = 0;
							g_tmpdisplay[i*3+2+j*g_frameWidth*6+Segoffset] = 255;
							// REGIONMAN
							g_tmpdisplay[i*3+j*g_frameWidth*6+Manoffset] = 0;
							g_tmpdisplay[i*3+1+j*g_frameWidth*6+Manoffset] = 255;
							g_tmpdisplay[i*3+2+j*g_frameWidth*6+Manoffset] = 0;
							// REGIONDEP
							g_tmpdisplay[i*3+j*g_frameWidth*6+Depoffset] = 255;
							g_tmpdisplay[i*3+1+j*g_frameWidth*6+Depoffset] = 255;
							g_tmpdisplay[i*3+2+j*g_frameWidth*6+Depoffset] = 255;
						}
						else
						{
							g_tmpdisplay[i*3+j*g_frameWidth*6] = g_rgbinput[i*3+j*g_frameWidth*3];
							g_tmpdisplay[i*3+1+j*g_frameWidth*6] = g_rgbinput[i*3+1+j*g_frameWidth*3];
							g_tmpdisplay[i*3+2+j*g_frameWidth*6] = g_rgbinput[i*3+2+j*g_frameWidth*3];

							g_tmpdisplay[i*3+j*g_frameWidth*6+Segoffset] = g_autosegdisplay[i*3+j*g_frameWidth*3];
							g_tmpdisplay[i*3+1+j*g_frameWidth*6+Segoffset] = g_autosegdisplay[i*3+1+j*g_frameWidth*3];
							g_tmpdisplay[i*3+2+j*g_frameWidth*6+Segoffset] = g_autosegdisplay[i*3+2+j*g_frameWidth*3];

							g_tmpdisplay[i*3+j*g_frameWidth*6+Manoffset] = g_manusegdisplay[i*3+j*g_frameWidth*3];
							g_tmpdisplay[i*3+1+j*g_frameWidth*6+Manoffset] = g_manusegdisplay[i*3+1+j*g_frameWidth*3];
							g_tmpdisplay[i*3+2+j*g_frameWidth*6+Manoffset] = g_manusegdisplay[i*3+2+j*g_frameWidth*3];

							g_tmpdisplay[i*3+j*g_frameWidth*6+Depoffset] = g_depthdisplay[i+j*g_frameWidth];
							g_tmpdisplay[i*3+1+j*g_frameWidth*6+Depoffset] = g_depthdisplay[i+j*g_frameWidth];
							g_tmpdisplay[i*3+2+j*g_frameWidth*6+Depoffset] = g_depthdisplay[i+j*g_frameWidth];
						}
					}
				gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
			          IMGWIDTHMAX-g_frameWidth, IMGHEIGHTMAX-g_frameHeight, g_frameWidth*2, g_frameHeight*2,
			          GDK_RGB_DITHER_MAX, g_tmpdisplay, g_frameWidth * 6);
				#else
				if(region==REGIONORG)
				{
					for(j=0;j<g_frameHeight;j++)
						for(i=0;i<g_frameWidth;i++)
						{
							if((abs(i-x)==g_segeditmodulerad&&abs(j-y)<=g_segeditmodulerad)||(abs(j-y)==g_segeditmodulerad&&abs(i-x)<=g_segeditmodulerad))
							{
								g_tmpdisplay[i*3+j*g_frameWidth*3] = 255;
								g_tmpdisplay[i*3+1+j*g_frameWidth*3] = 0;
								g_tmpdisplay[i*3+2+j*g_frameWidth*3] = 0;
							}
							else
							{
								g_tmpdisplay[i*3+j*g_frameWidth*3] = g_rgbinput[i*3+j*g_frameWidth*3];
								g_tmpdisplay[i*3+1+j*g_frameWidth*3] = g_rgbinput[i*3+1+j*g_frameWidth*3];
								g_tmpdisplay[i*3+2+j*g_frameWidth*3] = g_rgbinput[i*3+2+j*g_frameWidth*3];
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
							if((abs(i-x)==g_segeditmodulerad&&abs(j-y)<=g_segeditmodulerad)||(abs(j-y)==g_segeditmodulerad&&abs(i-x)<=g_segeditmodulerad))
							{
								g_tmpdisplay[i*3+j*g_frameWidth*3] = 0;
								g_tmpdisplay[i*3+1+j*g_frameWidth*3] = 0;
								g_tmpdisplay[i*3+2+j*g_frameWidth*3] = 255;
							}
							else
							{
								g_tmpdisplay[i*3+j*g_frameWidth*3] = g_autosegdisplay[i*3+j*g_frameWidth*3];
								g_tmpdisplay[i*3+1+j*g_frameWidth*3] = g_autosegdisplay[i*3+1+j*g_frameWidth*3];
								g_tmpdisplay[i*3+2+j*g_frameWidth*3] = g_autosegdisplay[i*3+2+j*g_frameWidth*3];
							}
						}					

					gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
							  IMGWIDTHMAX, IMGHEIGHTMAX-g_frameHeight, g_frameWidth, g_frameHeight,
							  GDK_RGB_DITHER_MAX, g_tmpdisplay, g_frameWidth * 3);
				}
				if(region==REGIONMAN)
				{
					for(j=0;j<g_frameHeight;j++)
						for(i=0;i<g_frameWidth;i++)
						{
							if((abs(i-x)==g_segeditmodulerad&&abs(j-y)<=g_segeditmodulerad)||(abs(j-y)==g_segeditmodulerad&&abs(i-x)<=g_segeditmodulerad))
							{
								g_tmpdisplay[i*3+j*g_frameWidth*3] = 0;
								g_tmpdisplay[i*3+1+j*g_frameWidth*3] = 255;
								g_tmpdisplay[i*3+2+j*g_frameWidth*3] = 0;
							}
							else
							{
								g_tmpdisplay[i*3+j*g_frameWidth*3] = g_manusegdisplay[i*3+j*g_frameWidth*3];
								g_tmpdisplay[i*3+1+j*g_frameWidth*3] = g_manusegdisplay[i*3+1+j*g_frameWidth*3];
								g_tmpdisplay[i*3+2+j*g_frameWidth*3] = g_manusegdisplay[i*3+2+j*g_frameWidth*3];
							}
						}
						
					gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
			          IMGWIDTHMAX-g_frameWidth, IMGHEIGHTMAX, g_frameWidth, g_frameHeight,
			          GDK_RGB_DITHER_MAX, g_tmpdisplay, g_frameWidth * 3);
				}
				if(region==REGIONDEP)
				{
					for(j=0;j<g_frameHeight;j++)
						for(i=0;i<g_frameWidth;i++)
						{
							if((abs(i-x)==g_segeditmodulerad&&abs(j-y)<=g_segeditmodulerad)||(abs(j-y)==g_segeditmodulerad&&abs(i-x)<=g_segeditmodulerad))
							{
								g_tmpdisplay[i+j*g_frameWidth] = 255;
							}
							else
							{
								g_tmpdisplay[i+j*g_frameWidth] = g_depthdisplay[i+j*g_frameWidth];
							}
						}
						
					gdk_draw_gray_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
			          IMGWIDTHMAX, IMGHEIGHTMAX, g_frameWidth, g_frameHeight,
			          GDK_RGB_DITHER_MAX, g_tmpdisplay, g_frameWidth);
				}
				#endif
				break;
			case SEGEDITMODULECIR:
			default:
				#if 1
				for(j=0;j<g_frameHeight;j++)
					for(i=0;i<g_frameWidth;i++)
					{
						ii = i-x;
						jj = j-y;
						dis = (int)sqrt((double)(ii*ii+jj*jj));
						if(dis==g_segeditmodulerad)
						{
							// REGIONORG
							g_tmpdisplay[i*3+j*g_frameWidth*6] = 255;
							g_tmpdisplay[i*3+1+j*g_frameWidth*6] = 0;
							g_tmpdisplay[i*3+2+j*g_frameWidth*6] = 0;
							// REGIONSEG
							g_tmpdisplay[i*3+j*g_frameWidth*6+Segoffset] = 0;
							g_tmpdisplay[i*3+1+j*g_frameWidth*6+Segoffset] = 0;
							g_tmpdisplay[i*3+2+j*g_frameWidth*6+Segoffset] = 255;
							// REGIONMAN
							g_tmpdisplay[i*3+j*g_frameWidth*6+Manoffset] = 0;
							g_tmpdisplay[i*3+1+j*g_frameWidth*6+Manoffset] = 255;
							g_tmpdisplay[i*3+2+j*g_frameWidth*6+Manoffset] = 0;
							// REGIONDEP
							g_tmpdisplay[i*3+j*g_frameWidth*6+Depoffset] = 255;
							g_tmpdisplay[i*3+1+j*g_frameWidth*6+Depoffset] = 255;
							g_tmpdisplay[i*3+2+j*g_frameWidth*6+Depoffset] = 255;
						}
						else
						{
							g_tmpdisplay[i*3+j*g_frameWidth*6] = g_rgbinput[i*3+j*g_frameWidth*3];
							g_tmpdisplay[i*3+1+j*g_frameWidth*6] = g_rgbinput[i*3+1+j*g_frameWidth*3];
							g_tmpdisplay[i*3+2+j*g_frameWidth*6] = g_rgbinput[i*3+2+j*g_frameWidth*3];

							g_tmpdisplay[i*3+j*g_frameWidth*6+Segoffset] = g_autosegdisplay[i*3+j*g_frameWidth*3];
							g_tmpdisplay[i*3+1+j*g_frameWidth*6+Segoffset] = g_autosegdisplay[i*3+1+j*g_frameWidth*3];
							g_tmpdisplay[i*3+2+j*g_frameWidth*6+Segoffset] = g_autosegdisplay[i*3+2+j*g_frameWidth*3];

							g_tmpdisplay[i*3+j*g_frameWidth*6+Manoffset] = g_manusegdisplay[i*3+j*g_frameWidth*3];
							g_tmpdisplay[i*3+1+j*g_frameWidth*6+Manoffset] = g_manusegdisplay[i*3+1+j*g_frameWidth*3];
							g_tmpdisplay[i*3+2+j*g_frameWidth*6+Manoffset] = g_manusegdisplay[i*3+2+j*g_frameWidth*3];

							g_tmpdisplay[i*3+j*g_frameWidth*6+Depoffset] = g_depthdisplay[i+j*g_frameWidth];
							g_tmpdisplay[i*3+1+j*g_frameWidth*6+Depoffset] = g_depthdisplay[i+j*g_frameWidth];
							g_tmpdisplay[i*3+2+j*g_frameWidth*6+Depoffset] = g_depthdisplay[i+j*g_frameWidth];
						}
					}
				gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
			          IMGWIDTHMAX-g_frameWidth, IMGHEIGHTMAX-g_frameHeight, g_frameWidth*2, g_frameHeight*2,
			          GDK_RGB_DITHER_MAX, g_tmpdisplay, g_frameWidth * 6);
				#else
				if(region==REGIONORG)
				{
					for(j=0;j<g_frameHeight;j++)
						for(i=0;i<g_frameWidth;i++)
						{
							ii = i-x;
							jj = j-y;
							dis = (int)sqrt((double)(ii*ii+jj*jj));
							if(dis==g_segeditmodulerad)
							{
								g_tmpdisplay[i*3+j*g_frameWidth*3] = 255;
								g_tmpdisplay[i*3+1+j*g_frameWidth*3] = 0;
								g_tmpdisplay[i*3+2+j*g_frameWidth*3] = 0;
							}
							else
							{
								g_tmpdisplay[i*3+j*g_frameWidth*3] = g_rgbinput[i*3+j*g_frameWidth*3];
								g_tmpdisplay[i*3+1+j*g_frameWidth*3] = g_rgbinput[i*3+1+j*g_frameWidth*3];
								g_tmpdisplay[i*3+2+j*g_frameWidth*3] = g_rgbinput[i*3+2+j*g_frameWidth*3];
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
							if(dis==g_segeditmodulerad)
							{
								g_tmpdisplay[i*3+j*g_frameWidth*3] = 0;
								g_tmpdisplay[i*3+1+j*g_frameWidth*3] = 0;
								g_tmpdisplay[i*3+2+j*g_frameWidth*3] = 255;
							}
							else
							{
								g_tmpdisplay[i*3+j*g_frameWidth*3] = g_autosegdisplay[i*3+j*g_frameWidth*3];
								g_tmpdisplay[i*3+1+j*g_frameWidth*3] = g_autosegdisplay[i*3+1+j*g_frameWidth*3];
								g_tmpdisplay[i*3+2+j*g_frameWidth*3] = g_autosegdisplay[i*3+2+j*g_frameWidth*3];
							}
						}

					gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
				          IMGWIDTHMAX, IMGHEIGHTMAX-g_frameHeight, g_frameWidth, g_frameHeight,
				          GDK_RGB_DITHER_MAX, g_tmpdisplay, g_frameWidth * 3);
				}	
				if(region==REGIONMAN)
				{
					for(j=0;j<g_frameHeight;j++)
						for(i=0;i<g_frameWidth;i++)
						{
							ii = i-x;
							jj = j-y;
							dis = (int)sqrt((double)(ii*ii+jj*jj));
							if(dis==g_segeditmodulerad)
							{
								g_tmpdisplay[i*3+j*g_frameWidth*3] = 0;
								g_tmpdisplay[i*3+1+j*g_frameWidth*3] = 255;
								g_tmpdisplay[i*3+2+j*g_frameWidth*3] = 0;
							}
							else
							{
								g_tmpdisplay[i*3+j*g_frameWidth*3] = g_manusegdisplay[i*3+j*g_frameWidth*3];
								g_tmpdisplay[i*3+1+j*g_frameWidth*3] = g_manusegdisplay[i*3+1+j*g_frameWidth*3];
								g_tmpdisplay[i*3+2+j*g_frameWidth*3] = g_manusegdisplay[i*3+2+j*g_frameWidth*3];
							}
						}
						
					gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
			          IMGWIDTHMAX-g_frameWidth, IMGHEIGHTMAX, g_frameWidth, g_frameHeight,
			          GDK_RGB_DITHER_MAX, g_tmpdisplay, g_frameWidth * 3);
				}	
				if(region==REGIONDEP)
				{
					for(j=0;j<g_frameHeight;j++)
						for(i=0;i<g_frameWidth;i++)
						{
							ii = i-x;
							jj = j-y;
							dis = (int)sqrt((double)(ii*ii+jj*jj));
							if(dis==g_segeditmodulerad)
							{
								g_tmpdisplay[i+j*g_frameWidth] = 255;
							}
							else
							{
								g_tmpdisplay[i+j*g_frameWidth] = g_depthdisplay[i+j*g_frameWidth];
							}
						}
						
					gdk_draw_gray_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
			          IMGWIDTHMAX, IMGHEIGHTMAX, g_frameWidth, g_frameHeight,
			          GDK_RGB_DITHER_MAX, g_tmpdisplay, g_frameWidth);
				}
				#endif
				break;
		}
	}
	else
	{
		if(g_frameResizeFlag==0)
		{
			if(g_segeditmodule==SEGEDITMODULESQU)
			{
				for(j=0;j<g_frameHeight*2;j++)
					for(i=0;i<g_frameWidth*2;i++)
					{
						if((abs(i-ex)==g_segeditmodulerad*2&&abs(j-ey)<=g_segeditmodulerad*2)||(abs(j-ey)==g_segeditmodulerad*2&&abs(i-ex)<=g_segeditmodulerad*2))
						{
							g_enlargedisplaytmp1[i*3+j*g_frameWidth*6] = 0;
							g_enlargedisplaytmp1[i*3+1+j*g_frameWidth*6] = 0;
							g_enlargedisplaytmp1[i*3+2+j*g_frameWidth*6] = 255;
						}
						else
						{
							g_enlargedisplaytmp1[i*3+j*g_frameWidth*6] = g_enlargedisplaytmp0[i*3+j*g_frameWidth*6];
							g_enlargedisplaytmp1[i*3+1+j*g_frameWidth*6] = g_enlargedisplaytmp0[i*3+1+j*g_frameWidth*6];
							g_enlargedisplaytmp1[i*3+2+j*g_frameWidth*6] = g_enlargedisplaytmp0[i*3+2+j*g_frameWidth*6];
						}
					}

			}
			else
			{
				for(j=0;j<g_frameHeight*2;j++)
					for(i=0;i<g_frameWidth*2;i++)
					{
						ii = i-ex;
						jj = j-ey;
						dis = (int)sqrt((double)(ii*ii+jj*jj));
						if(dis==g_segeditmodulerad*2)
						{
							g_enlargedisplaytmp1[i*3+j*g_frameWidth*6] = 0;
							g_enlargedisplaytmp1[i*3+1+j*g_frameWidth*6] = 0;
							g_enlargedisplaytmp1[i*3+2+j*g_frameWidth*6] = 255;
						}
						else
						{
							g_enlargedisplaytmp1[i*3+j*g_frameWidth*6] = g_enlargedisplaytmp0[i*3+j*g_frameWidth*6];
							g_enlargedisplaytmp1[i*3+1+j*g_frameWidth*6] = g_enlargedisplaytmp0[i*3+1+j*g_frameWidth*6];
							g_enlargedisplaytmp1[i*3+2+j*g_frameWidth*6] = g_enlargedisplaytmp0[i*3+2+j*g_frameWidth*6];
						}
					}

				
			}
		}
		else
		{
			
			if(g_segeditmodule==SEGEDITMODULESQU)
			{
				for(j=0;j<g_frameHeight*2;j++)
					for(i=0;i<g_frameWidth*2;i++)
					{
						if((abs(i-ex)==g_segeditmodulerad&&abs(j-ey)<=g_segeditmodulerad)||(abs(j-ey)==g_segeditmodulerad&&abs(i-ex)<=g_segeditmodulerad))
						{
							g_enlargedisplaytmp1[i*3+j*g_frameWidth*6] = 0;
							g_enlargedisplaytmp1[i*3+1+j*g_frameWidth*6] = 0;
							g_enlargedisplaytmp1[i*3+2+j*g_frameWidth*6] = 255;
						}
						else
						{
							g_enlargedisplaytmp1[i*3+j*g_frameWidth*6] = g_enlargedisplaytmp0[i*3+j*g_frameWidth*6];
							g_enlargedisplaytmp1[i*3+1+j*g_frameWidth*6] = g_enlargedisplaytmp0[i*3+1+j*g_frameWidth*6];
							g_enlargedisplaytmp1[i*3+2+j*g_frameWidth*6] = g_enlargedisplaytmp0[i*3+2+j*g_frameWidth*6];
						}
					}

			}
			else
			{
				for(j=0;j<g_frameHeight*2;j++)
					for(i=0;i<g_frameWidth*2;i++)
					{
						ii = i-ex;
						jj = j-ey;
						dis = (int)sqrt((double)(ii*ii+jj*jj));
						if(dis==g_segeditmodulerad)
						{
							g_enlargedisplaytmp1[i*3+j*g_frameWidth*6] = 0;
							g_enlargedisplaytmp1[i*3+1+j*g_frameWidth*6] = 0;
							g_enlargedisplaytmp1[i*3+2+j*g_frameWidth*6] = 255;
						}
						else
						{
							g_enlargedisplaytmp1[i*3+j*g_frameWidth*6] = g_enlargedisplaytmp0[i*3+j*g_frameWidth*6];
							g_enlargedisplaytmp1[i*3+1+j*g_frameWidth*6] = g_enlargedisplaytmp0[i*3+1+j*g_frameWidth*6];
							g_enlargedisplaytmp1[i*3+2+j*g_frameWidth*6] = g_enlargedisplaytmp0[i*3+2+j*g_frameWidth*6];
						}
					}
			}
		}
		gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
		          IMGWIDTHMAX-g_frameWidth, IMGHEIGHTMAX-g_frameHeight, g_frameWidth*2, g_frameHeight*2,
		          GDK_RGB_DITHER_MAX, g_enlargedisplaytmp1, g_frameWidth * 6);
	}
	return ;
}




void DrawDepEditContour(int x, int y, int ex, int ey, int region)
{
	int i, j, ii, jj;
	int dis;
	uint8_t val;
	int Segoffset = g_frameWidth*3;
	int Manoffset = g_frameHeight*g_frameWidth*6;
	int Depoffset = g_frameHeight*g_frameWidth*6+g_frameWidth*3;
	
	if(g_depeditmode<=1)
		val = 255;
	else
		val = 0;

	if(g_enlargeflag==0)
	{
		switch(g_depeditmodule)
		{
			case SEGEDITMODULESQU:
				#if 1
				for(j=0;j<g_frameHeight;j++)
					for(i=0;i<g_frameWidth;i++)
					{
						if((abs(i-x)==g_depeditmodulerad&&abs(j-y)<=g_depeditmodulerad)||(abs(j-y)==g_depeditmodulerad&&abs(i-x)<=g_depeditmodulerad))
						{
							// REGIONORG
							g_tmpdisplay[i*3+j*g_frameWidth*6] = 255;
							g_tmpdisplay[i*3+1+j*g_frameWidth*6] = 0;
							g_tmpdisplay[i*3+2+j*g_frameWidth*6] = 0;
							// REGIONSEG
							g_tmpdisplay[i*3+j*g_frameWidth*6+Segoffset] = 0;
							g_tmpdisplay[i*3+1+j*g_frameWidth*6+Segoffset] = 0;
							g_tmpdisplay[i*3+2+j*g_frameWidth*6+Segoffset] = 255;
							// REGIONMAN
							g_tmpdisplay[i*3+j*g_frameWidth*6+Manoffset] = 0;
							g_tmpdisplay[i*3+1+j*g_frameWidth*6+Manoffset] = 255;
							g_tmpdisplay[i*3+2+j*g_frameWidth*6+Manoffset] = 0;
							// REGIONDEP
							g_tmpdisplay[i*3+j*g_frameWidth*6+Depoffset] = val;
							g_tmpdisplay[i*3+1+j*g_frameWidth*6+Depoffset] = val;
							g_tmpdisplay[i*3+2+j*g_frameWidth*6+Depoffset] = val;
						}
						else
						{
							g_tmpdisplay[i*3+j*g_frameWidth*6] = g_rgbinput[i*3+j*g_frameWidth*3];
							g_tmpdisplay[i*3+1+j*g_frameWidth*6] = g_rgbinput[i*3+1+j*g_frameWidth*3];
							g_tmpdisplay[i*3+2+j*g_frameWidth*6] = g_rgbinput[i*3+2+j*g_frameWidth*3];

							g_tmpdisplay[i*3+j*g_frameWidth*6+Segoffset] = g_autosegdisplay[i*3+j*g_frameWidth*3];
							g_tmpdisplay[i*3+1+j*g_frameWidth*6+Segoffset] = g_autosegdisplay[i*3+1+j*g_frameWidth*3];
							g_tmpdisplay[i*3+2+j*g_frameWidth*6+Segoffset] = g_autosegdisplay[i*3+2+j*g_frameWidth*3];

							g_tmpdisplay[i*3+j*g_frameWidth*6+Manoffset] = g_manusegdisplay[i*3+j*g_frameWidth*3];
							g_tmpdisplay[i*3+1+j*g_frameWidth*6+Manoffset] = g_manusegdisplay[i*3+1+j*g_frameWidth*3];
							g_tmpdisplay[i*3+2+j*g_frameWidth*6+Manoffset] = g_manusegdisplay[i*3+2+j*g_frameWidth*3];

							g_tmpdisplay[i*3+j*g_frameWidth*6+Depoffset] = g_depthdisplay[i+j*g_frameWidth];
							g_tmpdisplay[i*3+1+j*g_frameWidth*6+Depoffset] = g_depthdisplay[i+j*g_frameWidth];
							g_tmpdisplay[i*3+2+j*g_frameWidth*6+Depoffset] = g_depthdisplay[i+j*g_frameWidth];
						}
					}
				gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
			          IMGWIDTHMAX-g_frameWidth, IMGHEIGHTMAX-g_frameHeight, g_frameWidth*2, g_frameHeight*2,
			          GDK_RGB_DITHER_MAX, g_tmpdisplay, g_frameWidth * 6);
				#else
				if(region==REGIONORG)
				{
					for(j=0;j<g_frameHeight;j++)
						for(i=0;i<g_frameWidth;i++)
						{
							if((abs(i-x)==g_depeditmodulerad&&abs(j-y)<=g_depeditmodulerad)||(abs(j-y)==g_depeditmodulerad&&abs(i-x)<=g_depeditmodulerad))
							{
								g_tmpdisplay[i*3+j*g_frameWidth*3] = 255;
								g_tmpdisplay[i*3+1+j*g_frameWidth*3] = 0;
								g_tmpdisplay[i*3+2+j*g_frameWidth*3] = 0;
							}
							else
							{
								g_tmpdisplay[i*3+j*g_frameWidth*3] = g_rgbinput[i*3+j*g_frameWidth*3];
								g_tmpdisplay[i*3+1+j*g_frameWidth*3] = g_rgbinput[i*3+1+j*g_frameWidth*3];
								g_tmpdisplay[i*3+2+j*g_frameWidth*3] = g_rgbinput[i*3+2+j*g_frameWidth*3];
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
							if((abs(i-x)==g_depeditmodulerad&&abs(j-y)<=g_depeditmodulerad)||(abs(j-y)==g_depeditmodulerad&&abs(i-x)<=g_depeditmodulerad))
							{
								g_tmpdisplay[i*3+j*g_frameWidth*3] = 0;
								g_tmpdisplay[i*3+1+j*g_frameWidth*3] = 0;
								g_tmpdisplay[i*3+2+j*g_frameWidth*3] = 255;
							}
							else
							{
								g_tmpdisplay[i*3+j*g_frameWidth*3] = g_autosegdisplay[i*3+j*g_frameWidth*3];
								g_tmpdisplay[i*3+1+j*g_frameWidth*3] = g_autosegdisplay[i*3+1+j*g_frameWidth*3];
								g_tmpdisplay[i*3+2+j*g_frameWidth*3] = g_autosegdisplay[i*3+2+j*g_frameWidth*3];
							}
						}					

					gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
							  IMGWIDTHMAX, IMGHEIGHTMAX-g_frameHeight, g_frameWidth, g_frameHeight,
							  GDK_RGB_DITHER_MAX, g_tmpdisplay, g_frameWidth * 3);
				}
				if(region==REGIONMAN)
				{
					for(j=0;j<g_frameHeight;j++)
						for(i=0;i<g_frameWidth;i++)
						{
							if((abs(i-x)==g_depeditmodulerad&&abs(j-y)<=g_depeditmodulerad)||(abs(j-y)==g_depeditmodulerad&&abs(i-x)<=g_depeditmodulerad))
							{
								g_tmpdisplay[i*3+j*g_frameWidth*3] = 0;
								g_tmpdisplay[i*3+1+j*g_frameWidth*3] = 255;
								g_tmpdisplay[i*3+2+j*g_frameWidth*3] = 0;
							}
							else
							{
								g_tmpdisplay[i*3+j*g_frameWidth*3] = g_manusegdisplay[i*3+j*g_frameWidth*3];
								g_tmpdisplay[i*3+1+j*g_frameWidth*3] = g_manusegdisplay[i*3+1+j*g_frameWidth*3];
								g_tmpdisplay[i*3+2+j*g_frameWidth*3] = g_manusegdisplay[i*3+2+j*g_frameWidth*3];
							}
						}
						
					gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
			          IMGWIDTHMAX-g_frameWidth, IMGHEIGHTMAX, g_frameWidth, g_frameHeight,
			          GDK_RGB_DITHER_MAX, g_tmpdisplay, g_frameWidth * 3);
				}
				if(region==REGIONDEP)
				{
					for(j=0;j<g_frameHeight;j++)
						for(i=0;i<g_frameWidth;i++)
						{
							if((abs(i-x)==g_depeditmodulerad&&abs(j-y)<=g_depeditmodulerad)||(abs(j-y)==g_depeditmodulerad&&abs(i-x)<=g_depeditmodulerad))
							{
								if(g_depeditmode<=1)
									g_tmpdisplay[i+j*g_frameWidth] = 255;
								else
									g_tmpdisplay[i+j*g_frameWidth] = 0;
							}
							else
							{
								g_tmpdisplay[i+j*g_frameWidth] = g_depthdisplay[i+j*g_frameWidth];
							}
						}
						
					gdk_draw_gray_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
			          IMGWIDTHMAX, IMGHEIGHTMAX, g_frameWidth, g_frameHeight,
			          GDK_RGB_DITHER_MAX, g_tmpdisplay, g_frameWidth);
				}
				#endif
				break;
			
			case SEGEDITMODULECIR:
			default:
				#if 1
				for(j=0;j<g_frameHeight;j++)
					for(i=0;i<g_frameWidth;i++)
					{
						ii = i-x;
						jj = j-y;
						dis = (int)sqrt((double)(ii*ii+jj*jj));
						if(dis==g_depeditmodulerad)
						{
							// REGIONORG
							g_tmpdisplay[i*3+j*g_frameWidth*6] = 255;
							g_tmpdisplay[i*3+1+j*g_frameWidth*6] = 0;
							g_tmpdisplay[i*3+2+j*g_frameWidth*6] = 0;
							// REGIONSEG
							g_tmpdisplay[i*3+j*g_frameWidth*6+Segoffset] = 0;
							g_tmpdisplay[i*3+1+j*g_frameWidth*6+Segoffset] = 0;
							g_tmpdisplay[i*3+2+j*g_frameWidth*6+Segoffset] = 255;
							// REGIONMAN
							g_tmpdisplay[i*3+j*g_frameWidth*6+Manoffset] = 0;
							g_tmpdisplay[i*3+1+j*g_frameWidth*6+Manoffset] = 255;
							g_tmpdisplay[i*3+2+j*g_frameWidth*6+Manoffset] = 0;
							// REGIONDEP
							g_tmpdisplay[i*3+j*g_frameWidth*6+Depoffset] = val;
							g_tmpdisplay[i*3+1+j*g_frameWidth*6+Depoffset] = val;
							g_tmpdisplay[i*3+2+j*g_frameWidth*6+Depoffset] = val;
						}
						else
						{
							g_tmpdisplay[i*3+j*g_frameWidth*6] = g_rgbinput[i*3+j*g_frameWidth*3];
							g_tmpdisplay[i*3+1+j*g_frameWidth*6] = g_rgbinput[i*3+1+j*g_frameWidth*3];
							g_tmpdisplay[i*3+2+j*g_frameWidth*6] = g_rgbinput[i*3+2+j*g_frameWidth*3];

							g_tmpdisplay[i*3+j*g_frameWidth*6+Segoffset] = g_autosegdisplay[i*3+j*g_frameWidth*3];
							g_tmpdisplay[i*3+1+j*g_frameWidth*6+Segoffset] = g_autosegdisplay[i*3+1+j*g_frameWidth*3];
							g_tmpdisplay[i*3+2+j*g_frameWidth*6+Segoffset] = g_autosegdisplay[i*3+2+j*g_frameWidth*3];

							g_tmpdisplay[i*3+j*g_frameWidth*6+Manoffset] = g_manusegdisplay[i*3+j*g_frameWidth*3];
							g_tmpdisplay[i*3+1+j*g_frameWidth*6+Manoffset] = g_manusegdisplay[i*3+1+j*g_frameWidth*3];
							g_tmpdisplay[i*3+2+j*g_frameWidth*6+Manoffset] = g_manusegdisplay[i*3+2+j*g_frameWidth*3];

							g_tmpdisplay[i*3+j*g_frameWidth*6+Depoffset] = g_depthdisplay[i+j*g_frameWidth];
							g_tmpdisplay[i*3+1+j*g_frameWidth*6+Depoffset] = g_depthdisplay[i+j*g_frameWidth];
							g_tmpdisplay[i*3+2+j*g_frameWidth*6+Depoffset] = g_depthdisplay[i+j*g_frameWidth];
						}
					}
				gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
			          IMGWIDTHMAX-g_frameWidth, IMGHEIGHTMAX-g_frameHeight, g_frameWidth*2, g_frameHeight*2,
			          GDK_RGB_DITHER_MAX, g_tmpdisplay, g_frameWidth * 6);
				#else
				if(region==REGIONORG)
				{
					for(j=0;j<g_frameHeight;j++)
						for(i=0;i<g_frameWidth;i++)
						{
							ii = i-x;
							jj = j-y;
							dis = (int)sqrt((double)(ii*ii+jj*jj));
							if(dis==g_depeditmodulerad)
							{
								g_tmpdisplay[i*3+j*g_frameWidth*3] = 255;
								g_tmpdisplay[i*3+1+j*g_frameWidth*3] = 0;
								g_tmpdisplay[i*3+2+j*g_frameWidth*3] = 0;
							}
							else
							{
								g_tmpdisplay[i*3+j*g_frameWidth*3] = g_rgbinput[i*3+j*g_frameWidth*3];
								g_tmpdisplay[i*3+1+j*g_frameWidth*3] = g_rgbinput[i*3+1+j*g_frameWidth*3];
								g_tmpdisplay[i*3+2+j*g_frameWidth*3] = g_rgbinput[i*3+2+j*g_frameWidth*3];
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
							if(dis==g_depeditmodulerad)
							{
								g_tmpdisplay[i*3+j*g_frameWidth*3] = 0;
								g_tmpdisplay[i*3+1+j*g_frameWidth*3] = 0;
								g_tmpdisplay[i*3+2+j*g_frameWidth*3] = 255;
							}
							else
							{
								g_tmpdisplay[i*3+j*g_frameWidth*3] = g_autosegdisplay[i*3+j*g_frameWidth*3];
								g_tmpdisplay[i*3+1+j*g_frameWidth*3] = g_autosegdisplay[i*3+1+j*g_frameWidth*3];
								g_tmpdisplay[i*3+2+j*g_frameWidth*3] = g_autosegdisplay[i*3+2+j*g_frameWidth*3];
							}
						}

					gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
				          IMGWIDTHMAX, IMGHEIGHTMAX-g_frameHeight, g_frameWidth, g_frameHeight,
				          GDK_RGB_DITHER_MAX, g_tmpdisplay, g_frameWidth * 3);
				}	
				if(region==REGIONMAN)
				{
					for(j=0;j<g_frameHeight;j++)
						for(i=0;i<g_frameWidth;i++)
						{
							ii = i-x;
							jj = j-y;
							dis = (int)sqrt((double)(ii*ii+jj*jj));
							if(dis==g_depeditmodulerad)
							{
								g_tmpdisplay[i*3+j*g_frameWidth*3] = 0;
								g_tmpdisplay[i*3+1+j*g_frameWidth*3] = 255;
								g_tmpdisplay[i*3+2+j*g_frameWidth*3] = 0;
							}
							else
							{
								g_tmpdisplay[i*3+j*g_frameWidth*3] = g_manusegdisplay[i*3+j*g_frameWidth*3];
								g_tmpdisplay[i*3+1+j*g_frameWidth*3] = g_manusegdisplay[i*3+1+j*g_frameWidth*3];
								g_tmpdisplay[i*3+2+j*g_frameWidth*3] = g_manusegdisplay[i*3+2+j*g_frameWidth*3];
							}
						}
						
					gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
			          IMGWIDTHMAX-g_frameWidth, IMGHEIGHTMAX, g_frameWidth, g_frameHeight,
			          GDK_RGB_DITHER_MAX, g_tmpdisplay, g_frameWidth * 3);
				}	
				if(region==REGIONDEP)
				{
					for(j=0;j<g_frameHeight;j++)
						for(i=0;i<g_frameWidth;i++)
						{
							ii = i-x;
							jj = j-y;
							dis = (int)sqrt((double)(ii*ii+jj*jj));
							if(dis==g_depeditmodulerad)
							{
								if(g_depeditmode<=1)
									g_tmpdisplay[i+j*g_frameWidth] = 255;
								else
									g_tmpdisplay[i+j*g_frameWidth] = 0;
							}
							else
							{
								g_tmpdisplay[i+j*g_frameWidth] = g_depthdisplay[i+j*g_frameWidth];
							}
						}
						
					gdk_draw_gray_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
			          IMGWIDTHMAX, IMGHEIGHTMAX, g_frameWidth, g_frameHeight,
			          GDK_RGB_DITHER_MAX, g_tmpdisplay, g_frameWidth);
				}
				#endif
				break;
		}
	}
	else
	{
		if(g_enlargeswitchflag==1)
		{
			if(g_frameResizeFlag==0)
			{
				//TODO add depth view
			}
			else
			{
				if(g_depeditmodule==SEGEDITMODULESQU)
				{
					for(j=0;j<g_frameHeight*2;j++)
						for(i=0;i<g_frameWidth*2;i++)
						{
							if((abs(i-ex)==g_depeditmodulerad&&abs(j-ey)<=g_depeditmodulerad)||(abs(j-ey)==g_depeditmodulerad&&abs(i-ex)<=g_depeditmodulerad))
							{
								if(g_depeditmode<=1)
									g_enlargedisplaytmp1[i+j*g_frameWidth*2] = 255;
								else
									g_enlargedisplaytmp1[i+j*g_frameWidth*2] = 0;
							}
							else
							{
								g_enlargedisplaytmp1[i+j*g_frameWidth*2] = g_enlargedisplaytmp0[i+j*g_frameWidth*2];
							}
						}

				}
				else
				{
					for(j=0;j<g_frameHeight*2;j++)
						for(i=0;i<g_frameWidth*2;i++)
						{
							ii = i-ex;
							jj = j-ey;
							dis = (int)sqrt((double)(ii*ii+jj*jj));
							if(dis==g_depeditmodulerad)
							{
								if(g_depeditmode<=1)
									g_enlargedisplaytmp1[i+j*g_frameWidth*2] = 255;
								else
									g_enlargedisplaytmp1[i+j*g_frameWidth*2] = 0;
							}
							else
							{
								g_enlargedisplaytmp1[i+j*g_frameWidth*2] = g_enlargedisplaytmp0[i+j*g_frameWidth*2];
							}
						}
				}
			}
			gdk_draw_gray_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
			          IMGWIDTHMAX-g_frameWidth, IMGHEIGHTMAX-g_frameHeight, g_frameWidth*2, g_frameHeight*2,
			          GDK_RGB_DITHER_MAX, g_enlargedisplaytmp1, g_frameWidth * 2);
		}
	}
	return ;
}


uint8_t addalphaR(uint8_t r, int idx)
{
	uint8_t tmp;
	tmp = (uint8_t)((r+colorset[idx][0])>>1);
	return tmp;
}

uint8_t addalphaG(uint8_t r, int idx)
{
	uint8_t tmp;
	tmp = (uint8_t)((r+colorset[idx][1])>>1);
	return tmp;
}

uint8_t addalphaB(uint8_t r, int idx)
{
	uint8_t tmp;
	tmp = (uint8_t)((r+colorset[idx][2])>>1);
	return tmp;
}

/* 在手工分割区画出分割结果*/
void Drawlabel(int * labels, int label)
{
	int i,j;
	int offset,idx=0;

	if(g_frames[g_frameIndex].SegFlag==1)
	{
	
		// 分割区显示alpha图，其余显示原图
		offset = label - g_frameWidth*g_frameHeight;

		for(i=0;i<=g_frames[g_frameIndex].LayerVisibleNum;i++)
		{
			if((offset&(0x1<<i))!=0)
			{
				idx = i;
				break;
			}
		}
		//printf("idx: %d\n", idx);

		for(j=0;j<g_frameHeight;j++)
			for(i=0;i<g_frameWidth;i++)
			{
				if(labels[i+j*g_frameWidth]==label)
				{
					g_manusegdisplay[i*3+j*g_frameWidth*3] = addalphaR(g_frames[g_frameIndex].rgbinput[i*3+j*g_frameWidth*3], idx);
					g_manusegdisplay[i*3+1+j*g_frameWidth*3] = addalphaG(g_frames[g_frameIndex].rgbinput[i*3+1+j*g_frameWidth*3], idx);
					g_manusegdisplay[i*3+2+j*g_frameWidth*3] = addalphaB(g_frames[g_frameIndex].rgbinput[i*3+2+j*g_frameWidth*3], idx);
				}
				else
				{
					g_manusegdisplay[i*3+j*g_frameWidth*3] = g_frames[g_frameIndex].rgbinput[i*3+j*g_frameWidth*3];
					g_manusegdisplay[i*3+1+j*g_frameWidth*3] = g_frames[g_frameIndex].rgbinput[i*3+1+j*g_frameWidth*3];
					g_manusegdisplay[i*3+2+j*g_frameWidth*3] = g_frames[g_frameIndex].rgbinput[i*3+2+j*g_frameWidth*3];
				}
			}
		gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
		              IMGWIDTHMAX-g_frameWidth, IMGHEIGHTMAX-g_frameHeight, g_frameWidth, g_frameHeight,
		              GDK_RGB_DITHER_MAX, g_frames[g_frameIndex].rgbinput, g_frameWidth * 3);
		gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
              IMGWIDTHMAX-g_frameWidth, IMGHEIGHTMAX, g_frameWidth, g_frameHeight,
              GDK_RGB_DITHER_MAX, g_manusegdisplay, g_frameWidth * 3);

	}
	else
	{
		//messagebox("Please segment the frame first.");
	}

	return ;

}

/* 显示多层的分割结果和深度情况*/
void ShowSegandDep(int mask)
{
	int i, j, k;
	int label;
	int offset,idx=0,m;
	int sz = g_frameWidth*g_frameHeight;

	if(g_frames[g_frameIndex].SegFlag==1)
	{
		for(i=0;i<sz;i++) g_segmentedflags[i]=0;
		for(k=1;k<=g_frames[g_frameIndex].LayerVisibleNum;k++)
		{
			if(mask&(1<<k))
			{
				label = sz + (1<<k);
				for(i=0;i<sz;i++)
				{
					if(label==g_frames[g_frameIndex].SegLabels[i])
						g_segmentedflags[i]=1;
				}
			}
		}

		for(j=0;j<g_frameHeight;j++)
		for(i=0;i<g_frameWidth;i++)
		{
			if(g_segmentedflags[i+j*g_frameWidth]==1)
			{
				/*手工分割区域*/
				offset = g_frames[g_frameIndex].SegLabels[i+j*g_frameWidth] - g_frameWidth*g_frameHeight;
				for(m=0;m<=g_frames[g_frameIndex].LayerVisibleNum;m++)
				{
					if((offset&(0x1<<m))!=0)
					{
						idx = m;
						break;
					}
				}
				
				g_manusegdisplay[i*3+j*g_frameWidth*3] = addalphaR(g_frames[g_frameIndex].rgbinput[i*3+j*g_frameWidth*3], idx);
				g_manusegdisplay[i*3+1+j*g_frameWidth*3] = addalphaG(g_frames[g_frameIndex].rgbinput[i*3+1+j*g_frameWidth*3], idx);
				g_manusegdisplay[i*3+2+j*g_frameWidth*3] = addalphaB(g_frames[g_frameIndex].rgbinput[i*3+2+j*g_frameWidth*3], idx);
				g_depthdisplay[i+j*g_frameWidth] = g_frames[g_frameIndex].SegDepths[i+j*g_frameWidth];
			}
			else
			{
				g_manusegdisplay[i*3+j*g_frameWidth*3] = g_frames[g_frameIndex].rgbinput[i*3+j*g_frameWidth*3];
				g_manusegdisplay[i*3+1+j*g_frameWidth*3] = g_frames[g_frameIndex].rgbinput[i*3+1+j*g_frameWidth*3];
				g_manusegdisplay[i*3+2+j*g_frameWidth*3] = g_frames[g_frameIndex].rgbinput[i*3+2+j*g_frameWidth*3];
				g_depthdisplay[i+j*g_frameWidth] = 0;
			}
		}
		gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
              IMGWIDTHMAX-g_frameWidth, IMGHEIGHTMAX, g_frameWidth, g_frameHeight,
              GDK_RGB_DITHER_MAX, g_manusegdisplay, g_frameWidth * 3);

		if(g_showalldepflag==1)
		{
			for(j=0;j<g_frameHeight;j++)
				for(i=0;i<g_frameWidth;i++)
				{
					g_depthdisplay[i+j*g_frameWidth] = g_frames[g_frameIndex].SegDepths[i+j*g_frameWidth];
				}
		}

		gdk_draw_gray_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
              IMGWIDTHMAX, IMGHEIGHTMAX, g_frameWidth, g_frameHeight,
              GDK_RGB_DITHER_MAX, g_depthdisplay, g_frameWidth);

	}
	else
	{
		//messagebox("Please segment the frame first.");
	}

	return ;
}

#define SEGLABEL0 0x10 // 初始标记点
#define SEGLABEL1 0x11 // 边缘点
#define SEGLABEL2 0x12 // 外边缘

// 对指定点进行区域增长，赋给相邻标记点同一标记值
// TODO
void SegRegionGrow(int x, int y, uint8_t * label)
{
#if 1
    // 不能保证联通区域
	int i;
	if(label[x+y*g_frameWidth]==1)
	{
		for(i=0;i<g_frameWidth*g_frameHeight;i++)
		{
			if(label[i]==1)
				label[i] = SEGLABEL0;
		}
	}
#else
    // 可能造成栈帧溢出
	label[x+y*g_frameWidth] = SEGLABEL0;

	// up
	if(y-1>=0&&label[x+(y-1)*g_frameWidth]==1)
	{
		SegRegionGrow(x, y-1, label);
	}
	// down
	if(y+1<g_frameHeight&&label[x+(y+1)*g_frameWidth]==1)
	{
		SegRegionGrow(x, y+1, label);
	}
	// left
	if(x-1>=0&&label[x-1+y*g_frameWidth]==1)
	{
		SegRegionGrow(x-1, y, label);
	}
	// right
	if(x+1<g_frameWidth&&label[x+1+y*g_frameWidth]==1)
	{
		SegRegionGrow(x+1, y, label);
	}
#endif
	return ;
}

void SegRegionEdge(uint8_t * tmplabels)
{
	int i, j;

	// 标记边缘
	for(j=0;j<g_frameHeight;j++)
	{
		for(i=0;i<g_frameWidth;i++)
		{
			if(tmplabels[i+j*g_frameWidth]==SEGLABEL0)
			{
				if(j==0||i==0||j==g_frameHeight-1||i==g_frameWidth-1)
					tmplabels[i+j*g_frameWidth] = SEGLABEL1;
				else if(tmplabels[i-1+j*g_frameWidth]<SEGLABEL0||
						tmplabels[i+1+j*g_frameWidth]<SEGLABEL0||
						tmplabels[i+(j-1)*g_frameWidth]<SEGLABEL0||
						tmplabels[i+(j+1)*g_frameWidth]<SEGLABEL0)
					tmplabels[i+j*g_frameWidth] = SEGLABEL1;
			}
		}
	}

	// 标记外边缘1
	for(j=0;j<g_frameHeight;j++)
		for(i=0;i<g_frameWidth;i++)
		{
			if(tmplabels[i+j*g_frameWidth]==SEGLABEL1)
			{
				// up
				if(j-1>=0&&tmplabels[i+(j-1)*g_frameWidth]<SEGLABEL0)
					tmplabels[i+(j-1)*g_frameWidth] = SEGLABEL2;
				// down
				if(j+1<g_frameHeight&&tmplabels[i+(j+1)*g_frameWidth]<SEGLABEL0)
					tmplabels[i+(j+1)*g_frameWidth] = SEGLABEL2;
				// left
				if(i-1>=0&&tmplabels[i-1+j*g_frameWidth]<SEGLABEL0)
					tmplabels[i-1+j*g_frameWidth] = SEGLABEL2;
				// right
				if(i+1<g_frameWidth&&tmplabels[i+1+j*g_frameWidth]<SEGLABEL0)
					tmplabels[i+1+j*g_frameWidth] = SEGLABEL2;
			}
		}

	return ;
}

void SegRegionEnlarge(int * labels, uint8_t * tmplabels, int label, int x, int y)
{
	int i,j,ii,jj;

	uint8_t r0, g0,b0, r1, g1, b1, r2, g2, b2;
	int value0, value1;
	int sizeorg = g_frameInWidth*g_frameInHeight;
	int size = g_frameWidth*g_frameHeight;
	int * orglabels = g_frames[g_frameIndex].SegLabels0;

	int l,r,u,d;

	l = H3D_MAX(0, x-g_segparamedgeenlargesize);
	r = H3D_MIN(g_frameWidth-1, x+g_segparamedgeenlargesize);
	u = H3D_MAX(0, y-g_segparamedgeenlargesize);
	d = H3D_MIN(g_frameHeight-1, y+g_segparamedgeenlargesize);

	for(j=u;j<=d;j++)
		for(i=l;i<=r;i++)
		{
			if(tmplabels[i+j*g_frameWidth]==SEGLABEL2)
			{
				r1 = g_frames[g_frameIndex].rgbinput[i*3+j*g_frameWidth*3];
				g1 = g_frames[g_frameIndex].rgbinput[i*3+1+j*g_frameWidth*3];
				b1 = g_frames[g_frameIndex].rgbinput[i*3+2+j*g_frameWidth*3];
				// 水平
				if((i-1>=0)&&(i+1<g_frameWidth))
				{
					if((tmplabels[i-1+j*g_frameWidth]==SEGLABEL1)&&(tmplabels[i+1+j*g_frameWidth]==SEGLABEL1))
					{
						labels[i+j*g_frameWidth]=label;
					}
					if((tmplabels[i-1+j*g_frameWidth]==SEGLABEL1)&&(labels[i+1+j*g_frameWidth]<size))
					{
						r0 = g_frames[g_frameIndex].rgbinput[(i-1)*3+j*g_frameWidth*3];
						g0 = g_frames[g_frameIndex].rgbinput[(i-1)*3+1+j*g_frameWidth*3];
						b0 = g_frames[g_frameIndex].rgbinput[(i-1)*3+2+j*g_frameWidth*3];
						r2 = g_frames[g_frameIndex].rgbinput[(i+1)*3+j*g_frameWidth*3];
						g2 = g_frames[g_frameIndex].rgbinput[(i+1)*3+1+j*g_frameWidth*3];
						b2 = g_frames[g_frameIndex].rgbinput[(i+1)*3+2+j*g_frameWidth*3];
						value0 = abs((int)r0-(int)r1)+abs((int)g0-(int)g1)+abs((int)b0-(int)b1);
						value1 = abs((int)r2-(int)r1)+abs((int)g2-(int)g1)+abs((int)b2-(int)b1);
						if(((value0<value1)||(value0<g_segparamedgeenlargeth))&&(labels[i+j*g_frameWidth]<size))
						{
							labels[i+j*g_frameWidth]=label;
							//continue ;
						}
					}
					if((labels[i-1+j*g_frameWidth]<size)&&(tmplabels[i+1+j*g_frameWidth]==SEGLABEL1))
					{
						r2 = g_frames[g_frameIndex].rgbinput[(i-1)*3+j*g_frameWidth*3];
						g2 = g_frames[g_frameIndex].rgbinput[(i-1)*3+1+j*g_frameWidth*3];
						b2 = g_frames[g_frameIndex].rgbinput[(i-1)*3+2+j*g_frameWidth*3];
						r0 = g_frames[g_frameIndex].rgbinput[(i+1)*3+j*g_frameWidth*3];
						g0 = g_frames[g_frameIndex].rgbinput[(i+1)*3+1+j*g_frameWidth*3];
						b0 = g_frames[g_frameIndex].rgbinput[(i+1)*3+2+j*g_frameWidth*3];
						value0 = abs((int)r0-(int)r1)+abs((int)g0-(int)g1)+abs((int)b0-(int)b1);
						value1 = abs((int)r2-(int)r1)+abs((int)g2-(int)g1)+abs((int)b2-(int)b1);
						if(((value0<value1)||(value0<g_segparamedgeenlargeth))&&(labels[i+j*g_frameWidth]<size))
						{
							labels[i+j*g_frameWidth]=label;
							//continue ;
						}
					}
				}
				// 垂直
				if((j-1>=0)&&(j+1<g_frameHeight))
				{
					if((tmplabels[i+(j-1)*g_frameWidth]==SEGLABEL1)&&(tmplabels[i+(j+1)*g_frameWidth]==SEGLABEL1))
					{
						labels[i+j*g_frameWidth]=label;
						if(g_frameResizeFlag==1)
						{
							for(jj=j*g_frameInHeight/g_frameHeight;jj<(j+1)*g_frameInHeight/g_frameHeight;jj++)
								for(ii=i*g_frameInWidth/g_frameWidth;ii<(i+1)*g_frameInWidth/g_frameWidth;ii++)
								{
									if(abs((int)g_yuvinput.y[i+j*g_frameWidth]-(int)g_yuvorginput.y[ii+jj*g_frameInWidth])<g_segparamedgeenlargeth)
										orglabels[ii+jj*g_frameInWidth] = label;
								}
						}
					}
					if((tmplabels[i+(j-1)*g_frameWidth]==SEGLABEL1)&&(labels[i+(j+1)*g_frameWidth]<size))
					{
						r0 = g_frames[g_frameIndex].rgbinput[i*3+(j-1)*g_frameWidth*3];
						g0 = g_frames[g_frameIndex].rgbinput[i*3+1+(j-1)*g_frameWidth*3];
						b0 = g_frames[g_frameIndex].rgbinput[i*3+2+(j-1)*g_frameWidth*3];
						r2 = g_frames[g_frameIndex].rgbinput[i*3+(j+1)*g_frameWidth*3];
						g2 = g_frames[g_frameIndex].rgbinput[i*3+1+(j+1)*g_frameWidth*3];
						b2 = g_frames[g_frameIndex].rgbinput[i*3+2+(j+1)*g_frameWidth*3];
						value0 = abs((int)r0-(int)r1)+abs((int)g0-(int)g1)+abs((int)b0-(int)b1);
						value1 = abs((int)r2-(int)r1)+abs((int)g2-(int)g1)+abs((int)b2-(int)b1);
						if(((value0<value1)||(value0<g_segparamedgeenlargeth))&&(labels[i+j*g_frameWidth]<size))
						{
							labels[i+j*g_frameWidth]=label;
							//continue ;
						}
					}
					if((labels[i+(j-1)*g_frameWidth]<size)&&(tmplabels[i+(j+1)*g_frameWidth]==SEGLABEL1))
					{
						r2 = g_frames[g_frameIndex].rgbinput[i*3+(j-1)*g_frameWidth*3];
						g2 = g_frames[g_frameIndex].rgbinput[i*3+1+(j-1)*g_frameWidth*3];
						b2 = g_frames[g_frameIndex].rgbinput[i*3+2+(j-1)*g_frameWidth*3];
						r0 = g_frames[g_frameIndex].rgbinput[i*3+(j+1)*g_frameWidth*3];
						g0 = g_frames[g_frameIndex].rgbinput[i*3+1+(j+1)*g_frameWidth*3];
						b0 = g_frames[g_frameIndex].rgbinput[i*3+2+(j+1)*g_frameWidth*3];
						value0 = abs((int)r0-(int)r1)+abs((int)g0-(int)g1)+abs((int)b0-(int)b1);
						value1 = abs((int)r2-(int)r1)+abs((int)g2-(int)g1)+abs((int)b2-(int)b1);
						if(((value0<value1)||(value0<g_segparamedgeenlargeth))&&(labels[i+j*g_frameWidth]<size))
						{
							labels[i+j*g_frameWidth]=label;
							//continue ;
						}
					}
				}
				// 135°45°
				if((j-1>=0)&&(j+1<g_frameHeight)&&(i-1>=0)&&(i+1<g_frameWidth))
				{
					if((tmplabels[i-1+(j-1)*g_frameWidth]==SEGLABEL1)&&(tmplabels[i+1+(j+1)*g_frameWidth]==SEGLABEL1))
					{
						labels[i+j*g_frameWidth]=label;
					}
					if((tmplabels[i-1+(j-1)*g_frameWidth]==SEGLABEL1)&&(labels[i+1+(j+1)*g_frameWidth]<size))
					{
						r0 = g_frames[g_frameIndex].rgbinput[(i-1)*3+(j-1)*g_frameWidth*3];
						g0 = g_frames[g_frameIndex].rgbinput[(i-1)*3+1+(j-1)*g_frameWidth*3];
						b0 = g_frames[g_frameIndex].rgbinput[(i-1)*3+2+(j-1)*g_frameWidth*3];
						r2 = g_frames[g_frameIndex].rgbinput[(i+1)*3+(j+1)*g_frameWidth*3];
						g2 = g_frames[g_frameIndex].rgbinput[(i+1)*3+1+(j+1)*g_frameWidth*3];
						b2 = g_frames[g_frameIndex].rgbinput[(i+1)*3+2+(j+1)*g_frameWidth*3];
						value0 = abs((int)r0-(int)r1)+abs((int)g0-(int)g1)+abs((int)b0-(int)b1);
						value1 = abs((int)r2-(int)r1)+abs((int)g2-(int)g1)+abs((int)b2-(int)b1);
						if(((value0<value1)||(value0<g_segparamedgeenlargeth))&&(labels[i+j*g_frameWidth]<size))
						{
							labels[i+j*g_frameWidth]=label;
							//continue ;
						}
					}
					if((labels[i-1+(j-1)*g_frameWidth]<size)&&(tmplabels[i+1+(j+1)*g_frameWidth]==SEGLABEL1))
					{
						r2 = g_frames[g_frameIndex].rgbinput[(i-1)*3+(j-1)*g_frameWidth*3];
						g2 = g_frames[g_frameIndex].rgbinput[(i-1)*3+1+(j-1)*g_frameWidth*3];
						b2 = g_frames[g_frameIndex].rgbinput[(i-1)*3+2+(j-1)*g_frameWidth*3];
						r0 = g_frames[g_frameIndex].rgbinput[(i+1)*3+(j+1)*g_frameWidth*3];
						g0 = g_frames[g_frameIndex].rgbinput[(i+1)*3+1+(j+1)*g_frameWidth*3];
						b0 = g_frames[g_frameIndex].rgbinput[(i+1)*3+2+(j+1)*g_frameWidth*3];
						value0 = abs((int)r0-(int)r1)+abs((int)g0-(int)g1)+abs((int)b0-(int)b1);
						value1 = abs((int)r2-(int)r1)+abs((int)g2-(int)g1)+abs((int)b2-(int)b1);
						if(((value0<value1)||(value0<g_segparamedgeenlargeth))&&(labels[i+j*g_frameWidth]<size))
						{
							labels[i+j*g_frameWidth]=label;
							//continue ;
						}
					}
					if((tmplabels[i+1+(j-1)*g_frameWidth]==SEGLABEL1)&&(tmplabels[i-1+(j+1)*g_frameWidth]==SEGLABEL1))
					{
						labels[i+j*g_frameWidth]=label;
					}
					if((tmplabels[i+1+(j-1)*g_frameWidth]==SEGLABEL1)&&(labels[i-1+(j+1)*g_frameWidth]<size))
					{
						r0 = g_frames[g_frameIndex].rgbinput[(i+1)*3+(j-1)*g_frameWidth*3];
						g0 = g_frames[g_frameIndex].rgbinput[(i+1)*3+1+(j-1)*g_frameWidth*3];
						b0 = g_frames[g_frameIndex].rgbinput[(i+1)*3+2+(j-1)*g_frameWidth*3];
						r2 = g_frames[g_frameIndex].rgbinput[(i-1)*3+(j+1)*g_frameWidth*3];
						g2 = g_frames[g_frameIndex].rgbinput[(i-1)*3+1+(j+1)*g_frameWidth*3];
						b2 = g_frames[g_frameIndex].rgbinput[(i-1)*3+2+(j+1)*g_frameWidth*3];
						value0 = abs((int)r0-(int)r1)+abs((int)g0-(int)g1)+abs((int)b0-(int)b1);
						value1 = abs((int)r2-(int)r1)+abs((int)g2-(int)g1)+abs((int)b2-(int)b1);
						if(((value0<value1)||(value0<g_segparamedgeenlargeth))&&(labels[i+j*g_frameWidth]<size))
						{
							labels[i+j*g_frameWidth]=label;
							//continue ;
						}
					}
					if((labels[i+1+(j-1)*g_frameWidth]<size)&&(tmplabels[i-1+(j+1)*g_frameWidth]==SEGLABEL1))
					{
						r2 = g_frames[g_frameIndex].rgbinput[(i+1)*3+(j-1)*g_frameWidth*3];
						g2 = g_frames[g_frameIndex].rgbinput[(i+1)*3+1+(j-1)*g_frameWidth*3];
						b2 = g_frames[g_frameIndex].rgbinput[(i+1)*3+2+(j-1)*g_frameWidth*3];
						r0 = g_frames[g_frameIndex].rgbinput[(i-1)*3+(j+1)*g_frameWidth*3];
						g0 = g_frames[g_frameIndex].rgbinput[(i-1)*3+1+(j+1)*g_frameWidth*3];
						b0 = g_frames[g_frameIndex].rgbinput[(i-1)*3+2+(j+1)*g_frameWidth*3];
						value0 = abs((int)r0-(int)r1)+abs((int)g0-(int)g1)+abs((int)b0-(int)b1);
						value1 = abs((int)r2-(int)r1)+abs((int)g2-(int)g1)+abs((int)b2-(int)b1);
						if(((value0<value1)||(value0<g_segparamedgeenlargeth))&&(labels[i+j*g_frameWidth]<size))
						{
							labels[i+j*g_frameWidth]=label;
							//continue ;
						}
					}
				}
				
			}
		}
	return ;
}

void SegEdgeEnlarge(int * labels,int label, int x, int y)
{
	int i;
	int size = g_frameWidth*g_frameHeight;
	int k;

	uint8_t *tmplabels = new uint8_t [size];

	for(k=0;k<g_segparamedgeenlargeiter;k++)
	{
		for(i=0;i<size;i++)
		{
			if(labels[i]==label)
				tmplabels[i] = 1;
			else
				tmplabels[i] = 0;
		}
		// 按标号区域增长
		SegRegionGrow(x, y, tmplabels);

		// 标记边缘与外边缘1和外边缘2
		SegRegionEdge(tmplabels);	

		// 水平+垂直扫描，判断是否进行扩散
		SegRegionEnlarge(labels, tmplabels, label, x, y);
	}
	if(tmplabels) delete [] tmplabels;
	
	return ;
}

// TODO 
void SegRegionGrow0(int x, int y, uint8_t * label)
{
#if 1
    // 不能保证联通区域
	int i;
	if(label[x+y*g_frameInWidth]==1)
	{
		for(i=0;i<g_frameInWidth*g_frameInHeight;i++)
		{
			if(label[i]==1)
				label[i] = SEGLABEL0;
		}
	}
#else
    // 可能造成栈帧溢出
	label[x+y*g_frameInWidth] = SEGLABEL0;

	// up
	if(y-1>=0&&label[x+(y-1)*g_frameInWidth]==1)
	{
		SegRegionGrow0(x, y-1, label);
	}
	// down
	if(y+1<g_frameInHeight&&label[x+(y+1)*g_frameInWidth]==1)
	{
		SegRegionGrow0(x, y+1, label);
	}
	// left
	if(x-1>=0&&label[x-1+y*g_frameInWidth]==1)
	{
		SegRegionGrow0(x-1, y, label);
	}
	// right
	if(x+1<g_frameInWidth&&label[x+1+y*g_frameInWidth]==1)
	{
		SegRegionGrow0(x+1, y, label);
	}
#endif
	return ;
}

void SegRegionEdge0(uint8_t * tmplabels)
{
	int i, j;

	// 标记边缘
	for(j=0;j<g_frameInHeight;j++)
	{
		for(i=0;i<g_frameInWidth;i++)
		{
			if(tmplabels[i+j*g_frameInWidth]==SEGLABEL0)
			{
				if(j==0||i==0||j==g_frameInHeight-1||i==g_frameInWidth-1)
					tmplabels[i+j*g_frameInWidth] = SEGLABEL1;
				else if(tmplabels[i-1+j*g_frameInWidth]<SEGLABEL0||
						tmplabels[i+1+j*g_frameInWidth]<SEGLABEL0||
						tmplabels[i+(j-1)*g_frameInWidth]<SEGLABEL0||
						tmplabels[i+(j+1)*g_frameInWidth]<SEGLABEL0)
					tmplabels[i+j*g_frameInWidth] = SEGLABEL1;
			}
		}
	}

	// 标记外边缘1
	for(j=0;j<g_frameInHeight;j++)
		for(i=0;i<g_frameInWidth;i++)
		{
			if(tmplabels[i+j*g_frameInWidth]==SEGLABEL1)
			{
				// up
				if(j-1>=0&&tmplabels[i+(j-1)*g_frameInWidth]<SEGLABEL0)
					tmplabels[i+(j-1)*g_frameInWidth] = SEGLABEL2;
				// down
				if(j+1<g_frameInHeight&&tmplabels[i+(j+1)*g_frameInWidth]<SEGLABEL0)
					tmplabels[i+(j+1)*g_frameInWidth] = SEGLABEL2;
				// left
				if(i-1>=0&&tmplabels[i-1+j*g_frameInWidth]<SEGLABEL0)
					tmplabels[i-1+j*g_frameInWidth] = SEGLABEL2;
				// right
				if(i+1<g_frameInWidth&&tmplabels[i+1+j*g_frameInWidth]<SEGLABEL0)
					tmplabels[i+1+j*g_frameInWidth] = SEGLABEL2;
			}
		}

	return ;
}


void SegRegionEnlarge0(int * labels, uint8_t * tmplabels, int label, int x, int y)
{
	int i,j,ii,jj;

	uint8_t r0, g0,b0, r1, g1, b1, r2, g2, b2;
	int value0, value1;
	int size = g_frameWidth*g_frameHeight;
	int * dislabels = g_frames[g_frameIndex].SegLabels;

	int l,r,u,d;

	l = H3D_MAX(0, x-g_segparamedgeenlargesize);
	r = H3D_MIN(g_frameInWidth-1, x+g_segparamedgeenlargesize);
	u = H3D_MAX(0, y-g_segparamedgeenlargesize);
	d = H3D_MIN(g_frameInHeight-1, y+g_segparamedgeenlargesize);

	for(j=u;j<=d;j++)
		for(i=l;i<=r;i++)
		{
			if(tmplabels[i+j*g_frameInWidth]==SEGLABEL2)
			{
				r1 = g_rgborginput[i*3+j*g_frameInWidth*3];
				g1 = g_rgborginput[i*3+1+j*g_frameInWidth*3];
				b1 = g_rgborginput[i*3+2+j*g_frameInWidth*3];
				// 水平
				if((i-1>=0)&&(i+1<g_frameInWidth))
				{
					if((tmplabels[i-1+j*g_frameInWidth]==SEGLABEL1)&&(tmplabels[i+1+j*g_frameInWidth]==SEGLABEL1))
					{
						labels[i+j*g_frameInWidth]=label;
						if(g_frameResizeFlag==1)
						{
							jj = j*g_frameHeight/g_frameInHeight;
							ii = i*g_frameWidth/g_frameInWidth;
							dislabels[ii+jj*g_frameWidth] = label;
						}
					}
					if((tmplabels[i-1+j*g_frameInWidth]==SEGLABEL1)&&(labels[i+1+j*g_frameInWidth]<size))
					{
						r0 = g_rgborginput[(i-1)*3+j*g_frameInWidth*3];
						g0 = g_rgborginput[(i-1)*3+1+j*g_frameInWidth*3];
						b0 = g_rgborginput[(i-1)*3+2+j*g_frameInWidth*3];
						r2 = g_rgborginput[(i+1)*3+j*g_frameInWidth*3];
						g2 = g_rgborginput[(i+1)*3+1+j*g_frameInWidth*3];
						b2 = g_rgborginput[(i+1)*3+2+j*g_frameInWidth*3];
						value0 = abs((int)r0-(int)r1)+abs((int)g0-(int)g1)+abs((int)b0-(int)b1);
						value1 = abs((int)r2-(int)r1)+abs((int)g2-(int)g1)+abs((int)b2-(int)b1);
						if(((value0<value1)||(value0<g_segparamedgeenlargeth))&&(labels[i+j*g_frameInWidth]<size))
						{
							labels[i+j*g_frameInWidth]=label;
							if(g_frameResizeFlag==1)
							{
								jj = j*g_frameHeight/g_frameInHeight;
								ii = i*g_frameWidth/g_frameInWidth;
								dislabels[ii+jj*g_frameWidth] = label;
							}
							//continue ;
						}
					}
					if((labels[i-1+j*g_frameInWidth]<size)&&(tmplabels[i+1+j*g_frameInWidth]==SEGLABEL1))
					{
						r2 = g_rgborginput[(i-1)*3+j*g_frameInWidth*3];
						g2 = g_rgborginput[(i-1)*3+1+j*g_frameInWidth*3];
						b2 = g_rgborginput[(i-1)*3+2+j*g_frameInWidth*3];
						r0 = g_rgborginput[(i+1)*3+j*g_frameInWidth*3];
						g0 = g_rgborginput[(i+1)*3+1+j*g_frameInWidth*3];
						b0 = g_rgborginput[(i+1)*3+2+j*g_frameInWidth*3];
						value0 = abs((int)r0-(int)r1)+abs((int)g0-(int)g1)+abs((int)b0-(int)b1);
						value1 = abs((int)r2-(int)r1)+abs((int)g2-(int)g1)+abs((int)b2-(int)b1);
						if(((value0<value1)||(value0<g_segparamedgeenlargeth))&&(labels[i+j*g_frameInWidth]<size))
						{
							labels[i+j*g_frameInWidth]=label;
							if(g_frameResizeFlag==1)
							{
								jj = j*g_frameHeight/g_frameInHeight;
								ii = i*g_frameWidth/g_frameInWidth;
								dislabels[ii+jj*g_frameWidth] = label;
							}
							//continue ;
						}
					}
				}
				// 垂直
				if((j-1>=0)&&(j+1<g_frameInHeight))
				{
					if((tmplabels[i+(j-1)*g_frameInWidth]==SEGLABEL1)&&(tmplabels[i+(j+1)*g_frameInWidth]==SEGLABEL1))
					{
						labels[i+j*g_frameInWidth]=label;
						if(g_frameResizeFlag==1)
						{
							jj = j*g_frameHeight/g_frameInHeight;
							ii = i*g_frameWidth/g_frameInWidth;
							dislabels[ii+jj*g_frameWidth] = label;
						}
					}
					if((tmplabels[i+(j-1)*g_frameInWidth]==SEGLABEL1)&&(labels[i+(j+1)*g_frameInWidth]<size))
					{
						r0 = g_rgborginput[i*3+(j-1)*g_frameInWidth*3];
						g0 = g_rgborginput[i*3+1+(j-1)*g_frameInWidth*3];
						b0 = g_rgborginput[i*3+2+(j-1)*g_frameInWidth*3];
						r2 = g_rgborginput[i*3+(j+1)*g_frameInWidth*3];
						g2 = g_rgborginput[i*3+1+(j+1)*g_frameInWidth*3];
						b2 = g_rgborginput[i*3+2+(j+1)*g_frameInWidth*3];
						value0 = abs((int)r0-(int)r1)+abs((int)g0-(int)g1)+abs((int)b0-(int)b1);
						value1 = abs((int)r2-(int)r1)+abs((int)g2-(int)g1)+abs((int)b2-(int)b1);
						if(((value0<value1)||(value0<g_segparamedgeenlargeth))&&(labels[i+j*g_frameInWidth]<size))
						{
							labels[i+j*g_frameInWidth]=label;
							if(g_frameResizeFlag==1)
							{
								jj = j*g_frameHeight/g_frameInHeight;
								ii = i*g_frameWidth/g_frameInWidth;
								dislabels[ii+jj*g_frameWidth] = label;
							}
							//continue ;
						}
					}
					if((labels[i+(j-1)*g_frameInWidth]<size)&&(tmplabels[i+(j+1)*g_frameInWidth]==SEGLABEL1))
					{
						r2 = g_rgborginput[i*3+(j-1)*g_frameInWidth*3];
						g2 = g_rgborginput[i*3+1+(j-1)*g_frameInWidth*3];
						b2 = g_rgborginput[i*3+2+(j-1)*g_frameInWidth*3];
						r0 = g_rgborginput[i*3+(j+1)*g_frameInWidth*3];
						g0 = g_rgborginput[i*3+1+(j+1)*g_frameInWidth*3];
						b0 = g_rgborginput[i*3+2+(j+1)*g_frameInWidth*3];
						value0 = abs((int)r0-(int)r1)+abs((int)g0-(int)g1)+abs((int)b0-(int)b1);
						value1 = abs((int)r2-(int)r1)+abs((int)g2-(int)g1)+abs((int)b2-(int)b1);
						if(((value0<value1)||(value0<g_segparamedgeenlargeth))&&(labels[i+j*g_frameInWidth]<size))
						{
							labels[i+j*g_frameInWidth]=label;
							if(g_frameResizeFlag==1)
							{
								jj = j*g_frameHeight/g_frameInHeight;
								ii = i*g_frameWidth/g_frameInWidth;
								dislabels[ii+jj*g_frameWidth] = label;
							}
							//continue ;
						}
					}
				}
				// 135°45°
				if((j-1>=0)&&(j+1<g_frameInHeight)&&(i-1>=0)&&(i+1<g_frameInWidth))
				{
					if((tmplabels[i-1+(j-1)*g_frameInWidth]==SEGLABEL1)&&(tmplabels[i+1+(j+1)*g_frameInWidth]==SEGLABEL1))
					{
						labels[i+j*g_frameInWidth]=label;
						if(g_frameResizeFlag==1)
						{
							jj = j*g_frameHeight/g_frameInHeight;
							ii = i*g_frameWidth/g_frameInWidth;
							dislabels[ii+jj*g_frameWidth] = label;
						}
					}
					if((tmplabels[i-1+(j-1)*g_frameInWidth]==SEGLABEL1)&&(labels[i+1+(j+1)*g_frameInWidth]<size))
					{
						r0 = g_rgborginput[(i-1)*3+(j-1)*g_frameInWidth*3];
						g0 = g_rgborginput[(i-1)*3+1+(j-1)*g_frameInWidth*3];
						b0 = g_rgborginput[(i-1)*3+2+(j-1)*g_frameInWidth*3];
						r2 = g_rgborginput[(i+1)*3+(j+1)*g_frameInWidth*3];
						g2 = g_rgborginput[(i+1)*3+1+(j+1)*g_frameInWidth*3];
						b2 = g_rgborginput[(i+1)*3+2+(j+1)*g_frameInWidth*3];
						value0 = abs((int)r0-(int)r1)+abs((int)g0-(int)g1)+abs((int)b0-(int)b1);
						value1 = abs((int)r2-(int)r1)+abs((int)g2-(int)g1)+abs((int)b2-(int)b1);
						if(((value0<value1)||(value0<g_segparamedgeenlargeth))&&(labels[i+j*g_frameInWidth]<size))
						{
							labels[i+j*g_frameInWidth]=label;
							if(g_frameResizeFlag==1)
							{
								jj = j*g_frameHeight/g_frameInHeight;
								ii = i*g_frameWidth/g_frameInWidth;
								dislabels[ii+jj*g_frameWidth] = label;
							}
							//continue ;
						}
					}
					if((labels[i-1+(j-1)*g_frameInWidth]<size)&&(tmplabels[i+1+(j+1)*g_frameInWidth]==SEGLABEL1))
					{
						r2 = g_rgborginput[(i-1)*3+(j-1)*g_frameInWidth*3];
						g2 = g_rgborginput[(i-1)*3+1+(j-1)*g_frameInWidth*3];
						b2 = g_rgborginput[(i-1)*3+2+(j-1)*g_frameInWidth*3];
						r0 = g_rgborginput[(i+1)*3+(j+1)*g_frameInWidth*3];
						g0 = g_rgborginput[(i+1)*3+1+(j+1)*g_frameInWidth*3];
						b0 = g_rgborginput[(i+1)*3+2+(j+1)*g_frameInWidth*3];
						value0 = abs((int)r0-(int)r1)+abs((int)g0-(int)g1)+abs((int)b0-(int)b1);
						value1 = abs((int)r2-(int)r1)+abs((int)g2-(int)g1)+abs((int)b2-(int)b1);
						if(((value0<value1)||(value0<g_segparamedgeenlargeth))&&(labels[i+j*g_frameInWidth]<size))
						{
							labels[i+j*g_frameInWidth]=label;
							if(g_frameResizeFlag==1)
							{
								jj = j*g_frameHeight/g_frameInHeight;
								ii = i*g_frameWidth/g_frameInWidth;
								dislabels[ii+jj*g_frameWidth] = label;
							}
							//continue ;
						}
					}
					if((tmplabels[i+1+(j-1)*g_frameInWidth]==SEGLABEL1)&&(tmplabels[i-1+(j+1)*g_frameInWidth]==SEGLABEL1))
					{
						labels[i+j*g_frameInWidth]=label;
						if(g_frameResizeFlag==1)
						{
							jj = j*g_frameHeight/g_frameInHeight;
							ii = i*g_frameWidth/g_frameInWidth;
							dislabels[ii+jj*g_frameWidth] = label;
						}
					}
					if((tmplabels[i+1+(j-1)*g_frameInWidth]==SEGLABEL1)&&(labels[i-1+(j+1)*g_frameInWidth]<size))
					{
						r0 = g_rgborginput[(i+1)*3+(j-1)*g_frameInWidth*3];
						g0 = g_rgborginput[(i+1)*3+1+(j-1)*g_frameInWidth*3];
						b0 = g_rgborginput[(i+1)*3+2+(j-1)*g_frameInWidth*3];
						r2 = g_rgborginput[(i-1)*3+(j+1)*g_frameInWidth*3];
						g2 = g_rgborginput[(i-1)*3+1+(j+1)*g_frameInWidth*3];
						b2 = g_rgborginput[(i-1)*3+2+(j+1)*g_frameInWidth*3];
						value0 = abs((int)r0-(int)r1)+abs((int)g0-(int)g1)+abs((int)b0-(int)b1);
						value1 = abs((int)r2-(int)r1)+abs((int)g2-(int)g1)+abs((int)b2-(int)b1);
						if(((value0<value1)||(value0<g_segparamedgeenlargeth))&&(labels[i+j*g_frameInWidth]<size))
						{
							labels[i+j*g_frameInWidth]=label;
							if(g_frameResizeFlag==1)
							{
								jj = j*g_frameHeight/g_frameInHeight;
								ii = i*g_frameWidth/g_frameInWidth;
								dislabels[ii+jj*g_frameWidth] = label;
							}
							//continue ;
						}
					}
					if((labels[i+1+(j-1)*g_frameInWidth]<size)&&(tmplabels[i-1+(j+1)*g_frameInWidth]==SEGLABEL1))
					{
						r2 = g_rgborginput[(i+1)*3+(j-1)*g_frameInWidth*3];
						g2 = g_rgborginput[(i+1)*3+1+(j-1)*g_frameInWidth*3];
						b2 = g_rgborginput[(i+1)*3+2+(j-1)*g_frameInWidth*3];
						r0 = g_rgborginput[(i-1)*3+(j+1)*g_frameInWidth*3];
						g0 = g_rgborginput[(i-1)*3+1+(j+1)*g_frameInWidth*3];
						b0 = g_rgborginput[(i-1)*3+2+(j+1)*g_frameInWidth*3];
						value0 = abs((int)r0-(int)r1)+abs((int)g0-(int)g1)+abs((int)b0-(int)b1);
						value1 = abs((int)r2-(int)r1)+abs((int)g2-(int)g1)+abs((int)b2-(int)b1);
						if(((value0<value1)||(value0<g_segparamedgeenlargeth))&&(labels[i+j*g_frameInWidth]<size))
						{
							labels[i+j*g_frameInWidth]=label;
							if(g_frameResizeFlag==1)
							{
								jj = j*g_frameHeight/g_frameInHeight;
								ii = i*g_frameWidth/g_frameInWidth;
								dislabels[ii+jj*g_frameWidth] = label;
							}
							//continue ;
						}
					}
				}
				
			}
		}
	return ;
}


void SegEdgeEnlarge0(int * labels,int label, int x, int y)
{
	int i;
	int size = g_frameInWidth*g_frameInHeight;
	int k;

	uint8_t *tmplabels = new uint8_t [size];

	for(k=0;k<g_segparamedgeenlargeiter;k++)
	{
		for(i=0;i<size;i++)
		{
			if(labels[i]==label)
				tmplabels[i] = 1;
			else
				tmplabels[i] = 0;
		}
		// 按标号区域增长
		SegRegionGrow0(x, y, tmplabels);

		// 标记边缘与外边缘1和外边缘2
		SegRegionEdge0(tmplabels);	

		// 水平+垂直扫描，判断是否进行扩散
		SegRegionEnlarge0(labels, tmplabels, label, x, y);
	}
	if(tmplabels) delete [] tmplabels;
	
	return ;
}



/* 在放大模式画出分割结果*/
void DrawEnlargelabel(int * labels, int label)
{
	int i,j, ii, jj;
	int offset,idx=0;
	int * orglabels = g_frames[g_frameIndex].SegLabels0;

	if(g_frames[g_frameIndex].SegFlag==1)
	{
	
		// 分割区显示alpha图，其余显示原图
		offset = label - g_frameWidth*g_frameHeight;

		for(i=0;i<=g_frames[g_frameIndex].LayerVisibleNum;i++)
		{
			if((offset&(0x1<<i))!=0)
			{
				idx = i;
				break;
			}
		}
		//printf("idx: %d\n", idx);
		if(g_frameResizeFlag==0)
		{
			for(j=0;j<g_frameHeight*2;j++)
				for(i=0;i<g_frameWidth*2;i++)
				{
					ii = i>>1;
					jj = j>>1;
					if(labels[ii+jj*g_frameWidth]==label)
					{
						g_enlargedisplaytmp0[i*3+j*g_frameWidth*6] = addalphaR(g_enlargedisplay[i*3+j*g_frameWidth*6], idx);
						g_enlargedisplaytmp0[i*3+1+j*g_frameWidth*6] = addalphaG(g_enlargedisplay[i*3+1+j*g_frameWidth*6], idx);
						g_enlargedisplaytmp0[i*3+2+j*g_frameWidth*6] = addalphaB(g_enlargedisplay[i*3+2+j*g_frameWidth*6], idx);
					}
					else
					{
						g_enlargedisplaytmp0[i*3+j*g_frameWidth*6] = g_enlargedisplay[i*3+j*g_frameWidth*6];
						g_enlargedisplaytmp0[i*3+1+j*g_frameWidth*6] = g_enlargedisplay[i*3+1+j*g_frameWidth*6];
						g_enlargedisplaytmp0[i*3+2+j*g_frameWidth*6] = g_enlargedisplay[i*3+2+j*g_frameWidth*6];
					}
				}
		}
		else
		{
			for(j=0;j<g_frameHeight*2;j++)
				for(i=0;i<g_frameWidth*2;i++)
				{
					ii = g_enlargeorgx + i;
					jj = g_enlargeorgy + j;
					if(orglabels[ii+jj*g_frameInWidth]==label)
					{
						g_enlargedisplaytmp0[i*3+j*g_frameWidth*6] = addalphaR(g_rgborginput[ii*3+jj*g_frameInWidth*3], idx);
						g_enlargedisplaytmp0[i*3+1+j*g_frameWidth*6] = addalphaG(g_rgborginput[ii*3+1+jj*g_frameInWidth*3], idx);
						g_enlargedisplaytmp0[i*3+2+j*g_frameWidth*6] = addalphaB(g_rgborginput[ii*3+2+jj*g_frameInWidth*3], idx);
					}
					else
					{
						g_enlargedisplaytmp0[i*3+j*g_frameWidth*6] = g_rgborginput[ii*3+jj*g_frameInWidth*3];
						g_enlargedisplaytmp0[i*3+1+j*g_frameWidth*6] = g_rgborginput[ii*3+1+jj*g_frameInWidth*3];
						g_enlargedisplaytmp0[i*3+2+j*g_frameWidth*6] = g_rgborginput[ii*3+2+jj*g_frameInWidth*3];
					}
				}
		}
		gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
		              IMGWIDTHMAX-g_frameWidth, IMGHEIGHTMAX-g_frameHeight, g_frameWidth*2, g_frameHeight*2,
		              GDK_RGB_DITHER_MAX, g_enlargedisplaytmp0, g_frameWidth * 6);

	}
	else
	{
		//messagebox("Please segment the frame first.");
	}

	return ;

}


void SegEnlargedisplay(int mask)
{

	int i, j, k, ii, jj;
	int label;
	int offset,idx=0,m;
	int szorg = g_frameInWidth*g_frameInHeight;
	int sz = g_frameWidth*g_frameHeight;

	if(g_frameResizeFlag==0)
	{
		for(i=0;i<sz;i++) 
			g_segmentedflags[i]=0;
		
		for(k=1;k<=g_frames[g_frameIndex].LayerVisibleNum;k++)
		{
			if(mask&(1<<k))
			{
				label = sz + (1<<k);
				for(i=0;i<sz;i++)
				{
					if(label==g_frames[g_frameIndex].SegLabels[i])
						g_segmentedflags[i]=1;
				}
			}
		}

		for(j=0;j<g_frameHeight*2;j++)
			for(i=0;i<g_frameWidth*2;i++)
			{
				ii = i>>1;
				jj = j>>1;
				if(g_segmentedflags[ii+jj*g_frameWidth]==1)
				{
					/*手工分割区域*/
					offset = g_frames[g_frameIndex].SegLabels[ii+jj*g_frameWidth] - g_frameWidth*g_frameHeight;
					for(m=0;m<=g_frames[g_frameIndex].LayerVisibleNum;m++)
					{
						if((offset&(0x1<<m))!=0)
						{
							idx = m;
							break;
						}
					}
					
					g_enlargedisplaytmp0[i*3+j*g_frameWidth*6] = addalphaR(g_enlargedisplay[i*3+j*g_frameWidth*6], idx);
					g_enlargedisplaytmp0[i*3+1+j*g_frameWidth*6] = addalphaG(g_enlargedisplay[i*3+1+j*g_frameWidth*6], idx);
					g_enlargedisplaytmp0[i*3+2+j*g_frameWidth*6] = addalphaB(g_enlargedisplay[i*3+2+j*g_frameWidth*6], idx);
				}
				else
				{
					g_enlargedisplaytmp0[i*3+j*g_frameWidth*6] = g_enlargedisplay[i*3+j*g_frameWidth*6];
					g_enlargedisplaytmp0[i*3+1+j*g_frameWidth*6] = g_enlargedisplay[i*3+1+j*g_frameWidth*6];
					g_enlargedisplaytmp0[i*3+2+j*g_frameWidth*6] = g_enlargedisplay[i*3+2+j*g_frameWidth*6];
				}
			}
	}
	else
	{
		for(i=0;i<szorg;i++) 
			g_segmentedflags[i]=0;
		
		for(k=1;k<=g_frames[g_frameIndex].LayerVisibleNum;k++)
		{
			if(mask&(1<<k))
			{
				label = sz + (1<<k);
				for(i=0;i<szorg;i++)
				{
					if(label==g_frames[g_frameIndex].SegLabels0[i])
						g_segmentedflags[i]=1;
				}
			}
		}


		if(g_enlargeswitchflag==0)
		{	
			for(j=0;j<g_frameHeight*2;j++)
				for(i=0;i<g_frameWidth*2;i++)
				{
					ii = i+g_enlargeorgx;
					jj = j+g_enlargeorgy;
					if(g_segmentedflags[ii+jj*g_frameInWidth]==1)
					{
						/*手工分割区域*/
						offset = g_frames[g_frameIndex].SegLabels0[ii+jj*g_frameInWidth] - sz;
						for(m=0;m<=g_frames[g_frameIndex].LayerVisibleNum;m++)
						{
							if((offset&(0x1<<m))!=0)
							{
								idx = m;
								break;
							}
						}
						
						g_enlargedisplaytmp0[i*3+j*g_frameWidth*6] = addalphaR(g_rgborginput[ii*3+jj*g_frameInWidth*3], idx);
						g_enlargedisplaytmp0[i*3+1+j*g_frameWidth*6] = addalphaG(g_rgborginput[ii*3+1+jj*g_frameInWidth*3], idx);
						g_enlargedisplaytmp0[i*3+2+j*g_frameWidth*6] = addalphaB(g_rgborginput[ii*3+2+jj*g_frameInWidth*3], idx);
					}
					else
					{
						g_enlargedisplaytmp0[i*3+j*g_frameWidth*6] = g_rgborginput[ii*3+jj*g_frameInWidth*3];
						g_enlargedisplaytmp0[i*3+1+j*g_frameWidth*6] = g_rgborginput[ii*3+1+jj*g_frameInWidth*3];
						g_enlargedisplaytmp0[i*3+2+j*g_frameWidth*6] = g_rgborginput[ii*3+2+jj*g_frameInWidth*3];
					}
				}
		}
		else
		{
			for(j=0;j<g_frameHeight*2;j++)
				for(i=0;i<g_frameWidth*2;i++)
				{
					ii = i+g_enlargeorgx;
					jj = j+g_enlargeorgy;
					if(g_segmentedflags[ii+jj*g_frameInWidth]==1||g_showalldepflag==1)
					{
						g_enlargedisplaytmp0[i+j*g_frameWidth*2] = g_frames[g_frameIndex].SegDepths0[ii+jj*g_frameInWidth];
					}
					else
					{
						g_enlargedisplaytmp0[i+j*g_frameWidth*2] = 0;
					}
				}
		}
	}

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
	
	return ;
}

void backgroundgeneration(int first, int last)
{
	int i,j,k;
	char filename[512];
	char filenamed[512];
	int * tmpsumr = NULL;
	int * tmpsumg = NULL;
	int * tmpsumb = NULL;
	int * tmpsumd = NULL;
	uint8_t * tmpavgr = NULL;
	uint8_t * tmpavgg = NULL;
	uint8_t * tmpavgb = NULL;
	uint8_t * tmpavgd = NULL;
	uint16_t * tmpcnt = NULL;

	if(g_frameMaxNum>0)
	{
		tmpsumr = new int[g_frameInWidth*g_frameInHeight];
		tmpsumg = new int[g_frameInWidth*g_frameInHeight];
		tmpsumb = new int[g_frameInWidth*g_frameInHeight];
		tmpsumd = new int[g_frameInWidth*g_frameInHeight];
		tmpavgr = new uint8_t[g_frameInWidth*g_frameInHeight];
		tmpavgg = new uint8_t[g_frameInWidth*g_frameInHeight];
		tmpavgb = new uint8_t[g_frameInWidth*g_frameInHeight];
		tmpavgd = new uint8_t[g_frameInWidth*g_frameInHeight];
		tmpcnt = new uint16_t[g_frameInWidth*g_frameInHeight];
		for(j=0;j<g_frameInHeight;j++)
			for(i=0;i<g_frameInWidth;i++)
			{
				tmpsumr[i+j*g_frameInWidth] = 0;
				tmpsumg[i+j*g_frameInWidth] = 0;
				tmpsumb[i+j*g_frameInWidth] = 0;
				tmpsumd[i+j*g_frameInWidth] = 0;
				tmpavgr[i+j*g_frameInWidth] = 0; // 默认背景为黑色
				tmpavgg[i+j*g_frameInWidth] = 0;
				tmpavgb[i+j*g_frameInWidth] = 255;
				tmpavgd[i+j*g_frameInWidth] = 0;
				tmpcnt[i+j*g_frameInWidth] = 0;
			}

		// 当前帧信息保存
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
		
		// TODO 进度提示，低分辨率情况
		for(k=first;k<=last;k++)
		{
			/* load frame info and image*/
			if(g_keyframeflag)
				sprintf(filename,"./%s/info/%d_infosnap_%d.dat",cfgparams.projectname,g_sectioncur,g_keyframenumarr[k]+1);
			else
				sprintf(filename,"./%s/info/%d_infosnap_%d.dat",cfgparams.projectname,g_sectioncur,k+1);
			if((g_frames[k].finfo=fopen(filename,"rb"))!=NULL)
			{
				// load info file
				loadinfofile(g_frames[k].finfo, k, 1);
				fclose(g_frames[k].finfo);

				#ifdef I64
				_fseeki64(g_fileInpYUV, (int64_t)k*IMAGE_SIZE(g_frameInWidth,g_frameInHeight), SEEK_SET);
				#else
				fseek(g_fileInpYUV, (unsigned long)k*IMAGE_SIZE(g_frameInWidth,g_frameInHeight), SEEK_SET);
				#endif
				fread(g_yuvorginput.y,(g_frameInWidth*g_frameInHeight),1,g_fileInpYUV);
				fread(g_yuvorginput.u,(g_frameInWidth*g_frameInHeight)>>2,1,g_fileInpYUV);
				fread(g_yuvorginput.v,(g_frameInWidth*g_frameInHeight)>>2,1,g_fileInpYUV);
				yuv2rgb(g_yuvorginput.y, g_yuvorginput.v, g_yuvorginput.u, g_rgborginputref, g_frameInWidth, g_frameInHeight);
				
				/* background (baselayer) data accumulation */
				g_frames[k].BackgroundFrame=first+1; // 保存帧序号，从1开始计数
				if(g_frames[k].LayerBaseNum>0)
				{
					for(j=0;j<g_frameInHeight;j++)
						for(i=0;i<g_frameInWidth;i++)
						{
							
							if((g_frames[k].SegLabels0[i+j*g_frameInWidth]>g_frameWidth*g_frameHeight)&&((1<<g_frames[k].LayerBaseNum)&(g_frames[k].SegLabels0[i+j*g_frameInWidth]-g_frameWidth*g_frameHeight)))
							{
								tmpsumr[i+j*g_frameInWidth] += g_rgborginputref[i*3+j*g_frameInWidth*3];
								tmpsumg[i+j*g_frameInWidth] += g_rgborginputref[i*3+1+j*g_frameInWidth*3];
								tmpsumb[i+j*g_frameInWidth] += g_rgborginputref[i*3+2+j*g_frameInWidth*3];
								tmpsumd[i+j*g_frameInWidth] += g_RefSegDepths0[i+j*g_frameInWidth];
								tmpcnt[i+j*g_frameInWidth] ++;
							}
						}
				}
				/* save background num */
				g_frames[k].BackgroundFrame = first+1;

				if(g_frames[k].SegFlag)// &&g_frames[g_frameIndex].ChangeFlag==1) // TODO 可优化，判断是否修改
				{
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
			printf("\rBG GENER INFO --- Background Generation frame index %d from %d to %d %.2lf%% ", k+1, first+1, last+1,((double)(k-first+1))/(last-first+1)*100);
		}

		// 当前帧信息恢复
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
		
		/* save background image */
		for(j=0;j<g_frameInHeight;j++)
			for(i=0;i<g_frameInWidth;i++)
			{
				if(tmpcnt[i+j*g_frameInWidth]>0)
				{
					tmpavgr[i+j*g_frameInWidth] = tmpsumr[i+j*g_frameInWidth]/tmpcnt[i+j*g_frameInWidth];
					tmpavgg[i+j*g_frameInWidth] = tmpsumg[i+j*g_frameInWidth]/tmpcnt[i+j*g_frameInWidth];
					tmpavgb[i+j*g_frameInWidth] = tmpsumb[i+j*g_frameInWidth]/tmpcnt[i+j*g_frameInWidth];
					tmpavgd[i+j*g_frameInWidth] = tmpsumd[i+j*g_frameInWidth]/tmpcnt[i+j*g_frameInWidth];
				}
			}

		//inpaintrgbimg(tmpavgr, tmpavgg, tmpavgb, g_frameInWidth, g_frameInHeight);
		
		IplImage* pImg, *pImgd;
		CvSize sz;
		sprintf(filename,"./%s/background/%d_backgroundsnap_%d.png",cfgparams.projectname,g_sectioncur,first+1);
		sprintf(filenamed,"./%s/background/%d_backgrounddepthsnap_%d.png",cfgparams.projectname,g_sectioncur,first+1);
		sz.width = g_frameInWidth;
		sz.height = g_frameInHeight;
		pImg = cvCreateImage(sz, IPL_DEPTH_8U, 3);
		pImgd = cvCreateImage(sz, IPL_DEPTH_8U, 1);
		for(j=0;j<g_frameInHeight;j++)
			for(i=0;i<g_frameInWidth;i++)
			{
				pImg->imageData[i*3+2+j*pImg->widthStep] = tmpavgr[i+j*g_frameInWidth];
				pImg->imageData[i*3+1+j*pImg->widthStep] = tmpavgg[i+j*g_frameInWidth];
				pImg->imageData[i*3+0+j*pImg->widthStep] = tmpavgb[i+j*g_frameInWidth];
				pImgd->imageData[i+j*pImgd->widthStep] = tmpavgd[i+j*g_frameInWidth];
			}

		cvSaveImage(filename, pImg);
		cvSaveImage(filenamed, pImgd);
		cvReleaseImage( &pImg );
		cvReleaseImage( &pImgd );

		/* return to current frame */
		//gotoframe(NULL, NULL);

		if(tmpsumr) delete [] tmpsumr;
		if(tmpsumg) delete [] tmpsumg;
		if(tmpsumb) delete [] tmpsumb;
		if(tmpsumd) delete [] tmpsumd;
		if(tmpavgr) delete [] tmpavgr;
		if(tmpavgg) delete [] tmpavgg;
		if(tmpavgb) delete [] tmpavgb;
		if(tmpavgd) delete [] tmpavgd;
		if(tmpcnt) delete [] tmpcnt;
	}

	return ;
}

