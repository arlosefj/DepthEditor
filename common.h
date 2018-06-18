#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <assert.h>
#include <gtk/gtk.h>

#include "language.h"

#define USERGB

#define USEVS2010

#define I64

#define int64_t  __int64
#define uint64_t unsigned __int64

typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;

#define H3DVERSION "3.6.18"

#define LAYERBUTTONNUMMAX 30

#define DEPTHMODEMAX 20

#define INPYUV 0
#define INPIMG 1
#define INPVID 2

#define H3D_MIN(A,B)	((A)<(B)?(A):(B))
#define H3D_MAX(A,B)	((A)>(B)?(A):(B))
#define H3D_SWAP(A,B,C)	{(C)=(A);(A)=(B);(B)=(C);}

#define IMAGE_SIZE(x,y) ((x)*(y)*3/2)
#define IMAGERGB_SIZE(x,y) ((x)*(y)*3)

/* 图像操作区*/
#define IMGWIDTH	980
#define IMGHEIGHT	604
#define IMGORGX		214
#define IMGORGY		75

#define IMGCENTERX	(IMGORGX+IMGWIDTH/2) // 214+490
#define IMGCENTERY	(IMGORGY+IMGHEIGHT/2) // 75+302

// 支持图像大小最大480x288 
// 限制图像大小1.界面简单，2.速度快
#define IMGWIDTHMAX 	480
#define IMGHEIGHTMAX	288

#define IMGWIDTHDEF 	1280
#define IMGHEIGHTDEF	720

/* 选项参数设置*/
#define PARAMSETWINWIDTH	600
#define PARAMSETWINHEIGHT	740
#define PARAMSETFRMWIDTH	570
#define PARAMSETFRMHEIGHT	630


#define SEGPARAM_NUMREGIONMIN 		50
#define SEGPARAM_NUMREGIONMAX 		1000
#define SEGPARAM_NUMREGIONDEF 		500
#define SEGPARAM_WEIGHTSPADISMIN 	5
#define SEGPARAM_WEIGHTSPADISMAX 	40
#define SEGPARAM_WEIGHTSPADISDEF 	20
#define SEGPARAM_ITERATEMIN 		2
#define SEGPARAM_ITERATEMAX 		40
#define SEGPARAM_ITERATEDEF 		15

#define SEGPARAM_AUTOEDGETHMIN		1
#define SEGPARAM_AUTOEDGETHMAX		30
#define SEGPARAM_AUTOEDGETHDEF		10

#define SEGPARAM_EDGEENLARGEITERMIN 1
#define SEGPARAM_EDGEENLARGEITERMAX 10
#define SEGPARAM_EDGEENLARGEITERDEF 5

#define SEGPARAM_EDGEENLARGESIZEMIN 5
#define SEGPARAM_EDGEENLARGESIZEMAX 30
#define SEGPARAM_EDGEENLARGESIZEDEF 10

#define SEGPARAM_EDGEENLARGETHMIN	1
#define SEGPARAM_EDGEENLARGETHMAX	30
#define SEGPARAM_EDGEENLARGETHDEF	6

#define REFPARAM_STRENGTHMIN 		1
#define REFPARAM_STRENGTHMAX 		5
#define REFPARAM_STRENGTHDEF		3
#define REFPARAM_WEIGHTSPADISMIN 	5
#define REFPARAM_WEIGHTSPADISMAX 	20
#define REFPARAM_WEIGHTSPADISDEF 	10
#define REFPARAM_ITERATEMIN 		2
#define REFPARAM_ITERATEMAX 		20
#define REFPARAM_ITERATEDEF 		10

#define INFPARAM_STRENGTHMIN		1
#define INFPARAM_STRENGTHMAX		6
#define INFPARAM_STRENGTHDEF		3

#define RENPARAM_ALPHAMIN			1
#define RENPARAM_ALPHAMAX			50
#define RENPARAM_ALPHADEF			30

#define RENPARAM_FPMIN				0
#define RENPARAM_FPMAX				255
#define RENPARAM_FPDEF				127

#define SEGEDITPARAM_RADMIN			1
#define SEGEDITPARAM_RADMAX			30
#define SEGEDITPARAM_RADDEF			3

#define DEPEDITPARAM_RADMIN			1
#define DEPEDITPARAM_RADMAX			30
#define DEPEDITPARAM_RADDEF			10

#define DEPFILTPARAM_DIMIN			0
#define DEPFILTPARAM_DIMAX			5
#define DEPFILTPARAM_DIDEF			3

#define DEPFILTPARAM_GSMIN			1
#define DEPFILTPARAM_GSMAX			50
#define DEPFILTPARAM_GSDEF			30

#define DEPAUTOPARAM_RIMIN			4
#define DEPAUTOPARAM_RIMAX			64
#define DEPAUTOPARAM_RIDEF			32

#define RENEDITPARAM_SRADMIN		1
#define RENEDITPARAM_SRADMAX		15
#define RENEDITPARAM_SRADDEF		5

#define RENEDITPARAM_RRADMIN		10
#define RENEDITPARAM_RRADMAX		60
#define RENEDITPARAM_RRADDEF		30

#define RENPARAM_ESTHMIN			0
#define RENPARAM_ESTHMAX			255
#define RENPARAM_ESTHDEF			10

// yuv播放
#define YUVPLAYPAUSE				0
#define YUVPLAYBACKWARD				1
#define YUVPLAYFORWARD				2

/* 推理模式*/
#define INFMODEPIX					0
#define INFMODEREG					1

/* 推理方向*/
#define INFDIRPREV					0
#define INFDIRNEXT					1
#define INFDIRPREVKEY				2
#define INFDIRNEXTKEY				3
#define INFDIRUSEDEF				4

/*帧跳转模式*/
#define JMPMODEFIRST	0
#define JMPMODELAST		1
#define JMPMODECUR 		2 // 当前
#define JMPMODEPREV		3 // 前一帧
#define JMPMODENEXT		4 //	后一帧
#define JMPMODEPREVK	5 // 前一关键帧
#define JMPMODENEXTK	6 // 后一关键帧

/* 深度图处理*/
#define MIDFILTER 		0x01 // 中值滤波
#define MEANFILTER 		0x02 // 平均滤波
#define GAUSEFILTER 	0x04 // 高斯滤波
#define UNGAUSEFILTER	0x08 // 非对称高斯滤波
#define EDGEFILTER		0x10 // Edge 空域滤波
#define JBFILTER		0x20 // 联合滤波
#define UNJBFILTER		0x40 // 非对称联合滤波
#define TEMFILTER		0x80 // 时域滤波

#define DEPPROCESSMASK1	0x03
#define DEPPROCESSMASK2	0x7C
#define DEPPROCESSMASK3	0x80

/* Render 输出格式*/
#define RENDEROUTLR		0
#define RENDEROUTAN		1

/* 色差选择*/
#define RENDEROUTAN_RE_BL	0 // 红蓝
#define RENDEROUTAN_RE_GR	1 // 红绿
#define RENDEROUTAN_GR_RE	2 // 绿红
#define RENDEROUTAN_BR_BL	3 // 棕蓝

/* Render 处理方法*/
#define RENDERPROCNONE			0
#define RENDERPROCHOLLFILL		1
#define RENDERPROCINPAINTING	2
#define RENDERPROCHHF			3

/* Render 边缘柔化处理*/
#define RENDEREDGESOFTENNON		0
#define RENDEREDGESOFTENMEAN	1
#define RENDEREDGESOFTENMID		2

/* 分割编辑模板*/
#define SEGEDITMODULESQU	0 // 方形
#define SEGEDITMODULECIR	1 // 圆形

/* 自动分割显示颜色*/
#define SEGEDITCOLORBLACK	0x0
#define SEGEDITCOLORWHITE	0xFFFFFF
#define SEGEDITCOLORRED		0xFF0000
#define SEGEDITCOLORGREEN	0x00FF00
#define SEGEDITCOLORBLUE	0x0000FF

/* 深度编辑模式*/
#define DEPEDITMODEINCFLAT	0 // 深度增加
#define DEPEDITMODEINCGRAD	1 // 深度增加渐变
#define DEPEDITMODEDECFLAT	2 // 深度减少
#define DEPEDITMODEDECGRAD	3 // 深度减少渐变

/* 深度编辑模板*/
#define DEPEDITMODULESQU 0 // 方形
#define DEPEDITMODULECIR 1 // 圆形

/* render 编辑选择模板*/
#define RENEDITSELMODESQU 0 // 方形
#define RENEDITSELMODECIR 1 // 圆形

/* render 编辑匹配模板*/
#define RENEDITMATCHMOD7X7 0 // 7x7
#define RENEDITMATCHMOD9X9 1 // 9x9
#define RENEDITMATCHMODBXB 2 // 11x11

/* render 编辑搜索起点选择*/
#define RENEDITSELSPSTRU	0
#define RENEDITSELSPMENU	1

#define REGIONORG	1
#define REGIONSEG	2
#define REGIONMAN	3
#define REGIONDEP	4

#define RENDERLABEL 10

#define RENDERWARP	0
#define RENDERPULL	1
#define RENDERPUSH	2

//inpaint
#define SOURCE 0
#define TARGET -1
#define BOUNDARY -2
#define winsize 4  // the window size

// 主窗口大小
#define MAINWINWIDTH 1280
#define MAINWINHEIGHT 768

/* 菜单栏*/
#define MENUBARORGX	3
#define MENUBARORGY	0

/* 工具栏*/
#define TOOLBARORGX	3
#define TOOLBARORGY	26

/* 层次操作区*/
#define LAYERWIDTH	200
#define LAYERHEIGHT	350
#define LAYERORGX	5
#define LAYERORGY	70

#define LAYERBUTOFFSET	18
#define LAYERBUTOFFSETX	40
#define LAYERBUTOFFSETY	23

/*参数区*/
#define PRMWIDTH	200
#define PRMHEIGHT	303
#define PRMORGX		5
#define PRMORGY		418

#define PRMOFFSET	15
#define PRMOFFSETX	80
#define PRMOFFSETY	26

#define PRMENTRYW	40
#define PRMENTRYH	25

#define SEGEDITBUTTONX 30
#define SEGEDITBUTTONY 470

/*帧操作区*/
#define FRMWIDTH	540
#define FRMHEIGHT	48
#define FRMORGX		215
#define FRMORGY		673

#define FRMOFFSET	15
#define FRMOFFSETX	55
#define FRMOFFSETY	15


#define KFRMWIDTH	430
#define KFRMHEIGHT	48
#define KFRMORGX	765
#define KFRMORGY	673

#define KFRMOFFSET	15
#define KFRMOFFSETX	55
#define KFRMOFFSETY	15

#define FRMENTRYW	60
#define FRMENTRYH	25

/*信息显示区*/
#ifdef WIN32
#define INFOORGX	200
#define INFOORGY	724
#else
#define INFOORGX	300
#define INFOORGY	6
#endif

#define MAXWIDTH 2048
#define MINWIDTH 192
#define STAGEMAX 8
#define MAXFRAMENR 100

typedef struct
{
	double grad_x;
	double grad_y;
}gradient; //the structure that record the gradient

typedef struct
{
	double norm_x;
	double norm_y;
}norm;  // the structure that record the norm（法线）

const int colorset[LAYERBUTTONNUMMAX][3] = {
		{0, 0, 0},
		{255, 0, 0},
		{0, 255, 0},
		{0, 0, 255},
		{0, 255, 255},
		{255, 0, 255},
		{255, 255, 0},
		{255, 128, 0},
		{128, 255, 0},
		{128, 0, 255},
		{255, 0, 128},
		{0, 128, 255},
		{0, 255, 128},
		{255, 128, 128},
		{128, 255, 128},
		{128, 128, 255},
		{255, 255, 128},
		{255, 128, 255},
		{128, 255, 255},
		{0, 128, 128},
		{128, 0, 128},
		{128, 128, 0},
		{128, 0, 0},
		{0, 128, 0},
		{0, 0, 128},
		{0, 0, 0},
		{64, 64, 64},
		{128, 128, 128},
		{192, 192, 192},
		{255, 255, 255}
	};
const int colorset16[LAYERBUTTONNUMMAX][3] = {
		{0, 0, 0},
		{0xffff, 0, 0},
		{0, 0xffff, 0},
		{0, 0, 0xffff},
		{0, 0xffff, 0xffff},
		{0xffff, 0, 0xffff},
		{0xffff, 0xffff, 0},
		{0xffff, 0x7fff, 0},
		{0x7fff, 0xffff, 0},
		{0x7fff, 0, 0xffff},
		{0xffff, 0, 0x7fff},
		{0, 0x7fff, 0xffff},
		{0, 0xffff, 0x7fff},
		{0xffff, 0x7fff, 0x7fff},
		{0x7fff, 0xffff, 0x7fff},
		{0x7fff, 0x7fff, 0xffff},
		{0xffff, 0xffff, 0x7fff},
		{0xffff, 0x7fff, 0xffff},
		{0x7fff, 0xffff, 0xffff},
		{0, 0x7fff, 0x7fff},
		{0x7fff, 0, 0x7fff},
		{0x7fff, 0x7fff, 0},
		{0x7fff, 0, 0},
		{0, 0x7fff, 0},
		{0, 0, 0x7fff},
		{0, 0, 0},
		{0x3fff, 0x3fff, 0x3fff},
		{0x7fff, 0x7fff, 0x7fff},
		{0xbfff, 0xbfff, 0xbfff},
		{0xffff, 0xffff, 0xffff}
	};

typedef struct
{
	//GtkWidget * addbutton;
	GtkWidget * layeredit[LAYERBUTTONNUMMAX];
	GtkWidget * layer[LAYERBUTTONNUMMAX];
	GtkWidget * layerlabel[LAYERBUTTONNUMMAX];
	int layernum[LAYERBUTTONNUMMAX];
}SLayerButton;

typedef struct
{
	GtkWidget *	window;

	/* 分割参数*/
	GtkWidget * segment_numofregion;
	GtkWidget * segment_weightofspadis;
	GtkWidget * segment_numofiter;
	
	GtkWidget * segment_autoedgeth;

	GtkWidget * segment_edgeenlargeiter;
	GtkWidget * segment_edgeenlargesize;
	GtkWidget * segment_edgeenlargeth;
	#if 0
	/* 求精参数*/
	GtkWidget * refine_strength;
	GtkWidget * refine_weightofspadis;
	GtkWidget * refine_numofiter;
	#endif
	/* 推理参数*/
	GtkWidget * inference_strength;
	/* Render 参数*/
	GtkWidget * render_alpha;
	GtkWidget * render_focalplane;
	GtkWidget * render_edgesoftenth;
	// TODO
	
}SParameterInp;

typedef struct 
{
	uint8_t *y;
	uint8_t *u;
	uint8_t *v;
	
}SYuvInput;

/* 新建工程时弹出窗口结构体*/
typedef struct
{
	GtkWidget *entrypname;
    GtkWidget *entryinputfile;
	GtkWidget *entryinwidth;
	GtkWidget *entryinheight;
	GtkWidget *entryintimestart;
	GtkWidget *entryintimeend;
	GtkWidget *entryinsectionduration;	
	GtkWidget *entryinkeyframeth;
	GtkWidget *entryinfofile;
	GtkWidget *entryoutputfile1;
	GtkWidget *entryoutputfile2;
	//GtkWidget *entryh264encparam;
}SCreateProject;


/* 设置depth 时弹出窗口结构体*/
typedef struct
{
    GtkWidget *window;
    GtkWidget *entrymax;
	GtkWidget *entrymin;
	GtkWidget *mode;
	int label;
}SDepthSet;

/* 选择推理参考帧时弹出窗口结构体*/
typedef struct
{
    GtkWidget *window;
	GtkWidget *entryfirst;
	GtkWidget *entrylast;
    GtkWidget *entryindex;
}SRefFrameSelect;

/* 帧号选择处理时弹出窗口结构体*/
typedef struct
{
    GtkWidget *window;
	GtkWidget *entryfirst;
	GtkWidget *entrylast;
}SFrameSelect;

/* 基本层设置时弹出窗口结构体*/
typedef struct
{
    GtkWidget *window;
	GtkWidget *entryfirst;
	GtkWidget *entrylast;
	GtkWidget *entrybase;
}SBaseLayerSelect;

/*基本深度设置时弹出窗口结构体*/
typedef struct
{
    GtkWidget *window;
	GtkWidget *entryfirst;
	GtkWidget *entrylast;
	GtkWidget *entrydepmin;
	GtkWidget *entrydepmax;
	GtkWidget *entrydepmod;
}SBaseDepthSelect;

/* 层次深度推理*/
typedef struct
{
    GtkWidget *window;
	GtkWidget *entryfirst;
	GtkWidget *entrylast;
	GtkWidget *entrylayernum;
	GtkWidget *entryfirstdmin;
	GtkWidget *entrylastdmin;
	GtkWidget *entryfirstdmax;
	GtkWidget *entrylastdmax;
	GtkWidget *entrydepthmode;
	
}SLayerDepthInfe;

/* 自动深度推理*/
typedef struct
{
    GtkWidget *window;
	GtkWidget *entryfirst;
	GtkWidget *entrylast;
	GtkWidget *entryreffirst;
	GtkWidget *entryreflast;
	GtkWidget *entryinfemode;
	
}SAutoDepthInfe;


typedef struct
{
	int framenum;
	int jumpmode;
}SFrameJump;

typedef struct SKeyFrameList
{
	int index;
	SKeyFrameList * prev;
	SKeyFrameList * next;
}SKeyFrameList;

typedef struct
{
	int setflag;
	int depthmin;
	int depthmax;
	int depthmode;
	
}SLayerDepth;

typedef struct
{	
	int KeyFlag; // 该帧是否为关键帧
	int SegFlag; //该帧是否已分割
	int DepFlag; // 该帧是否已有深度图
	int DepBaseFlag; // 该帧是否已有基本深度图
	//int RenderFlag;// 该帧是否已render
	//int SegLabel; // 分割标号
	int ChangeFlag; // 是否已有改动
	int SegLabelMask; // 分割标号掩膜
	int AutoSegLabel; // slic分割得到的块数
	/* 返回未精细前的分割所用， 多个返回*/
	//int RefineSegLabel; // refine slic分割的标号
	//int RefineSegNum; // refine slic分割得到的块数
	FILE * finfo;// 分割信息保存文件指针
	
	int LayerVisibleNum;
	int LayerEditNum;
	int LayerBaseNum;
	int BackgroundFrame;

	int* OrgLabels0;
	int* SegLabels0;
	uint8_t* SegDepths0;
	
	int* OrgLabels;
	int* SegLabels;
	uint8_t* SegDepths;

	uint8_t* rgbinput;

	SLayerDepth framedepth;

	SLayerDepth layerdepth[LAYERBUTTONNUMMAX]; // 每个层次的深度信息

}SFrameInfo;

typedef struct
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
}RGB;

enum H3D_RESULT
{
    H3D_RESULT_OK  = 0,
    H3D_RESULT_FAIL  = 1
};

typedef struct
{
	unsigned char * prev_data; // 前一帧数据
	unsigned char * cur_data;  // 当前帧数据
	unsigned char * next_data; // 后一帧数据，暂时不用
	int width;
	int height;
}H3D_YUV;

typedef struct
{
	unsigned short * cur_data[STAGEMAX]; // 当前计算得到的深度图
	unsigned short * prev_data; // 前一帧深度图
	unsigned short * next_data; // 后一帧深度图，暂时不用
	unsigned char * motionacc_data; // 深度累积图
	unsigned short * manual_data; // 人工标记深度图
	int width;
	int height;
}H3D_DEPTH;

typedef struct
{
	int Flag_sc;
	int Flag_ManualDepth;
}H3D_STATS;

typedef struct
{
	// 运动估计粒度0:8x8 1:4x4 2:2x2
	int Method_MotionBlockSize;
	
	// 图像下采样方法0:
	int Method_ImgDownscale;
	
	// 深度图上采样方法0:
	int Method_DepthUpscale;
	
	// 深度图后处理方法0:
	int Method_DepthPostprocess;
	
	// Render方法0: PULL
	int Method_Render;

	// 输出图格式0:leftright  1:anaglyph
	int Format_Out;
	
	// 初始化模板标号0:
	int InitialDepthTemplateIndex;

	// 文字区域突出开关0: OFF 1: ON
	int Flag_TextRegionHighlight;
	
	// 运动权重0-1 default: 0.6
	double MotionWeight;

	// 人工权重0-1 default: 0.8
	double ManualWeight;

	

	
}H3D_PARAM;

typedef struct
{
	int mvx;
	int mvy;
}H3D_MOTIONVECTOR;

typedef struct
{
	GtkWidget * processbar;
	int * flag;
	double * val;
}SProcessBar;

extern int g_setsegframesbarflag;
extern double g_setsegframesbarval;

extern SProcessBar g_Processbar;
extern GtkWidget * g_status_bar;
extern gint g_context_id;

extern int Flag_sc;
extern FILE * out_motiondepth;

extern FILE * g_projectSaveInfo;
extern FILE * g_projectSaveRender;

extern int g_changeflag;
extern int g_playflag;

extern SLayerButton layerbutton;

extern SFrameInfo *g_frames;

extern int g_inputfileformat;

extern SYuvInput g_yuvorginput;
extern SYuvInput g_yuvinput;
extern uint8_t * g_rgbinput;
extern uint8_t * g_rgbinputref;
extern uint8_t * g_rgborginputref;
extern uint8_t * g_rgborginput;
extern SYuvInput g_yuvinputref;
extern SYuvInput g_yuvorginputref;

extern uint8_t * g_rendertmp0;
extern uint8_t * g_rendertmp1;
extern uint8_t * g_renderorgtmp0;
extern uint8_t * g_renderorgtmp1;

extern uint8_t * g_autosegdisplay;
extern uint8_t * g_manusegdisplay;
extern uint8_t * g_enlargedisplay;
extern uint8_t * g_enlargedisplaytmp0;
extern uint8_t * g_enlargedisplaytmp1;

extern uint8_t * g_depthdisplay;
extern uint8_t * g_tmpdisplay;
extern uint8_t * g_tmpdisplay1;
extern uint8_t * g_tmpdisplay2;
extern uint8_t * g_tmpdisplay3;

extern uint8_t * g_renderdisplay1;
extern uint8_t * g_renderdisplay2;
extern uint8_t * g_renderdisplay3;
extern uint8_t * g_renderdisplay4;
extern uint8_t * g_renderdisplay5;
extern uint8_t * g_renderdisplay6;
extern uint8_t * g_renderdisplay7;
extern uint8_t * g_renderdisplay8;

extern int* g_OrgLabels0;
extern int* g_SegLabels0;
extern uint8_t* g_SegDepths0;

extern int* g_OrgLabels;
extern int* g_SegLabels;
extern uint8_t* g_SegDepths;

extern int* g_RefOrgLabels0;
extern int* g_RefSegLabels0;
extern uint8_t* g_RefSegDepths0;

extern int* g_RefOrgLabels;
extern int* g_RefSegLabels;
extern uint8_t* g_RefSegDepths;

extern int* g_Ref1OrgLabels0;
extern int* g_Ref1SegLabels0;
extern uint8_t* g_Ref1SegDepths0;

extern int* g_Ref1OrgLabels;
extern int* g_Ref1SegLabels;
extern uint8_t* g_Ref1SegDepths;

extern uint8_t *g_RefBaseDepth;
extern uint8_t *g_Ref1BaseDepth;


extern int g_tmpframeIndex;
extern int * g_tmpOrgLabels0;
extern int * g_tmpSegLabels0;
extern uint8_t * g_tmpSegDepths0;
extern int * g_tmpOrgLabels;
extern int * g_tmpSegLabels;
extern uint8_t * g_tmpSegDepths;

extern int g_regionr[SEGPARAM_NUMREGIONMAX+15];
extern int g_regiong[SEGPARAM_NUMREGIONMAX+15];
extern int g_regionb[SEGPARAM_NUMREGIONMAX+15];
extern int g_regionc[SEGPARAM_NUMREGIONMAX+15];

extern uint8_t * g_renderlabel1;
extern uint8_t * g_renderlabel2;

//extern int g_histdiffthreshold;

extern GtkWidget *g_wdgImgProcArea;
extern GtkWidget *g_wdgKeyFrameDrawArea;

//extern GtkWidget *g_wdgInpFormat0;
//extern GtkWidget *g_wdgInpFormat1;


extern GtkWidget *g_wdgFileOpenSelection;		// 打开文件选择
extern GtkWidget *g_wdgFileLoadRenSelection;	// 载入分割文件选择
extern GtkWidget *g_wdgFileSaveSeqCutSelection;	// 保存序列切割文件选择

extern GtkWidget *g_wdgMainInterface;			// 主界面
extern GtkWidget *g_wdgFrameNum;				// 帧序号label
extern GtkWidget *g_wdgKeyFrameNum;			// 关键帧序号label
extern GtkWidget *g_wdgInpFrameNum;			// 帧序号entry

extern GtkWidget *g_wdgDepProcnone0;
extern GtkWidget *g_wdgDepProcmedian;
extern GtkWidget *g_wdgDepProcmean;
extern GtkWidget *g_wdgDepProcnone1;
extern GtkWidget *g_wdgDepProcgaussion;
extern GtkWidget *g_wdgDepProcugaussion;
extern GtkWidget *g_wdgDepProcjbf;
extern GtkWidget *g_wdgDepProcujbf;
extern GtkWidget *g_wdgDepProcedge;
extern GtkWidget *g_wdgDepProcnone2;
extern GtkWidget *g_wdgDepProctimef;

extern GtkWidget *g_wdgRenderOutLR;
extern GtkWidget *g_wdgRenderOutAnag;

extern GtkWidget *g_wdgRenderOutAnRedBlue;
extern GtkWidget *g_wdgRenderOutAnRedGreen;
extern GtkWidget *g_wdgRenderOutAnGreenRed;
extern GtkWidget *g_wdgRenderOutAnBrownBlue;

extern GtkWidget *g_wdgRenderMethodWarp;
extern GtkWidget *g_wdgRenderMethodS2D;
extern GtkWidget *g_wdgRenderMethodD2S;

extern GtkWidget *g_wdgRenderProcNone;
extern GtkWidget *g_wdgRenderProcHollFill;
extern GtkWidget *g_wdgRenderProcInpainting;
extern GtkWidget *g_wdgRenderProcHHF;

extern GtkWidget *g_wdgNonEdit;
extern GtkWidget *g_wdgSegEdit;
extern GtkWidget *g_wdgMatEdit;
extern GtkWidget *g_wdgDepEdit;
extern GtkWidget *g_wdgRenEdit;

extern GtkWidget *g_wdgSegEditModule0;
extern GtkWidget *g_wdgSegEditModule1;
extern GtkWidget *g_wdgSegEditModuleRad;

extern GtkWidget *g_wdgSegEditColor0;
extern GtkWidget *g_wdgSegEditColor1;
extern GtkWidget *g_wdgSegEditColor2;
extern GtkWidget *g_wdgSegEditColor3;
extern GtkWidget *g_wdgSegEditColor4;

extern GtkWidget *g_wdgInfMode0;
extern GtkWidget *g_wdgInfMode1;

extern GtkWidget *g_wdgDepEditMode0;
extern GtkWidget *g_wdgDepEditMode1;
extern GtkWidget *g_wdgDepEditMode2;
extern GtkWidget *g_wdgDepEditMode3;

extern GtkWidget *g_wdgDepEditModule0;
extern GtkWidget *g_wdgDepEditModule1;
extern GtkWidget *g_wdgDepEditModuleRad;

extern GtkWidget *g_wdgDepFiltDilateIter;
extern GtkWidget *g_wdgDepFiltGauseSigmaX;
extern GtkWidget *g_wdgDepFiltGauseSigmaY;

extern GtkWidget *g_wdgSegmentMode;
extern GtkWidget *g_wdgRenderMode;

extern GtkWidget *g_wdgRenderEdgeSoftenNone;
extern GtkWidget *g_wdgRenderEdgeSoftenMean;
extern GtkWidget *g_wdgRenderEdgeSoftenMedian;

extern GtkWidget *g_wdgRenEditModule0;
extern GtkWidget *g_wdgRenEditModule1;
extern GtkWidget *g_wdgRenEditStartP0;
extern GtkWidget *g_wdgRenEditStartP1;

extern GtkWidget *g_wdgRenEditModuleRad;
extern GtkWidget *g_wdgRenEditRefRegionRad;
extern GtkWidget *g_wdgRenEditMatchModule0;
extern GtkWidget *g_wdgRenEditMatchModule1;
extern GtkWidget *g_wdgRenEditMatchModule2;

extern GtkWidget *g_wdgShowDepY;
extern GtkWidget *g_wdgShowDepN;
extern GtkWidget *g_wdgShowSegY;
extern GtkWidget *g_wdgShowSegN;

extern GtkWidget *g_wdgEnAutoDepOW;
extern GtkWidget *g_wdgDisAutoDepOW;
extern GtkWidget *g_wdgEnAutoDepAM;
extern GtkWidget *g_wdgDisAutoDepAM;


extern GtkWidget *g_wdgEditCircle;
extern GtkWidget *g_wdgEditSquare;
extern GtkWidget *g_wdgEditRadius;
extern GtkWidget *g_wdgEditStrength;
extern GtkWidget *g_wdgEditalpha;
extern GtkWidget *g_wdgEditfocal;

extern GtkWidget *g_wdgInfo;

extern FILE * g_fileInpYUV;
extern FILE * g_fileSaveDep;
extern FILE * g_fileSaveSeg;
extern FILE * g_fileLoadSeg;
extern FILE * g_fileLoadSegDep;
extern FILE * g_fileSaveRender;
extern FILE * g_fileSaveSeqcut;

extern int g_frameIndex;
extern int g_frameMaxNum;
extern int g_frameInWidth;
extern int g_frameInHeight;
extern int g_frameWidth;
extern int g_frameHeight;
extern int g_frameBaseWidth;
extern int g_frameBaseHeight;
extern int g_frameRenderRef;
extern int g_frameResizeFlag;

extern int g_segparamnumregion;
extern int g_segparamweightspadis;
extern int g_segparamiterate;
//extern int g_refparamstrength;
//extern int g_refparamweightspadis;
//extern int g_refparamiterate;

extern int g_segparamautoedgeth;
extern int g_segparamedgeenlargeiter;
extern int g_segparamedgeenlargesize;
extern int g_segparamedgeenlargeth;

extern SKeyFrameList g_keyframelist;

extern int g_inferencemode;
extern int g_inferencedirect;
extern int g_inferencestrength;

extern int g_renderalpha;
extern int g_renderforcalplane;
extern int g_renderflag;
extern int g_renderdisplayflag;

extern int g_depthprocessparam;
extern int g_renderoutputformat;
extern int g_renderprocmethod;
extern int g_renderoutputantype;

extern int g_enlargeflag;
extern int g_enlargeorgx;
extern int g_enlargeorgy;

extern int g_enlargeswitchflag;

extern int g_segeditflag;
extern int g_segeditmodule;
extern int g_segeditmodulerad;

extern int g_depeditflag;
extern int g_depeditmode;
extern int g_depeditmodule;
extern int g_depeditmodulerad;
extern int g_depfiltdilateiter;
extern int g_depfiltgausesigmax;
extern int g_depfiltgausesigmay;

extern int g_depautorefineiter;


extern int g_mateditflag;
extern int g_matselflag;
extern int g_mateditx;
extern int g_matedity;

extern int g_rendereditflag;
extern int g_renderrefflag;
extern int g_rendermethod;
extern int g_rendereditselmodule;
extern int g_rendereditstartpoint;
extern int g_rendereditselmodulerad;
extern int g_rendereditrefregionrad;
extern int g_rendereditmatchmodule;

extern int g_renderedgesoftenth;
extern int g_renderedgesoftenmethod;

extern int g_videoinfoflag;
extern int g_keyframeflag;
extern int g_keyframeth;
extern int * g_keyframenumarr;

extern int g_timestart;
extern int g_timeend;

extern int g_timetotal;
extern int g_sectioncur;
extern int g_sectionduration;
extern int g_sectionnum;
extern int g_sectionflag;

extern int g_isprojectopen;

extern uint8_t * g_segmentedflags;
extern uint8_t *g_BaseDepth;

extern int g_showalldepflag;
extern int g_showautosegflag;
extern int g_depoverwritten;
extern int g_depautoaddmotion;

extern int g_autosegcolor;

const char H3D_DepthFilePR[4] = {'D', 'E', 'P', ' '};
const char H3D_SegmentFilePR[4] = {'S', 'E', 'G', ' '};

static inline int H3D_MEDIAN( int a, int b, int c )
{
    int min = a, max =a;
    if( b < min )
        min = b;
    else
        max = b;    /* no need to do 'b > max' (more consuming than always doing affectation) */

    if( c < min )
        min = c;
    else if( c > max )
        max = c;

    return a + b + c - min - max;
}

static inline int H3D_CLIP3( int v, int i_min, int i_max )
{
    return ( (v < i_min) ? i_min : (v > i_max) ? i_max : v );
}

// gui main
GtkWidget* create_file_item();
GtkWidget* create_edit_item();
GtkWidget* create_segment_item();
GtkWidget* create_depth_item();
GtkWidget* create_render_item();
GtkWidget* create_help_item();
extern void statusbar_thread(char *buff);
void nonedit(GtkWidget * button,GtkWidget * data);
void setnorendermode(GtkWidget * button,GtkWidget * data);
void setrendermode(GtkWidget * button,GtkWidget * data);
void setshowdepy(GtkWidget * button,GtkWidget * data);
void setshowdepn(GtkWidget * button,GtkWidget * data);
void setshowautosegy(GtkWidget * button,GtkWidget * data);
void setshowautosegn(GtkWidget * button,GtkWidget * data);
void setenautodepoverwrite(GtkWidget * button,GtkWidget * data);
void setdisautodepoverwrite(GtkWidget * button,GtkWidget * data);
void setenautodepaddmotion(GtkWidget * button,GtkWidget * data);
void setdisautodepaddmotion(GtkWidget * button,GtkWidget * data);
void setcircle(GtkWidget * button,GtkWidget * data);
void setsquare(GtkWidget * button,GtkWidget * data);
void radiusminus(GtkWidget * button,GtkWidget * data);
void radiusplus(GtkWidget * button,GtkWidget * data);
void radiusset(GtkWidget * button,GtkWidget * data);
void radiusminusstrength(GtkWidget * button,GtkWidget * data);
void radiusplusstrength(GtkWidget * button,GtkWidget * data);
void radiussetstrength(GtkWidget * button,GtkWidget * data);
void quicksetalpha(GtkWidget * button,GtkWidget * data);
void quicksetfocal(GtkWidget * button,GtkWidget * data);
void parametersetting(GtkWidget * button,GtkWidget * data);
gint expose_event (GtkWidget *widget, GdkEventExpose *event);
void delete_event_fun();
void backwardplay(GtkWidget * button,GtkWidget * data);
void forwardplay(GtkWidget * button,GtkWidget * data);
bool scribble_button_press_event (GtkWidget *widget, GdkEventButton *event, gpointer data);
bool scribble_motion_notify_event (GtkWidget *widget, GdkEventMotion *event, gpointer data);
gboolean keypress_event (GtkWidget *widget,GdkEventKey *event,gpointer user_data);
gboolean keyrelease_event(GtkWidget * widget, GdkEventKey *event, gpointer data);
gboolean time_handler(gpointer widget);
void cancel_window(GtkWidget *button,GtkWidget *window);

//colorspace
void colorspace_init(void);
void yuv2rgb(uint8_t * srcy, uint8_t * srcu, uint8_t * srcv, uint8_t * dst, int width, int height);
void rgb2yuv(uint8_t * src, uint8_t * dsty, uint8_t * dstu, uint8_t * dstv, int width, int height);
void rgb2yuv_pixel(uint8_t *rgb, uint8_t * yuv);
void yuv2rgb_pixel(uint8_t *yuv, uint8_t * rgb);

//depthassignment
void Drawdepth(int * labels, int label);
void set_depth_value(int idx,int lb, int min, int max, int mode, int flag);
void get_depth_value(GtkWidget* button,SDepthSet *wdv);
void cancel_depth(GtkWidget *button,GtkWidget *window);
void get_depth_default_value(int lb, int * min, int *max, int *avg);
void Setdepth(int lb);
void depthbaseassign(int first, int last);
void depthframemodule(int first, int last, int min, int max, int mode);
void depthautoassign();
int depthautogen(int first, int last);
void depthimginfe(int srcidx, int dstidx);
void depthlayerinfe(int layernum, int first, int last, int firstmin, int firstmax, int lastmin, int lastmax, int depthmode);
void ShowDep(int mask);
void ShowEnlargeDep(int mask);
void getframemoduleframe(GtkWidget * button,GtkWidget * data);
void getbaseassignframe(GtkWidget * button,GtkWidget * data);
void getdepautogenframe(GtkWidget * button,GtkWidget * data);
void depthimginfeauto(int first, int last, int ref0idx, int ref1idx, int mode);


//file
void OpenInputFile(char *filename);
void OpenFrameInfoFile(char * filename);

void closeyuvfile();
//void savesegmentframe();
void helpfile(GtkWidget * button,GtkWidget * data);
void sequencecut(GtkWidget * button,GtkWidget * data);
void saveinfofile(FILE * finfo, int idx);
void loadinfofile(FILE * finfo, int idx, int refflag);
void swchkeyframemode(GtkWidget * button,GtkWidget * data);
void selsection(GtkWidget * button,GtkWidget * data);

//frame
void jumpframe(GtkWidget * button,SFrameJump * data);
void firstframe(GtkWidget * button,GtkWidget * data);
void lastframe(GtkWidget * button,GtkWidget * data);
void previousframe(GtkWidget * button,GtkWidget * data);
void nextframe(GtkWidget * button,GtkWidget * data);
void previouskeyframe(GtkWidget * button,GtkWidget * data);
void nextkeyframe(GtkWidget * button,GtkWidget * data);
void previousskeyframe(GtkWidget * button,GtkWidget * data);
void nextskeyframe(GtkWidget * button,GtkWidget * data);
void gotoframe(GtkWidget * button,GtkWidget * data);
void getrefframergb(SFrameJump data);
void GetKeyFrames();
void histogram(uint8_t *y, int * hist, int width, int height);
int histogramdiff(int *hist0, int *hist1);
void histogramcopy(int *hist0, int *hist1);
void insertkeyframe(int index);
void deletekeyframe(int index);
void yuvresize(SYuvInput in, SYuvInput out, int inw, int inh, int outw, int outh);
void yuvresize1(uint8_t* in, uint8_t* out, int inw, int inh, int outw, int outh);
void yresize(uint8_t* in, uint8_t* out, int inw, int inh, int outw, int outh);

SKeyFrameList * getcurkeyframe(int index);
void AutoKeyFrameScan(GtkWidget * button,GtkWidget * data);
void SetKeyframe();
void SetNonKeyframe();
void freekeyframe();
void ShowAll();


// inference
void getrefframe(GtkWidget * button,GtkWidget * data);
void inferenceframe(int refframenum);
void setinfmode0();
void setinfmode1();


// depthprocess
void setnone0();
void setmedian();
void setmean();
void setnone1();
void setgaussion();
void setugaussion();
void setedge();
void setjbf();
void setujbf();
void setnone2();
void settimef();
void setdemode0();
void setdemode1();
void setdemode2();
void setdemode3();
void setdemodule0();
void setdemodule1();
void DepthProcess(uint8_t * in_buffer, uint8_t * in_prev_depth_buffer,
                      uint8_t * in_depth_buffer, uint8_t * out_buffer, 
                      int width, int height, int isKey, uint8_t param);
void depthimgproc(int first, int last);
void getdepprocframe(GtkWidget * button,GtkWidget * data);
void getdepinfeframe(GtkWidget * button,GtkWidget * data);
void getlayerdepinfeframe(GtkWidget * button,GtkWidget * data);
void getautodepinfeframe(GtkWidget * button,GtkWidget * data);
void depedit(GtkWidget * button,GtkWidget * data);
void DepthFineEdit(uint8_t * Depths, int * labels, int lb, int rad, int x, int y, int width, int height);

// render
void setleftright();
void setanaglyph();
void setredblue();
void setredgreen();
void setgreenred();
void setbrownblue();
void setwarp();
void sets2d();
void setd2s();
void sethhf();
void setnone();
void sethollfill();
void setinpainting();
void setresnone();
void setresmean();
void setresmedian();
void DepthRendering(uint8_t * in_buffer, uint8_t * out_buffer, int inwidth, int inheight, int format);
//void renderframes(int idx, uint8_t * out_buffer);
void rendercurframe();
void loadrendercurframe();
void cancelrenderfineedit();
void getrenderframes(GtkWidget * button,GtkWidget * data);
void renderframes2h264(GtkWidget * button,GtkWidget * data);
void autotranscode(GtkWidget * button,GtkWidget * data);
void renedit(GtkWidget * button,GtkWidget * data);
void setremodule0();
void setremodule1();
void setremode0();
void setremode1();
void setremode2();
void setrestartp0();
void setrestartp1();
void RenderEditSelRegion(int x, int y, int region);
void DrawRenEditContour(int x, int y, int region);
void RenderBlockMatching(int x, int y, int region);
void Render3DImage( const uint8_t * inSource, const uint8_t * inDepth,
	uint8_t * outRender, int inSceneDepth_8bit, int inFocalZ, int w, int h);

// inpaint
void Convert2Gray(double *r, double *g, double *b, double *gray, int width, int height);
void DrawBoundary(uint8_t *label, int * m_mark, double * m_confid, int * m_left, int * m_right, int * m_bottom, int *m_top, int m_width, int m_height);
double priority(int i, int j, double *m_confid, int *m_mark, double * m_gray,int m_width, int m_height);
double ComputeConfidence(int i, int j, double *m_confid, int m_width, int m_height);
double ComputeData(int i, int j, int *m_mark, double * m_gray, int m_width, int m_height);
gradient GetGradient(int i, int j, double *m_gray, int m_width, int m_height);
norm GetNorm(int i, int j, int *m_mark, int m_width, int m_height);
bool draw_source(bool *m_source, int *m_mark, int m_width, int m_height);
bool PatchTexture(int x, int y, int *patch_x, int *patch_y, int *m_mark, bool * m_source, double* m_r, double *m_g, double *m_b, int m_width, int m_height);
bool update(int target_x, int target_y, int source_x, int source_y, double confid, uint8_t * label, int *m_mark, 
				double *m_gray, double *m_confid, double *m_r, double *m_g, double *m_b, int m_width, int m_height);
bool TargetExist(int *m_mark, int m_top, int m_bottom, int m_left, int m_right, int m_width);
void UpdateBoundary(int i, int j, uint8_t *label, int * m_mark, int m_width, int m_height);// just update the area near the changed patch. (+-2 pixels)
void UpdatePri(int i, int j, int *m_mark, double *m_pri, double * m_confid, double * m_gray, int m_width, int m_height); // just update the area near the changed patch. (+-3 pixels)
void inpaintrgbimg(uint8_t * srcr, uint8_t * srcg, uint8_t * srcb, int width, int height);


//layer
//void add_layer(GtkWidget * button,GtkWidget * data);
void all_layer(GtkWidget * button,GtkWidget * data);
void draw_layer(GtkWidget * button, GtkWidget * data);
void seteditlayer(GtkWidget * button, GtkWidget * data);
void CombineVisibleLayer();
void setremainlayer();
void setbaselayer();
void getbaselayers(GtkWidget * button,GtkWidget * data);


//segmentation
void SegmentImg(GtkWidget *button,GtkWidget *window);
void SegmentImgNoShow(int idx);
void SegmentWarning();
void getsegmentframe(GtkWidget * button,GtkWidget * data);
int segmentframes(int first, int last);
int SegmentCheck();
void CancelRefine();
void AutoRegionSeg(int *labels, int *oldlabels, uint8_t * rgbinp, int width, int height, int number);
void Automenuseg();
void savesegmenttmp();
void loadsegmenttmp();
void loadsegmenttmp1();
void ClearEdgeLabels(int * Labels, int *oldlabels, int width, int height, int edge);
void Setlabel(int * labels, int oldlabel, int newlabel, int sz);
void SetOldlabel(int * labels, int * oldlabels, int oldlabel, int newlabel, int sz);
void SetOldlabelMask(int * labels, int * oldlabels, int mask, int x, int y, int width, int height);
void SetOldlabelMask0(int * labels0, int * oldlabels0, int * labels, int * oldlabels, int mask, int x, int y);
void SetOldlabelRegion(int * labels, int * oldlabels, int mask, int x, int y);
void Drawlabel(int * labels, int label);
void ShowSegandDep(int mask);
void segedit(GtkWidget * button,GtkWidget * data);
void matedit(GtkWidget * button,GtkWidget * data);
void SegEditLabel(int * labels, int newlabel, int x, int y);
void setsemodule0();
void setsemodule1();
void setsecolor0();
void setsecolor1();
void setsecolor2();
void setsecolor3();
void setsecolor4();
void CalcRegionAvgRgb(int * label, uint8_t *inp,int w, int h, int number);
void DrawSegEditContour(int x, int y, int ex, int ey, int region);
void DrawDepEditContour(int x, int y, int ex, int ey, int region);
void SegEdgeEnlarge(int * labels,int label, int x, int y);
void SegEnlargedisplay(int mask);
void SegEdgeEnlarge0(int * labels,int label, int x, int y);
void SegEnlargedisplay0(int mask);
void SetOldlabelRegion0(int * labels, int * oldlabels, int mask, int x, int y);
void SegEditLabel0(int * labels, int newlabel, int x, int y);
void SetMatSelxy(int x, int y);
void SegMattingLabel(int * labels, int * oldlabels, int x, int y, int width, int height);
void SegMattingLabel0(int * labels, int * oldlabels, int x, int y, int width, int height);
void LabelResize(int * srclabel, int * dstlabel, uint8_t *srcinp, uint8_t *dstinp, int srcw, int srch, int dstw, int dsth);
void DrawEnlargelabel(int * labels, int label);
void getbackgroundframe(GtkWidget * button,GtkWidget * data);
void backgroundgeneration(int first, int last);

// image process
void InterpImageBilinear(unsigned char* src, unsigned char* dst, int src_width, int src_height, int dst_width, int dst_height);
void InterpYuvBilinear(unsigned char* src, unsigned char* dst, int src_width, int src_height, int dst_width, int dst_height);

// project 
void newprojectfile(GtkWidget * button, GtkWidget * data);
void openprojectfile(GtkWidget * button, GtkWidget * data);
void saveprojectfile(GtkWidget * button, GtkWidget * data);
void closeprojectfile(GtkWidget * button, GtkWidget * data);
void OpenProject(GtkWidget *widget,gpointer *data);


// depthediter
int str2num(char *s);
void set_resolution(GtkWidget * button,GtkWidget * data);
void messagebox(const char* str);
void get_setting_value();
void setrendermode(GtkWidget * button,GtkWidget * data);

// depthmapauto motion
H3D_RESULT CreateDepthMap(int frameNumber, const H3D_YUV input, H3D_DEPTH output, int delayed, const H3D_PARAM *pParameter);
H3D_RESULT Rendering(int frameNumber, const H3D_YUV input_yuv, const H3D_DEPTH  input_depth,  H3D_YUV output_stereo, int delayed, const H3D_PARAM *pParameter);
H3D_RESULT MotionCompute(short * depthfrommotion, unsigned char *prev_data, unsigned char *cur_data, int block_width, int block_height, int block_size);
H3D_RESULT MotionDepthAcc(unsigned char *macc, short * depthfrommotion, int block_width, int block_height, int block_size);
H3D_RESULT MotionDepthPre(short * depthfrommotion, unsigned char *macc, int block_width, int block_height, int block_size);



#endif //_COMMON_H_

