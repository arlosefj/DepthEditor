
/* OPENCV */
#include "cv.h"   
#include "highgui.h"   

#include "CSuperpixelSeg.h"
#include "project.h"
#include <io.h>

int g_inputfileformat = INPVID;

FILE * g_fileInpYUV = NULL;
FILE * g_fileSaveDep = NULL;
FILE * g_fileSaveSeg = NULL;
FILE * g_fileLoadSeg = NULL;
FILE * g_fileLoadSegDep = NULL;

FILE * g_fileSaveRender = NULL;
FILE * g_fileSaveSeqcut = NULL;

FILE * g_projectSaveInfo = NULL;
FILE * g_projectSaveRender = NULL;

// yuv原始图像输入
SYuvInput g_yuvorginput;
// yuv图像输入
SYuvInput g_yuvinput;
// 原始rgb缓冲区
uint8_t * g_rgbinput = NULL;
// Render参考缓冲区
uint8_t * g_rgbinputref = NULL;
// rgb原始缓冲区
uint8_t * g_rgborginput = NULL;
// Render参考缓冲区
uint8_t * g_rgborginputref = NULL;
// Render参考缓冲区
SYuvInput g_yuvinputref;
// Render参考缓冲区
SYuvInput g_yuvorginputref;

// Render修复时用缓冲区
uint8_t * g_rendertmp0 = NULL;
uint8_t * g_rendertmp1 = NULL;
uint8_t * g_renderorgtmp0 = NULL;
uint8_t * g_renderorgtmp1 = NULL;

// 原始图像缓冲区
uint8_t * g_autosegdisplayorg = NULL;
// 自动分割显示缓冲区
uint8_t * g_autosegdisplay = NULL;
// 手工分割显示缓冲区
uint8_t * g_manusegdisplay = NULL;

// 显示缓冲区(放大)
uint8_t * g_enlargedisplay = NULL;
// 显示缓冲区(放大)
uint8_t * g_enlargedisplaytmp0 = NULL;
// 显示缓冲区(放大)
uint8_t * g_enlargedisplaytmp1 = NULL;
// 深度显示缓冲区
uint8_t * g_depthdisplay = NULL;
// 暂时显示缓冲区
uint8_t * g_tmpdisplay = NULL;
// 暂时显示缓冲区①
uint8_t * g_tmpdisplay1 = NULL;
// 暂时显示缓冲区②
uint8_t * g_tmpdisplay2 = NULL;
// 暂时显示缓冲区③
uint8_t * g_tmpdisplay3 = NULL;

uint8_t * g_renderdisplay1 = NULL;
uint8_t * g_renderdisplay2 = NULL;
uint8_t * g_renderdisplay3 = NULL;
uint8_t * g_renderdisplay4 = NULL;
uint8_t * g_renderdisplay5 = NULL;
uint8_t * g_renderdisplay6 = NULL;
uint8_t * g_renderdisplay7 = NULL;
uint8_t * g_renderdisplay8 = NULL;

// 帧信息
int* g_OrgLabels0 = NULL;
int* g_SegLabels0 = NULL;
uint8_t* g_SegDepths0 = NULL;
int* g_OrgLabels = NULL;
int* g_SegLabels = NULL;
uint8_t* g_SegDepths = NULL;
int* g_RefOrgLabels0 = NULL;
int* g_RefSegLabels0 = NULL;
uint8_t* g_RefSegDepths0 = NULL;
int* g_RefOrgLabels = NULL;
int* g_RefSegLabels = NULL;
uint8_t* g_RefSegDepths = NULL;
int* g_Ref1OrgLabels0 = NULL;
int* g_Ref1SegLabels0 = NULL;
uint8_t* g_Ref1SegDepths0 = NULL;
int* g_Ref1OrgLabels = NULL;
int* g_Ref1SegLabels = NULL;
uint8_t* g_Ref1SegDepths = NULL;


// 基础深度
//uint8_t * g_BaseRgbInput = NULL;
uint8_t *g_BaseDepth = NULL;
uint8_t *g_RefBaseDepth = NULL;
uint8_t *g_Ref1BaseDepth = NULL;

int g_tmpframeIndex = 0;
// ctrl + z临时保存
int * g_tmpOrgLabels0 = NULL;
int * g_tmpSegLabels0 = NULL;
uint8_t * g_tmpSegDepths0 = NULL;
int * g_tmpOrgLabels = NULL;
int * g_tmpSegLabels = NULL;
uint8_t * g_tmpSegDepths = NULL;


// render标记
uint8_t * g_renderlabel1 = NULL;
uint8_t * g_renderlabel2 = NULL;

// 分割显示标记
uint8_t * g_segmentedflags = NULL;

int g_videoinfoflag = 0; // 视频信息标志
int g_keyframeflag = 1; // 是否采用关键帧模式
int g_keyframeth = 512; // 关键帧判断阈值
int g_timestart = 0; // 开始时间
int g_timeend = 1;   // 结束时间

int g_timetotal = 0; // 总时长
int g_sectioncur = 1; // 当前段号
int g_sectionduration = 30; // 当前段时长
int g_sectionnum = 1; // 总段数
int g_sectionflag = 1; // 是否分段标记
int g_sectionlastdura = 0; // 最后一段时长

int * g_keyframenumarr = NULL;

void InputInit()
{
	int i,j;
	char str[16];
	int sizeorg = g_frameInWidth*g_frameInHeight;
	int size = g_frameWidth*g_frameHeight;
	
	int maxhw, hw, stage;
	GdkColor color_green;
	color_green.red = 0x0; color_green.green = 0xffff; color_green.blue = 0x0;

	set_resolution(NULL, NULL);

	if(g_keyframeflag)
		sprintf(str,"%d:%d/%d", 1, 1, g_frameMaxNum);
	else
		sprintf(str,"%4d /%4d", 1, g_frameMaxNum);
	gtk_label_set_text((GtkLabel *)g_wdgFrameNum,str);
	gtk_widget_modify_fg(GTK_WIDGET(g_wdgFrameNum), GTK_STATE_NORMAL, &color_green);
	sprintf(str,"%d", 1);
	gtk_entry_set_text(GTK_ENTRY(g_wdgInpFrameNum), str);

	
	// 基本深度层宽高
	maxhw = H3D_MAX(g_frameInWidth, g_frameInHeight);
	if(maxhw>MAXWIDTH)
	{
		printf("SIZE	ERROR --- Frame size should be less than 2048.\n");
		return ;
	}
	if(maxhw<MINWIDTH)
	{
		printf("SIZE	ERROR --- Frame size should be more than 192.\n");
		return ;
	}
	
	hw = maxhw;
	for(stage=2; ;stage++) // 最少有两层
	{
		hw = hw>>1;
		if(hw<=40)
			break;
	}
	
	g_frameBaseWidth = g_frameInWidth>>(stage-1);
	g_frameBaseHeight = g_frameInHeight>>(stage-1);
	
	printf("SIZE	 INFO --- Org %dx%d Display %dx%d Base %dx%d\n", g_frameInWidth, g_frameInHeight, g_frameWidth, g_frameHeight, g_frameBaseWidth, g_frameBaseHeight);

	//if(g_frameResizeFlag==1)
	{
		g_yuvorginput.y = new uint8_t [sizeorg];
		g_yuvorginput.u = new uint8_t [sizeorg>>2];
		g_yuvorginput.v = new uint8_t [sizeorg>>2];
		g_yuvorginputref.y = new uint8_t [sizeorg];
		g_yuvorginputref.u = new uint8_t [sizeorg>>2];
		g_yuvorginputref.v = new uint8_t [sizeorg>>2];
		g_rgborginput = new uint8_t [sizeorg*3];				
		g_rgborginputref = new uint8_t [sizeorg*3];
		g_renderorgtmp0 = new uint8_t [sizeorg*6];
		g_renderorgtmp1 = new uint8_t [sizeorg*6];
	}
	
	g_yuvinput.y = new uint8_t [size];
	g_yuvinput.u = new uint8_t [size>>2];
	g_yuvinput.v = new uint8_t [size>>2];
	g_rgbinput = new uint8_t [size*3];
	g_rgbinputref = new uint8_t [size*3];
	g_rendertmp0 = new uint8_t [size*6];
	g_rendertmp1 = new uint8_t [size*6];
	g_yuvinputref.y = new uint8_t [size];
	g_yuvinputref.u = new uint8_t [size>>2];
	g_yuvinputref.v = new uint8_t [size>>2];
	g_autosegdisplay = new uint8_t [size*3];
	g_manusegdisplay = new uint8_t [size*3];

	g_OrgLabels0 = new int [sizeorg];
	g_SegLabels0 = new int [sizeorg];
	g_RefOrgLabels0 = new int [sizeorg];
	g_RefSegLabels0 = new int [sizeorg];
	g_OrgLabels = new int [size];
	g_SegLabels = new int [size];
	g_RefOrgLabels = new int [size];
	g_RefSegLabels = new int [size];
	g_SegDepths0 = new uint8_t [sizeorg];
	g_RefSegDepths0 = new uint8_t [sizeorg];
	g_SegDepths = new uint8_t [size];
	g_RefSegDepths = new uint8_t [size];

	g_Ref1OrgLabels0 = new int [sizeorg];
	g_Ref1SegLabels0 = new int [sizeorg];
	g_Ref1OrgLabels = new int [size];
	g_Ref1SegLabels = new int [size];
	g_Ref1SegDepths0 = new uint8_t [sizeorg];
	g_Ref1SegDepths = new uint8_t [size];
	
#ifdef WIN32
	g_enlargedisplay = new uint8_t [size*12 + size];
	g_enlargedisplay = g_enlargedisplay + size;
	g_enlargedisplaytmp0 = new uint8_t [size*12 + size];
	g_enlargedisplaytmp0 = g_enlargedisplaytmp0 + size;
	g_enlargedisplaytmp1 = new uint8_t [size*12 + size];
	g_enlargedisplaytmp1 = g_enlargedisplaytmp1 + size;
#else
	g_enlargedisplay = new uint8_t [size*12];
	g_enlargedisplaytmp0 = new uint8_t [size*12];
	g_enlargedisplaytmp1 = new uint8_t [size*12];
#endif
	g_depthdisplay = new uint8_t [size];
	g_tmpdisplay = new uint8_t [size*3*4];
	g_tmpdisplay1 = new uint8_t [size*3];
	g_tmpdisplay2 = new uint8_t [size*3];
	g_tmpdisplay3 = new uint8_t [size*3];
#if 1
	g_renderdisplay1 = new uint8_t [size*3];
	g_renderdisplay2 = new uint8_t [size*3];
	g_renderdisplay3 = new uint8_t [size*3];
	g_renderdisplay4 = new uint8_t [size*3];
	g_renderdisplay5 = new uint8_t [size*3];
	g_renderdisplay6 = new uint8_t [size*3];
	g_renderdisplay7 = new uint8_t [size*3];
	g_renderdisplay8 = new uint8_t [size*3];
#endif
	g_renderlabel1 = new uint8_t [size];
	g_renderlabel2 = new uint8_t [size];

	g_segmentedflags = new uint8_t [sizeorg];

	g_BaseDepth = new uint8_t [g_frameBaseWidth*g_frameBaseHeight];
	g_RefBaseDepth = new uint8_t [g_frameBaseWidth*g_frameBaseHeight];
	g_Ref1BaseDepth = new uint8_t [g_frameBaseWidth*g_frameBaseHeight];

	for(i=0;i<size;i++)
	{
		g_depthdisplay[i]=0;
		g_renderlabel1[i]=0;
		g_renderlabel2[i]=0;
		g_segmentedflags[i]=0;
	}

	for(i=0;i<g_frameBaseWidth*g_frameBaseHeight;i++)
	{
		g_BaseDepth[i] = 0;
	}

	/* 3?ê??ˉ??*/
	g_frames = new SFrameInfo [g_frameMaxNum];
	for(i=0;i<g_frameMaxNum;i++)
	{
		g_frames[i].KeyFlag = 0;
		g_frames[i].SegFlag = 0;
		g_frames[i].DepFlag = 0;
		g_frames[i].DepBaseFlag = 0;
		g_frames[i].ChangeFlag = 0;
		g_frames[i].LayerEditNum = 0;
		g_frames[i].LayerBaseNum = 1;
		g_frames[i].BackgroundFrame = -1;
		g_frames[i].SegLabelMask = 0;
		g_frames[i].AutoSegLabel = 1;
		//g_frames[i].RefineSegLabel = 0;
		//g_frames[i].RefineSegNum = 0;
		g_frames[i].finfo = NULL;

		g_frames[i].OrgLabels0 = g_OrgLabels0;
		g_frames[i].SegLabels0 = g_SegLabels0;
		g_frames[i].SegDepths0 = g_SegDepths0;
		g_frames[i].OrgLabels = g_OrgLabels;
		g_frames[i].SegLabels = g_SegLabels;
		g_frames[i].SegDepths = g_SegDepths;
		//g_frames[i].Segmented = NULL;
		
		g_frames[i].LayerVisibleNum = 1;
		g_frames[i].rgbinput = g_rgbinput;

		g_frames[i].framedepth.setflag = 0;
		g_frames[i].framedepth.depthmin = 0;
		g_frames[i].framedepth.depthmax = 0;
		g_frames[i].framedepth.depthmode = 0;

		for(j=0;j<LAYERBUTTONNUMMAX;j++)
		{
			g_frames[i].layerdepth[j].setflag = 0;
			g_frames[i].layerdepth[j].depthmin = 0;
			g_frames[i].layerdepth[j].depthmax = 0;
			g_frames[i].layerdepth[j].depthmode = 0;
		}
	}
	g_frames[0].KeyFlag = 1;
	
	g_frameIndex = 0;

	g_tmpOrgLabels0 = new int[sizeorg];
	g_tmpSegLabels0 = new int[sizeorg];
	g_tmpSegDepths0 = new uint8_t[sizeorg];
	g_tmpOrgLabels = new int[size];
	g_tmpSegLabels = new int[size];
	g_tmpSegDepths = new uint8_t[size];

	for(i=0;i<sizeorg;i++)
	{
		g_frames[g_frameIndex].OrgLabels0[i] = g_frames[g_frameIndex].SegLabels0[i] = 0;
		g_frames[g_frameIndex].SegDepths0[i] = 255;
	}
		
	for(i=0;i<size;i++)
	{
		g_frames[g_frameIndex].OrgLabels[i] = g_frames[g_frameIndex].SegLabels[i] = 0;
		g_frames[g_frameIndex].SegDepths[i] = 255;
	}

	if(g_inputfileformat!=INPIMG)
	{
		if(g_frameResizeFlag==1)
		{
			fread(g_yuvorginput.y,sizeorg,1,g_fileInpYUV);
			fread(g_yuvorginput.u,sizeorg>>2,1,g_fileInpYUV);
			fread(g_yuvorginput.v,sizeorg>>2,1,g_fileInpYUV);
			yuvresize(g_yuvorginput, g_yuvinput, g_frameInWidth, g_frameInHeight, g_frameWidth, g_frameHeight);
			yuv2rgb(g_yuvorginput.y, g_yuvorginput.v, g_yuvorginput.u, g_rgborginput, g_frameInWidth, g_frameInHeight);
			memcpy(g_rgborginputref, g_rgborginput, g_frameInWidth*g_frameInHeight*3);
		}
		else
		{
			fread(g_yuvinput.y,size,1,g_fileInpYUV);
			fread(g_yuvinput.u,size>>2,1,g_fileInpYUV);
			fread(g_yuvinput.v,size>>2,1,g_fileInpYUV);
		}
		
		yuv2rgb(g_yuvinput.y, g_yuvinput.v, g_yuvinput.u, g_frames[g_frameIndex].rgbinput, g_frameWidth, g_frameHeight);
	}

}

void OpenInputFile(char *filename)
{
	int i,j;
#ifdef I64
	int64_t filebytes = 0;
#else
	unsigned long filebytes = 0;
#endif
	
	int width, height;
	CvSize sz;
	IplImage* img;
	char tmp[256];
	int fileexist = 0;

	
	
	if((g_fileInpYUV=fopen(filename,"rb"))==NULL)
	{
		printf("FILE    ERROR --- input file %s open failed!\n", filename);
		messagebox(MSGBOX_IFOFAIL);
		//if(filename) free(filename);
		return ;
	}
	else
	{
		printf("FILE     INFO --- input file open successed!\n");
		
		if(g_inputfileformat==INPYUV)
		{
			width = g_frameInWidth;
			height = g_frameInHeight;

			g_frameResizeFlag = 0;
			if(width>IMGWIDTHMAX)
			{
				g_frameResizeFlag = 1;
				width = IMGWIDTHMAX;
				height = g_frameInHeight*IMGWIDTHMAX/g_frameInWidth;
			}
			if(height>IMGHEIGHTMAX)
			{
				g_frameResizeFlag = 1;
				width = g_frameInWidth*IMGHEIGHTMAX/g_frameInHeight;
				height = IMGHEIGHTMAX;
			}

			g_frameWidth = width;
			g_frameHeight = height;
			
			#ifdef I64
			_fseeki64(g_fileInpYUV, 0, SEEK_END);
			filebytes = _ftelli64(g_fileInpYUV);
			_fseeki64(g_fileInpYUV, 0, SEEK_SET);
			#else
			fseek(g_fileInpYUV, 0, SEEK_END);
			filebytes = ftell(g_fileInpYUV);
			fseek(g_fileInpYUV, 0, SEEK_SET);
			#endif

			g_frameMaxNum = (int)(filebytes/IMAGE_SIZE(g_frameInWidth,g_frameInHeight));

			InputInit();

		}
		else if(g_inputfileformat==INPIMG)
		{
			img = cvLoadImage(filename);

			if(img==NULL)
			{
				messagebox(MSGBOX_LIMGERR);
				return ;
			}
			else
			{
				g_frameMaxNum = 1;
				g_frameResizeFlag = 0;
				
				g_frameInWidth = img->width;
				g_frameInHeight = img->height;
				
				if(g_frameInWidth>IMGWIDTHMAX)
				{
					g_frameResizeFlag = 1;
					g_frameWidth = IMGWIDTHMAX;
					g_frameHeight = img->height*IMGWIDTHMAX/img->width;
				}
				if(g_frameHeight>IMGHEIGHTMAX)
				{
					g_frameResizeFlag = 1;
					g_frameWidth = img->width*IMGHEIGHTMAX/img->height;
					g_frameHeight = IMGHEIGHTMAX;
				}
								
				InputInit();
					
				sz.width = g_frameWidth;
				sz.height = g_frameHeight;
				
				IplImage* imgrsz = cvCreateImage(sz,img->depth,3);
				cvResize(img,imgrsz,CV_INTER_CUBIC);
				
				sprintf(tmp,"./%s/orgresize.png",cfgparams.projectname);
				cvSaveImage(tmp, imgrsz);

				if(g_frameResizeFlag==1)
				{
					for(j=0;j<g_frameInHeight;j++)
						for(i=0;i<g_frameInWidth;i++)
						{
							// 注意r,g,b 顺序 
							g_rgborginput[i*3+j*3*g_frameInWidth] = g_rgborginputref[i*3+j*3*g_frameInWidth] = (uint8_t)(img->imageData[i*3+2+j*img->widthStep]);
							g_rgborginput[i*3+1+j*3*g_frameInWidth] = g_rgborginputref[i*3+1+j*3*g_frameInWidth] = (uint8_t)(img->imageData[i*3+1+j*img->widthStep]);
							g_rgborginput[i*3+2+j*3*g_frameInWidth] = g_rgborginputref[i*3+2+j*3*g_frameInWidth] = (uint8_t)(img->imageData[i*3+0+j*img->widthStep]);
						}
					rgb2yuv(g_rgborginput, g_yuvorginput.y,g_yuvorginput.u,g_yuvorginput.v, g_frameInWidth, g_frameInHeight);
					for(j=0;j<g_frameHeight;j++)
						for(i=0;i<g_frameWidth;i++)
						{
							// 注意r,g,b 顺序 
							g_rgbinput[i*3+j*3*g_frameWidth] = (uint8_t)(imgrsz->imageData[i*3+2+j*imgrsz->widthStep]);
							g_rgbinput[i*3+1+j*3*g_frameWidth] = (uint8_t)(imgrsz->imageData[i*3+1+j*imgrsz->widthStep]);
							g_rgbinput[i*3+2+j*3*g_frameWidth] = (uint8_t)(imgrsz->imageData[i*3+0+j*imgrsz->widthStep]);
						}
					rgb2yuv(g_rgbinput, g_yuvinput.y, g_yuvinput.v, g_yuvinput.u, g_frameWidth, g_frameHeight);
				}
				else
				{
					for(j=0;j<g_frameHeight;j++)
						for(i=0;i<g_frameWidth;i++)
						{
							// 注意r,g,b 顺序 
							g_rgbinputref[i*3+0+j*3*g_frameWidth] = g_rgbinput[i*3+j*3*g_frameWidth] = (uint8_t)(imgrsz->imageData[i*3+2+j*imgrsz->widthStep]);
							g_rgbinputref[i*3+1+j*3*g_frameWidth] = g_rgbinput[i*3+1+j*3*g_frameWidth] = (uint8_t)(imgrsz->imageData[i*3+1+j*imgrsz->widthStep]);
							g_rgbinputref[i*3+2+j*3*g_frameWidth] = g_rgbinput[i*3+2+j*3*g_frameWidth] = (uint8_t)(imgrsz->imageData[i*3+0+j*imgrsz->widthStep]);
						}
					rgb2yuv(g_rgbinput, g_yuvinput.y, g_yuvinput.v, g_yuvinput.u, g_frameWidth, g_frameHeight);
				}

				cvReleaseImage(&img);
				cvReleaseImage(&imgrsz);
			}
			
		}
		else if(g_inputfileformat==INPVID)
		{
			char cmd[1024];

			if(g_videoinfoflag==0)
			{
				FILE * fmediainfo = NULL;
				sprintf(cmd, "ffmpeg.exe -y -ss 0 -t 1 -i %s -vcodec copy -bsf h264_mp4toannexb -acodec copy %s/%s_tmp.ts\n", filename, cfgparams.projectname, cfgparams.projectname);
				printf("CMD     DEBUG --- [%s:%d] %s", __FILE__, __LINE__, cmd);
				system(cmd);
				if((fmediainfo = fopen("mediainfo","rb"))==NULL)
				{
					printf("FILE    ERROR --- load mediainfo failed!\n");
					return ;
				}
				fread(&g_timetotal, 4, 1, fmediainfo);
				printf("FILE     INFO --- Media length: %d seconds\n",g_timetotal);
				fclose(fmediainfo);
				
				sprintf(cmd, "del %s\\%s_tmp.ts\n", cfgparams.projectname, cfgparams.projectname);
				system(cmd);

				if(g_timetotal<=0)
				{
					g_timetotal = 1;
				}
				if(g_timeend>g_timetotal)
					g_timeend = g_timetotal;
				if(g_timestart>g_timeend)
					g_timestart = g_timetotal-1;

				//printf("#######################g_sectionduration %d \n", g_sectionduration);

				if(g_sectionduration<=0)
				{
					g_sectionduration = 30;
				}

				
				if(g_sectionflag==0)
				{
					g_sectionnum = 0;
					g_sectionlastdura = g_timeend-g_timestart;
				}
				else/* 默认分段*/
				{
					g_sectionnum = (g_timeend-g_timestart)/g_sectionduration;
					g_sectionlastdura = (g_timeend-g_timestart)%g_sectionduration;
				}
				
				if(g_sectionlastdura!=0)
					g_sectionnum++;
				else
					g_sectionlastdura = g_sectionduration;

				g_videoinfoflag = 1;

			}
			
			if(g_keyframeflag==0)
			{
				sprintf(tmp, "%s/%s_%d.yuv", cfgparams.projectname, cfgparams.projectname,g_sectioncur);
				fileexist = (_access(tmp,0)==0);
				if(!fileexist)
				{
					if(g_sectionflag==1)
					{
						if(g_sectioncur<g_sectionnum)
						{
							sprintf(cmd, "ffmpeg.exe -y -ss %d -t %d -i %s -s %dx%d %s/%s_%d.yuv\n", g_timestart+(g_sectioncur-1)*g_sectionduration, g_sectionduration, filename, g_frameInWidth, g_frameInHeight, cfgparams.projectname, cfgparams.projectname,g_sectioncur);
							printf("CMD     DEBUG --- [%s:%d] %s", __FILE__, __LINE__, cmd);
						}
						else // 最后一段
						{
							g_sectionlastdura = (g_timeend-g_timestart)%g_sectionduration;
							if(g_sectionlastdura==0)
								g_sectionlastdura = g_sectionduration;
							sprintf(cmd, "ffmpeg.exe -y -ss %d -t %d -i %s -s %dx%d %s/%s_%d.yuv\n", g_timestart+(g_sectioncur-1)*g_sectionduration, g_sectionlastdura, filename, g_frameInWidth, g_frameInHeight, cfgparams.projectname, cfgparams.projectname,g_sectioncur);
							printf("CMD     DEBUG --- [%s:%d] %s", __FILE__, __LINE__, cmd);
						}
					}
					else
					{
						sprintf(cmd, "ffmpeg.exe -y -ss %d -t %d -i %s -s %dx%d %s/%s_%d.yuv\n", g_timestart, g_sectionlastdura, filename, g_frameInWidth, g_frameInHeight, cfgparams.projectname, cfgparams.projectname,g_sectioncur);
						printf("CMD     DEBUG --- [%s:%d] %s", __FILE__, __LINE__, cmd);
					}
					system(cmd);
				}
				//sprintf(tmp, "%s/%s_%d.yuv", cfgparams.projectname, cfgparams.projectname,g_sectioncur);
			}
			else
			{
				sprintf(tmp, "%s/%s_%d_key.yuv", cfgparams.projectname, cfgparams.projectname,g_sectioncur);
				fileexist = (_access(tmp,0)==0);
				if(!fileexist)
				{
					if(g_sectionflag==1)
					{
						if(g_sectioncur<g_sectionnum)
						{
							sprintf(cmd, "ffmpeg.exe -y -ss %d -t %d -scenecut 1 -scenecutth %d -i %s -s %dx%d %s/%s_%d_key.yuv\n", g_timestart+(g_sectioncur-1)*g_sectionduration, g_sectionduration, g_keyframeth, filename, g_frameInWidth, g_frameInHeight, cfgparams.projectname, cfgparams.projectname,g_sectioncur);
							printf("CMD     DEBUG --- [%s:%d] %s", __FILE__, __LINE__, cmd);
						}
						else // 最后一段
						{
							g_sectionlastdura = (g_timeend-g_timestart)%g_sectionduration;
							if(g_sectionlastdura==0)
								g_sectionlastdura = g_sectionduration;
							sprintf(cmd, "ffmpeg.exe -y -ss %d -t %d -scenecut 1 -scenecutth %d -i %s -s %dx%d %s/%s_%d_key.yuv\n", g_timestart+(g_sectioncur-1)*g_sectionduration, g_sectionlastdura, g_keyframeth, filename, g_frameInWidth, g_frameInHeight, cfgparams.projectname, cfgparams.projectname,g_sectioncur);
							printf("CMD     DEBUG --- [%s:%d] %s", __FILE__, __LINE__, cmd);
						}
					}
					else
					{
						sprintf(cmd, "ffmpeg.exe -y -ss %d -t %d -scenecut 1 -scenecutth %d -i %s -s %dx%d %s/%s_%d_key.yuv\n", g_timestart, g_sectionlastdura, g_keyframeth, filename, g_frameInWidth, g_frameInHeight, cfgparams.projectname, cfgparams.projectname,g_sectioncur);
						printf("CMD     DEBUG --- [%s:%d] %s", __FILE__, __LINE__, cmd);
					}
					system(cmd);
					
					sprintf(cmd, "move keyframes.info %s/keyframes_%d.info", cfgparams.projectname, g_sectioncur);
					system(cmd);
				}
				//sprintf(tmp, "%s/%s_%d_key.yuv", cfgparams.projectname, cfgparams.projectname,g_sectioncur);
			}
			
			if((g_fileInpYUV=fopen(tmp,"rb"))==NULL)
			{
				printf("FILE    ERROR --- video file %s open failed!\n", tmp);
				messagebox(MSGBOX_VFOFAIL);
				return ;
			}
			else
			{
				printf("FILE     INFO --- video file open successed!\n");
				
				width = g_frameInWidth;
				height = g_frameInHeight;

				g_frameResizeFlag = 0;
				if(width>IMGWIDTHMAX)
				{
					g_frameResizeFlag = 1;
					width = IMGWIDTHMAX;
					height = g_frameInHeight*IMGWIDTHMAX/g_frameInWidth;
				}
				if(height>IMGHEIGHTMAX)
				{
					g_frameResizeFlag = 1;
					width = g_frameInWidth*IMGHEIGHTMAX/g_frameInHeight;
					height = IMGHEIGHTMAX;
				}

				g_frameWidth = width;
				g_frameHeight = height;

				#ifdef I64
				_fseeki64(g_fileInpYUV, 0, SEEK_END);
				filebytes = _ftelli64(g_fileInpYUV);
				_fseeki64(g_fileInpYUV, 0, SEEK_SET);
				#else
				fseek(g_fileInpYUV, 0, SEEK_END);
				filebytes = ftell(g_fileInpYUV);
				fseek(g_fileInpYUV, 0, SEEK_SET);
				#endif

				g_frameMaxNum = (int)(filebytes/IMAGE_SIZE(g_frameInWidth,g_frameInHeight));
				//printf(" max frames : %d\n", g_frameMaxNum);
				if(g_keyframeflag)
				{
					FILE * fkeyframeinfo = NULL;
					int keyframenum = 0;
					
					sprintf(tmp, "%s/keyframes_%d.info", cfgparams.projectname, g_sectioncur);
					fkeyframeinfo = fopen(tmp, "rb");
					fseek(fkeyframeinfo, 0, SEEK_END);
					filebytes = ftell(fkeyframeinfo);
					fseek(fkeyframeinfo, 0, SEEK_SET);
					keyframenum = (int)filebytes/4;
					if(keyframenum!=g_frameMaxNum)
						printf("FILE    ERROR --- video file and keyframe info file don't match!\n");
					g_keyframenumarr = new int[keyframenum];
					fread(g_keyframenumarr, 4, keyframenum, fkeyframeinfo);
					fclose(fkeyframeinfo);
					#if 0
					for(i=0;i<keyframenum;i++)
					{
						if((i%10)==0) printf("\n");
						printf("%d ", g_keyframenumarr[i]);
						
					}
					printf("\n");
					#endif
				}

				InputInit();
			}
		}
		else
		{
			// unknow format
		}
		
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
		
			
		//if(filename) free(filename);
		return ;
	}

}

void OpenFrameInfoFile(char * filename)
{
	int len = 0;
	int i,j;
	SFrameJump jmp;
	
	if((g_fileLoadSeg=fopen(filename,"rb"))==NULL)
    {
		printf("FILE    ERROR --- Load keyframe info file %s open failed!\n", filename);
		return ;
    }
    else
    {
		fseek(g_fileLoadSeg,0,SEEK_END);
		len=ftell(g_fileLoadSeg);
		fseek(g_fileLoadSeg,0,SEEK_SET);

		if(len!=0)
		{
			printf("FILE     INFO --- Load keyframe info file open successed!\n");

			for(j=0;j<len;j+=4)
			{
				fread(&i, 4, 1, g_fileLoadSeg); // frame index
				if(i<g_frameMaxNum)
					g_frames[i].KeyFlag = 1;				
			}
			fclose(g_fileLoadSeg);

			GetKeyFrames();

			jmp.framenum = 0;
			jmp.jumpmode = JMPMODECUR;

			if(g_frameMaxNum>0) // 表示有yuv文件打开
				jumpframe(NULL, &jmp);
			else
				messagebox(MSGBOX_OFFIRST);
			
			return ;
		}
	}
	return ;
}
// 删除，这里的infofile用作关键帧信息存储等整体存储TODO
/*
void OpenInfoFile(char * filename)
{
    int n=0;
	int i=0;
	int j;
	#ifdef I64
	int64_t len=0;
	#else
	unsigned long len=0;
	#endif
	char filepr[4];
	int width, height;
	int szorg = g_frameInWidth*g_frameInHeight;
	int sz = g_frameWidth*g_frameHeight;
	SFrameJump jmp;

	if((g_fileLoadSeg=fopen(filename,"rb"))==NULL)
    {
		printf("load segment file open failed!\n");
		return ;
    }
    else
    {
    	#ifdef I64
		_fseeki64(g_fileLoadSeg,0,SEEK_END);
		len=_ftelli64(g_fileLoadSeg);
		_fseeki64(g_fileLoadSeg,0,SEEK_SET);
		#else
		fseek(g_fileLoadSeg,0,SEEK_END);
		len=ftell(g_fileLoadSeg);
		fseek(g_fileLoadSeg,0,SEEK_SET);
		#endif

		if(len!=0)
		{
			printf("load segment file open successed!\n");
			//for(i=0;i<g_frameMaxNum;i++)
			//while (!feof(g_fileLoadSeg)&&i<g_frameMaxNum)
			while (i<g_frameMaxNum)
			{
				fread(filepr, sizeof(filepr), 1, g_fileLoadSeg);
				if(filepr[0]!=H3D_SegmentFilePR[0]||filepr[1]!=H3D_SegmentFilePR[1]||filepr[2]!=H3D_SegmentFilePR[2]||filepr[3]!=H3D_SegmentFilePR[3])
				{
					messagebox("Load segment file format is wrong!");
					return ;
				}
				fread(&i, 4, 1, g_fileLoadSeg); // frame index
				fread(&width, 4, 1, g_fileLoadSeg); // frame width
				fread(&height, 4, 1, g_fileLoadSeg); // frame height
				fread(&g_frames[i].SegFlag, 4, 1, g_fileLoadSeg);
				fread(&g_frames[i].KeyFlag, 4, 1, g_fileLoadSeg);
				fread(&g_frames[i].AutoSegLabel, 4, 1, g_fileLoadSeg);
				fread(&g_frames[i].SegLabelMask, 4, 1, g_fileLoadSeg);
				fread(&g_frames[i].LayerVisibleNum, 4, 1, g_fileLoadSeg);

				// TODO 注意逻辑，前面认为可以load其他info文件，所以需要判断segflag，目前仅仅是打开文件时load info文件，所以不论什么情况都需要创建labels和depths
				g_frames[i].SegFlag = 1;

				g_frames[i].OrgLabels0 = new int[szorg];
				g_frames[i].SegLabels0 = new int[szorg];
				g_frames[i].SegDepths0 = new uint8_t[szorg];

				g_frames[i].OrgLabels = new int[sz];
				g_frames[i].SegLabels = new int[sz];
				g_frames[i].SegDepths = new uint8_t[sz];

				g_frames[i].LayerEditNum = 0;

				if(g_frameResizeFlag==1)
				{
					fread(g_frames[i].OrgLabels0, 4, szorg,g_fileLoadSeg);
					fread(g_frames[i].SegLabels0, 4, szorg,g_fileLoadSeg);
					LabelResize(g_frames[i].OrgLabels0,g_frames[i].OrgLabels,g_yuvorginput.y, g_yuvinput.y, width,height,g_frameWidth,g_frameHeight);
					LabelResize(g_frames[i].SegLabels0,g_frames[i].SegLabels,g_yuvorginput.y, g_yuvinput.y, width,height,g_frameWidth,g_frameHeight);
				}
				else
				{
					fread(g_frames[i].OrgLabels, 4, width*height,g_fileLoadSeg);
					fread(g_frames[i].SegLabels, 4, width*height,g_fileLoadSeg);
					//fread(g_frames[i].SegDepths, 4, width*height,g_fileLoadSeg);
				}
				
				for(j=0;j<g_frames[i].LayerVisibleNum;j++)
				{
					fread(&g_frames[i].layerdepth[j].setflag, 4, 1, g_fileLoadSeg);
					fread(&g_frames[i].layerdepth[j].depthmin, 4, 1, g_fileLoadSeg);
					fread(&g_frames[i].layerdepth[j].depthmax, 4, 1, g_fileLoadSeg);
					fread(&g_frames[i].layerdepth[j].depthmode, 4, 1, g_fileLoadSeg);
					int lb = g_frameWidth*g_frameHeight + (1<<j);
					if(g_frames[i].layerdepth[j].setflag)
					{
						set_depth_value(i, lb, g_frames[i].layerdepth[j].depthmin, g_frames[i].layerdepth[j].depthmax, g_frames[i].layerdepth[j].depthmode);
						if(g_frameResizeFlag==1)
							set_depth_value0(i, lb, g_frames[i].layerdepth[j].depthmin, g_frames[i].layerdepth[j].depthmax, g_frames[i].layerdepth[j].depthmode);
					}
				}

				
				
				jmp.framenum = i;
				#ifdef I64
				if(len==_ftelli64(g_fileLoadSeg))
					break;
				#else
				if(len==ftell(g_fileLoadSeg))
					break;
				#endif
				
			}

			//printf(" n: %d width: %d height: %d \n", n, width, height);
			//printf(" g_frameAutoSegLabel: %d g_frameSegLabel: %x g_frameSegLabelMask: %d \n", g_frameAutoSegLabel, g_frameSegLabel, g_frameSegLabelMask);
			fclose(g_fileLoadSeg);

			jmp.jumpmode = JMPMODECUR;

			GetKeyFrames();

			if(g_frameMaxNum>0) // 表示有yuv文件打开
				jumpframe(NULL, &jmp);
			else
				messagebox("Please open a file first.");
			
			return ;
		}
	}

}
*/
/*
void savesegmentframe()
{

	int i= g_frameIndex;
	int j;
	char filename[128];

	sprintf(filename,"./%s/segsnap_%d.dat",cfgparams.projectname,i+1);
	FILE * segsnapfile;
	
	if((segsnapfile=fopen(filename,"wb"))==NULL)
	{
		printf("save segment snap file %s open failed!\n", filename);
		return ;
	}
	else
	{
		printf("save segment snap file %s open successed!\n", filename);

		if(g_frames[i].SegFlag==1)
		{
			fwrite(H3D_SegmentFilePR, sizeof(H3D_SegmentFilePR), 1, segsnapfile);
			fwrite(&i, 4, 1, segsnapfile); // frame index
			fwrite(&g_frameInWidth, 4, 1, segsnapfile); // frame width
			fwrite(&g_frameInHeight, 4, 1, segsnapfile); // frame height
			fwrite(&g_frames[i].SegFlag, 4, 1, segsnapfile);
			fwrite(&g_frames[i].KeyFlag, 4, 1, segsnapfile);
			//fwrite(&g_frames[i].RefineSegLabel, 4, 1, segsnapfile);
			//fwrite(&g_frames[i].RefineSegNum, 4, 1, segsnapfile);
			fwrite(&g_frames[i].AutoSegLabel, 4, 1, segsnapfile);
			fwrite(&g_frames[i].SegLabelMask, 4, 1, segsnapfile);
			fwrite(&g_frames[i].LayerVisibleNum, 4, 1, segsnapfile);
			if(g_frameResizeFlag==1)
			{
				fwrite(g_frames[i].OrgLabels0, 4, g_frameInWidth*g_frameInHeight,segsnapfile);
				fwrite(g_frames[i].SegLabels0, 4, g_frameInWidth*g_frameInHeight,segsnapfile);
			}
			else
			{
				fwrite(g_frames[i].OrgLabels, 4, g_frameWidth*g_frameHeight,segsnapfile);
				fwrite(g_frames[i].SegLabels, 4, g_frameWidth*g_frameHeight,segsnapfile);
			}
			
			for(j=0;j<=g_frames[i].LayerVisibleNum;j++)
			{
				fwrite(&g_frames[i].layerdepth[j].setflag, 4, 1, segsnapfile);
				fwrite(&g_frames[i].layerdepth[j].depthmin, 4, 1, segsnapfile);
				fwrite(&g_frames[i].layerdepth[j].depthmax, 4, 1, segsnapfile);
				fwrite(&g_frames[i].layerdepth[j].depthmode, 4, 1, segsnapfile);
			}
			//fwrite(g_frames[i].SegDepths, 4, g_frameWidth*g_frameHeight,g_fileSaveSeg);
		}
		
		fclose(segsnapfile);
		
		return ;
	}
		
}
*/
/*
void saverendercurframe()
{
	int i, j;
	
	char filename[128];
	
	CvSize sz;
	IplImage* stereoimg;

	if(g_frameMaxNum>0)
	{
		sprintf(filename,"./%s/rendersnap_%d.png",cfgparams.projectname,g_frameIndex+1);
				
		sz.width = g_frameWidth*2;
		sz.height = g_frameHeight;
		
		stereoimg = cvCreateImage(sz, IPL_DEPTH_8U, 3);
		for(j=0;j<g_frameHeight;j++)
			for(i=0;i<g_frameWidth;i++)
			{
				stereoimg->imageData[i*3+2+j*stereoimg->widthStep] = g_tmpdisplay1[i*3+0+j*3*g_frameWidth];
				stereoimg->imageData[i*3+1+j*stereoimg->widthStep] = g_tmpdisplay1[i*3+1+j*3*g_frameWidth];
				stereoimg->imageData[i*3+0+j*stereoimg->widthStep] = g_tmpdisplay1[i*3+2+j*3*g_frameWidth];
				stereoimg->imageData[i*3+2+g_frameWidth*3+j*stereoimg->widthStep] = g_tmpdisplay2[i*3+0+j*3*g_frameWidth];
				stereoimg->imageData[i*3+1+g_frameWidth*3+j*stereoimg->widthStep] = g_tmpdisplay2[i*3+1+j*3*g_frameWidth];
				stereoimg->imageData[i*3+0+g_frameWidth*3+j*stereoimg->widthStep] = g_tmpdisplay2[i*3+2+j*3*g_frameWidth];
			}
		
		cvSaveImage(filename, stereoimg);
		cvReleaseImage( &stereoimg );
	}
		
	return ;

}
*/
void cancelrenderfineedit()
{
	int i, j;
	IplImage* imgtmp;	
	
	char filename[128];
	
	if(g_frameMaxNum>0||g_renderoutputformat==RENDEROUTAN) 
	{
		// 保存tmp0 -> tmp1
		if(g_keyframeflag)
			sprintf(filename,"./%s/render/%d_rendersnap_%d.png",cfgparams.projectname,g_sectioncur,g_keyframenumarr[g_frameIndex]+1);
		else
			sprintf(filename,"./%s/render/%d_rendersnap_%d.png",cfgparams.projectname,g_sectioncur,g_frameIndex+1);
		imgtmp = cvLoadImage(filename);
		if(g_frameResizeFlag==1)
		{
			for(j=0;j<g_frameInHeight;j++)
				for(i=0;i<g_frameInWidth*2;i++)
				{
					imgtmp->imageData[i*3+2+j*imgtmp->widthStep] = g_renderorgtmp1[i*3+0+j*g_frameInWidth*6] = g_renderorgtmp0[i*3+0+j*g_frameInWidth*6];
					imgtmp->imageData[i*3+1+j*imgtmp->widthStep] = g_renderorgtmp1[i*3+1+j*g_frameInWidth*6] = g_renderorgtmp0[i*3+1+j*g_frameInWidth*6];
					imgtmp->imageData[i*3+0+j*imgtmp->widthStep] = g_renderorgtmp1[i*3+2+j*g_frameInWidth*6] = g_renderorgtmp0[i*3+2+j*g_frameInWidth*6];
				}
		}
		else
		{
			for(j=0;j<g_frameHeight;j++)
				for(i=0;i<g_frameWidth*2;i++)
				{
					imgtmp->imageData[i*3+2+j*imgtmp->widthStep] = g_rendertmp0[i*3+0+j*g_frameWidth*6];
					imgtmp->imageData[i*3+1+j*imgtmp->widthStep] = g_rendertmp0[i*3+1+j*g_frameWidth*6];
					imgtmp->imageData[i*3+0+j*imgtmp->widthStep] = g_rendertmp0[i*3+2+j*g_frameWidth*6];
				}
		}
		cvSaveImage(filename, imgtmp);
		cvReleaseImage( &imgtmp );
		
		for(j=0;j<g_frameHeight;j++)
			for(i=0;i<g_frameWidth;i++)
			{
				g_tmpdisplay1[i*3+0+j*3*g_frameWidth] = g_rendertmp1[i*3+0+j*g_frameWidth*6] = g_rendertmp0[i*3+0+j*g_frameWidth*6];
				g_tmpdisplay1[i*3+1+j*3*g_frameWidth] = g_rendertmp1[i*3+1+j*g_frameWidth*6] = g_rendertmp0[i*3+1+j*g_frameWidth*6];
				g_tmpdisplay1[i*3+2+j*3*g_frameWidth] = g_rendertmp1[i*3+2+j*g_frameWidth*6] = g_rendertmp0[i*3+2+j*g_frameWidth*6];
				g_tmpdisplay2[i*3+0+j*3*g_frameWidth] = g_rendertmp1[i*3+0+g_frameWidth*3+j*g_frameWidth*6] = g_rendertmp0[i*3+0+g_frameWidth*3+j*g_frameWidth*6];
				g_tmpdisplay2[i*3+1+j*3*g_frameWidth] = g_rendertmp1[i*3+1+g_frameWidth*3+j*g_frameWidth*6] = g_rendertmp0[i*3+1+g_frameWidth*3+j*g_frameWidth*6];
				g_tmpdisplay2[i*3+2+j*3*g_frameWidth] = g_rendertmp1[i*3+2+g_frameWidth*3+j*g_frameWidth*6] = g_rendertmp0[i*3+2+g_frameWidth*3+j*g_frameWidth*6];
			}
		ShowAll();
	}
	else
	{
		//if(g_frameMaxNum<=0)
			//messagebox("Please open a file first.");
	}
	return ;
}

void closeyuvfile()
{
	if(g_frameMaxNum>0)
	{
		//if(g_frameResizeFlag==1)
		{
			if(g_yuvorginput.y) delete [] g_yuvorginput.y;
			if(g_yuvorginput.u) delete [] g_yuvorginput.u;
			if(g_yuvorginput.v) delete [] g_yuvorginput.v;
			if(g_yuvorginputref.y) delete [] g_yuvorginputref.y;
			if(g_yuvorginputref.u) delete [] g_yuvorginputref.u;
			if(g_yuvorginputref.v) delete [] g_yuvorginputref.v;
			if(g_rgborginput) delete [] g_rgborginput;
			if(g_rgborginputref) delete [] g_rgborginputref;
			if(g_renderorgtmp0) delete [] g_renderorgtmp0;
			if(g_renderorgtmp1) delete [] g_renderorgtmp1;
		}
		if(g_yuvinput.y) delete [] g_yuvinput.y;
		if(g_yuvinput.u) delete [] g_yuvinput.u;
		if(g_yuvinput.v) delete [] g_yuvinput.v;
		if(g_rgbinput)	delete [] g_rgbinput;
		if(g_rgbinputref)	delete [] g_rgbinputref;
		if(g_rendertmp0) delete [] g_rendertmp0;
		if(g_rendertmp1) delete [] g_rendertmp1;
		if(g_yuvinputref.y) delete [] g_yuvinputref.y;
		if(g_yuvinputref.u) delete [] g_yuvinputref.u;
		if(g_yuvinputref.v) delete [] g_yuvinputref.v;
		if(g_autosegdisplay) delete [] g_autosegdisplay;
		if(g_manusegdisplay) delete [] g_manusegdisplay;

		if(g_OrgLabels0) delete [] g_OrgLabels0;
		if(g_SegLabels0) delete [] g_SegLabels0;
		if(g_RefOrgLabels0) delete [] g_RefOrgLabels0;
		if(g_RefSegLabels0) delete [] g_RefSegLabels0;
		if(g_OrgLabels) delete [] g_OrgLabels;
		if(g_SegLabels) delete [] g_SegLabels;
		if(g_RefOrgLabels) delete [] g_RefOrgLabels;
		if(g_RefSegLabels) delete [] g_RefSegLabels;
		if(g_SegDepths0) delete [] g_SegDepths0;
		if(g_RefSegDepths0) delete [] g_RefSegDepths0;
		if(g_SegDepths) delete [] g_SegDepths;
		if(g_RefSegDepths) delete [] g_RefSegDepths;

		if(g_Ref1OrgLabels0) delete [] g_Ref1OrgLabels0;
		if(g_Ref1SegLabels0) delete [] g_Ref1SegLabels0;
		if(g_Ref1OrgLabels) delete [] g_Ref1OrgLabels;
		if(g_Ref1SegLabels) delete [] g_Ref1SegLabels;
		if(g_Ref1SegDepths0) delete [] g_Ref1SegDepths0;
		if(g_Ref1SegDepths) delete [] g_Ref1SegDepths;
			
		#ifdef WIN32
		if(g_enlargedisplay) delete [] (g_enlargedisplay-g_frameWidth*g_frameHeight);
		if(g_enlargedisplaytmp0) delete [] (g_enlargedisplaytmp0-g_frameWidth*g_frameHeight);
		if(g_enlargedisplaytmp1) delete [] (g_enlargedisplaytmp1-g_frameWidth*g_frameHeight);
		#else
		if(g_enlargedisplay) delete [] g_enlargedisplay;
		if(g_enlargedisplaytmp0) delete [] g_enlargedisplaytmp0;
		if(g_enlargedisplaytmp1) delete [] g_enlargedisplaytmp1;
		#endif
		if(g_depthdisplay) delete [] g_depthdisplay;
		if(g_tmpdisplay) delete [] g_tmpdisplay;
		if(g_tmpdisplay1) delete [] g_tmpdisplay1;
		if(g_tmpdisplay2) delete [] g_tmpdisplay2;
		if(g_tmpdisplay3) delete [] g_tmpdisplay3;
		
		if(g_renderdisplay1) delete [] g_renderdisplay1;
		if(g_renderdisplay2) delete [] g_renderdisplay2;
		if(g_renderdisplay3) delete [] g_renderdisplay3;
		if(g_renderdisplay4) delete [] g_renderdisplay4;
		if(g_renderdisplay5) delete [] g_renderdisplay5;
		if(g_renderdisplay6) delete [] g_renderdisplay6;
		if(g_renderdisplay7) delete [] g_renderdisplay7;
		if(g_renderdisplay8) delete [] g_renderdisplay8;
		
		if(g_renderlabel1) delete [] g_renderlabel1;
		if(g_renderlabel2) delete [] g_renderlabel2;
		if(g_segmentedflags) delete [] g_segmentedflags;
		if(g_BaseDepth) delete [] g_BaseDepth;
		if(g_RefBaseDepth) delete [] g_RefBaseDepth;
		if(g_Ref1BaseDepth) delete [] g_Ref1BaseDepth;
		
		if(g_tmpOrgLabels0) delete [] g_tmpOrgLabels0;
		if(g_tmpSegLabels0) delete [] g_tmpSegLabels0;
		if(g_tmpSegDepths0) delete [] g_tmpSegDepths0;
		if(g_tmpOrgLabels) delete [] g_tmpOrgLabels;
		if(g_tmpSegLabels) delete [] g_tmpSegLabels;
		if(g_tmpSegDepths) delete [] g_tmpSegDepths;

		if(g_keyframenumarr) delete [] g_keyframenumarr;

		#if 0
		for(i=0;i<g_frameMaxNum;i++)
		{
			if(g_frames[i].OrgLabels0) delete [] g_frames[i].OrgLabels0;
			if(g_frames[i].SegLabels0) delete [] g_frames[i].SegLabels0;
			if(g_frames[i].SegDepths0) delete [] g_frames[i].SegDepths0;
			if(g_frames[i].OrgLabels) delete [] g_frames[i].OrgLabels;
			if(g_frames[i].SegLabels) delete [] g_frames[i].SegLabels;
			if(g_frames[i].SegDepths) delete [] g_frames[i].SegDepths;
		}
		#endif

		// 清除关键帧链表
		freekeyframe();
		g_frameMaxNum = 0;
	}

	//gtk_main_quit();

	return ;
}

void saveinfofile(FILE * finfo, int idx)
{
	int j;
	// 已设置分割或有深度信息的情况才保存，保存某一帧 
	if(g_frames[idx].SegFlag==1||g_frames[idx].DepFlag==1)
	{
		fwrite(H3D_SegmentFilePR, sizeof(H3D_SegmentFilePR), 1, finfo);
		fwrite(&idx, 4, 1, finfo); // frame index
		fwrite(&g_frameInWidth, 4, 1, finfo); // frame width
		fwrite(&g_frameInHeight, 4, 1, finfo); // frame height
		fwrite(&g_frames[idx].SegFlag, 4, 1, finfo);
		fwrite(&g_frames[idx].KeyFlag, 4, 1, finfo);
		fwrite(&g_frames[idx].AutoSegLabel, 4, 1, finfo);
		fwrite(&g_frames[idx].SegLabelMask, 4, 1, finfo);
		fwrite(&g_frames[idx].LayerVisibleNum, 4, 1, finfo);
		if(g_frameResizeFlag==1)
		{
			fwrite(g_OrgLabels0, 4, g_frameInWidth*g_frameInHeight,finfo);
			fwrite(g_SegLabels0, 4, g_frameInWidth*g_frameInHeight,finfo);
		}
		else
		{
			fwrite(g_OrgLabels, 4, g_frameWidth*g_frameHeight,finfo);
			fwrite(g_SegLabels, 4, g_frameWidth*g_frameHeight,finfo);
		}
		
		for(j=0;j<=g_frames[idx].LayerVisibleNum;j++)
		{
			fwrite(&g_frames[idx].layerdepth[j].setflag, 4, 1, finfo);
			fwrite(&g_frames[idx].layerdepth[j].depthmin, 4, 1, finfo);
			fwrite(&g_frames[idx].layerdepth[j].depthmax, 4, 1, finfo);
			fwrite(&g_frames[idx].layerdepth[j].depthmode, 4, 1, finfo);
		}

		fwrite(&g_frames[idx].DepFlag, 4, 1, finfo);
		fwrite(&g_frames[idx].LayerBaseNum, 4, 1, finfo);
		
		if(g_frameResizeFlag==1)
		{
			fwrite(g_SegDepths0, 4, g_frameInWidth*g_frameInHeight/4,finfo);
		}
		else
		{
			fwrite(g_SegDepths, 4, g_frameWidth*g_frameHeight/4,finfo);
		}

		fwrite(&g_frames[idx].BackgroundFrame, 4, 1, finfo);
		fwrite(&g_frames[idx].DepBaseFlag, 4, 1, finfo);
		fwrite(g_BaseDepth, 1, g_frameBaseWidth*g_frameBaseHeight, finfo);
		fwrite(&g_frames[idx].framedepth.setflag, 4, 1, finfo);
		fwrite(&g_frames[idx].framedepth.depthmin, 4, 1, finfo);
		fwrite(&g_frames[idx].framedepth.depthmax, 4, 1, finfo);
		fwrite(&g_frames[idx].framedepth.depthmode, 4, 1, finfo);
	}
	return ;
}

void loadinfofile(FILE * finfo, int idx, int refflag)
{
	char filepr[4];
	int width, height;
	int j;

	int * OrgLabels0, * SegLabels0 ,* OrgLabels,* SegLabels;
	uint8_t * SegDepths0 , * SegDepths ,* BaseDepth;

	if(refflag==0)
	{
		OrgLabels0 = g_OrgLabels0;
		SegLabels0 = g_SegLabels0;
		OrgLabels = g_OrgLabels;
		SegLabels = g_SegLabels;
		SegDepths0 = g_SegDepths0;
		SegDepths = g_SegDepths;
		BaseDepth = g_BaseDepth;
	}
	else if(refflag==1)
	{
		OrgLabels0 = g_RefOrgLabels0;
		SegLabels0 = g_RefSegLabels0;
		OrgLabels = g_RefOrgLabels;
		SegLabels = g_RefSegLabels;
		SegDepths0 = g_RefSegDepths0;
		SegDepths = g_RefSegDepths;
		BaseDepth = g_RefBaseDepth;
	}
	else // 多帧参考时使用
	{
		OrgLabels0 = g_Ref1OrgLabels0;
		SegLabels0 = g_Ref1SegLabels0;
		OrgLabels = g_Ref1OrgLabels;
		SegLabels = g_Ref1SegLabels;
		SegDepths0 = g_Ref1SegDepths0;
		SegDepths = g_Ref1SegDepths;
		BaseDepth = g_Ref1BaseDepth;
	}
	
	fread(filepr, sizeof(filepr), 1, finfo);
	if(filepr[0]!=H3D_SegmentFilePR[0]||filepr[1]!=H3D_SegmentFilePR[1]||filepr[2]!=H3D_SegmentFilePR[2]||filepr[3]!=H3D_SegmentFilePR[3])
	{
		if(refflag==0)
		{
			printf("FILE    ERROR --- Load information file format is wrong.\n");
			messagebox(MSGBOX_LIFFW);
		}
		else
		{
			printf("FILE    ERROR --- Load reference information file format is wrong.\n");
			messagebox(MSGBOX_LRIFFW);
		}
		return ;
	}
	fread(&idx, 4, 1, finfo); // frame index
	fread(&width, 4, 1, finfo); // frame width
	fread(&height, 4, 1, finfo); // frame height
	fread(&g_frames[idx].SegFlag, 4, 1, finfo);
	fread(&g_frames[idx].KeyFlag, 4, 1, finfo);
	fread(&g_frames[idx].AutoSegLabel, 4, 1, finfo);
	fread(&g_frames[idx].SegLabelMask, 4, 1, finfo);
	fread(&g_frames[idx].LayerVisibleNum, 4, 1, finfo);
	
	g_frames[idx].LayerEditNum = 0;
	
	if(g_frameResizeFlag==1)
	{
		fread(OrgLabels0, 4, width*height,finfo);
		fread(SegLabels0, 4, width*height,finfo);
		LabelResize(OrgLabels0,OrgLabels,g_yuvorginput.y, g_yuvinput.y, width,height,g_frameWidth,g_frameHeight);
		LabelResize(SegLabels0,SegLabels,g_yuvorginput.y, g_yuvinput.y, width,height,g_frameWidth,g_frameHeight);
		g_frames[idx].OrgLabels0 = OrgLabels0;
		g_frames[idx].SegLabels0 = SegLabels0;
		g_frames[idx].SegDepths0 = SegDepths0;
	}
	else
	{
		fread(OrgLabels, 4, width*height,finfo);
		fread(SegLabels, 4, width*height,finfo);
	}

	g_frames[idx].OrgLabels = OrgLabels;
	g_frames[idx].SegLabels = SegLabels;
	g_frames[idx].SegDepths = SegDepths;
	
	for(j=0;j<=g_frames[idx].LayerVisibleNum;j++)
	{
		fread(&g_frames[idx].layerdepth[j].setflag, 4, 1, finfo);
		fread(&g_frames[idx].layerdepth[j].depthmin, 4, 1, finfo);
		fread(&g_frames[idx].layerdepth[j].depthmax, 4, 1, finfo);
		fread(&g_frames[idx].layerdepth[j].depthmode, 4, 1, finfo);
		#if 0
		int lb = g_frameWidth*g_frameHeight + (1<<j);
		if(g_frames[idx].layerdepth[j].setflag)
		{
			if(g_frameResizeFlag==1)
			{
				set_depth_value0(idx, lb, g_frames[idx].layerdepth[j].depthmin, g_frames[idx].layerdepth[j].depthmax, g_frames[idx].layerdepth[j].depthmode);
			}
			else
				set_depth_value(idx, lb, g_frames[idx].layerdepth[j].depthmin, g_frames[idx].layerdepth[j].depthmax, g_frames[idx].layerdepth[j].depthmode);
		}
		#endif
	}

	fread(&g_frames[idx].DepFlag, 4, 1, finfo);
	fread(&g_frames[idx].LayerBaseNum, 4, 1, finfo);

	if(g_frameResizeFlag==1)
	{
		fread(SegDepths0, 4, width*height/4,finfo);
		yresize(SegDepths0,SegDepths,g_frameInWidth,g_frameInHeight,g_frameWidth,g_frameHeight);
	}
	else
	{
		fread(SegDepths, 4, width*height/4,finfo);
	}
	fread(&g_frames[idx].BackgroundFrame, 4, 1, finfo);
	fread(&g_frames[idx].DepBaseFlag, 4, 1, finfo);
	fread(BaseDepth, 1, g_frameBaseWidth*g_frameBaseHeight, finfo);
	fread(&g_frames[idx].framedepth.setflag, 4, 1, finfo);
	fread(&g_frames[idx].framedepth.depthmin, 4, 1, finfo);
	fread(&g_frames[idx].framedepth.depthmax, 4, 1, finfo);
	fread(&g_frames[idx].framedepth.depthmode, 4, 1, finfo);
	return ;
}

