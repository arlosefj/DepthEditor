#ifndef _LANGUAGE_H_
#define _LANGUAGE_H_

#define USE_CHINESE

#ifdef USE_CHINESE
/*������*/
#define MAIN_WIND_NAME		(g_locale_to_utf8("��ά����ת��",-1,NULL,NULL,NULL))

#define AREA_LAYER			(g_locale_to_utf8("�ֲ������",-1,NULL,NULL,NULL))
#define AREA_FRMEDIT		(g_locale_to_utf8("֡�༭��",-1,NULL,NULL,NULL))
#define AREA_QKSET 			(g_locale_to_utf8("���������",-1,NULL,NULL,NULL))
#define AREA_FRAME 			(g_locale_to_utf8("֡ѡ����",-1,NULL,NULL,NULL))
#define AREA_KEYFRM 		(g_locale_to_utf8("�ؼ�֡ѡ����",-1,NULL,NULL,NULL))

#define AREA_FRMEDIT_SEGM	(g_locale_to_utf8("�ָ�ģʽ",-1,NULL,NULL,NULL))
#define AREA_FRMEDIT_RENM	(g_locale_to_utf8("��Ⱦģʽ",-1,NULL,NULL,NULL))

#define AREA_QKSET_SAD		(g_locale_to_utf8("��ʾ���в�:",-1,NULL,NULL,NULL))
#define AREA_QKSET_YES		(g_locale_to_utf8("  ��  ",-1,NULL,NULL,NULL))
#define AREA_QKSET_No		(g_locale_to_utf8("  ��  ",-1,NULL,NULL,NULL))
#define AREA_QKSET_SAS		(g_locale_to_utf8("��ʾ�Զ��ָ�:",-1,NULL,NULL,NULL))
#define AREA_QKSET_RA		(g_locale_to_utf8("�ֹ��༭����:",-1,NULL,NULL,NULL))
#define AREA_QKSET_NR		(g_locale_to_utf8("      ��  ��  ��  ",-1,NULL,NULL,NULL))
#define AREA_QKSET_SR		(g_locale_to_utf8("  ��  ��  ��  ��  ",-1,NULL,NULL,NULL))
#define AREA_QKSET_MR		(g_locale_to_utf8("  ��  ͼ  ��  ��  ",-1,NULL,NULL,NULL))
#define AREA_QKSET_DR		(g_locale_to_utf8("  ��  ��  ��  ��  ",-1,NULL,NULL,NULL))
#define AREA_QKSET_RM		(g_locale_to_utf8("�༭ģ��:",-1,NULL,NULL,NULL))
#define AREA_QKSET_CIR		(g_locale_to_utf8("  Բ  ��  ",-1,NULL,NULL,NULL))
#define AREA_QKSET_SQU		(g_locale_to_utf8("  ��  ��  ",-1,NULL,NULL,NULL))
#define AREA_QKSET_MSS		(g_locale_to_utf8("��ͼ�ָ�ǿ��:",-1,NULL,NULL,NULL))
#define AREA_QKSET_RP		(g_locale_to_utf8("��Ⱦ����:",-1,NULL,NULL,NULL))
#define AREA_QKSET_RR		(g_locale_to_utf8("  ��  Ⱦ  ��  ��  ",-1,NULL,NULL,NULL))

#define AREA_LAYER_ALS		(g_locale_to_utf8("  ��  ��  ��  ��  ",-1,NULL,NULL,NULL))
#define AREA_LAYER_L		(g_locale_to_utf8("  ��� ",-1,NULL,NULL,NULL))

/*������*/
#define TOOLBAR_NEWPROJ			(g_locale_to_utf8("�½�һ������",-1,NULL,NULL,NULL))
#define TOOLBAR_OPENPROJ		(g_locale_to_utf8("��һ������",-1,NULL,NULL,NULL))
#define TOOLBAR_SAVEPROJ		(g_locale_to_utf8("���浱ǰ����",-1,NULL,NULL,NULL))
#define TOOLBAR_CLOSEPROJ		(g_locale_to_utf8("�رյ�ǰ����",-1,NULL,NULL,NULL))
#define TOOLBAR_CUTSEQ			(g_locale_to_utf8("������Ƶ����",-1,NULL,NULL,NULL))
#define TOOLBAR_OPTCONF			(g_locale_to_utf8("ѡ������",-1,NULL,NULL,NULL))
#define TOOLBAR_SEGIMG			(g_locale_to_utf8("�Զ��ָǰ֡",-1,NULL,NULL,NULL))
#define TOOLBAR_CHKSEGDONE		(g_locale_to_utf8("����ֹ��ָ��Ƿ����",-1,NULL,NULL,NULL))
#define TOOLBAR_AUTOSEG			(g_locale_to_utf8("�Զ�ʣ��ָ�",-1,NULL,NULL,NULL))
#define TOOLBAR_REVSEG			(g_locale_to_utf8("�ָ���һ�ηָ�",-1,NULL,NULL,NULL))
#define TOOLBAR_INF				(g_locale_to_utf8("������֡����",-1,NULL,NULL,NULL))
#define TOOLBAR_AUTODEPGEN		(g_locale_to_utf8("�Զ��������",-1,NULL,NULL,NULL))
#define TOOLBAR_DEPPROC			(g_locale_to_utf8("��ȴ���",-1,NULL,NULL,NULL))
#define TOOLBAR_DEPINF			(g_locale_to_utf8("֡�������",-1,NULL,NULL,NULL))
#define TOOLBAR_LDEPINF			(g_locale_to_utf8("����������",-1,NULL,NULL,NULL))
#define TOOLBAR_ADEPINF			(g_locale_to_utf8("��/˫���������",-1,NULL,NULL,NULL))
#define TOOLBAR_RENDFRM			(g_locale_to_utf8("��Ⱦ��ǰ֡",-1,NULL,NULL,NULL))
#define TOOLBAR_CANLASTREND		(g_locale_to_utf8("�ָ���һ����Ⱦ�༭",-1,NULL,NULL,NULL))
#define TOOLBAR_RENDTOYUV		(g_locale_to_utf8("��Ⱦ���ΪYUV�ļ�",-1,NULL,NULL,NULL))
#define TOOLBAR_RENDTOH264		(g_locale_to_utf8("��Ⱦ���ΪH264�ļ�",-1,NULL,NULL,NULL))

/*�ļ��˵�*/
#define MENU_FILE			(g_locale_to_utf8("�ļ�(_F)",-1,NULL,NULL,NULL))
#define MENU_FILE_NEWPROJ	(g_locale_to_utf8("�½�����(_N)",-1,NULL,NULL,NULL))
#define MENU_FILE_OPENPROJ	(g_locale_to_utf8("�򿪹���(_O)",-1,NULL,NULL,NULL))
#define MENU_FILE_SAVEPROJ	(g_locale_to_utf8("���湤��(_S)",-1,NULL,NULL,NULL))
#define MENU_FILE_CLOSEPROJ	(g_locale_to_utf8("�رչ���(_C)",-1,NULL,NULL,NULL))
#define MENU_FILE_EXIT		(g_locale_to_utf8("�˳�(_E)",-1,NULL,NULL,NULL))

/*�༭�˵�*/
#define MENU_EDIT			(g_locale_to_utf8("�༭(_E)",-1,NULL,NULL,NULL))
#define MENU_EDIT_UNDO		(g_locale_to_utf8("����(_U)",-1,NULL,NULL,NULL))
#define MENU_EDIT_REDO		(g_locale_to_utf8("�ָ�(_R)",-1,NULL,NULL,NULL))
#define MENU_EDIT_SWKEYMODE	(g_locale_to_utf8("�ؼ�֡��ͨģʽ�л�(_K)",-1,NULL,NULL,NULL))
#define MENU_EDIT_SECSEL	(g_locale_to_utf8("Ƭ��ѡ��(_S)",-1,NULL,NULL,NULL))
#define MENU_EDIT_SEQCUT	(g_locale_to_utf8("���м���(_C)",-1,NULL,NULL,NULL))
#define MENU_EDIT_OPTION 	(g_locale_to_utf8("ѡ��(_O)",-1,NULL,NULL,NULL))

/*�ָ�˵�*/
#define MENU_SEGM			(g_locale_to_utf8("�ָ�(_S)",-1,NULL,NULL,NULL))
#define MENU_SEGM_SEGFRMS	(g_locale_to_utf8("��֡�ָ�(_S)",-1,NULL,NULL,NULL))
#define MENU_SEGM_CHECK		(g_locale_to_utf8("���δ�ָ�(_C)",-1,NULL,NULL,NULL))
#define MENU_SEGM_AUTOREF	(g_locale_to_utf8("�Զ��ָ���(_A)",-1,NULL,NULL,NULL))
#define MENU_SEGM_COMVISLAY	(g_locale_to_utf8("�ϲ��ɼ���(_V)",-1,NULL,NULL,NULL))
#define MENU_SEGM_SETREMLAY	(g_locale_to_utf8("����δ���Ϊ��ǰ��(_R)",-1,NULL,NULL,NULL))
#define MENU_SEGM_SETBSLAY	(g_locale_to_utf8("���õ�ǰ��Ϊ������(_S)",-1,NULL,NULL,NULL))
#define MENU_SEGM_SETBSLAYS	(g_locale_to_utf8("��֡���û�����(_B)",-1,NULL,NULL,NULL))
#define MENU_SEGM_BGGEN		(g_locale_to_utf8("��������(_G)",-1,NULL,NULL,NULL))
#define MENU_SEGM_INF		(g_locale_to_utf8("����(_I)",-1,NULL,NULL,NULL))

/*��Ȳ˵�*/
#define MENU_DEPT			(g_locale_to_utf8("���(_D)",-1,NULL,NULL,NULL))
#define MENU_DEPT_FRMDEPMOD	(g_locale_to_utf8("֡���ģ��(_M)",-1,NULL,NULL,NULL))
#define MENU_DEPT_BSDEPASSG	(g_locale_to_utf8("�����������(_D)",-1,NULL,NULL,NULL))
#define MENU_DEPT_AUTOGEN	(g_locale_to_utf8("�Զ�����(_G)",-1,NULL,NULL,NULL))
#define MENU_DEPT_AUTOASSG	(g_locale_to_utf8("�Զ���ֵ(_A)",-1,NULL,NULL,NULL))
#define MENU_DEPT_PROC		(g_locale_to_utf8("��ȴ���(_P)",-1,NULL,NULL,NULL))
#define MENU_DEPT_FRMINF	(g_locale_to_utf8("֡�������(_F)",-1,NULL,NULL,NULL))
#define MENU_DEPT_LAYINF	(g_locale_to_utf8("����������(_L)",-1,NULL,NULL,NULL))
#define MENU_DEPT_AUTOINF	(g_locale_to_utf8("�Զ��������(_I)",-1,NULL,NULL,NULL))

/*��Ⱦ�˵�*/
#define MENU_REND			(g_locale_to_utf8("��Ⱦ(_R)",-1,NULL,NULL,NULL))
#define MENU_REND_REND		(g_locale_to_utf8("��Ⱦ��ǰ֡(_R)",-1,NULL,NULL,NULL))
#define MENU_REND_TOYUV		(g_locale_to_utf8("��Ⱦ���ΪYUV�ļ�(_Y)",-1,NULL,NULL,NULL))
#define MENU_REND_TOH264	(g_locale_to_utf8("��Ⱦ���ΪH264�ļ�(_H)",-1,NULL,NULL,NULL))
#define MENU_REND_AUTOTRANS	(g_locale_to_utf8("�Զ���Ⱦ���(_T)",-1,NULL,NULL,NULL))

/*�����˵�*/
#define MENU_HELP			(g_locale_to_utf8("����(_H)",-1,NULL,NULL,NULL))
#define MENU_HELP_HELP		(g_locale_to_utf8("����(_H)",-1,NULL,NULL,NULL))
#define MENU_HELP_LOG		(g_locale_to_utf8("��־(_L)",-1,NULL,NULL,NULL))
#define MENU_HELP_ABOUT		(g_locale_to_utf8("����(_A)",-1,NULL,NULL,NULL))

/*ѡ������*/
#define CONF_OPT 			(g_locale_to_utf8("ѡ������",-1,NULL,NULL,NULL))

#define CONF_SEGMENT		(g_locale_to_utf8("�ָ����",-1,NULL,NULL,NULL))
#define CONF_SEG_PARA		(g_locale_to_utf8("�Զ��ָ����",-1,NULL,NULL,NULL))
#define CONF_SEG_NOR		(g_locale_to_utf8("�ָ�������Ŀ",-1,NULL,NULL,NULL))
#define CONF_SEG_NORV		(g_locale_to_utf8("50 - 1000 Ĭ�� 500",-1,NULL,NULL,NULL))
#define CONF_SEG_WOSD		(g_locale_to_utf8("�ռ����Ȩ��",-1,NULL,NULL,NULL))
#define CONF_SEG_WOSDV		(g_locale_to_utf8("5 - 40 Ĭ�� 20",-1,NULL,NULL,NULL))
#define CONF_SEG_NOI		(g_locale_to_utf8("��������",-1,NULL,NULL,NULL))
#define CONF_SEG_NOIV		(g_locale_to_utf8("2 - 40 Ĭ�� 15",-1,NULL,NULL,NULL))
#define CONF_SEG_MSS		(g_locale_to_utf8("��ͼǿ��",-1,NULL,NULL,NULL))
#define CONF_SEG_MSSV		(g_locale_to_utf8("1 - 30 Ĭ�� 10",-1,NULL,NULL,NULL))
#define CONF_SEG_ASC		(g_locale_to_utf8("�Զ��ָ�߽���ɫ",-1,NULL,NULL,NULL))
#define CONF_SEG_BLK		(g_locale_to_utf8("��",-1,NULL,NULL,NULL))
#define CONF_SEG_WHT		(g_locale_to_utf8("��",-1,NULL,NULL,NULL))
#define CONF_SEG_RED		(g_locale_to_utf8("��",-1,NULL,NULL,NULL))
#define CONF_SEG_GRN		(g_locale_to_utf8("��",-1,NULL,NULL,NULL))
#define CONF_SEG_BLUE		(g_locale_to_utf8("��",-1,NULL,NULL,NULL))
#define CONF_SEG_EEI		(g_locale_to_utf8("��Ե��չ��������",-1,NULL,NULL,NULL))
#define CONF_SEG_EEIV		(g_locale_to_utf8("1 - 10 Ĭ�� 5",-1,NULL,NULL,NULL))
#define CONF_SEG_EEWS		(g_locale_to_utf8("��Ե��չ���ڴ�С",-1,NULL,NULL,NULL))
#define CONF_SEG_EEWSV		(g_locale_to_utf8("5 - 30 Ĭ�� 10",-1,NULL,NULL,NULL))
#define CONF_SEG_EET		(g_locale_to_utf8("��Ե��չ��ֵ",-1,NULL,NULL,NULL))
#define CONF_SEG_EETV		(g_locale_to_utf8("1 - 30 Ĭ�� 6",-1,NULL,NULL,NULL))
#define CONF_SEG_SEM		(g_locale_to_utf8("�ָ�༭ģʽ",-1,NULL,NULL,NULL))
#define CONF_SEG_MDL		(g_locale_to_utf8("ģ��",-1,NULL,NULL,NULL))
#define CONF_SQUARE			(g_locale_to_utf8("����",-1,NULL,NULL,NULL))
#define CONF_CIRCLE			(g_locale_to_utf8("Բ��",-1,NULL,NULL,NULL))
#define CONF_SEG_RAD		(g_locale_to_utf8("�뾶��С",-1,NULL,NULL,NULL))
#define CONF_SEG_RADV		(g_locale_to_utf8(" 1 - 30 Ĭ�� 3",-1,NULL,NULL,NULL))
#define CONF_SEG_SIP		(g_locale_to_utf8("�ָ��������",-1,NULL,NULL,NULL))
#define CONF_SEG_MODE		(g_locale_to_utf8("����ģʽ",-1,NULL,NULL,NULL))
#define CONF_SEG_MPIX		(g_locale_to_utf8("��������",-1,NULL,NULL,NULL))
#define CONF_SEG_MREG		(g_locale_to_utf8("��������",-1,NULL,NULL,NULL))
#define CONF_SEG_IS			(g_locale_to_utf8("����ǿ��",-1,NULL,NULL,NULL))
#define CONF_SEG_ISV		(g_locale_to_utf8("1 - 6 Ĭ�� 3",-1,NULL,NULL,NULL))

#define CONF_DEPPROC		(g_locale_to_utf8("��ȴ���",-1,NULL,NULL,NULL))
#define CONF_DP_DFS			(g_locale_to_utf8("����˲�ѡ��(������θ�ѡһ)",-1,NULL,NULL,NULL))
#define CONF_NONE			(g_locale_to_utf8("���˲�",-1,NULL,NULL,NULL))
#define CONF_DP_MED			(g_locale_to_utf8("��ֵ",-1,NULL,NULL,NULL))
#define CONF_DP_MEAN		(g_locale_to_utf8("��ֵ",-1,NULL,NULL,NULL))
#define CONF_DP_GAUS		(g_locale_to_utf8("��˹",-1,NULL,NULL,NULL))
#define CONF_DP_HGAUS		(g_locale_to_utf8("ˮƽ��˹",-1,NULL,NULL,NULL))
#define CONF_DP_EDGE		(g_locale_to_utf8("��Ե",-1,NULL,NULL,NULL))
#define CONF_DP_JBF			(g_locale_to_utf8("����",-1,NULL,NULL,NULL))
#define CONF_DP_HJBF		(g_locale_to_utf8("ˮƽ����",-1,NULL,NULL,NULL))
#define CONF_DP_GFS			(g_locale_to_utf8("��˹�˲�������",-1,NULL,NULL,NULL))
#define CONF_DP_GFSV		(g_locale_to_utf8(" 1 - 50 Ĭ�� 30",-1,NULL,NULL,NULL))
#define CONF_DP_TEMP		(g_locale_to_utf8("ʱ��",-1,NULL,NULL,NULL))
#define CONF_DP_DFI			(g_locale_to_utf8("��̬�˲�����",-1,NULL,NULL,NULL))
#define CONF_DP_DFIV		(g_locale_to_utf8(" 0 - 5 Ĭ�� 3",-1,NULL,NULL,NULL))
#define CONF_DP_DFEM		(g_locale_to_utf8("��ȱ༭ģʽ",-1,NULL,NULL,NULL))
#define CONF_DP_DFEMIF		(g_locale_to_utf8("����",-1,NULL,NULL,NULL))
#define CONF_DP_DFEMIG		(g_locale_to_utf8("����",-1,NULL,NULL,NULL))
#define CONF_DP_DFEMDF		(g_locale_to_utf8("����",-1,NULL,NULL,NULL))
#define CONF_DP_DFEMDG		(g_locale_to_utf8("�ݼ�",-1,NULL,NULL,NULL))
#define CONF_DP_MDL			(g_locale_to_utf8("ģ��",-1,NULL,NULL,NULL))
#define CONF_DP_RAD			(g_locale_to_utf8("�뾶",-1,NULL,NULL,NULL))
#define CONF_DP_RADV		(g_locale_to_utf8(" 1 - 30 Ĭ�� 10",-1,NULL,NULL,NULL))
#define CONF_DP_ADG			(g_locale_to_utf8("�Զ��������",-1,NULL,NULL,NULL))
#define CONF_DP_OED			(g_locale_to_utf8("�����������",-1,NULL,NULL,NULL))
#define CONF_DP_AM			(g_locale_to_utf8("�˶�����",-1,NULL,NULL,NULL))
#define CONF_ENABLE			(g_locale_to_utf8("��",-1,NULL,NULL,NULL))
#define CONF_DISABLE		(g_locale_to_utf8("�ر�",-1,NULL,NULL,NULL))
#define CONF_DP_RI			(g_locale_to_utf8("�󾫵�������",-1,NULL,NULL,NULL))
#define CONF_DP_RIV			(g_locale_to_utf8(" 4 - 64 Ĭ�� 32",-1,NULL,NULL,NULL))

#define CONF_RANDER			(g_locale_to_utf8("��Ⱦ���",-1,NULL,NULL,NULL))
#define CONF_RND_RP			(g_locale_to_utf8("��Ⱦ����",-1,NULL,NULL,NULL))
#define CONF_RND_OF			(g_locale_to_utf8("�����ʽ",-1,NULL,NULL,NULL))
#define CONF_RND_LR			(g_locale_to_utf8("�����ӵ�",-1,NULL,NULL,NULL))
#define CONF_RND_ANAG		(g_locale_to_utf8("ƫɫ���",-1,NULL,NULL,NULL))
#define CONF_RND_AT			(g_locale_to_utf8("ƫɫ����",-1,NULL,NULL,NULL))
#define CONF_RND_RB			(g_locale_to_utf8("����",-1,NULL,NULL,NULL))
#define CONF_RND_RG			(g_locale_to_utf8("����",-1,NULL,NULL,NULL))
#define CONF_RND_GR			(g_locale_to_utf8("�̺�",-1,NULL,NULL,NULL))
#define CONF_RND_BB			(g_locale_to_utf8("����",-1,NULL,NULL,NULL))
#define CONF_RND_RM			(g_locale_to_utf8("��Ⱦ����",-1,NULL,NULL,NULL))
#define CONF_RND_WARP		(g_locale_to_utf8("Warp�㷨",-1,NULL,NULL,NULL))
#define CONF_RND_S2D		(g_locale_to_utf8("Pull�㷨",-1,NULL,NULL,NULL))
#define CONF_RND_D2S		(g_locale_to_utf8("Push�㷨",-1,NULL,NULL,NULL))
#define CONF_RND_RPRO		(g_locale_to_utf8("��Ⱦ����",-1,NULL,NULL,NULL))
#define CONF_RND_NONE		(g_locale_to_utf8("������",-1,NULL,NULL,NULL))
#define CONF_RND_HF			(g_locale_to_utf8("�ڽ���ֵ",-1,NULL,NULL,NULL))
#define CONF_RND_IPT		(g_locale_to_utf8("ͼ���޸�",-1,NULL,NULL,NULL))
#define CONF_RND_HHF		(g_locale_to_utf8("��ֱ��ʲ�ֵ",-1,NULL,NULL,NULL))
#define CONF_RND_ALP		(g_locale_to_utf8(" ƽ�Ʋ����� ",-1,NULL,NULL,NULL))
#define CONF_RND_ALPV		(g_locale_to_utf8(" 1 - 50 Ĭ�� 30",-1,NULL,NULL,NULL))
#define CONF_RND_FP			(g_locale_to_utf8("��ƽ��",-1,NULL,NULL,NULL))
#define CONF_RND_FPV		(g_locale_to_utf8(" 0 - 255 Ĭ�� 127",-1,NULL,NULL,NULL))
#define CONF_RND_RES		(g_locale_to_utf8("��Ⱦ��Ե�ữ",-1,NULL,NULL,NULL))
#define CONF_RND_MEAN		(g_locale_to_utf8("��ֵ�ữ",-1,NULL,NULL,NULL))
#define CONF_RND_MED		(g_locale_to_utf8("��ֵ�ữ",-1,NULL,NULL,NULL))
#define CONF_RND_ET			(g_locale_to_utf8("�ữ��ֵ",-1,NULL,NULL,NULL))
#define CONF_RND_ETV		(g_locale_to_utf8(" 0 - 255 Ĭ�� 10",-1,NULL,NULL,NULL))
#define CONF_RND_RFEM		(g_locale_to_utf8("��Ⱦ�༭ģʽ",-1,NULL,NULL,NULL))
#define CONF_RND_MDL		(g_locale_to_utf8("ģ��",-1,NULL,NULL,NULL))
#define CONF_RND_RAD		(g_locale_to_utf8("�뾶",-1,NULL,NULL,NULL))
#define CONF_RND_RADV		(g_locale_to_utf8(" 1 - 15 Ĭ�� 5",-1,NULL,NULL,NULL))
#define CONF_RND_RRR		(g_locale_to_utf8("�ο�����뾶",-1,NULL,NULL,NULL))
#define CONF_RND_RRRV		(g_locale_to_utf8(" 10 - 60 Ĭ�� 30",-1,NULL,NULL,NULL))
#define CONF_RND_MM			(g_locale_to_utf8("ƥ��ģ���С",-1,NULL,NULL,NULL))
#define CONF_RND_SP			(g_locale_to_utf8("��ʼ��",-1,NULL,NULL,NULL))
#define CONF_RND_SPS		(g_locale_to_utf8("�Զ�",-1,NULL,NULL,NULL))
#define CONF_RND_SPM		(g_locale_to_utf8("�ֶ�",-1,NULL,NULL,NULL))

#define CONF_OK				(g_locale_to_utf8("ȷ��",-1,NULL,NULL,NULL))
#define CONF_CANCEL			(g_locale_to_utf8("ȡ��",-1,NULL,NULL,NULL))

/*����*/
#define FILE_SELECT			(g_locale_to_utf8("ѡ��һ���ļ�",-1,NULL,NULL,NULL))
// �½����̴���
#define PROJ_NEW			(g_locale_to_utf8("�½�����",-1,NULL,NULL,NULL))
#define PROJ_NEW_NAME		(g_locale_to_utf8("�������ƣ�",-1,NULL,NULL,NULL))
#define PROJ_NEW_INPFN		(g_locale_to_utf8("�����ļ����ƣ�",-1,NULL,NULL,NULL))
#define PROJ_NEW_INPF		(g_locale_to_utf8("�����ļ���ʽ��",-1,NULL,NULL,NULL))
#define PROJ_NEW_INPFV		(g_locale_to_utf8("��Ƶ",-1,NULL,NULL,NULL))
#define PROJ_NEW_INPFY		(g_locale_to_utf8("YUV����",-1,NULL,NULL,NULL))
#define PROJ_NEW_INPFI		(g_locale_to_utf8("ͼ��",-1,NULL,NULL,NULL))
#define PROJ_NEW_INPR		(g_locale_to_utf8("����ֱ���",-1,NULL,NULL,NULL))
#define PROJ_NEW_IFN		(g_locale_to_utf8("������Ϣ�ļ����ƣ�",-1,NULL,NULL,NULL))
#define PROJ_NEW_OFN		(g_locale_to_utf8("����ļ����ƣ�",-1,NULL,NULL,NULL))
#define PROJ_NEW_VD			(g_locale_to_utf8("��Ƶ����(��)��",-1,NULL,NULL,NULL))
#define PROJ_NEW_FROM		(g_locale_to_utf8(" �� ",-1,NULL,NULL,NULL))
#define PROJ_NEW_TO			(g_locale_to_utf8(" �� ",-1,NULL,NULL,NULL))
#define PROJ_NEW_NS			(g_locale_to_utf8("���ֶ�",-1,NULL,NULL,NULL))
#define PROJ_NEW_HS			(g_locale_to_utf8("�ֶ�",-1,NULL,NULL,NULL))
#define PROJ_NEW_SD			(g_locale_to_utf8("ÿ�γ���(��)��",-1,NULL,NULL,NULL))
#define PROJ_NEW_NKM		(g_locale_to_utf8("��ͨģʽ",-1,NULL,NULL,NULL))
#define PROJ_NEW_KM			(g_locale_to_utf8("�ؼ�֡ģʽ",-1,NULL,NULL,NULL))
#define PROJ_NEW_KDT		(g_locale_to_utf8("�ؼ�֡�����ֵ��",-1,NULL,NULL,NULL))

#define PROJ_OPEN_SEL		(g_locale_to_utf8("ѡ��һ�������ļ�",-1,NULL,NULL,NULL))

#define INFO_STRINGS		(g_locale_to_utf8(str,-1,NULL,NULL,NULL))

/*��Ϣ��*/
#define MSGBOX_INFO			(g_locale_to_utf8("",-1,NULL,NULL,NULL))
#define MSGBOX_NRFS			(g_locale_to_utf8("û��ѡ��ο�֡",-1,NULL,NULL,NULL))
#define MSGBOX_RF0IERR		(g_locale_to_utf8("��ȡ�ο�֡0��Ϣʧ��",-1,NULL,NULL,NULL))
#define MSGBOX_RF0NSEG		(g_locale_to_utf8("�ο�֡0û�зָ�",-1,NULL,NULL,NULL))
#define MSGBOX_RF1IERR		(g_locale_to_utf8("��ȡ�ο�֡1��Ϣʧ��",-1,NULL,NULL,NULL))
#define MSGBOX_RF1NSEG		(g_locale_to_utf8("�ο�֡1û�зָ�",-1,NULL,NULL,NULL))
#define MSGBOX_IFOFAIL		(g_locale_to_utf8("�������ļ�ʧ��",-1,NULL,NULL,NULL))
#define MSGBOX_LIMGERR		(g_locale_to_utf8("����ͼ���ļ�ʧ��",-1,NULL,NULL,NULL))
#define MSGBOX_VFOFAIL		(g_locale_to_utf8("����Ƶ�ļ�ʧ��",-1,NULL,NULL,NULL))
#define MSGBOX_OFFIRST		(g_locale_to_utf8("���ȴ�һ���ļ��ٽ��в���",-1,NULL,NULL,NULL))
#define MSGBOX_LIFFW		(g_locale_to_utf8("������Ϣ�ļ���ʽ����",-1,NULL,NULL,NULL))
#define MSGBOX_LRIFFW		(g_locale_to_utf8("����ο���Ϣ�ļ���ʽ����",-1,NULL,NULL,NULL))
#define MSGBOX_LRIERR		(g_locale_to_utf8("������Ⱦͼ���С����",-1,NULL,NULL,NULL))
#define MSGBOX_ORYUVFE		(g_locale_to_utf8("����ȾYUV�ļ�ʧ��",-1,NULL,NULL,NULL))
#define MSGBOX_SRFF			(g_locale_to_utf8("���ȷָ�ο�֡",-1,NULL,NULL,NULL))
#define MSGBOX_PEXIST		(g_locale_to_utf8("�������Ѿ�����",-1,NULL,NULL,NULL))
#define MSGBOX_PPFFAIL		(g_locale_to_utf8("���������ļ�ʧ��",-1,NULL,NULL,NULL))
#define MSGBOX_ERM			(g_locale_to_utf8("��������Ⱦģʽ",-1,NULL,NULL,NULL))

#define LABEL_COVER			(g_locale_to_utf8("����",-1,NULL,NULL,NULL))
#define LABEL_ALPHA			(g_locale_to_utf8("��Ⱦƽ�Ʋ�����(1-50)",-1,NULL,NULL,NULL))
#define LABEL_FORCAL		(g_locale_to_utf8("��Ⱦ��ƽ��(0-255)",-1,NULL,NULL,NULL))
#define LABEL_IDRM			(g_locale_to_utf8("��������ȷ�Χ(0-255)��ģʽ(0-19)",-1,NULL,NULL,NULL))
#define LABEL_MSEL			(g_locale_to_utf8("���ģʽ(0-19)",-1,NULL,NULL,NULL))
#define LABEL_SELFS			(g_locale_to_utf8("��ѡ����֡��Χ",-1,NULL,NULL,NULL))
#define LABEL_FROM			(g_locale_to_utf8("��",-1,NULL,NULL,NULL))
#define LABEL_TO			(g_locale_to_utf8("��",-1,NULL,NULL,NULL))
#define LABEL_BLN			(g_locale_to_utf8("��������",-1,NULL,NULL,NULL))
#define LABEL_WCCS			(g_locale_to_utf8("���棺�ò����������ǰ�ֹ��ָ���",-1,NULL,NULL,NULL))
#define LABEL_PWPX264		(g_locale_to_utf8("������x264�������",-1,NULL,NULL,NULL))
#define LABEL_PWPFFMPEG		(g_locale_to_utf8("������ffmpeg����",-1,NULL,NULL,NULL))
#define LABEL_SELRF			(g_locale_to_utf8("��ѡ��ο�֡���",-1,NULL,NULL,NULL))
#define LABEL_SELIF			(g_locale_to_utf8("��ѡ����Ҫ����֡��Χ",-1,NULL,NULL,NULL))
#define LABEL_SELS			(g_locale_to_utf8("��ѡ��ֶα��",-1,NULL,NULL,NULL))
#define LABEL_IM			(g_locale_to_utf8("����ģʽ(0-1)",-1,NULL,NULL,NULL))
#define LABEL_RF			(g_locale_to_utf8("ǰ�ο�֡���",-1,NULL,NULL,NULL))
#define LABEL_RL			(g_locale_to_utf8("��ο�֡���",-1,NULL,NULL,NULL))
#define LABEL_LN			(g_locale_to_utf8("��α��(1-30)",-1,NULL,NULL,NULL))
#define LABEL_PF			(g_locale_to_utf8("ǰһ֡",-1,NULL,NULL,NULL))
#define LABEL_NF			(g_locale_to_utf8("��һ֡",-1,NULL,NULL,NULL))
#define LABEL_PKF			(g_locale_to_utf8("ǰһ�ؼ�֡",-1,NULL,NULL,NULL))
#define LABEL_NKF			(g_locale_to_utf8("��һ�ؼ�֡",-1,NULL,NULL,NULL))
#define LABEL_OF			(g_locale_to_utf8("����֡",-1,NULL,NULL,NULL))

#define TITLE_DA			(g_locale_to_utf8("��ȸ�ֵ",-1,NULL,NULL,NULL))
#define TITLE_FDMFS			(g_locale_to_utf8("��֡���ģ��ѡ��",-1,NULL,NULL,NULL))
#define TITLE_AGBDAFS		(g_locale_to_utf8("�����Զ�������ȸ�ֵ��֡���ѡ��",-1,NULL,NULL,NULL))
#define TITLE_DAGFS			(g_locale_to_utf8("��������Զ����ɵ�֡���ѡ��",-1,NULL,NULL,NULL))
#define TITLE_DPFS			(g_locale_to_utf8("������ȴ����֡���ѡ��",-1,NULL,NULL,NULL))
#define TITLE_DIFS			(g_locale_to_utf8("������������֡���ѡ��",-1,NULL,NULL,NULL))
#define TITLE_LDIFS			(g_locale_to_utf8("���в����������֡���ѡ��",-1,NULL,NULL,NULL))
#define TITLE_ADIFS			(g_locale_to_utf8("�����Զ���������֡���ѡ��",-1,NULL,NULL,NULL))
#define TITLE_CSFS			(g_locale_to_utf8("����YUV���м��е�֡���ѡ��",-1,NULL,NULL,NULL))
#define TITLE_VSS			(g_locale_to_utf8("��Ƶ�ֶκ�ѡ��",-1,NULL,NULL,NULL))
#define TITLE_IRFS			(g_locale_to_utf8("��������Ĳο�֡���ѡ��",-1,NULL,NULL,NULL))
#define TITLE_RFS			(g_locale_to_utf8("������Ⱦ��֡���ѡ��",-1,NULL,NULL,NULL))
#define TITLE_ADT			(g_locale_to_utf8("�Զ�2D��3Dת��",-1,NULL,NULL,NULL))
#define TITLE_RFHF			(g_locale_to_utf8("��Ⱦ��Ƶ��H.264�����ļ�",-1,NULL,NULL,NULL))
#define TITLE_ASFS			(g_locale_to_utf8("�����Զ��ָ��֡���ѡ��",-1,NULL,NULL,NULL))
#define TITLE_SF			(g_locale_to_utf8("�ָǰ֡",-1,NULL,NULL,NULL))
#define TITLE_BGFS			(g_locale_to_utf8("���б������ɵ�֡���ѡ��",-1,NULL,NULL,NULL))
#define TITLE_FS			(g_locale_to_utf8("֡���ѡ��",-1,NULL,NULL,NULL))


#else

/*������*/
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

/*������*/
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

/*�ļ��˵�*/
#define MENU_FILE			"_File"
#define MENU_FILE_NEWPROJ	"_New Project"
#define MENU_FILE_OPENPROJ	"_Open Project"
#define MENU_FILE_SAVEPROJ	"_Save Project"
#define MENU_FILE_CLOSEPROJ	"_Close Project"
#define MENU_FILE_EXIT		"_Exit"

/*�༭�˵�*/
#define MENU_EDIT			"_Edit"
#define MENU_EDIT_UNDO		"_Undo"
#define MENU_EDIT_REDO		"_Redo"
#define MENU_EDIT_SWKEYMODE	"Switch _Keyframe Mode"
#define MENU_EDIT_SECSEL	"_Select Section"
#define MENU_EDIT_SEQCUT	"_Cut Sequence"
#define MENU_EDIT_OPTION 	"_Options"

/*�ָ�˵�*/
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

/*��Ȳ˵�*/
#define MENU_DEPT			"_Depth"
#define MENU_DEPT_FRMDEPMOD	"Frame Depth _Module"
#define MENU_DEPT_BSDEPASSG	"Base_Depth Assignment"
#define MENU_DEPT_AUTOGEN	"Auto _Generation"
#define MENU_DEPT_AUTOASSG	"_Auto Assignment"
#define MENU_DEPT_PROC		"_Processing"
#define MENU_DEPT_FRMINF	"_Frame Inference"
#define MENU_DEPT_LAYINF	"_Layer Inference"
#define MENU_DEPT_AUTOINF	"Auto _Inference"

/*��Ⱦ�˵�*/
#define MENU_REND			"_Render"
#define MENU_REND_REND		"_Rendering"
#define MENU_REND_TOYUV		"To_YuvFile"
#define MENU_REND_TOH264	"To_H264File"
#define MENU_REND_AUTOTRANS	"Auto_Trans"

/*�����˵�*/
#define MENU_HELP		"_Help"
#define MENU_HELP_HELP	"_Help"
#define MENU_HELP_LOG	"_Log"
#define MENU_HELP_ABOUT	"_About"

/*ѡ������*/
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

/*����*/
#define FILE_SELECT		"Select a File"
// �½����̴���
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

/*��Ϣ��*/
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

