
#include "common.h"
#include "project.h"
// 初始点击一层，打开文件后该层不起作用，要先关闭再打开 TODO


/*
void add_layer(GtkWidget * button,GtkWidget * data)
{
	//printf("add_layer\n");

	if(g_frameMaxNum>0)
	{
		g_frames[g_frameIndex].LayerVisibleNum++;
		if(g_frames[g_frameIndex].LayerVisibleNum>LAYERBUTTONNUMMAX)
			g_frames[g_frameIndex].LayerVisibleNum = LAYERBUTTONNUMMAX;
		//else
		//{
			//gtk_widget_show ((GtkWidget *)layerbutton.layer[g_frames[g_frameIndex].LayerVisibleNum-1]); 
			//gtk_widget_show ((GtkWidget *)layerbutton.layeredit[g_frames[g_frameIndex].LayerVisibleNum-1]); 
			//gtk_widget_show ((GtkWidget *)layerbutton.layerlabel[g_frames[g_frameIndex].LayerVisibleNum-1]); 
		//}
	}
	else
		messagebox("Please open a file first.");


	return ;
}
*/

void all_layer(GtkWidget * button,GtkWidget * data)
{
	int i;

	if(g_frameMaxNum>0)
	{
		if(GTK_TOGGLE_BUTTON(button)->active) // 不知道为什么这个是反的??? TODO
		{
			//printf("draw_layer %d active\n", 0);
			for(i=1;i<=g_frames[g_frameIndex].LayerVisibleNum;i++)
			{
				gtk_toggle_button_set_state((GtkToggleButton *)layerbutton.layer[i], false);
				g_frames[g_frameIndex].SegLabelMask &= (~(1<<i));
			}
		}
		else
		{
			//printf("draw_layer %d inactive\n", 0);
			for(i=1;i<=g_frames[g_frameIndex].LayerVisibleNum;i++)
			{
				gtk_toggle_button_set_state((GtkToggleButton *)layerbutton.layer[i], true);
				g_frames[g_frameIndex].SegLabelMask |= 1<<i;
			}
		}
		//g_frames[g_frameIndex].SegLabel = g_frameWidth*g_frameHeight + g_frames[g_frameIndex].SegLabelMask;
		//printf("a g_frameSegLabel: %x \n", g_frameSegLabel);

		if(g_enlargeflag==0)
		{
			if(g_renderflag==0)
			{
				//可优化
				gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
				              IMGWIDTHMAX-g_frameWidth, IMGHEIGHTMAX-g_frameHeight, g_frameWidth, g_frameHeight,
				              GDK_RGB_DITHER_MAX, g_frames[g_frameIndex].rgbinput, g_frameWidth * 3);
				ShowSegandDep(g_frames[g_frameIndex].SegLabelMask);
			}
			else
			{
				ShowDep(g_frames[g_frameIndex].SegLabelMask);
			}
		}
		else
		{
			SegEnlargedisplay(g_frames[g_frameIndex].SegLabelMask);
		}
	}
	else
	{
		//messagebox("Please open a file first.");
	}
	
	return ;
}

void setbaselayer()
{	
	if(g_frameMaxNum>0)
	{
		if(g_frames[g_frameIndex].LayerEditNum>0)
			g_frames[g_frameIndex].LayerBaseNum = g_frames[g_frameIndex].LayerEditNum;
	}
	else
	{
		//messagebox("Please open a file first.");
	}
	return ;
}


void seteditlayer(GtkWidget * button, GtkWidget * data)
{
	int i;
	i = *(int *)(data);
	
	if(g_frameMaxNum>0)
	{
		g_frames[g_frameIndex].LayerEditNum = i;
		if(i>0)
		{
			if(GTK_TOGGLE_BUTTON((GtkToggleButton *)layerbutton.layer[i])->active)
			{
				//g_frames[g_frameIndex].SegLabelMask |= 1<<i;
				//printf("draw_layer %d %x active\n", i, i);
				//gtk_toggle_button_set_state((GtkToggleButton *)layerbutton.layer[i], true);
			}
			else
			{
				g_frames[g_frameIndex].SegLabelMask |= 1<<i;
				//printf("draw_layer %d %x inactive\n", i, i);
				gtk_toggle_button_set_state((GtkToggleButton *)layerbutton.layer[i], true);

				//g_frames[g_frameIndex].SegLabel = g_frameWidth*g_frameHeight + g_frames[g_frameIndex].SegLabelMask;

				if(g_enlargeflag==0)
				{
					if(g_renderflag==0)
					{
						//可优化
						gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
						              IMGWIDTHMAX-g_frameWidth, IMGHEIGHTMAX-g_frameHeight, g_frameWidth, g_frameHeight,
						              GDK_RGB_DITHER_MAX, g_frames[g_frameIndex].rgbinput, g_frameWidth * 3);
						ShowSegandDep(g_frames[g_frameIndex].SegLabelMask);
					}
					else
					{
						ShowDep(g_frames[g_frameIndex].SegLabelMask);
					}
				}
				else
				{
					SegEnlargedisplay(g_frames[g_frameIndex].SegLabelMask);
				}
			}
		}
		else
		{
			g_frames[g_frameIndex].LayerEditNum = 0;
		}		

		if(g_frames[g_frameIndex].LayerEditNum>g_frames[g_frameIndex].LayerVisibleNum)
			g_frames[g_frameIndex].LayerVisibleNum = g_frames[g_frameIndex].LayerEditNum;
	}
	else
	{
		//messagebox("Please open a file first.");
	}
	return ;
}

void get_baselayers(GtkWidget* button,SBaseLayerSelect *wdv)
{
	int i;
	int framefirst, framelast, baselayer;
	char filename[256];
	
    char *str=(char *)malloc(4*sizeof(char));

	str=(char*)gtk_entry_get_text(GTK_ENTRY(wdv->entryfirst));
	framefirst = str2num(str);  
	if(framefirst<1)  framefirst=1;
	if(framefirst>g_frameMaxNum)  framefirst=g_frameMaxNum;
	str=(char*)gtk_entry_get_text(GTK_ENTRY(wdv->entrylast));
	framelast = str2num(str);  
	if(framelast<1)  framelast=1;
	if(framelast>g_frameMaxNum)  framelast=g_frameMaxNum;
	str=(char*)gtk_entry_get_text(GTK_ENTRY(wdv->entrybase));
	baselayer = str2num(str);  
	if(baselayer<1)  baselayer=1;
	if(baselayer>=LAYERBUTTONNUMMAX)  baselayer=1;

	if(framefirst>framelast)
		framelast = framefirst;

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
		
	for(i=framefirst-1;i<framelast;i++)
	{
		if(g_keyframeflag)
			sprintf(filename,"./%s/info/%d_infosnap_%d.dat",cfgparams.projectname,g_sectioncur,g_keyframenumarr[i]+1);
		else
			sprintf(filename,"./%s/info/%d_infosnap_%d.dat",cfgparams.projectname,g_sectioncur,i+1);
		if((g_frames[i].finfo=fopen(filename,"rb"))!=NULL)
		{
			// load info file
			loadinfofile(g_frames[i].finfo, i, 0);
			fclose(g_frames[i].finfo);
		}
		g_frames[i].LayerBaseNum = baselayer;

		if(g_frames[i].SegFlag)// &&g_frames[g_frameIndex].ChangeFlag==1) // TODO 可优化，判断是否修改
		{
			if(g_keyframeflag)
				sprintf(filename,"./%s/info/%d_infosnap_%d.dat",cfgparams.projectname,g_sectioncur,g_keyframenumarr[i]+1);
			else
				sprintf(filename,"./%s/info/%d_infosnap_%d.dat",cfgparams.projectname,g_sectioncur,i+1);
			if((g_frames[i].finfo=fopen(filename,"wb"))!=NULL)
			{
				// save info file
				saveinfofile(g_frames[i].finfo, i);
				fclose(g_frames[i].finfo);
			}
		}
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

    gtk_widget_hide(wdv->window);

	//if(str) free(str);
}


void getbaselayers(GtkWidget * button,GtkWidget * data)
{
	char buffer[128];

	GtkWidget *window, *entryfirst, *entrylast, *entrybase;
    GtkWidget *label,*vbox,*hbox,*b_ok,*b_cancel, *separator;
	
	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request(window,360,180);
    gtk_window_set_title(GTK_WINDOW(window),TITLE_FS);
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

	hbox=gtk_hbox_new(FALSE,50);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(LABEL_BLN);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	entrybase= gtk_entry_new();
	//hbox=gtk_hbox_new(FALSE,50);
	//gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	sprintf(buffer, "%d", 0);
	gtk_widget_set_size_request( entrybase,70, 25);
    gtk_box_pack_start(GTK_BOX(hbox),entrybase,FALSE,FALSE,0);
	gtk_entry_set_text(GTK_ENTRY(entrybase), buffer);

	SBaseLayerSelect *wdv=(SBaseLayerSelect *)malloc(sizeof(SBaseLayerSelect));
    wdv->window=window;
	wdv->entryfirst=entryfirst;
	wdv->entrylast=entrylast;
	wdv->entrybase=entrybase;

	hbox=gtk_hbox_new(FALSE,50);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
    b_ok=gtk_button_new_with_label(CONF_OK);
    g_signal_connect(G_OBJECT(b_ok),"clicked",G_CALLBACK(get_baselayers),wdv);
    gtk_box_pack_start(GTK_BOX(hbox),b_ok,FALSE,FALSE,50);
    b_cancel=gtk_button_new_with_label(CONF_CANCEL);
    g_signal_connect(G_OBJECT(b_cancel),"clicked",G_CALLBACK(cancel_window),window);
    gtk_box_pack_start(GTK_BOX(hbox),b_cancel,FALSE,FALSE,0);

    gtk_widget_show_all(window);

}


void draw_layer(GtkWidget * button, GtkWidget * data)
{
	int i;
	i = *(int *)(data);

	if(g_frameMaxNum>0)
	{
		if(GTK_TOGGLE_BUTTON(button)->active)
		{
			g_frames[g_frameIndex].SegLabelMask |= 1<<i;
			//printf("draw_layer %d %x active\n", i, g_frameSegLabelMask);
		}
		else
		{
			g_frames[g_frameIndex].SegLabelMask &= (~(1<<i));
			//printf("draw_layer %d %x inactive\n", i, g_frameSegLabelMask);
		}

		//g_frames[g_frameIndex].SegLabel = g_frameWidth*g_frameHeight + g_frames[g_frameIndex].SegLabelMask;

		
		if(g_enlargeflag==0)
		{
			if(g_renderflag==0)
			{
				//可优化
				gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
				              IMGWIDTHMAX-g_frameWidth, IMGHEIGHTMAX-g_frameHeight, g_frameWidth, g_frameHeight,
				              GDK_RGB_DITHER_MAX, g_frames[g_frameIndex].rgbinput, g_frameWidth * 3);
				ShowSegandDep(g_frames[g_frameIndex].SegLabelMask);
			}
			else
			{
				ShowDep(g_frames[g_frameIndex].SegLabelMask);
			}
		}
		else
		{
			SegEnlargedisplay(g_frames[g_frameIndex].SegLabelMask);
		}
	}
	else
	{
		//messagebox("Please open a file first.");
	}
	
	return ;
}

/* 合并可见层次 */
void CombineVisibleLayer()
{
	int i, k;
	int label;
	int idx=0;
	int sz = g_frameWidth*g_frameHeight;
	int szorg = g_frameInWidth*g_frameInHeight;
	int mask  = 0;
	int mink = 1;

	if(g_frameMaxNum>0&&g_frames[g_frameIndex].SegFlag==1)
	{
		mask  = g_frames[g_frameIndex].SegLabelMask;
		if(g_frameResizeFlag==0)
		{
			for(i=0;i<sz;i++) g_segmentedflags[i]=0;
			for(k=1;k<=g_frames[g_frameIndex].LayerVisibleNum;k++)
			{
				if(mask&(1<<k))
				{
					mink = k;
					break;
				}
			}
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

			label = sz + (1<<mink);

			for(i=0;i<sz;i++)
			{
				if(g_segmentedflags[i]==1)
				{
					g_frames[g_frameIndex].SegLabels[i] = label;
				}
			}
		}
		else
		{
			for(i=0;i<szorg;i++) g_segmentedflags[i]=0;
			for(k=1;k<=g_frames[g_frameIndex].LayerVisibleNum;k++)
			{
				if(mask&(1<<k))
				{
					mink = k;
					break;
				}
			}
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

			label = sz + (1<<mink);

			for(i=0;i<szorg;i++)
			{
				if(g_segmentedflags[i]==1)
				{
					g_frames[g_frameIndex].SegLabels0[i] = label;
				}
			}			
			LabelResize(g_frames[g_frameIndex].SegLabels0, g_frames[g_frameIndex].SegLabels, g_yuvorginput.y, g_yuvinput.y, g_frameInWidth, g_frameInHeight, g_frameWidth, g_frameHeight);
		}

		if(g_enlargeflag==0)
		{
			if(g_renderflag==0)
			{
				//可优化
				gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
				              IMGWIDTHMAX-g_frameWidth, IMGHEIGHTMAX-g_frameHeight, g_frameWidth, g_frameHeight,
				              GDK_RGB_DITHER_MAX, g_frames[g_frameIndex].rgbinput, g_frameWidth * 3);
				ShowSegandDep(g_frames[g_frameIndex].SegLabelMask);
			}
			else
			{
				ShowDep(g_frames[g_frameIndex].SegLabelMask);
			}
		}
		else
		{
			SegEnlargedisplay(g_frames[g_frameIndex].SegLabelMask);
		}
		
	}
	else
	{
		//messagebox("Please segment the frame first.");
	}
	return ;
}

void setremainlayer()
{
	int i;
	int label;
	int sz = g_frameWidth*g_frameHeight;
	int szorg = g_frameInWidth*g_frameInHeight;


	if(g_frameMaxNum>0&&g_frames[g_frameIndex].SegFlag==1)
	{
		if(g_frameResizeFlag==0)
		{
			label = sz + (1<<g_frames[g_frameIndex].LayerEditNum);

			for(i=0;i<sz;i++)
			{
				if(g_frames[g_frameIndex].SegLabels[i]<sz)
				{
					g_frames[g_frameIndex].SegLabels[i] = label;
				}
			}
		}
		else
		{
			label = sz + (1<<g_frames[g_frameIndex].LayerEditNum);

			for(i=0;i<szorg;i++)
			{
				if(g_frames[g_frameIndex].SegLabels0[i]<sz)
				{
					g_frames[g_frameIndex].SegLabels0[i] = label;
				}
			}			
			LabelResize(g_frames[g_frameIndex].SegLabels0, g_frames[g_frameIndex].SegLabels, g_yuvorginput.y, g_yuvinput.y, g_frameInWidth, g_frameInHeight, g_frameWidth, g_frameHeight);
		}

		if(g_enlargeflag==0)
		{
			if(g_renderflag==0)
			{
				//可优化
				gdk_draw_rgb_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
				              IMGWIDTHMAX-g_frameWidth, IMGHEIGHTMAX-g_frameHeight, g_frameWidth, g_frameHeight,
				              GDK_RGB_DITHER_MAX, g_frames[g_frameIndex].rgbinput, g_frameWidth * 3);
				ShowSegandDep(g_frames[g_frameIndex].SegLabelMask);
			}
			else
			{
				ShowDep(g_frames[g_frameIndex].SegLabelMask);
			}
		}
		else
		{
			SegEnlargedisplay(g_frames[g_frameIndex].SegLabelMask);
		}
		
	}
	else
	{
		//messagebox("Please segment the frame first.");
	}
	return ;

}

