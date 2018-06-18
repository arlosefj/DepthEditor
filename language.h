#ifndef _LANGUAGE_H_
#define _LANGUAGE_H_

#define USE_CHINESE

#ifdef USE_CHINESE
/*主窗口*/
#define MAIN_WIND_NAME		(g_locale_to_utf8("三维立体转换",-1,NULL,NULL,NULL))

#define AREA_LAYER			(g_locale_to_utf8("分层操作区",-1,NULL,NULL,NULL))
#define AREA_FRMEDIT		(g_locale_to_utf8("帧编辑区",-1,NULL,NULL,NULL))
#define AREA_QKSET 			(g_locale_to_utf8("快捷设置区",-1,NULL,NULL,NULL))
#define AREA_FRAME 			(g_locale_to_utf8("帧选择区",-1,NULL,NULL,NULL))
#define AREA_KEYFRM 		(g_locale_to_utf8("关键帧选择区",-1,NULL,NULL,NULL))

#define AREA_FRMEDIT_SEGM	(g_locale_to_utf8("分割模式",-1,NULL,NULL,NULL))
#define AREA_FRMEDIT_RENM	(g_locale_to_utf8("渲染模式",-1,NULL,NULL,NULL))

#define AREA_QKSET_SAD		(g_locale_to_utf8("显示所有层:",-1,NULL,NULL,NULL))
#define AREA_QKSET_YES		(g_locale_to_utf8("  是  ",-1,NULL,NULL,NULL))
#define AREA_QKSET_No		(g_locale_to_utf8("  否  ",-1,NULL,NULL,NULL))
#define AREA_QKSET_SAS		(g_locale_to_utf8("显示自动分割:",-1,NULL,NULL,NULL))
#define AREA_QKSET_RA		(g_locale_to_utf8("手工编辑操作:",-1,NULL,NULL,NULL))
#define AREA_QKSET_NR		(g_locale_to_utf8("      不  编  辑  ",-1,NULL,NULL,NULL))
#define AREA_QKSET_SR		(g_locale_to_utf8("  分  割  编  辑  ",-1,NULL,NULL,NULL))
#define AREA_QKSET_MR		(g_locale_to_utf8("  抠  图  编  辑  ",-1,NULL,NULL,NULL))
#define AREA_QKSET_DR		(g_locale_to_utf8("  深  度  编  辑  ",-1,NULL,NULL,NULL))
#define AREA_QKSET_RM		(g_locale_to_utf8("编辑模板:",-1,NULL,NULL,NULL))
#define AREA_QKSET_CIR		(g_locale_to_utf8("  圆  形  ",-1,NULL,NULL,NULL))
#define AREA_QKSET_SQU		(g_locale_to_utf8("  方  形  ",-1,NULL,NULL,NULL))
#define AREA_QKSET_MSS		(g_locale_to_utf8("抠图分割强度:",-1,NULL,NULL,NULL))
#define AREA_QKSET_RP		(g_locale_to_utf8("渲染参数:",-1,NULL,NULL,NULL))
#define AREA_QKSET_RR		(g_locale_to_utf8("  渲  染  编  辑  ",-1,NULL,NULL,NULL))

#define AREA_LAYER_ALS		(g_locale_to_utf8("  所  有  层  次  ",-1,NULL,NULL,NULL))
#define AREA_LAYER_L		(g_locale_to_utf8("  层次 ",-1,NULL,NULL,NULL))

/*工具栏*/
#define TOOLBAR_NEWPROJ			(g_locale_to_utf8("新建一个工程",-1,NULL,NULL,NULL))
#define TOOLBAR_OPENPROJ		(g_locale_to_utf8("打开一个工程",-1,NULL,NULL,NULL))
#define TOOLBAR_SAVEPROJ		(g_locale_to_utf8("保存当前工程",-1,NULL,NULL,NULL))
#define TOOLBAR_CLOSEPROJ		(g_locale_to_utf8("关闭当前工程",-1,NULL,NULL,NULL))
#define TOOLBAR_CUTSEQ			(g_locale_to_utf8("剪切视频序列",-1,NULL,NULL,NULL))
#define TOOLBAR_OPTCONF			(g_locale_to_utf8("选项设置",-1,NULL,NULL,NULL))
#define TOOLBAR_SEGIMG			(g_locale_to_utf8("自动分割当前帧",-1,NULL,NULL,NULL))
#define TOOLBAR_CHKSEGDONE		(g_locale_to_utf8("检查手工分割是否完成",-1,NULL,NULL,NULL))
#define TOOLBAR_AUTOSEG			(g_locale_to_utf8("自动剩余分割",-1,NULL,NULL,NULL))
#define TOOLBAR_REVSEG			(g_locale_to_utf8("恢复上一次分割",-1,NULL,NULL,NULL))
#define TOOLBAR_INF				(g_locale_to_utf8("从其他帧推理",-1,NULL,NULL,NULL))
#define TOOLBAR_AUTODEPGEN		(g_locale_to_utf8("自动深度生成",-1,NULL,NULL,NULL))
#define TOOLBAR_DEPPROC			(g_locale_to_utf8("深度处理",-1,NULL,NULL,NULL))
#define TOOLBAR_DEPINF			(g_locale_to_utf8("帧深度推理",-1,NULL,NULL,NULL))
#define TOOLBAR_LDEPINF			(g_locale_to_utf8("层次深度推理",-1,NULL,NULL,NULL))
#define TOOLBAR_ADEPINF			(g_locale_to_utf8("单/双向深度推理",-1,NULL,NULL,NULL))
#define TOOLBAR_RENDFRM			(g_locale_to_utf8("渲染当前帧",-1,NULL,NULL,NULL))
#define TOOLBAR_CANLASTREND		(g_locale_to_utf8("恢复上一次渲染编辑",-1,NULL,NULL,NULL))
#define TOOLBAR_RENDTOYUV		(g_locale_to_utf8("渲染输出为YUV文件",-1,NULL,NULL,NULL))
#define TOOLBAR_RENDTOH264		(g_locale_to_utf8("渲染输出为H264文件",-1,NULL,NULL,NULL))

/*文件菜单*/
#define MENU_FILE			(g_locale_to_utf8("文件(_F)",-1,NULL,NULL,NULL))
#define MENU_FILE_NEWPROJ	(g_locale_to_utf8("新建工程(_N)",-1,NULL,NULL,NULL))
#define MENU_FILE_OPENPROJ	(g_locale_to_utf8("打开工程(_O)",-1,NULL,NULL,NULL))
#define MENU_FILE_SAVEPROJ	(g_locale_to_utf8("保存工程(_S)",-1,NULL,NULL,NULL))
#define MENU_FILE_CLOSEPROJ	(g_locale_to_utf8("关闭工程(_C)",-1,NULL,NULL,NULL))
#define MENU_FILE_EXIT		(g_locale_to_utf8("退出(_E)",-1,NULL,NULL,NULL))

/*编辑菜单*/
#define MENU_EDIT			(g_locale_to_utf8("编辑(_E)",-1,NULL,NULL,NULL))
#define MENU_EDIT_UNDO		(g_locale_to_utf8("撤销(_U)",-1,NULL,NULL,NULL))
#define MENU_EDIT_REDO		(g_locale_to_utf8("恢复(_R)",-1,NULL,NULL,NULL))
#define MENU_EDIT_SWKEYMODE	(g_locale_to_utf8("关键帧普通模式切换(_K)",-1,NULL,NULL,NULL))
#define MENU_EDIT_SECSEL	(g_locale_to_utf8("片段选择(_S)",-1,NULL,NULL,NULL))
#define MENU_EDIT_SEQCUT	(g_locale_to_utf8("序列剪切(_C)",-1,NULL,NULL,NULL))
#define MENU_EDIT_OPTION 	(g_locale_to_utf8("选项(_O)",-1,NULL,NULL,NULL))

/*分割菜单*/
#define MENU_SEGM			(g_locale_to_utf8("分割(_S)",-1,NULL,NULL,NULL))
#define MENU_SEGM_SEGFRMS	(g_locale_to_utf8("多帧分割(_S)",-1,NULL,NULL,NULL))
#define MENU_SEGM_CHECK		(g_locale_to_utf8("检查未分割(_C)",-1,NULL,NULL,NULL))
#define MENU_SEGM_AUTOREF	(g_locale_to_utf8("自动分割求精(_A)",-1,NULL,NULL,NULL))
#define MENU_SEGM_COMVISLAY	(g_locale_to_utf8("合并可见层(_V)",-1,NULL,NULL,NULL))
#define MENU_SEGM_SETREMLAY	(g_locale_to_utf8("设置未标记为当前层(_R)",-1,NULL,NULL,NULL))
#define MENU_SEGM_SETBSLAY	(g_locale_to_utf8("设置当前层为基本层(_S)",-1,NULL,NULL,NULL))
#define MENU_SEGM_SETBSLAYS	(g_locale_to_utf8("多帧设置基本层(_B)",-1,NULL,NULL,NULL))
#define MENU_SEGM_BGGEN		(g_locale_to_utf8("背景生成(_G)",-1,NULL,NULL,NULL))
#define MENU_SEGM_INF		(g_locale_to_utf8("推理(_I)",-1,NULL,NULL,NULL))

/*深度菜单*/
#define MENU_DEPT			(g_locale_to_utf8("深度(_D)",-1,NULL,NULL,NULL))
#define MENU_DEPT_FRMDEPMOD	(g_locale_to_utf8("帧深度模板(_M)",-1,NULL,NULL,NULL))
#define MENU_DEPT_BSDEPASSG	(g_locale_to_utf8("基本深度设置(_D)",-1,NULL,NULL,NULL))
#define MENU_DEPT_AUTOGEN	(g_locale_to_utf8("自动生成(_G)",-1,NULL,NULL,NULL))
#define MENU_DEPT_AUTOASSG	(g_locale_to_utf8("自动赋值(_A)",-1,NULL,NULL,NULL))
#define MENU_DEPT_PROC		(g_locale_to_utf8("深度处理(_P)",-1,NULL,NULL,NULL))
#define MENU_DEPT_FRMINF	(g_locale_to_utf8("帧深度推理(_F)",-1,NULL,NULL,NULL))
#define MENU_DEPT_LAYINF	(g_locale_to_utf8("层次深度推理(_L)",-1,NULL,NULL,NULL))
#define MENU_DEPT_AUTOINF	(g_locale_to_utf8("自动深度推理(_I)",-1,NULL,NULL,NULL))

/*渲染菜单*/
#define MENU_REND			(g_locale_to_utf8("渲染(_R)",-1,NULL,NULL,NULL))
#define MENU_REND_REND		(g_locale_to_utf8("渲染当前帧(_R)",-1,NULL,NULL,NULL))
#define MENU_REND_TOYUV		(g_locale_to_utf8("渲染输出为YUV文件(_Y)",-1,NULL,NULL,NULL))
#define MENU_REND_TOH264	(g_locale_to_utf8("渲染输出为H264文件(_H)",-1,NULL,NULL,NULL))
#define MENU_REND_AUTOTRANS	(g_locale_to_utf8("自动渲染输出(_T)",-1,NULL,NULL,NULL))

/*帮助菜单*/
#define MENU_HELP			(g_locale_to_utf8("帮助(_H)",-1,NULL,NULL,NULL))
#define MENU_HELP_HELP		(g_locale_to_utf8("帮助(_H)",-1,NULL,NULL,NULL))
#define MENU_HELP_LOG		(g_locale_to_utf8("日志(_L)",-1,NULL,NULL,NULL))
#define MENU_HELP_ABOUT		(g_locale_to_utf8("关于(_A)",-1,NULL,NULL,NULL))

/*选项设置*/
#define CONF_OPT 			(g_locale_to_utf8("选项设置",-1,NULL,NULL,NULL))

#define CONF_SEGMENT		(g_locale_to_utf8("分割参数",-1,NULL,NULL,NULL))
#define CONF_SEG_PARA		(g_locale_to_utf8("自动分割参数",-1,NULL,NULL,NULL))
#define CONF_SEG_NOR		(g_locale_to_utf8("分割区域数目",-1,NULL,NULL,NULL))
#define CONF_SEG_NORV		(g_locale_to_utf8("50 - 1000 默认 500",-1,NULL,NULL,NULL))
#define CONF_SEG_WOSD		(g_locale_to_utf8("空间距离权重",-1,NULL,NULL,NULL))
#define CONF_SEG_WOSDV		(g_locale_to_utf8("5 - 40 默认 20",-1,NULL,NULL,NULL))
#define CONF_SEG_NOI		(g_locale_to_utf8("迭代次数",-1,NULL,NULL,NULL))
#define CONF_SEG_NOIV		(g_locale_to_utf8("2 - 40 默认 15",-1,NULL,NULL,NULL))
#define CONF_SEG_MSS		(g_locale_to_utf8("抠图强度",-1,NULL,NULL,NULL))
#define CONF_SEG_MSSV		(g_locale_to_utf8("1 - 30 默认 10",-1,NULL,NULL,NULL))
#define CONF_SEG_ASC		(g_locale_to_utf8("自动分割边界颜色",-1,NULL,NULL,NULL))
#define CONF_SEG_BLK		(g_locale_to_utf8("黑",-1,NULL,NULL,NULL))
#define CONF_SEG_WHT		(g_locale_to_utf8("白",-1,NULL,NULL,NULL))
#define CONF_SEG_RED		(g_locale_to_utf8("红",-1,NULL,NULL,NULL))
#define CONF_SEG_GRN		(g_locale_to_utf8("绿",-1,NULL,NULL,NULL))
#define CONF_SEG_BLUE		(g_locale_to_utf8("蓝",-1,NULL,NULL,NULL))
#define CONF_SEG_EEI		(g_locale_to_utf8("边缘扩展迭代次数",-1,NULL,NULL,NULL))
#define CONF_SEG_EEIV		(g_locale_to_utf8("1 - 10 默认 5",-1,NULL,NULL,NULL))
#define CONF_SEG_EEWS		(g_locale_to_utf8("边缘扩展窗口大小",-1,NULL,NULL,NULL))
#define CONF_SEG_EEWSV		(g_locale_to_utf8("5 - 30 默认 10",-1,NULL,NULL,NULL))
#define CONF_SEG_EET		(g_locale_to_utf8("边缘扩展阈值",-1,NULL,NULL,NULL))
#define CONF_SEG_EETV		(g_locale_to_utf8("1 - 30 默认 6",-1,NULL,NULL,NULL))
#define CONF_SEG_SEM		(g_locale_to_utf8("分割编辑模式",-1,NULL,NULL,NULL))
#define CONF_SEG_MDL		(g_locale_to_utf8("模板",-1,NULL,NULL,NULL))
#define CONF_SQUARE			(g_locale_to_utf8("方形",-1,NULL,NULL,NULL))
#define CONF_CIRCLE			(g_locale_to_utf8("圆形",-1,NULL,NULL,NULL))
#define CONF_SEG_RAD		(g_locale_to_utf8("半径大小",-1,NULL,NULL,NULL))
#define CONF_SEG_RADV		(g_locale_to_utf8(" 1 - 30 默认 3",-1,NULL,NULL,NULL))
#define CONF_SEG_SIP		(g_locale_to_utf8("分割推理参数",-1,NULL,NULL,NULL))
#define CONF_SEG_MODE		(g_locale_to_utf8("推理模式",-1,NULL,NULL,NULL))
#define CONF_SEG_MPIX		(g_locale_to_utf8("基于像素",-1,NULL,NULL,NULL))
#define CONF_SEG_MREG		(g_locale_to_utf8("基于区域",-1,NULL,NULL,NULL))
#define CONF_SEG_IS			(g_locale_to_utf8("推理强度",-1,NULL,NULL,NULL))
#define CONF_SEG_ISV		(g_locale_to_utf8("1 - 6 默认 3",-1,NULL,NULL,NULL))

#define CONF_DEPPROC		(g_locale_to_utf8("深度处理",-1,NULL,NULL,NULL))
#define CONF_DP_DFS			(g_locale_to_utf8("深度滤波选择(三个层次各选一)",-1,NULL,NULL,NULL))
#define CONF_NONE			(g_locale_to_utf8("不滤波",-1,NULL,NULL,NULL))
#define CONF_DP_MED			(g_locale_to_utf8("中值",-1,NULL,NULL,NULL))
#define CONF_DP_MEAN		(g_locale_to_utf8("均值",-1,NULL,NULL,NULL))
#define CONF_DP_GAUS		(g_locale_to_utf8("高斯",-1,NULL,NULL,NULL))
#define CONF_DP_HGAUS		(g_locale_to_utf8("水平高斯",-1,NULL,NULL,NULL))
#define CONF_DP_EDGE		(g_locale_to_utf8("边缘",-1,NULL,NULL,NULL))
#define CONF_DP_JBF			(g_locale_to_utf8("联合",-1,NULL,NULL,NULL))
#define CONF_DP_HJBF		(g_locale_to_utf8("水平联合",-1,NULL,NULL,NULL))
#define CONF_DP_GFS			(g_locale_to_utf8("高斯滤波Σ参数",-1,NULL,NULL,NULL))
#define CONF_DP_GFSV		(g_locale_to_utf8(" 1 - 50 默认 30",-1,NULL,NULL,NULL))
#define CONF_DP_TEMP		(g_locale_to_utf8("时域",-1,NULL,NULL,NULL))
#define CONF_DP_DFI			(g_locale_to_utf8("形态滤波次数",-1,NULL,NULL,NULL))
#define CONF_DP_DFIV		(g_locale_to_utf8(" 0 - 5 默认 3",-1,NULL,NULL,NULL))
#define CONF_DP_DFEM		(g_locale_to_utf8("深度编辑模式",-1,NULL,NULL,NULL))
#define CONF_DP_DFEMIF		(g_locale_to_utf8("增加",-1,NULL,NULL,NULL))
#define CONF_DP_DFEMIG		(g_locale_to_utf8("递增",-1,NULL,NULL,NULL))
#define CONF_DP_DFEMDF		(g_locale_to_utf8("减少",-1,NULL,NULL,NULL))
#define CONF_DP_DFEMDG		(g_locale_to_utf8("递减",-1,NULL,NULL,NULL))
#define CONF_DP_MDL			(g_locale_to_utf8("模板",-1,NULL,NULL,NULL))
#define CONF_DP_RAD			(g_locale_to_utf8("半径",-1,NULL,NULL,NULL))
#define CONF_DP_RADV		(g_locale_to_utf8(" 1 - 30 默认 10",-1,NULL,NULL,NULL))
#define CONF_DP_ADG			(g_locale_to_utf8("自动深度生成",-1,NULL,NULL,NULL))
#define CONF_DP_OED			(g_locale_to_utf8("覆盖已有深度",-1,NULL,NULL,NULL))
#define CONF_DP_AM			(g_locale_to_utf8("运动叠加",-1,NULL,NULL,NULL))
#define CONF_ENABLE			(g_locale_to_utf8("打开",-1,NULL,NULL,NULL))
#define CONF_DISABLE		(g_locale_to_utf8("关闭",-1,NULL,NULL,NULL))
#define CONF_DP_RI			(g_locale_to_utf8("求精迭代次数",-1,NULL,NULL,NULL))
#define CONF_DP_RIV			(g_locale_to_utf8(" 4 - 64 默认 32",-1,NULL,NULL,NULL))

#define CONF_RANDER			(g_locale_to_utf8("渲染输出",-1,NULL,NULL,NULL))
#define CONF_RND_RP			(g_locale_to_utf8("渲染参数",-1,NULL,NULL,NULL))
#define CONF_RND_OF			(g_locale_to_utf8("输出格式",-1,NULL,NULL,NULL))
#define CONF_RND_LR			(g_locale_to_utf8("左右视点",-1,NULL,NULL,NULL))
#define CONF_RND_ANAG		(g_locale_to_utf8("偏色输出",-1,NULL,NULL,NULL))
#define CONF_RND_AT			(g_locale_to_utf8("偏色类型",-1,NULL,NULL,NULL))
#define CONF_RND_RB			(g_locale_to_utf8("红蓝",-1,NULL,NULL,NULL))
#define CONF_RND_RG			(g_locale_to_utf8("红绿",-1,NULL,NULL,NULL))
#define CONF_RND_GR			(g_locale_to_utf8("绿红",-1,NULL,NULL,NULL))
#define CONF_RND_BB			(g_locale_to_utf8("棕蓝",-1,NULL,NULL,NULL))
#define CONF_RND_RM			(g_locale_to_utf8("渲染方法",-1,NULL,NULL,NULL))
#define CONF_RND_WARP		(g_locale_to_utf8("Warp算法",-1,NULL,NULL,NULL))
#define CONF_RND_S2D		(g_locale_to_utf8("Pull算法",-1,NULL,NULL,NULL))
#define CONF_RND_D2S		(g_locale_to_utf8("Push算法",-1,NULL,NULL,NULL))
#define CONF_RND_RPRO		(g_locale_to_utf8("渲染后处理",-1,NULL,NULL,NULL))
#define CONF_RND_NONE		(g_locale_to_utf8("不处理",-1,NULL,NULL,NULL))
#define CONF_RND_HF			(g_locale_to_utf8("邻近赋值",-1,NULL,NULL,NULL))
#define CONF_RND_IPT		(g_locale_to_utf8("图像修复",-1,NULL,NULL,NULL))
#define CONF_RND_HHF		(g_locale_to_utf8("多分辨率插值",-1,NULL,NULL,NULL))
#define CONF_RND_ALP		(g_locale_to_utf8(" 平移参数α ",-1,NULL,NULL,NULL))
#define CONF_RND_ALPV		(g_locale_to_utf8(" 1 - 50 默认 30",-1,NULL,NULL,NULL))
#define CONF_RND_FP			(g_locale_to_utf8("焦平面",-1,NULL,NULL,NULL))
#define CONF_RND_FPV		(g_locale_to_utf8(" 0 - 255 默认 127",-1,NULL,NULL,NULL))
#define CONF_RND_RES		(g_locale_to_utf8("渲染边缘柔化",-1,NULL,NULL,NULL))
#define CONF_RND_MEAN		(g_locale_to_utf8("均值柔化",-1,NULL,NULL,NULL))
#define CONF_RND_MED		(g_locale_to_utf8("中值柔化",-1,NULL,NULL,NULL))
#define CONF_RND_ET			(g_locale_to_utf8("柔化阈值",-1,NULL,NULL,NULL))
#define CONF_RND_ETV		(g_locale_to_utf8(" 0 - 255 默认 10",-1,NULL,NULL,NULL))
#define CONF_RND_RFEM		(g_locale_to_utf8("渲染编辑模式",-1,NULL,NULL,NULL))
#define CONF_RND_MDL		(g_locale_to_utf8("模板",-1,NULL,NULL,NULL))
#define CONF_RND_RAD		(g_locale_to_utf8("半径",-1,NULL,NULL,NULL))
#define CONF_RND_RADV		(g_locale_to_utf8(" 1 - 15 默认 5",-1,NULL,NULL,NULL))
#define CONF_RND_RRR		(g_locale_to_utf8("参考区域半径",-1,NULL,NULL,NULL))
#define CONF_RND_RRRV		(g_locale_to_utf8(" 10 - 60 默认 30",-1,NULL,NULL,NULL))
#define CONF_RND_MM			(g_locale_to_utf8("匹配模板大小",-1,NULL,NULL,NULL))
#define CONF_RND_SP			(g_locale_to_utf8("起始点",-1,NULL,NULL,NULL))
#define CONF_RND_SPS		(g_locale_to_utf8("自动",-1,NULL,NULL,NULL))
#define CONF_RND_SPM		(g_locale_to_utf8("手动",-1,NULL,NULL,NULL))

#define CONF_OK				(g_locale_to_utf8("确定",-1,NULL,NULL,NULL))
#define CONF_CANCEL			(g_locale_to_utf8("取消",-1,NULL,NULL,NULL))

/*工程*/
#define FILE_SELECT			(g_locale_to_utf8("选择一个文件",-1,NULL,NULL,NULL))
// 新建工程窗口
#define PROJ_NEW			(g_locale_to_utf8("新建工程",-1,NULL,NULL,NULL))
#define PROJ_NEW_NAME		(g_locale_to_utf8("工程名称：",-1,NULL,NULL,NULL))
#define PROJ_NEW_INPFN		(g_locale_to_utf8("输入文件名称：",-1,NULL,NULL,NULL))
#define PROJ_NEW_INPF		(g_locale_to_utf8("输入文件格式：",-1,NULL,NULL,NULL))
#define PROJ_NEW_INPFV		(g_locale_to_utf8("视频",-1,NULL,NULL,NULL))
#define PROJ_NEW_INPFY		(g_locale_to_utf8("YUV序列",-1,NULL,NULL,NULL))
#define PROJ_NEW_INPFI		(g_locale_to_utf8("图像",-1,NULL,NULL,NULL))
#define PROJ_NEW_INPR		(g_locale_to_utf8("输入分辨率",-1,NULL,NULL,NULL))
#define PROJ_NEW_IFN		(g_locale_to_utf8("保存信息文件名称：",-1,NULL,NULL,NULL))
#define PROJ_NEW_OFN		(g_locale_to_utf8("输出文件名称：",-1,NULL,NULL,NULL))
#define PROJ_NEW_VD			(g_locale_to_utf8("视频长度(秒)：",-1,NULL,NULL,NULL))
#define PROJ_NEW_FROM		(g_locale_to_utf8(" 从 ",-1,NULL,NULL,NULL))
#define PROJ_NEW_TO			(g_locale_to_utf8(" 到 ",-1,NULL,NULL,NULL))
#define PROJ_NEW_NS			(g_locale_to_utf8("不分段",-1,NULL,NULL,NULL))
#define PROJ_NEW_HS			(g_locale_to_utf8("分段",-1,NULL,NULL,NULL))
#define PROJ_NEW_SD			(g_locale_to_utf8("每段长度(秒)：",-1,NULL,NULL,NULL))
#define PROJ_NEW_NKM		(g_locale_to_utf8("普通模式",-1,NULL,NULL,NULL))
#define PROJ_NEW_KM			(g_locale_to_utf8("关键帧模式",-1,NULL,NULL,NULL))
#define PROJ_NEW_KDT		(g_locale_to_utf8("关键帧检测阈值：",-1,NULL,NULL,NULL))

#define PROJ_OPEN_SEL		(g_locale_to_utf8("选择一个工程文件",-1,NULL,NULL,NULL))

#define INFO_STRINGS		(g_locale_to_utf8(str,-1,NULL,NULL,NULL))

/*消息窗*/
#define MSGBOX_INFO			(g_locale_to_utf8("",-1,NULL,NULL,NULL))
#define MSGBOX_NRFS			(g_locale_to_utf8("没有选择参考帧",-1,NULL,NULL,NULL))
#define MSGBOX_RF0IERR		(g_locale_to_utf8("读取参考帧0信息失败",-1,NULL,NULL,NULL))
#define MSGBOX_RF0NSEG		(g_locale_to_utf8("参考帧0没有分割",-1,NULL,NULL,NULL))
#define MSGBOX_RF1IERR		(g_locale_to_utf8("读取参考帧1信息失败",-1,NULL,NULL,NULL))
#define MSGBOX_RF1NSEG		(g_locale_to_utf8("参考帧1没有分割",-1,NULL,NULL,NULL))
#define MSGBOX_IFOFAIL		(g_locale_to_utf8("打开输入文件失败",-1,NULL,NULL,NULL))
#define MSGBOX_LIMGERR		(g_locale_to_utf8("载入图像文件失败",-1,NULL,NULL,NULL))
#define MSGBOX_VFOFAIL		(g_locale_to_utf8("打开视频文件失败",-1,NULL,NULL,NULL))
#define MSGBOX_OFFIRST		(g_locale_to_utf8("请先打开一个文件再进行操作",-1,NULL,NULL,NULL))
#define MSGBOX_LIFFW		(g_locale_to_utf8("载入信息文件格式错误",-1,NULL,NULL,NULL))
#define MSGBOX_LRIFFW		(g_locale_to_utf8("载入参考信息文件格式错误",-1,NULL,NULL,NULL))
#define MSGBOX_LRIERR		(g_locale_to_utf8("载入渲染图像大小错误",-1,NULL,NULL,NULL))
#define MSGBOX_ORYUVFE		(g_locale_to_utf8("打开渲染YUV文件失败",-1,NULL,NULL,NULL))
#define MSGBOX_SRFF			(g_locale_to_utf8("请先分割参考帧",-1,NULL,NULL,NULL))
#define MSGBOX_PEXIST		(g_locale_to_utf8("工程名已经存在",-1,NULL,NULL,NULL))
#define MSGBOX_PPFFAIL		(g_locale_to_utf8("解析工程文件失败",-1,NULL,NULL,NULL))
#define MSGBOX_ERM			(g_locale_to_utf8("请输入渲染模式",-1,NULL,NULL,NULL))

#define LABEL_COVER			(g_locale_to_utf8("覆盖",-1,NULL,NULL,NULL))
#define LABEL_ALPHA			(g_locale_to_utf8("渲染平移参数α(1-50)",-1,NULL,NULL,NULL))
#define LABEL_FORCAL		(g_locale_to_utf8("渲染焦平面(0-255)",-1,NULL,NULL,NULL))
#define LABEL_IDRM			(g_locale_to_utf8("请输入深度范围(0-255)和模式(0-19)",-1,NULL,NULL,NULL))
#define LABEL_MSEL			(g_locale_to_utf8("深度模式(0-19)",-1,NULL,NULL,NULL))
#define LABEL_SELFS			(g_locale_to_utf8("请选择处理帧范围",-1,NULL,NULL,NULL))
#define LABEL_FROM			(g_locale_to_utf8("从",-1,NULL,NULL,NULL))
#define LABEL_TO			(g_locale_to_utf8("到",-1,NULL,NULL,NULL))
#define LABEL_BLN			(g_locale_to_utf8("基本层编号",-1,NULL,NULL,NULL))
#define LABEL_WCCS			(g_locale_to_utf8("警告：该操作会清除当前手工分割结果",-1,NULL,NULL,NULL))
#define LABEL_PWPX264		(g_locale_to_utf8("请输入x264编码参数",-1,NULL,NULL,NULL))
#define LABEL_PWPFFMPEG		(g_locale_to_utf8("请输入ffmpeg参数",-1,NULL,NULL,NULL))
#define LABEL_SELRF			(g_locale_to_utf8("请选择参考帧编号",-1,NULL,NULL,NULL))
#define LABEL_SELIF			(g_locale_to_utf8("请选择需要推理帧范围",-1,NULL,NULL,NULL))
#define LABEL_SELS			(g_locale_to_utf8("请选择分段编号",-1,NULL,NULL,NULL))
#define LABEL_IM			(g_locale_to_utf8("推理模式(0-1)",-1,NULL,NULL,NULL))
#define LABEL_RF			(g_locale_to_utf8("前参考帧编号",-1,NULL,NULL,NULL))
#define LABEL_RL			(g_locale_to_utf8("后参考帧编号",-1,NULL,NULL,NULL))
#define LABEL_LN			(g_locale_to_utf8("层次编号(1-30)",-1,NULL,NULL,NULL))
#define LABEL_PF			(g_locale_to_utf8("前一帧",-1,NULL,NULL,NULL))
#define LABEL_NF			(g_locale_to_utf8("后一帧",-1,NULL,NULL,NULL))
#define LABEL_PKF			(g_locale_to_utf8("前一关键帧",-1,NULL,NULL,NULL))
#define LABEL_NKF			(g_locale_to_utf8("后一关键帧",-1,NULL,NULL,NULL))
#define LABEL_OF			(g_locale_to_utf8("其他帧",-1,NULL,NULL,NULL))

#define TITLE_DA			(g_locale_to_utf8("深度赋值",-1,NULL,NULL,NULL))
#define TITLE_FDMFS			(g_locale_to_utf8("整帧深度模板选择",-1,NULL,NULL,NULL))
#define TITLE_AGBDAFS		(g_locale_to_utf8("进行自动基本深度赋值的帧序号选择",-1,NULL,NULL,NULL))
#define TITLE_DAGFS			(g_locale_to_utf8("进行深度自动生成的帧序号选择",-1,NULL,NULL,NULL))
#define TITLE_DPFS			(g_locale_to_utf8("进行深度处理的帧序号选择",-1,NULL,NULL,NULL))
#define TITLE_DIFS			(g_locale_to_utf8("进行深度推理的帧序号选择",-1,NULL,NULL,NULL))
#define TITLE_LDIFS			(g_locale_to_utf8("进行层次深度推理的帧序号选择",-1,NULL,NULL,NULL))
#define TITLE_ADIFS			(g_locale_to_utf8("进行自动深度推理的帧序号选择",-1,NULL,NULL,NULL))
#define TITLE_CSFS			(g_locale_to_utf8("进行YUV序列剪切的帧序号选择",-1,NULL,NULL,NULL))
#define TITLE_VSS			(g_locale_to_utf8("视频分段号选择",-1,NULL,NULL,NULL))
#define TITLE_IRFS			(g_locale_to_utf8("进行推理的参考帧序号选择",-1,NULL,NULL,NULL))
#define TITLE_RFS			(g_locale_to_utf8("进行渲染的帧序号选择",-1,NULL,NULL,NULL))
#define TITLE_ADT			(g_locale_to_utf8("自动2D到3D转码",-1,NULL,NULL,NULL))
#define TITLE_RFHF			(g_locale_to_utf8("渲染视频到H.264编码文件",-1,NULL,NULL,NULL))
#define TITLE_ASFS			(g_locale_to_utf8("进行自动分割的帧序号选择",-1,NULL,NULL,NULL))
#define TITLE_SF			(g_locale_to_utf8("分割当前帧",-1,NULL,NULL,NULL))
#define TITLE_BGFS			(g_locale_to_utf8("进行背景生成的帧序号选择",-1,NULL,NULL,NULL))
#define TITLE_FS			(g_locale_to_utf8("帧序号选择",-1,NULL,NULL,NULL))


#else

/*主窗口*/
#define MAIN_WIND_NAME	"Depth Editer"

#define AREA_LAYER		"Layer"
#define AREA_FRMEDIT	"Frame Segmentation and Depth Assignment"
#define AREA_QKSET 		"Quick Setting"
#define AREA_FRAME 		"Frames"
#define AREA_KEYFRM 	"Key Frames"

#define AREA_FRMEDIT_SEGM	"Segmentation Mode"
#define AREA_FRMEDIT_RENM	"Rendering Mode"

#define AREA_QKSET_SAD	"Show All Depth:"
#define AREA_QKSET_YES	"Yes"
#define AREA_QKSET_No	"No"
#define AREA_QKSET_SAS	"Show Auto Segmentation:"
#define AREA_QKSET_RA	"Refine action:"
#define AREA_QKSET_NR	"No                 Refine"
#define AREA_QKSET_SR	"Segmentation Refine"
#define AREA_QKSET_MR	"Matting          Refine"
#define AREA_QKSET_DR	"Depth            Refine"
#define AREA_QKSET_RM	"Refine module:"
#define AREA_QKSET_CIR	"Circle"
#define AREA_QKSET_SQU	"Square"
#define AREA_QKSET_MSS	"Matting segment strength:"
#define AREA_QKSET_RP	"Rendering Parameters:"
#define AREA_QKSET_RR	"Rendering      Refine"

#define AREA_LAYER_ALS	"   ALL LAYERS   "
#define AREA_LAYER_L	"  LAYER "

/*工具栏*/
#define TOOLBAR_NEWPROJ			"create a project"
#define TOOLBAR_OPENPROJ		"open a project"
#define TOOLBAR_SAVEPROJ		"save the project"
#define TOOLBAR_CLOSEPROJ		"close the project"
#define TOOLBAR_CUTSEQ			"Cut Sequence"
#define TOOLBAR_OPTCONF			"Option Configurations"
#define TOOLBAR_SEGIMG			"Segment the image"
#define TOOLBAR_CHKSEGDONE		"Check the handwork segment is done"
#define TOOLBAR_AUTOSEG			"Automatic Segment"
#define TOOLBAR_REVSEG			"Reverse Segment"
#define TOOLBAR_INF				"Inference from other frame"
#define TOOLBAR_AUTODEPGEN		"Auto depth generation"
#define TOOLBAR_DEPPROC			"Depth Processing"
#define TOOLBAR_DEPINF			"Depth Inference"
#define TOOLBAR_LDEPINF			"Layer Depth Inference"
#define TOOLBAR_ADEPINF			"Auto Depth Inference"
#define TOOLBAR_RENDFRM			"Render frame"
#define TOOLBAR_CANLASTREND		"Cancel last render fine edit"
#define TOOLBAR_RENDTOYUV		"Render frames to yuv file"
#define TOOLBAR_RENDTOH264		"Render frames to h.264 file"

/*文件菜单*/
#define MENU_FILE			"_File"
#define MENU_FILE_NEWPROJ	"_New Project"
#define MENU_FILE_OPENPROJ	"_Open Project"
#define MENU_FILE_SAVEPROJ	"_Save Project"
#define MENU_FILE_CLOSEPROJ	"_Close Project"
#define MENU_FILE_EXIT		"_Exit"

/*编辑菜单*/
#define MENU_EDIT			"_Edit"
#define MENU_EDIT_UNDO		"_Undo"
#define MENU_EDIT_REDO		"_Redo"
#define MENU_EDIT_SWKEYMODE	"Switch _Keyframe Mode"
#define MENU_EDIT_SECSEL	"_Select Section"
#define MENU_EDIT_SEQCUT	"_Cut Sequence"
#define MENU_EDIT_OPTION 	"_Options"

/*分割菜单*/
#define MENU_SEGM			"_Segmentation"
#define MENU_SEGM_SEGFRMS	"_Segment Frames"
#define MENU_SEGM_CHECK		"_Check"
#define MENU_SEGM_AUTOREF	"_Automatic Refine"
#define MENU_SEGM_COMVISLAY	"Combine _Visible Layer"
#define MENU_SEGM_SETREMLAY	"Set _Remain as Layer"
#define MENU_SEGM_SETBSLAY	"_Set as Base Layer"
#define MENU_SEGM_SETBSLAYS	"Set _Base Layers"
#define MENU_SEGM_BGGEN		"Background _Generation"
#define MENU_SEGM_INF		"_Inference"

/*深度菜单*/
#define MENU_DEPT			"_Depth"
#define MENU_DEPT_FRMDEPMOD	"Frame Depth _Module"
#define MENU_DEPT_BSDEPASSG	"Base_Depth Assignment"
#define MENU_DEPT_AUTOGEN	"Auto _Generation"
#define MENU_DEPT_AUTOASSG	"_Auto Assignment"
#define MENU_DEPT_PROC		"_Processing"
#define MENU_DEPT_FRMINF	"_Frame Inference"
#define MENU_DEPT_LAYINF	"_Layer Inference"
#define MENU_DEPT_AUTOINF	"Auto _Inference"

/*渲染菜单*/
#define MENU_REND			"_Render"
#define MENU_REND_REND		"_Rendering"
#define MENU_REND_TOYUV		"To_YuvFile"
#define MENU_REND_TOH264	"To_H264File"
#define MENU_REND_AUTOTRANS	"Auto_Trans"

/*帮助菜单*/
#define MENU_HELP		"_Help"
#define MENU_HELP_HELP	"_Help"
#define MENU_HELP_LOG	"_Log"
#define MENU_HELP_ABOUT	"_About"

/*选项设置*/
#define CONF_OPT 		"Options"

#define CONF_SEGMENT	"Segmentation"
#define CONF_SEG_PARA	"Segment Parameters"
#define CONF_SEG_NOR	"number of regions:"
#define CONF_SEG_NORV	"50 - 1000 default 500"
#define CONF_SEG_WOSD	"weight of spatial distance:"
#define CONF_SEG_WOSDV	"5 - 40 default 20"
#define CONF_SEG_NOI	"number of iteration:"
#define CONF_SEG_NOIV	"2 - 40 default 15"
#define CONF_SEG_MSS	"Matting segment strength:"
#define CONF_SEG_MSSV	"1 - 30 default 10"
#define CONF_SEG_ASC	"AutoSeg Color"
#define CONF_SEG_BLK	"Black"
#define CONF_SEG_WHT	"White"
#define CONF_SEG_RED	"Red"
#define CONF_SEG_GRN	"Green"
#define CONF_SEG_BLUE	"Blue"
#define CONF_SEG_EEI	"Edge enlarge iteration:"
#define CONF_SEG_EEIV	"1 - 10 default 5"
#define CONF_SEG_EEWS	"Edge enlarge window size:"
#define CONF_SEG_EEWSV	"5 - 30 default 10"
#define CONF_SEG_EET	"Edge enlarge threshold:"
#define CONF_SEG_EETV	"1 - 30 default 6"
#define CONF_SEG_SEM	"Segmentation Edit Mode"
#define CONF_SEG_MDL	"Module"
#define CONF_SQUARE		"Square"
#define CONF_CIRCLE		"Circle"
#define CONF_SEG_RAD	"Radius"
#define CONF_SEG_RADV	" 1 - 30 default 3"
#define CONF_SEG_SIP	"Segment Inference Parameters"
#define CONF_SEG_MODE	"Mode"
#define CONF_SEG_MPIX	"Pixel"
#define CONF_SEG_MREG	"Region"
#define CONF_SEG_IS		"inference strength:"
#define CONF_SEG_ISV	"1 - 6 default 3"

#define CONF_DEPPROC	"Depth Processing"
#define CONF_DP_DFS		"Depth Filter Selection"
#define CONF_NONE		"None"
#define CONF_DP_MED		"Median"
#define CONF_DP_MEAN	"Mean"
#define CONF_DP_GAUS	"Gaussion"
#define CONF_DP_HGAUS	"HGaussion"
#define CONF_DP_EDGE	"Edge"
#define CONF_DP_JBF		"JBF"
#define CONF_DP_HJBF	"HJBF"
#define CONF_DP_GFS		" Gaussion Filter Sigma "
#define CONF_DP_GFSV	" 1 - 50 default 30"
#define CONF_DP_TEMP	"Temporal"
#define CONF_DP_DFI		" Dilate Filter Iterate "
#define CONF_DP_DFIV	" 0 - 5 default 3"
#define CONF_DP_DFEM	"Depth Fine Edit Mode"
#define CONF_DP_DFEMIF	"Inc&Flat"
#define CONF_DP_DFEMIG	"Inc&Grad"
#define CONF_DP_DFEMDF	"Dec&Flat"
#define CONF_DP_DFEMDG	"Dec&Grad"
#define CONF_DP_MDL		"Module"
#define CONF_DP_RAD		" Radius "
#define CONF_DP_RADV	" 1 - 30 default 10"
#define CONF_DP_ADG		"Auto Depth Generation"
#define CONF_DP_OED		"Overwrite Existing Depth"
#define CONF_DP_AM		"Add Motion"
#define CONF_ENABLE		"Enable"
#define CONF_DISABLE	"Disable"
#define CONF_DP_RI		" Refine Iterate "
#define CONF_DP_RIV		" 4 - 64 default 32"

#define CONF_RANDER		"Rendering"
#define CONF_RND_RP		"Rendering Parameter"
#define CONF_RND_OF		" Output Format "
#define CONF_RND_LR		"LeftRight"
#define CONF_RND_ANAG	"Anaglyph"
#define CONF_RND_AT		" Anaglyph Type "
#define CONF_RND_RB		"Red&Blue"
#define CONF_RND_RG		"Red&Green"
#define CONF_RND_GR		"Green&Red"
#define CONF_RND_BB		"Brown&Blue"
#define CONF_RND_RM		" Render Method "
#define CONF_RND_WARP	"Warp"
#define CONF_RND_S2D	"PULL"
#define CONF_RND_D2S	"PUSH"
#define CONF_RND_RPRO	" Render Process "
#define CONF_RND_NONE	"None"
#define CONF_RND_HF		"HollFill"
#define CONF_RND_IPT	"Inpainting"
#define CONF_RND_HHF	"HHF"
#define CONF_RND_ALP	" Alpha "
#define CONF_RND_ALPV	" 1 - 50 default 30"
#define CONF_RND_FP		" Focal Plane "
#define CONF_RND_FPV	" 0 - 255 default 127"
#define CONF_RND_RES	" Render Edge Soften "
#define CONF_RND_MEAN	"Mean"
#define CONF_RND_MED	"Median"
#define CONF_RND_ET		" Edge Threshold "
#define CONF_RND_ETV	" 0 - 255 default 10"
#define CONF_RND_RFEM	"Render Fine Edit Mode"
#define CONF_RND_MDL	"Module"
#define CONF_RND_RAD	" Radius "
#define CONF_RND_RADV	" 1 - 15 default 5"
#define CONF_RND_RRR	" Ref Region Radius "
#define CONF_RND_RRRV	" 10 - 60 default 30"
#define CONF_RND_MM		"Match Module"
#define CONF_RND_SP		"Start Point"
#define CONF_RND_SPS	"Structure"
#define CONF_RND_SPM	"Manual"

#define CONF_OK			" O K "
#define CONF_CANCEL		"Cancel"

/*工程*/
#define FILE_SELECT		"Select a File"
// 新建工程窗口
#define PROJ_NEW		"New Project"
#define PROJ_NEW_NAME	"Project Name: "
#define PROJ_NEW_INPFN	"Input  File Name: "
#define PROJ_NEW_INPF	"Input Format: "
#define PROJ_NEW_INPFV	"VIDEO"
#define PROJ_NEW_INPFY	"YUV"
#define PROJ_NEW_INPFI	"IMG"
#define PROJ_NEW_INPR	"Input Resolution: "
#define PROJ_NEW_IFN	"Info   File Name: "
#define PROJ_NEW_OFN	"Output File Name: "
#define PROJ_NEW_VD		"Video duration(sec): "
#define PROJ_NEW_FROM	" From "
#define PROJ_NEW_TO		" to "
#define PROJ_NEW_NS		"No Section"
#define PROJ_NEW_HS		"Have Section"
#define PROJ_NEW_SD		"Section duration(sec): "
#define PROJ_NEW_NKM	"Non Keyframe Mode"
#define PROJ_NEW_KM		"Use Keyframe Mode"
#define PROJ_NEW_KDT	"Keyframe Detect Threshold: "

#define PROJ_OPEN_SEL	"Select Project File"

#define INFO_STRINGS	str

/*消息窗*/
#define MSGBOX_INFO		"Information"
#define MSGBOX_NRFS		"No reference frame selected."
#define MSGBOX_RF0IERR	"Reference frame 0 info error."
#define MSGBOX_RF0NSEG	"Reference frame 0 has not segmented."
#define MSGBOX_RF1IERR	"Reference frame 1 info error."
#define MSGBOX_RF1NSEG	"Reference frame 1 has not segmented."
#define MSGBOX_IFOFAIL	"input file open failed!\n"
#define MSGBOX_LIMGERR	"Load Image Error!"
#define MSGBOX_VFOFAIL	"video file open failed!\n"
#define MSGBOX_OFFIRST	"Please open a file first."
#define MSGBOX_LIFFW	"Load information file format is wrong!"
#define MSGBOX_LRIFFW	"Load reference information file format is wrong!"
#define MSGBOX_LRIERR	"Load Render Image has wrong width or height value."
#define MSGBOX_ORYUVFE	"Can't open the rendered yuv file"
#define MSGBOX_SRFF		"Please segment the reference frame first."
#define MSGBOX_PEXIST	"The project is exist!\n"
#define MSGBOX_PPFFAIL	"Parsing project file Failed"
#define MSGBOX_ERM		"please enter the render mode"

#define LABEL_COVER		"Cover"
#define LABEL_ALPHA		"alpha(1 - 50)"
#define LABEL_FORCAL	"forcal(0-255)"
#define LABEL_IDRM		"Please input the depth range(0 - 255) and mode(0-19):"
#define LABEL_MSEL		"Mode(0- 19):"
#define LABEL_SELFS		"Please select the frames: "
#define LABEL_FROM		"From"
#define LABEL_TO		"To"
#define LABEL_BLN		"Base layer number: "
#define LABEL_WCCS		"Warning: this action will clear the current segmentation!"
#define LABEL_PWPX264	"Please write the parameters (for x264): "
#define LABEL_PWPFFMPEG	"Please write the parameters (for ffmpeg): "
#define LABEL_SELRF		"Please select the reference frame: "
#define LABEL_SELIF		"Please select the inference frames: "
#define LABEL_SELS		"Please select the section: "
#define LABEL_IM		"Inference Mode(0-1):"
#define LABEL_RF		"Reference First"
#define LABEL_RL		"Last"
#define LABEL_LN		"Layer Number(1-31):"
#define LABEL_PF		"Previous Frame"
#define LABEL_NF		"Next Frame"
#define LABEL_PKF		"Previous KeyFrame"
#define LABEL_NKF		"Next KeyFrame"
#define LABEL_OF		"Other Frame"

#define TITLE_DA		"Depth Assignment"
#define TITLE_FDMFS		"Frame Depth Module Frame Selection"
#define TITLE_AGBDAFS	"Auto Gerneration Base Depth Assignment Frame Selection"
#define TITLE_DAGFS		"Depth Auto Gerneration Frame Selection"
#define TITLE_DPFS		"Depth Processing Frame Selection"
#define TITLE_DIFS		"Depth Inference Frames Selection"
#define TITLE_LDIFS		"Layer Depth Inference Frames Selection"
#define TITLE_ADIFS		"Auto Depth Inference Frames Selection"
#define TITLE_CSFS		"Cut Sequence Frame Selection"
#define TITLE_VSS		"Video Section Selection"
#define TITLE_IRFS		"Inference Reference Frame Selection"
#define TITLE_RFS		"Render Frames Selection"
#define TITLE_ADT		"Auto DD2DDD Transcode"
#define TITLE_RFHF		"Render Frames to H.264 encode file"
#define TITLE_ASFS		" Auto Segmentation Frame Selection"
#define TITLE_SF		"Segment the Frame"
#define TITLE_BGFS		"Background Generation Frame Selection"
#define TITLE_FS		"Frame Selection"

#endif

#endif /* _LANGUAGE_H_ */

