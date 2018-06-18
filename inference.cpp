
#include "CSuperpixelSeg.h"
#include "project.h"

int g_regionhistdiffthreshold[INFPARAM_STRENGTHMAX] = {8192, 4096, 2048,1024,512,256}; // ��ֵ����Ӧ, TODO

int g_inferenceedgewidth[INFPARAM_STRENGTHMAX] = {0, 2, 4, 6, 8, 10}; // ��Ե���

int g_inferencestrength = INFPARAM_STRENGTHDEF;

int g_inferencemode = INFMODEREG;
int g_inferencedirect = INFDIRPREV;

typedef struct
{
	int cnt;			// ��ǰ֡������������
	int avgx;			// ��ǰ֡���������ĵ�����x
	int avgy;			// ��ǰ֡���������ĵ�����y
	
	int reflabel;		// �ο�֡��Ӧλ��ԭʼ���
	int refcnt;			// �ο�֡��Ӧ��������ظ���
	
	int histdiff;		// ��Ӧ����ֱ��ͼ��
	int refhist[256]; 	// �ο�֡������ֱ��ͼ
	int curhist[256]; 	// ��ǰ֡������ֱ��ͼ
	
}SRegionInfo;

//#define EDGEINF // ���ڱ�Ե������

#define INFWINSIZE 9

void inferenceframe(int refframenum)
{
	int i,j,k;
	int labelnum;
	char filename[256];
	SRegionInfo * regionarr;
	int size = g_frameWidth*g_frameHeight;
	int SegLabelMask, setflag, depthmin, depthmax, depthmode, lb;
	int * diffflags;
	//int depallset = 0;
	
	diffflags = new int[g_frameInWidth*g_frameInHeight];
	
	
	//refframenum = refframenum -1;

	// �ο�֡��Ϣ����
	if(g_keyframeflag)
		sprintf(filename,"./%s/info/%d_infosnap_%d.dat",cfgparams.projectname,g_sectioncur,g_keyframenumarr[refframenum]+1);
	else
		sprintf(filename,"./%s/info/%d_infosnap_%d.dat",cfgparams.projectname,g_sectioncur,refframenum+1);
	if((g_frames[refframenum].finfo=fopen(filename,"rb"))!=NULL)
	{
		// load info file
		loadinfofile(g_frames[refframenum].finfo, refframenum, 1);
		fclose(g_frames[refframenum].finfo);
		//printf("load info file %s\n", filename);
	}

	if(g_frameMaxNum>0 && refframenum>=0 && g_frames[refframenum].SegFlag == 1&& refframenum!=g_frameIndex)
	{
		uint8_t * tmpy = new uint8_t [g_frameWidth*g_frameHeight];
		// ���봦��֡
		if(g_keyframeflag)
			sprintf(filename,"./%s/info/%d_infosnap_%d.dat",cfgparams.projectname,g_sectioncur,g_keyframenumarr[g_frameIndex]+1);
		else
			sprintf(filename,"./%s/info/%d_infosnap_%d.dat",cfgparams.projectname,g_sectioncur,g_frameIndex+1);
		if((g_frames[g_frameIndex].finfo=fopen(filename,"rb"))!=NULL)
		{
			loadinfofile(g_frames[g_frameIndex].finfo, g_frameIndex, 0);
			fclose(g_frames[g_frameIndex].finfo);
		}
		/* �ָ�*/
		if(g_frames[g_frameIndex].SegFlag == 0||g_frames[g_frameIndex].AutoSegLabel <= g_segparamnumregion/4)
		{
			if(g_inputfileformat!=INPIMG)
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
					yuv2rgb(g_yuvorginput.y, g_yuvorginput.v, g_yuvorginput.u, g_rgborginput, g_frameInWidth, g_frameInHeight);
				}
				else
				{
					fread(g_yuvinput.y,size,1,g_fileInpYUV);
					fread(g_yuvinput.u,size>>2,1,g_fileInpYUV);
					fread(g_yuvinput.v,size>>2,1,g_fileInpYUV);
				}

				yuv2rgb(g_yuvinput.y, g_yuvinput.v, g_yuvinput.u, g_frames[g_frameIndex].rgbinput, g_frameWidth, g_frameHeight);
			}
			//SegmentImg(NULL, NULL);
			SegmentImgNoShow(g_frameIndex);
		}

		if(g_inferencemode==INFMODEPIX)
		{
#ifdef EDGEINF
			if(g_frameResizeFlag==1)
			{
				for(i=0;i<g_frameInWidth*g_frameInHeight;i++)
				{
					g_frames[g_frameIndex].SegLabels0[i] = g_frames[refframenum].SegLabels0[i];
				}
			}
			
			for(i=0;i<g_frameWidth*g_frameHeight;i++)
			{
				g_frames[g_frameIndex].SegLabels[i] = g_frames[refframenum].SegLabels[i];
			}
			
			if(g_inferenceedgewidth[g_inferencestrength-1]!=0) // ��Ե�޸�
			{
				int edge = g_inferenceedgewidth[g_inferencestrength-1];

				/* �����Ե���ֱ��,ע�Ᵽ���������*/
				if(g_frameResizeFlag==1)
				{
					ClearEdgeLabels(g_frames[g_frameIndex].SegLabels0, g_frames[g_frameIndex].OrgLabels0, g_frameInWidth, g_frameInHeight, edge);
					LabelResize(g_frames[g_frameIndex].SegLabels0,g_frames[g_frameIndex].SegLabels,g_yuvorginput.y, g_yuvinput.y, g_frameInWidth,g_frameInHeight,g_frameWidth,g_frameHeight);
				}
				else
				{
					ClearEdgeLabels(g_frames[g_frameIndex].SegLabels, g_frames[g_frameIndex].OrgLabels, g_frameWidth, g_frameHeight, edge);
				}
				
				/* ��Ե�����ɢ*/
				for(i=0;i<edge;i++)
					Automenuseg();
			}
#else
		 // �����������
		if(g_frameResizeFlag==1)
			{
				//printf("inferenceframe %d %d \n", g_frameIndex, refframenum);
				for(i=0;i<g_frameInWidth*g_frameInHeight;i++)
				{
					g_frames[g_frameIndex].SegLabels0[i] = g_frames[refframenum].SegLabels0[i];
				}
			}
			
			for(i=0;i<g_frameWidth*g_frameHeight;i++)
			{
				g_frames[g_frameIndex].SegLabels[i] = g_frames[refframenum].SegLabels[i];
			}
#endif
		}
		else //if(g_inferencemode==INFMODEREG)
		{
			labelnum = g_frames[g_frameIndex].AutoSegLabel;

			/* ��ÿ����������*/
			regionarr = new SRegionInfo [labelnum+1]; // û�б��0���Զ��ָ��Ŵ�1��ʼ�����Ҫ+1
			for(i=0;i<labelnum+1;i++)
			{
				regionarr[i].cnt = regionarr[i].avgx = regionarr[i].avgy = regionarr[i].refcnt = regionarr[i].histdiff = 0;
				for(j=0;j<256;j++)
				{
					regionarr[i].refhist[j] = 0;
					regionarr[i].curhist[j] = 0;
				}
			}

			for(j=0;j<g_frameHeight;j++)
				for(i=0;i<g_frameWidth;i++)
				{
					regionarr[g_frames[g_frameIndex].OrgLabels[i+j*g_frameWidth]].avgx += i;
					regionarr[g_frames[g_frameIndex].OrgLabels[i+j*g_frameWidth]].avgy += j;
					regionarr[g_frames[g_frameIndex].OrgLabels[i+j*g_frameWidth]].cnt ++;
				}

			//printf("labelnum: %d  \n",labelnum );
			for(i=1;i<labelnum;i++) //
			{
				//printf(" %d   %d \n",i,  regionarr[i].cnt);
				//assert(regionarr[i].cnt!=0);
				// TODO avg x y���겻һ������Ӧ������
				if(regionarr[i].cnt!=0)
				{
					regionarr[i].avgx = regionarr[i].avgx / regionarr[i].cnt ;
					regionarr[i].avgy = regionarr[i].avgy / regionarr[i].cnt ;
				}
			}

			/* �ҵ��������Ķ�Ӧ�ο�֡λ�õ�ԭʼ���*/
			for(i=1;i<labelnum;i++)
			{
				if(regionarr[i].cnt!=0)
					regionarr[i].reflabel = g_frames[refframenum].OrgLabels[regionarr[i].avgx+regionarr[i].avgy*g_frameWidth];
			}

			/* ������������ֱ��ͼ*/
			#ifdef I64
			_fseeki64(g_fileInpYUV, (int64_t)refframenum*IMAGE_SIZE(g_frameInWidth,g_frameInHeight), SEEK_SET);
			#else
			fseek(g_fileInpYUV, (unsigned long)refframenum*IMAGE_SIZE(g_frameInWidth,g_frameInHeight), SEEK_SET);
			#endif
			if(g_frameResizeFlag==1)
			{
				uint8_t * tmpy1 = new uint8_t [g_frameInWidth*g_frameInHeight];
				
				fread(tmpy1,g_frameInWidth*g_frameInHeight,1,g_fileInpYUV);
				yresize(tmpy1, tmpy, g_frameInWidth, g_frameInHeight, g_frameWidth, g_frameHeight);
				delete []tmpy1;
				//yuvresize(g_yuvorginput, g_yuvinput, g_frameInWidth, g_frameInHeight, g_frameWidth, g_frameHeight);
			}
			else
			{
				//uint8_t * tmpy = new uint8_t [size];
				fread(tmpy,size,1,g_fileInpYUV);
			}
			
			for(k=1;k<labelnum;k++)
			{
				if(regionarr[k].cnt!=0)
				{
					for(j=0;j<g_frameHeight;j++)
						for(i=0;i<g_frameWidth;i++)
						{
							if(g_frames[refframenum].OrgLabels[i+j*g_frameWidth]==regionarr[k].reflabel)
							{
								regionarr[k].refhist[tmpy[i+j*g_frameWidth]]++;
								regionarr[k].refcnt ++;
							}
							if(g_frames[g_frameIndex].OrgLabels[i+j*g_frameWidth]==k)
							{
								regionarr[k].curhist[g_yuvinput.y[i+j*g_frameWidth]]++;
							}
						}
				}
			}

			/* �������ֱ��ͼ��С����ֵ���ο�֡λ�÷ָ��Ÿ�����ǰ֡����*/
			// ע��ֱ��ͼͳһ��
			for(k=1;k<labelnum;k++)
			{
				if(regionarr[k].cnt!=0)
				{
					for(i=0;i<256;i++)
					{
						
						regionarr[k].curhist[i] = regionarr[k].curhist[i] * regionarr[k].refcnt / regionarr[k].cnt;
						regionarr[k].histdiff += abs(regionarr[k].curhist[i]-regionarr[k].refhist[i]);
					}
					if(regionarr[k].histdiff<g_regionhistdiffthreshold[g_inferencestrength-1])
					{
						for(j=0;j<g_frameHeight;j++)
							for(i=0;i<g_frameWidth;i++)
							{
								if(g_frames[g_frameIndex].OrgLabels[i+j*g_frameWidth]==k)
								{
									g_frames[g_frameIndex].SegLabels[i+j*g_frameWidth] = g_frames[refframenum].SegLabels[regionarr[k].avgx+regionarr[k].avgy*g_frameWidth];
								}
							}
						
						if(g_frameResizeFlag==1)
						{
							int ii,jj;
							for(j=0;j<g_frameInHeight;j++)
								for(i=0;i<g_frameInWidth;i++)
								{
									if(g_frames[g_frameIndex].OrgLabels0[i+j*g_frameInWidth]==k)
									{
										ii = regionarr[k].avgx*g_frameInWidth/g_frameWidth;
										jj = regionarr[k].avgy*g_frameInHeight/g_frameHeight;
										g_frames[g_frameIndex].SegLabels0[i+j*g_frameInWidth] = g_frames[refframenum].SegLabels0[ii+jj*g_frameInWidth];
									}
								}
						}
					}
				}
			}
			/* �������*/
			delete [] regionarr;

			// ����벻һ�µĵط������Ϊԭ��label			
			if(g_frameResizeFlag==1)
			{
				for(j=0;j<g_frameInHeight;j++)
					for(i=0;i<g_frameInWidth;i++)
					{
						if(g_frames[g_frameIndex].SegLabels0[i+j*g_frameInWidth] != g_frames[refframenum].SegLabels0[i+j*g_frameInWidth])
						{
							g_frames[g_frameIndex].SegLabels0[i+j*g_frameInWidth] = g_frames[g_frameIndex].OrgLabels0[i+j*g_frameInWidth];
							diffflags[i+j*g_frameInWidth] = 1;
						}
						else
							diffflags[i+j*g_frameInWidth] = 0;
					}
				AutoRegionSeg(g_frames[g_frameIndex].SegLabels0, g_frames[g_frameIndex].OrgLabels0, g_rgborginput, g_frameInWidth, g_frameInHeight, g_frames[g_frameIndex].AutoSegLabel);
				LabelResize(g_frames[g_frameIndex].SegLabels0,g_frames[g_frameIndex].SegLabels,g_yuvorginput.y, g_yuvinput.y, g_frameInWidth,g_frameInHeight,g_frameWidth,g_frameHeight);
			}
			else
			{	
				for(j=0;j<g_frameHeight;j++)
					for(i=0;i<g_frameWidth;i++)
					{
						if(g_frames[g_frameIndex].SegLabels[i+j*g_frameWidth] != g_frames[refframenum].SegLabels[i+j*g_frameWidth])
						{
							g_frames[g_frameIndex].SegLabels[i+j*g_frameWidth] = g_frames[g_frameIndex].OrgLabels[i+j*g_frameWidth];
							diffflags[i+j*g_frameWidth] = 1;
						}
						else
							diffflags[i+j*g_frameWidth] = 0;
					}
				AutoRegionSeg(g_frames[g_frameIndex].SegLabels, g_frames[g_frameIndex].OrgLabels, g_rgbinput, g_frameWidth, g_frameHeight, g_frames[g_frameIndex].AutoSegLabel);
			}

		}

		/* ȫ�������ϣ�����Ӧ����ֵ���ֵ*/
		while(g_frames[g_frameIndex].LayerVisibleNum<g_frames[refframenum].LayerVisibleNum)
		{
			g_frames[g_frameIndex].LayerVisibleNum++;
			
			if(g_frames[g_frameIndex].LayerVisibleNum>LAYERBUTTONNUMMAX)
				g_frames[g_frameIndex].LayerVisibleNum = LAYERBUTTONNUMMAX;
			/*else
			{
				gtk_widget_show ((GtkWidget *)layerbutton.layer[g_frames[g_frameIndex].LayerVisibleNum-1]); 
				gtk_widget_show ((GtkWidget *)layerbutton.layeredit[g_frames[g_frameIndex].LayerVisibleNum-1]); 
				gtk_widget_show ((GtkWidget *)layerbutton.layerlabel[g_frames[g_frameIndex].LayerVisibleNum-1]); 
			}*/
		}

		
		setflag = g_frames[g_frameIndex].framedepth.setflag = g_frames[refframenum].framedepth.setflag;
		depthmin = g_frames[g_frameIndex].framedepth.depthmin = g_frames[refframenum].framedepth.depthmin;
		depthmax = g_frames[g_frameIndex].framedepth.depthmax = g_frames[refframenum].framedepth.depthmax;
		depthmode = g_frames[g_frameIndex].framedepth.depthmode = g_frames[refframenum].framedepth.depthmode;

		if(setflag)
		{
			if(g_frameResizeFlag==1)
			{
				set_depth_value(g_frameIndex, 0, depthmin, depthmax, depthmode,1);
				yresize(g_SegDepths0,g_SegDepths,g_frameInWidth,g_frameInHeight,g_frameWidth,g_frameHeight);
			}
			else
				set_depth_value(g_frameIndex, 0, depthmin, depthmax, depthmode,0);
		}
		
		for(i=1;i<=g_frames[refframenum].LayerVisibleNum;i++)
		{
			lb = g_frameWidth*g_frameHeight + (1<<i);
			setflag = g_frames[g_frameIndex].layerdepth[i].setflag = g_frames[refframenum].layerdepth[i].setflag;
			depthmin = g_frames[g_frameIndex].layerdepth[i].depthmin = g_frames[refframenum].layerdepth[i].depthmin;
			depthmax = g_frames[g_frameIndex].layerdepth[i].depthmax = g_frames[refframenum].layerdepth[i].depthmax;
			depthmode = g_frames[g_frameIndex].layerdepth[i].depthmode = g_frames[refframenum].layerdepth[i].depthmode;
			if(setflag)
			{
				if(g_frameResizeFlag==1)
				{
					set_depth_value(g_frameIndex, lb, depthmin, depthmax, depthmode,1);
					yresize(g_SegDepths0,g_SegDepths,g_frameInWidth,g_frameInHeight,g_frameWidth,g_frameHeight);
				}
				else
					set_depth_value(g_frameIndex, lb, depthmin, depthmax, depthmode,0);
			}
		}
		if(g_frames[refframenum].DepBaseFlag)
		{
			g_frames[g_frameIndex].DepBaseFlag = 1;
			for(j=0;j<g_frameBaseHeight;j++)
				for(i=0;i<g_frameBaseWidth;i++)
				{
					g_BaseDepth[i+j*g_frameBaseWidth] = g_RefBaseDepth[i+j*g_frameBaseWidth];
				}
		}

		
		// ��ȸ�ֵ
		int m,n,ii,jj;
		double weighttmp;
		double weightsum;
		double depthsum;
		double weightdep;
		int cr,cg,cb, rr,rg,rb;
		//depallset = 1;
		if(g_frameResizeFlag==1)
		{
			for(j=0;j<g_frameInHeight;j++)
				for(i=0;i<g_frameInWidth;i++)
				{
					weightsum = 0.0;
					depthsum = 0.0;
					cr = g_rgborginput[i*3+j*g_frameInWidth*3];
					cg = g_rgborginput[i*3+1+j*g_frameInWidth*3];
					cb = g_rgborginput[i*3+2+j*g_frameInWidth*3];
					for(n=-1*INFWINSIZE;n<=INFWINSIZE;n++)
						for(m=-1*INFWINSIZE;m<=INFWINSIZE;m++)
						{
							ii = i+m;
							jj = j+n;
							if(ii>=0&&ii<g_frameInWidth&&jj>=0&&jj<g_frameInHeight)
							{
								rr = g_rgborginputref[ii*3+jj*g_frameInWidth*3];
								rg = g_rgborginputref[ii*3+1+jj*g_frameInWidth*3];
								rb = g_rgborginputref[ii*3+2+jj*g_frameInWidth*3];
								weighttmp = pow(2,-0.125*(abs(cr-rr)+abs(cg-rg)+abs(cb-rb)));
								weightsum += weighttmp;
								depthsum += weighttmp*g_RefSegDepths0[ii+jj*g_frameInWidth];
							}
						}
					weightdep = depthsum/weightsum;
					if(weightdep>=255.0)
						g_SegDepths0[i+j*g_frameInWidth] = 255;
					else
						g_SegDepths0[i+j*g_frameInWidth] = (uint8_t)weightdep;
				}
			yresize(g_SegDepths0,g_SegDepths,g_frameInWidth,g_frameInHeight,g_frameWidth,g_frameHeight);
		}
		else
		{
			for(j=0;j<g_frameHeight;j++)
				for(i=0;i<g_frameWidth;i++)
				{
					weightsum = 0.0;
					depthsum = 0.0;
					cr = g_rgbinput[i*3+j*g_frameInWidth*3];
					cg = g_rgbinput[i*3+1+j*g_frameInWidth*3];
					cb = g_rgbinput[i*3+2+j*g_frameInWidth*3];
					for(n=-1*INFWINSIZE;n<=INFWINSIZE;n++)
						for(m=-1*INFWINSIZE;m<=INFWINSIZE;m++)
						{
							ii = i+m;
							jj = j+n;
							if(ii>=0&&ii<g_frameWidth&&jj>=0&&jj<g_frameHeight)
							{
								
								rr = g_rgbinputref[ii*3+jj*g_frameInWidth*3];
								rg = g_rgbinputref[ii*3+1+jj*g_frameInWidth*3];
								rb = g_rgbinputref[ii*3+2+jj*g_frameInWidth*3];
								weighttmp = pow(2,-0.125*(abs(cr-rr)+abs(cg-rg)+abs(cb-rb)));
								weightsum += weighttmp;
								depthsum += weighttmp*g_RefSegDepths[ii+jj*g_frameWidth];
							}
						}
					weightdep = depthsum/weightsum;
					if(weightdep>=255.0)
						g_SegDepths[i+j*g_frameWidth] = 255;
					else
						g_SegDepths[i+j*g_frameWidth] = (uint8_t)weightdep;
				}
		}

		// �������
		if(g_inferencemode==INFMODEREG)
		{
			if(g_frameResizeFlag==1)
			{
				for(j=0;j<g_frameInHeight;j++)
					for(i=0;i<g_frameInWidth;i++)
					{
						if(diffflags[i+j*g_frameInWidth] == 0)
						{
							g_SegDepths0[i+j*g_frameInWidth] = g_RefSegDepths0[i+j*g_frameInWidth];
						}
					}
				yresize(g_SegDepths0,g_SegDepths,g_frameInWidth,g_frameInHeight,g_frameWidth,g_frameHeight);
			}
			else
			{	
				for(j=0;j<g_frameHeight;j++)
					for(i=0;i<g_frameWidth;i++)
					{
						if(diffflags[i+j*g_frameWidth] == 0)
						{
							g_SegDepths[i+j*g_frameWidth] = g_RefSegDepths[i+j*g_frameWidth];
						}
					}
			}
		}
		else if(g_inferenceedgewidth[g_inferencestrength-1]==0) // ���ؼ�����
		{
			if(g_frameResizeFlag==1)
			{
				for(j=0;j<g_frameInHeight;j++)
					for(i=0;i<g_frameInWidth;i++)
					{
						g_SegDepths0[i+j*g_frameInWidth] = g_RefSegDepths0[i+j*g_frameInWidth];
					}
				yresize(g_SegDepths0,g_SegDepths,g_frameInWidth,g_frameInHeight,g_frameWidth,g_frameHeight);
			}
			else
			{	
				for(j=0;j<g_frameHeight;j++)
					for(i=0;i<g_frameWidth;i++)
					{
						g_SegDepths[i+j*g_frameWidth] = g_RefSegDepths[i+j*g_frameWidth];
					}
			}
		}

		g_frames[g_frameIndex].LayerEditNum = g_frames[refframenum].LayerEditNum;
		SegLabelMask = g_frames[g_frameIndex].SegLabelMask = g_frames[refframenum].SegLabelMask;
		g_frames[g_frameIndex].LayerVisibleNum = g_frames[refframenum].LayerVisibleNum;
		
		/* ȫ����ʾ*/ // TODO ������ʾ
		//ShowSegandDep(SegLabelMask);

		/* �洢��Ϣ���ļ�*/
		if(g_keyframeflag)
			sprintf(filename,"./%s/info/%d_infosnap_%d.dat",cfgparams.projectname,g_sectioncur,g_keyframenumarr[g_frameIndex]+1);
		else
			sprintf(filename,"./%s/info/%d_infosnap_%d.dat",cfgparams.projectname,g_sectioncur,g_frameIndex+1);
		if((g_frames[g_frameIndex].finfo=fopen(filename,"wb"))!=NULL)
		{
			// save info file
			saveinfofile(g_frames[g_frameIndex].finfo, g_frameIndex);
			fclose(g_frames[g_frameIndex].finfo);
			//printf("save info file %s\n", filename);
		}

		delete [] tmpy;
		
	}
	else
	{
		if(g_frameMaxNum==0)
		{
			//messagebox("Please open the input file first.");
		}
		else if(g_frames[refframenum].SegFlag == 0)
			messagebox(MSGBOX_SRFF);
	}
	delete [] diffflags;
	
	return ;
}


