#include <io.h>
/* OPENCV */
#include "cv.h"   
#include "highgui.h"  

#include "CSuperpixelSeg.h"
#include "project.h"

/*
 * Depth Editer
 */

SLayerButton layerbutton;

SParameterInp g_parameterinput;

// 编辑改变标记
int g_changeflag = 0;

// 放大时深度与分割切换标记
int g_enlargeswitchflag = 0;

// 播放标志
int g_playflag = YUVPLAYPAUSE;

// 是否放大标志
int g_enlargeflag = 0;
int g_enlargeorgx = 0;
int g_enlargeorgy = 0;

// 分割编辑模式
int g_segeditflag = 0;
int g_segeditmodule = SEGEDITMODULECIR;
int g_segeditmodulerad = SEGEDITPARAM_RADDEF;

// matting编辑模式
int g_mateditflag = 0;
int g_matselflag = 0;
int g_mateditx = 0;
int g_matedity = 0;

// 深度编辑模式
int g_depeditflag = 0;
int g_depeditmode = DEPEDITMODEINCGRAD;
int g_depeditmodule = DEPEDITMODULECIR;
int g_depeditmodulerad = DEPEDITPARAM_RADDEF;

int g_depfiltgausesigmax = DEPFILTPARAM_GSDEF;
int g_depfiltgausesigmay = DEPFILTPARAM_GSDEF;
int g_depfiltdilateiter = DEPFILTPARAM_DIDEF;

// render编辑模式
int g_rendereditflag = 0;
int g_rendereditselmodule = RENEDITSELMODESQU;
int g_rendereditselmodulerad = RENEDITPARAM_SRADDEF;
int g_rendereditrefregionrad = RENEDITPARAM_RRADDEF;
int g_rendereditmatchmodule = RENEDITMATCHMOD9X9;
int g_rendereditstartpoint = RENEDITSELSPMENU;

extern GtkWidget *g_wdgProcessBar;

#ifndef _DEBUG
//#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
#pragma comment(linker,"/subsystem:windows /entry:WinMainCRTStartup") 
#endif

#define TIMEINTER 50

int main( int argc, char **argv ) {

	int i;
	char lvnum[30]; //(char *)malloc(10*sizeof(char));

	GtkWidget *window;						// 主窗口
	GtkWidget *file_item, *edit_item, *segment_item, *depth_item, *render_item, *help_item;    /*菜单项*/
    GtkWidget *menu_bar;            /*菜单*/
	GtkWidget *toolbar, *openfile, *closeproject, *seqcut, *conf, *depauto;// 工具栏
	GtkWidget *segment, *segcheck, *autoseg, *inference, *segreverse;
	#if 0
	GtkWidget *cancelref, *loadsegment,*loadsegdep, *savesegment;
	#endif
	GtkWidget *depinfe, *deplayerinfe, *depautoinfe, *depproc, *savedepth, *render, *renderframes, *renderframesh264, /* *saverender, *loadrender,  *cancelrender, */ *closefile;

	GtkWidget *frame1;	// 分区域
	GtkWidget *frame2;
	GtkWidget *frame3;
	GtkWidget *frame4;
	GtkWidget *frame5;
	GtkWidget *scrolled_window, *scrolled_window1; 
	GtkWidget *vbox, *hbox, *vbox1;
	GtkWidget *separator, *label;

	//GtkWidget *label1, *label2, *setres;
	GtkWidget * iconw, *box, *button;
	GdkColor color[LAYERBUTTONNUMMAX];
	SFrameJump jmp;

	//SetDllDirectory(".\\dll");
	if (!g_thread_supported()) 
      g_thread_init(NULL); 

     gdk_threads_init();
	
	/*new UI*/
	
	gtk_init( &argc, &argv );
	gtk_rc_parse("./gtkrc");
	colorspace_init();
	gdk_rgb_init ();
	gdk_rgb_set_install (TRUE);
	

	/*主窗口*/
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window),MAIN_WIND_NAME);
	gtk_widget_set_size_request(window, MAINWINWIDTH, MAINWINHEIGHT);
	gtk_container_set_border_width (GTK_CONTAINER (window), 0);
	gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_NONE);/*将窗口的显示位置设定为左上角*/
	//gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_NONE);
    g_signal_connect(G_OBJECT(window),"destroy",G_CALLBACK(delete_event_fun),NULL);

	/* 主界面 */
	g_wdgMainInterface = gtk_fixed_new();
	gtk_widget_set_usize(g_wdgMainInterface,MAINWINWIDTH,MAINWINHEIGHT);
	gtk_container_add( GTK_CONTAINER(window), g_wdgMainInterface );

	/* 菜单栏*/
	menu_bar = gtk_menu_bar_new ();
    gtk_fixed_put(GTK_FIXED(g_wdgMainInterface),menu_bar,MENUBARORGX,MENUBARORGY);
    file_item=create_file_item();                //调用自定义创建菜单file的函数
    gtk_menu_bar_append (GTK_MENU_BAR (menu_bar), file_item);
    edit_item=create_edit_item();
    gtk_menu_bar_append(GTK_MENU_BAR(menu_bar),edit_item);
	segment_item=create_segment_item();
    gtk_menu_bar_append(GTK_MENU_BAR(menu_bar),segment_item);
	depth_item=create_depth_item();
    gtk_menu_bar_append(GTK_MENU_BAR(menu_bar),depth_item);
	render_item=create_render_item();
    gtk_menu_bar_append(GTK_MENU_BAR(menu_bar),render_item);
	help_item=create_help_item();
    gtk_menu_bar_append(GTK_MENU_BAR(menu_bar),help_item);		

	/* 工具栏*/
	toolbar=gtk_toolbar_new();    //设置工具栏格式
    gtk_toolbar_set_orientation (GTK_TOOLBAR (toolbar), GTK_ORIENTATION_HORIZONTAL);
    gtk_toolbar_set_style (GTK_TOOLBAR (toolbar), GTK_TOOLBAR_BOTH);
    gtk_container_set_border_width (GTK_CONTAINER (toolbar), 0);

	gtk_toolbar_append_space(GTK_TOOLBAR(toolbar));
	iconw=gtk_image_new_from_file("./image/newproj.png");
	openfile=gtk_toolbar_append_item(GTK_TOOLBAR(toolbar),NULL,
			TOOLBAR_NEWPROJ,"Private",iconw,GTK_SIGNAL_FUNC(newprojectfile),NULL);
	iconw=gtk_image_new_from_file("./image/openproj.png");
	savedepth=gtk_toolbar_append_item(GTK_TOOLBAR(toolbar),NULL,
			TOOLBAR_OPENPROJ,"Private",iconw,GTK_SIGNAL_FUNC(openprojectfile),NULL);
	iconw=gtk_image_new_from_file("./image/saveproj.png");
	closefile=gtk_toolbar_append_item(GTK_TOOLBAR(toolbar),NULL,
			TOOLBAR_SAVEPROJ,"Private",iconw,GTK_SIGNAL_FUNC(saveprojectfile),NULL); 
	iconw=gtk_image_new_from_file("./image/closeproj.png");
	closeproject=gtk_toolbar_append_item(GTK_TOOLBAR(toolbar),NULL,
			TOOLBAR_CLOSEPROJ,"Private",iconw,GTK_SIGNAL_FUNC(closeprojectfile),NULL); 

	gtk_toolbar_append_space(GTK_TOOLBAR(toolbar));
	iconw=gtk_image_new_from_file("./image/seqcut.png");
	seqcut=gtk_toolbar_append_item(GTK_TOOLBAR(toolbar),NULL,
			TOOLBAR_CUTSEQ,"Private",iconw,GTK_SIGNAL_FUNC(sequencecut),NULL); 

	gtk_toolbar_append_space(GTK_TOOLBAR(toolbar));
	iconw=gtk_image_new_from_file("./image/conf.png");
	conf=gtk_toolbar_append_item(GTK_TOOLBAR(toolbar),NULL,
			TOOLBAR_OPTCONF,"Private",iconw,GTK_SIGNAL_FUNC(parametersetting),NULL); 

	gtk_toolbar_append_space(GTK_TOOLBAR(toolbar));
	iconw=gtk_image_new_from_file("./image/seg.png");
	segment=gtk_toolbar_append_item(GTK_TOOLBAR(toolbar),NULL,
            TOOLBAR_SEGIMG,"Private",iconw,GTK_SIGNAL_FUNC(SegmentWarning),NULL); 
	iconw=gtk_image_new_from_file("./image/segcheck.png");
	segcheck=gtk_toolbar_append_item(GTK_TOOLBAR(toolbar),NULL,
            TOOLBAR_CHKSEGDONE,"Private",iconw,GTK_SIGNAL_FUNC(SegmentCheck),NULL);
	iconw=gtk_image_new_from_file("./image/autoseg.png");
	autoseg=gtk_toolbar_append_item(GTK_TOOLBAR(toolbar),NULL,
            TOOLBAR_AUTOSEG,"Private",iconw,GTK_SIGNAL_FUNC(Automenuseg),NULL);
	iconw=gtk_image_new_from_file("./image/segrev.png");
	segreverse=gtk_toolbar_append_item(GTK_TOOLBAR(toolbar),NULL,
            TOOLBAR_REVSEG,"Private",iconw,GTK_SIGNAL_FUNC(loadsegmenttmp),NULL);
	#if 0
	iconw=gtk_image_new_from_file("./image/cancelref.png");
	cancelref=gtk_toolbar_append_item(GTK_TOOLBAR(toolbar),NULL,
			"Cancel the last refine","Private",iconw,GTK_SIGNAL_FUNC(CancelRefine),NULL);
	iconw=gtk_image_new_from_file("./image/saveseg.png");
	savesegment=gtk_toolbar_append_item(GTK_TOOLBAR(toolbar),NULL,
			"Save a segment file","Private",iconw,GTK_SIGNAL_FUNC(savesegmentfile),NULL);
	iconw=gtk_image_new_from_file("./image/loadseg.png");
	loadsegment=gtk_toolbar_append_item(GTK_TOOLBAR(toolbar),NULL,
			"Load a segment file","Private",iconw,GTK_SIGNAL_FUNC(loadsegmentfile),NULL);
	iconw=gtk_image_new_from_file("./image/loadsegdep.png");
	loadsegdep=gtk_toolbar_append_item(GTK_TOOLBAR(toolbar),NULL,
			"Load a segment and depth file","Private",iconw,GTK_SIGNAL_FUNC(loadsegdepfile),NULL);
	#endif

	gtk_toolbar_append_space(GTK_TOOLBAR(toolbar));
	iconw=gtk_image_new_from_file("./image/Infe.png");
	inference=gtk_toolbar_append_item(GTK_TOOLBAR(toolbar),NULL,
			TOOLBAR_INF,"Private",iconw,GTK_SIGNAL_FUNC(getrefframe),NULL);

	
	gtk_toolbar_append_space(GTK_TOOLBAR(toolbar));
	iconw=gtk_image_new_from_file("./image/depauto.png");
	depauto=gtk_toolbar_append_item(GTK_TOOLBAR(toolbar),NULL,
			TOOLBAR_AUTODEPGEN,"Private",iconw,GTK_SIGNAL_FUNC(depthautoassign),NULL);
	iconw=gtk_image_new_from_file("./image/depproc.png");
	depproc=gtk_toolbar_append_item(GTK_TOOLBAR(toolbar),NULL,
			TOOLBAR_DEPPROC,"Private",iconw,GTK_SIGNAL_FUNC(getdepprocframe),NULL);
	iconw=gtk_image_new_from_file("./image/depinfe.png");
	depinfe=gtk_toolbar_append_item(GTK_TOOLBAR(toolbar),NULL,
			TOOLBAR_DEPINF,"Private",iconw,GTK_SIGNAL_FUNC(getdepinfeframe),NULL);
	iconw=gtk_image_new_from_file("./image/deplayerinfe.png");
	deplayerinfe=gtk_toolbar_append_item(GTK_TOOLBAR(toolbar),NULL,
			TOOLBAR_LDEPINF,"Private",iconw,GTK_SIGNAL_FUNC(getlayerdepinfeframe),NULL);
	iconw=gtk_image_new_from_file("./image/cancelrender.png");
	depautoinfe=gtk_toolbar_append_item(GTK_TOOLBAR(toolbar),NULL,
			TOOLBAR_ADEPINF,"Private",iconw,GTK_SIGNAL_FUNC(getautodepinfeframe),NULL);
	
	gtk_toolbar_append_space(GTK_TOOLBAR(toolbar));
	
	iconw=gtk_image_new_from_file("./image/render.png");
	render=gtk_toolbar_append_item(GTK_TOOLBAR(toolbar),NULL,
			TOOLBAR_RENDFRM,"Private",iconw,GTK_SIGNAL_FUNC(rendercurframe),NULL);
	//iconw=gtk_image_new_from_file("./image/cancelrender.png");
	//cancelrender=gtk_toolbar_append_item(GTK_TOOLBAR(toolbar),NULL,
			//TOOLBAR_CANLASTREND,"Private",iconw,GTK_SIGNAL_FUNC(cancelrenderfineedit),NULL);
	//iconw=gtk_image_new_from_file("./image/saverender.png");
	//saverender=gtk_toolbar_append_item(GTK_TOOLBAR(toolbar),NULL,
			//"Save Render frame","Private",iconw,GTK_SIGNAL_FUNC(saverendercurframe),NULL);
	//iconw=gtk_image_new_from_file("./image/loadrender.png");
	//loadrender=gtk_toolbar_append_item(GTK_TOOLBAR(toolbar),NULL,
			//"Load Render frame","Private",iconw,GTK_SIGNAL_FUNC(loadrendercurframe),NULL);
	iconw=gtk_image_new_from_file("./image/renderframes.png");
	renderframes=gtk_toolbar_append_item(GTK_TOOLBAR(toolbar),NULL,
			TOOLBAR_RENDTOYUV,"Private",iconw,GTK_SIGNAL_FUNC(getrenderframes),NULL);

	iconw=gtk_image_new_from_file("./image/renderframesh264.png");
	renderframesh264=gtk_toolbar_append_item(GTK_TOOLBAR(toolbar),NULL,
			TOOLBAR_RENDTOH264,"Private",iconw,GTK_SIGNAL_FUNC(renderframes2h264),NULL);
	
	gtk_toolbar_append_space(GTK_TOOLBAR(toolbar));
	gtk_fixed_put(GTK_FIXED(g_wdgMainInterface),toolbar,TOOLBARORGX,TOOLBARORGY);

	/* 分区域*/
	frame1 = gtk_frame_new(AREA_LAYER);
	gtk_widget_set_size_request( frame1, LAYERWIDTH, LAYERHEIGHT);
	gtk_fixed_put(GTK_FIXED(g_wdgMainInterface),frame1,LAYERORGX,LAYERORGY);

	frame2 = gtk_frame_new(AREA_FRMEDIT);
	gtk_widget_set_size_request( frame2, IMGWIDTH, IMGHEIGHT);
	gtk_fixed_put(GTK_FIXED(g_wdgMainInterface),frame2,IMGORGX,IMGORGY-5);
	
	frame3 = gtk_frame_new(AREA_QKSET);
	gtk_widget_set_size_request( frame3, PRMWIDTH, PRMHEIGHT);
	gtk_fixed_put(GTK_FIXED(g_wdgMainInterface),frame3,PRMORGX,PRMORGY);

	frame4 = gtk_frame_new(AREA_FRAME);
	gtk_widget_set_size_request( frame4, FRMWIDTH, FRMHEIGHT);
	gtk_fixed_put(GTK_FIXED(g_wdgMainInterface),frame4,FRMORGX,FRMORGY);

	frame5 = gtk_frame_new(AREA_KEYFRM);
	gtk_widget_set_size_request( frame5, KFRMWIDTH, KFRMHEIGHT);
	gtk_fixed_put(GTK_FIXED(g_wdgMainInterface),frame5,KFRMORGX,KFRMORGY);

	/* 显示模式选择*/
	g_wdgSegmentMode = gtk_toggle_button_new_with_label(AREA_FRMEDIT_SEGM);
	gtk_fixed_put(GTK_FIXED(g_wdgMainInterface),g_wdgSegmentMode, 958, 54);
	gtk_widget_set_size_request( g_wdgSegmentMode, 120, 26);
	g_wdgRenderMode = gtk_toggle_button_new_with_label(AREA_FRMEDIT_RENM);
	gtk_fixed_put(GTK_FIXED(g_wdgMainInterface),g_wdgRenderMode, 1074, 54);
	gtk_widget_set_size_request( g_wdgRenderMode, 120, 26);
	/*iconw=gtk_image_new_from_file("./image/segmentmode.png");
	box=gtk_hbox_new(FALSE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (box), 2);
	gtk_box_pack_start (GTK_BOX (box), iconw, FALSE, FALSE, 3);
	button = gtk_button_new (); 
	gtk_container_add (GTK_CONTAINER (button), box);
	gtk_fixed_put(GTK_FIXED(g_wdgMainInterface),button,36, 80);
	g_signal_connect( G_OBJECT(button), "clicked", G_CALLBACK(setnorendermode), NULL );
	iconw=gtk_image_new_from_file("./image/rendermode.png");
	box=gtk_hbox_new(FALSE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (box), 2);
	gtk_box_pack_start (GTK_BOX (box), iconw, FALSE, FALSE, 3);
	button = gtk_button_new (); 
	gtk_container_add (GTK_CONTAINER (button), box);
	gtk_fixed_put(GTK_FIXED(g_wdgMainInterface),button,116, 80);
	g_signal_connect( G_OBJECT(button), "clicked", G_CALLBACK(setrendermode), NULL );*/
	
	//gtk_radio_button_set_group(GTK_RADIO_BUTTON(g_wdgRenderMode),gtk_radio_button_group(GTK_RADIO_BUTTON(g_wdgSegmentMode)));

	gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(g_wdgSegmentMode),  TRUE); 
	gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(g_wdgRenderMode),  FALSE); 
	
	g_signal_connect( G_OBJECT(g_wdgSegmentMode), "released", G_CALLBACK(setnorendermode), NULL );
	g_signal_connect( G_OBJECT(g_wdgRenderMode), "released", G_CALLBACK(setrendermode), NULL );
		
	/* 层次操作区*/
	for(i=0;i<LAYERBUTTONNUMMAX;i++)
	{
		color[i].red = colorset16[i][0];
    	color[i].green = colorset16[i][1];
    	color[i].blue = colorset16[i][2];  
	}

	//layerbutton.addbutton = gtk_button_new_with_label("   ADD LAYER   ");
	//gtk_fixed_put(GTK_FIXED(g_wdgMainInterface),layerbutton.addbutton, LAYERORGX+LAYERBUTOFFSETX , LAYERORGY+LAYERBUTOFFSET);
	//g_signal_connect( G_OBJECT(layerbutton.addbutton), "clicked", G_CALLBACK(add_layer), NULL );

	layerbutton.layernum[0] = 0;
	layerbutton.layer[0]= gtk_check_button_new_with_label(AREA_LAYER_ALS);
	gtk_fixed_put(GTK_FIXED(g_wdgMainInterface),layerbutton.layer[0], LAYERORGX+LAYERBUTOFFSETX-1, LAYERORGY+LAYERBUTOFFSETY-5);
	g_signal_connect( G_OBJECT(layerbutton.layer[0]), "pressed", G_CALLBACK(all_layer), NULL );
	gtk_widget_modify_text(layerbutton.layer[0],GTK_STATE_NORMAL, &color[0]);

	layerbutton.layeredit[0]= gtk_radio_button_new(NULL);
	gtk_fixed_put(GTK_FIXED(g_wdgMainInterface),layerbutton.layeredit[0], LAYERORGX+LAYERBUTOFFSETX-23, LAYERORGY+LAYERBUTOFFSETY-5);

	g_signal_connect( G_OBJECT(layerbutton.layeredit[0]), "released", G_CALLBACK(seteditlayer), &layerbutton.layernum[0] );

	scrolled_window = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_set_size_request( scrolled_window, LAYERWIDTH-10, LAYERHEIGHT-35);
	gtk_container_set_border_width (GTK_CONTAINER (scrolled_window), 10);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),	GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
	gtk_fixed_put(GTK_FIXED(g_wdgMainInterface),scrolled_window,LAYERORGX+5, LAYERORGY+33);
	gtk_widget_show (scrolled_window);

	vbox=gtk_vbox_new(TRUE, 0);

	for(i=1;i<LAYERBUTTONNUMMAX;i++)
	{
		hbox=gtk_hbox_new(FALSE,0);
    	gtk_box_pack_start(GTK_BOX(vbox),hbox,TRUE,TRUE,0);
		
		layerbutton.layernum[i] = i;
		layerbutton.layeredit[i]= gtk_radio_button_new(NULL);
		gtk_widget_set_size_request( layerbutton.layeredit[i], 20, 20);
		gtk_box_pack_start(GTK_BOX(hbox),layerbutton.layeredit[i],FALSE,FALSE,0);
		//gtk_fixed_put(GTK_FIXED(g_wdgMainInterface),layerbutton.layeredit[i], LAYERORGX+LAYERBUTOFFSETX-20, LAYERORGY+LAYERBUTOFFSET+LAYERBUTOFFSETY+8+LAYERBUTOFFSETY*i);
		layerbutton.layer[i]= gtk_check_button_new();
		gtk_widget_set_size_request( layerbutton.layer[i], 30, 20);
		gtk_box_pack_start(GTK_BOX(hbox),layerbutton.layer[i],FALSE,FALSE,0);
		//gtk_fixed_put(GTK_FIXED(g_wdgMainInterface),layerbutton.layer[i], LAYERORGX+LAYERBUTOFFSETX, LAYERORGY+LAYERBUTOFFSET+LAYERBUTOFFSETY+8+LAYERBUTOFFSETY*i);
		layerbutton.layerlabel[i] = gtk_label_new(lvnum);
		sprintf_s(lvnum,"%s %2d", AREA_LAYER_L,i);
		gtk_label_set_text((GtkLabel *)layerbutton.layerlabel[i],lvnum);
		gtk_widget_modify_fg(GTK_WIDGET(layerbutton.layerlabel[i]), GTK_STATE_NORMAL, &color[i]);
		gtk_box_pack_start(GTK_BOX(hbox),layerbutton.layerlabel[i],FALSE,FALSE,0);
		//gtk_fixed_put(GTK_FIXED(g_wdgMainInterface),layerbutton.layerlabel[i], LAYERORGX+LAYERBUTOFFSETX+20, LAYERORGY+LAYERBUTOFFSET+LAYERBUTOFFSETY+10+LAYERBUTOFFSETY*i);
		//gtk_widget_modify_text(layerbutton.layer[i],GTK_STATE_SELECTED, &color[i]);
		//gtk_widget_modify_bg(GTK_WIDGET(layerbutton.layer[i]), GTK_STATE_SELECTED,  &color[i]);
		gtk_radio_button_set_group(GTK_RADIO_BUTTON(layerbutton.layeredit[i]),gtk_radio_button_group(GTK_RADIO_BUTTON(layerbutton.layeredit[0])));

		g_signal_connect( G_OBJECT(layerbutton.layeredit[i]), "released", G_CALLBACK(seteditlayer), &layerbutton.layernum[i] );
		g_signal_connect( G_OBJECT(layerbutton.layer[i]), "released", G_CALLBACK(draw_layer), &layerbutton.layernum[i] );
		
	}

	gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scrolled_window), vbox);
	gtk_widget_show (vbox);
	
	/* 图像操作区*/
	g_wdgImgProcArea = gtk_drawing_area_new();
		
	gtk_widget_set_size_request( g_wdgImgProcArea, IMGWIDTHMAX*2, IMGHEIGHTMAX*2);
	gtk_fixed_put(GTK_FIXED(g_wdgMainInterface),g_wdgImgProcArea,IMGCENTERX-IMGWIDTHMAX,IMGCENTERY-IMGHEIGHTMAX);
	gtk_signal_connect (GTK_OBJECT (g_wdgImgProcArea), "expose_event", (GtkSignalFunc)expose_event, NULL);

	gtk_signal_connect( GTK_OBJECT( g_wdgMainInterface ), "key_press_event", GTK_SIGNAL_FUNC( keypress_event ), NULL );
	gtk_signal_connect (GTK_OBJECT (g_wdgMainInterface), "key_release_event", GTK_SIGNAL_FUNC (keyrelease_event), NULL);

	//g_wdgKeyFrameDrawArea = gtk_drawing_area_new();
	//gtk_widget_set_size_request( g_wdgKeyFrameDrawArea, KFRMWIDTH, KFRMHEIGHT);
	//gtk_fixed_put(GTK_FIXED(g_wdgMainInterface),g_wdgKeyFrameDrawArea,KFRMORGX,KFRMORGY);

	/* 参数区*/
	/* 使能标记*/
	scrolled_window1 = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_set_size_request( scrolled_window1, PRMWIDTH, PRMHEIGHT-15);
	gtk_container_set_border_width (GTK_CONTAINER (scrolled_window1), 5);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window1),	GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_fixed_put(GTK_FIXED(g_wdgMainInterface),scrolled_window1,PRMORGX, PRMORGY+15);
	gtk_widget_show (scrolled_window1);
	
	vbox1=gtk_vbox_new(FALSE,3);
	//gtk_container_add(GTK_CONTAINER(frame3),vbox1);
	gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scrolled_window1), vbox1);

	hbox=gtk_hbox_new(FALSE,0);
    gtk_box_pack_start(GTK_BOX(vbox1),hbox,FALSE,FALSE,0);
	label=gtk_label_new(AREA_QKSET_SAD);
	gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	gtk_label_set_justify((GtkLabel *)label, GTK_JUSTIFY_LEFT);

	hbox=gtk_hbox_new(FALSE,0);
    gtk_box_pack_start(GTK_BOX(vbox1),hbox,FALSE,FALSE,0);
	g_wdgShowDepY = gtk_radio_button_new_with_label(NULL, AREA_QKSET_YES);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgShowDepY,FALSE,FALSE,0);
	g_wdgShowDepN = gtk_radio_button_new_with_label(NULL, AREA_QKSET_No);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgShowDepN,FALSE,FALSE,0);

	gtk_radio_button_set_group(GTK_RADIO_BUTTON(g_wdgShowDepN),gtk_radio_button_group(GTK_RADIO_BUTTON(g_wdgShowDepY)));

	g_signal_connect( G_OBJECT(g_wdgShowDepY), "released", G_CALLBACK(setshowdepy), NULL );
	g_signal_connect( G_OBJECT(g_wdgShowDepN), "released", G_CALLBACK(setshowdepn), NULL );

	hbox=gtk_hbox_new(FALSE,0);
    gtk_box_pack_start(GTK_BOX(vbox1),hbox,FALSE,FALSE,0);
	label=gtk_label_new(AREA_QKSET_SAS);
	gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	gtk_label_set_justify((GtkLabel *)label, GTK_JUSTIFY_LEFT);

	hbox=gtk_hbox_new(FALSE,0);
    gtk_box_pack_start(GTK_BOX(vbox1),hbox,FALSE,FALSE,0);
	g_wdgShowSegY = gtk_radio_button_new_with_label(NULL, AREA_QKSET_YES);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgShowSegY,FALSE,FALSE,0);
	g_wdgShowSegN = gtk_radio_button_new_with_label(NULL, AREA_QKSET_No);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgShowSegN,FALSE,FALSE,0);

	gtk_radio_button_set_group(GTK_RADIO_BUTTON(g_wdgShowSegY),gtk_radio_button_group(GTK_RADIO_BUTTON(g_wdgShowSegN)));

	g_signal_connect( G_OBJECT(g_wdgShowSegY), "released", G_CALLBACK(setshowautosegy), NULL );
	g_signal_connect( G_OBJECT(g_wdgShowSegN), "released", G_CALLBACK(setshowautosegn), NULL );

	hbox=gtk_hbox_new(FALSE,0);
    gtk_box_pack_start(GTK_BOX(vbox1),hbox,FALSE,FALSE,0);
	label=gtk_label_new(AREA_QKSET_RA);
	gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	gtk_label_set_justify((GtkLabel *)label, GTK_JUSTIFY_LEFT);
	
	hbox=gtk_hbox_new(FALSE,0);
    gtk_box_pack_start(GTK_BOX(vbox1),hbox,FALSE,FALSE,0);
	g_wdgNonEdit = gtk_radio_button_new_with_label(NULL, AREA_QKSET_NR);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgNonEdit,TRUE,TRUE,0);
	hbox=gtk_hbox_new(FALSE,0);
    gtk_box_pack_start(GTK_BOX(vbox1),hbox,FALSE,FALSE,0);
	g_wdgSegEdit = gtk_radio_button_new_with_label(NULL, AREA_QKSET_SR);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgSegEdit,TRUE,TRUE,0);
	hbox=gtk_hbox_new(FALSE,0);
    gtk_box_pack_start(GTK_BOX(vbox1),hbox,FALSE,FALSE,0);
	g_wdgMatEdit = gtk_radio_button_new_with_label(NULL, AREA_QKSET_MR);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgMatEdit,TRUE,TRUE,0);
	hbox=gtk_hbox_new(FALSE,0);
    gtk_box_pack_start(GTK_BOX(vbox1),hbox,FALSE,FALSE,0);
	g_wdgDepEdit = gtk_radio_button_new_with_label(NULL, AREA_QKSET_DR);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgDepEdit,TRUE,TRUE,0);
	

	separator = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox1),separator,FALSE,FALSE,0);

	hbox=gtk_hbox_new(FALSE,0);
    gtk_box_pack_start(GTK_BOX(vbox1),hbox,FALSE,FALSE,0);
	label=gtk_label_new(AREA_QKSET_RM);
	gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	gtk_label_set_justify((GtkLabel *)label, GTK_JUSTIFY_LEFT);

	hbox=gtk_hbox_new(FALSE,0);
    gtk_box_pack_start(GTK_BOX(vbox1),hbox,FALSE,FALSE,0);
	g_wdgEditCircle = gtk_radio_button_new_with_label(NULL, AREA_QKSET_CIR);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgEditCircle,FALSE,FALSE,0);
	g_wdgEditSquare = gtk_radio_button_new_with_label(NULL, AREA_QKSET_SQU);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgEditSquare,FALSE,FALSE,0);

	gtk_radio_button_set_group(GTK_RADIO_BUTTON(g_wdgEditSquare),gtk_radio_button_group(GTK_RADIO_BUTTON(g_wdgEditCircle)));

	g_signal_connect( G_OBJECT(g_wdgEditCircle), "released", G_CALLBACK(setcircle), NULL );
	g_signal_connect( G_OBJECT(g_wdgEditSquare), "released", G_CALLBACK(setsquare), NULL );

	hbox=gtk_hbox_new(FALSE,0);
    gtk_box_pack_start(GTK_BOX(vbox1),hbox,FALSE,FALSE,0);

	iconw=gtk_image_new_from_file("./image/radiusminus.png");
	box=gtk_hbox_new(FALSE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (box), 2);
	gtk_box_pack_start (GTK_BOX (box), iconw, FALSE, FALSE, 3);
	button = gtk_button_new (); 
	gtk_container_add (GTK_CONTAINER (button), box);
	gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, FALSE, 3);
	g_signal_connect( G_OBJECT(button), "clicked", G_CALLBACK(radiusminus), NULL );

	g_wdgEditRadius = gtk_entry_new_with_max_length(8);
    gtk_entry_set_text(GTK_ENTRY(g_wdgEditRadius), "0");
	gtk_widget_set_size_request( g_wdgEditRadius, 45, 25);
	gtk_entry_set_alignment(GTK_ENTRY(g_wdgEditRadius), 0.5);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgEditRadius,FALSE,FALSE,0);

	iconw=gtk_image_new_from_file("./image/radiusplus.png");
	box=gtk_hbox_new(FALSE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (box), 2);
	gtk_box_pack_start (GTK_BOX (box), iconw, FALSE, FALSE, 3);
	button = gtk_button_new (); 
	gtk_container_add (GTK_CONTAINER (button), box);
	gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, FALSE, 3);
	g_signal_connect( G_OBJECT(button), "clicked", G_CALLBACK(radiusplus), NULL );

	iconw=gtk_image_new_from_file("./image/radiusset.png");
	box=gtk_hbox_new(FALSE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (box), 2);
	gtk_box_pack_start (GTK_BOX (box), iconw, FALSE, FALSE, 3);
	button = gtk_button_new (); 
	gtk_container_add (GTK_CONTAINER (button), box);
	gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, FALSE, 3);
	g_signal_connect( G_OBJECT(button), "clicked", G_CALLBACK(radiusset), NULL );

	separator = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox1),separator,FALSE,FALSE,0);

	hbox=gtk_hbox_new(FALSE,0);
    gtk_box_pack_start(GTK_BOX(vbox1),hbox,FALSE,FALSE,0);
	label=gtk_label_new(AREA_QKSET_MSS);
	gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	gtk_label_set_justify((GtkLabel *)label, GTK_JUSTIFY_LEFT);

	hbox=gtk_hbox_new(FALSE,0);
    gtk_box_pack_start(GTK_BOX(vbox1),hbox,FALSE,FALSE,0);

	iconw=gtk_image_new_from_file("./image/radiusminus.png");
	box=gtk_hbox_new(FALSE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (box), 2);
	gtk_box_pack_start (GTK_BOX (box), iconw, FALSE, FALSE, 3);
	button = gtk_button_new (); 
	gtk_container_add (GTK_CONTAINER (button), box);
	gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, FALSE, 3);
	g_signal_connect( G_OBJECT(button), "clicked", G_CALLBACK(radiusminusstrength), NULL );

	g_wdgEditStrength = gtk_entry_new_with_max_length(8);
    gtk_entry_set_text(GTK_ENTRY(g_wdgEditStrength), "0");
	gtk_widget_set_size_request( g_wdgEditStrength, 45, 25);
	gtk_entry_set_alignment(GTK_ENTRY(g_wdgEditStrength), 0.5);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgEditStrength,FALSE,FALSE,0);

	iconw=gtk_image_new_from_file("./image/radiusplus.png");
	box=gtk_hbox_new(FALSE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (box), 2);
	gtk_box_pack_start (GTK_BOX (box), iconw, FALSE, FALSE, 3);
	button = gtk_button_new (); 
	gtk_container_add (GTK_CONTAINER (button), box);
	gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, FALSE, 3);
	g_signal_connect( G_OBJECT(button), "clicked", G_CALLBACK(radiusplusstrength), NULL );

	iconw=gtk_image_new_from_file("./image/radiusset.png");
	box=gtk_hbox_new(FALSE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (box), 2);
	gtk_box_pack_start (GTK_BOX (box), iconw, FALSE, FALSE, 3);
	button = gtk_button_new (); 
	gtk_container_add (GTK_CONTAINER (button), box);
	gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, FALSE, 3);
	g_signal_connect( G_OBJECT(button), "clicked", G_CALLBACK(radiussetstrength), NULL );

	separator = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox1),separator,FALSE,FALSE,0);

	hbox=gtk_hbox_new(FALSE,0);
    gtk_box_pack_start(GTK_BOX(vbox1),hbox,FALSE,FALSE,0);
	label=gtk_label_new(AREA_QKSET_RP);
	gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	gtk_label_set_justify((GtkLabel *)label, GTK_JUSTIFY_LEFT);

	hbox=gtk_hbox_new(FALSE,0);
    gtk_box_pack_start(GTK_BOX(vbox1),hbox,FALSE,FALSE,0);
	g_wdgRenEdit = gtk_radio_button_new_with_label(NULL, AREA_QKSET_RR);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgRenEdit,TRUE,TRUE,0);
	
	gtk_radio_button_set_group(GTK_RADIO_BUTTON(g_wdgSegEdit),gtk_radio_button_group(GTK_RADIO_BUTTON(g_wdgNonEdit)));
	gtk_radio_button_set_group(GTK_RADIO_BUTTON(g_wdgMatEdit),gtk_radio_button_group(GTK_RADIO_BUTTON(g_wdgNonEdit)));
	gtk_radio_button_set_group(GTK_RADIO_BUTTON(g_wdgDepEdit),gtk_radio_button_group(GTK_RADIO_BUTTON(g_wdgNonEdit)));
	gtk_radio_button_set_group(GTK_RADIO_BUTTON(g_wdgRenEdit),gtk_radio_button_group(GTK_RADIO_BUTTON(g_wdgNonEdit)));

	g_signal_connect( G_OBJECT(g_wdgNonEdit), "released", G_CALLBACK(nonedit), NULL );
	g_signal_connect( G_OBJECT(g_wdgSegEdit), "released", G_CALLBACK(segedit), NULL );
	g_signal_connect( G_OBJECT(g_wdgMatEdit), "released", G_CALLBACK(matedit), NULL );
	g_signal_connect( G_OBJECT(g_wdgDepEdit), "released", G_CALLBACK(depedit), NULL );
	g_signal_connect( G_OBJECT(g_wdgRenEdit), "released", G_CALLBACK(renedit), NULL );

	hbox=gtk_hbox_new(FALSE,0);
    gtk_box_pack_start(GTK_BOX(vbox1),hbox,FALSE,FALSE,0);
	label=gtk_label_new(LABEL_ALPHA);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	g_wdgEditalpha = gtk_entry_new_with_max_length(8);
    gtk_entry_set_text(GTK_ENTRY(g_wdgEditalpha), "0");
	gtk_widget_set_size_request( g_wdgEditalpha, 50, 25);
	gtk_entry_set_alignment(GTK_ENTRY(g_wdgEditalpha), 0.5);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgEditalpha,FALSE,FALSE,0);
	iconw=gtk_image_new_from_file("./image/radiusset.png");
	box=gtk_hbox_new(FALSE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (box), 2);
	gtk_box_pack_start (GTK_BOX (box), iconw, FALSE, FALSE, 3);
	button = gtk_button_new (); 
	gtk_container_add (GTK_CONTAINER (button), box);
	gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, FALSE, 3);
	g_signal_connect( G_OBJECT(button), "clicked", G_CALLBACK(quicksetalpha), NULL );

	hbox=gtk_hbox_new(FALSE,0);
    gtk_box_pack_start(GTK_BOX(vbox1),hbox,FALSE,FALSE,0);
	label=gtk_label_new(LABEL_FORCAL);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	g_wdgEditfocal = gtk_entry_new_with_max_length(8);
    gtk_entry_set_text(GTK_ENTRY(g_wdgEditfocal), "0");
	gtk_widget_set_size_request( g_wdgEditfocal, 50, 25);
	gtk_entry_set_alignment(GTK_ENTRY(g_wdgEditfocal), 0.5);
	gtk_box_pack_start(GTK_BOX(hbox),g_wdgEditfocal,FALSE,FALSE,0);
	iconw=gtk_image_new_from_file("./image/radiusset.png");
	box=gtk_hbox_new(FALSE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (box), 2);
	gtk_box_pack_start (GTK_BOX (box), iconw, FALSE, FALSE, 3);
	button = gtk_button_new (); 
	gtk_container_add (GTK_CONTAINER (button), box);
	gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, FALSE, 3);
	g_signal_connect( G_OBJECT(button), "clicked", G_CALLBACK(quicksetfocal), NULL );
	

	gtk_widget_show (vbox1);
	
	/* 帧操作区*/
	iconw=gtk_image_new_from_file("./image/firstframe.png");
	box=gtk_hbox_new(FALSE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (box), 2);
	gtk_box_pack_start (GTK_BOX (box), iconw, FALSE, FALSE, 3);
	button = gtk_button_new (); 
	gtk_container_add (GTK_CONTAINER (button), box);
	gtk_fixed_put(GTK_FIXED(g_wdgMainInterface),button,FRMORGX+FRMOFFSET, FRMORGY+FRMOFFSETY);
	g_signal_connect( G_OBJECT(button), "clicked", G_CALLBACK(firstframe), NULL );

	iconw=gtk_image_new_from_file("./image/backwardplay.png");
	box=gtk_hbox_new(FALSE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (box), 2);
	gtk_box_pack_start (GTK_BOX (box), iconw, FALSE, FALSE, 3);
	button = gtk_button_new (); 
	gtk_container_add (GTK_CONTAINER (button), box);
	gtk_fixed_put(GTK_FIXED(g_wdgMainInterface),button,FRMORGX+FRMOFFSET+FRMOFFSETX, FRMORGY+FRMOFFSETY);
	g_signal_connect( G_OBJECT(button), "clicked", G_CALLBACK(backwardplay), NULL );

	iconw=gtk_image_new_from_file("./image/previousframe.png");
	box=gtk_hbox_new(FALSE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (box), 2);
	gtk_box_pack_start (GTK_BOX (box), iconw, FALSE, FALSE, 3);
	button = gtk_button_new (); 
	gtk_container_add (GTK_CONTAINER (button), box);
	gtk_fixed_put(GTK_FIXED(g_wdgMainInterface),button,FRMORGX+FRMOFFSET+2*FRMOFFSETX, FRMORGY+FRMOFFSETY);
	g_signal_connect( G_OBJECT(button), "clicked", G_CALLBACK(previousframe), NULL );

	g_wdgFrameNum = gtk_label_new("  0  /   0");
    gtk_fixed_put(GTK_FIXED(g_wdgMainInterface),g_wdgFrameNum, FRMORGX+FRMOFFSET+3*FRMOFFSETX-3, FRMORGY+FRMOFFSETY+5);
	gtk_label_set_justify((GtkLabel *)g_wdgFrameNum, GTK_JUSTIFY_CENTER);

	iconw=gtk_image_new_from_file("./image/nextframe.png");
	box=gtk_hbox_new(FALSE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (box), 2);
	gtk_box_pack_start (GTK_BOX (box), iconw, FALSE, FALSE, 3);
	button = gtk_button_new (); 
	gtk_container_add (GTK_CONTAINER (button), box);
	gtk_fixed_put(GTK_FIXED(g_wdgMainInterface),button,FRMORGX+FRMOFFSET+2*FRMOFFSETX+130, FRMORGY+FRMOFFSETY);
	g_signal_connect( G_OBJECT(button), "clicked", G_CALLBACK(nextframe), NULL );

	iconw=gtk_image_new_from_file("./image/forwardplay.png");
	box=gtk_hbox_new(FALSE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (box), 2);
	gtk_box_pack_start (GTK_BOX (box), iconw, FALSE, FALSE, 3);
	button = gtk_button_new (); 
	gtk_container_add (GTK_CONTAINER (button), box);
	gtk_fixed_put(GTK_FIXED(g_wdgMainInterface),button,FRMORGX+FRMOFFSET+3*FRMOFFSETX+130, FRMORGY+FRMOFFSETY);
	g_signal_connect( G_OBJECT(button), "clicked", G_CALLBACK(forwardplay), NULL );

	iconw=gtk_image_new_from_file("./image/lastframe.png");
	box=gtk_hbox_new(FALSE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (box), 2);
	gtk_box_pack_start (GTK_BOX (box), iconw, FALSE, FALSE, 3);
	button = gtk_button_new (); 
	gtk_container_add (GTK_CONTAINER (button), box);
	gtk_fixed_put(GTK_FIXED(g_wdgMainInterface),button,FRMORGX+FRMOFFSET+4*FRMOFFSETX+130, FRMORGY+FRMOFFSETY);
	g_signal_connect( G_OBJECT(button), "clicked", G_CALLBACK(lastframe), NULL );

	g_wdgInpFrameNum = gtk_entry_new_with_max_length(8);
    gtk_entry_set_text(GTK_ENTRY(g_wdgInpFrameNum), "0");
	gtk_widget_set_size_request( g_wdgInpFrameNum, FRMENTRYW, FRMENTRYH);
	gtk_entry_set_alignment(GTK_ENTRY(g_wdgInpFrameNum), 0.5);
    gtk_fixed_put(GTK_FIXED(g_wdgMainInterface),g_wdgInpFrameNum, FRMORGX+FRMOFFSET+5*FRMOFFSETX+130, FRMORGY+FRMOFFSETY+1);

	jmp.framenum = 0;
	jmp.jumpmode = JMPMODECUR;
	iconw=gtk_image_new_from_file("./image/jumpframe.png");
	box=gtk_hbox_new(FALSE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (box), 2);
	gtk_box_pack_start (GTK_BOX (box), iconw, FALSE, FALSE, 3);
	button = gtk_button_new (); 
	gtk_container_add (GTK_CONTAINER (button), box);
	gtk_fixed_put(GTK_FIXED(g_wdgMainInterface),button,FRMORGX+FRMOFFSET+5*FRMOFFSETX+200, FRMORGY+FRMOFFSETY);
	g_signal_connect( G_OBJECT(button), "clicked", G_CALLBACK(gotoframe), NULL );

	/*关键帧操作区*/
	iconw=gtk_image_new_from_file("./image/autoscan.png");
	box=gtk_hbox_new(FALSE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (box), 2);
	gtk_box_pack_start (GTK_BOX (box), iconw, FALSE, FALSE, 3);
	button = gtk_button_new (); 
	gtk_container_add (GTK_CONTAINER (button), box);
	gtk_fixed_put(GTK_FIXED(g_wdgMainInterface),button,KFRMORGX+KFRMOFFSET, KFRMORGY+KFRMOFFSETY);
	g_signal_connect( G_OBJECT(button), "clicked", G_CALLBACK(AutoKeyFrameScan), NULL );

	iconw=gtk_image_new_from_file("./image/previouskeyframe.png");
	box=gtk_hbox_new(FALSE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (box), 2);
	gtk_box_pack_start (GTK_BOX (box), iconw, FALSE, FALSE, 3);
	button = gtk_button_new (); 
	gtk_container_add (GTK_CONTAINER (button), box);
	gtk_fixed_put(GTK_FIXED(g_wdgMainInterface),button,KFRMORGX+KFRMOFFSET+KFRMOFFSETX, KFRMORGY+KFRMOFFSETY);
	g_signal_connect( G_OBJECT(button), "clicked", G_CALLBACK(previouskeyframe), NULL );
	
	g_wdgKeyFrameNum = gtk_label_new("   0  -  0 -  0  ");
    gtk_fixed_put(GTK_FIXED(g_wdgMainInterface),g_wdgKeyFrameNum, KFRMORGX+KFRMOFFSET+2*KFRMOFFSETX-10, KFRMORGY+KFRMOFFSETY+5);

	iconw=gtk_image_new_from_file("./image/nextkeyframe.png");
	box=gtk_hbox_new(FALSE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (box), 2);
	gtk_box_pack_start (GTK_BOX (box), iconw, FALSE, FALSE, 3);
	button = gtk_button_new (); 
	gtk_container_add (GTK_CONTAINER (button), box);
	gtk_fixed_put(GTK_FIXED(g_wdgMainInterface),button,KFRMORGX+KFRMOFFSET+2*KFRMOFFSETX+120, KFRMORGY+KFRMOFFSETY);
	g_signal_connect( G_OBJECT(button), "clicked", G_CALLBACK(nextkeyframe), NULL );

	iconw=gtk_image_new_from_file("./image/setkeyframe.png");
	box=gtk_hbox_new(FALSE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (box), 2);
	gtk_box_pack_start (GTK_BOX (box), iconw, FALSE, FALSE, 3);
	button = gtk_button_new (); 
	gtk_container_add (GTK_CONTAINER (button), box);
	gtk_fixed_put(GTK_FIXED(g_wdgMainInterface),button,KFRMORGX+KFRMOFFSET+3*KFRMOFFSETX+120, KFRMORGY+KFRMOFFSETY);
	g_signal_connect( G_OBJECT(button), "clicked", G_CALLBACK(SetKeyframe), NULL );

	iconw=gtk_image_new_from_file("./image/setnonkeyframe.png");
	box=gtk_hbox_new(FALSE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (box), 2);
	gtk_box_pack_start (GTK_BOX (box), iconw, FALSE, FALSE, 3);
	button = gtk_button_new (); 
	gtk_container_add (GTK_CONTAINER (button), box);
	gtk_fixed_put(GTK_FIXED(g_wdgMainInterface),button,KFRMORGX+KFRMOFFSET+4*KFRMOFFSETX+120, KFRMORGY+KFRMOFFSETY);
	g_signal_connect( G_OBJECT(button), "clicked", G_CALLBACK(SetNonKeyframe), NULL );

	/* Info */
	g_wdgInfo = gtk_label_new("   Section 1 of 1 Time 0s to 0s Coordinate Position X: 0 Y: 0   /   Color R: 0 G: 0 B: 0   /   Layer: 0 Base: 0  /   Depth: 0  /  Render Ref: 0  ");
    gtk_fixed_put(GTK_FIXED(g_wdgMainInterface),g_wdgInfo, INFOORGX, INFOORGY);

#if 1
	g_wdgProcessBar = gtk_progress_bar_new ();
	gtk_progress_bar_set_orientation( GTK_PROGRESS_BAR(g_wdgProcessBar), GTK_PROGRESS_LEFT_TO_RIGHT );
	gtk_progress_bar_set_fraction ( GTK_PROGRESS_BAR(g_wdgProcessBar), 0.0);
	gtk_fixed_put(GTK_FIXED(g_wdgMainInterface),g_wdgProcessBar, 25, INFOORGY);
	gtk_widget_set_size_request( g_wdgProcessBar, 136, 18);
	g_Processbar.processbar = g_wdgProcessBar;
	g_Processbar.flag = &g_setsegframesbarflag;
	g_Processbar.val = &g_setsegframesbarval;
	SProcessBar * pbar = &g_Processbar;
#endif

	g_status_bar = gtk_statusbar_new (); 
	gtk_fixed_put(GTK_FIXED(g_wdgMainInterface),g_status_bar, 360, 5);
	gtk_widget_set_size_request( g_status_bar, 810, 18);
	//gtk_statusbar_set_alignment(GTK_STATUSBAR (g_status_bar), 1);
	gtk_widget_show (g_status_bar);
    g_context_id = gtk_statusbar_get_context_id(GTK_STATUSBAR (g_status_bar), "Statusbar");
 	gtk_statusbar_push (GTK_STATUSBAR (g_status_bar), GPOINTER_TO_INT(g_context_id), "2D to 3D Converter");

	/*显示主窗口*/
	g_signal_connect( G_OBJECT(window), "delete-event", G_CALLBACK(delete_event_fun), NULL );

	g_signal_connect (window, "button_press_event",
			G_CALLBACK (scribble_button_press_event), NULL);
	g_signal_connect (window, "motion_notify_event",
			G_CALLBACK (scribble_motion_notify_event), NULL);
	gtk_widget_set_events (window, gtk_widget_get_events (window)| GDK_BUTTON_PRESS_MASK| GDK_POINTER_MOTION_MASK| GDK_POINTER_MOTION_HINT_MASK);
	//gtk_widget_set_events (window, gtk_widget_get_events (window)|GDK_ALL_EVENTS_MASK);

	
	gtk_widget_show_all( GTK_WIDGET(window) ); 

	gtk_timeout_add(TIMEINTER, (GSourceFunc) time_handler, pbar);
	
	/* Start main loop */
	gdk_threads_enter(); 
    gtk_main();
	gdk_threads_leave(); 

	return 0;
}

