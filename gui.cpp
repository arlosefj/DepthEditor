#include <io.h>
/* OPENCV */
#include "cv.h"   
#include "highgui.h"  

#include "CSuperpixelSeg.h"
#include "project.h"

GtkWidget *g_wdgFileOpenSelection;		// 打开文件选择
GtkWidget *g_wdgFileLoadRenSelection;	// 载入分割文件选择
GtkWidget *g_wdgFileSaveSeqCutSelection;	// 保存序列切割文件选择

GtkWidget *g_wdgMainInterface;			// 主界面

GtkWidget *g_wdgFrameNum;				// 帧序号label
GtkWidget *g_wdgKeyFrameNum;			// 关键帧序号label
GtkWidget *g_wdgInpFrameNum;			// 帧序号entry

GtkWidget *g_wdgImgProcArea;
GtkWidget *g_wdgKeyFrameDrawArea;

//GtkWidget *g_wdgImgProcArea;
//GtkWidget *g_wdgInpFormat0, *g_wdgInpFormat1;

GtkWidget *g_wdgDepProcnone0, *g_wdgDepProcmedian, *g_wdgDepProcmean;
GtkWidget *g_wdgDepProcnone1, *g_wdgDepProcgaussion, *g_wdgDepProcugaussion;
GtkWidget *g_wdgDepProcjbf, *g_wdgDepProcujbf, *g_wdgDepProcedge;
GtkWidget *g_wdgDepProcnone2, *g_wdgDepProctimef;

GtkWidget *g_wdgRenderOutLR, *g_wdgRenderOutAnag;
GtkWidget *g_wdgRenderOutAnRedBlue, *g_wdgRenderOutAnRedGreen, *g_wdgRenderOutAnGreenRed, *g_wdgRenderOutAnBrownBlue;

GtkWidget *g_wdgRenderMethodWarp, *g_wdgRenderMethodS2D, *g_wdgRenderMethodD2S;
GtkWidget *g_wdgRenderProcNone, *g_wdgRenderProcHollFill, *g_wdgRenderProcInpainting, *g_wdgRenderProcHHF;
GtkWidget *g_wdgRenderEdgeSoftenNone, *g_wdgRenderEdgeSoftenMean, *g_wdgRenderEdgeSoftenMedian;
GtkWidget *g_wdgSegmentMode, *g_wdgRenderMode;
GtkWidget *g_wdgEditCircle, *g_wdgEditSquare, *g_wdgEditRadius, *g_wdgEditStrength, *g_wdgEditalpha, *g_wdgEditfocal;
GtkWidget *g_wdgShowDepY, *g_wdgShowDepN, *g_wdgShowSegY, *g_wdgShowSegN, *g_wdgEnAutoDepOW, *g_wdgDisAutoDepOW, *g_wdgEnAutoDepAM, *g_wdgDisAutoDepAM;

GtkWidget *g_wdgNonEdit, *g_wdgSegEdit, *g_wdgMatEdit, *g_wdgDepEdit, *g_wdgRenEdit;

GtkWidget *g_wdgSegEditModule0, *g_wdgSegEditModule1, *g_wdgSegEditModuleRad;
GtkWidget *g_wdgSegEditColor0, *g_wdgSegEditColor1, *g_wdgSegEditColor2, *g_wdgSegEditColor3, *g_wdgSegEditColor4;

GtkWidget *g_wdgInfMode0, *g_wdgInfMode1;

GtkWidget *g_wdgDepEditMode0, *g_wdgDepEditMode1, *g_wdgDepEditMode2, *g_wdgDepEditMode3;
GtkWidget *g_wdgDepEditModule0, *g_wdgDepEditModule1, * g_wdgDepEditModuleRad, * g_wdgDepFiltDilateIter, * g_wdgDepAutoRefineIter;
GtkWidget *g_wdgDepFiltGauseSigmaX, *g_wdgDepFiltGauseSigmaY;

GtkWidget *g_wdgRenEditModule0, *g_wdgRenEditModule1, *g_wdgRenEditModuleRad, *g_wdgRenEditRefRegionRad;
GtkWidget *g_wdgRenEditStartP0, *g_wdgRenEditStartP1;
GtkWidget *g_wdgRenEditMatchModule0, *g_wdgRenEditMatchModule1, *g_wdgRenEditMatchModule2;

GtkWidget *g_wdgInfo;

GtkWidget *g_wdgProcessBar;

extern int g_sectionlastdura;

/* 主界面*/
#define GUI_MAINWIN 0
//TODO 判断有效性
int str2num(char *s)
{

 int rt=0;

 while (*s>='0'&&*s<='9')
  {
   rt=rt*10+(*s-'0');

   ++s;
  }

  return rt;
}
gint expose_event (GtkWidget *widget, GdkEventExpose *event)
{
    ShowAll();
    return FALSE;
}

void delete_event_fun()
{
	if(g_frameMaxNum>0)
	{
		saveprojectfile(NULL, NULL);
		closeyuvfile();
	}
	gtk_main_quit();
}

void f_button(GtkWidget * button,GtkWidget * data)
{
    gtk_widget_hide(data);
}
#if 0
void setinp0()
{
	g_inputfileformat = INPYUV;
	//gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgInpFormat0), TRUE);
}

void setinp1()
{
	g_inputfileformat = INPIMG;
	//gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgInpFormat1), TRUE);
}
#endif

void messagebox(const char* str)
{
    GtkWidget * window,*table,*label,*button;
    /////
    window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window),MSGBOX_INFO);
    gtk_widget_set_size_request(GTK_WIDGET(window),300,100);
    gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
    g_signal_connect(G_OBJECT(window),"destroy",G_CALLBACK(gtk_widget_destroy),NULL);
    /////
    table=gtk_table_new(2,3,FALSE);
    gtk_container_add(GTK_CONTAINER(window),table);
    ///
    label=gtk_label_new(str);
    gtk_table_attach_defaults(GTK_TABLE(table),label,1,2,0,1);
    gtk_widget_show(label);
    
    
    button=gtk_button_new_with_label(CONF_OK);
    gtk_table_attach(GTK_TABLE(table),button,1,2,1,2,GTK_SHRINK,GTK_FILL,0,0);
    g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(f_button),window);
    gtk_widget_show(button);
    gtk_widget_show_all(window);
}


void loginfo(GtkWidget * button,GtkWidget * data)
{
	return ;
}


void aboutinfo(GtkWidget * button,GtkWidget * data)
{
	char str[128];
	#ifdef USE_CHINESE
	sprintf_s(str, "      2D到3D转换  \n       版本号 %s\n www.hua3d.com ", H3DVERSION);
	#else
	sprintf_s(str, "      DD2DDD  \n       version %s\n www.hua3d.com ", H3DVERSION);
	#endif
	messagebox(INFO_STRINGS);
	return ;
}

GtkWidget *g_wdgParamSetting;
SParameterInp Sparameterinput;
SParameterInp * pSparameterinput = &Sparameterinput;


void get_setting_value()
{
	int value;
	//int change = 0;
	char tmp[16];
    char *str=(char *)malloc(120*sizeof(char));

    str=(char*)gtk_entry_get_text(GTK_ENTRY(pSparameterinput->segment_numofregion));
	value = str2num(str);  
	if(value<SEGPARAM_NUMREGIONMIN)  value=SEGPARAM_NUMREGIONMIN;
	if(value>SEGPARAM_NUMREGIONMAX)  value=SEGPARAM_NUMREGIONMAX;
	if(g_segparamnumregion != value)
	{
		g_segparamnumregion = value;
		//change = 1;
	}
	
	str=(char*)gtk_entry_get_text(GTK_ENTRY(pSparameterinput->segment_weightofspadis));
	value = str2num(str);  
	if(value<SEGPARAM_WEIGHTSPADISMIN)  value=SEGPARAM_WEIGHTSPADISMIN;
	if(value>SEGPARAM_WEIGHTSPADISMAX)  value=SEGPARAM_WEIGHTSPADISMAX;
	if(g_segparamweightspadis != value)
	{
		g_segparamweightspadis = value;
		//change = 1;
	}
	
	str=(char*)gtk_entry_get_text(GTK_ENTRY(pSparameterinput->segment_numofiter));
	value = str2num(str);  
	if(value<SEGPARAM_ITERATEMIN)  value=SEGPARAM_ITERATEMIN;
	if(value>SEGPARAM_ITERATEMAX)  value=SEGPARAM_ITERATEMAX;
	if(g_segparamiterate != value)
	{
		g_segparamiterate = value;
		//change = 1;
	}

	str=(char*)gtk_entry_get_text(GTK_ENTRY(pSparameterinput->segment_autoedgeth));
	value = str2num(str);  
	if(value<SEGPARAM_AUTOEDGETHMIN)  value=SEGPARAM_AUTOEDGETHMIN;
	if(value>SEGPARAM_AUTOEDGETHMAX)  value=SEGPARAM_AUTOEDGETHMAX;
	g_segparamautoedgeth = value;

	str=(char*)gtk_entry_get_text(GTK_ENTRY(pSparameterinput->segment_edgeenlargeiter));
	value = str2num(str);  
	if(value<SEGPARAM_EDGEENLARGEITERMIN)  value=SEGPARAM_EDGEENLARGEITERMIN;
	if(value>SEGPARAM_EDGEENLARGEITERMAX)  value=SEGPARAM_EDGEENLARGEITERMAX;
	g_segparamedgeenlargeiter = value;

	str=(char*)gtk_entry_get_text(GTK_ENTRY(pSparameterinput->segment_edgeenlargesize));
	value = str2num(str);  
	if(value<SEGPARAM_EDGEENLARGESIZEMIN)  value=SEGPARAM_EDGEENLARGESIZEMIN;
	if(value>SEGPARAM_EDGEENLARGESIZEMAX)  value=SEGPARAM_EDGEENLARGESIZEMAX;
	g_segparamedgeenlargesize = value;

	str=(char*)gtk_entry_get_text(GTK_ENTRY(pSparameterinput->segment_edgeenlargeth));
	value = str2num(str);  
	if(value<SEGPARAM_EDGEENLARGETHMIN)  value=SEGPARAM_EDGEENLARGETHMIN;
	if(value>SEGPARAM_EDGEENLARGETHMAX)  value=SEGPARAM_EDGEENLARGETHMAX;
	g_segparamedgeenlargeth = value;
	
	#if 0
	str=(char*)gtk_entry_get_text(GTK_ENTRY(pSparameterinput->refine_strength));
	value = str2num(str);  
	if(value<REFPARAM_STRENGTHMIN)  value=REFPARAM_STRENGTHMIN;
	if(value>REFPARAM_STRENGTHMAX)  value=REFPARAM_STRENGTHMAX;
	if(g_refparamstrength != value)
	{
		g_refparamstrength = value;
		//change = 1;
	}
	
	str=(char*)gtk_entry_get_text(GTK_ENTRY(pSparameterinput->refine_weightofspadis));
	value = str2num(str);  
	if(value<REFPARAM_WEIGHTSPADISMIN)  value=REFPARAM_WEIGHTSPADISMIN;
	if(value>REFPARAM_WEIGHTSPADISMAX)  value=REFPARAM_WEIGHTSPADISMAX;
	if(g_refparamweightspadis != value)
	{
		g_refparamweightspadis = value;
		//change = 1;
	}

	str=(char*)gtk_entry_get_text(GTK_ENTRY(pSparameterinput->refine_numofiter));
	value = str2num(str);  
	if(value<REFPARAM_ITERATEMIN)  value=SEGPARAM_ITERATEMIN;
	if(value>REFPARAM_ITERATEMAX)  value=SEGPARAM_ITERATEMAX;
	if(g_refparamiterate != value)
	{
		g_refparamiterate = value;
		//change = 1;
	}
	#endif

	str=(char*)gtk_entry_get_text(GTK_ENTRY(pSparameterinput->inference_strength));
	value = str2num(str);  
	if(value<INFPARAM_STRENGTHMIN)  value=INFPARAM_STRENGTHMIN;
	if(value>INFPARAM_STRENGTHMAX)  value=INFPARAM_STRENGTHMAX;
	g_inferencestrength = value;

	str=(char*)gtk_entry_get_text(GTK_ENTRY(pSparameterinput->render_alpha));
	value = str2num(str);  
	if(value<RENPARAM_ALPHAMIN)  value=RENPARAM_ALPHAMIN;
	if(value>RENPARAM_ALPHAMAX)  value=RENPARAM_ALPHAMAX;
	g_renderalpha= value;

	str=(char*)gtk_entry_get_text(GTK_ENTRY(pSparameterinput->render_focalplane));
	value = str2num(str);  
	if(value<RENPARAM_FPMIN)  value=RENPARAM_FPMIN;
	if(value>RENPARAM_FPMAX)  value=RENPARAM_FPMAX;
	g_renderforcalplane = value;

	sprintf_s(tmp,"%d", g_renderalpha);
	gtk_entry_set_text(GTK_ENTRY(g_wdgEditalpha), tmp);
	sprintf_s(tmp,"%d", g_renderforcalplane);
	gtk_entry_set_text(GTK_ENTRY(g_wdgEditfocal), tmp);

	str=(char*)gtk_entry_get_text(GTK_ENTRY(pSparameterinput->render_edgesoftenth));
	value = str2num(str);  
	if(value<RENPARAM_ESTHMIN)  value=RENPARAM_ESTHMIN;
	if(value>RENPARAM_ESTHMAX)  value=RENPARAM_ESTHMAX;
	g_renderedgesoftenth = value;

	str=(char*)gtk_entry_get_text(GTK_ENTRY(g_wdgSegEditModuleRad));
	value = str2num(str);  
	if(value<SEGEDITPARAM_RADMIN)  value=SEGEDITPARAM_RADMIN;
	if(value>SEGEDITPARAM_RADMAX)  value=SEGEDITPARAM_RADMAX;
	g_segeditmodulerad = value;

	str=(char*)gtk_entry_get_text(GTK_ENTRY(g_wdgDepFiltGauseSigmaX));
	value = str2num(str);  
	if(value<DEPFILTPARAM_GSMIN)  value=DEPFILTPARAM_GSMIN;
	if(value>DEPFILTPARAM_GSMAX)  value=DEPFILTPARAM_GSMAX;
	g_depfiltgausesigmax = value;

	str=(char*)gtk_entry_get_text(GTK_ENTRY(g_wdgDepFiltGauseSigmaY));
	value = str2num(str);  
	if(value<DEPFILTPARAM_GSMIN)  value=DEPFILTPARAM_GSMIN;
	if(value>DEPFILTPARAM_GSMAX)  value=DEPFILTPARAM_GSMAX;
	g_depfiltgausesigmay = value;

	str=(char*)gtk_entry_get_text(GTK_ENTRY(g_wdgDepFiltDilateIter));
	value = str2num(str);  
	if(value<DEPFILTPARAM_DIMIN)  value=DEPFILTPARAM_DIMIN;
	if(value>DEPFILTPARAM_DIMAX)  value=DEPFILTPARAM_DIMAX;
	g_depfiltdilateiter = value;

	str=(char*)gtk_entry_get_text(GTK_ENTRY(g_wdgDepEditModuleRad));
	value = str2num(str);  
	if(value<DEPEDITPARAM_RADMIN)  value=DEPEDITPARAM_RADMIN;
	if(value>DEPEDITPARAM_RADMAX)  value=DEPEDITPARAM_RADMAX;
	g_depeditmodulerad = value;

	str=(char*)gtk_entry_get_text(GTK_ENTRY(g_wdgDepAutoRefineIter));
	value = str2num(str);  
	if(value<DEPAUTOPARAM_RIMIN)  value=DEPAUTOPARAM_RIMIN;
	if(value>DEPAUTOPARAM_RIMAX)  value=DEPAUTOPARAM_RIMAX;
	g_depautorefineiter = value;

	str=(char*)gtk_entry_get_text(GTK_ENTRY(g_wdgRenEditModuleRad));
	value = str2num(str);  
	if(value<RENEDITPARAM_SRADMIN)  value=RENEDITPARAM_SRADMIN;
	if(value>RENEDITPARAM_SRADMAX)  value=RENEDITPARAM_SRADMAX;
	g_rendereditselmodulerad = value;

	str=(char*)gtk_entry_get_text(GTK_ENTRY(g_wdgRenEditRefRegionRad));
	value = str2num(str);  
	if(value<RENEDITPARAM_RRADMIN)  value=RENEDITPARAM_RRADMIN;
	if(value>RENEDITPARAM_RRADMAX)  value=RENEDITPARAM_RRADMAX;
	g_rendereditrefregionrad = value;
	
	// 当分割参数改变时重置分割标志THINK
	//if(g_frameMaxNum>0&&change==1)
		//g_frames[g_frameIndex].SegFlag = 0; // 只是改变当前帧的分割标志 ，多帧情况? TODO THINK
	
    gtk_widget_hide (g_wdgParamSetting);

}

void parametersetting(GtkWidget * button,GtkWidget * data)
{

    //GtkWidget *window;
    //GtkWidget *g_wdgParamSetting;
    GtkWidget *button_ok;
	GtkWidget *button_cancel;
    GtkWidget *table;
    GtkWidget *notebook;
    GtkWidget *frame;
    GtkWidget *label;
	GtkWidget *hbox, *vbox;
    //GtkWidget *checkbutton;
	GtkWidget *separator;
	
    char bufferf[128];
    char bufferl[128];
      
    g_wdgParamSetting = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request(g_wdgParamSetting,PARAMSETWINWIDTH,PARAMSETWINHEIGHT);
    gtk_window_set_title(GTK_WINDOW(g_wdgParamSetting),CONF_OPT);
    g_signal_connect(G_OBJECT(g_wdgParamSetting),"destroy",G_CALLBACK(gtk_widget_destroy),NULL);
	g_signal_connect (G_OBJECT (g_wdgParamSetting), "delete_event",G_CALLBACK (gtk_widget_destroy), NULL);
    gtk_window_set_position(GTK_WINDOW(g_wdgParamSetting),GTK_WIN_POS_NONE);

    gtk_container_set_border_width (GTK_CONTAINER (g_wdgParamSetting), 5);
 
    table = gtk_table_new (6, 6, FALSE);
    gtk_container_add (GTK_CONTAINER (g_wdgParamSetting), table);
   
    notebook = gtk_notebook_new ();
    gtk_notebook_set_tab_pos (GTK_NOTEBOOK (notebook), GTK_POS_TOP);
    gtk_table_attach_defaults (GTK_TABLE (table), notebook, 0, 6, 0, 5);
    gtk_widget_show (notebook);

	/* 分割参数设置*/
	sprintf_s(bufferf, CONF_SEGMENT);

	frame = gtk_frame_new (bufferf);
	gtk_container_set_border_width (GTK_CONTAINER (frame), 10);
	gtk_widget_set_size_request (frame, PARAMSETFRMWIDTH, PARAMSETFRMHEIGHT);
	gtk_widget_show (frame);

	label = gtk_label_new (bufferf);
	gtk_notebook_append_page (GTK_NOTEBOOK (notebook), frame, label);

	vbox=gtk_vbox_new(FALSE,15);
    gtk_container_add(GTK_CONTAINER(frame),vbox);

	hbox=gtk_hbox_new(TRUE,0);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);

	label=gtk_label_new(CONF_SEG_PARA);
	gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);

	hbox=gtk_hbox_new(TRUE,0);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	
    label=gtk_label_new(CONF_SEG_NOR);
	gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	gtk_label_set_justify((GtkLabel *)label, GTK_JUSTIFY_LEFT);
    pSparameterinput->segment_numofregion=gtk_entry_new();
	gtk_box_pack_start(GTK_BOX(hbox),pSparameterinput->segment_numofregion,FALSE,FALSE,0);
	sprintf_s(bufferl, "%d", g_segparamnumregion);
	gtk_entry_set_text(GTK_ENTRY(pSparameterinput->segment_numofregion), bufferl);
	gtk_entry_set_alignment(GTK_ENTRY(pSparameterinput->segment_numofregion), 0.5);
	label=gtk_label_new(CONF_SEG_NORV);
	gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
    
	hbox=gtk_hbox_new(TRUE,0);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(CONF_SEG_WOSD);
	gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	gtk_label_set_justify((GtkLabel *)label, GTK_JUSTIFY_LEFT);
    pSparameterinput->segment_weightofspadis=gtk_entry_new();
	gtk_box_pack_start(GTK_BOX(hbox),pSparameterinput->segment_weightofspadis,FALSE,FALSE,0);
	sprintf_s(bufferl, "%d", g_segparamweightspadis);
	gtk_entry_set_text(GTK_ENTRY(pSparameterinput->segment_weightofspadis), bufferl);
	gtk_entry_set_alignment(GTK_ENTRY(pSparameterinput->segment_weightofspadis), 0.5);
	label=gtk_label_new(CONF_SEG_WOSDV);
	gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);

	hbox=gtk_hbox_new(TRUE,0);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(CONF_SEG_NOI);
	gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	gtk_label_set_justify((GtkLabel *)label, GTK_JUSTIFY_LEFT);
    pSparameterinput->segment_numofiter=gtk_entry_new();
	gtk_box_pack_start(GTK_BOX(hbox),pSparameterinput->segment_numofiter,FALSE,FALSE,0);
	sprintf_s(bufferl, "%d", g_segparamiterate);
	gtk_entry_set_text(GTK_ENTRY(pSparameterinput->segment_numofiter), bufferl);
	gtk_entry_set_alignment(GTK_ENTRY(pSparameterinput->segment_numofiter), 0.5);
	label=gtk_label_new(CONF_SEG_NOIV);
	gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);

	hbox=gtk_hbox_new(TRUE,0);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(CONF_SEG_MSS);
	gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	gtk_label_set_justify((GtkLabel *)label, GTK_JUSTIFY_LEFT);
    pSparameterinput->segment_autoedgeth=gtk_entry_new();
	gtk_box_pack_start(GTK_BOX(hbox),pSparameterinput->segment_autoedgeth,FALSE,FALSE,0);
	sprintf_s(bufferl, "%d", g_segparamautoedgeth);
	gtk_entry_set_text(GTK_ENTRY(pSparameterinput->segment_autoedgeth), bufferl);
	gtk_entry_set_alignment(GTK_ENTRY(pSparameterinput->segment_autoedgeth), 0.5);
	label=gtk_label_new(CONF_SEG_MSSV);
	gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);

	hbox=gtk_hbox_new(TRUE,0);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(CONF_SEG_ASC);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);	
	g_wdgSegEditColor0= gtk_radio_button_new_with_label(NULL, CONF_SEG_BLK);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgSegEditColor0,FALSE,FALSE,0);
	g_wdgSegEditColor1= gtk_radio_button_new_with_label(NULL, CONF_SEG_WHT);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgSegEditColor1,FALSE,FALSE,0);
	g_wdgSegEditColor2= gtk_radio_button_new_with_label(NULL, CONF_SEG_RED);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgSegEditColor2,FALSE,FALSE,0);
	g_wdgSegEditColor3= gtk_radio_button_new_with_label(NULL, CONF_SEG_GRN);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgSegEditColor3,FALSE,FALSE,0);
	g_wdgSegEditColor4= gtk_radio_button_new_with_label(NULL, CONF_SEG_BLUE);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgSegEditColor4,FALSE,FALSE,0);

	gtk_radio_button_set_group(GTK_RADIO_BUTTON(g_wdgSegEditColor1),gtk_radio_button_group(GTK_RADIO_BUTTON(g_wdgSegEditColor0)));
	gtk_radio_button_set_group(GTK_RADIO_BUTTON(g_wdgSegEditColor2),gtk_radio_button_group(GTK_RADIO_BUTTON(g_wdgSegEditColor0)));
	gtk_radio_button_set_group(GTK_RADIO_BUTTON(g_wdgSegEditColor3),gtk_radio_button_group(GTK_RADIO_BUTTON(g_wdgSegEditColor0)));
	gtk_radio_button_set_group(GTK_RADIO_BUTTON(g_wdgSegEditColor4),gtk_radio_button_group(GTK_RADIO_BUTTON(g_wdgSegEditColor0)));

	g_signal_connect( G_OBJECT(g_wdgSegEditColor0), "released", G_CALLBACK(setsecolor0), NULL );
	g_signal_connect( G_OBJECT(g_wdgSegEditColor1), "released", G_CALLBACK(setsecolor1), NULL );
	g_signal_connect( G_OBJECT(g_wdgSegEditColor2), "released", G_CALLBACK(setsecolor2), NULL );
	g_signal_connect( G_OBJECT(g_wdgSegEditColor3), "released", G_CALLBACK(setsecolor3), NULL );
	g_signal_connect( G_OBJECT(g_wdgSegEditColor4), "released", G_CALLBACK(setsecolor4), NULL );

	if(g_autosegcolor==SEGEDITCOLORBLACK)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgSegEditColor0), TRUE);
	else if(g_autosegcolor==SEGEDITCOLORWHITE)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgSegEditColor1), TRUE);
	else if(g_autosegcolor==SEGEDITCOLORRED)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgSegEditColor2), TRUE);
	else if(g_autosegcolor==SEGEDITCOLORGREEN)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgSegEditColor3), TRUE);
	else //if(g_autosegcolor==SEGEDITCOLORBLUE)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgSegEditColor4), TRUE);

	separator = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox),separator,FALSE,FALSE,0);

	hbox=gtk_hbox_new(TRUE,0);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(CONF_SEG_EEI);
	gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	gtk_label_set_justify((GtkLabel *)label, GTK_JUSTIFY_LEFT);
    pSparameterinput->segment_edgeenlargeiter=gtk_entry_new();
	gtk_box_pack_start(GTK_BOX(hbox),pSparameterinput->segment_edgeenlargeiter,FALSE,FALSE,0);
	sprintf_s(bufferl, "%d", g_segparamedgeenlargeiter);
	gtk_entry_set_text(GTK_ENTRY(pSparameterinput->segment_edgeenlargeiter), bufferl);
	gtk_entry_set_alignment(GTK_ENTRY(pSparameterinput->segment_edgeenlargeiter), 0.5);
	label=gtk_label_new(CONF_SEG_EEIV);
	gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);

	hbox=gtk_hbox_new(TRUE,0);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(CONF_SEG_EEWS);
	gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	gtk_label_set_justify((GtkLabel *)label, GTK_JUSTIFY_LEFT);
    pSparameterinput->segment_edgeenlargesize=gtk_entry_new();
	gtk_box_pack_start(GTK_BOX(hbox),pSparameterinput->segment_edgeenlargesize,FALSE,FALSE,0);
	sprintf_s(bufferl, "%d", g_segparamedgeenlargesize);
	gtk_entry_set_text(GTK_ENTRY(pSparameterinput->segment_edgeenlargesize), bufferl);
	gtk_entry_set_alignment(GTK_ENTRY(pSparameterinput->segment_edgeenlargesize), 0.5);
	label=gtk_label_new(CONF_SEG_EEWSV);
	gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);

	hbox=gtk_hbox_new(TRUE,0);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(CONF_SEG_EET);
	gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	gtk_label_set_justify((GtkLabel *)label, GTK_JUSTIFY_LEFT);
    pSparameterinput->segment_edgeenlargeth=gtk_entry_new();
	gtk_box_pack_start(GTK_BOX(hbox),pSparameterinput->segment_edgeenlargeth,FALSE,FALSE,0);
	sprintf_s(bufferl, "%d", g_segparamedgeenlargeth);
	gtk_entry_set_text(GTK_ENTRY(pSparameterinput->segment_edgeenlargeth), bufferl);
	gtk_entry_set_alignment(GTK_ENTRY(pSparameterinput->segment_edgeenlargeth), 0.5);
	label=gtk_label_new(CONF_SEG_EETV);
	gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);

	separator = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox),separator,FALSE,FALSE,0);
	
    label=gtk_label_new(CONF_SEG_SEM);
    gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

	hbox=gtk_hbox_new(TRUE,0);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(CONF_SEG_MDL);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);	
	g_wdgSegEditModule0= gtk_radio_button_new_with_label(NULL, CONF_SQUARE);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgSegEditModule0,FALSE,FALSE,0);
	g_wdgSegEditModule1= gtk_radio_button_new_with_label(NULL, CONF_CIRCLE);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgSegEditModule1,FALSE,FALSE,0);

	gtk_radio_button_set_group(GTK_RADIO_BUTTON(g_wdgSegEditModule0),gtk_radio_button_group(GTK_RADIO_BUTTON(g_wdgSegEditModule1)));

	g_signal_connect( G_OBJECT(g_wdgSegEditModule0), "released", G_CALLBACK(setsemodule0), NULL );
	g_signal_connect( G_OBJECT(g_wdgSegEditModule1), "released", G_CALLBACK(setsemodule1), NULL );

	if(g_segeditmodule==SEGEDITMODULESQU)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgSegEditModule0), TRUE);
	else //if(g_segeditmodule==SEGEDITMODULECIR)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgSegEditModule1), TRUE);

	hbox=gtk_hbox_new(TRUE,0);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(CONF_SEG_RAD);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);	
	g_wdgSegEditModuleRad = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(hbox),g_wdgSegEditModuleRad,FALSE,FALSE,0);
	sprintf_s(bufferl, "%d", g_segeditmodulerad);
	gtk_entry_set_text(GTK_ENTRY(g_wdgSegEditModuleRad), bufferl);
	gtk_entry_set_alignment(GTK_ENTRY(g_wdgSegEditModuleRad), 0.5);
	
	label=gtk_label_new(CONF_SEG_RADV);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);

	separator = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox),separator,FALSE,FALSE,0);

	hbox=gtk_hbox_new(TRUE,0);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(CONF_SEG_SIP);
	gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);

	hbox=gtk_hbox_new(TRUE,0);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(CONF_SEG_MODE);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);	
	g_wdgInfMode0 = gtk_radio_button_new_with_label(NULL, CONF_SEG_MPIX);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgInfMode0,FALSE,FALSE,0);
	g_wdgInfMode1 = gtk_radio_button_new_with_label(NULL, CONF_SEG_MREG);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgInfMode1,FALSE,FALSE,0);

	gtk_radio_button_set_group(GTK_RADIO_BUTTON(g_wdgInfMode1),gtk_radio_button_group(GTK_RADIO_BUTTON(g_wdgInfMode0)));

	g_signal_connect( G_OBJECT(g_wdgInfMode0), "released", G_CALLBACK(setinfmode0), NULL );
	g_signal_connect( G_OBJECT(g_wdgInfMode1), "released", G_CALLBACK(setinfmode1), NULL );

	if(g_inferencemode==INFMODEPIX)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgInfMode0), TRUE);
	else //if(g_inferencemode==INFMODEREG)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgInfMode1), TRUE);

	hbox=gtk_hbox_new(TRUE,0);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(CONF_SEG_IS);
	gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	gtk_label_set_justify((GtkLabel *)label, GTK_JUSTIFY_LEFT);
    pSparameterinput->inference_strength=gtk_entry_new();
	gtk_box_pack_start(GTK_BOX(hbox),pSparameterinput->inference_strength,FALSE,FALSE,0);
	sprintf_s(bufferl, "%d", g_inferencestrength);
	gtk_entry_set_text(GTK_ENTRY(pSparameterinput->inference_strength), bufferl);
	gtk_entry_set_alignment(GTK_ENTRY(pSparameterinput->inference_strength), 0.5);
	label=gtk_label_new(CONF_SEG_ISV);
	gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);

	/* Depthmap 参数设置*/
	sprintf_s(bufferf, CONF_DEPPROC);

	frame = gtk_frame_new (bufferf);
	gtk_container_set_border_width (GTK_CONTAINER (frame), 10);
	gtk_widget_set_size_request (frame, PARAMSETFRMWIDTH, PARAMSETFRMHEIGHT);
	gtk_widget_show (frame);

	label = gtk_label_new (bufferf);
	gtk_notebook_append_page (GTK_NOTEBOOK (notebook), frame, label);

	vbox=gtk_vbox_new(FALSE,15);
    gtk_container_add(GTK_CONTAINER(frame),vbox);

    label=gtk_label_new(CONF_DP_DFS);
    gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

	hbox=gtk_hbox_new(TRUE,0);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	
	g_wdgDepProcnone0 = gtk_radio_button_new_with_label(NULL, CONF_NONE);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgDepProcnone0,FALSE,FALSE,0);
	g_wdgDepProcmedian= gtk_radio_button_new_with_label(NULL, CONF_DP_MED);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgDepProcmedian,FALSE,FALSE,0);
	g_wdgDepProcmean= gtk_radio_button_new_with_label(NULL, CONF_DP_MEAN);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgDepProcmean,FALSE,FALSE,0);
	gtk_radio_button_set_group(GTK_RADIO_BUTTON(g_wdgDepProcmedian),gtk_radio_button_group(GTK_RADIO_BUTTON(g_wdgDepProcnone0)));
	gtk_radio_button_set_group(GTK_RADIO_BUTTON(g_wdgDepProcmean),gtk_radio_button_group(GTK_RADIO_BUTTON(g_wdgDepProcnone0)));

	separator = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox),separator,FALSE,FALSE,0);

	hbox=gtk_hbox_new(TRUE,0);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	
	g_wdgDepProcnone1 = gtk_radio_button_new_with_label(NULL, CONF_NONE);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgDepProcnone1,FALSE,FALSE,0);
	g_wdgDepProcgaussion= gtk_radio_button_new_with_label(NULL, CONF_DP_GAUS);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgDepProcgaussion,FALSE,FALSE,0);
	g_wdgDepProcugaussion= gtk_radio_button_new_with_label(NULL, CONF_DP_HGAUS);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgDepProcugaussion,FALSE,FALSE,0);

	//hbox=gtk_hbox_new(TRUE,0);
    //gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);

	g_wdgDepProcedge= gtk_radio_button_new_with_label(NULL, CONF_DP_EDGE);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgDepProcedge,FALSE,FALSE,0);
	g_wdgDepProcjbf = gtk_radio_button_new_with_label(NULL, CONF_DP_JBF);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgDepProcjbf,FALSE,FALSE,0);
	g_wdgDepProcujbf= gtk_radio_button_new_with_label(NULL, CONF_DP_HJBF);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgDepProcujbf,FALSE,FALSE,0);
	
	gtk_radio_button_set_group(GTK_RADIO_BUTTON(g_wdgDepProcgaussion),gtk_radio_button_group(GTK_RADIO_BUTTON(g_wdgDepProcnone1)));
	gtk_radio_button_set_group(GTK_RADIO_BUTTON(g_wdgDepProcugaussion),gtk_radio_button_group(GTK_RADIO_BUTTON(g_wdgDepProcnone1)));
	gtk_radio_button_set_group(GTK_RADIO_BUTTON(g_wdgDepProcjbf),gtk_radio_button_group(GTK_RADIO_BUTTON(g_wdgDepProcnone1)));
	gtk_radio_button_set_group(GTK_RADIO_BUTTON(g_wdgDepProcujbf),gtk_radio_button_group(GTK_RADIO_BUTTON(g_wdgDepProcnone1)));
	gtk_radio_button_set_group(GTK_RADIO_BUTTON(g_wdgDepProcedge),gtk_radio_button_group(GTK_RADIO_BUTTON(g_wdgDepProcnone1)));

	hbox=gtk_hbox_new(TRUE,0);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	
	label=gtk_label_new(CONF_DP_GFS);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);	
	g_wdgDepFiltGauseSigmaX = gtk_entry_new();
	gtk_widget_set_size_request( g_wdgDepFiltGauseSigmaX, 120, 20);
    gtk_box_pack_start(GTK_BOX(hbox),g_wdgDepFiltGauseSigmaX,FALSE,FALSE,0);
	sprintf_s(bufferl, "%d", g_depfiltgausesigmax);
	gtk_entry_set_text(GTK_ENTRY(g_wdgDepFiltGauseSigmaX), bufferl);
	gtk_entry_set_alignment(GTK_ENTRY(g_wdgDepFiltGauseSigmaX), 0.5);

	g_wdgDepFiltGauseSigmaY = gtk_entry_new();
	gtk_widget_set_size_request( g_wdgDepFiltGauseSigmaY, 120, 20);
    gtk_box_pack_start(GTK_BOX(hbox),g_wdgDepFiltGauseSigmaY,FALSE,FALSE,0);
	sprintf_s(bufferl, "%d", g_depfiltgausesigmay);
	gtk_entry_set_text(GTK_ENTRY(g_wdgDepFiltGauseSigmaY), bufferl);
	gtk_entry_set_alignment(GTK_ENTRY(g_wdgDepFiltGauseSigmaY), 0.5);
	
	label=gtk_label_new(CONF_DP_GFSV);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);

	separator = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox),separator,FALSE,FALSE,0);

	hbox=gtk_hbox_new(TRUE,0);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);

	g_wdgDepProcnone2 = gtk_radio_button_new_with_label(NULL, CONF_NONE);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgDepProcnone2,FALSE,FALSE,0);
	g_wdgDepProctimef = gtk_radio_button_new_with_label(NULL, CONF_DP_TEMP);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgDepProctimef,FALSE,FALSE,0);
	gtk_radio_button_set_group(GTK_RADIO_BUTTON(g_wdgDepProctimef),gtk_radio_button_group(GTK_RADIO_BUTTON(g_wdgDepProcnone2)));

	g_signal_connect( G_OBJECT(g_wdgDepProcnone0), "released", G_CALLBACK(setnone0), NULL );
	g_signal_connect( G_OBJECT(g_wdgDepProcmedian), "released", G_CALLBACK(setmedian), NULL );
	g_signal_connect( G_OBJECT(g_wdgDepProcmean), "released", G_CALLBACK(setmean), NULL );
	g_signal_connect( G_OBJECT(g_wdgDepProcnone1), "released", G_CALLBACK(setnone1), NULL );
	g_signal_connect( G_OBJECT(g_wdgDepProcgaussion), "released", G_CALLBACK(setgaussion), NULL );
	g_signal_connect( G_OBJECT(g_wdgDepProcugaussion), "released", G_CALLBACK(setugaussion), NULL );
	g_signal_connect( G_OBJECT(g_wdgDepProcedge), "released", G_CALLBACK(setedge), NULL );
	g_signal_connect( G_OBJECT(g_wdgDepProcjbf), "released", G_CALLBACK(setjbf), NULL );
	g_signal_connect( G_OBJECT(g_wdgDepProcujbf), "released", G_CALLBACK(setujbf), NULL );
	g_signal_connect( G_OBJECT(g_wdgDepProcnone2), "released", G_CALLBACK(setnone2), NULL );
	g_signal_connect( G_OBJECT(g_wdgDepProctimef), "released", G_CALLBACK(settimef), NULL );

	if(g_depthprocessparam&MIDFILTER)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgDepProcmedian), TRUE);
	if(g_depthprocessparam&MEANFILTER)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgDepProcmean), TRUE);
	if(g_depthprocessparam&GAUSEFILTER)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgDepProcgaussion), TRUE);
	if(g_depthprocessparam&UNGAUSEFILTER)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgDepProcugaussion), TRUE);
	if(g_depthprocessparam&EDGEFILTER)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgDepProcedge), TRUE);
	if(g_depthprocessparam&JBFILTER)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgDepProcjbf), TRUE);
	if(g_depthprocessparam&UNJBFILTER)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgDepProcujbf), TRUE);
	if(g_depthprocessparam&TEMFILTER)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgDepProctimef), TRUE);
		
	separator = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox),separator,FALSE,FALSE,0);

	hbox=gtk_hbox_new(TRUE,0);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	
	label=gtk_label_new(CONF_DP_DFI);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);	
	g_wdgDepFiltDilateIter = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(hbox),g_wdgDepFiltDilateIter,FALSE,FALSE,0);
	sprintf_s(bufferl, "%d", g_depfiltdilateiter);
	gtk_entry_set_text(GTK_ENTRY(g_wdgDepFiltDilateIter), bufferl);
	gtk_entry_set_alignment(GTK_ENTRY(g_wdgDepFiltDilateIter), 0.5);
	
	label=gtk_label_new(CONF_DP_DFIV);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	
	separator = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox),separator,FALSE,FALSE,0);

	label=gtk_label_new(CONF_DP_DFEM);
    gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

	hbox=gtk_hbox_new(TRUE,0);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	
	g_wdgDepEditMode0= gtk_radio_button_new_with_label(NULL, CONF_DP_DFEMIF);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgDepEditMode0,FALSE,FALSE,0);
	g_wdgDepEditMode1= gtk_radio_button_new_with_label(NULL, CONF_DP_DFEMIG);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgDepEditMode1,FALSE,FALSE,0);
	g_wdgDepEditMode2= gtk_radio_button_new_with_label(NULL, CONF_DP_DFEMDF);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgDepEditMode2,FALSE,FALSE,0);
	g_wdgDepEditMode3= gtk_radio_button_new_with_label(NULL, CONF_DP_DFEMDG);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgDepEditMode3,FALSE,FALSE,0);
	gtk_radio_button_set_group(GTK_RADIO_BUTTON(g_wdgDepEditMode0),gtk_radio_button_group(GTK_RADIO_BUTTON(g_wdgDepEditMode1)));
	gtk_radio_button_set_group(GTK_RADIO_BUTTON(g_wdgDepEditMode2),gtk_radio_button_group(GTK_RADIO_BUTTON(g_wdgDepEditMode1)));
	gtk_radio_button_set_group(GTK_RADIO_BUTTON(g_wdgDepEditMode3),gtk_radio_button_group(GTK_RADIO_BUTTON(g_wdgDepEditMode1)));

	g_signal_connect( G_OBJECT(g_wdgDepEditMode0), "released", G_CALLBACK(setdemode0), NULL );
	g_signal_connect( G_OBJECT(g_wdgDepEditMode1), "released", G_CALLBACK(setdemode1), NULL );
	g_signal_connect( G_OBJECT(g_wdgDepEditMode2), "released", G_CALLBACK(setdemode2), NULL );
	g_signal_connect( G_OBJECT(g_wdgDepEditMode3), "released", G_CALLBACK(setdemode3), NULL );

	if(g_depeditmode==DEPEDITMODEINCFLAT)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgDepEditMode0), TRUE);
	else if(g_depeditmode==DEPEDITMODEINCGRAD)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgDepEditMode1), TRUE);
	else if(g_depeditmode==DEPEDITMODEDECFLAT)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgDepEditMode2), TRUE);
	else //if(g_depeditmode==DEPEDITMODEDECGRAD)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgDepEditMode3), TRUE);

	hbox=gtk_hbox_new(TRUE,0);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(CONF_DP_MDL);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);	
	g_wdgDepEditModule0= gtk_radio_button_new_with_label(NULL, CONF_SQUARE);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgDepEditModule0,FALSE,FALSE,0);
	g_wdgDepEditModule1= gtk_radio_button_new_with_label(NULL, CONF_CIRCLE);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgDepEditModule1,FALSE,FALSE,0);

	gtk_radio_button_set_group(GTK_RADIO_BUTTON(g_wdgDepEditModule0),gtk_radio_button_group(GTK_RADIO_BUTTON(g_wdgDepEditModule1)));

	g_signal_connect( G_OBJECT(g_wdgDepEditModule0), "released", G_CALLBACK(setdemodule0), NULL );
	g_signal_connect( G_OBJECT(g_wdgDepEditModule1), "released", G_CALLBACK(setdemodule1), NULL );

	if(g_depeditmodule==DEPEDITMODULESQU)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgDepEditModule0), TRUE);
	else //if(g_depeditmodule==DEPEDITMODULECIR)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgDepEditModule1), TRUE);

	hbox=gtk_hbox_new(TRUE,0);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(CONF_DP_RAD);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);	
	g_wdgDepEditModuleRad = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(hbox),g_wdgDepEditModuleRad,FALSE,FALSE,0);
	sprintf_s(bufferl, "%d", g_depeditmodulerad);
	gtk_entry_set_text(GTK_ENTRY(g_wdgDepEditModuleRad), bufferl);
	gtk_entry_set_alignment(GTK_ENTRY(g_wdgDepEditModuleRad), 0.5);
	
	label=gtk_label_new(CONF_DP_RADV);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);

	separator = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox),separator,FALSE,FALSE,0);

	hbox=gtk_hbox_new(TRUE,0);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(CONF_DP_ADG);
	gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);

	hbox=gtk_hbox_new(TRUE,0);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(CONF_DP_OED);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);	
	g_wdgEnAutoDepOW = gtk_radio_button_new_with_label(NULL, CONF_ENABLE);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgEnAutoDepOW,FALSE,FALSE,0);
	g_wdgDisAutoDepOW = gtk_radio_button_new_with_label(NULL, CONF_DISABLE);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgDisAutoDepOW,FALSE,FALSE,0);
	gtk_radio_button_set_group(GTK_RADIO_BUTTON(g_wdgDisAutoDepOW),gtk_radio_button_group(GTK_RADIO_BUTTON(g_wdgEnAutoDepOW)));

	g_signal_connect( G_OBJECT(g_wdgEnAutoDepOW), "released", G_CALLBACK(setenautodepoverwrite), NULL );
	g_signal_connect( G_OBJECT(g_wdgDisAutoDepOW), "released", G_CALLBACK(setdisautodepoverwrite), NULL );

	hbox=gtk_hbox_new(TRUE,0);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(CONF_DP_AM);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);	
	g_wdgEnAutoDepAM = gtk_radio_button_new_with_label(NULL, CONF_ENABLE);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgEnAutoDepAM,FALSE,FALSE,0);
	g_wdgDisAutoDepAM = gtk_radio_button_new_with_label(NULL, CONF_DISABLE);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgDisAutoDepAM,FALSE,FALSE,0);
	gtk_radio_button_set_group(GTK_RADIO_BUTTON(g_wdgDisAutoDepAM),gtk_radio_button_group(GTK_RADIO_BUTTON(g_wdgEnAutoDepAM)));

	g_signal_connect( G_OBJECT(g_wdgEnAutoDepAM), "released", G_CALLBACK(setenautodepaddmotion), NULL );
	g_signal_connect( G_OBJECT(g_wdgDisAutoDepAM), "released", G_CALLBACK(setdisautodepaddmotion), NULL );

	hbox=gtk_hbox_new(TRUE,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(CONF_DP_RI);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);	
	g_wdgDepAutoRefineIter = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(hbox),g_wdgDepAutoRefineIter,FALSE,FALSE,0);
	sprintf_s(bufferl, "%d", g_depautorefineiter);
	gtk_entry_set_text(GTK_ENTRY(g_wdgDepAutoRefineIter), bufferl);
	gtk_entry_set_alignment(GTK_ENTRY(g_wdgDepAutoRefineIter), 0.5);
	
	label=gtk_label_new(CONF_DP_RIV);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);

	//separator = gtk_hseparator_new();
	//gtk_box_pack_start(GTK_BOX(vbox),separator,FALSE,FALSE,0);

	/* Rendering 编辑参数*/
	sprintf_s(bufferf, CONF_RANDER);

	frame = gtk_frame_new (bufferf);
	gtk_container_set_border_width (GTK_CONTAINER (frame), 10);
	gtk_widget_set_size_request (frame, PARAMSETFRMWIDTH, PARAMSETFRMHEIGHT);
	gtk_widget_show (frame);

	label = gtk_label_new (bufferf);
	gtk_notebook_append_page (GTK_NOTEBOOK (notebook), frame, label);

	vbox=gtk_vbox_new(FALSE,15);
    gtk_container_add(GTK_CONTAINER(frame),vbox);

	label=gtk_label_new(CONF_RND_RP);
    gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);
	
	hbox=gtk_hbox_new(TRUE,0);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(CONF_RND_OF);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);	
	g_wdgRenderOutLR = gtk_radio_button_new_with_label(NULL, CONF_RND_LR);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgRenderOutLR,FALSE,FALSE,0);
	g_wdgRenderOutAnag = gtk_radio_button_new_with_label(NULL, CONF_RND_ANAG);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgRenderOutAnag,FALSE,FALSE,0);
	gtk_radio_button_set_group(GTK_RADIO_BUTTON(g_wdgRenderOutAnag),gtk_radio_button_group(GTK_RADIO_BUTTON(g_wdgRenderOutLR)));

	g_signal_connect( G_OBJECT(g_wdgRenderOutLR), "released", G_CALLBACK(setleftright), NULL );
	g_signal_connect( G_OBJECT(g_wdgRenderOutAnag), "released", G_CALLBACK(setanaglyph), NULL );

	if(g_renderoutputformat==RENDEROUTLR)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgRenderOutLR), TRUE);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgRenderOutAnag), TRUE);

	hbox=gtk_hbox_new(TRUE,0);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(CONF_RND_AT);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);	
	g_wdgRenderOutAnRedBlue = gtk_radio_button_new_with_label(NULL, CONF_RND_RB);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgRenderOutAnRedBlue,FALSE,FALSE,0);
	g_wdgRenderOutAnRedGreen = gtk_radio_button_new_with_label(NULL, CONF_RND_RG);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgRenderOutAnRedGreen,FALSE,FALSE,0);
	g_wdgRenderOutAnGreenRed = gtk_radio_button_new_with_label(NULL, CONF_RND_GR);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgRenderOutAnGreenRed,FALSE,FALSE,0);
	g_wdgRenderOutAnBrownBlue = gtk_radio_button_new_with_label(NULL, CONF_RND_BB);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgRenderOutAnBrownBlue,FALSE,FALSE,0);
	gtk_radio_button_set_group(GTK_RADIO_BUTTON(g_wdgRenderOutAnRedGreen),gtk_radio_button_group(GTK_RADIO_BUTTON(g_wdgRenderOutAnRedBlue)));
	gtk_radio_button_set_group(GTK_RADIO_BUTTON(g_wdgRenderOutAnGreenRed),gtk_radio_button_group(GTK_RADIO_BUTTON(g_wdgRenderOutAnRedBlue)));
	gtk_radio_button_set_group(GTK_RADIO_BUTTON(g_wdgRenderOutAnBrownBlue),gtk_radio_button_group(GTK_RADIO_BUTTON(g_wdgRenderOutAnRedBlue)));

	g_signal_connect( G_OBJECT(g_wdgRenderOutAnRedBlue), "released", G_CALLBACK(setredblue), NULL );
	g_signal_connect( G_OBJECT(g_wdgRenderOutAnRedGreen), "released", G_CALLBACK(setredgreen), NULL );
	g_signal_connect( G_OBJECT(g_wdgRenderOutAnGreenRed), "released", G_CALLBACK(setgreenred), NULL );
	g_signal_connect( G_OBJECT(g_wdgRenderOutAnBrownBlue), "released", G_CALLBACK(setbrownblue), NULL );

	if(g_renderoutputantype==RENDEROUTAN_RE_BL)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgRenderOutAnRedBlue), TRUE);
	else if(g_renderoutputantype==RENDEROUTAN_RE_GR)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgRenderOutAnRedGreen), TRUE);
	else if(g_renderoutputantype==RENDEROUTAN_GR_RE)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgRenderOutAnGreenRed), TRUE);
	else //if(g_renderoutputantype==RENDEROUTAN_RE_GR)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgRenderOutAnBrownBlue), TRUE);

	hbox=gtk_hbox_new(TRUE,0);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(CONF_RND_RM);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);	
	g_wdgRenderMethodWarp= gtk_radio_button_new_with_label(NULL, CONF_RND_WARP);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgRenderMethodWarp,FALSE,FALSE,0);
	g_wdgRenderMethodS2D= gtk_radio_button_new_with_label(NULL, CONF_RND_S2D);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgRenderMethodS2D,FALSE,FALSE,0);
	g_wdgRenderMethodD2S= gtk_radio_button_new_with_label(NULL, CONF_RND_D2S);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgRenderMethodD2S,FALSE,FALSE,0);

	gtk_radio_button_set_group(GTK_RADIO_BUTTON(g_wdgRenderMethodS2D),gtk_radio_button_group(GTK_RADIO_BUTTON(g_wdgRenderMethodD2S)));
	gtk_radio_button_set_group(GTK_RADIO_BUTTON(g_wdgRenderMethodWarp),gtk_radio_button_group(GTK_RADIO_BUTTON(g_wdgRenderMethodD2S)));

	g_signal_connect( G_OBJECT(g_wdgRenderMethodWarp), "released", G_CALLBACK(setwarp), NULL );
	g_signal_connect( G_OBJECT(g_wdgRenderMethodS2D), "released", G_CALLBACK(sets2d), NULL );
	g_signal_connect( G_OBJECT(g_wdgRenderMethodD2S), "released", G_CALLBACK(setd2s), NULL );

	if(g_rendermethod==RENDERWARP)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgRenderMethodWarp), TRUE);
	else if(g_rendermethod==RENDERPULL)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgRenderMethodS2D), TRUE);
	else //if(g_rendermethod==RENDERPUSH)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgRenderMethodD2S), TRUE);

	hbox=gtk_hbox_new(TRUE,0);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(CONF_RND_RPRO);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);	
	g_wdgRenderProcNone= gtk_radio_button_new_with_label(NULL, CONF_RND_NONE);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgRenderProcNone,FALSE,FALSE,0);
	g_wdgRenderProcHollFill= gtk_radio_button_new_with_label(NULL, CONF_RND_HF);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgRenderProcHollFill,FALSE,FALSE,0);
	g_wdgRenderProcInpainting= gtk_radio_button_new_with_label(NULL, CONF_RND_IPT);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgRenderProcInpainting,FALSE,FALSE,0);
	g_wdgRenderProcHHF= gtk_radio_button_new_with_label(NULL, CONF_RND_HHF);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgRenderProcHHF,FALSE,FALSE,0);
	gtk_radio_button_set_group(GTK_RADIO_BUTTON(g_wdgRenderProcHollFill),gtk_radio_button_group(GTK_RADIO_BUTTON(g_wdgRenderProcNone)));
	gtk_radio_button_set_group(GTK_RADIO_BUTTON(g_wdgRenderProcInpainting),gtk_radio_button_group(GTK_RADIO_BUTTON(g_wdgRenderProcNone)));
	gtk_radio_button_set_group(GTK_RADIO_BUTTON(g_wdgRenderProcHHF),gtk_radio_button_group(GTK_RADIO_BUTTON(g_wdgRenderProcNone)));

	g_signal_connect( G_OBJECT(g_wdgRenderProcNone), "released", G_CALLBACK(setnone), NULL );
	g_signal_connect( G_OBJECT(g_wdgRenderProcHollFill), "released", G_CALLBACK(sethollfill), NULL );
	g_signal_connect( G_OBJECT(g_wdgRenderProcInpainting), "released", G_CALLBACK(setinpainting), NULL );
	g_signal_connect( G_OBJECT(g_wdgRenderProcHHF), "released", G_CALLBACK(sethhf), NULL );

	if(g_renderprocmethod==RENDERPROCNONE)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgRenderProcNone), TRUE);
	else if(g_renderprocmethod==RENDERPROCHOLLFILL)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgRenderProcHollFill), TRUE);
	else if(g_renderprocmethod==RENDERPROCINPAINTING)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgRenderProcInpainting), TRUE);
	else //if(g_renderprocmethod==RENDERPROCHHF)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgRenderProcHHF), TRUE);
	
	hbox=gtk_hbox_new(TRUE,0);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(CONF_RND_ALP);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);	
	pSparameterinput->render_alpha = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(hbox),pSparameterinput->render_alpha,FALSE,FALSE,0);
	sprintf_s(bufferl, "%d", g_renderalpha);
	gtk_entry_set_text(GTK_ENTRY(pSparameterinput->render_alpha), bufferl);
	gtk_entry_set_alignment(GTK_ENTRY(pSparameterinput->render_alpha), 0.5);
	label=gtk_label_new(CONF_RND_ALPV);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);

	hbox=gtk_hbox_new(TRUE,0);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(CONF_RND_FP);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);	
	pSparameterinput->render_focalplane = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(hbox),pSparameterinput->render_focalplane,FALSE,FALSE,0);
	sprintf_s(bufferl, "%d", g_renderforcalplane);
	gtk_entry_set_text(GTK_ENTRY(pSparameterinput->render_focalplane), bufferl);
	gtk_entry_set_alignment(GTK_ENTRY(pSparameterinput->render_focalplane), 0.5);
	label=gtk_label_new(CONF_RND_FPV);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);

	hbox=gtk_hbox_new(TRUE,0);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(CONF_RND_RES);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);	
	g_wdgRenderEdgeSoftenNone= gtk_radio_button_new_with_label(NULL, CONF_RND_NONE);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgRenderEdgeSoftenNone,FALSE,FALSE,0);
	g_wdgRenderEdgeSoftenMean= gtk_radio_button_new_with_label(NULL, CONF_RND_MEAN);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgRenderEdgeSoftenMean,FALSE,FALSE,0);
	g_wdgRenderEdgeSoftenMedian= gtk_radio_button_new_with_label(NULL, CONF_RND_MED);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgRenderEdgeSoftenMedian,FALSE,FALSE,0);
	gtk_radio_button_set_group(GTK_RADIO_BUTTON(g_wdgRenderEdgeSoftenNone),gtk_radio_button_group(GTK_RADIO_BUTTON(g_wdgRenderEdgeSoftenMean)));
	gtk_radio_button_set_group(GTK_RADIO_BUTTON(g_wdgRenderEdgeSoftenMedian),gtk_radio_button_group(GTK_RADIO_BUTTON(g_wdgRenderEdgeSoftenMean)));

	g_signal_connect( G_OBJECT(g_wdgRenderEdgeSoftenNone), "released", G_CALLBACK(setresnone), NULL );
	g_signal_connect( G_OBJECT(g_wdgRenderEdgeSoftenMean), "released", G_CALLBACK(setresmean), NULL );
	g_signal_connect( G_OBJECT(g_wdgRenderEdgeSoftenMedian), "released", G_CALLBACK(setresmedian), NULL );

	if(g_renderedgesoftenmethod==RENDEREDGESOFTENNON)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgRenderEdgeSoftenNone), TRUE);
	else if(g_renderedgesoftenmethod==RENDEREDGESOFTENMEAN)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgRenderEdgeSoftenMean), TRUE);
	else // (g_renderedgesoftenmethod==RENDEREDGESOFTENMID)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgRenderEdgeSoftenMedian), TRUE);
	
	hbox=gtk_hbox_new(TRUE,0);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(CONF_RND_ET);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);	
	pSparameterinput->render_edgesoftenth = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(hbox),pSparameterinput->render_edgesoftenth,FALSE,FALSE,0);
	sprintf_s(bufferl, "%d", g_renderedgesoftenth);
	gtk_entry_set_text(GTK_ENTRY(pSparameterinput->render_edgesoftenth), bufferl);
	gtk_entry_set_alignment(GTK_ENTRY(pSparameterinput->render_edgesoftenth), 0.5);
	label=gtk_label_new(CONF_RND_ETV);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	
	separator = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox),separator,FALSE,FALSE,0);

	label=gtk_label_new(CONF_RND_RFEM);
    gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

	hbox=gtk_hbox_new(TRUE,0);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	
	label=gtk_label_new(CONF_RND_MDL);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);	
	g_wdgRenEditModule0= gtk_radio_button_new_with_label(NULL, CONF_SQUARE);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgRenEditModule0,FALSE,FALSE,0);
	g_wdgRenEditModule1= gtk_radio_button_new_with_label(NULL, CONF_CIRCLE);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgRenEditModule1,FALSE,FALSE,0);

	gtk_radio_button_set_group(GTK_RADIO_BUTTON(g_wdgRenEditModule1),gtk_radio_button_group(GTK_RADIO_BUTTON(g_wdgRenEditModule0)));

	g_signal_connect( G_OBJECT(g_wdgRenEditModule0), "released", G_CALLBACK(setremodule0), NULL );
	g_signal_connect( G_OBJECT(g_wdgRenEditModule1), "released", G_CALLBACK(setremodule1), NULL );

	if(g_rendereditselmodule==RENEDITSELMODESQU)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgRenEditModule0), TRUE);
	else //if(g_segeditmodule==SEGEDITMODULECIR)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgRenEditModule1), TRUE);

	hbox=gtk_hbox_new(TRUE,0);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(CONF_RND_RAD);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);	
	g_wdgRenEditModuleRad = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(hbox),g_wdgRenEditModuleRad,FALSE,FALSE,0);
	sprintf_s(bufferl, "%d", g_rendereditselmodulerad);
	gtk_entry_set_text(GTK_ENTRY(g_wdgRenEditModuleRad), bufferl);
	gtk_entry_set_alignment(GTK_ENTRY(g_wdgRenEditModuleRad), 0.5);
	
	label=gtk_label_new(CONF_RND_RADV);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);

	hbox=gtk_hbox_new(TRUE,0);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(CONF_RND_RRR);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);	
	g_wdgRenEditRefRegionRad = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(hbox),g_wdgRenEditRefRegionRad,FALSE,FALSE,0);
	sprintf_s(bufferl, "%d", g_rendereditrefregionrad);
	gtk_entry_set_text(GTK_ENTRY(g_wdgRenEditRefRegionRad), bufferl);
	gtk_entry_set_alignment(GTK_ENTRY(g_wdgRenEditRefRegionRad), 0.5);
	
	label=gtk_label_new(CONF_RND_RRRV);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);

	hbox=gtk_hbox_new(TRUE,0);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	
	label=gtk_label_new(CONF_RND_MM);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);	
	
	g_wdgRenEditMatchModule0 = gtk_radio_button_new_with_label(NULL, "7x7");
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgRenEditMatchModule0,FALSE,FALSE,0);
	g_wdgRenEditMatchModule1 = gtk_radio_button_new_with_label(NULL, "9x9");
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgRenEditMatchModule1,FALSE,FALSE,0);
	g_wdgRenEditMatchModule2 = gtk_radio_button_new_with_label(NULL, "11x11");
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgRenEditMatchModule2,FALSE,FALSE,0);

	gtk_radio_button_set_group(GTK_RADIO_BUTTON(g_wdgRenEditMatchModule0),gtk_radio_button_group(GTK_RADIO_BUTTON(g_wdgRenEditMatchModule1)));
	gtk_radio_button_set_group(GTK_RADIO_BUTTON(g_wdgRenEditMatchModule2),gtk_radio_button_group(GTK_RADIO_BUTTON(g_wdgRenEditMatchModule1)));

	g_signal_connect( G_OBJECT(g_wdgRenEditMatchModule0), "released", G_CALLBACK(setremode0), NULL );
	g_signal_connect( G_OBJECT(g_wdgRenEditMatchModule1), "released", G_CALLBACK(setremode1), NULL );
	g_signal_connect( G_OBJECT(g_wdgRenEditMatchModule2), "released", G_CALLBACK(setremode2), NULL );

	if(g_rendereditmatchmodule==RENEDITMATCHMOD7X7)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgRenEditMatchModule0), TRUE);
	else if(g_rendereditmatchmodule==RENEDITMATCHMOD9X9)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgRenEditMatchModule1), TRUE);
	else // (g_rendereditmatchmodule==RENEDITMATCHMODBXB)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgRenEditMatchModule2), TRUE);

	hbox=gtk_hbox_new(TRUE,0);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	
	label=gtk_label_new(CONF_RND_SP);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);	
	g_wdgRenEditStartP0 = gtk_radio_button_new_with_label(NULL, CONF_RND_SPS);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgRenEditStartP0,FALSE,FALSE,0);
	g_wdgRenEditStartP1 = gtk_radio_button_new_with_label(NULL, CONF_RND_SPM);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgRenEditStartP1,FALSE,FALSE,0);

	gtk_radio_button_set_group(GTK_RADIO_BUTTON(g_wdgRenEditStartP0),gtk_radio_button_group(GTK_RADIO_BUTTON(g_wdgRenEditStartP1)));

	g_signal_connect( G_OBJECT(g_wdgRenEditStartP0), "released", G_CALLBACK(setrestartp0), NULL );
	g_signal_connect( G_OBJECT(g_wdgRenEditStartP1), "released", G_CALLBACK(setrestartp1), NULL );

	if(g_rendereditstartpoint==RENEDITSELSPSTRU)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgRenEditStartP0), TRUE);
	else //if(g_rendereditstartpoint==RENEDITSELSPMENU)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgRenEditStartP1), TRUE);
	
	separator = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox),separator,FALSE,FALSE,0);

	/* ok and cancel */
    gtk_notebook_set_current_page (GTK_NOTEBOOK (notebook), 0);

    button_ok = gtk_button_new_with_label (CONF_OK);
    g_signal_connect_swapped (G_OBJECT (button_ok), "clicked",
                           G_CALLBACK (get_setting_value), NULL);
	    gtk_table_attach_defaults (GTK_TABLE (table), button_ok, 1, 2, 5, 6);
   
    button_cancel = gtk_button_new_with_label (CONF_CANCEL);
    g_signal_connect_swapped (G_OBJECT (button_cancel), "clicked",
                           G_CALLBACK (gtk_widget_hide),
                           g_wdgParamSetting);
    gtk_table_attach_defaults (GTK_TABLE (table), button_cancel, 4, 5, 5, 6);

    gtk_widget_show_all (g_wdgParamSetting);

	return ;
}

GtkWidget* create_file_item()
{
    GtkWidget *file_menu;    //定义菜单
    GtkWidget *file_item;
	GtkWidget *newproject, *openproject, *saveproject, *closeproject, *exitproject, *separator;

    //-------------------创建file菜单----------------------------------
    file_menu=gtk_menu_new();                //创建菜单
    /* 创建菜单项 */
    newproject=gtk_menu_item_new_with_mnemonic(MENU_FILE_NEWPROJ);
    openproject = gtk_menu_item_new_with_mnemonic (MENU_FILE_OPENPROJ);
	saveproject = gtk_menu_item_new_with_mnemonic (MENU_FILE_SAVEPROJ);
	closeproject = gtk_menu_item_new_with_mnemonic (MENU_FILE_CLOSEPROJ);
	exitproject = gtk_menu_item_new_with_mnemonic (MENU_FILE_EXIT);
	separator = gtk_separator_menu_item_new();

    /* 将它们加到菜单中 */
    gtk_menu_append(GTK_MENU(file_menu),newproject);
    gtk_menu_append (GTK_MENU (file_menu),openproject);
	gtk_menu_append (GTK_MENU (file_menu),saveproject);
    gtk_menu_append (GTK_MENU (file_menu),closeproject);
	gtk_menu_append (GTK_MENU (file_menu),separator);  
	gtk_menu_append (GTK_MENU (file_menu),exitproject);  
    
    /* 将回调函数绑定到activate信号 */
    g_signal_connect_swapped(G_OBJECT(newproject),"activate",G_CALLBACK(newprojectfile),NULL);
	g_signal_connect_swapped (G_OBJECT(openproject), "activate",G_CALLBACK (openprojectfile),NULL);
	g_signal_connect_swapped (G_OBJECT(saveproject), "activate",G_CALLBACK (saveprojectfile),NULL);
	g_signal_connect_swapped (G_OBJECT(closeproject), "activate",G_CALLBACK (closeprojectfile),NULL);
	g_signal_connect_swapped (G_OBJECT(exitproject), "activate",G_CALLBACK (delete_event_fun),NULL);

    file_item = gtk_menu_item_new_with_mnemonic(MENU_FILE);
    //gtk_widget_set_size_request(GTK_WIDGET(file_item),50,20);
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (file_item), file_menu);

    return file_item;
}

GtkWidget* create_edit_item()
{
    GtkWidget *edit_menu;    //定义菜单
    GtkWidget *edit_item;
    GtkWidget *undo_item,*redo_item, *selkey_item, *selsec_item, *cutseq_item, *opt_item, *separator;    //edit 菜单下的菜单项

    //-------------------创建tool菜单----------------------------------
    edit_menu=gtk_menu_new();                //创建菜单
    /* 创建菜单项 */
    undo_item=gtk_menu_item_new_with_mnemonic(MENU_EDIT_UNDO);
	redo_item=gtk_menu_item_new_with_mnemonic(MENU_EDIT_REDO);
	selkey_item=gtk_menu_item_new_with_mnemonic(MENU_EDIT_SWKEYMODE);
	selsec_item=gtk_menu_item_new_with_mnemonic(MENU_EDIT_SECSEL);
	cutseq_item=gtk_menu_item_new_with_mnemonic(MENU_EDIT_SEQCUT);
	opt_item = gtk_menu_item_new_with_mnemonic (MENU_EDIT_OPTION);

	separator = gtk_separator_menu_item_new();
    
    /* 将它们加到菜单中 */
    gtk_menu_append(GTK_MENU (edit_menu),undo_item);
	gtk_menu_append(GTK_MENU (edit_menu),redo_item);
	gtk_menu_append(GTK_MENU (edit_menu),separator);
	gtk_menu_append(GTK_MENU (edit_menu),selkey_item);
	gtk_menu_append(GTK_MENU (edit_menu),selsec_item);
	gtk_menu_append(GTK_MENU (edit_menu),cutseq_item);
	//gtk_menu_append(GTK_MENU (edit_menu),separator);
	gtk_menu_append(GTK_MENU (edit_menu),opt_item);
    
    /* 将回调函数绑定到activate信号 */
    g_signal_connect_swapped(G_OBJECT(undo_item),"activate",G_CALLBACK(loadsegmenttmp),NULL);
	g_signal_connect_swapped(G_OBJECT(redo_item),"activate",G_CALLBACK(loadsegmenttmp1),NULL);
	g_signal_connect_swapped(G_OBJECT(selkey_item),"activate",G_CALLBACK(swchkeyframemode),NULL);
	g_signal_connect_swapped(G_OBJECT(selsec_item),"activate",G_CALLBACK(selsection),NULL);
	g_signal_connect_swapped(G_OBJECT(cutseq_item),"activate",G_CALLBACK(sequencecut),NULL);
	g_signal_connect_swapped (G_OBJECT(opt_item), "activate", G_CALLBACK (parametersetting),NULL);   

    edit_item = gtk_menu_item_new_with_mnemonic(MENU_EDIT);
    //gtk_widget_set_size_request(GTK_WIDGET(file_item),50,20);
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (edit_item), edit_menu);

    return edit_item;

}

GtkWidget* create_segment_item()
{
    GtkWidget *seg_menu;    //定义菜单
    GtkWidget *seg_item;
    GtkWidget *segment_item,*check_item,*auto_item, *comb_item, *remain_item, *base_item, *bases_item, *background_item,*infe_item, *separator, *separator1;    //edit 菜单下的菜单项

    //-------------------创建tool菜单----------------------------------
    seg_menu=gtk_menu_new();                //创建菜单
    /* 创建菜单项 */
    segment_item=gtk_menu_item_new_with_mnemonic(MENU_SEGM_SEGFRMS);
	check_item=gtk_menu_item_new_with_mnemonic(MENU_SEGM_CHECK);
	auto_item=gtk_menu_item_new_with_mnemonic(MENU_SEGM_AUTOREF);
	comb_item=gtk_menu_item_new_with_mnemonic(MENU_SEGM_COMVISLAY);
	remain_item=gtk_menu_item_new_with_mnemonic(MENU_SEGM_SETREMLAY);
	base_item=gtk_menu_item_new_with_mnemonic(MENU_SEGM_SETBSLAY);
	bases_item=gtk_menu_item_new_with_mnemonic(MENU_SEGM_SETBSLAYS);
	background_item=gtk_menu_item_new_with_mnemonic(MENU_SEGM_BGGEN);
	infe_item = gtk_menu_item_new_with_mnemonic (MENU_SEGM_INF);

	separator = gtk_separator_menu_item_new();
	separator1 = gtk_separator_menu_item_new();
    
    /* 将它们加到菜单中 */
    gtk_menu_append(GTK_MENU (seg_menu),segment_item);
	gtk_menu_append(GTK_MENU (seg_menu),check_item);
	gtk_menu_append(GTK_MENU (seg_menu),auto_item);
	gtk_menu_append(GTK_MENU (seg_menu),separator);
	gtk_menu_append(GTK_MENU (seg_menu),comb_item);
	gtk_menu_append(GTK_MENU (seg_menu),remain_item);
	gtk_menu_append(GTK_MENU (seg_menu),base_item);
	gtk_menu_append(GTK_MENU (seg_menu),bases_item);
	gtk_menu_append(GTK_MENU (seg_menu),background_item);
	gtk_menu_append(GTK_MENU (seg_menu),separator1);
	gtk_menu_append(GTK_MENU (seg_menu),infe_item);
    
    /* 将回调函数绑定到activate信号 */
    g_signal_connect_swapped(G_OBJECT(segment_item),"activate",G_CALLBACK(getsegmentframe),NULL);
	g_signal_connect_swapped(G_OBJECT(check_item),"activate",G_CALLBACK(SegmentCheck),NULL);
	g_signal_connect_swapped(G_OBJECT(auto_item),"activate",G_CALLBACK(Automenuseg),NULL);
	g_signal_connect_swapped(G_OBJECT(comb_item),"activate",G_CALLBACK(CombineVisibleLayer),NULL);
	g_signal_connect_swapped(G_OBJECT(remain_item),"activate",G_CALLBACK(setremainlayer),NULL);
	g_signal_connect_swapped(G_OBJECT(base_item),"activate",G_CALLBACK(setbaselayer),NULL);
	g_signal_connect_swapped(G_OBJECT(bases_item),"activate",G_CALLBACK(getbaselayers),NULL);
	g_signal_connect_swapped(G_OBJECT(background_item),"activate",G_CALLBACK(getbackgroundframe),NULL);
	g_signal_connect_swapped (G_OBJECT(infe_item), "activate", G_CALLBACK (getrefframe),NULL);   

    seg_item = gtk_menu_item_new_with_mnemonic(MENU_SEGM);
    //gtk_widget_set_size_request(GTK_WIDGET(file_item),50,20);
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (seg_item), seg_menu);

    return seg_item;

}

GtkWidget* create_depth_item()
{
    GtkWidget *dep_menu;    //定义菜单
    GtkWidget *dep_item;
    GtkWidget *fmodule_item, *baseassign_item, *auto_item, *auto_assign_item, *proc_item,*infe_item, *layerinfe_item, *autoinfe_item, *separator;    //edit 菜单下的菜单项

    //-------------------创建tool菜单----------------------------------
    dep_menu=gtk_menu_new();                //创建菜单
    /* 创建菜单项 */
	fmodule_item=gtk_menu_item_new_with_mnemonic(MENU_DEPT_FRMDEPMOD);
	baseassign_item=gtk_menu_item_new_with_mnemonic(MENU_DEPT_BSDEPASSG);
    auto_item=gtk_menu_item_new_with_mnemonic(MENU_DEPT_AUTOGEN);
	auto_assign_item=gtk_menu_item_new_with_mnemonic(MENU_DEPT_AUTOASSG);
	proc_item=gtk_menu_item_new_with_mnemonic(MENU_DEPT_PROC);
	infe_item=gtk_menu_item_new_with_mnemonic(MENU_DEPT_FRMINF);
	layerinfe_item = gtk_menu_item_new_with_mnemonic(MENU_DEPT_LAYINF);
	autoinfe_item = gtk_menu_item_new_with_mnemonic(MENU_DEPT_AUTOINF);

	separator = gtk_separator_menu_item_new();
    
    /* 将它们加到菜单中 */
	gtk_menu_append(GTK_MENU (dep_menu),fmodule_item);
	gtk_menu_append(GTK_MENU (dep_menu),baseassign_item);
    gtk_menu_append(GTK_MENU (dep_menu),auto_item);
	gtk_menu_append(GTK_MENU (dep_menu),auto_assign_item);
	gtk_menu_append(GTK_MENU (dep_menu),proc_item);
	gtk_menu_append(GTK_MENU (dep_menu),separator);
	gtk_menu_append(GTK_MENU (dep_menu),infe_item);
	gtk_menu_append(GTK_MENU (dep_menu),layerinfe_item);
	gtk_menu_append(GTK_MENU (dep_menu),autoinfe_item);
    
    /* 将回调函数绑定到activate信号 */
	g_signal_connect_swapped(G_OBJECT(fmodule_item),"activate",G_CALLBACK(getframemoduleframe),NULL);
	g_signal_connect_swapped(G_OBJECT(baseassign_item),"activate",G_CALLBACK(getbaseassignframe),NULL);
    g_signal_connect_swapped(G_OBJECT(auto_item),"activate",G_CALLBACK(getdepautogenframe),NULL);
	g_signal_connect_swapped(G_OBJECT(auto_assign_item),"activate",G_CALLBACK(depthautoassign),NULL);
	g_signal_connect_swapped(G_OBJECT(proc_item),"activate",G_CALLBACK(getdepprocframe),NULL);
	g_signal_connect_swapped(G_OBJECT(infe_item),"activate",G_CALLBACK(getdepinfeframe),NULL);
	g_signal_connect_swapped (G_OBJECT(layerinfe_item), "activate", G_CALLBACK (getlayerdepinfeframe),NULL);   
	g_signal_connect_swapped (G_OBJECT(autoinfe_item), "activate", G_CALLBACK (getautodepinfeframe),NULL);  

    dep_item = gtk_menu_item_new_with_mnemonic (MENU_DEPT);
    //gtk_widget_set_size_request(GTK_WIDGET(file_item),50,20);
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (dep_item), dep_menu);

    return dep_item;

}

GtkWidget* create_render_item()
{
    GtkWidget *render_menu;    //定义菜单
    GtkWidget *render_item;
    GtkWidget *frame_item,*yuv_item,*h264_item, *autotrans_item;    //edit 菜单下的菜单项

    //-------------------创建tool菜单----------------------------------
    render_menu=gtk_menu_new();                //创建菜单
    /* 创建菜单项 */
    frame_item=gtk_menu_item_new_with_mnemonic(MENU_REND_REND);
	yuv_item=gtk_menu_item_new_with_mnemonic(MENU_REND_TOYUV);
	h264_item=gtk_menu_item_new_with_mnemonic(MENU_REND_TOH264);
	autotrans_item = gtk_menu_item_new_with_mnemonic(MENU_REND_AUTOTRANS);

    /* 将它们加到菜单中 */
    gtk_menu_append(GTK_MENU (render_menu),frame_item);
	gtk_menu_append(GTK_MENU (render_menu),yuv_item);
	gtk_menu_append(GTK_MENU (render_menu),h264_item);
	gtk_menu_append(GTK_MENU (render_menu),autotrans_item);
    
    /* 将回调函数绑定到activate信号 */
    g_signal_connect_swapped(G_OBJECT(frame_item),"activate",G_CALLBACK(rendercurframe),NULL);
	g_signal_connect_swapped(G_OBJECT(yuv_item),"activate",G_CALLBACK(getrenderframes),NULL);
	g_signal_connect_swapped(G_OBJECT(h264_item),"activate",G_CALLBACK(renderframes2h264),NULL);
	g_signal_connect_swapped(G_OBJECT(autotrans_item),"activate",G_CALLBACK(autotranscode),NULL);

    render_item = gtk_menu_item_new_with_mnemonic (MENU_REND);
    //gtk_widget_set_size_request(GTK_WIDGET(file_item),50,20);
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (render_item), render_menu);

    return render_item;

}


GtkWidget* create_help_item()
{
    GtkWidget *help_menu;    //定义菜单
    GtkWidget *help_item;
    GtkWidget *hlp_item,*log_item,*about_item;    //help 菜单下的菜单项

    //-------------------创建help菜单----------------------------------
    help_menu=gtk_menu_new();                //创建菜单
    /* 创建菜单项 */
    hlp_item=gtk_menu_item_new_with_mnemonic(MENU_HELP_HELP);
	log_item = gtk_menu_item_new_with_mnemonic (MENU_HELP_LOG);
    about_item = gtk_menu_item_new_with_mnemonic(MENU_HELP_ABOUT);

    /* 将它们加到菜单中 */
    gtk_menu_append(GTK_MENU(help_menu),hlp_item);
	gtk_menu_append (GTK_MENU (help_menu),log_item);
    gtk_menu_append (GTK_MENU (help_menu),about_item);
    
    /* 将回调函数绑定到activate信号 */
    g_signal_connect_swapped(G_OBJECT(hlp_item),"activate",G_CALLBACK(helpfile),NULL);
	g_signal_connect_swapped (G_OBJECT(log_item), "activate",G_CALLBACK (loginfo),NULL);
	g_signal_connect_swapped (G_OBJECT(about_item), "activate",G_CALLBACK (aboutinfo),NULL);

    help_item = gtk_menu_item_new_with_mnemonic(MENU_HELP);
    //gtk_widget_set_size_request(GTK_WIDGET(file_item),50,20);
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (help_item), help_menu);

    return help_item;


}

void set_resolution(GtkWidget * button,GtkWidget * data)
{
	char width[8];
	char height[8];
	char *swidth = width;
	char *sheight = height;

	//printf("resolution: %d x %d\n", g_frameWidth, g_frameHeight);	
	if(g_frameInWidth>4096)
	{
		g_frameInWidth=4096;
	}
	if(g_frameInWidth<0)
	{
		g_frameInWidth=4;
	}
	if(g_frameInHeight>4096)
	{
		g_frameInHeight=4096;
	}
	if(g_frameInHeight<0)
	{
		g_frameInHeight=4;
	}

	uint8_t * tmp = new uint8_t [IMGWIDTHMAX*IMGHEIGHTMAX*4];

	//背景颜色为黑色
	for(int i=0;i<IMGWIDTHMAX*IMGHEIGHTMAX*4;i++)
		tmp[i] = 238;
	gdk_draw_gray_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
          0, 0, IMGWIDTHMAX*2, IMGHEIGHTMAX*2,
          GDK_RGB_DITHER_MAX, tmp, IMGWIDTHMAX*2);
	if(tmp) delete [] tmp;

	return ;
}

int CoordPosTrans(int xi, int yi, int * xo, int * yo)
{
	int region=0;
	if(g_enlargeflag==0)
	{
		/*原始区*/
		if((xi>=IMGCENTERX-g_frameWidth)&&(xi<IMGCENTERX)&&(yi>=IMGCENTERY-g_frameHeight)&&(yi<IMGCENTERY))
		{
			region = REGIONORG;
			*xo = xi+g_frameWidth-IMGCENTERX;
			*yo = yi+g_frameHeight-IMGCENTERY;
		}
		/*分割区*/
		else if((xi>=IMGCENTERX)&&(xi<IMGCENTERX+g_frameWidth)&&(yi>=IMGCENTERY-g_frameHeight)&&(yi<IMGCENTERY))
		{
			region = REGIONSEG;
			*xo = xi-IMGCENTERX;
			*yo = yi+g_frameHeight-IMGCENTERY;
		}
		/*手工设置区*/
		else if((xi>=IMGCENTERX-g_frameWidth)&&(xi<IMGCENTERX)&&(yi>=IMGCENTERY)&&(yi<IMGCENTERY+g_frameHeight))
		{
			region = REGIONMAN;
			*xo = xi+g_frameWidth-IMGCENTERX;
			*yo = yi-IMGCENTERY;
		}
		/*深度区*/
		else if((xi>=IMGCENTERX)&&(xi<IMGCENTERX+g_frameWidth)&&(yi>=IMGCENTERY)&&(yi<IMGCENTERY+g_frameHeight))
		{
			region = REGIONDEP;
			*xo = xi-IMGCENTERX;
			*yo = yi-IMGCENTERY;
		}
		else
		{
			*xo=0;
			*yo=0;
			region = 0;
		}
	}
	else
	{
		if(g_frameResizeFlag==0)
		{
			if((xi>=IMGCENTERX-g_frameWidth)&&(xi<IMGCENTERX+g_frameWidth)&&(yi>=IMGCENTERY-g_frameHeight)&&(yi<IMGCENTERY+g_frameHeight))
			{
				*xo = (xi - (IMGCENTERX-g_frameWidth))>>1;
				*yo = (yi - (IMGCENTERY-g_frameHeight))>>1;
				region = REGIONORG;
			}
			else
			{
				*xo=0;
				*yo=0;
				region = 0;
			}
		}
		else
		{
			if((xi>=IMGCENTERX-g_frameWidth)&&(xi<IMGCENTERX+g_frameWidth)&&(yi>=IMGCENTERY-g_frameHeight)&&(yi<IMGCENTERY+g_frameHeight))
			{
				*xo = xi - (IMGCENTERX-g_frameWidth) + g_enlargeorgx;
				*yo = yi - (IMGCENTERY-g_frameHeight) + g_enlargeorgy;
				region = REGIONORG;
			}
			else
			{
				*xo=0;
				*yo=0;
				region = 0;
			}
		}
	}

	return region;
}


/*鼠标点击事件*/
bool
scribble_button_press_event (GtkWidget *widget,
		GdkEventButton *event,
		gpointer data)
{
	int px, py;
	int epx, epy;
	int lb;
	int olb, nlb;
	int sizeorg = g_frameInWidth*g_frameInHeight;
	int size = g_frameWidth*g_frameHeight;
	int mask;
	int region=0;
	int rad;
	GdkModifierType state;

	if(g_frameMaxNum>0&&g_frames[g_frameIndex].SegFlag==1)
	{
		mask = g_frames[g_frameIndex].SegLabelMask;
		gdk_window_get_pointer (event->window, &px, &py, &state);
		
		if (event->button == 1)
		{
			/*双击*/
			if(event->type == GDK_2BUTTON_PRESS)
			{
				//px = (int)event->x;
				//py = (int)event->y;

				region = CoordPosTrans(px, py, &epx, &epy);

				// Render 匹配
				if(g_renderflag==1)
				{
					if(g_rendereditflag==1&&g_renderoutputformat==RENDEROUTLR)
						RenderBlockMatching(epx, epy, region);
					/*深度区*/
					if(region==REGIONDEP)
					{
						lb = g_frames[g_frameIndex].SegLabels[epx+epy*g_frameWidth];
						/*双击设置深度*/
						if(lb>size&&g_depeditflag==0)//event->type == GDK_2BUTTON_PRESS&&
						{
							Setdepth(lb);
							/*画出深度图*/
							//ShowDep(g_frames[g_frameIndex].SegLabelMask);
							g_changeflag = 1;
						}
						else if(g_depeditflag==1) //event->type == GDK_2BUTTON_PRESS&&
						{
							if(g_frameResizeFlag==1)
							{
								rad = g_enlargeflag==0?g_depeditmodulerad*g_frameInWidth/g_frameWidth:g_depeditmodulerad;
								DepthFineEdit(g_frames[g_frameIndex].SegDepths0, g_frames[g_frameIndex].SegLabels0, lb, rad, epx*g_frameInWidth/g_frameWidth, epy*g_frameInHeight/g_frameHeight, g_frameInWidth, g_frameInHeight);
								yresize(g_SegDepths0,g_SegDepths,g_frameInWidth,g_frameInHeight,g_frameWidth,g_frameHeight);
							}
							else
							{
								rad = g_depeditmodulerad;
								DepthFineEdit(g_frames[g_frameIndex].SegDepths, g_frames[g_frameIndex].SegLabels, lb, rad, epx, epy, g_frameWidth, g_frameHeight);
							}
							ShowDep(g_frames[g_frameIndex].SegLabelMask);
							g_changeflag = 1;
						}
					}
				}
				else
				{
					if(event->state&GDK_CONTROL_MASK)
					{
						// TODO 可以添加边缘扩散代码
					}
					else
					{
						if(g_enlargeflag==0)
						{
							/*分割区双击放大或缩小*/ 
							if((g_frames[g_frameIndex].SegFlag==1)&&region>=REGIONORG&&region<=REGIONMAN&&g_depeditflag==0)
							{
								//printf(" g_enlargeflag == 0\n");
								if(g_frameResizeFlag==0)
								{
									// TODO 打开某一帧时只做一次即可
									InterpImageBilinear(g_frames[g_frameIndex].rgbinput, g_enlargedisplay, g_frameWidth, g_frameHeight, g_frameWidth*2, g_frameHeight*2);
								}
								else
								{
									// TODO g_frameInWidth<2*g_frameWidth的情况
									g_enlargeorgx = epx*g_frameInWidth/g_frameWidth - g_frameWidth;
									if(g_enlargeorgx<0)
										g_enlargeorgx = 0;
									if(g_enlargeorgx+2*g_frameWidth>g_frameInWidth)
										g_enlargeorgx = g_frameInWidth-2*g_frameWidth;
									g_enlargeorgy = epy*g_frameInHeight/g_frameHeight- g_frameHeight;
									if(g_enlargeorgy<0)
										g_enlargeorgy = 0;
									if(g_enlargeorgy+2*g_frameHeight>g_frameInHeight)
										g_enlargeorgy = g_frameInHeight-2*g_frameHeight;
								}
								g_enlargeflag = 1;
								// 显示放大后的分割效果
								SegEnlargedisplay(g_frames[g_frameIndex].SegLabelMask);
								
							}
							/*深度区*/
							if(region==REGIONDEP)
							{
								lb = g_frames[g_frameIndex].SegLabels[epx+epy*g_frameWidth];
								/*双击设置深度*/
								if(lb>size&&g_depeditflag==0)//event->type == GDK_2BUTTON_PRESS&&
								{
									Setdepth(lb);
									/*画出深度图*/
									//ShowDep(g_frames[g_frameIndex].SegLabelMask);
									g_changeflag = 1;
								}
							}
							/* 深度编辑*/
							if(g_depeditflag==1) //event->type == GDK_2BUTTON_PRESS&&
							{
								lb = g_frames[g_frameIndex].SegLabels[epx+epy*g_frameWidth];
								if(g_frameResizeFlag==1)
								{
									rad = g_depeditmodulerad*g_frameInWidth/g_frameWidth;
									DepthFineEdit(g_frames[g_frameIndex].SegDepths0, g_frames[g_frameIndex].SegLabels0, lb, rad, epx*g_frameInWidth/g_frameWidth, epy*g_frameInHeight/g_frameHeight, g_frameInWidth, g_frameInHeight);
									yresize(g_SegDepths0,g_SegDepths,g_frameInWidth,g_frameInHeight,g_frameWidth,g_frameHeight);
								}
								else
								{
									rad = g_depeditmodulerad;
									DepthFineEdit(g_frames[g_frameIndex].SegDepths, g_frames[g_frameIndex].SegLabels, lb, rad, epx, epy, g_frameWidth, g_frameHeight);
								}
								ShowDep(g_frames[g_frameIndex].SegLabelMask);
								g_changeflag = 1;
							}
						}
						else // g_enlargeflag == 1
						{
							//printf(" g_enlargeflag == 1\n");
							if(g_enlargeswitchflag==0)
							{
								g_enlargeflag = 0;
								g_enlargeorgx = 0;
								g_enlargeorgy = 0;
								//gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
						          //    IMGWIDTHMAX, IMGHEIGHTMAX-g_frameHeight, g_frameWidth, g_frameHeight,
						            //  GDK_RGB_DITHER_MAX, g_autosegdisplay, g_frameWidth * 3);
								ShowAll();
							}
							else
							{
								lb = g_frames[g_frameIndex].SegLabels0[epx+epy*g_frameInWidth];
								/*双击设置深度*/
								if(lb>size&&g_depeditflag==0)//event->type == GDK_2BUTTON_PRESS&&
								{
									Setdepth(lb);
									/*画出深度图*/
									ShowEnlargeDep(g_frames[g_frameIndex].SegLabelMask);
									g_changeflag = 1;
								}
								else if(g_depeditflag==1) //event->type == GDK_2BUTTON_PRESS&&
								{
									rad = g_depeditmodulerad;
									DepthFineEdit(g_frames[g_frameIndex].SegDepths0, g_frames[g_frameIndex].SegLabels0, lb, rad, epx, epy, g_frameInWidth, g_frameInHeight);
									yresize(g_SegDepths0,g_SegDepths,g_frameInWidth,g_frameInHeight,g_frameWidth,g_frameHeight);
									ShowEnlargeDep(g_frames[g_frameIndex].SegLabelMask);
									g_changeflag = 1;
								}
							}
							//g_enlargeflag = 0;
						}
					}
				}
			}
			else// 单击
			{
				//px = (int)event->x;
				//py = (int)event->y;
				
				region = CoordPosTrans(px, py, &epx, &epy);

				if(region>=REGIONORG)
				{

					if(event->state&GDK_CONTROL_MASK&&g_renderflag==0) // 按住ctrl键
					{
						if(g_frameResizeFlag==0||g_enlargeflag==0)
							lb = g_frames[g_frameIndex].SegLabels[epx+epy*g_frameWidth];
						else
							lb = g_frames[g_frameIndex].SegLabels0[epx+epy*g_frameInWidth];
						/* 边缘扩散 */
						if(((lb-size)&&mask)!=0)
						{
							if(g_frameResizeFlag==0)
							{
								SegEdgeEnlarge(g_frames[g_frameIndex].SegLabels,lb, epx, epy);
								ShowSegandDep(mask);
							}
							else
							{
								if(g_enlargeflag==0)
								{
									// 低分辨率模式下不起作用(原则: 操作都在高分辨率下进行)
									//SegEdgeEnlarge(g_frames[g_frameIndex].SegLabels,lb, epx, epy);
									//SegEnlargedisplay(mask);
								}
								else
								{
									SegEdgeEnlarge0(g_frames[g_frameIndex].SegLabels0,lb, epx, epy);
									SegEnlargedisplay(mask);
								}
							}
							g_changeflag = 1;
						}
					}
					
					/* 这两个条件保证只有一层使能情况才开启编辑功能*/
					//if((mask!=0) && ((mask&(mask-1))==0) &&g_renderflag==0) 
					if((g_frames[g_frameIndex].LayerEditNum>0)&&(g_frames[g_frameIndex].LayerEditNum<=LAYERBUTTONNUMMAX)&&g_renderflag==0)
					{
						if(g_enlargeflag==0)
						{			
							lb = g_frames[g_frameIndex].SegLabels[epx+epy*g_frameWidth];
							nlb = size+(1<<g_frames[g_frameIndex].LayerEditNum);
							#if 0
							if(event->state&GDK_CONTROL_MASK)
							{
								// 边缘扩散
								//if(g_segeditflag==0)
								if(lb==g_frames[g_frameIndex].SegLabel)
								{
									SegEdgeEnlarge(g_frames[g_frameIndex].SegLabels,lb, epx, epy);
									/*画出分割图*/
									Drawlabel(g_frames[g_frameIndex].SegLabels, g_frames[g_frameIndex].SegLabel);
									/*画出深度图*/
									Drawdepth(g_frames[g_frameIndex].SegLabels, g_frames[g_frameIndex].SegLabel);
								}
							}
							else
							#endif
							{
								if(lb!=0&&lb<size&&g_segeditflag==0&&g_mateditflag==0/*g_frames[g_frameIndex].SegLabel!=0&&lb!=g_frames[g_frameIndex].SegLabel*/)
								{
									/*设置新标号*/
									Setlabel(g_frames[g_frameIndex].SegLabels, lb, nlb, size);
									if(g_frameResizeFlag==1)
										Setlabel(g_frames[g_frameIndex].SegLabels0, lb, nlb, sizeorg);
									/*画出分割图和深度图*/
									ShowSegandDep(g_frames[g_frameIndex].SegLabelMask);
									g_changeflag = 1;
								}
								if(g_segeditflag==1)
								{
									if(g_frameResizeFlag==0)
										SegEditLabel(g_frames[g_frameIndex].SegLabels, nlb, epx, epy);
									else
										SegEditLabel0(g_frames[g_frameIndex].SegLabels0, nlb, epx*g_frameInWidth/g_frameWidth, epy*g_frameInHeight/g_frameHeight);
									/*画出分割图和深度图*/
									ShowSegandDep(g_frames[g_frameIndex].SegLabelMask);
									g_changeflag = 1;
								}
								if(g_mateditflag==1)
								{
									if(g_frameResizeFlag==0)
										SegMattingLabel(g_frames[g_frameIndex].SegLabels, g_frames[g_frameIndex].OrgLabels, epx, epy, g_frameWidth, g_frameHeight);
									else
										SegMattingLabel0(g_frames[g_frameIndex].SegLabels0, g_frames[g_frameIndex].OrgLabels0, epx*g_frameInWidth/g_frameWidth, epy*g_frameInHeight/g_frameHeight, g_frameInWidth, g_frameInHeight);
									/*画出分割图和深度图*/
									ShowSegandDep(g_frames[g_frameIndex].SegLabelMask);
									g_changeflag = 1;
								}
							}
						}
						else // g_enlargeflag = 1
						{
							//if((mask!=0) && ((mask&(mask-1))==0) &&g_renderflag==0)
							//if((g_frames[g_frameIndex].LayerEditNum>0)&&(g_frames[g_frameIndex].LayerEditNum<=LAYERBUTTONNUMMAX) &&g_renderflag==0)
							//{
							if(g_frameResizeFlag==1)
								lb = g_frames[g_frameIndex].SegLabels0[epx+epy*g_frameInWidth];
							else
								lb = g_frames[g_frameIndex].SegLabels[epx+epy*g_frameWidth];
							nlb = size+(1<<g_frames[g_frameIndex].LayerEditNum);
							#if 0
							if(event->state&GDK_CONTROL_MASK)
							{
								// 边缘扩散
								if(lb==g_frames[g_frameIndex].SegLabel)
								{
									SegEdgeEnlarge(g_frames[g_frameIndex].SegLabels,lb, epx, epy);
									/*画出分割图*/
									DrawEnlargelabel(g_frames[g_frameIndex].SegLabels, g_frames[g_frameIndex].SegLabel);
								}
							}
							else
							#endif
							{
								if(lb!=0&&lb<size&&g_segeditflag==0&&g_mateditflag==0/*g_frames[g_frameIndex].SegLabel!=0&&lb!=g_frames[g_frameIndex].SegLabel*/)
								{
									/*设置新标号*/
									Setlabel(g_frames[g_frameIndex].SegLabels, lb, nlb, size);
									if(g_frameResizeFlag==1)
										Setlabel(g_frames[g_frameIndex].SegLabels0, lb, nlb, sizeorg);
									/*画出分割图*/
									SegEnlargedisplay(g_frames[g_frameIndex].SegLabelMask);
									g_changeflag = 1;
								}
								if(g_segeditflag==1)
								{
									if(g_frameResizeFlag==0)
										SegEditLabel(g_frames[g_frameIndex].SegLabels, nlb, epx, epy);
									else
										SegEditLabel0(g_frames[g_frameIndex].SegLabels0, nlb, epx, epy);
									/*画出分割图*/
									SegEnlargedisplay(g_frames[g_frameIndex].SegLabelMask);
									g_changeflag = 1;
								}
								if(g_mateditflag==1)
								{
									if(g_frameResizeFlag==0)
										SegMattingLabel(g_frames[g_frameIndex].SegLabels, g_frames[g_frameIndex].OrgLabels, epx, epy, g_frameWidth, g_frameHeight);
									else
										SegMattingLabel0(g_frames[g_frameIndex].SegLabels0, g_frames[g_frameIndex].OrgLabels0, epx, epy, g_frameInWidth, g_frameInHeight);
									/*画出分割图*/
									SegEnlargedisplay(g_frames[g_frameIndex].SegLabelMask);
									g_changeflag = 1;
								}
							}
							//}
						}
					}
					// render精细编辑模式
					if(g_renderflag==1&&g_rendereditflag==1&&region<=REGIONMAN&&g_renderoutputformat==RENDEROUTLR)
					{
						RenderEditSelRegion(epx, epy, region);
					}
				}	
			}
		}
		
		/* 右键*/
		if(event->button == 3 && g_renderflag==0)
		{
			//px = (int)event->x;
			//py = (int)event->y;

			region = CoordPosTrans(px, py, &epx, &epy);

			if(region >= REGIONORG)
			{
				//if((mask!=0) && ((mask&(mask-1))==0))
				{
					if(g_enlargeflag==0)
					{
						lb = g_frames[g_frameIndex].SegLabels[epx+epy*g_frameWidth];
						olb = g_frames[g_frameIndex].OrgLabels[epx+epy*g_frameWidth];
						nlb = size+(1<<g_frames[g_frameIndex].LayerEditNum);
						/*鼠标点处返回为原来的label*/
						/*设置标号*/
						if(g_segeditflag==0&&g_mateditflag==0)//&&lb==nlb)
						{
							#if 0
							if(g_frameResizeFlag==1)
								SetOldlabel(g_frames[g_frameIndex].SegLabels0, g_frames[g_frameIndex].OrgLabels0, lb, olb, sizeorg);
							SetOldlabel(g_frames[g_frameIndex].SegLabels, g_frames[g_frameIndex].OrgLabels, lb, olb, size);
							#else
							if(g_frameResizeFlag==1)
								SetOldlabelMask0(g_frames[g_frameIndex].SegLabels0, g_frames[g_frameIndex].OrgLabels0, g_frames[g_frameIndex].SegLabels, g_frames[g_frameIndex].OrgLabels, g_frames[g_frameIndex].SegLabelMask, epx, epy);
							else
								SetOldlabelMask(g_frames[g_frameIndex].SegLabels, g_frames[g_frameIndex].OrgLabels, g_frames[g_frameIndex].SegLabelMask, epx, epy, g_frameWidth, g_frameHeight);
							#endif
							/*画出分割图和深度图*/
							ShowSegandDep(g_frames[g_frameIndex].SegLabelMask);
							g_changeflag = 1;
						}
						if(g_segeditflag==1)
						{
							if(g_frameResizeFlag==0)
								SetOldlabelRegion(g_frames[g_frameIndex].SegLabels, g_frames[g_frameIndex].OrgLabels, g_frames[g_frameIndex].SegLabelMask, epx, epy);
							else
								SetOldlabelRegion0(g_frames[g_frameIndex].SegLabels0, g_frames[g_frameIndex].OrgLabels0, g_frames[g_frameIndex].SegLabelMask, epx*g_frameInWidth/g_frameWidth, epy*g_frameInHeight/g_frameHeight);
							ShowSegandDep(g_frames[g_frameIndex].SegLabelMask);
							g_changeflag = 1;
						}
						if(g_mateditflag==1)
						{
							if(g_frameResizeFlag==0)
								SetMatSelxy(epx, epy);
							else
								SetMatSelxy(epx*g_frameInWidth/g_frameWidth, epy*g_frameInHeight/g_frameHeight);
						}
					}
					else // g_enlargeflag = 1
					{
						if(g_frameResizeFlag==0)
						{
							lb = g_frames[g_frameIndex].SegLabels[epx+epy*g_frameWidth];
							olb = g_frames[g_frameIndex].OrgLabels[epx+epy*g_frameWidth];
						}
						else
						{
							lb = g_frames[g_frameIndex].SegLabels0[epx+epy*g_frameInWidth];
							olb = g_frames[g_frameIndex].OrgLabels0[epx+epy*g_frameInWidth];
						}
						nlb = size+(1<<g_frames[g_frameIndex].LayerEditNum);
						/*鼠标点处返回为原来的label*/
						/*设置标号*/
						if(g_segeditflag==0&&g_mateditflag==0)//&&lb==nlb)
						{
							#if 0
							if(g_frameResizeFlag==1)
								SetOldlabel(g_frames[g_frameIndex].SegLabels0, g_frames[g_frameIndex].OrgLabels0, lb, olb, sizeorg);
							SetOldlabel(g_frames[g_frameIndex].SegLabels, g_frames[g_frameIndex].OrgLabels, lb, olb, size);
							#else
							if(g_frameResizeFlag==1)
								SetOldlabelMask0(g_frames[g_frameIndex].SegLabels0, g_frames[g_frameIndex].OrgLabels0, g_frames[g_frameIndex].SegLabels, g_frames[g_frameIndex].OrgLabels, g_frames[g_frameIndex].SegLabelMask, epx, epy);
							else
								SetOldlabelMask(g_frames[g_frameIndex].SegLabels, g_frames[g_frameIndex].OrgLabels, g_frames[g_frameIndex].SegLabelMask, lb, olb, g_frameWidth, g_frameHeight);
							#endif
							/*画出分割图*/
							SegEnlargedisplay(g_frames[g_frameIndex].SegLabelMask);
							g_changeflag = 1;
						}
						if(g_segeditflag==1)
						{
							if(g_frameResizeFlag==0)
								SetOldlabelRegion(g_frames[g_frameIndex].SegLabels, g_frames[g_frameIndex].OrgLabels, g_frames[g_frameIndex].SegLabelMask, epx, epy);
							else
								SetOldlabelRegion0(g_frames[g_frameIndex].SegLabels0, g_frames[g_frameIndex].OrgLabels0, g_frames[g_frameIndex].SegLabelMask, epx, epy);
							SegEnlargedisplay(g_frames[g_frameIndex].SegLabelMask);
							g_changeflag = 1;
						}
						if(g_mateditflag==1)
						{
							SetMatSelxy(epx, epy);
						}
					}
				}
			}
		}

		/*if(event->state&GDK_SCROLL_MASK)
		{
			printf("sconl up\n");
		}
		if(event->button == 5)
		{
			printf("sconl down\n");
		}*/
	}
	else
	{
		if(g_frameMaxNum==0)
		{
			//messagebox("Please open a file first.");
			return false;
		}
		else if(g_frames[g_frameIndex].SegFlag==0)
		{
			//messagebox("Please segment the frame first.");
			return false;
		}
	}
	return true;
}

/*鼠标移动事件*/
bool
scribble_motion_notify_event (GtkWidget *widget,
		GdkEventMotion *event,
		gpointer data)
{

	int px, py;
	int epx, epy;
	int lb, olb, nlb;
	int sizeorg = g_frameInWidth*g_frameInHeight;
	int size = g_frameWidth*g_frameHeight;
	int mask;// = g_frames[g_frameIndex].SegLabel-size;
	GdkModifierType state;
	char str[512];
	int r=0;
	int g=0;
	int b=0;
	int l=0;
	int d=0;
	int base=0;
	int region = 0;
	GdkCursor *cursor;

	gdk_window_get_pointer (event->window, &px, &py, &state);

	region = CoordPosTrans(px, py, &epx, &epy);

	if(region >= REGIONORG)
	{
		cursor = gdk_cursor_new (GDK_CROSSHAIR);
	    gdk_window_set_cursor(event->window,cursor);
	}
	else
	{
		cursor = gdk_cursor_new (GDK_ARROW);
	    gdk_window_set_cursor(event->window,cursor);
	}

	if(g_frameMaxNum>0&&g_frames[g_frameIndex].SegFlag==1)
	{
		mask =  g_frames[g_frameIndex].SegLabelMask;
		
		if (state & GDK_BUTTON1_MASK)
		{
			//region = CoordPosTrans(px, py, &epx, &epy);

			if(region >= REGIONORG)
			{
				//if((mask!=0) && ((mask&(mask-1))==0) &&g_renderflag==0)
				if((g_frames[g_frameIndex].LayerEditNum>0)&&(g_frames[g_frameIndex].LayerEditNum<=LAYERBUTTONNUMMAX) &&g_renderflag==0)
				{
					if(g_enlargeflag==0)
					{
						lb = g_frames[g_frameIndex].SegLabels[epx+epy*g_frameWidth];
						nlb = size+(1<<g_frames[g_frameIndex].LayerEditNum);
						/*鼠标点处设置为seglabel(第一个条件保证已经分割，第二个条件保证选中)*/
						if(lb!=0&&lb<size&&g_segeditflag==0&&g_mateditflag==0/*g_frames[g_frameIndex].SegLabel!=0&&lb!=g_frames[g_frameIndex].SegLabel*/)
						{
							/*设置新标号*/
							Setlabel(g_frames[g_frameIndex].SegLabels, lb, nlb, size);
							if(g_frameResizeFlag==1)
								Setlabel(g_frames[g_frameIndex].SegLabels0, lb, nlb, sizeorg);
							/*画出分割图和深度图*/
							ShowSegandDep(g_frames[g_frameIndex].SegLabelMask);
							g_changeflag = 1;
						}
						if(g_segeditflag==1)
						{
							if(g_frameResizeFlag==0)
								SegEditLabel(g_frames[g_frameIndex].SegLabels, nlb, epx, epy);
							else
								SegEditLabel0(g_frames[g_frameIndex].SegLabels0, nlb, epx*g_frameInWidth/g_frameWidth, epy*g_frameInHeight/g_frameHeight);
							/*画出分割图和深度图*/
							ShowSegandDep(g_frames[g_frameIndex].SegLabelMask);
							g_changeflag = 1;
						}
						if(g_mateditflag==1)
						{
							if(g_frameResizeFlag==0)
								SegMattingLabel(g_frames[g_frameIndex].SegLabels, g_frames[g_frameIndex].OrgLabels, epx, epy, g_frameWidth, g_frameHeight);
							else
								SegMattingLabel0(g_frames[g_frameIndex].SegLabels0, g_frames[g_frameIndex].OrgLabels0, epx*g_frameInWidth/g_frameWidth, epy*g_frameInHeight/g_frameHeight, g_frameInWidth, g_frameInHeight);
							/*画出分割图和深度图*/
							ShowSegandDep(g_frames[g_frameIndex].SegLabelMask);
							g_changeflag = 1;
						}

					}
					else // g_enlargeflag = 1
					{
						if(g_frameResizeFlag==1)
							lb = g_frames[g_frameIndex].SegLabels0[epx+epy*g_frameInWidth];
						else
							lb = g_frames[g_frameIndex].SegLabels[epx+epy*g_frameWidth];
						nlb = size+(1<<g_frames[g_frameIndex].LayerEditNum);
						if(lb!=0&&lb<size&&g_segeditflag==0&&g_mateditflag==0/*g_frames[g_frameIndex].SegLabel!=0&&lb!=g_frames[g_frameIndex].SegLabel*/)
						{
							/*设置新标号*/
							Setlabel(g_frames[g_frameIndex].SegLabels, lb, nlb, size);
							if(g_frameResizeFlag==1)
								Setlabel(g_frames[g_frameIndex].SegLabels0, lb, nlb, sizeorg);
							/*画出分割图*/
							SegEnlargedisplay(g_frames[g_frameIndex].SegLabelMask);
							g_changeflag = 1;
						}
						if(g_segeditflag==1)
						{
							if(g_frameResizeFlag==0)
								SegEditLabel(g_frames[g_frameIndex].SegLabels, nlb, epx, epy);
							else
								SegEditLabel0(g_frames[g_frameIndex].SegLabels0, nlb, epx, epy);
							/*画出分割图*/
							SegEnlargedisplay(g_frames[g_frameIndex].SegLabelMask);
							g_changeflag = 1;
						}
						if(g_mateditflag==1)
						{
							if(g_frameResizeFlag==0)
								SegMattingLabel(g_frames[g_frameIndex].SegLabels, g_frames[g_frameIndex].OrgLabels, epx, epy, g_frameWidth, g_frameHeight);
							else
								SegMattingLabel0(g_frames[g_frameIndex].SegLabels0, g_frames[g_frameIndex].OrgLabels0, epx, epy, g_frameInWidth, g_frameInHeight);
							/*画出分割图*/
							SegEnlargedisplay(g_frames[g_frameIndex].SegLabelMask);
							g_changeflag = 1;
						}
					}
				}
				// render精细编辑模式
				if(g_renderflag==1&&g_rendereditflag==1&&region<=REGIONSEG&&g_renderoutputformat==RENDEROUTLR)
				{
					//printf("g_rendereditflag = 1\n");
					RenderEditSelRegion(epx, epy, region);
				}
			}
			
		}
		if((state & GDK_BUTTON3_MASK)&& (g_renderflag==0)) // 右键
		{
			px = (int)event->x;
			py = (int)event->y;

			//region = CoordPosTrans(px, py, &epx, &epy);

			if(region >= REGIONORG)
			{
				//if((mask!=0) && ((mask&(mask-1))==0))
				{
					if(g_enlargeflag == 0)
					{
						lb = g_frames[g_frameIndex].SegLabels[epx+epy*g_frameWidth];
						olb = g_frames[g_frameIndex].OrgLabels[epx+epy*g_frameWidth];
						nlb = size+(1<<g_frames[g_frameIndex].LayerEditNum);
						/*鼠标点处返回为原来的label*/
						/*设置标号*/
						if(g_segeditflag==0&&g_mateditflag==0)//&&lb==nlb)
						{
							#if 0
							if(g_frameResizeFlag==1)
								SetOldlabel(g_frames[g_frameIndex].SegLabels0, g_frames[g_frameIndex].OrgLabels0, lb, olb, sizeorg);
							SetOldlabel(g_frames[g_frameIndex].SegLabels, g_frames[g_frameIndex].OrgLabels, lb, olb, size);
							#else
							if(g_frameResizeFlag==1)
								SetOldlabelMask0(g_frames[g_frameIndex].SegLabels0, g_frames[g_frameIndex].OrgLabels0, g_frames[g_frameIndex].SegLabels, g_frames[g_frameIndex].OrgLabels, g_frames[g_frameIndex].SegLabelMask, epx, epy);
							else
								SetOldlabelMask(g_frames[g_frameIndex].SegLabels, g_frames[g_frameIndex].OrgLabels, g_frames[g_frameIndex].SegLabelMask, lb, olb, g_frameWidth, g_frameHeight);
							#endif
							/*画出分割图和深度图*/
							ShowSegandDep(g_frames[g_frameIndex].SegLabelMask);
							g_changeflag = 1;
						}
						if(g_segeditflag==1)
						{
							if(g_frameResizeFlag==0)
								SetOldlabelRegion(g_frames[g_frameIndex].SegLabels, g_frames[g_frameIndex].OrgLabels, g_frames[g_frameIndex].SegLabelMask, epx, epy);
							else
								SetOldlabelRegion0(g_frames[g_frameIndex].SegLabels0, g_frames[g_frameIndex].OrgLabels0, g_frames[g_frameIndex].SegLabelMask, epx*g_frameInWidth/g_frameWidth, epy*g_frameInHeight/g_frameHeight);
							ShowSegandDep(g_frames[g_frameIndex].SegLabelMask);
							g_changeflag = 1;
						}

					}
					else // g_enlargeflag = 1
					{
						if(g_frameResizeFlag==0)
						{
							lb = g_frames[g_frameIndex].SegLabels[epx+epy*g_frameWidth];
							olb = g_frames[g_frameIndex].OrgLabels[epx+epy*g_frameWidth];
						}
						else
						{
							lb = g_frames[g_frameIndex].SegLabels0[epx+epy*g_frameInWidth];
							olb = g_frames[g_frameIndex].OrgLabels0[epx+epy*g_frameInWidth];
						}
						nlb = size+(1<<g_frames[g_frameIndex].LayerEditNum);
						/*鼠标点处返回为原来的label*/
						/*设置标号*/
						if(g_segeditflag==0&&g_mateditflag==0)//&&lb==nlb)
						{
							#if 0
							if(g_frameResizeFlag==1)
								SetOldlabel(g_frames[g_frameIndex].SegLabels0, g_frames[g_frameIndex].OrgLabels0, lb, olb, sizeorg);
							SetOldlabel(g_frames[g_frameIndex].SegLabels, g_frames[g_frameIndex].OrgLabels, lb, olb, size);
							#else
							if(g_frameResizeFlag==1)
								SetOldlabelMask0(g_frames[g_frameIndex].SegLabels0, g_frames[g_frameIndex].OrgLabels0, g_frames[g_frameIndex].SegLabels, g_frames[g_frameIndex].OrgLabels, g_frames[g_frameIndex].SegLabelMask, epx, epy);
							else
								SetOldlabelMask(g_frames[g_frameIndex].SegLabels, g_frames[g_frameIndex].OrgLabels, g_frames[g_frameIndex].SegLabelMask, lb, olb, g_frameWidth, g_frameHeight);
							#endif
							/*画出分割图*/
							SegEnlargedisplay(g_frames[g_frameIndex].SegLabelMask);
							g_changeflag = 1;
						}
						if(g_segeditflag==1)
						{
							if(g_frameResizeFlag==0)
								SetOldlabelRegion(g_frames[g_frameIndex].SegLabels, g_frames[g_frameIndex].OrgLabels, g_frames[g_frameIndex].SegLabelMask, epx, epy);
							else
								SetOldlabelRegion0(g_frames[g_frameIndex].SegLabels0, g_frames[g_frameIndex].OrgLabels0, g_frames[g_frameIndex].SegLabelMask, epx, epy);
							SegEnlargedisplay(g_frames[g_frameIndex].SegLabelMask);
							g_changeflag = 1;
						}
					}
				}
			}	
		}
		
		/*添加显示info代码TODO*/
		#if 0
		if((px>=IMGCENTERX-g_frameWidth)&&(px<IMGCENTERX)&&(py>=IMGCENTERY-g_frameHeight)&&(py<IMGCENTERY))
		{
			epx = px+g_frameWidth-IMGCENTERX;
			epy = py+g_frameHeight-IMGCENTERY;
			if(g_segeditflag==1)
				DrawSegEditContour(epx, epy, REGIONORG);
			if(g_renderflag==1&&g_rendereditflag==1)
				DrawRenEditContour(epx, epy, REGIONORG);
		}
		else if((px>=IMGCENTERX)&&(px<IMGCENTERX+g_frameWidth)&&(py>=IMGCENTERY-g_frameHeight)&&(py<IMGCENTERY))
		{
			epx = px-IMGCENTERX;
			epy = py+g_frameHeight-IMGCENTERY;
			if(g_segeditflag==1)
				DrawSegEditContour(epx, epy, REGIONSEG);
			if(g_renderflag==1&&g_rendereditflag==1)
				DrawRenEditContour(epx, epy, REGIONSEG);
		}
		else if((px>=IMGCENTERX-g_frameWidth)&&(px<IMGCENTERX)&&(py>=IMGCENTERY)&&(py<IMGCENTERY+g_frameHeight))
		{
			epx = px+g_frameWidth-IMGCENTERX;
			epy = py-IMGCENTERY;
			if(g_segeditflag==1)
				DrawSegEditContour(epx, epy, REGIONMAN);
		}
		else if((px>=IMGCENTERX)&&(px<IMGCENTERX+g_frameWidth)&&(py>=IMGCENTERY)&&(py<IMGCENTERY+g_frameHeight))
		{
			epx = px-IMGCENTERX;
			epy = py-IMGCENTERY;
			if(g_segeditflag==1)
				DrawSegEditContour(epx, epy, REGIONDEP);
		}
		else
		{
			epx = epy = 0;
		}
		#else
		if(region>=REGIONORG)
		{
			if((g_segeditflag+g_mateditflag)==1&&g_enlargeswitchflag==0)
				DrawSegEditContour(epx, epy, px-IMGCENTERX+g_frameWidth, py-IMGCENTERY+g_frameHeight, region);
			if(g_renderflag==1&&g_rendereditflag==1&&g_renderoutputformat==RENDEROUTLR)
				DrawRenEditContour(epx, epy, region);
			if(g_depeditflag==1)
				DrawDepEditContour(epx, epy, px-IMGCENTERX+g_frameWidth, py-IMGCENTERY+g_frameHeight, region);
		}
		#endif

		if(g_frameResizeFlag==0||g_enlargeflag==0)
		{
			r = g_frames[g_frameIndex].rgbinput[epx*3+epy*g_frameWidth*3];
			g = g_frames[g_frameIndex].rgbinput[epx*3+1+epy*g_frameWidth*3];
			b = g_frames[g_frameIndex].rgbinput[epx*3+2+epy*g_frameWidth*3];
			lb = g_frames[g_frameIndex].SegLabels[epx+epy*g_frameWidth];
			d = g_frames[g_frameIndex].SegDepths[epx+epy*g_frameWidth];
		}
		else
		{
			r = g_rgborginput[epx*3+epy*g_frameInWidth*3];
			g = g_rgborginput[epx*3+1+epy*g_frameInWidth*3];
			b = g_rgborginput[epx*3+2+epy*g_frameInWidth*3];
			lb = g_frames[g_frameIndex].SegLabels0[epx+epy*g_frameInWidth];
			d = g_frames[g_frameIndex].SegDepths0[epx+epy*g_frameInWidth];
		}
		
		if(lb<size)
		{
			l=0;
			//d=0;
			//base=0;
		}
		else
		{
			int offset = lb - size;

			for(int i=0;i<=g_frames[g_frameIndex].LayerVisibleNum;i++)
			{
				if((offset&(0x1<<i))!=0)
				{
					l = i;
					break;
				}
			}
			//if(g_frameResizeFlag==0||g_enlargeflag==0)
				//d=g_frames[g_frameIndex].SegDepths[epx+epy*g_frameWidth];
			//else
				//d=g_frames[g_frameIndex].SegDepths0[epx+epy*g_frameInWidth];
		}
		base = g_frames[g_frameIndex].LayerBaseNum;

		if(g_frames[g_frameIndex].DepFlag==0)
		{
			d = 0;
		}
		
		sprintf_s(str,"   Section %d of %d Time %ds to %ds Coordinate Position X: %3d Y: %3d   /   Color R: %3d G: %3d B: %3d   /   Layer: %2d Base: %2d  /   Depth: %3d   /  Render Ref:  %3d", g_sectioncur, g_sectionnum, g_timestart+(g_sectioncur-1)*g_sectionduration, g_timestart+g_sectioncur*g_sectionduration, epx,epy, r,g,b,l,base,d, g_frameRenderRef+1);
		gtk_label_set_text((GtkLabel *)g_wdgInfo,str);
		
	}
	else if(g_frameMaxNum>0)
	{
		if(g_frameResizeFlag==0||g_enlargeflag==0)
		{
			r = g_frames[g_frameIndex].rgbinput[epx*3+epy*g_frameWidth*3];
			g = g_frames[g_frameIndex].rgbinput[epx*3+1+epy*g_frameWidth*3];
			b = g_frames[g_frameIndex].rgbinput[epx*3+2+epy*g_frameWidth*3];
		}
		else
		{
			r = g_rgborginput[epx*3+epy*g_frameInWidth*3];
			g = g_rgborginput[epx*3+1+epy*g_frameInWidth*3];
			b = g_rgborginput[epx*3+2+epy*g_frameInWidth*3];
		}
		l = 0;
		base = 0;

		if(g_frames[g_frameIndex].DepFlag==1)
		{
			if(g_frameResizeFlag==0||g_enlargeflag==0)
			{
				d = g_frames[g_frameIndex].SegDepths[epx+epy*g_frameWidth];
			}
			else
			{
				d = g_frames[g_frameIndex].SegDepths0[epx+epy*g_frameInWidth];
			}
		}
		else
			d = 0;
		
		sprintf_s(str,"   Section %d of %d Time %ds to %ds Coordinate Position X: %3d Y: %3d   /   Color R: %3d G: %3d B: %3d   /   Layer: %2d Base: %2d  /   Depth: %3d   /  Render Ref:  %3d", g_sectioncur, g_sectionnum, g_timestart+(g_sectioncur-1)*g_sectionduration, g_timestart+g_sectioncur*g_sectionduration, epx,epy, r,g,b,l,base,d, g_frameRenderRef+1);
		gtk_label_set_text((GtkLabel *)g_wdgInfo,str);
		// none
	}
	return true;
}

/*
+--------------------+-----------------+-----------------+
 |        GTK常用功能键值对照表(Ctrl、Alt、Shift)         |
 +--------------------+-----------------+-----------------+
 |   GDK define       |    键值         |     说明        |
 +--------------------+-----------------+-----------------+
 |  GDK_Shift_L       |   0xFFE1        |   左Shift键     |
 +--------------------+-----------------+-----------------+
 |  GDK_Shift_R       |   0xFFE2        |   右Shift键     |
 +--------------------+-----------------+-----------------+
 |  GDK_Control_L     |   0xFFE3        |   左Ctrl键      |
 +--------------------+-----------------+-----------------+
 |  GDK_Control_R     |   0xFFE4        |   右Ctrl键      |
 +--------------------+-----------------+-----------------+
 |  GDK_Alt_L         |   0xFFE9        |   左Alt键       |
 +--------------------+-----------------+-----------------+
 |  GDK_Alt_R         |   0xFFEA        |   右Alt键       |
 +--------------------+-----------------+-----------------+
 其中的键值定义在/usr/include/gtk-2.0/gdk/gdkkeysyms.h文件里。
 #define GDK_KEY_Left 0xff51
 #define GDK_KEY_Up 0xff52
 #define GDK_KEY_Right 0xff53
 #define GDK_KEY_Down 0xff54
*/
bool Shift_press_flag = FALSE;
bool Ctrl_press_flag = FALSE;
bool Alt_press_flag = FALSE;

gboolean keypress_event (GtkWidget *widget,GdkEventKey *event,gpointer user_data)
{

	// 判断组合键
	if (event->keyval==0xFFE1 || event->keyval== 0xFFE2){
		Shift_press_flag = TRUE; 			  //Shift键按下
		return FALSE;
	}
	else if (event->keyval==0xFFE3 || event->keyval== 0xFFE4){
		Ctrl_press_flag = TRUE;				  //Ctrl键按下
		return FALSE;
	}
	else if (event->keyval==0xFFE9 || event->keyval== 0xFFEA){
		Alt_press_flag = TRUE; 			  //Alt键按下
		return FALSE;
	}

	if(g_frameMaxNum>0&&g_frames[g_frameIndex].SegFlag==1&&g_enlargeflag==1&&g_frameResizeFlag==1)
	{
		// 判断放大模式下的图像移动
		if(event->keyval==0xff53) // right
		{
			g_enlargeorgx += 10;
			if(g_enlargeorgx+2*g_frameWidth>g_frameInWidth)
				g_enlargeorgx = g_frameInWidth-2*g_frameWidth;
			else
				SegEnlargedisplay(g_frames[g_frameIndex].SegLabelMask);
			
		}
		if(event->keyval==0xff54) // down
		{
			g_enlargeorgy += 10;
			if(g_enlargeorgy+2*g_frameHeight>g_frameInHeight)
				g_enlargeorgy = g_frameInHeight-2*g_frameHeight;
			else
				SegEnlargedisplay(g_frames[g_frameIndex].SegLabelMask);
		}
		if(event->keyval==0xff51) // left
		{
			g_enlargeorgx -= 10;
			if(g_enlargeorgx<0)
				g_enlargeorgx = 0;
			else
				SegEnlargedisplay(g_frames[g_frameIndex].SegLabelMask);
		}
		if(event->keyval==0xff52) // up
		{
			g_enlargeorgy -= 10;
			if(g_enlargeorgy<0)
				g_enlargeorgy = 0;
			else
				SegEnlargedisplay(g_frames[g_frameIndex].SegLabelMask);
		}

		// 切换分割和深度显示
		if(event->keyval==0x063)
		{
			if(g_enlargeswitchflag==0)
			{
				g_enlargeswitchflag = 1;
			}
			else
			{
				g_enlargeswitchflag = 0;
			}
			SegEnlargedisplay(g_frames[g_frameIndex].SegLabelMask);
		}
			
	}
#if 0
	// Ctrl+z
	while (Ctrl_press_flag){
		if (event->keyval== 0x07A || event->keyval== 0x05A)
		{
			//printf("Ctrl+z pressed!");	 // z  0x07a  D  0x05a
			/* add ctrl+z code */
			loadsegmenttmp();
			return TRUE;
		}
	}		
#endif
	return TRUE; 			//其它键

}

gboolean keyrelease_event(GtkWidget * widget, GdkEventKey *event, gpointer data)
{
	if (event->keyval==0xFFE1 || event->keyval== 0xFFE2){
		Shift_press_flag = FALSE;              //Shift键释放
		return FALSE;
	}
	else if (event->keyval==0xFFE3 || event->keyval== 0xFFE4){
		Ctrl_press_flag = FALSE;              //Ctrl键释放
		return FALSE;
	}
	else if (event->keyval==0xFFE9 || event->keyval== 0xFFEA){
		Alt_press_flag = FALSE;              //Alt键释放
		return FALSE;
	}
	return TRUE;
}


void nonedit(GtkWidget * button,GtkWidget * data)
{

	g_segeditflag = 0;
	g_mateditflag = 0;
	g_matselflag = 0;
	g_depeditflag = 0;
	g_rendereditflag = 0;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgNonEdit), TRUE);

	return ;
}

void setnorendermode(GtkWidget * button,GtkWidget * data)
{

    g_renderflag = 0;
	g_renderdisplayflag = 0;
	g_enlargeflag = 0;
	g_segeditflag = 0;
	g_mateditflag = 0;
	g_matselflag = 0;
	g_depeditflag = 0;
	g_rendereditflag = 0;
	ShowAll();
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgSegmentMode), TRUE);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgRenderMode), FALSE);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgNonEdit), TRUE);
	
	return ;
}

void setrendermode(GtkWidget * button,GtkWidget * data)
{
	int i,j;
	int fileexist = 0;
	char filename[128];
	
	IplImage* stereoimg;
	IplImage* stereoimg1;
	CvSize sz;

	if(g_frameMaxNum>0)
	{
		//printf("no g_wdgRenderMode\n");
		g_renderflag = 1;
		g_enlargeflag = 0;
		g_segeditflag = 0;
		g_mateditflag = 0;
		g_depeditflag = 0;
		g_rendereditflag = 0;

		//如果存在render文件，读取文件内容，并显示，否则，显示当前图像
		if(g_keyframeflag)
			sprintf_s(filename,"./%s/render/%d_rendersnap_%d.png",cfgparams.projectname,g_sectioncur,g_keyframenumarr[g_frameIndex]+1);
		else
			sprintf_s(filename,"./%s/render/%d_rendersnap_%d.png",cfgparams.projectname,g_sectioncur,g_frameIndex+1);
		fileexist = (_access(filename,0)==0);
		if(fileexist)
		{
			stereoimg = cvLoadImage(filename);
			if(stereoimg->width==g_frameInWidth*2)
			{
				sz.width = g_frameWidth*2;
				g_renderoutputformat = RENDEROUTLR;
			}
			else if(stereoimg->width==g_frameInWidth)
			{
				sz.width = g_frameWidth;
				g_renderoutputformat = RENDEROUTAN;
			}
			else
			{
				messagebox(MSGBOX_LRIERR);
				cvReleaseImage( &stereoimg );
				ShowAll();
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgRenderMode), TRUE);
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgSegmentMode), FALSE);
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgNonEdit), TRUE);
				return ;
			}
			sz.height = g_frameHeight;
			stereoimg1 = cvCreateImage(sz, IPL_DEPTH_8U, 3);
			cvResize(stereoimg, stereoimg1,CV_INTER_CUBIC);

			if(g_renderoutputformat == RENDEROUTLR)
			{
				for(j=0;j<g_frameHeight;j++)
					for(i=0;i<g_frameWidth;i++)
					{
						g_tmpdisplay1[i*3+0+j*3*g_frameWidth] = stereoimg1->imageData[i*3+2+j*stereoimg1->widthStep];
						g_tmpdisplay1[i*3+1+j*3*g_frameWidth] = stereoimg1->imageData[i*3+1+j*stereoimg1->widthStep];
						g_tmpdisplay1[i*3+2+j*3*g_frameWidth] = stereoimg1->imageData[i*3+0+j*stereoimg1->widthStep];
						g_tmpdisplay2[i*3+0+j*3*g_frameWidth] = stereoimg1->imageData[i*3+2+g_frameWidth*3+j*stereoimg1->widthStep];
						g_tmpdisplay2[i*3+1+j*3*g_frameWidth] = stereoimg1->imageData[i*3+1+g_frameWidth*3+j*stereoimg1->widthStep];
						g_tmpdisplay2[i*3+2+j*3*g_frameWidth] = stereoimg1->imageData[i*3+0+g_frameWidth*3+j*stereoimg1->widthStep];
					}
			}
			else
			{
				for(j=0;j<g_frameHeight;j++)
					for(i=0;i<g_frameWidth;i++)
					{
						g_tmpdisplay1[i*3+0+j*3*g_frameWidth] = stereoimg1->imageData[i*3+2+j*stereoimg1->widthStep];
						g_tmpdisplay1[i*3+1+j*3*g_frameWidth] = stereoimg1->imageData[i*3+1+j*stereoimg1->widthStep];
						g_tmpdisplay1[i*3+2+j*3*g_frameWidth] = stereoimg1->imageData[i*3+0+j*stereoimg1->widthStep];
						g_tmpdisplay2[i*3+0+j*3*g_frameWidth] = stereoimg1->imageData[i*3+2+j*stereoimg1->widthStep];
						g_tmpdisplay2[i*3+1+j*3*g_frameWidth] = stereoimg1->imageData[i*3+1+j*stereoimg1->widthStep];
						g_tmpdisplay2[i*3+2+j*3*g_frameWidth] = stereoimg1->imageData[i*3+0+j*stereoimg1->widthStep];
					}
			}
			cvReleaseImage( &stereoimg1 );
			cvReleaseImage( &stereoimg );	
		
			ShowAll();
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgRenderMode), TRUE);
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgSegmentMode), FALSE);
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgNonEdit), TRUE);
		}
		else
		{
			//printf("no rendered frame\n");
			ShowAll();
		}
	}

	return ;
}

void setshowdepy(GtkWidget * button,GtkWidget * data)
{
	g_showalldepflag = 1;
	ShowAll();
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgShowDepY), TRUE);

	return ;
}

void setshowdepn(GtkWidget * button,GtkWidget * data)
{
	g_showalldepflag = 0;
	ShowAll();
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgShowDepN), TRUE);

	return ;
}

void setshowautosegy(GtkWidget * button,GtkWidget * data)
{
	g_showautosegflag = 1;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgShowSegY), TRUE);

	return ;
}

void setshowautosegn(GtkWidget * button,GtkWidget * data)
{
	g_showautosegflag = 0;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgShowSegN), TRUE);

	return ;
}


void setenautodepoverwrite(GtkWidget * button,GtkWidget * data)
{
	g_depoverwritten = 1;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgEnAutoDepOW), TRUE);

	return ;
}

void setdisautodepoverwrite(GtkWidget * button,GtkWidget * data)
{
	g_depoverwritten = 0;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgDisAutoDepOW), TRUE);

	return ;
}

void setenautodepaddmotion(GtkWidget * button,GtkWidget * data)
{
	g_depautoaddmotion = 1;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgEnAutoDepAM), TRUE);

	return ;
}

void setdisautodepaddmotion(GtkWidget * button,GtkWidget * data)
{
	g_depautoaddmotion  = 0;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgDisAutoDepAM), TRUE);

	return ;
}



void setcircle(GtkWidget * button,GtkWidget * data)
{
	if(g_segeditflag==1)
	{
		g_segeditmodule = SEGEDITMODULECIR;
	}
	if(g_mateditflag==1)
	{
		g_segeditmodule = SEGEDITMODULECIR;
	}
	if(g_depeditflag==1)
	{
		g_depeditmodule = DEPEDITMODULECIR;
	}
	if(g_rendereditflag==1)
	{
		g_rendereditselmodule = RENEDITSELMODECIR;
	}
	
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgEditCircle), TRUE);

	return ;
}

void setsquare(GtkWidget * button,GtkWidget * data)
{
	if(g_segeditflag==1)
	{
		g_segeditmodule = SEGEDITMODULESQU;
	}
	if(g_mateditflag==1)
	{
		g_segeditmodule = SEGEDITMODULESQU;
	}
	if(g_depeditflag==1)
	{
		g_depeditmodule = DEPEDITMODULESQU;
	}
	if(g_rendereditflag==1)
	{
		g_rendereditselmodule = RENEDITSELMODESQU;
	}
	
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgEditSquare), TRUE);

	return ;
}

void radiusminusstrength(GtkWidget * button,GtkWidget * data)
{
	char str[32];
	g_segparamautoedgeth--;
	if(g_segparamautoedgeth<SEGPARAM_AUTOEDGETHMIN)
		g_segparamautoedgeth=SEGPARAM_AUTOEDGETHMIN;
	sprintf_s(str,"%d", g_segparamautoedgeth);
	gtk_entry_set_text(GTK_ENTRY(g_wdgEditStrength), str);
	
	return ;
}

void radiusplusstrength(GtkWidget * button,GtkWidget * data)
{
	char str[32];
	g_segparamautoedgeth++;
	if(g_segparamautoedgeth>SEGPARAM_AUTOEDGETHMAX)
		g_segparamautoedgeth=SEGPARAM_AUTOEDGETHMAX;
	sprintf_s(str,"%d", g_segparamautoedgeth);
	gtk_entry_set_text(GTK_ENTRY(g_wdgEditStrength), str);
	
	return ;
}

void radiussetstrength(GtkWidget * button,GtkWidget * data)
{
	char *str=(char *)malloc(32*sizeof(char));
	char str1[32];
	int value;

	str=(char*)gtk_entry_get_text(GTK_ENTRY(g_wdgEditStrength));
	value = str2num(str); 
	
	if(value<SEGPARAM_AUTOEDGETHMIN)  value=SEGPARAM_AUTOEDGETHMIN;
	if(value>SEGPARAM_AUTOEDGETHMAX)  value=SEGPARAM_AUTOEDGETHMAX;
	g_segparamautoedgeth=value;
	sprintf_s(str1,"%d", g_segparamautoedgeth);
	gtk_entry_set_text(GTK_ENTRY(g_wdgEditStrength), str1);
	
	return ;
}

void quicksetalpha(GtkWidget * button,GtkWidget * data)
{
	char *str=(char *)malloc(32*sizeof(char));
	char str1[32];
	int value;

	str=(char*)gtk_entry_get_text(GTK_ENTRY(g_wdgEditalpha));
	value = str2num(str); 
	
	if(value<RENPARAM_ALPHAMIN)  value=RENPARAM_ALPHAMIN;
	if(value>RENPARAM_ALPHAMAX)  value=RENPARAM_ALPHAMAX;
	g_renderalpha=value;
	sprintf_s(str1,"%d", g_renderalpha);
	gtk_entry_set_text(GTK_ENTRY(g_wdgEditalpha), str1);
	
	return ;
}

void quicksetfocal(GtkWidget * button,GtkWidget * data)
{
	char *str=(char *)malloc(32*sizeof(char));
	char str1[32];
	int value;

	str=(char*)gtk_entry_get_text(GTK_ENTRY(g_wdgEditfocal));
	value = str2num(str); 
	
	if(value<RENPARAM_FPMIN)  value=RENPARAM_FPMIN;
	if(value>RENPARAM_FPMAX)  value=RENPARAM_FPMAX;
	g_renderforcalplane=value;
	sprintf_s(str1,"%d", g_renderforcalplane);
	gtk_entry_set_text(GTK_ENTRY(g_wdgEditfocal), str1);
	
	return ;
}

void radiusminus(GtkWidget * button,GtkWidget * data)
{
	char str[32];
	
	if(g_segeditflag==1||g_mateditflag==1)
	{
		g_segeditmodulerad--;
		if(g_segeditmodulerad<SEGEDITPARAM_RADMIN)
			g_segeditmodulerad=SEGEDITPARAM_RADMIN;
		sprintf_s(str,"%d", g_segeditmodulerad);
		gtk_entry_set_text(GTK_ENTRY(g_wdgEditRadius), str);
	}

	if(g_depeditflag==1)
	{
		g_depeditmodulerad--;
		if(g_depeditmodulerad<DEPEDITPARAM_RADMIN)
			g_depeditmodulerad=DEPEDITPARAM_RADMIN;
		sprintf_s(str,"%d", g_depeditmodulerad);
		gtk_entry_set_text(GTK_ENTRY(g_wdgEditRadius), str);
	}

	if(g_rendereditflag==1)
	{
		g_rendereditselmodulerad--;
		if(g_rendereditselmodulerad<RENEDITPARAM_SRADMIN)
			g_rendereditselmodulerad=RENEDITPARAM_SRADMIN;
		sprintf_s(str,"%d", g_rendereditselmodulerad);
		gtk_entry_set_text(GTK_ENTRY(g_wdgEditRadius), str);
	}
	
	return ;
}

void radiusplus(GtkWidget * button,GtkWidget * data)
{
	char str[32];
	
	if(g_segeditflag==1||g_mateditflag==1)
	{
		g_segeditmodulerad++;
		if(g_segeditmodulerad>SEGEDITPARAM_RADMAX)
			g_segeditmodulerad=SEGEDITPARAM_RADMAX;
		sprintf_s(str,"%d", g_segeditmodulerad);
		gtk_entry_set_text(GTK_ENTRY(g_wdgEditRadius), str);
	}

	if(g_depeditflag==1)
	{
		g_depeditmodulerad++;
		if(g_depeditmodulerad>DEPEDITPARAM_RADMAX)
			g_depeditmodulerad=DEPEDITPARAM_RADMAX;
		sprintf_s(str,"%d", g_depeditmodulerad);
		gtk_entry_set_text(GTK_ENTRY(g_wdgEditRadius), str);
	}

	if(g_rendereditflag==1)
	{
		g_rendereditselmodulerad++;
		if(g_rendereditselmodulerad>RENEDITPARAM_SRADMAX)
			g_rendereditselmodulerad=RENEDITPARAM_SRADMAX;
		sprintf_s(str,"%d", g_rendereditselmodulerad);
		gtk_entry_set_text(GTK_ENTRY(g_wdgEditRadius), str);
	}
	
	return ;
}

void radiusset(GtkWidget * button,GtkWidget * data)
{
	char *str=(char *)malloc(32*sizeof(char));
	char str1[32];
	int value;

	str=(char*)gtk_entry_get_text(GTK_ENTRY(g_wdgEditRadius));
	value = str2num(str);  
	
	if(g_segeditflag==1||g_mateditflag==1)
	{
		if(value<SEGEDITPARAM_RADMIN)  value=SEGEDITPARAM_RADMIN;
		if(value>SEGEDITPARAM_RADMAX)  value=SEGEDITPARAM_RADMAX;
		g_segeditmodulerad=value;
		sprintf_s(str1,"%d", g_segeditmodulerad);
		gtk_entry_set_text(GTK_ENTRY(g_wdgEditRadius), str1);
	}

	if(g_depeditflag==1)
	{
		if(value<DEPEDITPARAM_RADMIN)  value=DEPEDITPARAM_RADMIN;
		if(value>DEPEDITPARAM_RADMAX)  value=DEPEDITPARAM_RADMAX;
		g_depeditmodulerad=value;
		sprintf_s(str1,"%d", g_depeditmodulerad);
		gtk_entry_set_text(GTK_ENTRY(g_wdgEditRadius), str1);
	}

	if(g_rendereditflag==1)
	{
		if(value<RENEDITPARAM_SRADMIN)  value=RENEDITPARAM_SRADMIN;
		if(value>RENEDITPARAM_SRADMAX)  value=RENEDITPARAM_SRADMAX;
		g_rendereditselmodulerad=value;
		sprintf_s(str1,"%d", g_rendereditselmodulerad);
		gtk_entry_set_text(GTK_ENTRY(g_wdgEditRadius), str1);
	}
	
	return ;
}




void backwardplay(GtkWidget * button,GtkWidget * data)
{
	g_playflag = YUVPLAYBACKWARD;
	return ;
}

void forwardplay(GtkWidget * button,GtkWidget * data)
{
	g_playflag = YUVPLAYFORWARD;
	return ;
}

int g_timecnt = 0;
int g_timecnt1 = 0;
static int g_tog = 0;
static int g_togflag = 0;
#define RENDERDISPLAYTIMESTEP 4
SProcessBar g_Processbar;
GtkWidget *g_status_bar;
gint g_context_id;

void statusbar_thread(char *buff)
{
	g_usleep(100);
	gdk_threads_enter(); 
	gtk_statusbar_push (GTK_STATUSBAR (g_status_bar), GPOINTER_TO_INT(g_context_id), (g_locale_to_utf8(buff,-1,NULL,NULL,NULL)));
	gdk_threads_leave();
	return ;
}


gboolean time_handler(gpointer widget)
{
	int i,j;
	int size = g_frameWidth*g_frameHeight;
	char indexstr[10];
	char * pindexstr = indexstr;
	char str[32];
	SKeyFrameList * key;
	int keyprev, keynext;
	char filename[256];
	SProcessBar *pbar = (SProcessBar *)widget;

	if(g_frameMaxNum>0&&(g_inputfileformat==INPYUV||g_inputfileformat==INPVID)&&feof(g_fileInpYUV)==0)
	{
		if(g_playflag!=YUVPLAYPAUSE)
		{			
			if(g_keyframeflag)
				sprintf_s(str,"%d:%d/%d", g_frameIndex+1, g_keyframenumarr[g_frameIndex]+1, g_frameMaxNum);
			else
				sprintf_s(str,"%4d /%4d", g_frameIndex+1, g_frameMaxNum);
			gtk_label_set_text((GtkLabel *)g_wdgFrameNum,str);

			if(g_keyframeflag)
			{
				int keyidxprev, keyidxnext;
				keyidxprev = H3D_MAX(0,g_frameIndex-1);
				keyidxnext = H3D_MIN(g_frameIndex+1,g_frameMaxNum-1);
				sprintf_s(str," %4d - %4d - %4d ", g_keyframenumarr[keyidxprev]+1, g_keyframenumarr[g_frameIndex]+1,g_keyframenumarr[keyidxnext]+1);
			}
			else
			{
				key = getcurkeyframe(g_frameIndex);
				if(key->prev!=NULL)
					keyprev = key->prev->index;
				else
					keyprev = key->index;
				if(key->next!=NULL)
					keynext = key->next->index;
				else
					keynext = key->index;
				sprintf_s(str," %4d - %4d - %4d ", keyprev+1, key->index+1,keynext+1);
			}
			gtk_label_set_text((GtkLabel *)g_wdgKeyFrameNum,str);
			gtk_label_set_justify((GtkLabel *)g_wdgKeyFrameNum, GTK_JUSTIFY_CENTER);
			
			
			sprintf_s(str,"%d", g_frameIndex+1);
			gtk_entry_set_text(GTK_ENTRY(g_wdgInpFrameNum), str);
				
			if(g_frameIndex < g_frameMaxNum && g_frameIndex >= 0)
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
					//yuvresize(g_yuvorginputref, g_yuvinputref, g_frameInWidth, g_frameInHeight, g_frameWidth, g_frameHeight);
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

				// render显示所用，默认为原图像
				for(j=0;j<g_frameHeight;j++)
					for(i=0;i<g_frameWidth;i++)
					{
						g_tmpdisplay1[i*3+0+j*3*g_frameWidth] = 
						g_tmpdisplay2[i*3+0+j*3*g_frameWidth] = 
						g_tmpdisplay3[i*3+0+j*3*g_frameWidth] = g_frames[g_frameIndex].rgbinput[i*3+0+j*3*g_frameWidth];
						g_tmpdisplay1[i*3+1+j*3*g_frameWidth] = 
						g_tmpdisplay2[i*3+1+j*3*g_frameWidth] = 
						g_tmpdisplay3[i*3+1+j*3*g_frameWidth] = g_frames[g_frameIndex].rgbinput[i*3+1+j*3*g_frameWidth];
						g_tmpdisplay1[i*3+2+j*3*g_frameWidth] = 
						g_tmpdisplay2[i*3+2+j*3*g_frameWidth] = 
						g_tmpdisplay3[i*3+2+j*3*g_frameWidth] = g_frames[g_frameIndex].rgbinput[i*3+2+j*3*g_frameWidth];
					}
				
				// 载入当前帧信息
				if(g_keyframeflag)
					sprintf(filename,"./%s/info/%d_infosnap_%d.dat",cfgparams.projectname,g_sectioncur,g_keyframenumarr[g_frameIndex]+1);
				else
					sprintf(filename,"./%s/info/%d_infosnap_%d.dat",cfgparams.projectname,g_sectioncur,g_frameIndex+1);
				if(g_showalldepflag==1)
				{
					if((g_frames[g_frameIndex].finfo=fopen(filename,"rb"))!=NULL)
					{
						// load info file
						loadinfofile(g_frames[g_frameIndex].finfo, g_frameIndex, 0);
						fclose(g_frames[g_frameIndex].finfo);
					}
					if(g_frames[g_frameIndex].DepFlag==1)
					{
						for(i=0;i<size;i++)
							g_depthdisplay[i] = g_frames[g_frameIndex].SegDepths[i];
						//printf("\rload info %d of section %d", g_frameIndex+1, g_sectioncur);
					}
					else
					{
						for(i=0;i<size;i++)
							g_depthdisplay[i] = 0;
					}
				}
				else
				{
					for(i=0;i<size;i++)
						g_depthdisplay[i] = 0;
					//printf("\rNo load info %d of section %d", g_frameIndex+1, g_sectioncur);
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
				
				if(g_playflag==YUVPLAYFORWARD) // 正向播放
				{
					g_frameIndex++;
					if(g_frameIndex>=g_frameMaxNum)
					{
						g_frameIndex = g_frameMaxNum-1;
						g_playflag = YUVPLAYPAUSE;
						gotoframe(NULL, NULL);
					}
				}
				if(g_playflag==YUVPLAYBACKWARD) // 反向播放
				{
					g_frameIndex--;
					if(g_frameIndex<0)
					{
						g_frameIndex = 0;
						g_playflag = YUVPLAYPAUSE;
						gotoframe(NULL, NULL);
					}
				}
			}

			
		}
		
	}

	// undo redo
	if(g_frameMaxNum>0)
	{
		g_timecnt++;
		g_timecnt1++;
		if(g_timecnt>25*5) // 5 second
		{
			if(g_changeflag==0)
			{
				//printf("save segment....\n");
				savesegmenttmp();
			}
			else
			{
				if(g_timecnt1>25*30) // 30 second
				{
					g_changeflag = 0;
					//printf("set g_changeflag = 0\n");
					g_timecnt1 = 0;
				}
			}
			g_timecnt = 0;
		}
	}

	
	if(g_frameMaxNum>0&&g_renderdisplayflag==1)
	{
		if(g_tog<=RENDERDISPLAYTIMESTEP)
		{
			gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
		              IMGWIDTHMAX-g_frameWidth, IMGHEIGHTMAX, g_frameWidth, g_frameHeight,
		              GDK_RGB_DITHER_MAX, g_renderdisplay1, g_frameWidth * 3);
			if(g_togflag==0) g_tog++;
			else			 g_tog--;
			if(g_tog<RENDERDISPLAYTIMESTEP>>1) g_togflag = 0;
			
		}
		else if(g_tog<=2*RENDERDISPLAYTIMESTEP)
		{
			gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
		              IMGWIDTHMAX-g_frameWidth, IMGHEIGHTMAX, g_frameWidth, g_frameHeight,
		              GDK_RGB_DITHER_MAX, g_renderdisplay2, g_frameWidth * 3);
			if(g_togflag==0) g_tog++;
			else			 g_tog--;
		}
		else if(g_tog<=3*RENDERDISPLAYTIMESTEP)
		{
			gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
		              IMGWIDTHMAX-g_frameWidth, IMGHEIGHTMAX, g_frameWidth, g_frameHeight,
		              GDK_RGB_DITHER_MAX, g_renderdisplay3, g_frameWidth * 3);
			if(g_togflag==0) g_tog++;
			else			 g_tog--;
		}
		else if(g_tog<=4*RENDERDISPLAYTIMESTEP)
		{
			gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
		              IMGWIDTHMAX-g_frameWidth, IMGHEIGHTMAX, g_frameWidth, g_frameHeight,
		              GDK_RGB_DITHER_MAX, g_renderdisplay4, g_frameWidth * 3);
			if(g_togflag==0) g_tog++;
			else			 g_tog--;
		}
		else if(g_tog<=5*RENDERDISPLAYTIMESTEP)
		{
			gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
		              IMGWIDTHMAX-g_frameWidth, IMGHEIGHTMAX, g_frameWidth, g_frameHeight,
		              GDK_RGB_DITHER_MAX, g_renderdisplay5, g_frameWidth * 3);
			if(g_togflag==0) g_tog++;
			else			 g_tog--;
		}
		else if(g_tog<=6*RENDERDISPLAYTIMESTEP)
		{
			gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
		              IMGWIDTHMAX-g_frameWidth, IMGHEIGHTMAX, g_frameWidth, g_frameHeight,
		              GDK_RGB_DITHER_MAX, g_renderdisplay6, g_frameWidth * 3);
			if(g_togflag==0) g_tog++;
			else			 g_tog--;
		}
		else if(g_tog<=7*RENDERDISPLAYTIMESTEP)
		{
			gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
		              IMGWIDTHMAX-g_frameWidth, IMGHEIGHTMAX, g_frameWidth, g_frameHeight,
		              GDK_RGB_DITHER_MAX, g_renderdisplay7, g_frameWidth * 3);
			if(g_togflag==0) g_tog++;
			else			 g_tog--;
		}
		else //if(g_tog<=8*RENDERDISPLAYTIMESTEP)
		{
			gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
		              IMGWIDTHMAX-g_frameWidth, IMGHEIGHTMAX, g_frameWidth, g_frameHeight,
		              GDK_RGB_DITHER_MAX, g_renderdisplay8, g_frameWidth * 3);
			if(g_togflag==0) g_tog++;
			else			 g_tog--;
			if(g_tog>7*RENDERDISPLAYTIMESTEP+(RENDERDISPLAYTIMESTEP>>1)) g_togflag = 1;
		}
	}

	// segment process
	gtk_progress_bar_pulse(GTK_PROGRESS_BAR(pbar->processbar));
	
	if(g_setsegframesbarflag==1)
	{
		//printf(" time_handler segment process %.2lf\n", g_setsegframesbarval);
		gtk_progress_bar_set_fraction ( GTK_PROGRESS_BAR(pbar->processbar), (gdouble)(*(pbar->val)));
	}
	
	return 1;
}

#define GUI_DEPTHASSIGNMENT 0

/* depthassignment */
void get_depth_value(GtkWidget* button,SDepthSet *wdv)
{
	int depthmin, depthmax, depthmode;
	
    char *str=(char *)malloc(4*sizeof(char));
    str=(char*)gtk_entry_get_text(GTK_ENTRY(wdv->entrymin));
	depthmin = str2num(str);  
	if(depthmin<0)  depthmin=0;
	if(depthmin>255)  depthmin=255;
	str=(char*)gtk_entry_get_text(GTK_ENTRY(wdv->entrymax));
	depthmax = str2num(str);  
	if(depthmax<0)  depthmax=0;
	if(depthmax>255)  depthmax=255;
	
	if(depthmax<depthmin) depthmax = depthmin;

	str=(char*)gtk_entry_get_text(GTK_ENTRY(wdv->mode));
	depthmode = str2num(str);  
	if(depthmode<0)  depthmode=0;
	if(depthmode>=DEPTHMODEMAX)  depthmode=DEPTHMODEMAX-1;

	set_depth_value(g_frameIndex, wdv->label, depthmin, depthmax, depthmode,0);
	if(g_frameResizeFlag==1)
		set_depth_value(g_frameIndex, wdv->label, depthmin, depthmax, depthmode,1);
	
	ShowDep(g_frames[g_frameIndex].SegLabelMask);

    gtk_widget_destroy(wdv->window);

	//if(str) free(str);
}

void cancel_depth(GtkWidget *button,GtkWidget *window)
{
    gtk_widget_hide(window);
}

void Setdepth(int lb)
{

	int i;
	int depthmax, depthmin, depthavg;
	char str[128];
	char bufferl[128];

	GtkWidget *window,*entrymax, *entrymin, *mode;
    GtkWidget *label,*vbox,*hbox,*b_ok,*b_cancel;
	GtkWidget* image;

	
	int offset, idx;

	offset = lb - g_frameWidth*g_frameHeight;

	for(i=0;i<=g_frames[g_frameIndex].LayerVisibleNum;i++)
	{
		if((offset&(0x1<<i))!=0)
		{
			idx = i;
			break;
		}
	}
    
    window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request(window,360,460);
    gtk_window_set_title(GTK_WINDOW(window),TITLE_DA);
    g_signal_connect(G_OBJECT(window),"destroy",G_CALLBACK(gtk_widget_destroy),NULL);
    gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_NONE);

	// 计算默认深度值
	get_depth_default_value(lb, &depthmin, &depthmax, &depthavg);
    
    vbox=gtk_vbox_new(FALSE,15);
    gtk_container_add(GTK_CONTAINER(window),vbox);

    label=gtk_label_new(LABEL_IDRM);
    gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

	#ifdef USE_CHINESE
	sprintf_s(str, "(推荐值: %d - %d 均值 %d)",depthmin, depthmax, depthavg);
	#else
	sprintf_s(str, "(Prompt: %d - %d average %d)",depthmin, depthmax, depthavg);
	#endif
	label=gtk_label_new(INFO_STRINGS);
    gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

	hbox=gtk_hbox_new(TRUE,0);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(" Min(0-255):");
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
    entrymin=gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(hbox),entrymin,FALSE,FALSE,0);

	hbox=gtk_hbox_new(TRUE,0);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(" Max(0-255):");
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
    entrymax=gtk_entry_new();
	
    gtk_box_pack_start(GTK_BOX(hbox),entrymax,FALSE,FALSE,0);

	hbox=gtk_hbox_new(TRUE,0);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(LABEL_MSEL);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
    mode=gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(hbox),mode,FALSE,FALSE,0);

	if(g_frames[g_frameIndex].layerdepth[idx].setflag == 0)
	{
		sprintf_s(bufferl, "%d", depthmin);
		gtk_entry_set_text(GTK_ENTRY(entrymin), bufferl);
		sprintf_s(bufferl, "%d", depthmax);
		gtk_entry_set_text(GTK_ENTRY(entrymax), bufferl);
		sprintf_s(bufferl, "%d", 1);
		gtk_entry_set_text(GTK_ENTRY(mode), bufferl);
	}
	else
	{
		sprintf_s(bufferl, "%d", g_frames[g_frameIndex].layerdepth[idx].depthmin);
		gtk_entry_set_text(GTK_ENTRY(entrymin), bufferl);
		sprintf_s(bufferl, "%d", g_frames[g_frameIndex].layerdepth[idx].depthmax);
		gtk_entry_set_text(GTK_ENTRY(entrymax), bufferl);
		sprintf_s(bufferl, "%d", g_frames[g_frameIndex].layerdepth[idx].depthmode);
		gtk_entry_set_text(GTK_ENTRY(mode), bufferl);
	}

	hbox=gtk_hbox_new(TRUE,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(" 0:");
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	image=gtk_image_new_from_file("./image/depmode0.png");  
	gtk_box_pack_start(GTK_BOX(hbox),image,FALSE,FALSE,0);
	label=gtk_label_new(" 1:");
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	image=gtk_image_new_from_file("./image/depmode1.png");  
	gtk_box_pack_start(GTK_BOX(hbox),image,FALSE,FALSE,0);
	label=gtk_label_new(" 2:");
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	image=gtk_image_new_from_file("./image/depmode2.png");  
	gtk_box_pack_start(GTK_BOX(hbox),image,FALSE,FALSE,0);
	label=gtk_label_new(" 3:");
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	image=gtk_image_new_from_file("./image/depmode3.png");  
	gtk_box_pack_start(GTK_BOX(hbox),image,FALSE,FALSE,0);
	

	hbox=gtk_hbox_new(TRUE,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(" 4:");
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	image=gtk_image_new_from_file("./image/depmode4.png");  
	gtk_box_pack_start(GTK_BOX(hbox),image,FALSE,FALSE,0);
	label=gtk_label_new(" 5:");
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	image=gtk_image_new_from_file("./image/depmode5.png");  
	gtk_box_pack_start(GTK_BOX(hbox),image,FALSE,FALSE,0);
	label=gtk_label_new(" 6:");
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	image=gtk_image_new_from_file("./image/depmode6.png");  
	gtk_box_pack_start(GTK_BOX(hbox),image,FALSE,FALSE,0);
	label=gtk_label_new(" 7:");
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	image=gtk_image_new_from_file("./image/depmode7.png");  
	gtk_box_pack_start(GTK_BOX(hbox),image,FALSE,FALSE,0);
	
	hbox=gtk_hbox_new(TRUE,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(" 8:");
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	image=gtk_image_new_from_file("./image/depmode8.png");  
	gtk_box_pack_start(GTK_BOX(hbox),image,FALSE,FALSE,0);
	label=gtk_label_new(" 9:");
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	image=gtk_image_new_from_file("./image/depmode9.png");  
	gtk_box_pack_start(GTK_BOX(hbox),image,FALSE,FALSE,0);
	label=gtk_label_new("10:");
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	image=gtk_image_new_from_file("./image/depmode10.png");  
	gtk_box_pack_start(GTK_BOX(hbox),image,FALSE,FALSE,0);
	label=gtk_label_new("11:");
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	image=gtk_image_new_from_file("./image/depmode11.png");  
	gtk_box_pack_start(GTK_BOX(hbox),image,FALSE,FALSE,0);
	

	hbox=gtk_hbox_new(TRUE,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new("12:");
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	image=gtk_image_new_from_file("./image/depmode12.png");  
	gtk_box_pack_start(GTK_BOX(hbox),image,FALSE,FALSE,0);
	label=gtk_label_new("13:");
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	image=gtk_image_new_from_file("./image/depmode13.png");  
	gtk_box_pack_start(GTK_BOX(hbox),image,FALSE,FALSE,0);
	label=gtk_label_new("14:");
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	image=gtk_image_new_from_file("./image/depmode14.png");  
	gtk_box_pack_start(GTK_BOX(hbox),image,FALSE,FALSE,0);
	label=gtk_label_new("15:");
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	image=gtk_image_new_from_file("./image/depmode15.png");  
	gtk_box_pack_start(GTK_BOX(hbox),image,FALSE,FALSE,0);

	hbox=gtk_hbox_new(TRUE,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new("16:");
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	image=gtk_image_new_from_file("./image/depmode16.png");  
	gtk_box_pack_start(GTK_BOX(hbox),image,FALSE,FALSE,0);
	label=gtk_label_new("17:");
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	image=gtk_image_new_from_file("./image/depmode17.png");  
	gtk_box_pack_start(GTK_BOX(hbox),image,FALSE,FALSE,0);
	label=gtk_label_new("18:");
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	image=gtk_image_new_from_file("./image/depmode18.png");  
	gtk_box_pack_start(GTK_BOX(hbox),image,FALSE,FALSE,0);
	label=gtk_label_new("19:");
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	image=gtk_image_new_from_file("./image/depmode19.png");  
	gtk_box_pack_start(GTK_BOX(hbox),image,FALSE,FALSE,0);
	    
    SDepthSet *wdv=(SDepthSet *)malloc(sizeof(SDepthSet));
    wdv->window=window;
    wdv->entrymax=entrymax;
	wdv->entrymin=entrymin;
	wdv->mode=mode;
	wdv->label=lb;

	hbox=gtk_hbox_new(FALSE,50);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
    b_ok=gtk_button_new_with_label(CONF_OK);
    g_signal_connect(G_OBJECT(b_ok),"clicked",G_CALLBACK(get_depth_value),wdv);
    gtk_box_pack_start(GTK_BOX(hbox),b_ok,FALSE,FALSE,50);
    b_cancel=gtk_button_new_with_label(CONF_CANCEL);
    g_signal_connect(G_OBJECT(b_cancel),"clicked",G_CALLBACK(cancel_depth),window);
    gtk_box_pack_start(GTK_BOX(hbox),b_cancel,FALSE,FALSE,0);

    gtk_widget_show_all(window);	

	return ;
}

/*Depthprocessing*/
#define GUI_DEPTHPROCESSING 0

void get_depautogen_frame(GtkWidget* button,SFrameSelect *wdv)
{
	int depframefirst, depframelast;
	
	char *str=(char *)malloc(4*sizeof(char));

	str=(char*)gtk_entry_get_text(GTK_ENTRY(wdv->entryfirst));
	depframefirst = str2num(str);  
	if(depframefirst<1)  depframefirst=1;
	if(depframefirst>g_frameMaxNum)  depframefirst=g_frameMaxNum;
	str=(char*)gtk_entry_get_text(GTK_ENTRY(wdv->entrylast));
	depframelast = str2num(str);  
	if(depframelast<1)	depframelast=1;
	if(depframelast>g_frameMaxNum)	depframelast=g_frameMaxNum;

	if(depframefirst>depframelast)
		depframelast = depframefirst;

	depthautogen(depframefirst-1, depframelast-1);

	gtk_widget_hide(wdv->window);

}

void get_depproc_frame(GtkWidget* button,SFrameSelect *wdv)
{
	//int i;
	int depframefirst, depframelast;
	//SFrameJump stmp;
	
    char *str=(char *)malloc(4*sizeof(char));

	str=(char*)gtk_entry_get_text(GTK_ENTRY(wdv->entryfirst));
	depframefirst = str2num(str);  
	if(depframefirst<1)  depframefirst=1;
	if(depframefirst>g_frameMaxNum)  depframefirst=g_frameMaxNum;
	str=(char*)gtk_entry_get_text(GTK_ENTRY(wdv->entrylast));
	depframelast = str2num(str);  
	if(depframelast<1)  depframelast=1;
	if(depframelast>g_frameMaxNum)  depframelast=g_frameMaxNum;

	if(depframefirst>depframelast)
		depframelast = depframefirst;

	//printf("%d %d %d\n", infframefirst, infframelast, refframenum);
	//stmp.jumpmode = JMPMODECUR;
	//for(i=depframefirst;i<=depframelast;i++)
	//{
		//sprintf(str,"%d", i); 
		//gtk_entry_set_text(GTK_ENTRY(g_wdgInpFrameNum), str);
		//g_frameIndex = i-1;
	depthimgproc(depframefirst-1, depframelast-1);
	//}

	//if(g_enlargeflag==0)
		//jumpframe(NULL,&stmp);

    gtk_widget_hide(wdv->window);

	//if(str) free(str);
}

void cancel_window(GtkWidget *button,GtkWidget *window)
{
	gtk_widget_hide(window);
}

void get_depinfe_frame(GtkWidget* button,SFrameSelect *wdv)
{
	int i;
	int depframefirst, depframelast;
	SFrameJump stmp;
	
    char *str=(char *)malloc(4*sizeof(char));

	str=(char*)gtk_entry_get_text(GTK_ENTRY(wdv->entryfirst));
	depframefirst = str2num(str);  
	if(depframefirst<1)  depframefirst=1;
	if(depframefirst>g_frameMaxNum)  depframefirst=g_frameMaxNum;
	str=(char*)gtk_entry_get_text(GTK_ENTRY(wdv->entrylast));
	depframelast = str2num(str);  
	if(depframelast<1)  depframelast=1;
	if(depframelast>g_frameMaxNum)  depframelast=g_frameMaxNum;

	if(depframefirst>depframelast)
		depframelast = depframefirst;

	//printf("%d %d %d\n", infframefirst, infframelast, refframenum);
	stmp.jumpmode = JMPMODECUR;
	for(i=depframefirst;i<=depframelast;i++)
	{
		depthimginfe(g_frameIndex, i-1);
	}
	//jumpframe(NULL,&stmp);

    gtk_widget_hide(wdv->window);

	//if(str) free(str);
}

void cancel_depinfe(GtkWidget *button,GtkWidget *window)
{
	gtk_widget_hide(window);
}

void get_layerdepinfe_frame(GtkWidget* button,SLayerDepthInfe *wdv)
{
	int depframefirst, depframelast, layernum, firstmin, firstmax, lastmin, lastmax, depthmode;

	//char str[12];
    char *str;//=(char *)malloc(12*sizeof(char));

	str=(char*)gtk_entry_get_text(GTK_ENTRY(wdv->entryfirst));
	depframefirst = str2num(str);  
	if(depframefirst<1)  depframefirst=1;
	if(depframefirst>g_frameMaxNum)  depframefirst=g_frameMaxNum;
	str=(char*)gtk_entry_get_text(GTK_ENTRY(wdv->entrylast));
	depframelast = str2num(str);  
	if(depframelast<1)  depframelast=1;
	if(depframelast>g_frameMaxNum)  depframelast=g_frameMaxNum;

	if(depframefirst>depframelast)
		depframelast = depframefirst;

	str=(char*)gtk_entry_get_text(GTK_ENTRY(wdv->entrylayernum));
	layernum = str2num(str);  
	if(layernum<1)  layernum=1;
	if(layernum>LAYERBUTTONNUMMAX)  layernum=LAYERBUTTONNUMMAX;

	str=(char*)gtk_entry_get_text(GTK_ENTRY(wdv->entryfirstdmin));
	firstmin = str2num(str);  
	if(firstmin<0)  firstmin=0;
	if(firstmin>255)  firstmin=255;
	str=(char*)gtk_entry_get_text(GTK_ENTRY(wdv->entryfirstdmax));
	firstmax = str2num(str);  
	if(firstmax<0)  firstmax=0;
	if(firstmax>255)  firstmax=255;

	if(firstmax<firstmin) firstmax = firstmin;

	str=(char*)gtk_entry_get_text(GTK_ENTRY(wdv->entrylastdmin));
	lastmin = str2num(str);  
	if(lastmin<0)  lastmin=0;
	if(lastmin>255)  lastmin=255;
	str=(char*)gtk_entry_get_text(GTK_ENTRY(wdv->entrylastdmax));
	lastmax = str2num(str);  
	if(lastmax<0)  lastmax=0;
	if(lastmax>255)  lastmax=255;

	if(lastmax<lastmin) lastmax = lastmin;

	str=(char*)gtk_entry_get_text(GTK_ENTRY(wdv->entrydepthmode));
	depthmode = str2num(str);  
	if(depthmode<0)  depthmode=0;
	if(depthmode>=DEPTHMODEMAX)  depthmode=DEPTHMODEMAX-1;
	
	depthlayerinfe(layernum, depframefirst-1, depframelast-1, firstmin, firstmax, lastmin, lastmax, depthmode);

    gtk_widget_hide(wdv->window);

	//if(str) free(str);
}

void cancel_layerdepinfe(GtkWidget *button,GtkWidget *window)
{
	gtk_widget_hide(window);
}

void get_autodepinfe_frame(GtkWidget* button,SAutoDepthInfe *wdv)
{
	int depframefirst, depframelast, reffirst, reflast, infemode;

	//char str[12];
    char *str;//=(char *)malloc(12*sizeof(char));

	str=(char*)gtk_entry_get_text(GTK_ENTRY(wdv->entryfirst));
	depframefirst = str2num(str);  
	if(depframefirst<1)  depframefirst=1;
	if(depframefirst>g_frameMaxNum)  depframefirst=g_frameMaxNum;
	str=(char*)gtk_entry_get_text(GTK_ENTRY(wdv->entrylast));
	depframelast = str2num(str);  
	if(depframelast<1)  depframelast=1;
	if(depframelast>g_frameMaxNum)  depframelast=g_frameMaxNum;

	if(depframefirst>depframelast)
		depframelast = depframefirst;

	/* 参考序号为0 表示不参考该帧*/
	str=(char*)gtk_entry_get_text(GTK_ENTRY(wdv->entryreffirst));
	reffirst = str2num(str);  
	if(reffirst<1)  reffirst=0;
	if(reffirst>g_frameMaxNum)  reffirst=0;
	str=(char*)gtk_entry_get_text(GTK_ENTRY(wdv->entryreflast));
	reflast = str2num(str);  
	if(reflast<1)  reflast=0;
	if(reflast>g_frameMaxNum)  reflast=0;

	if(reffirst>reflast) reflast = 0;

	/* 参考帧必须与被推理帧不同*/
	if(reffirst>=depframefirst) reffirst = 0;
	if(reflast<=depframelast) reflast = 0;

	str=(char*)gtk_entry_get_text(GTK_ENTRY(wdv->entryinfemode));
	infemode = str2num(str);  
	if(infemode!=0)  infemode=1;
	
	depthimginfeauto(depframefirst-1, depframelast-1, reffirst-1, reflast-1, infemode);

    gtk_widget_hide(wdv->window);

	//if(str) free(str);
}

void cancel_autodepinfe(GtkWidget *button,GtkWidget *window)
{
	gtk_widget_hide(window);
}


void get_depframemodule_frame(GtkWidget* button,SBaseDepthSelect *wdv)
{
	int depframefirst, depframelast;
	int depthmin, depthmax, depthmode;
	    
	char *str=(char *)malloc(4*sizeof(char));

	str=(char*)gtk_entry_get_text(GTK_ENTRY(wdv->entryfirst));
	depframefirst = str2num(str);  
	if(depframefirst<1)  depframefirst=1;
	if(depframefirst>g_frameMaxNum)  depframefirst=g_frameMaxNum;
	str=(char*)gtk_entry_get_text(GTK_ENTRY(wdv->entrylast));
	depframelast = str2num(str);  
	if(depframelast<1)	depframelast=1;
	if(depframelast>g_frameMaxNum)	depframelast=g_frameMaxNum;

	if(depframefirst>depframelast)
		depframelast = depframefirst;

	str=(char*)gtk_entry_get_text(GTK_ENTRY(wdv->entrydepmin));
	depthmin = str2num(str);  
	if(depthmin<0)  depthmin=0;
	if(depthmin>255)  depthmin=255;
	str=(char*)gtk_entry_get_text(GTK_ENTRY(wdv->entrydepmax));
	depthmax = str2num(str);  
	if(depthmax<0)  depthmax=0;
	if(depthmax>255)  depthmax=255;
	
	if(depthmax<depthmin) depthmax = depthmin;

	str=(char*)gtk_entry_get_text(GTK_ENTRY(wdv->entrydepmod));
	depthmode = str2num(str);  
	if(depthmode<0)  depthmode=0;
	if(depthmode>=DEPTHMODEMAX)  depthmode=DEPTHMODEMAX-1;

	//set_depth_value(g_frameIndex, wdv->label, depthmin, depthmax, depthmode);
	//if(g_frameResizeFlag==1)
		//set_depth_value0(g_frameIndex, wdv->label, depthmin, depthmax, depthmode);
	
	//ShowDep(g_frames[g_frameIndex].SegLabelMask);

	depthframemodule(depframefirst-1, depframelast-1, depthmin, depthmax, depthmode);

	gtk_widget_hide(wdv->window);

}

void getframemoduleframe(GtkWidget * button,GtkWidget * data)
{
	char buffer[128];

	GtkWidget *window, *entryfirst, *entrylast, *entrymax, *entrymin, *mode, * image;
    GtkWidget *label,*vbox,*hbox,*b_ok,*b_cancel, *separator;
	
	if(g_frameMaxNum>0)
	{
		window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
		gtk_widget_set_size_request(window,360,480);
		gtk_window_set_title(GTK_WINDOW(window),TITLE_FDMFS);
		g_signal_connect(G_OBJECT(window),"destroy",G_CALLBACK(gtk_widget_destroy),NULL);
		gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_NONE);

    
		vbox=gtk_vbox_new(FALSE,15);
		gtk_container_add(GTK_CONTAINER(window),vbox);

		separator = gtk_hseparator_new();
		gtk_box_pack_start(GTK_BOX(vbox),separator,FALSE,FALSE,0);
	
		label=gtk_label_new(LABEL_SELFS);
		gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

		hbox=gtk_hbox_new(FALSE,50);
		gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
		label=gtk_label_new(LABEL_FROM);
		gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
		entryfirst= gtk_entry_new();
		gtk_widget_set_size_request( entryfirst, 70, 25);
		gtk_box_pack_start(GTK_BOX(hbox),entryfirst,FALSE,FALSE,0);
		sprintf(buffer, "%d", g_frameIndex+1);
		gtk_entry_set_text(GTK_ENTRY(entryfirst), buffer);
		label=gtk_label_new(LABEL_TO);
		gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
		entrylast= gtk_entry_new();
		gtk_widget_set_size_request( entrylast,70, 25);
		gtk_box_pack_start(GTK_BOX(hbox),entrylast,FALSE,FALSE,0);
		gtk_entry_set_text(GTK_ENTRY(entrylast), buffer);

		label=gtk_label_new(LABEL_IDRM);
		gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

		hbox=gtk_hbox_new(TRUE,0);
		gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
		label=gtk_label_new(" Min(0-255):");
		gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
		entrymin=gtk_entry_new();
		gtk_box_pack_start(GTK_BOX(hbox),entrymin,FALSE,FALSE,0);

		hbox=gtk_hbox_new(TRUE,0);
		gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
		label=gtk_label_new(" Max(0-255):");
		gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
		entrymax=gtk_entry_new();
	
		gtk_box_pack_start(GTK_BOX(hbox),entrymax,FALSE,FALSE,0);

		hbox=gtk_hbox_new(TRUE,0);
		gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
		label=gtk_label_new(LABEL_MSEL);
		gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
		mode=gtk_entry_new();
		gtk_box_pack_start(GTK_BOX(hbox),mode,FALSE,FALSE,0);

		if(g_frames[g_frameIndex].framedepth.setflag == 0)
		{
			sprintf_s(buffer, "%d", 0);
			gtk_entry_set_text(GTK_ENTRY(entrymin), buffer);
			sprintf_s(buffer, "%d", 255);
			gtk_entry_set_text(GTK_ENTRY(entrymax), buffer);
			sprintf_s(buffer, "%d", 1);
			gtk_entry_set_text(GTK_ENTRY(mode), buffer);
		}
		else
		{
			sprintf_s(buffer, "%d", g_frames[g_frameIndex].framedepth.depthmin);
			gtk_entry_set_text(GTK_ENTRY(entrymin), buffer);
			sprintf_s(buffer, "%d", g_frames[g_frameIndex].framedepth.depthmax);
			gtk_entry_set_text(GTK_ENTRY(entrymax), buffer);
			sprintf_s(buffer, "%d", g_frames[g_frameIndex].framedepth.depthmode);
			gtk_entry_set_text(GTK_ENTRY(mode), buffer);
		}


		hbox=gtk_hbox_new(TRUE,0);
		gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
		label=gtk_label_new(" 0:");
		gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
		image=gtk_image_new_from_file("./image/depmode0.png");  
		gtk_box_pack_start(GTK_BOX(hbox),image,FALSE,FALSE,0);
		label=gtk_label_new(" 1:");
		gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
		image=gtk_image_new_from_file("./image/depmode1.png");  
		gtk_box_pack_start(GTK_BOX(hbox),image,FALSE,FALSE,0);
		label=gtk_label_new(" 2:");
		gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
		image=gtk_image_new_from_file("./image/depmode2.png");  
		gtk_box_pack_start(GTK_BOX(hbox),image,FALSE,FALSE,0);
		label=gtk_label_new(" 3:");
		gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
		image=gtk_image_new_from_file("./image/depmode3.png");  
		gtk_box_pack_start(GTK_BOX(hbox),image,FALSE,FALSE,0);
	

		hbox=gtk_hbox_new(TRUE,0);
		gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
		label=gtk_label_new(" 4:");
		gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
		image=gtk_image_new_from_file("./image/depmode4.png");  
		gtk_box_pack_start(GTK_BOX(hbox),image,FALSE,FALSE,0);
		label=gtk_label_new(" 5:");
		gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
		image=gtk_image_new_from_file("./image/depmode5.png");  
		gtk_box_pack_start(GTK_BOX(hbox),image,FALSE,FALSE,0);
		label=gtk_label_new(" 6:");
		gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
		image=gtk_image_new_from_file("./image/depmode6.png");  
		gtk_box_pack_start(GTK_BOX(hbox),image,FALSE,FALSE,0);
		label=gtk_label_new(" 7:");
		gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
		image=gtk_image_new_from_file("./image/depmode7.png");  
		gtk_box_pack_start(GTK_BOX(hbox),image,FALSE,FALSE,0);
	
		hbox=gtk_hbox_new(TRUE,0);
		gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
		label=gtk_label_new(" 8:");
		gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
		image=gtk_image_new_from_file("./image/depmode8.png");  
		gtk_box_pack_start(GTK_BOX(hbox),image,FALSE,FALSE,0);
		label=gtk_label_new(" 9:");
		gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
		image=gtk_image_new_from_file("./image/depmode9.png");  
		gtk_box_pack_start(GTK_BOX(hbox),image,FALSE,FALSE,0);
		label=gtk_label_new("10:");
		gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
		image=gtk_image_new_from_file("./image/depmode10.png");  
		gtk_box_pack_start(GTK_BOX(hbox),image,FALSE,FALSE,0);
		label=gtk_label_new("11:");
		gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
		image=gtk_image_new_from_file("./image/depmode11.png");  
		gtk_box_pack_start(GTK_BOX(hbox),image,FALSE,FALSE,0);
	

		hbox=gtk_hbox_new(TRUE,0);
		gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
		label=gtk_label_new("12:");
		gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
		image=gtk_image_new_from_file("./image/depmode12.png");  
		gtk_box_pack_start(GTK_BOX(hbox),image,FALSE,FALSE,0);
		label=gtk_label_new("13:");
		gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
		image=gtk_image_new_from_file("./image/depmode13.png");  
		gtk_box_pack_start(GTK_BOX(hbox),image,FALSE,FALSE,0);
		label=gtk_label_new("14:");
		gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
		image=gtk_image_new_from_file("./image/depmode14.png");  
		gtk_box_pack_start(GTK_BOX(hbox),image,FALSE,FALSE,0);
		label=gtk_label_new("15:");
		gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
		image=gtk_image_new_from_file("./image/depmode15.png");  
		gtk_box_pack_start(GTK_BOX(hbox),image,FALSE,FALSE,0);

		hbox=gtk_hbox_new(TRUE,0);
		gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
		label=gtk_label_new("16:");
		gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
		image=gtk_image_new_from_file("./image/depmode16.png");  
		gtk_box_pack_start(GTK_BOX(hbox),image,FALSE,FALSE,0);
		label=gtk_label_new("17:");
		gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
		image=gtk_image_new_from_file("./image/depmode17.png");  
		gtk_box_pack_start(GTK_BOX(hbox),image,FALSE,FALSE,0);
		label=gtk_label_new("18:");
		gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
		image=gtk_image_new_from_file("./image/depmode18.png");  
		gtk_box_pack_start(GTK_BOX(hbox),image,FALSE,FALSE,0);
		label=gtk_label_new("19:");
		gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
		image=gtk_image_new_from_file("./image/depmode19.png");  
		gtk_box_pack_start(GTK_BOX(hbox),image,FALSE,FALSE,0);

		SBaseDepthSelect *wdv=(SBaseDepthSelect *)malloc(sizeof(SBaseDepthSelect));
		wdv->window=window;
		wdv->entryfirst=entryfirst;
		wdv->entrylast=entrylast;
		wdv->entrydepmin=entrymin;
		wdv->entrydepmax=entrymax;
		wdv->entrydepmod=mode;

		hbox=gtk_hbox_new(FALSE,50);
		gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
		b_ok=gtk_button_new_with_label(CONF_OK);
		g_signal_connect(G_OBJECT(b_ok),"clicked",G_CALLBACK(get_depframemodule_frame),wdv);
		gtk_box_pack_start(GTK_BOX(hbox),b_ok,FALSE,FALSE,50);
		b_cancel=gtk_button_new_with_label(CONF_CANCEL);
		g_signal_connect(G_OBJECT(b_cancel),"clicked",G_CALLBACK(cancel_window),window);
		gtk_box_pack_start(GTK_BOX(hbox),b_cancel,FALSE,FALSE,0);

		gtk_widget_show_all(window);
	}

}

void get_baseassign_frame(GtkWidget* button,SFrameSelect *wdv)
{
	int depframefirst, depframelast;
	
	char *str=(char *)malloc(4*sizeof(char));

	str=(char*)gtk_entry_get_text(GTK_ENTRY(wdv->entryfirst));
	depframefirst = str2num(str);  
	if(depframefirst<1)  depframefirst=1;
	if(depframefirst>g_frameMaxNum)  depframefirst=g_frameMaxNum;
	str=(char*)gtk_entry_get_text(GTK_ENTRY(wdv->entrylast));
	depframelast = str2num(str);  
	if(depframelast<1)	depframelast=1;
	if(depframelast>g_frameMaxNum)	depframelast=g_frameMaxNum;

	if(depframefirst>depframelast)
		depframelast = depframefirst;

	depthbaseassign(depframefirst-1, depframelast-1);

	gtk_widget_hide(wdv->window);

}

void getbaseassignframe(GtkWidget * button,GtkWidget * data)
{
	char buffer[128];

	GtkWidget *window, *entryfirst, *entrylast;
    GtkWidget *label,*vbox,*hbox,*b_ok,*b_cancel, *separator;
	
	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request(window,360,150);
    gtk_window_set_title(GTK_WINDOW(window),TITLE_AGBDAFS);
    g_signal_connect(G_OBJECT(window),"destroy",G_CALLBACK(gtk_widget_destroy),NULL);
    gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_NONE);

    
    vbox=gtk_vbox_new(FALSE,15);
    gtk_container_add(GTK_CONTAINER(window),vbox);

	separator = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox),separator,FALSE,FALSE,0);
	
	label=gtk_label_new(LABEL_SELFS);
    gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

	hbox=gtk_hbox_new(FALSE,50);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(LABEL_FROM);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	entryfirst= gtk_entry_new();
	gtk_widget_set_size_request( entryfirst, 70, 25);
    gtk_box_pack_start(GTK_BOX(hbox),entryfirst,FALSE,FALSE,0);
	sprintf(buffer, "%d", g_frameIndex+1);
	gtk_entry_set_text(GTK_ENTRY(entryfirst), buffer);
	label=gtk_label_new(LABEL_TO);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	entrylast= gtk_entry_new();
	gtk_widget_set_size_request( entrylast,70, 25);
    gtk_box_pack_start(GTK_BOX(hbox),entrylast,FALSE,FALSE,0);
	gtk_entry_set_text(GTK_ENTRY(entrylast), buffer);

	SFrameSelect *wdv=(SFrameSelect *)malloc(sizeof(SFrameSelect));
    wdv->window=window;
	wdv->entryfirst=entryfirst;
	wdv->entrylast=entrylast;

	hbox=gtk_hbox_new(FALSE,50);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
    b_ok=gtk_button_new_with_label(CONF_OK);
    g_signal_connect(G_OBJECT(b_ok),"clicked",G_CALLBACK(get_baseassign_frame),wdv);
    gtk_box_pack_start(GTK_BOX(hbox),b_ok,FALSE,FALSE,50);
    b_cancel=gtk_button_new_with_label(CONF_CANCEL);
    g_signal_connect(G_OBJECT(b_cancel),"clicked",G_CALLBACK(cancel_window),window);
    gtk_box_pack_start(GTK_BOX(hbox),b_cancel,FALSE,FALSE,0);

    gtk_widget_show_all(window);

}



void getdepautogenframe(GtkWidget * button,GtkWidget * data)
{
	char buffer[128];

	GtkWidget *window, *entryfirst, *entrylast;
    GtkWidget *label,*vbox,*hbox,*b_ok,*b_cancel, *separator;
	
	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request(window,360,150);
    gtk_window_set_title(GTK_WINDOW(window),TITLE_DAGFS);
    g_signal_connect(G_OBJECT(window),"destroy",G_CALLBACK(gtk_widget_destroy),NULL);
    gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_NONE);

    
    vbox=gtk_vbox_new(FALSE,15);
    gtk_container_add(GTK_CONTAINER(window),vbox);

	separator = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox),separator,FALSE,FALSE,0);
	
	label=gtk_label_new(LABEL_SELFS);
    gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

	hbox=gtk_hbox_new(FALSE,50);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(LABEL_FROM);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	entryfirst= gtk_entry_new();
	gtk_widget_set_size_request( entryfirst, 70, 25);
    gtk_box_pack_start(GTK_BOX(hbox),entryfirst,FALSE,FALSE,0);
	sprintf(buffer, "%d", g_frameIndex+1);
	gtk_entry_set_text(GTK_ENTRY(entryfirst), buffer);
	label=gtk_label_new(LABEL_TO);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	entrylast= gtk_entry_new();
	gtk_widget_set_size_request( entrylast,70, 25);
    gtk_box_pack_start(GTK_BOX(hbox),entrylast,FALSE,FALSE,0);
	gtk_entry_set_text(GTK_ENTRY(entrylast), buffer);

	SFrameSelect *wdv=(SFrameSelect *)malloc(sizeof(SFrameSelect));
    wdv->window=window;
	wdv->entryfirst=entryfirst;
	wdv->entrylast=entrylast;

	hbox=gtk_hbox_new(FALSE,50);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
    b_ok=gtk_button_new_with_label(CONF_OK);
    g_signal_connect(G_OBJECT(b_ok),"clicked",G_CALLBACK(get_depautogen_frame),wdv);
    gtk_box_pack_start(GTK_BOX(hbox),b_ok,FALSE,FALSE,50);
    b_cancel=gtk_button_new_with_label(CONF_CANCEL);
    g_signal_connect(G_OBJECT(b_cancel),"clicked",G_CALLBACK(cancel_window),window);
    gtk_box_pack_start(GTK_BOX(hbox),b_cancel,FALSE,FALSE,0);

    gtk_widget_show_all(window);

}

void getdepprocframe(GtkWidget * button,GtkWidget * data)
{
	char buffer[128];

	GtkWidget *window, *entryfirst, *entrylast;
    GtkWidget *label,*vbox,*hbox,*b_ok,*b_cancel, *separator;
	
	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request(window,360,150);
    gtk_window_set_title(GTK_WINDOW(window),TITLE_DPFS);
    g_signal_connect(G_OBJECT(window),"destroy",G_CALLBACK(gtk_widget_destroy),NULL);
    gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_NONE);

    
    vbox=gtk_vbox_new(FALSE,15);
    gtk_container_add(GTK_CONTAINER(window),vbox);

	separator = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox),separator,FALSE,FALSE,0);
	
	label=gtk_label_new(LABEL_SELFS);
    gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

	hbox=gtk_hbox_new(FALSE,50);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(LABEL_FROM);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	entryfirst= gtk_entry_new();
	gtk_widget_set_size_request( entryfirst, 70, 25);
    gtk_box_pack_start(GTK_BOX(hbox),entryfirst,FALSE,FALSE,0);
	sprintf(buffer, "%d", g_frameIndex+1);
	gtk_entry_set_text(GTK_ENTRY(entryfirst), buffer);
	label=gtk_label_new(LABEL_TO);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	entrylast= gtk_entry_new();
	gtk_widget_set_size_request( entrylast,70, 25);
    gtk_box_pack_start(GTK_BOX(hbox),entrylast,FALSE,FALSE,0);
	gtk_entry_set_text(GTK_ENTRY(entrylast), buffer);

	SFrameSelect *wdv=(SFrameSelect *)malloc(sizeof(SFrameSelect));
    wdv->window=window;
	wdv->entryfirst=entryfirst;
	wdv->entrylast=entrylast;

	hbox=gtk_hbox_new(FALSE,50);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
    b_ok=gtk_button_new_with_label(CONF_OK);
    g_signal_connect(G_OBJECT(b_ok),"clicked",G_CALLBACK(get_depproc_frame),wdv);
    gtk_box_pack_start(GTK_BOX(hbox),b_ok,FALSE,FALSE,50);
    b_cancel=gtk_button_new_with_label(CONF_CANCEL);
    g_signal_connect(G_OBJECT(b_cancel),"clicked",G_CALLBACK(cancel_window),window);
    gtk_box_pack_start(GTK_BOX(hbox),b_cancel,FALSE,FALSE,0);

    gtk_widget_show_all(window);

}

void getdepinfeframe(GtkWidget * button,GtkWidget * data)
{
	char buffer[128];

	GtkWidget *window, *entryfirst, *entrylast;
    GtkWidget *label,*vbox,*hbox,*b_ok,*b_cancel, *separator;
	
	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request(window,360,150);
    gtk_window_set_title(GTK_WINDOW(window),TITLE_DIFS);
    g_signal_connect(G_OBJECT(window),"destroy",G_CALLBACK(gtk_widget_destroy),NULL);
    gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_NONE);

    
    vbox=gtk_vbox_new(FALSE,15);
    gtk_container_add(GTK_CONTAINER(window),vbox);

	separator = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox),separator,FALSE,FALSE,0);
	
	label=gtk_label_new(LABEL_SELFS);
    gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

	hbox=gtk_hbox_new(FALSE,50);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(LABEL_FROM);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	entryfirst= gtk_entry_new();
	gtk_widget_set_size_request( entryfirst, 70, 25);
    gtk_box_pack_start(GTK_BOX(hbox),entryfirst,FALSE,FALSE,0);
	sprintf(buffer, "%d", g_frameIndex+1);
	gtk_entry_set_text(GTK_ENTRY(entryfirst), buffer);
	label=gtk_label_new(LABEL_TO);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	entrylast= gtk_entry_new();
	gtk_widget_set_size_request( entrylast,70, 25);
    gtk_box_pack_start(GTK_BOX(hbox),entrylast,FALSE,FALSE,0);
	gtk_entry_set_text(GTK_ENTRY(entrylast), buffer);

	SFrameSelect *wdv=(SFrameSelect *)malloc(sizeof(SFrameSelect));
    wdv->window=window;
	wdv->entryfirst=entryfirst;
	wdv->entrylast=entrylast;

	hbox=gtk_hbox_new(FALSE,50);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
    b_ok=gtk_button_new_with_label(CONF_OK);
    g_signal_connect(G_OBJECT(b_ok),"clicked",G_CALLBACK(get_depinfe_frame),wdv);
    gtk_box_pack_start(GTK_BOX(hbox),b_ok,FALSE,FALSE,50);
    b_cancel=gtk_button_new_with_label(CONF_CANCEL);
    g_signal_connect(G_OBJECT(b_cancel),"clicked",G_CALLBACK(cancel_depinfe),window);
    gtk_box_pack_start(GTK_BOX(hbox),b_cancel,FALSE,FALSE,0);

    gtk_widget_show_all(window);

}

void getlayerdepinfeframe(GtkWidget * button,GtkWidget * data)
{
	char buffer[128];

	GtkWidget *window, *entryfirst, *entrylast, *entrylayernum, *entryfirstdmin, *entryfirstdmax, *entrylastdmin, *entrylastdmax, *entrydepthmode;
    GtkWidget *label,*vbox,*hbox,*b_ok,*b_cancel, *separator;
	
	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request(window,360,300);
    gtk_window_set_title(GTK_WINDOW(window),TITLE_LDIFS);
    g_signal_connect(G_OBJECT(window),"destroy",G_CALLBACK(gtk_widget_destroy),NULL);
    gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_NONE);

    
    vbox=gtk_vbox_new(FALSE,15);
    gtk_container_add(GTK_CONTAINER(window),vbox);

	separator = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox),separator,FALSE,FALSE,0);
	
	label=gtk_label_new(LABEL_SELFS);
    gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

	hbox=gtk_hbox_new(FALSE,50);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(LABEL_FROM);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	entryfirst= gtk_entry_new();
	gtk_widget_set_size_request( entryfirst, 50, 25);
    gtk_box_pack_start(GTK_BOX(hbox),entryfirst,FALSE,FALSE,0);
	sprintf(buffer, "%d", g_frameIndex+1);
	gtk_entry_set_text(GTK_ENTRY(entryfirst), buffer);
	label=gtk_label_new(LABEL_TO);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	entrylast= gtk_entry_new();
	gtk_widget_set_size_request( entrylast,50, 25);
    gtk_box_pack_start(GTK_BOX(hbox),entrylast,FALSE,FALSE,0);
	gtk_entry_set_text(GTK_ENTRY(entrylast), buffer);

	hbox=gtk_hbox_new(FALSE,50);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(LABEL_LN);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	entrylayernum= gtk_entry_new();
	gtk_widget_set_size_request( entrylayernum, 50, 25);
    gtk_box_pack_start(GTK_BOX(hbox),entrylayernum,FALSE,FALSE,0);

	hbox=gtk_hbox_new(FALSE,50);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new("Min(0-255)");
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	entryfirstdmin= gtk_entry_new();
	gtk_widget_set_size_request( entryfirstdmin, 50, 25);
    gtk_box_pack_start(GTK_BOX(hbox),entryfirstdmin,FALSE,FALSE,0);
	//sprintf(buffer, "%d", g_frameIndex+1);
	//gtk_entry_set_text(GTK_ENTRY(entryfirst), buffer);
	label=gtk_label_new("------");
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	entrylastdmin= gtk_entry_new();
	gtk_widget_set_size_request( entrylastdmin,50, 25);
    gtk_box_pack_start(GTK_BOX(hbox),entrylastdmin,FALSE,FALSE,0);
	//gtk_entry_set_text(GTK_ENTRY(entrylastdmin), buffer);

	hbox=gtk_hbox_new(FALSE,50);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new("Max(0-255)");
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	entryfirstdmax= gtk_entry_new();
	gtk_widget_set_size_request( entryfirstdmax, 50, 25);
    gtk_box_pack_start(GTK_BOX(hbox),entryfirstdmax,FALSE,FALSE,0);
	label=gtk_label_new("------");
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	entrylastdmax= gtk_entry_new();
	gtk_widget_set_size_request( entrylastdmax,50, 25);
    gtk_box_pack_start(GTK_BOX(hbox),entrylastdmax,FALSE,FALSE,0);

	hbox=gtk_hbox_new(FALSE,50);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(LABEL_MSEL);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	entrydepthmode= gtk_entry_new();
	gtk_widget_set_size_request( entrydepthmode, 50, 25);
    gtk_box_pack_start(GTK_BOX(hbox),entrydepthmode,FALSE,FALSE,0);

	SLayerDepthInfe *wdv=(SLayerDepthInfe*)malloc(sizeof(SLayerDepthInfe));
    wdv->window=window;
	wdv->entryfirst=entryfirst;
	wdv->entrylast=entrylast;
	wdv->entrylayernum=entrylayernum;
	wdv->entryfirstdmin=entryfirstdmin;
	wdv->entrylastdmin=entrylastdmin;
	wdv->entryfirstdmax=entryfirstdmax;
	wdv->entrylastdmax=entrylastdmax;
	wdv->entrydepthmode=entrydepthmode;

	hbox=gtk_hbox_new(FALSE,50);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
    b_ok=gtk_button_new_with_label(CONF_OK);
    g_signal_connect(G_OBJECT(b_ok),"clicked",G_CALLBACK(get_layerdepinfe_frame),wdv);
    gtk_box_pack_start(GTK_BOX(hbox),b_ok,FALSE,FALSE,50);
    b_cancel=gtk_button_new_with_label(CONF_CANCEL);
    g_signal_connect(G_OBJECT(b_cancel),"clicked",G_CALLBACK(cancel_layerdepinfe),window);
    gtk_box_pack_start(GTK_BOX(hbox),b_cancel,FALSE,FALSE,0);

    gtk_widget_show_all(window);

}

void getautodepinfeframe(GtkWidget * button,GtkWidget * data)
{
	char buffer[128];

	GtkWidget *window, *entryfirst, *entrylast, *entryreffirst, *entryreflast, *entryinfemode;
    GtkWidget *label,*vbox,*hbox,*b_ok,*b_cancel, *separator;
	
	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request(window,360,200);
    gtk_window_set_title(GTK_WINDOW(window),TITLE_ADIFS);
    g_signal_connect(G_OBJECT(window),"destroy",G_CALLBACK(gtk_widget_destroy),NULL);
    gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_NONE);

    
    vbox=gtk_vbox_new(FALSE,15);
    gtk_container_add(GTK_CONTAINER(window),vbox);

	separator = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox),separator,FALSE,FALSE,0);
	
	label=gtk_label_new(LABEL_SELFS);
    gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

	hbox=gtk_hbox_new(FALSE,50);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(LABEL_FROM);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	entryfirst= gtk_entry_new();
	gtk_widget_set_size_request( entryfirst, 50, 25);
    gtk_box_pack_start(GTK_BOX(hbox),entryfirst,FALSE,FALSE,0);
	sprintf(buffer, "%d", g_frameIndex+1);
	gtk_entry_set_text(GTK_ENTRY(entryfirst), buffer);
	label=gtk_label_new(LABEL_TO);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	entrylast= gtk_entry_new();
	gtk_widget_set_size_request( entrylast,50, 25);
    gtk_box_pack_start(GTK_BOX(hbox),entrylast,FALSE,FALSE,0);
	gtk_entry_set_text(GTK_ENTRY(entrylast), buffer);

	hbox=gtk_hbox_new(FALSE,50);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(LABEL_RF);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	entryreffirst= gtk_entry_new();
	gtk_widget_set_size_request( entryreffirst, 50, 25);
    gtk_box_pack_start(GTK_BOX(hbox),entryreffirst,FALSE,FALSE,0);
	sprintf(buffer, "%d", g_frameIndex);
	gtk_entry_set_text(GTK_ENTRY(entryreffirst), buffer);
	label=gtk_label_new(LABEL_RL);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	entryreflast= gtk_entry_new();
	gtk_widget_set_size_request( entryreflast,50, 25);
    gtk_box_pack_start(GTK_BOX(hbox),entryreflast,FALSE,FALSE,0);
	gtk_entry_set_text(GTK_ENTRY(entryreflast), buffer);

	hbox=gtk_hbox_new(FALSE,50);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(LABEL_IM);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	entryinfemode= gtk_entry_new();
	gtk_widget_set_size_request( entryinfemode, 50, 25);
    gtk_box_pack_start(GTK_BOX(hbox),entryinfemode,FALSE,FALSE,0);
	sprintf(buffer, "%d", 0);
	gtk_entry_set_text(GTK_ENTRY(entryinfemode), buffer);

	SAutoDepthInfe *wdv=(SAutoDepthInfe*)malloc(sizeof(SAutoDepthInfe));
    wdv->window=window;
	wdv->entryfirst=entryfirst;
	wdv->entrylast=entrylast;
	wdv->entryreffirst=entryreffirst;
	wdv->entryreflast=entryreflast;
	wdv->entryinfemode=entryinfemode;

	hbox=gtk_hbox_new(FALSE,50);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
    b_ok=gtk_button_new_with_label(CONF_OK);
    g_signal_connect(G_OBJECT(b_ok),"clicked",G_CALLBACK(get_autodepinfe_frame),wdv);
    gtk_box_pack_start(GTK_BOX(hbox),b_ok,FALSE,FALSE,50);
    b_cancel=gtk_button_new_with_label(CONF_CANCEL);
    g_signal_connect(G_OBJECT(b_cancel),"clicked",G_CALLBACK(cancel_autodepinfe),window);
    gtk_box_pack_start(GTK_BOX(hbox),b_cancel,FALSE,FALSE,0);

    gtk_widget_show_all(window);

}


void setdemode0()
{
	g_depeditmode = DEPEDITMODEINCFLAT;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgDepEditMode0), TRUE);
	return ;
}

void setdemode1()
{
	g_depeditmode = DEPEDITMODEINCGRAD;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgDepEditMode1), TRUE);
	return ;
}

void setdemode2()
{
	g_depeditmode = DEPEDITMODEDECFLAT;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgDepEditMode2), TRUE);
	return ;
}

void setdemode3()
{
	g_depeditmode = DEPEDITMODEDECGRAD;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgDepEditMode3), TRUE);
	return ;
}

void setdemodule0()
{
	g_depeditmodule = DEPEDITMODULESQU;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgDepEditModule0), TRUE);
	return ;
}

void setdemodule1()
{
	g_depeditmodule = DEPEDITMODULECIR;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgDepEditModule1), TRUE);
	return ;
}


void depedit(GtkWidget * button,GtkWidget * data)
{
	char str[32];
	
	if(g_frameMaxNum>0)
	{
		g_segeditflag = 0;
		g_mateditflag = 0;
		g_matselflag = 0;
		g_depeditflag = 1;
		g_rendereditflag = 0;
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgDepEdit), TRUE);

		if(g_depeditmodule == DEPEDITMODULECIR)
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgEditCircle), TRUE);
		else
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgEditSquare), TRUE);

		sprintf_s(str,"%d", g_depeditmodulerad);
		gtk_entry_set_text(GTK_ENTRY(g_wdgEditRadius), str);
	}
	else
	{
		//messagebox("Please open a file first.");
	}
	
	return ;
}

/* File */
#define GUI_FILE 0
void OpenFile1(GtkWidget *widget,gpointer *data)
{
	char filename[128];//=(char *)malloc(128*sizeof(char));
	
	/*输出选择的文件名*/
	//g_print("%s\n",gtk_file_selection_get_filename(GTK_FILE_SELECTION(g_wdgFileOpenSelection)));
	sprintf(filename,"%s",gtk_file_selection_get_filename(GTK_FILE_SELECTION(g_wdgFileOpenSelection)));
	gtk_widget_hide(GTK_WIDGET(g_wdgFileOpenSelection));
	OpenInputFile(filename);

}

void CancelFile(GtkWidget *widget,gpointer *data)
{
	gtk_widget_hide(GTK_WIDGET(g_wdgFileOpenSelection));
}

void get_seqcut_frame(GtkWidget* button,SFrameSelect *wdv)
{
	int i=0;
	char *filename=(char *)malloc(128*sizeof(char));
	int cutframefirst, cutframelast;
	int size = IMAGE_SIZE(g_frameInWidth,g_frameInHeight);
	
    char *str=(char *)malloc(4*sizeof(char));
	uint8_t * tmp = new uint8_t[size];
	
	str=(char*)gtk_entry_get_text(GTK_ENTRY(wdv->entryfirst));
	cutframefirst = str2num(str);  
	if(cutframefirst<1)  cutframefirst=1;
	if(cutframefirst>g_frameMaxNum)  cutframefirst=g_frameMaxNum;
	str=(char*)gtk_entry_get_text(GTK_ENTRY(wdv->entrylast));
	cutframelast = str2num(str);  
	if(cutframelast<1)  cutframelast=1;
	if(cutframelast>g_frameMaxNum)  cutframelast=g_frameMaxNum;

	if(cutframefirst>cutframelast)
		cutframelast = cutframefirst;

	/*输出选择的文件名*/
	//g_print("%s\n",gtk_file_selection_get_filename(GTK_FILE_SELECTION(g_wdgFileOpenSelection)));
	sprintf(filename,"%s",gtk_file_selection_get_filename(GTK_FILE_SELECTION(g_wdgFileSaveSeqCutSelection)));
	if((g_fileSaveSeqcut=fopen(filename,"wb"))==NULL)
    {
		printf("FILE    ERROR --- Save seqcut file %s open failed!\n", filename);
		if(filename) free(filename);
		delete [] tmp;
		return ;
    }
    else
    {
		printf("FILE     INFO --- Save seqcut file open successed!\n");
		i=cutframefirst-1;
		#ifdef I64
		_fseeki64(g_fileInpYUV, (int64_t)i*size, SEEK_SET);
		#else
		fseek(g_fileInpYUV, (unsigned long)i*size, SEEK_SET);
		#endif
		for(;i<cutframelast;i++)
		{
			fread(tmp,size,1,g_fileInpYUV);
			fwrite(tmp, size, 1, g_fileSaveSeqcut);
		}
		
		fclose(g_fileSaveSeqcut);
		gtk_widget_hide(wdv->window);
		if(filename) free(filename);
		delete [] tmp;
		
		return ;
	}

    

	//if(str) free(str);
}

void cancel_seqcut(GtkWidget *button,GtkWidget *window)
{
	gtk_widget_hide(window);
}

void OpenSeqCutFile(GtkWidget *widget,gpointer *data)
{
    
	GtkWidget *window, *entryfirst, *entrylast;
    GtkWidget *label,*vbox,*hbox,*b_ok,*b_cancel, *separator;
	char buffer[32];
	
	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request(window,360,150);
    gtk_window_set_title(GTK_WINDOW(window),TITLE_CSFS);
    g_signal_connect(G_OBJECT(window),"destroy",G_CALLBACK(gtk_widget_destroy),NULL);
    gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_NONE);

    
    vbox=gtk_vbox_new(FALSE,15);
    gtk_container_add(GTK_CONTAINER(window),vbox);

	separator = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox),separator,FALSE,FALSE,0);
	
	label=gtk_label_new(LABEL_SELFS);
    gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

	hbox=gtk_hbox_new(FALSE,50);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(LABEL_FROM);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	entryfirst= gtk_entry_new();
	gtk_widget_set_size_request( entryfirst, 70, 25);
    gtk_box_pack_start(GTK_BOX(hbox),entryfirst,FALSE,FALSE,0);
	sprintf(buffer, "%d", g_frameIndex+1);
	gtk_entry_set_text(GTK_ENTRY(entryfirst), buffer);
	label=gtk_label_new(LABEL_TO);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	entrylast= gtk_entry_new();
	gtk_widget_set_size_request( entrylast,70, 25);
    gtk_box_pack_start(GTK_BOX(hbox),entrylast,FALSE,FALSE,0);
	gtk_entry_set_text(GTK_ENTRY(entrylast), buffer);

	SFrameSelect *wdv=(SFrameSelect *)malloc(sizeof(SFrameSelect));
    wdv->window=window;
	wdv->entryfirst=entryfirst;
	wdv->entrylast=entrylast;

	hbox=gtk_hbox_new(FALSE,50);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
    b_ok=gtk_button_new_with_label(CONF_OK);
    g_signal_connect(G_OBJECT(b_ok),"clicked",G_CALLBACK(get_seqcut_frame),wdv);
    gtk_box_pack_start(GTK_BOX(hbox),b_ok,FALSE,FALSE,50);
    b_cancel=gtk_button_new_with_label(CONF_CANCEL);
    g_signal_connect(G_OBJECT(b_cancel),"clicked",G_CALLBACK(cancel_seqcut),window);
    gtk_box_pack_start(GTK_BOX(hbox),b_cancel,FALSE,FALSE,0);

	gtk_widget_hide(GTK_WIDGET(g_wdgFileSaveSeqCutSelection));

    gtk_widget_show_all(window);
		
		

}


void CancelSeqCutFile(GtkWidget *widget,gpointer *data)
{
	gtk_widget_hide(GTK_WIDGET(g_wdgFileSaveSeqCutSelection));
}

void OpenLoadRenFile(GtkWidget *widget,gpointer *data)
{
	int i, j;
	IplImage* stereoimg;
	
	char filename[128];

	/*输出选择的文件名*/
	sprintf(filename,"%s",gtk_file_selection_get_filename(GTK_FILE_SELECTION(g_wdgFileLoadRenSelection)));

	if(g_frameMaxNum>0)
	{
		stereoimg = cvLoadImage(filename);

		if(stereoimg->width==g_frameWidth*2&&stereoimg->height==g_frameHeight)
		{
			for(j=0;j<g_frameHeight;j++)
				for(i=0;i<g_frameWidth;i++)
				{
					g_tmpdisplay1[i*3+0+j*3*g_frameWidth] = stereoimg->imageData[i*3+2+j*stereoimg->widthStep];
					g_tmpdisplay1[i*3+1+j*3*g_frameWidth] = stereoimg->imageData[i*3+1+j*stereoimg->widthStep];
					g_tmpdisplay1[i*3+2+j*3*g_frameWidth] = stereoimg->imageData[i*3+0+j*stereoimg->widthStep];
					g_tmpdisplay2[i*3+0+j*3*g_frameWidth] = stereoimg->imageData[i*3+2+g_frameWidth*3+j*stereoimg->widthStep];
					g_tmpdisplay2[i*3+1+j*3*g_frameWidth] = stereoimg->imageData[i*3+1+g_frameWidth*3+j*stereoimg->widthStep];
					g_tmpdisplay2[i*3+2+j*3*g_frameWidth] = stereoimg->imageData[i*3+0+g_frameWidth*3+j*stereoimg->widthStep];
				}
			ShowAll();
		}
		else
		{
			messagebox(MSGBOX_LRIERR);
		}

		cvReleaseImage( &stereoimg );
	}
	else
	{
		//messagebox("Please open a file first.");
	}
	
	gtk_widget_hide(GTK_WIDGET(g_wdgFileLoadRenSelection));
	
	return ;

}

void CancelLoadRenFile(GtkWidget *widget,gpointer *data)
{
	gtk_widget_hide(GTK_WIDGET(g_wdgFileLoadRenSelection));
}

void loadrendercurframe()
{
	char filename[128];
	if(g_keyframeflag)
		sprintf(filename,"./%s/render/%d_rendersnap_%d.png",cfgparams.projectname,g_sectioncur,g_keyframenumarr[g_frameIndex]+1);
	else
		sprintf(filename,"./%s/render/%d_rendersnap_%d.png",cfgparams.projectname,g_sectioncur,g_frameIndex+1);
	
	g_wdgFileLoadRenSelection=gtk_file_selection_new("Select Rendered File");/*创建文件选择构件*/
	gtk_window_set_position(GTK_WINDOW(g_wdgFileLoadRenSelection),GTK_WIN_POS_NONE);
	gtk_file_selection_set_filename(GTK_FILE_SELECTION(g_wdgFileLoadRenSelection),filename);
	gtk_signal_connect(GTK_OBJECT((GTK_FILE_SELECTION(g_wdgFileLoadRenSelection))->ok_button),"clicked",GTK_SIGNAL_FUNC(OpenLoadRenFile),NULL);
	gtk_signal_connect(GTK_OBJECT((GTK_FILE_SELECTION(g_wdgFileLoadRenSelection))->cancel_button),"clicked",GTK_SIGNAL_FUNC(CancelLoadRenFile),NULL);
	gtk_widget_show(g_wdgFileLoadRenSelection);
	
	return ;
}



void sequencecut(GtkWidget * button,GtkWidget * data)
{
	if(g_inputfileformat==INPYUV||g_inputfileformat==INPVID)
	{
		g_wdgFileSaveSeqCutSelection=gtk_file_selection_new("Select output File");/*创建文件选择构件*/
		gtk_window_set_position(GTK_WINDOW(g_wdgFileSaveSeqCutSelection),GTK_WIN_POS_NONE);
		gtk_file_selection_set_filename(GTK_FILE_SELECTION(g_wdgFileSaveSeqCutSelection),"*.yuv");
		gtk_signal_connect(GTK_OBJECT((GTK_FILE_SELECTION(g_wdgFileSaveSeqCutSelection))->ok_button),"clicked",GTK_SIGNAL_FUNC(OpenSeqCutFile),NULL);
		gtk_signal_connect(GTK_OBJECT((GTK_FILE_SELECTION(g_wdgFileSaveSeqCutSelection))->cancel_button),"clicked",GTK_SIGNAL_FUNC(CancelSeqCutFile),NULL);
		gtk_widget_show(g_wdgFileSaveSeqCutSelection);
	}
}

extern FILE * g_fileproject;
extern void WriteProjectFile(InputParameters * params);

void get_sel_section(GtkWidget* button,SFrameSelect *wdv)
{
	int cursection;
	char name[256];
	
    char *str=(char *)malloc(4*sizeof(char));

	str=(char*)gtk_entry_get_text(GTK_ENTRY(wdv->entryfirst));
	cursection = str2num(str);  
	if(cursection<1)  cursection=1;
	if(cursection>g_sectionnum)  cursection=g_sectionnum;

	// 注意顺序，因为有些信息需要保存，关闭时应保证g_sectioncur还未改变
	closeprojectfile(NULL, NULL);
	
	g_sectioncur = cursection;

	g_isprojectopen = 1;

	sprintf_s(name,"./%s/%s.dpj",cfgparams.projectname,cfgparams.projectname);
	if((g_fileproject = fopen(name,"w"))==NULL)
	{
		printf("FILE    ERROR --- Saving project file %s failed!\n", name);
		return ;
	}
	else
	{
		// 仅改变g_sectioncur
		cfgparams.sectioncur = g_sectioncur;

		WriteProjectFile(&cfgparams);
		
		printf("FILE     INFO --- Saving project file successed!\n");

		fclose(g_fileproject);
	}
	
	OpenProject(NULL, NULL);

    gtk_widget_hide(wdv->window);

}

void swchkeyframemode(GtkWidget * button,GtkWidget * data)
{
	char name[256];
	
	if(g_inputfileformat==INPVID)
	{
		closeprojectfile(NULL, NULL);
		
		g_isprojectopen = 1;

		sprintf_s(name,"./%s/%s.dpj",cfgparams.projectname,cfgparams.projectname);
		if((g_fileproject = fopen(name,"w"))==NULL)
		{
			printf("FILE    ERROR --- Saving project file %s failed!\n", name);
			return ;
		}
		else
		{
			if(g_keyframeflag==1)
				g_keyframeflag = 0;
			else
				g_keyframeflag = 1;
			// 仅改变keyframeflag
			cfgparams.keyframeflag = g_keyframeflag;

			WriteProjectFile(&cfgparams);
			
			printf("FILE     INFO --- Saving project file successed!\n");

			fclose(g_fileproject);
		}
		
		OpenProject(NULL, NULL);
	}
	else
		g_keyframeflag = 0;
}


void selsection(GtkWidget * button,GtkWidget * data)
{
	char buffer[128];

	GtkWidget *window, *entryfirst;//, *entrylast;
    GtkWidget *label,*vbox,*hbox,*b_ok,*b_cancel, *separator;
	
	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request(window,360,120);
    gtk_window_set_title(GTK_WINDOW(window),TITLE_VSS);
    g_signal_connect(G_OBJECT(window),"destroy",G_CALLBACK(gtk_widget_destroy),NULL);
    gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_NONE);

    
    vbox=gtk_vbox_new(FALSE,15);
    gtk_container_add(GTK_CONTAINER(window),vbox);

	separator = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox),separator,FALSE,FALSE,0);
	
	label=gtk_label_new(LABEL_SELS);
    gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

	hbox=gtk_hbox_new(FALSE,50);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	entryfirst= gtk_entry_new();
	gtk_widget_set_size_request( entryfirst, 70, 25);
    gtk_box_pack_start(GTK_BOX(hbox),entryfirst,FALSE,FALSE,0);
	sprintf(buffer, "%d", g_sectioncur);
	gtk_entry_set_text(GTK_ENTRY(entryfirst), buffer);
	sprintf(buffer, "of %d", g_sectionnum);
	label=gtk_label_new(buffer);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);

	SFrameSelect *wdv=(SFrameSelect *)malloc(sizeof(SFrameSelect));
    wdv->window=window;
	wdv->entryfirst=entryfirst;
	wdv->entrylast=entryfirst;

	hbox=gtk_hbox_new(FALSE,50);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
    b_ok=gtk_button_new_with_label(CONF_OK);
    g_signal_connect(G_OBJECT(b_ok),"clicked",G_CALLBACK(get_sel_section),wdv);
    gtk_box_pack_start(GTK_BOX(hbox),b_ok,FALSE,FALSE,50);
    b_cancel=gtk_button_new_with_label(CONF_CANCEL);
    g_signal_connect(G_OBJECT(b_cancel),"clicked",G_CALLBACK(cancel_window),window);
    gtk_box_pack_start(GTK_BOX(hbox),b_cancel,FALSE,FALSE,0);

    gtk_widget_show_all(window);

}



void helpfile(GtkWidget * button,GtkWidget * data)
{
#ifdef WIN32
	system("explorer.exe 使用说明.chm");
#else
	system("firefox ./webhelp/index.htm &");
#endif
	return ;
}

/* Frame */
#define GUI_FRAME 0

void jumpframe(GtkWidget * button, SFrameJump * data)
{
	int i,j;
	int size = g_frameWidth*g_frameHeight;
	int sizeorg = g_frameInWidth*g_frameInHeight;
	char indexstr[10];
	char * pindexstr = indexstr;
	char str[32];
	char filename[256];
	SKeyFrameList * key;
	int keyprev, keynext;
	GdkColor color_red;
	GdkColor color_green;
	GdkColor color_blue;
	GdkColor color_purple;
	int prevframenum;

	color_red.red = 0xffff; color_red.green = 0x0; color_red.blue = 0x0;
	color_green.red = 0x0; color_green.green = 0xffff; color_green.blue = 0x0;
	color_blue.red = 0x0; color_blue.green = 0x0; color_blue.blue = 0xffff;
	color_purple.red = 0xffff; color_purple.green = 0x0; color_purple.blue = 0xffff;

	pindexstr =(char *)gtk_entry_get_text(GTK_ENTRY(g_wdgInpFrameNum));
	/* 添加判断TODO */
	data->framenum = str2num(pindexstr);

	g_playflag = YUVPLAYPAUSE;

	if(g_frameMaxNum!=0) // 表示已经读取文件了或读入非图像文件
	{
		prevframenum = g_frameIndex;
		switch(data->jumpmode)
		{
			case JMPMODEFIRST:
				g_frameIndex = 0;
				break;
			case JMPMODELAST:
				g_frameIndex = g_frameMaxNum-1;
				break;
			case JMPMODECUR:
				g_frameIndex = data->framenum-1;
				if(g_frameIndex<0)
					g_frameIndex = 0;
				if(g_frameIndex>=g_frameMaxNum)
					g_frameIndex = g_frameMaxNum-1;
				break;
			case JMPMODEPREV:
				g_frameIndex--;
				if(g_frameIndex<0)
					g_frameIndex = 0;
				break;
			case JMPMODENEXT:
				g_frameIndex++;
				if(g_frameIndex>=g_frameMaxNum)
					g_frameIndex = g_frameMaxNum-1;
				break;
			case JMPMODEPREVK:
				key = getcurkeyframe(g_frameIndex);
				if(key->prev!=NULL)
					g_frameIndex = key->prev->index;
				else
					g_frameIndex = key->index;
				break;
			case JMPMODENEXTK:
				key = getcurkeyframe(g_frameIndex);
				if(key->next!=NULL)
					g_frameIndex = key->next->index;
				break;
			default:
				g_frameIndex = data->framenum-1;
				if(g_frameIndex<0)
					g_frameIndex = 0;
				if(g_frameIndex>=g_frameMaxNum)
					g_frameIndex = g_frameMaxNum-1;
				break;
		}

		if(data->jumpmode!=JMPMODECUR)
		{
			g_enlargeflag = 0;
			//g_renderflag = 0;
			g_renderdisplayflag = 0;
			g_segeditflag = 0;
			g_mateditflag = 0;
			g_matselflag = 0;
			g_depeditflag = 0;
			g_rendereditflag = 0;
			g_enlargeswitchflag = 0;
			
			//gtk_toggle_button_set_state((GtkToggleButton *)g_wdgSegmentMode, true);
			//gtk_toggle_button_set_state((GtkToggleButton *)g_wdgRenderMode, false);
			
			gtk_toggle_button_set_state((GtkToggleButton *)g_wdgNonEdit, true);
			gtk_toggle_button_set_state((GtkToggleButton *)g_wdgSegEdit, false);
			gtk_toggle_button_set_state((GtkToggleButton *)g_wdgMatEdit, false);
			gtk_toggle_button_set_state((GtkToggleButton *)g_wdgDepEdit, false);
			gtk_toggle_button_set_state((GtkToggleButton *)g_wdgRenEdit, false);

			gtk_toggle_button_set_state((GtkToggleButton *)layerbutton.layeredit[0], true);
			g_frames[g_frameIndex].LayerEditNum = 0;
		}

		// 跳到其他帧时，原帧信息保存
		if(prevframenum!=g_frameIndex)
		{
			if(g_frames[prevframenum].SegFlag)// &&g_frames[g_frameIndex].ChangeFlag==1) // TODO 可优化，判断是否修改
			{
				if(g_keyframeflag)
					sprintf(filename,"./%s/info/%d_infosnap_%d.dat",cfgparams.projectname,g_sectioncur,g_keyframenumarr[prevframenum]+1);
				else
					sprintf(filename,"./%s/info/%d_infosnap_%d.dat",cfgparams.projectname,g_sectioncur,prevframenum+1);
				if((g_frames[prevframenum].finfo=fopen(filename,"wb"))!=NULL)
				{
					saveinfofile(g_frames[prevframenum].finfo, prevframenum);
					fclose(g_frames[prevframenum].finfo);
				}
			}
			//printf("prevframenum %d, currentframenum %d\n", prevframenum, g_frameIndex);
		}

		//printf("g_frameIndex :%d\n", g_frameIndex);
		g_frameRenderRef = g_frameIndex;
		
		savesegmenttmp();

		if(g_keyframeflag)
			sprintf_s(str,"%d:%d/%d", g_frameIndex+1, g_keyframenumarr[g_frameIndex]+1, g_frameMaxNum);
		else
			sprintf_s(str,"%4d /%4d", g_frameIndex+1, g_frameMaxNum);
		gtk_label_set_text((GtkLabel *)g_wdgFrameNum,str);

		if(g_keyframeflag)
		{
			int keyidxprev, keyidxnext;
			keyidxprev = H3D_MAX(0,g_frameIndex-1);
			keyidxnext = H3D_MIN(g_frameIndex+1,g_frameMaxNum-1);
			sprintf_s(str," %4d - %4d - %4d ", g_keyframenumarr[keyidxprev]+1, g_keyframenumarr[g_frameIndex]+1,g_keyframenumarr[keyidxnext]+1);
		}
		else
		{
			key = getcurkeyframe(g_frameIndex);
			if(key->prev!=NULL)
				keyprev = key->prev->index;
			else
				keyprev = key->index;
			if(key->next!=NULL)
				keynext = key->next->index;
			else
				keynext = key->index;
			sprintf_s(str," %4d - %4d - %4d ", keyprev+1, key->index+1,keynext+1);
		}
		gtk_label_set_text((GtkLabel *)g_wdgKeyFrameNum,str);
		gtk_label_set_justify((GtkLabel *)g_wdgKeyFrameNum, GTK_JUSTIFY_CENTER);
		
		sprintf_s(str,"%d", g_frameIndex+1);
		gtk_entry_set_text(GTK_ENTRY(g_wdgInpFrameNum), str);

		if(g_frameIndex < g_frameMaxNum && g_frameIndex >= 0)
		{
			// 读入图像
			if(g_inputfileformat==INPYUV||g_inputfileformat==INPVID)
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
					//yuvresize(g_yuvorginputref, g_yuvinputref, g_frameInWidth, g_frameInHeight, g_frameWidth, g_frameHeight);
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

			// render显示所用，默认为原图像
			for(j=0;j<g_frameHeight;j++)
				for(i=0;i<g_frameWidth;i++)
				{
					g_tmpdisplay1[i*3+0+j*3*g_frameWidth] = 
					g_tmpdisplay2[i*3+0+j*3*g_frameWidth] = 
					g_tmpdisplay3[i*3+0+j*3*g_frameWidth] = 
					g_rgbinputref[i*3+0+j*3*g_frameWidth] = g_frames[g_frameIndex].rgbinput[i*3+0+j*3*g_frameWidth];
					g_tmpdisplay1[i*3+1+j*3*g_frameWidth] = 
					g_tmpdisplay2[i*3+1+j*3*g_frameWidth] = 
					g_tmpdisplay3[i*3+1+j*3*g_frameWidth] = 
					g_rgbinputref[i*3+1+j*3*g_frameWidth] = g_frames[g_frameIndex].rgbinput[i*3+1+j*3*g_frameWidth];
					g_tmpdisplay1[i*3+2+j*3*g_frameWidth] = 
					g_tmpdisplay2[i*3+2+j*3*g_frameWidth] = 
					g_tmpdisplay3[i*3+2+j*3*g_frameWidth] = 
					g_rgbinputref[i*3+2+j*3*g_frameWidth] = g_frames[g_frameIndex].rgbinput[i*3+2+j*3*g_frameWidth];
				}

			if(g_keyframeflag)
				sprintf(filename,"./%s/info/%d_infosnap_%d.dat",cfgparams.projectname,g_sectioncur,g_keyframenumarr[g_frameIndex]+1);
			else
				sprintf(filename,"./%s/info/%d_infosnap_%d.dat",cfgparams.projectname,g_sectioncur,g_frameIndex+1);
			// 帧信息载入
			if((g_frames[g_frameIndex].finfo=fopen(filename,"rb"))!=NULL)
			{
				// load info file
				loadinfofile(g_frames[g_frameIndex].finfo, g_frameIndex, 0);
				fclose(g_frames[g_frameIndex].finfo);
				//g_frames[g_frameIndex].ChangeFlag = 0;
			}
			
			if(data->jumpmode!=JMPMODECUR)
			{
				gtk_toggle_button_set_state((GtkToggleButton *)layerbutton.layer[0], false);
				all_layer(layerbutton.layer[0], NULL);
				gtk_toggle_button_set_state((GtkToggleButton *)layerbutton.layer[0], true);
			}
			// 显示层次信息
			for(i=1;i<=g_frames[g_frameIndex].LayerVisibleNum;i++)
			{
				if(g_frames[g_frameIndex].SegLabelMask&(1<<i))
					gtk_toggle_button_set_state((GtkToggleButton *)layerbutton.layer[i], true);
				else
					gtk_toggle_button_set_state((GtkToggleButton *)layerbutton.layer[i], false);
			}
			for(i=g_frames[g_frameIndex].LayerVisibleNum+1;i<LAYERBUTTONNUMMAX;i++)
			{
				gtk_toggle_button_set_state((GtkToggleButton *)layerbutton.layer[i], false);
			}

			if(g_renderflag==0)
			{

				if(g_frames[g_frameIndex].SegFlag)
				{
					for(i=0;i<size*3;i++)
						g_autosegdisplay[i] = g_frames[g_frameIndex].rgbinput[i];
					#if 1
					/*SLIC分割图像*/
					if(g_showautosegflag)
					{
						CSuperpixelSeg superpixelseg;
						superpixelseg.DrawContoursAroundSegments(g_autosegdisplay, g_frames[g_frameIndex].OrgLabels, g_frameWidth, g_frameHeight, g_autosegcolor);
					}
					#endif
					CalcRegionAvgRgb(g_frames[g_frameIndex].OrgLabels, g_rgbinput, g_frameWidth, g_frameHeight, g_segparamnumregion+10); 
					
					/*结果显示*/
					gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
			              IMGWIDTHMAX-g_frameWidth, IMGHEIGHTMAX-g_frameHeight, g_frameWidth, g_frameHeight,
			              GDK_RGB_DITHER_MAX, g_frames[g_frameIndex].rgbinput, g_frameWidth * 3);
					gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
		              IMGWIDTHMAX, IMGHEIGHTMAX-g_frameHeight, g_frameWidth, g_frameHeight,
		              GDK_RGB_DITHER_MAX, g_autosegdisplay, g_frameWidth * 3);
					
					/* 手工分割区域和深度图区域显示*/
					ShowSegandDep(g_frames[g_frameIndex].SegLabelMask);

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
				//printf("enter render mode index %d\n", g_frameIndex);
				setrendermode(NULL, NULL);
			}

			/* 根据分割情况和是否为关键帧改变颜色*/
			if(g_frames[g_frameIndex].SegFlag==1)
			{
				if(g_frames[g_frameIndex].KeyFlag==1||g_keyframeflag)
					gtk_widget_modify_fg(GTK_WIDGET(g_wdgFrameNum), GTK_STATE_NORMAL, &color_red);
				else
					gtk_widget_modify_fg(GTK_WIDGET(g_wdgFrameNum), GTK_STATE_NORMAL, &color_blue);
				//printf("Frame %d Seg\n", g_frameIndex+1);
			}
			else
			{
				if(g_frames[g_frameIndex].KeyFlag==1||g_keyframeflag)
					gtk_widget_modify_fg(GTK_WIDGET(g_wdgFrameNum), GTK_STATE_NORMAL, &color_green);
				else
					gtk_widget_modify_fg(GTK_WIDGET(g_wdgFrameNum), GTK_STATE_NORMAL, &color_purple);
				//printf("Frame %d No Seg\n", g_frameIndex+1);
			}
			
		}
		else
		{
			// do nothing
		}
	}
	else
	{
		//messagebox("Please open a file first.");
	}
	return ;
}

void setprevframe(GtkWidget *button,GtkWidget *entry)
{
	char buffer[128];
	int framenum;
	g_inferencedirect = INFDIRPREV;
	framenum = g_frameIndex - 1;
	if(framenum<0)
		framenum = 0;
	sprintf_s(buffer, "%d", framenum+1);
	gtk_entry_set_text(GTK_ENTRY(entry), buffer);
}

void setnextframe(GtkWidget *button,GtkWidget *entry)
{
	char buffer[128];
	int framenum;
	g_inferencedirect = INFDIRNEXT;
	framenum = g_frameIndex + 1;
	if(framenum>g_frameMaxNum-1)
		framenum = g_frameMaxNum-1;
	sprintf_s(buffer, "%d", framenum+1);
	gtk_entry_set_text(GTK_ENTRY(entry), buffer);
}

void setprevkeyframe(GtkWidget *button,GtkWidget *entry)
{
	char buffer[128];
	int framenum;
	SKeyFrameList * key;
	if(g_keyframeflag)
	{
		g_inferencedirect = INFDIRPREV;
		framenum = g_frameIndex - 1;
		if(framenum<0)
			framenum = 0;
	}
	else
	{
		g_inferencedirect = INFDIRPREVKEY;
		key = getcurkeyframe(g_frameIndex);
		if(key->prev!=NULL&&g_frameIndex==key->index)
		{
			framenum = key->prev->index;
		}
		else
			framenum = key->index;
	}
	sprintf_s(buffer, "%d", framenum+1);
	gtk_entry_set_text(GTK_ENTRY(entry), buffer);
}

void setnextkeyframe(GtkWidget *button,GtkWidget *entry)
{
	char buffer[128];
	int framenum;
	SKeyFrameList * key;
	if(g_keyframeflag)
	{
		g_inferencedirect = INFDIRNEXT;
		framenum = g_frameIndex + 1;
		if(framenum>g_frameMaxNum-1)
			framenum = g_frameMaxNum-1;
	}
	else
	{
		g_inferencedirect = INFDIRNEXTKEY;
		key = getcurkeyframe(g_frameIndex);
		if(key->next!=NULL)
			framenum = key->next->index;
		else
			framenum = key->index;
	}
	sprintf_s(buffer, "%d", framenum+1);
	gtk_entry_set_text(GTK_ENTRY(entry), buffer);
}

void setotherframe(GtkWidget *button,GtkWidget *entry)
{
	char buffer[128];
	int framenum;

	g_inferencedirect = INFDIRUSEDEF;
	
	framenum = g_frameIndex - 1;
	if(framenum<0)
		framenum = 0;
	sprintf_s(buffer, "%d", framenum+1);
	gtk_entry_set_text(GTK_ENTRY(entry), buffer);
}


void AutoKeyFrameScan(GtkWidget * button,GtkWidget * data)
{
	int i;
	int hist0[256];
	int hist1[256];
	int size = g_frameWidth*g_frameHeight;
	char str[32];
	char tmp[256];
	char cmd[1024];
	int fileexist;
	SKeyFrameList * key;
	int keyprev, keynext;

	if(g_frameMaxNum>0&&(g_inputfileformat==INPYUV||g_inputfileformat==INPVID)&&g_keyframeflag==0)
	{
		if(g_inputfileformat==INPYUV)
		{
			#ifdef I64
			_fseeki64(g_fileInpYUV, (int64_t)0, SEEK_SET);
			#else
			fseek(g_fileInpYUV, 0, SEEK_SET);
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
				fread(g_yuvinput.y,size,1,g_fileInpYUV);
				fread(g_yuvinput.u,size>>2,1,g_fileInpYUV);
				fread(g_yuvinput.v,size>>2,1,g_fileInpYUV);
			}
			histogram(g_yuvinput.y, hist0, g_frameWidth, g_frameHeight);	
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
						sprintf(cmd, "ffmpeg.exe -y -ss %d -t %d -scenecut 1 -scenecutth %d -i %s -s %dx%d %s/%s_%d_key.yuv\n", g_timestart+(g_sectioncur-1)*g_sectionduration, g_sectionduration, g_keyframeth, cfgparams.inputfile, g_frameInWidth, g_frameInHeight, cfgparams.projectname, cfgparams.projectname,g_sectioncur);
						printf("CMD     DEBUG --- [%s:%d] %s", __FILE__, __LINE__, cmd);
					}
					else // 最后一段
					{
						g_sectionlastdura = (g_timeend-g_timestart)%g_sectionduration;
						if(g_sectionlastdura==0)
							g_sectionlastdura = g_sectionduration;
						sprintf(cmd, "ffmpeg.exe -y -ss %d -t %d -scenecut 1 -scenecutth %d -i %s -s %dx%d %s/%s_%d_key.yuv\n", g_timestart+(g_sectioncur-1)*g_sectionduration, g_sectionlastdura, g_keyframeth, cfgparams.inputfile, g_frameInWidth, g_frameInHeight, cfgparams.projectname, cfgparams.projectname,g_sectioncur);
						printf("CMD     DEBUG --- [%s:%d] %s", __FILE__, __LINE__, cmd);
					}
				}
				else
				{
					sprintf(cmd, "ffmpeg.exe -y -ss %d -t 1 -scenecut 1 -scenecutth %d -i %s -s %dx%d %s/%s_%d_key.yuv\n", g_timestart, g_keyframeth, cfgparams.inputfile, g_frameInWidth, g_frameInHeight, cfgparams.projectname, cfgparams.projectname,g_sectioncur);
					printf("CMD     DEBUG --- [%s:%d] %s", __FILE__, __LINE__, cmd);
				}
				system(cmd);
				
				sprintf(cmd, "move keyframes.info %s/keyframes_%d.info", cfgparams.projectname, g_sectioncur);
				system(cmd);
			}
		}

		g_keyframelist.index = 0;
		g_keyframelist.next = g_keyframelist.prev = NULL;
		
		if(g_inputfileformat==INPYUV)
		{
			for(i=1;i<g_frameMaxNum;i++)
			{
				
				if(g_frameResizeFlag==1)
				{
					fread(g_yuvorginput.y,(g_frameInWidth*g_frameInHeight),1,g_fileInpYUV);
					fread(g_yuvorginput.u,(g_frameInWidth*g_frameInHeight)>>2,1,g_fileInpYUV);
					fread(g_yuvorginput.v,(g_frameInWidth*g_frameInHeight)>>2,1,g_fileInpYUV);
					yuvresize(g_yuvorginput, g_yuvinput, g_frameInWidth, g_frameInHeight, g_frameWidth, g_frameHeight);
				}
				else
				{
					fread(g_yuvinput.y,size,1,g_fileInpYUV);
					fread(g_yuvinput.u,size>>2,1,g_fileInpYUV);
					fread(g_yuvinput.v,size>>2,1,g_fileInpYUV);
				}
				histogram(g_yuvinput.y, hist1, g_frameWidth, g_frameHeight);
				if(g_frames[i].KeyFlag==1) // 已经由用户自动标记为关键帧了
				{
					histogramcopy(hist0, hist1);
					insertkeyframe(i);
				}
				else if(histogramdiff(hist0, hist1)>g_keyframeth*8*10)
				{
					//printf("histogramdiff %d\n", histogramdiff(hist0, hist1));
					g_frames[i].KeyFlag=1;
					histogramcopy(hist0, hist1);
					insertkeyframe(i);
					//g_keyframenum++;
				}
				//histogramcopy(hist0, hist1);
			}
		}
		else
		{
			FILE * fkeyframeinfo = NULL;
			int keyframenum = 0;
			int filebytes = 0;
			
			sprintf(tmp, "%s/keyframes_%d.info", cfgparams.projectname, g_sectioncur);
			fkeyframeinfo = fopen(tmp, "rb");
			if(fkeyframeinfo==NULL)
			{
				printf("FILE    ERROR --- keyframe info file %s open failed!\n", tmp);
				return ;
			}
			fseek(fkeyframeinfo, 0, SEEK_END);
			filebytes = ftell(fkeyframeinfo);
			fseek(fkeyframeinfo, 0, SEEK_SET);
			keyframenum = (int)filebytes/4;
			g_keyframenumarr = new int[keyframenum];
			fread(g_keyframenumarr, 4, keyframenum, fkeyframeinfo);
			fclose(fkeyframeinfo);

			for(i=0;i<keyframenum;i++)
			{
				if(g_keyframenumarr[i]<g_frameMaxNum)
					g_frames[g_keyframenumarr[i]].KeyFlag = 1;
			}

			for(i=1;i<g_frameMaxNum;i++)
			{
				if(g_frames[i].KeyFlag==1) // 包括已经由用户自动标记为关键帧的帧了
					insertkeyframe(i);
			}
		}

		key = getcurkeyframe(g_frameIndex);
		if(key->prev!=NULL)
			keyprev = key->prev->index;
		else
			keyprev = key->index;
		if(key->next!=NULL)
			keynext = key->next->index;
		else
			keynext = key->index;
		sprintf_s(str," %4d - %4d - %4d ", keyprev+1, key->index+1,keynext+1);
		gtk_label_set_text((GtkLabel *)g_wdgKeyFrameNum,str);
		gtk_label_set_justify((GtkLabel *)g_wdgKeyFrameNum, GTK_JUSTIFY_CENTER);
	}
	else
	{
		//messagebox("Please open a file first.");
	}

	//printf("keyframenum %d\n", g_keyframenum);

	
}

void SetKeyframe()
{
	char str[32];
	SKeyFrameList * key;
	int keyprev, keynext;
	GdkColor color_red;
	GdkColor color_green;

	color_red.red = 0xffff; color_red.green = 0x0; color_red.blue = 0x0;
	color_green.red = 0x0; color_green.green = 0xffff; color_green.blue = 0x0;

	if(g_frameMaxNum>0&&g_keyframeflag==0)
	{
		if(g_frames[g_frameIndex].KeyFlag==0)
		{
			g_frames[g_frameIndex].KeyFlag=1;
			insertkeyframe(g_frameIndex);
			
			if(g_frames[g_frameIndex].SegFlag==1)
				gtk_widget_modify_fg(GTK_WIDGET(g_wdgFrameNum), GTK_STATE_NORMAL, &color_red);
			else
				gtk_widget_modify_fg(GTK_WIDGET(g_wdgFrameNum), GTK_STATE_NORMAL, &color_green);
			
			key = getcurkeyframe(g_frameIndex);
			if(key->prev!=NULL)
				keyprev = key->prev->index;
			else
				keyprev = key->index;
			if(key->next!=NULL)
				keynext = key->next->index;
			else
				keynext = key->index;
			sprintf_s(str," %4d - %4d - %4d ", keyprev+1, key->index+1,keynext+1);
			gtk_label_set_text((GtkLabel *)g_wdgKeyFrameNum,str);
			gtk_label_set_justify((GtkLabel *)g_wdgKeyFrameNum, GTK_JUSTIFY_CENTER);
		}
	}
	else
	{
		//messagebox("Please open a file first.");
	}

	
}

void SetNonKeyframe()
{
	char str[32];
	SKeyFrameList * key;
	int keyprev, keynext;
	GdkColor color_blue;
	GdkColor color_purple;

	color_blue.red = 0x0; color_blue.green = 0x0; color_blue.blue = 0xffff;
	color_purple.red = 0xffff; color_purple.green = 0x0; color_purple.blue = 0xffff;

	if(g_frameMaxNum>0&&g_keyframeflag==0)
	{
		if(g_frames[g_frameIndex].KeyFlag==1&&g_frameIndex!=0)
		{
			g_frames[g_frameIndex].KeyFlag=0;
			deletekeyframe(g_frameIndex);

			if(g_frames[g_frameIndex].SegFlag==1)
				gtk_widget_modify_fg(GTK_WIDGET(g_wdgFrameNum), GTK_STATE_NORMAL, &color_blue);
			else
				gtk_widget_modify_fg(GTK_WIDGET(g_wdgFrameNum), GTK_STATE_NORMAL, &color_purple);

			key = getcurkeyframe(g_frameIndex);
			if(key->prev!=NULL)
				keyprev = key->prev->index;
			else
				keyprev = key->index;
			if(key->next!=NULL)
				keynext = key->next->index;
			else
				keynext = key->index;
			sprintf_s(str," %4d - %4d - %4d ", keyprev+1, key->index+1,keynext+1);
			gtk_label_set_text((GtkLabel *)g_wdgKeyFrameNum,str);
			gtk_label_set_justify((GtkLabel *)g_wdgKeyFrameNum, GTK_JUSTIFY_CENTER);
		}
	}
	else
	{
		//messagebox("Please open a file first.");
	}
	
	
}

/* inference */
#define GUI_INFERENCE 0

void get_reference_frame(GtkWidget* button,SRefFrameSelect *wdv)
{
	int i;
	int infframefirst, infframelast;
	int refframenum;
	SFrameJump stmp;
	
    char str[8];
	char *str1;// = new char[8];

	str1=(char*)gtk_entry_get_text(GTK_ENTRY(wdv->entryfirst));
	infframefirst = str2num(str1);  
	if(infframefirst<1)  infframefirst=1;
	if(infframefirst>g_frameMaxNum)  infframefirst=g_frameMaxNum;
	str1=(char*)gtk_entry_get_text(GTK_ENTRY(wdv->entrylast));
	infframelast = str2num(str1);  
	if(infframelast<1)  infframelast=1;
	if(infframelast>g_frameMaxNum)  infframelast=g_frameMaxNum;

	if(infframefirst>infframelast)
		infframelast = infframefirst;
	
    str1=(char*)gtk_entry_get_text(GTK_ENTRY(wdv->entryindex));
	refframenum = str2num(str1);  
	if(refframenum<1)  refframenum=1;
	if(refframenum>g_frameMaxNum)  refframenum=g_frameMaxNum;

	stmp.framenum = g_frameIndex + 1;
	stmp.jumpmode = JMPMODECUR;

	if(g_inputfileformat==INPYUV||g_inputfileformat==INPVID)
	{
		switch(g_inferencedirect)
		{
			case INFDIRPREV:
				for(i=infframefirst;i<=infframelast;i++)
				{
					sprintf_s(str,"%d", i);
					gtk_entry_set_text(GTK_ENTRY(g_wdgInpFrameNum), str);
					g_frameIndex = i-1;
					inferenceframe(g_frameIndex-1);
					printf("\rINFER    INFO --- Segment and Depth Inference frame index %d from %d to %d ref frame previous %.2lf%% ", i, infframefirst, infframelast, ((double)(i-infframefirst+1))/(infframelast-infframefirst+1)*100);
				}
				break;
			case INFDIRNEXT:
				for(i=infframelast;i>=infframefirst;i--)
				{
					sprintf_s(str,"%d", i);
					gtk_entry_set_text(GTK_ENTRY(g_wdgInpFrameNum), str);
					g_frameIndex = i-1;
					inferenceframe(g_frameIndex+1);
					printf("\rINFER    INFO --- Segment and Depth Inference frame index %d from %d to %d ref frame next %.2lf%% ", i, infframefirst, infframelast, ((double)(i-infframefirst+1))/(infframelast-infframefirst+1)*100);
				}
				break;
			case INFDIRPREVKEY:
			case INFDIRNEXTKEY:
			case INFDIRUSEDEF:
			default:
				for(i=infframefirst;i<=infframelast;i++)
				{
					sprintf_s(str,"%d", i);
					gtk_entry_set_text(GTK_ENTRY(g_wdgInpFrameNum), str);
					g_frameIndex = i-1;
					inferenceframe(refframenum-1);
					printf("\rINFER    INFO --- Segment and Depth Inference frame index %d from %d to %d ref frame %d %.2lf%% ", i, infframefirst, infframelast, refframenum, ((double)(i-infframefirst+1))/(infframelast-infframefirst+1)*100);
				}
				break;
		}
		printf("\n");
	}
	
	jumpframe(NULL,&stmp);

    gtk_widget_hide(wdv->window);

	//if(str) free(str);
}

void cancel_inference(GtkWidget *button,GtkWidget *window)
{
	gtk_widget_hide(window);
}

void getrefframe(GtkWidget * button,GtkWidget * data)
{
	char buffer[128];

	GtkWidget *window, *entryfirst, *entrylast, *entryindex, *prevframe, *nextframe, *prevkeyframe, *nextkeyframe, *otherframe;
    GtkWidget *label,*vbox,*hbox,*b_ok,*b_cancel, *separator;
	
	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request(window,360,400);
    gtk_window_set_title(GTK_WINDOW(window),TITLE_IRFS);
    g_signal_connect(G_OBJECT(window),"destroy",G_CALLBACK(gtk_widget_destroy),NULL);
    gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_NONE);

    
    vbox=gtk_vbox_new(FALSE,15);
    gtk_container_add(GTK_CONTAINER(window),vbox);

	separator = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox),separator,FALSE,FALSE,0);
	
	label=gtk_label_new(LABEL_SELIF);
    gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

	hbox=gtk_hbox_new(FALSE,50);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(LABEL_FROM);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	entryfirst= gtk_entry_new();
	gtk_widget_set_size_request( entryfirst, 70, 25);
    gtk_box_pack_start(GTK_BOX(hbox),entryfirst,FALSE,FALSE,0);
	sprintf_s(buffer, "%d", g_frameIndex+1);
	gtk_entry_set_text(GTK_ENTRY(entryfirst), buffer);
	label=gtk_label_new(LABEL_TO);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	entrylast= gtk_entry_new();
	gtk_widget_set_size_request( entrylast,70, 25);
    gtk_box_pack_start(GTK_BOX(hbox),entrylast,FALSE,FALSE,0);
	gtk_entry_set_text(GTK_ENTRY(entrylast), buffer);

	separator = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox),separator,FALSE,FALSE,0);
	
	label=gtk_label_new(LABEL_SELRF);
    gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);
	
	prevframe = gtk_radio_button_new_with_label(NULL, LABEL_PF);
	gtk_box_pack_start(GTK_BOX(vbox),prevframe,FALSE,FALSE,0);
	nextframe = gtk_radio_button_new_with_label(NULL, LABEL_NF);
	gtk_box_pack_start(GTK_BOX(vbox),nextframe,FALSE,FALSE,0);
	prevkeyframe = gtk_radio_button_new_with_label(NULL, LABEL_PKF);
	gtk_box_pack_start(GTK_BOX(vbox),prevkeyframe,FALSE,FALSE,0);
	nextkeyframe = gtk_radio_button_new_with_label(NULL, LABEL_NKF);
	gtk_box_pack_start(GTK_BOX(vbox),nextkeyframe,FALSE,FALSE,0);
	otherframe = gtk_radio_button_new_with_label(NULL, LABEL_OF);
	gtk_box_pack_start(GTK_BOX(vbox),otherframe,FALSE,FALSE,0);
	
	gtk_radio_button_set_group(GTK_RADIO_BUTTON(nextframe),gtk_radio_button_group(GTK_RADIO_BUTTON(prevframe)));
	gtk_radio_button_set_group(GTK_RADIO_BUTTON(prevkeyframe),gtk_radio_button_group(GTK_RADIO_BUTTON(prevframe)));
	gtk_radio_button_set_group(GTK_RADIO_BUTTON(nextkeyframe),gtk_radio_button_group(GTK_RADIO_BUTTON(prevframe)));
	gtk_radio_button_set_group(GTK_RADIO_BUTTON(otherframe),gtk_radio_button_group(GTK_RADIO_BUTTON(prevframe)));

	entryindex = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(vbox),entryindex,FALSE,FALSE,0);

	sprintf_s(buffer, "%d", g_frameIndex);
	gtk_entry_set_text(GTK_ENTRY(entryindex), buffer);

	g_signal_connect( G_OBJECT(prevframe), "released", G_CALLBACK(setprevframe), entryindex );
	g_signal_connect( G_OBJECT(nextframe), "released", G_CALLBACK(setnextframe), entryindex );
	g_signal_connect( G_OBJECT(prevkeyframe), "released", G_CALLBACK(setprevkeyframe), entryindex );
	g_signal_connect( G_OBJECT(nextkeyframe), "released", G_CALLBACK(setnextkeyframe), entryindex );
	g_signal_connect( G_OBJECT(otherframe), "released", G_CALLBACK(setotherframe), entryindex );
	
	SRefFrameSelect *wdv=(SRefFrameSelect *)malloc(sizeof(SRefFrameSelect));
    wdv->window=window;
	wdv->entryfirst=entryfirst;
	wdv->entrylast=entrylast;
    wdv->entryindex=entryindex;

	hbox=gtk_hbox_new(FALSE,50);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
    b_ok=gtk_button_new_with_label(CONF_OK);
    g_signal_connect(G_OBJECT(b_ok),"clicked",G_CALLBACK(get_reference_frame),wdv);
    gtk_box_pack_start(GTK_BOX(hbox),b_ok,FALSE,FALSE,50);
    b_cancel=gtk_button_new_with_label(CONF_CANCEL);
    g_signal_connect(G_OBJECT(b_cancel),"clicked",G_CALLBACK(cancel_inference),window);
    gtk_box_pack_start(GTK_BOX(hbox),b_cancel,FALSE,FALSE,0);

    gtk_widget_show_all(window);

}

void setinfmode0()
{
	g_inferencemode = INFMODEPIX;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgInfMode0), TRUE);
	return ;
}

void setinfmode1()
{
	g_inferencemode = INFMODEREG;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgInfMode1), TRUE);
	return ;
}

/* render */
#define GUI_RENDER 0

void setremodule0()
{
	g_rendereditselmodule = RENEDITSELMODESQU;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgRenEditModule0), TRUE);
	return ;
}

void setremodule1()
{
	g_rendereditselmodule = RENEDITSELMODECIR;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgRenEditModule1), TRUE);
	return ;
}

void setremode0()
{
	g_rendereditmatchmodule = RENEDITMATCHMOD7X7;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgRenEditMatchModule0), TRUE);
	return ;
}

void setremode1()
{
	g_rendereditmatchmodule = RENEDITMATCHMOD9X9;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgRenEditMatchModule1), TRUE);
	return ;
}

void setremode2()
{
	g_rendereditmatchmodule = RENEDITMATCHMODBXB;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgRenEditMatchModule2), TRUE);
	return ;
}

void setrestartp0()
{
	g_rendereditstartpoint = RENEDITSELSPSTRU;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgRenEditStartP0), TRUE);
	return ;
}

void setrestartp1()
{
	g_rendereditstartpoint = RENEDITSELSPMENU;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgRenEditStartP1), TRUE);
	return ;
}

void get_render_frames(GtkWidget* button,SFrameSelect *wdv)
{
	int i,j, k;
	int framefirst, framelast;
	int width = g_frameInWidth;
	int sizergb = IMAGERGB_SIZE(g_frameInWidth,g_frameInHeight);
	int sizeyuv = IMAGE_SIZE(g_frameInWidth,g_frameInHeight);
	uint8_t * in_rgb = NULL;
	uint8_t * in_yuv = NULL;
	uint8_t * out_yuv;// = new uint8_t [sizeyuv];
	SYuvInput inyuv;
	int cnt = 0;
	char str[128];
	int fileexist = 0;
	char filename[FILE_NAME_SIZE];
	int num;
	SFrameJump stmp;
	
	IplImage* stereoimg;

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
	in_rgb = new uint8_t [sizergb];
	in_yuv = new uint8_t [sizeyuv];
	
    char *str1=(char *)malloc(8*sizeof(char));

	str1=(char*)gtk_entry_get_text(GTK_ENTRY(wdv->entryfirst));
	framefirst = str2num(str1);  
	if(framefirst<1)  framefirst=1;
	if(framefirst>g_frameMaxNum)  framefirst=g_frameMaxNum;
	str1=(char*)gtk_entry_get_text(GTK_ENTRY(wdv->entrylast));
	framelast = str2num(str1);  
	if(framelast<1)  framelast=1;
	if(framelast>g_frameMaxNum)  framelast=g_frameMaxNum;

	if(framefirst>framelast)
		framelast = framefirst;
	
	if((g_projectSaveRender=fopen(cfgparams.outputfile1,"wb"))==NULL)
    {
		printf("FILE    ERROR --- Save render file %s open failed!\n", cfgparams.outputfile1);
		if(in_rgb) delete [] in_rgb;
		if(in_yuv) delete [] in_yuv;
		if(out_yuv) delete [] out_yuv;

		gtk_widget_hide(wdv->window);
		return ;
    }
    else
    {
		printf("FILE     INFO --- Save render file open successed!\n");
		for(k=framefirst;k<=framelast;k++)
		{
			g_frameIndex = k-1;
			if(g_frames[g_frameIndex].SegFlag==1) 
			{
				if(g_keyframeflag)
					sprintf(filename,"./%s/render/%d_rendersnap_%d.png",cfgparams.projectname,g_sectioncur,g_keyframenumarr[g_frameIndex]+1);
				else
					sprintf(filename,"./%s/render/%d_rendersnap_%d.png",cfgparams.projectname,g_sectioncur,g_frameIndex+1);
				fileexist = (_access(filename,0)==0);
				if(fileexist)
				{
					uint8_t * out_rgb = new uint8_t[width*g_frameInHeight*3];
					stereoimg = cvLoadImage(filename);
					
					for(j=0;j<g_frameInHeight;j++)
						for(i=0;i<width;i++)
						{
							out_rgb[i*3+0+j*3*width] = stereoimg->imageData[i*3+2+j*stereoimg->widthStep];
							out_rgb[i*3+1+j*3*width] = stereoimg->imageData[i*3+1+j*stereoimg->widthStep];
							out_rgb[i*3+2+j*3*width] = stereoimg->imageData[i*3+0+j*stereoimg->widthStep];
						}
					rgb2yuv(out_rgb, out_yuv,(out_yuv+width*g_frameInHeight*5/4),(out_yuv+width*g_frameInHeight),width,g_frameInHeight);
					
					delete []out_rgb;
					cvReleaseImage( &stereoimg );	
				}
				else
				{
					if(g_inputfileformat==INPYUV||g_inputfileformat==INPVID)
					{
						inyuv.y = in_yuv;
						inyuv.u = in_yuv + g_frameInWidth*g_frameInHeight;
						inyuv.v = in_yuv + g_frameInWidth*g_frameInHeight*5/4;
						#ifdef I64
						_fseeki64(g_fileInpYUV, (int64_t)g_frameIndex*sizeyuv, SEEK_SET);
						#else
						fseek(g_fileInpYUV, (unsigned long)g_frameIndex*sizeyuv, SEEK_SET);
						#endif
						fread(in_yuv,sizeyuv,1,g_fileInpYUV);
						yuv2rgb(inyuv.y, inyuv.v, inyuv.u, in_rgb, g_frameInWidth, g_frameInHeight);
						DepthRendering(in_rgb, out_yuv, g_frameInWidth, g_frameInHeight, g_renderoutputformat);
						printf("\rRENDER   INFO --- Depth Rendering frame index %d from %d to %d %.2lf%% ", k, framefirst, framelast,((double)(k-framefirst+1))/(framelast-framefirst+1)*100);
					}
					else if(g_inputfileformat==INPIMG)
					{
						DepthRendering(g_rgborginput, out_yuv, g_frameInWidth, g_frameInHeight, g_renderoutputformat);
					}
					else
					{
						// do nothing
					}
				}

				fwrite(out_yuv, 1, IMAGE_SIZE(width,g_frameInHeight),g_projectSaveRender);
				cnt ++;
				num = g_frameIndex;
			}
			//saverendercurframe();
		}
		printf("\n");
	
		fclose(g_projectSaveRender);
		
	}
	
	sprintf(str,"%d", num+1);
	gtk_entry_set_text(GTK_ENTRY(g_wdgInpFrameNum), str);
	stmp.jumpmode = JMPMODECUR;
	jumpframe(NULL,&stmp);

	if(in_rgb) delete [] in_rgb;
	if(in_yuv) delete [] in_yuv;
	if(out_yuv) delete [] out_yuv;

	gtk_widget_hide(wdv->window);

	#ifdef USE_CHINESE
	sprintf(str,"%d个渲染帧已保存",cnt);
	#else
	sprintf(str,"%d rendered frames saved",cnt);
	#endif
	messagebox(INFO_STRINGS);
	
	return ;

	//if(str) free(str);
}

void render_frames_ffmpeg(GtkWidget* button,SFrameSelect *wdv)
{
	char cmd[1024];
	
	sprintf(cmd, "ffmpeg.exe -y %s", (char*)gtk_entry_get_text(GTK_ENTRY(wdv->entryfirst)));
	printf("CMD     DEBUG --- [%s:%d] %s", __FILE__, __LINE__, cmd);
    system(cmd);

	gtk_widget_hide(wdv->window);
	
	return ;

	//if(str) free(str);
}


void render_frames_h264(GtkWidget* button,SFrameSelect *wdv)
{
	char cmd[1024];
	
	if(_access(cfgparams.outputfile1,0))
    {
		messagebox(MSGBOX_ORYUVFE);
		gtk_widget_hide(wdv->window);
		return ;
    }
    else
    {
		sprintf(cmd, "x264.exe %s", (char*)gtk_entry_get_text(GTK_ENTRY(wdv->entryfirst)));
		printf("CMD     DEBUG --- [%s:%d] %s", __FILE__, __LINE__, cmd);
        system(cmd);
		//messagebox("Can't open the rendered yuv file");
	}

	gtk_widget_hide(wdv->window);
	
	return ;

	//if(str) free(str);
}

void cancel_renderframes(GtkWidget *button,GtkWidget *window)
{
	gtk_widget_hide(window);
}


void getrenderframes(GtkWidget * button,GtkWidget * data)
{
	char buffer[128];

	GtkWidget *window, *entryfirst, *entrylast;
    GtkWidget *label,*vbox,*hbox,*b_ok,*b_cancel, *separator;
	
	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request(window,360,150);
    gtk_window_set_title(GTK_WINDOW(window),TITLE_RFS);
    g_signal_connect(G_OBJECT(window),"destroy",G_CALLBACK(gtk_widget_destroy),NULL);
    gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_NONE);

    
    vbox=gtk_vbox_new(FALSE,15);
    gtk_container_add(GTK_CONTAINER(window),vbox);

	separator = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox),separator,FALSE,FALSE,0);
	
	label=gtk_label_new(LABEL_SELFS);
    gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

	hbox=gtk_hbox_new(FALSE,50);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(LABEL_FROM);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	entryfirst= gtk_entry_new();
	gtk_widget_set_size_request( entryfirst, 70, 25);
    gtk_box_pack_start(GTK_BOX(hbox),entryfirst,FALSE,FALSE,0);
	sprintf(buffer, "%d", g_frameIndex+1);
	gtk_entry_set_text(GTK_ENTRY(entryfirst), buffer);
	label=gtk_label_new(LABEL_TO);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	entrylast= gtk_entry_new();
	gtk_widget_set_size_request( entrylast,70, 25);
    gtk_box_pack_start(GTK_BOX(hbox),entrylast,FALSE,FALSE,0);
	gtk_entry_set_text(GTK_ENTRY(entrylast), buffer);

	SFrameSelect *wdv=(SFrameSelect *)malloc(sizeof(SFrameSelect));
    wdv->window=window;
	wdv->entryfirst=entryfirst;
	wdv->entrylast=entrylast;

	hbox=gtk_hbox_new(FALSE,50);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
    b_ok=gtk_button_new_with_label(CONF_OK);
    g_signal_connect(G_OBJECT(b_ok),"clicked",G_CALLBACK(get_render_frames),wdv);
    gtk_box_pack_start(GTK_BOX(hbox),b_ok,FALSE,FALSE,50);
    b_cancel=gtk_button_new_with_label(CONF_CANCEL);
    g_signal_connect(G_OBJECT(b_cancel),"clicked",G_CALLBACK(cancel_renderframes),window);
    gtk_box_pack_start(GTK_BOX(hbox),b_cancel,FALSE,FALSE,0);

    gtk_widget_show_all(window);

}
extern int g_sectionlastdura;

void autotranscode(GtkWidget * button,GtkWidget * data)
{
	char buffer[1024];

	GtkWidget *window, *entryparam;
    GtkWidget *label,*vbox,*hbox,*b_ok,*b_cancel;
	
	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request(window,720,150);
    gtk_window_set_title(GTK_WINDOW(window),TITLE_ADT);
    g_signal_connect(G_OBJECT(window),"destroy",G_CALLBACK(gtk_widget_destroy),NULL);
    gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_NONE);

    vbox=gtk_vbox_new(FALSE,15);
    gtk_container_add(GTK_CONTAINER(window),vbox);
	
	label=gtk_label_new(LABEL_PWPFFMPEG);
    gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

	hbox=gtk_hbox_new(FALSE,50);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	entryparam= gtk_entry_new();
	gtk_widget_set_size_request( entryparam, 600, 25);
    gtk_box_pack_start(GTK_BOX(hbox),entryparam,FALSE,FALSE,0);
	if(g_sectionflag==1)
	{
		if(g_sectioncur<g_sectionnum)
			sprintf(buffer, "-ss %d -t %d -i %s -dddsection %d -dddipp ./%s -acodec mp2 -ac 2 -ab 192k -vol 200 -vcodec libx264 -s %dx%d -b:v 6000000 -dd2ddd 1 -framerate 25.0 -r 25.0 -force_fps %s_%d.ts", g_timestart+(g_sectioncur-1)*g_sectionduration, g_sectionduration, cfgparams.inputfile, g_sectioncur, cfgparams.projectname, g_frameInWidth, g_frameInHeight, cfgparams.outputfile2, g_sectioncur);
		else // 最后一段
		{
			g_sectionlastdura = (g_timeend-g_timestart)%g_sectionduration;
			if(g_sectionlastdura==0)
				g_sectionlastdura = g_sectionduration;
			sprintf(buffer, "-ss %d -t %d -i %s -dddsection %d -dddipp ./%s -acodec mp2 -ac 2 -ab 192k -vol 200 -vcodec libx264 -s %dx%d -b:v 6000000 -dd2ddd 1 -framerate 25.0 -r 25.0 -force_fps %s_%d.ts", g_timestart+(g_sectioncur-1)*g_sectionduration, g_sectionlastdura, cfgparams.inputfile, g_sectioncur, cfgparams.projectname, g_frameInWidth, g_frameInHeight, cfgparams.outputfile2, g_sectioncur);
		}
	}
	else
		sprintf(buffer, "-ss %d -t %d -i %s -dddsection 1 -dddipp ./%s -acodec mp2 -ac 2 -ab 192k -vol 200 -vcodec libx264 -s %dx%d -b:v 6000000 -dd2ddd 1 -framerate 25.0 -r 25.0 -force_fps %s.ts", g_timestart,(g_timeend-g_timestart), cfgparams.inputfile, cfgparams.projectname, g_frameInWidth, g_frameInHeight, cfgparams.outputfile2);
	gtk_entry_set_text(GTK_ENTRY(entryparam), buffer);

	SFrameSelect *wdv=(SFrameSelect *)malloc(sizeof(SFrameSelect));
    wdv->window=window;
	wdv->entryfirst=entryparam;

	hbox=gtk_hbox_new(FALSE,50);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
    b_ok=gtk_button_new_with_label(CONF_OK);
    g_signal_connect(G_OBJECT(b_ok),"clicked",G_CALLBACK(render_frames_ffmpeg),wdv);
    gtk_box_pack_start(GTK_BOX(hbox),b_ok,FALSE,FALSE,50);
    b_cancel=gtk_button_new_with_label(CONF_CANCEL);
    g_signal_connect(G_OBJECT(b_cancel),"clicked",G_CALLBACK(cancel_renderframes),window);
    gtk_box_pack_start(GTK_BOX(hbox),b_cancel,FALSE,FALSE,0);

    gtk_widget_show_all(window);

}

void renderframes2h264(GtkWidget * button,GtkWidget * data)
{
	char buffer[1024];

	GtkWidget *window, *entryparam;
    GtkWidget *label,*vbox,*hbox,*b_ok,*b_cancel;
	
	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request(window,720,150);
    gtk_window_set_title(GTK_WINDOW(window),TITLE_RFHF);
    g_signal_connect(G_OBJECT(window),"destroy",G_CALLBACK(gtk_widget_destroy),NULL);
    gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_NONE);

    vbox=gtk_vbox_new(FALSE,15);
    gtk_container_add(GTK_CONTAINER(window),vbox);
	
	label=gtk_label_new(LABEL_PWPX264);
    gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

	hbox=gtk_hbox_new(FALSE,50);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	entryparam= gtk_entry_new();
	gtk_widget_set_size_request( entryparam, 600, 25);
    gtk_box_pack_start(GTK_BOX(hbox),entryparam,FALSE,FALSE,0);
	sprintf(buffer, "-q 16 --fps 30 --input-res %dx%d -o %s %s", g_frameInWidth*2, g_frameInHeight, cfgparams.outputfile2, cfgparams.outputfile1);
	gtk_entry_set_text(GTK_ENTRY(entryparam), buffer);

	SFrameSelect *wdv=(SFrameSelect *)malloc(sizeof(SFrameSelect));
    wdv->window=window;
	wdv->entryfirst=entryparam;

	hbox=gtk_hbox_new(FALSE,50);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
    b_ok=gtk_button_new_with_label(CONF_OK);
    g_signal_connect(G_OBJECT(b_ok),"clicked",G_CALLBACK(render_frames_h264),wdv);
    gtk_box_pack_start(GTK_BOX(hbox),b_ok,FALSE,FALSE,50);
    b_cancel=gtk_button_new_with_label(CONF_CANCEL);
    g_signal_connect(G_OBJECT(b_cancel),"clicked",G_CALLBACK(cancel_renderframes),window);
    gtk_box_pack_start(GTK_BOX(hbox),b_cancel,FALSE,FALSE,0);

    gtk_widget_show_all(window);

}

/* Segment */
#define GUI_SEGMENT 0

void segedit(GtkWidget * button,GtkWidget * data)
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
			g_rendereditflag = 0;
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgNonEdit), TRUE);
			return ;
		}
		else
		{
			g_segeditflag = 1;
			g_mateditflag = 0;
			g_matselflag = 0;
			g_depeditflag = 0;
			g_rendereditflag = 0;
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgSegEdit), TRUE);

			if(g_segeditmodule == SEGEDITMODULECIR)
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgEditCircle), TRUE);
			else
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgEditSquare), TRUE);

			sprintf_s(str,"%d", g_segeditmodulerad);
			gtk_entry_set_text(GTK_ENTRY(g_wdgEditRadius), str);
		}		
	}
	else
	{
		//messagebox("Please open a file first.");
	}
	
	return ;
}

void matedit(GtkWidget * button,GtkWidget * data)
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
			g_rendereditflag = 0;
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgNonEdit), TRUE);
			return ;
		}
		else
		{
			g_segeditflag = 0;
			g_mateditflag = 1;
			g_matselflag = 0;
			g_depeditflag = 0;
			g_rendereditflag = 0;
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgMatEdit), TRUE);

			if(g_segeditmodule == SEGEDITMODULECIR)
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgEditCircle), TRUE);
			else
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgEditSquare), TRUE);

			sprintf_s(str,"%d", g_segeditmodulerad);
			gtk_entry_set_text(GTK_ENTRY(g_wdgEditRadius), str);
		}		
	}
	else
	{
		//messagebox("Please open a file first.");
	}
	
	return ;
}


void setsemodule0()
{
	g_segeditmodule = SEGEDITMODULESQU;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgSegEditModule0), TRUE);
	return ;
}

void setsemodule1()
{
	g_segeditmodule = SEGEDITMODULECIR;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgSegEditModule1), TRUE);
	return ;
}

void setsecolor0()
{
	g_autosegcolor = SEGEDITCOLORBLACK;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgSegEditColor0), TRUE);
	return ;
}

void setsecolor1()
{
	g_autosegcolor = SEGEDITCOLORWHITE;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgSegEditColor1), TRUE);
	return ;
}

void setsecolor2()
{
	g_autosegcolor = SEGEDITCOLORRED;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgSegEditColor2), TRUE);
	return ;
}

void setsecolor3()
{
	g_autosegcolor = SEGEDITCOLORGREEN;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgSegEditColor3), TRUE);
	return ;
}

void setsecolor4()
{
	g_autosegcolor = SEGEDITCOLORBLUE;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgSegEditColor4), TRUE);
	return ;
}

void cancel_segment(GtkWidget *button,GtkWidget *window)
{
	gtk_widget_hide(window);
}

void get_segment_frame(GtkWidget* button,SFrameSelect *wdv)
{
	int segframefirst, segframelast;
	
	char *str=(char *)malloc(4*sizeof(char));

	str=(char*)gtk_entry_get_text(GTK_ENTRY(wdv->entryfirst));
	segframefirst = str2num(str);  
	if(segframefirst<1)  segframefirst=1;
	if(segframefirst>g_frameMaxNum)  segframefirst=g_frameMaxNum;
	str=(char*)gtk_entry_get_text(GTK_ENTRY(wdv->entrylast));
	segframelast = str2num(str);  
	if(segframelast<1)	segframelast=1;
	if(segframelast>g_frameMaxNum)	segframelast=g_frameMaxNum;

	if(segframefirst>segframelast)
		segframelast = segframefirst;

	segmentframes(segframefirst-1, segframelast-1);

	gtk_widget_hide(wdv->window);

}


void getsegmentframe(GtkWidget * button,GtkWidget * data)
{
	char buffer[128];

	GtkWidget *window, *entryfirst, *entrylast;
    GtkWidget *label,*vbox,*hbox,*b_ok,*b_cancel, *separator;
	
	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request(window,360,150);
    gtk_window_set_title(GTK_WINDOW(window),TITLE_ASFS);
    g_signal_connect(G_OBJECT(window),"destroy",G_CALLBACK(gtk_widget_destroy),NULL);
    gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_NONE);

    
    vbox=gtk_vbox_new(FALSE,15);
    gtk_container_add(GTK_CONTAINER(window),vbox);

	separator = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox),separator,FALSE,FALSE,0);
#if 0
	g_wdgProcessBar = gtk_progress_bar_new ();
	gtk_progress_bar_set_orientation( GTK_PROGRESS_BAR(g_wdgProcessBar), GTK_PROGRESS_LEFT_TO_RIGHT );
	gtk_progress_bar_set_fraction ( GTK_PROGRESS_BAR(g_wdgProcessBar), 0.0);
	gtk_box_pack_start(GTK_BOX(vbox),g_wdgProcessBar,FALSE,FALSE,0);
#endif
	label=gtk_label_new(LABEL_SELFS);
    gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);
	
	hbox=gtk_hbox_new(FALSE,50);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(LABEL_FROM);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	entryfirst= gtk_entry_new();
	gtk_widget_set_size_request( entryfirst, 70, 25);
    gtk_box_pack_start(GTK_BOX(hbox),entryfirst,FALSE,FALSE,0);
	sprintf(buffer, "%d", g_frameIndex+1);
	gtk_entry_set_text(GTK_ENTRY(entryfirst), buffer);
	label=gtk_label_new(LABEL_TO);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	entrylast= gtk_entry_new();
	gtk_widget_set_size_request( entrylast,70, 25);
    gtk_box_pack_start(GTK_BOX(hbox),entrylast,FALSE,FALSE,0);
	gtk_entry_set_text(GTK_ENTRY(entrylast), buffer);

	SFrameSelect *wdv=(SFrameSelect *)malloc(sizeof(SFrameSelect));
    wdv->window=window;
	wdv->entryfirst=entryfirst;
	wdv->entrylast=entrylast;

	

	hbox=gtk_hbox_new(FALSE,50);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
    b_ok=gtk_button_new_with_label(CONF_OK);
    g_signal_connect(G_OBJECT(b_ok),"clicked",G_CALLBACK(get_segment_frame),wdv);
    gtk_box_pack_start(GTK_BOX(hbox),b_ok,FALSE,FALSE,50);
    b_cancel=gtk_button_new_with_label(CONF_CANCEL);
    g_signal_connect(G_OBJECT(b_cancel),"clicked",G_CALLBACK(cancel_window),window);
    gtk_box_pack_start(GTK_BOX(hbox),b_cancel,FALSE,FALSE,0);

    gtk_widget_show_all(window);

}


void SegmentWarning()
{
	if(g_frameMaxNum==0)
	{
		//messagebox("Please open a file first.");
		return;
	}

	if(g_frames[g_frameIndex].SegFlag == 0|| g_frames[g_frameIndex].AutoSegLabel <= g_segparamnumregion/10)
	{
		SegmentImg(NULL, NULL);
	}
	else
	{
		GtkWidget *window;
	    GtkWidget *label,*vbox,*hbox,*b_ok,*b_cancel, *separator;
		
		window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	    gtk_widget_set_size_request(window,360,100);
	    gtk_window_set_title(GTK_WINDOW(window),TITLE_SF);
	    g_signal_connect(G_OBJECT(window),"destroy",G_CALLBACK(gtk_widget_destroy),NULL);
	    gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_NONE);
	    
	    vbox=gtk_vbox_new(FALSE,15);
	    gtk_container_add(GTK_CONTAINER(window),vbox);

		separator = gtk_hseparator_new();
		gtk_box_pack_start(GTK_BOX(vbox),separator,FALSE,FALSE,0);
		
		label=gtk_label_new(LABEL_WCCS);
	    gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

		hbox=gtk_hbox_new(FALSE,50);
	    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	    b_ok=gtk_button_new_with_label(LABEL_COVER);
	    g_signal_connect(G_OBJECT(b_ok),"clicked",G_CALLBACK(SegmentImg),window);
	    gtk_box_pack_start(GTK_BOX(hbox),b_ok,FALSE,FALSE,50);
	    b_cancel=gtk_button_new_with_label(CONF_CANCEL);
	    g_signal_connect(G_OBJECT(b_cancel),"clicked",G_CALLBACK(cancel_segment),window);
	    gtk_box_pack_start(GTK_BOX(hbox),b_cancel,FALSE,FALSE,0);

	    gtk_widget_show_all(window);
	}
}

void get_background_frame(GtkWidget* button,SFrameSelect *wdv)
{
	//int i;
	int depframefirst, depframelast;
	//SFrameJump stmp;
	
    char *str=(char *)malloc(4*sizeof(char));

	str=(char*)gtk_entry_get_text(GTK_ENTRY(wdv->entryfirst));
	depframefirst = str2num(str);  
	if(depframefirst<1)  depframefirst=1;
	if(depframefirst>g_frameMaxNum)  depframefirst=g_frameMaxNum;
	str=(char*)gtk_entry_get_text(GTK_ENTRY(wdv->entrylast));
	depframelast = str2num(str);  
	if(depframelast<1)  depframelast=1;
	if(depframelast>g_frameMaxNum)  depframelast=g_frameMaxNum;

	if(depframefirst>depframelast)
		depframelast = depframefirst;

	//printf("%d %d %d\n", infframefirst, infframelast, refframenum);
	//stmp.jumpmode = JMPMODECUR;
	//for(i=depframefirst;i<=depframelast;i++)
	//{
		//sprintf(str,"%d", i); 
		//gtk_entry_set_text(GTK_ENTRY(g_wdgInpFrameNum), str);
		//g_frameIndex = i-1;
	backgroundgeneration(depframefirst-1, depframelast-1);
	//}

	//if(g_enlargeflag==0)
		//jumpframe(NULL,&stmp);

    gtk_widget_hide(wdv->window);

	//if(str) free(str);
}


void getbackgroundframe(GtkWidget * button,GtkWidget * data)
{
	char buffer[128];

	GtkWidget *window, *entryfirst, *entrylast;
    GtkWidget *label,*vbox,*hbox,*b_ok,*b_cancel, *separator;
	
	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request(window,360,150);
    gtk_window_set_title(GTK_WINDOW(window),TITLE_BGFS);
    g_signal_connect(G_OBJECT(window),"destroy",G_CALLBACK(gtk_widget_destroy),NULL);
    gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_NONE);

    
    vbox=gtk_vbox_new(FALSE,15);
    gtk_container_add(GTK_CONTAINER(window),vbox);

	separator = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox),separator,FALSE,FALSE,0);
	
	label=gtk_label_new(LABEL_SELFS);
    gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

	hbox=gtk_hbox_new(FALSE,50);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(LABEL_FROM);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	entryfirst= gtk_entry_new();
	gtk_widget_set_size_request( entryfirst, 70, 25);
    gtk_box_pack_start(GTK_BOX(hbox),entryfirst,FALSE,FALSE,0);
	sprintf(buffer, "%d", g_frameIndex+1);
	gtk_entry_set_text(GTK_ENTRY(entryfirst), buffer);
	label=gtk_label_new(LABEL_TO);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	entrylast= gtk_entry_new();
	gtk_widget_set_size_request( entrylast,70, 25);
    gtk_box_pack_start(GTK_BOX(hbox),entrylast,FALSE,FALSE,0);
	gtk_entry_set_text(GTK_ENTRY(entrylast), buffer);

	SFrameSelect *wdv=(SFrameSelect *)malloc(sizeof(SFrameSelect));
    wdv->window=window;
	wdv->entryfirst=entryfirst;
	wdv->entrylast=entrylast;

	hbox=gtk_hbox_new(FALSE,50);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
    b_ok=gtk_button_new_with_label(CONF_OK);
    g_signal_connect(G_OBJECT(b_ok),"clicked",G_CALLBACK(get_background_frame),wdv);
    gtk_box_pack_start(GTK_BOX(hbox),b_ok,FALSE,FALSE,50);
    b_cancel=gtk_button_new_with_label(CONF_CANCEL);
    g_signal_connect(G_OBJECT(b_cancel),"clicked",G_CALLBACK(cancel_window),window);
    gtk_box_pack_start(GTK_BOX(hbox),b_cancel,FALSE,FALSE,0);

    gtk_widget_show_all(window);

}



