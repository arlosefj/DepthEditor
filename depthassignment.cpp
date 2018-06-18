/* OPENCV */
#include "cv.h"   
#include "highgui.h"   

#include "common.h"
#include "project.h"

void Drawdepth(int * labels, int label)
{
	int i,j;
	
	
	for(j=0;j<g_frameHeight;j++)
		for(i=0;i<g_frameWidth;i++)
		{
			if(labels[i+j*g_frameWidth]==label)
				g_depthdisplay[i+j*g_frameWidth] = g_frames[g_frameIndex].SegDepths[i+j*g_frameWidth];
			else
				g_depthdisplay[i+j*g_frameWidth] = 0;
		}

	if(g_showalldepflag==1)
	{
		for(j=0;j<g_frameHeight;j++)
			for(i=0;i<g_frameWidth;i++)
			{
				g_depthdisplay[i+j*g_frameWidth] = g_frames[g_frameIndex].SegDepths[i+j*g_frameWidth];
			}
	}

	// 深度区
	gdk_draw_gray_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
          IMGWIDTHMAX, IMGHEIGHTMAX, g_frameWidth, g_frameHeight,
          GDK_RGB_DITHER_MAX, g_depthdisplay, g_frameWidth);


	return ;

}

int distancexy(int x0, int y0, int x1, int y1)
{
	return (int)sqrt((double)((x0-x1)*(x0-x1)+(y0-y1)*(y0-y1)));
	//return ((x0-x1)*(x0-x1)+(y0-y1)*(y0-y1));
}

void set_depth_value(int idx, int lb, int min, int max, int mode, int flag)
{
	int i, j;
	int maxj=0, minj=0, midj, maxi=0, mini=0,midi,maxij, minij, midij, tmpij, firstflag=0;
	int centerx, centery;
	int sumx = 0;
	int sumy = 0;
	int cnt=0;
	int maxdistance = 0;
	int distance;
	int offset, index;

	int width, height;
	int * SegLabels;
	uint8_t *SegDepths;
	int * tmpbaselabel = NULL;

	if(flag==0)
	{
		width = g_frameWidth;
		height = g_frameHeight;
		SegLabels = g_frames[idx].SegLabels;
		SegDepths = g_frames[idx].SegDepths;
	}
	else if(flag==1)
	{
		width = g_frameInWidth;
		height = g_frameInHeight;
		SegLabels = g_frames[idx].SegLabels0;
		SegDepths = g_frames[idx].SegDepths0;
	}
	else
	{
		tmpbaselabel = new int[g_frameBaseWidth*g_frameBaseHeight];
		width = g_frameBaseWidth;
		height = g_frameBaseHeight;
		SegLabels = tmpbaselabel;
		SegDepths = g_BaseDepth;
	}

	if(lb!=0)
	{
		offset = lb - g_frameWidth*g_frameHeight;

		for(i=0;i<=g_frames[idx].LayerVisibleNum;i++)
		{
			if((offset&(0x1<<i))!=0)
			{
				index = i;
				break;
			}
		}
	}

	g_frames[idx].DepFlag = 1;

	switch(mode)
	{
		case 0: // background
			for(i=0;i<width*height;i++)
			{
				if(SegLabels[i]==lb||lb==0)
					SegDepths[i]= 0;
			}
			break;
		case 1: // planar
			for(i=0;i<width*height;i++)
			{
				if(SegLabels[i]==lb||lb==0)
					SegDepths[i]=(min+max)>>1;
			}
			break;
		case 2: // spherical in to out
			
			/* center */
			for(j=0;j<height;j++)
			{
				for(i=0;i<width;i++)
				{
					if(SegLabels[i+j*width]==lb||lb==0)
					{
						sumx += i;
						sumy += j;
						cnt++; 
					}
				}
			}
			//assert(cnt!=0); // 应该不会为0
			if(cnt!=0)
			{
				centerx = sumx/cnt;
				centery = sumy/cnt;
			}
			else
			{
				break;
			}
			/* maxdistance */
			for(j=0;j<height;j++)
			{
				for(i=0;i<width;i++)
				{
					if(SegLabels[i+j*width]==lb||lb==0)
					{
						distance = distancexy(centerx,centery,i,j);
						if(distance>maxdistance)
							maxdistance = distance;
					}
				}
			}
			/* depth */
			for(j=0;j<height;j++)
			{
				for(i=0;i<width;i++)
				{
					if(SegLabels[i+j*width]==lb||lb==0)
					{
						distance = distancexy(centerx,centery,i,j); // 与上面重复计算，可优化TODO
						SegDepths[i+j*width] = (uint8_t)(max - (distance)*(max-min)/maxdistance);
					}
				}
			}
			break;
		case 3: // spherical out to in
			/* center */
			for(j=0;j<height;j++)
			{
				for(i=0;i<width;i++)
				{
					if(SegLabels[i+j*width]==lb||lb==0)
					{
						sumx += i;
						sumy += j;
						cnt++; 
					}
				}
			}
			//assert(cnt!=0); // 应该不会为0
			if(cnt!=0)
			{
				centerx = sumx/cnt;
				centery = sumy/cnt;
			}
			else
			{
				break;
			}
			/* maxdistance */
			for(j=0;j<height;j++)
			{
				for(i=0;i<width;i++)
				{
					if(SegLabels[i+j*width]==lb||lb==0)
					{
						distance = distancexy(centerx,centery,i,j);
						if(distance>maxdistance)
							maxdistance = distance;
					}
				}
			}
			/* depth */
			for(j=0;j<height;j++)
			{
				for(i=0;i<width;i++)
				{
					if(SegLabels[i+j*width]==lb||lb==0)
					{
						distance = distancexy(centerx,centery,i,j); // 与上面重复计算，可优化TODO
						SegDepths[i+j*width] = (uint8_t)(min + (distance)*(max-min)/maxdistance);
					}
				}
			}
			break;
		case 4: // inclined planar up to down
			for(j=0;j<height;j++)
			{
				for(i=0;i<width;i++)
				{
					if(SegLabels[i+j*width]==lb||lb==0)
					{
						if(firstflag==0)
						{
							minj = j;
							firstflag = 1;
						}
						maxj = j;
					}
				}
			}
			if(minj!=maxj)
			{
				for(j=minj;j<=maxj;j++)
				{
					for(i=0;i<width;i++)
					{
						
						if(SegLabels[i+j*width]==lb||lb==0)
						{
							SegDepths[i+j*width] = (uint8_t)(min + (j-minj)*(max-min)/(maxj-minj));
						}
					}
				}
			}
			else
			{
				for(i=0;i<width;i++)
					{
						
						if(SegLabels[i+minj*width]==lb||lb==0)
						{
							SegDepths[i+minj*width] = (uint8_t)((min+max)>>1);
						}
					}
			}
			break;
		case 5: // inclined planar down to up 
			for(j=0;j<height;j++)
			{
				for(i=0;i<width;i++)
				{
					if(SegLabels[i+j*width]==lb||lb==0)
					{
						if(firstflag==0)
						{
							minj = j;
							firstflag = 1;
						}
						maxj = j;
					}
				}
			}
			if(minj!=maxj)
			{
				for(j=minj;j<=maxj;j++)
				{
					for(i=0;i<width;i++)
					{
						
						if(SegLabels[i+j*width]==lb||lb==0)
						{
							SegDepths[i+j*width] = (uint8_t)(max - (j-minj)*(max-min)/(maxj-minj));
						}
					}
				}
			}
			else
			{
				for(i=0;i<width;i++)
					{
						
						if(SegLabels[i+minj*width]==lb||lb==0)
						{
							SegDepths[i+minj*width] = (uint8_t)((min+max)>>1);
						}
					}
			}
			break;
		case 6: // inclined planar right to left
			for(i=0;i<width;i++)
			{
				for(j=0;j<height;j++)
				{
					if(SegLabels[i+j*width]==lb||lb==0)
					{
						if(firstflag==0)
						{
							mini = i;
							firstflag = 1;
						}
						maxi = i;
					}
				}
			}
			if(mini!=maxi)
			{
				for(i=mini;i<=maxi;i++)
				{
					for(j=0;j<height;j++)
					{
						
						if(SegLabels[i+j*width]==lb||lb==0)
						{
							SegDepths[i+j*width] = (uint8_t)(min + (i-mini)*(max-min)/(maxi-mini));
						}
					}
				}
			}
			else
			{
				for(j=0;j<height;j++)
					{
						
						if(SegLabels[mini+j*width]==lb||lb==0)
						{
							SegDepths[mini+j*width] = (uint8_t)((min+max)>>1);
						}
					}
			}
			break;
		case 7: // inclined planar left to right
			for(i=0;i<width;i++)
			{
				for(j=0;j<height;j++)
				{
					if(SegLabels[i+j*width]==lb||lb==0)
					{
						if(firstflag==0)
						{
							mini = i;
							firstflag = 1;
						}
						maxi = i;
					}
				}
			}
			if(mini!=maxi)
			{
				for(i=mini;i<=maxi;i++)
				{
					for(j=0;j<height;j++)
					{
						
						if(SegLabels[i+j*width]==lb||lb==0)
						{
							SegDepths[i+j*width] = (uint8_t)(max - (i-mini)*(max-min)/(maxi-mini));
						}
					}
				}
			}
			else
			{
				for(j=0;j<height;j++)
					{
						
						if(SegLabels[mini+j*width]==lb||lb==0)
						{
							SegDepths[mini+j*width] = (uint8_t)((min+max)>>1);
						}
					}
			}
			break;
		case 8: // inclined planar center to up down
			for(j=0;j<height;j++)
			{
				for(i=0;i<width;i++)
				{
					if(SegLabels[i+j*width]==lb||lb==0)
					{
						if(firstflag==0)
						{
							minj = j;
							firstflag = 1;
						}
						maxj = j;
					}
				}
			}
			midj = (minj+maxj)>>1;
			if(minj!=midj)
			{
				for(j=minj;j<=midj;j++)
				{
					for(i=0;i<width;i++)
					{
						
						if(SegLabels[i+j*width]==lb||lb==0)
						{
							SegDepths[i+j*width] = (uint8_t)(min + (j-minj)*(max-min)/(midj-minj));
						}
					}
				}
			}
			else
			{
				for(i=0;i<width;i++)
					{
						
						if(SegLabels[i+minj*width]==lb||lb==0)
						{
							SegDepths[i+minj*width] = (uint8_t)((min+max)>>1);
						}
					}
			}
			if(maxj!=midj)
			{
				for(j=midj;j<=maxj;j++)
				{
					for(i=0;i<width;i++)
					{
						
						if(SegLabels[i+j*width]==lb||lb==0)
						{
							SegDepths[i+j*width] = (uint8_t)(max - (j-midj)*(max-min)/(maxj-midj));
						}
					}
				}
			}
			else
			{
				for(i=0;i<width;i++)
					{
						
						if(SegLabels[i+midj*width]==lb||lb==0)
						{
							SegDepths[i+midj*width] = (uint8_t)((min+max)>>1);
						}
					}
			}
			break;
		case 9: // inclined planar up down to center
			for(j=0;j<height;j++)
			{
				for(i=0;i<width;i++)
				{
					if(SegLabels[i+j*width]==lb||lb==0)
					{
						if(firstflag==0)
						{
							minj = j;
							firstflag = 1;
						}
						maxj = j;
					}
				}
			}
			midj = (minj+maxj)>>1;
			if(minj!=midj)
			{
				for(j=minj;j<=midj;j++)
				{
					for(i=0;i<width;i++)
					{
						
						if(SegLabels[i+j*width]==lb||lb==0)
						{
							SegDepths[i+j*width] = (uint8_t)(max - (j-minj)*(max-min)/(midj-minj));
						}
					}
				}
			}
			else
			{
				for(i=0;i<width;i++)
					{
						
						if(SegLabels[i+minj*width]==lb||lb==0)
						{
							SegDepths[i+minj*width] = (uint8_t)((min+max)>>1);
						}
					}
			}
			if(maxj!=midj)
			{
				for(j=midj;j<=maxj;j++)
				{
					for(i=0;i<width;i++)
					{
						
						if(SegLabels[i+j*width]==lb||lb==0)
						{
							SegDepths[i+j*width] = (uint8_t)(min + (j-midj)*(max-min)/(maxj-midj));
						}
					}
				}
			}
			else
			{
				for(i=0;i<width;i++)
					{
						
						if(SegLabels[i+midj*width]==lb||lb==0)
						{
							SegDepths[i+midj*width] = (uint8_t)((min+max)>>1);
						}
					}
			}
			break;
		case 10: // inclined planar center to left right 
			for(i=0;i<width;i++)
			{
				for(j=0;j<height;j++)
				{
					if(SegLabels[i+j*width]==lb||lb==0)
					{
						if(firstflag==0)
						{
							mini = i;
							firstflag = 1;
						}
						maxi = i;
					}
				}
			}
			midi = (maxi+mini)>>1;
			
			if(mini!=midi)
			{
				for(i=mini;i<=midi;i++)
				{
					for(j=0;j<height;j++)
					{
						
						if(SegLabels[i+j*width]==lb||lb==0)
						{
							SegDepths[i+j*width] = (uint8_t)(min + (i-mini)*(max-min)/(midi-mini));
						}
					}
				}
			}
			else
			{
				for(j=0;j<height;j++)
					{
						
						if(SegLabels[mini+j*width]==lb||lb==0)
						{
							SegDepths[mini+j*width] = (uint8_t)((min+max)>>1);
						}
					}
			}
			if(maxi!=midi)
			{
				for(i=midi;i<=maxi;i++)
				{
					for(j=0;j<height;j++)
					{
						
						if(SegLabels[i+j*width]==lb||lb==0)
						{
							SegDepths[i+j*width] = (uint8_t)(max - (i-midi)*(max-min)/(maxi-midi));
						}
					}
				}
			}
			else
			{
				for(j=0;j<height;j++)
					{
						
						if(SegLabels[midi+j*width]==lb||lb==0)
						{
							SegDepths[midi+j*width] = (uint8_t)((min+max)>>1);
						}
					}
			}
			break;
		case 11: // inclined planar left right to center
			for(i=0;i<width;i++)
			{
				for(j=0;j<height;j++)
				{
					if(SegLabels[i+j*width]==lb||lb==0)
					{
						if(firstflag==0)
						{
							mini = i;
							firstflag = 1;
						}
						maxi = i;
					}
				}
			}
			midi = (maxi+mini)>>1;
			
			if(mini!=midi)
			{
				for(i=mini;i<=midi;i++)
				{
					for(j=0;j<height;j++)
					{
						
						if(SegLabels[i+j*width]==lb||lb==0)
						{
							SegDepths[i+j*width] = (uint8_t)(max - (i-mini)*(max-min)/(midi-mini));
						}
					}
				}
			}
			else
			{
				for(j=0;j<height;j++)
					{
						
						if(SegLabels[mini+j*width]==lb||lb==0)
						{
							SegDepths[mini+j*width] = (uint8_t)((min+max)>>1);
						}
					}
			}
			if(maxi!=midi)
			{
				for(i=midi;i<=maxi;i++)
				{
					for(j=0;j<height;j++)
					{
						
						if(SegLabels[i+j*width]==lb||lb==0)
						{
							SegDepths[i+j*width] = (uint8_t)(min + (i-midi)*(max-min)/(maxi-midi));
						}
					}
				}
			}
			else
			{
				for(j=0;j<height;j++)
					{
						
						if(SegLabels[midi+j*width]==lb||lb==0)
						{
							SegDepths[midi+j*width] = (uint8_t)((min+max)>>1);
						}
					}
			}
			break;
		case 12: // inclined planar upleft to downright
			minij = width+height;
			maxij = 0;
			for(i=0;i<width;i++)
			{
				for(j=0;j<height;j++)
				{
					if(SegLabels[i+j*width]==lb||lb==0)
					{
						tmpij = i+j;
						if(tmpij>maxij) maxij = tmpij;
						if(tmpij<minij)	minij = tmpij;
					}
				}
			}
			if(minij!=maxij)
			{
				for(i=0;i<width;i++)
				{
					for(j=0;j<height;j++)
					{
						
						if(SegLabels[i+j*width]==lb||lb==0)
						{
							SegDepths[i+j*width] = (uint8_t)(max - ((i+j)-minij)*(max-min)/(maxij-minij));
						}
					}
				}
			}
			else
			{
				for(i=0;i<width;i++)
				for(j=0;j<height;j++)
					{
						
						if(SegLabels[i+j*width]==lb||lb==0)
						{
							SegDepths[i+j*width] = (uint8_t)((min+max)>>1);
						}
					}
			}
			break;
		case 13: // inclined planar downright to upleft
			minij = width+height;
			maxij = 0;
			for(i=0;i<width;i++)
			{
				for(j=0;j<height;j++)
				{
					if(SegLabels[i+j*width]==lb||lb==0)
					{
						tmpij = i+j;
						if(tmpij>maxij) maxij = tmpij;
						if(tmpij<minij)	minij = tmpij;
					}
				}
			}
			if(minij!=maxij)
			{
				for(i=0;i<width;i++)
				{
					for(j=0;j<height;j++)
					{
						
						if(SegLabels[i+j*width]==lb||lb==0)
						{
							SegDepths[i+j*width] = (uint8_t)(min + ((i+j)-minij)*(max-min)/(maxij-minij));
						}
					}
				}
			}
			else
			{
				for(i=0;i<width;i++)
				for(j=0;j<height;j++)
					{
						
						if(SegLabels[i+j*width]==lb||lb==0)
						{
							SegDepths[i+j*width] = (uint8_t)((min+max)>>1);
						}
					}
			}
			break;
		case 14: // inclined planar upright to downleft
			minij = width+height;
			maxij = 0;
			for(i=0;i<width;i++)
			{
				for(j=0;j<height;j++)
				{
					if(SegLabels[i+j*width]==lb||lb==0)
					{
						tmpij = width-i+j;
						if(tmpij>maxij) maxij = tmpij;
						if(tmpij<minij)	minij = tmpij;
					}
				}
			}
			if(minij!=maxij)
			{
				for(i=0;i<width;i++)
				{
					for(j=0;j<height;j++)
					{
						
						if(SegLabels[i+j*width]==lb||lb==0)
						{
							SegDepths[i+j*width] = (uint8_t)(max - ((width-i+j)-minij)*(max-min)/(maxij-minij));
						}
					}
				}
			}
			else
			{
				for(i=0;i<width;i++)
				for(j=0;j<height;j++)
					{
						
						if(SegLabels[i+j*width]==lb||lb==0)
						{
							SegDepths[i+j*width] = (uint8_t)((min+max)>>1);
						}
					}
			}
			break;
		case 15: // inclined planar downleft to upright
			minij = width+height;
			maxij = 0;
			for(i=0;i<width;i++)
			{
				for(j=0;j<height;j++)
				{
					if(SegLabels[i+j*width]==lb||lb==0)
					{
						tmpij = width-i+j;
						if(tmpij>maxij) maxij = tmpij;
						if(tmpij<minij)	minij = tmpij;
					}
				}
			}
			if(minij!=maxij)
			{
				for(i=0;i<width;i++)
				{
					for(j=0;j<height;j++)
					{
						
						if(SegLabels[i+j*width]==lb||lb==0)
						{
							SegDepths[i+j*width] = (uint8_t)(min + ((width-i+j)-minij)*(max-min)/(maxij-minij));
						}
					}
				}
			}
			else
			{
				for(i=0;i<width;i++)
				for(j=0;j<height;j++)
					{
						
						if(SegLabels[i+j*width]==lb||lb==0)
						{
							SegDepths[i+j*width] = (uint8_t)((min+max)>>1);
						}
					}
			}
			break;
		case 16: // inclined planar upleft to downright
			minij = width+height;
			maxij = 0;
			for(i=0;i<width;i++)
			{
				for(j=0;j<height;j++)
				{
					if(SegLabels[i+j*width]==lb||lb==0)
					{
						tmpij = i+j;
						if(tmpij>maxij) maxij = tmpij;
						if(tmpij<minij)	minij = tmpij;
					}
				}
			}
			midij = (minij+maxij)>>1;
			if(minij!=midij)
			{
				for(i=0;i<width;i++)
				{
					for(j=0;j<height;j++)
					{
						
						if((SegLabels[i+j*width]==lb||lb==0)&&(i+j)<=midij)
						{
							SegDepths[i+j*width] = (uint8_t)(min + ((i+j)-minij)*(max-min)/(midij-minij));
						}
					}
				}
			}
			else
			{
				for(i=0;i<width;i++)
				for(j=0;j<height;j++)
					{
						
						if((SegLabels[i+j*width]==lb||lb==0)&&(i+j)==midij)
						{
							SegDepths[i+j*width] = (uint8_t)((min+max)>>1);
						}
					}
			}
			if(maxij!=midij)
			{
				for(i=0;i<width;i++)
				{
					for(j=0;j<height;j++)
					{
						
						if((SegLabels[i+j*width]==lb||lb==0)&&(i+j)>=midij)
						{
							SegDepths[i+j*width] = (uint8_t)(max - ((i+j)-midij)*(max-min)/(maxij-midij));
						}
					}
				}
			}
			else
			{
				for(i=0;i<width;i++)
				for(j=0;j<height;j++)
					{
						
						if((SegLabels[i+j*width]==lb||lb==0)&&(i+j)==midij)
						{
							SegDepths[i+j*width] = (uint8_t)((min+max)>>1);
						}
					}
			}
			break;
		case 17: // inclined planar upleft to downright
			minij = width+height;
			maxij = 0;
			for(i=0;i<width;i++)
			{
				for(j=0;j<height;j++)
				{
					if(SegLabels[i+j*width]==lb||lb==0)
					{
						tmpij = i+j;
						if(tmpij>maxij) maxij = tmpij;
						if(tmpij<minij)	minij = tmpij;
					}
				}
			}
			midij = (minij+maxij)>>1;
			if(minij!=midij)
			{
				for(i=0;i<width;i++)
				{
					for(j=0;j<height;j++)
					{
						
						if((SegLabels[i+j*width]==lb||lb==0)&&(i+j)<=midij)
						{
							SegDepths[i+j*width] = (uint8_t)(max - ((i+j)-minij)*(max-min)/(midij-minij));
						}
					}
				}
			}
			else
			{
				for(i=0;i<width;i++)
				for(j=0;j<height;j++)
					{
						
						if((SegLabels[i+j*width]==lb||lb==0)&&(i+j)==midij)
						{
							SegDepths[i+j*width] = (uint8_t)((min+max)>>1);
						}
					}
			}
			if(maxij!=midij)
			{
				for(i=0;i<width;i++)
				{
					for(j=0;j<height;j++)
					{
						
						if((SegLabels[i+j*width]==lb||lb==0)&&(i+j)>=midij)
						{
							SegDepths[i+j*width] = (uint8_t)(min + ((i+j)-midij)*(max-min)/(maxij-midij));
						}
					}
				}
			}
			else
			{
				for(i=0;i<width;i++)
				for(j=0;j<height;j++)
					{
						
						if((SegLabels[i+j*width]==lb||lb==0)&&(i+j)==midij)
						{
							SegDepths[i+j*width] = (uint8_t)((min+max)>>1);
						}
					}
			}
			break;
		case 18: // inclined planar upleft to downright
			minij = width+height;
			maxij = 0;
			for(i=0;i<width;i++)
			{
				for(j=0;j<height;j++)
				{
					if(SegLabels[i+j*width]==lb||lb==0)
					{
						tmpij = width-i+j;
						if(tmpij>maxij) maxij = tmpij;
						if(tmpij<minij)	minij = tmpij;
					}
				}
			}
			midij = (minij+maxij)>>1;
			if(minij!=midij)
			{
				for(i=0;i<width;i++)
				{
					for(j=0;j<height;j++)
					{
						
						if((SegLabels[i+j*width]==lb||lb==0)&&(width-i+j)<=midij)
						{
							SegDepths[i+j*width] = (uint8_t)(min + ((width-i+j)-minij)*(max-min)/(midij-minij));
						}
					}
				}
			}
			else
			{
				for(i=0;i<width;i++)
				for(j=0;j<height;j++)
					{
						
						if((SegLabels[i+j*width]==lb||lb==0)&&(width-i+j)==midij)
						{
							SegDepths[i+j*width] = (uint8_t)((min+max)>>1);
						}
					}
			}
			if(maxij!=midij)
			{
				for(i=0;i<width;i++)
				{
					for(j=0;j<height;j++)
					{
						
						if((SegLabels[i+j*width]==lb||lb==0)&&(width-i+j)>=midij)
						{
							SegDepths[i+j*width] = (uint8_t)(max - ((width-i+j)-midij)*(max-min)/(maxij-midij));
						}
					}
				}
			}
			else
			{
				for(i=0;i<width;i++)
				for(j=0;j<height;j++)
					{
						
						if((SegLabels[i+j*width]==lb||lb==0)&&(width-i+j)==midij)
						{
							SegDepths[i+j*width] = (uint8_t)((min+max)>>1);
						}
					}
			}
			break;
		case 19: // inclined planar upleft to downright
			minij = width+height;
			maxij = 0;
			for(i=0;i<width;i++)
			{
				for(j=0;j<height;j++)
				{
					if(SegLabels[i+j*width]==lb||lb==0)
					{
						tmpij = width-i+j;
						if(tmpij>maxij) maxij = tmpij;
						if(tmpij<minij)	minij = tmpij;
					}
				}
			}
			midij = (minij+maxij)>>1;
			if(minij!=midij)
			{
				for(i=0;i<width;i++)
				{
					for(j=0;j<height;j++)
					{
						
						if((SegLabels[i+j*width]==lb||lb==0)&&(width-i+j)<=midij)
						{
							SegDepths[i+j*width] = (uint8_t)(max - ((width-i+j)-minij)*(max-min)/(midij-minij));
						}
					}
				}
			}
			else
			{
				for(i=0;i<width;i++)
				for(j=0;j<height;j++)
					{
						
						if((SegLabels[i+j*width]==lb||lb==0)&&(width-i+j)==midij)
						{
							SegDepths[i+j*width] = (uint8_t)((min+max)>>1);
						}
					}
			}
			if(maxij!=midij)
			{
				for(i=0;i<width;i++)
				{
					for(j=0;j<height;j++)
					{
						
						if((SegLabels[i+j*width]==lb||lb==0)&&(width-i+j)>=midij)
						{
							SegDepths[i+j*width] = (uint8_t)(min + ((width-i+j)-midij)*(max-min)/(maxij-midij));
						}
					}
				}
			}
			else
			{
				for(i=0;i<width;i++)
				for(j=0;j<height;j++)
					{
						
						if((SegLabels[i+j*width]==lb||lb==0)&&(width-i+j)==midij)
						{
							SegDepths[i+j*width] = (uint8_t)((min+max)>>1);
						}
					}
			}
			break;
		default: // 其他情况待实现TODO
			for(i=0;i<width*height;i++)
			{
				if(SegLabels[i]==lb||lb==0)
					SegDepths[i]=(uint8_t)((min+max)>>1);
			}
			break;
	}

	/* 保存该层次的深度信息*/
	if(lb!=0)
	{
		g_frames[idx].layerdepth[index].setflag = 1; // 删除层次的情况怎么做? TODO
		g_frames[idx].layerdepth[index].depthmin = min;
		g_frames[idx].layerdepth[index].depthmax = max;
		g_frames[idx].layerdepth[index].depthmode = mode;
	}
	else
	{
		g_frames[idx].framedepth.setflag =  1;
		g_frames[idx].framedepth.depthmin = min;
		g_frames[idx].framedepth.depthmax = max;
		g_frames[idx].framedepth.depthmode = mode;
	}

	if(flag==2) // base
		delete [] tmpbaselabel;
	
	return ;
}


void get_depth_default_value(int lb, int * min, int *max, int *avg)
{
	int i, j;
	int sum = 0;
	int cnt = 0; 
	int dep;
	int firstflag = 0;

	for(j=0;j<g_frameHeight;j++)
	{
		dep = j*255/g_frameHeight;
		for(i=0;i<g_frameWidth;i++)
		{
			if(g_frames[g_frameIndex].SegLabels[i+j*g_frameWidth]==lb)
			{
				if(firstflag==0)
				{
					*min = dep;
					firstflag = 1;
				}
				*max = dep;
				sum += dep;
				cnt++;
			}
		}
	}
	if(cnt!=0)
		*avg = sum/cnt;
	else
	{
		*avg = 0;
		*max = 0;
		*min = 0;
	}

	return ;
}

/* 显示多层深度情况*/
void ShowDep(int mask)
{
	int i, j, k;
	int label;
	int idx=0;
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
				//set_depth_value(label, g_frames[g_frameIndex].layerdepth[k].depthmin, g_frames[g_frameIndex].layerdepth[k].depthmax, g_frames[g_frameIndex].layerdepth[k].depthmode);
			}
		}

		for(j=0;j<g_frameHeight;j++)
		for(i=0;i<g_frameWidth;i++)
		{
			if(g_segmentedflags[i+j*g_frameWidth]==1||g_showalldepflag==1)
			{
				g_depthdisplay[i+j*g_frameWidth] = g_frames[g_frameIndex].SegDepths[i+j*g_frameWidth];
			}
			else
			{
				g_depthdisplay[i+j*g_frameWidth] = 0;
			}
		}

		// 深度区
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

void ShowEnlargeDep(int mask)
{
	int i, j, k, ii, jj;
	int label;
	int idx=0;
	int szorg = g_frameInWidth*g_frameInHeight;
	int sz = g_frameWidth*g_frameHeight;

	if(g_frameResizeFlag==0)
	{
		// TODO add depth view
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

	gdk_draw_gray_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
			          IMGWIDTHMAX-g_frameWidth, IMGHEIGHTMAX-g_frameHeight, g_frameWidth*2, g_frameHeight*2,
			          GDK_RGB_DITHER_MAX, g_enlargedisplaytmp0, g_frameWidth*2);
	
	return ;

}

void depthbaseassign(int first, int last)
{
	int i,j,k;

	char filename[256];
	uint8_t * basedep = new uint8_t[g_frameBaseWidth*g_frameBaseHeight];
	
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

		//根据当前深度计算基本深度
		if(g_frameResizeFlag==1)
			yresize(g_frames[g_frameIndex].SegDepths0, basedep, g_frameInWidth, g_frameInHeight, g_frameBaseWidth, g_frameBaseHeight);
		else
			yresize(g_frames[g_frameIndex].SegDepths, basedep, g_frameWidth, g_frameHeight, g_frameBaseWidth, g_frameBaseHeight);

		//printf("first %d last %d\n", first, last);
		for(k=first;k<=last;k++)
		{
			g_frameIndex = k;
			
			// 载入处理帧
			if(g_keyframeflag)
				sprintf(filename,"./%s/info/%d_infosnap_%d.dat",cfgparams.projectname,g_sectioncur,g_keyframenumarr[g_frameIndex]+1);
			else
				sprintf(filename,"./%s/info/%d_infosnap_%d.dat",cfgparams.projectname,g_sectioncur,g_frameIndex+1);
			if((g_frames[k].finfo=fopen(filename,"rb"))!=NULL)
			{
				loadinfofile(g_frames[k].finfo, k, 0);
				fclose(g_frames[k].finfo);
				printf("\rDEPTH    INFO --- Depth Base Assignment load  information frame index %d from %d to %d %.2lf%% ", k+1, first+1, last+1,((double)(k-first+1))/(last-first+1)*100);
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
				printf("\rDEPTH    INFO --- Depth Base Assignment automatic segment frame index %d from %d to %d %.2lf%% ", k+1, first+1, last+1,((double)(k-first+1))/(last-first+1)*100);
				//}
			}
			
			
			g_frames[g_frameIndex].DepBaseFlag = 1;
			for(i=0;i<g_frameBaseWidth*g_frameBaseHeight;i++)
				g_BaseDepth[i] = basedep[i];

			// 关键帧模式下保存基本深度信息
			if(g_keyframeflag)
				sprintf(filename,"./%s/info/%d_basedep_%d.dat",cfgparams.projectname,g_sectioncur,g_keyframenumarr[g_frameIndex]+1);
			else
				sprintf(filename,"./%s/info/%d_basedep_%d.dat",cfgparams.projectname,g_sectioncur,g_frameIndex+1);
			if((g_frames[g_frameIndex].finfo=fopen(filename,"wb"))!=NULL)
			{
				// save base depth file
				fwrite(basedep, 1, g_frameBaseWidth*g_frameBaseHeight, g_frames[g_frameIndex].finfo);
				fclose(g_frames[g_frameIndex].finfo);
			}

			
			// 保存深度信息
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
		printf("\n");

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
	//#else

	
		if(g_enlargeflag==0)
		{
			for(j=0;j<g_frameBaseHeight;j++)
				for(i=0;i<g_frameBaseWidth;i++)
				{
					g_depthdisplay[i+j*g_frameWidth] = g_BaseDepth[i+j*g_frameBaseWidth];
				}
			gdk_draw_gray_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
	          IMGWIDTHMAX, IMGHEIGHTMAX, g_frameWidth, g_frameHeight,
	          GDK_RGB_DITHER_MAX, g_depthdisplay, g_frameWidth);
		}
	#endif
	}
	else
	{
		//messagebox("Please open a file first.");
	}
	delete [] basedep;

	return ;
}


void depthframemodule(int first, int last, int min, int max, int mode)
{
	int k;

	char filename[256];
	
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

		//printf("first %d last %d\n", first, last);
		for(k=first;k<=last;k++)
		{
			g_frameIndex = k;
			
			// 载入处理帧
			if(g_keyframeflag)
				sprintf(filename,"./%s/info/%d_infosnap_%d.dat",cfgparams.projectname,g_sectioncur,g_keyframenumarr[g_frameIndex]+1);
			else
				sprintf(filename,"./%s/info/%d_infosnap_%d.dat",cfgparams.projectname,g_sectioncur,g_frameIndex+1);
			if((g_frames[k].finfo=fopen(filename,"rb"))!=NULL)
			{
				loadinfofile(g_frames[k].finfo, k, 0);
				fclose(g_frames[k].finfo);
				printf("\rDEPTH    INFO --- Depth Base Generation load  information frame index %d from %d to %d %.2lf%% ", k+1, first+1, last+1,((double)(k-first+1))/(last-first+1)*100);

				set_depth_value(g_frameIndex, 0, min, max, mode,0);
				if(g_frameResizeFlag==1)
					set_depth_value(g_frameIndex, 0, min, max, mode,1);
				
				if(g_frameResizeFlag==1)
					yresize(g_frames[g_frameIndex].SegDepths0, g_BaseDepth, g_frameInWidth, g_frameInHeight, g_frameBaseWidth, g_frameBaseHeight);
				else
					yresize(g_frames[g_frameIndex].SegDepths, g_BaseDepth, g_frameWidth, g_frameHeight, g_frameBaseWidth, g_frameBaseHeight);

				if(g_keyframeflag)
					sprintf(filename,"./%s/info/%d_basedep_%d.dat",cfgparams.projectname,g_sectioncur,g_keyframenumarr[g_frameIndex]+1);
				else
					sprintf(filename,"./%s/info/%d_basedep_%d.dat",cfgparams.projectname,g_sectioncur,g_frameIndex+1);
				if((g_frames[g_frameIndex].finfo=fopen(filename,"wb"))!=NULL)
				{
					// save base depth file
					fwrite(g_BaseDepth, 1, g_frameBaseWidth*g_frameBaseHeight, g_frames[g_frameIndex].finfo);
					fclose(g_frames[g_frameIndex].finfo);
				}

				// 保存深度信息
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
			else
			{
				// 未分割
				if(g_keyframeflag==0)
				{
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
					//SegmentImg(NULL, NULL);
					SegmentImgNoShow(k);
					printf("\rDEPTH    INFO --- Depth Base Generation automatic segment frame index %d from %d to %d %.2lf%% ", k+1, first+1, last+1,((double)(k-first+1))/(last-first+1)*100);
					//}
					set_depth_value(g_frameIndex, 0, min, max, mode,0);
					if(g_frameResizeFlag==1)
						set_depth_value(g_frameIndex, 0, min, max, mode,1);
					
					if(g_frameResizeFlag==1)
						yresize(g_frames[g_frameIndex].SegDepths0, g_BaseDepth, g_frameInWidth, g_frameInHeight, g_frameBaseWidth, g_frameBaseHeight);
					else
						yresize(g_frames[g_frameIndex].SegDepths, g_BaseDepth, g_frameWidth, g_frameHeight, g_frameBaseWidth, g_frameBaseHeight);

					sprintf(filename,"./%s/info/%d_basedep_%d.dat",cfgparams.projectname,g_sectioncur,g_frameIndex+1);
					if((g_frames[g_frameIndex].finfo=fopen(filename,"wb"))!=NULL)
					{
						// save base depth file
						fwrite(g_BaseDepth, 1, g_frameBaseWidth*g_frameBaseHeight, g_frames[g_frameIndex].finfo);
						fclose(g_frames[g_frameIndex].finfo);
					}

					// 保存深度信息
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
				else
				{
					set_depth_value(g_frameIndex, 0, min, max, mode,2);
					sprintf(filename,"./%s/info/%d_basedep_%d.dat",cfgparams.projectname,g_sectioncur,g_keyframenumarr[g_frameIndex]+1);
					if((g_frames[g_frameIndex].finfo=fopen(filename,"wb"))!=NULL)
					{
						// save base depth file
						fwrite(g_BaseDepth, 1, g_frameBaseWidth*g_frameBaseHeight, g_frames[g_frameIndex].finfo);
						fclose(g_frames[g_frameIndex].finfo);
					}
					printf("\rDEPTH    INFO --- Depth Base Generation automatic module set frame index %d from %d to %d %.2lf%% ", k+1, first+1, last+1,((double)(k-first+1))/(last-first+1)*100);
				}
			}

			
		}
		printf("\n");

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

	return ;
}

void depthautoassign()
{
	int j;
	int depthmax, depthmin, depthavg;

	if(g_frameMaxNum>0&&g_frames[g_frameIndex].SegFlag==1)
	{
		if(g_frames[g_frameIndex].framedepth.setflag)
		{
			if(g_frameResizeFlag==1)
			{
				set_depth_value(g_frameIndex, 0, g_frames[g_frameIndex].framedepth.depthmin, g_frames[g_frameIndex].framedepth.depthmax, g_frames[g_frameIndex].framedepth.depthmode,1);
				yresize(g_SegDepths0,g_SegDepths,g_frameInWidth,g_frameInHeight,g_frameWidth,g_frameHeight);
			}
			else
				set_depth_value(g_frameIndex, 0, g_frames[g_frameIndex].framedepth.depthmin, g_frames[g_frameIndex].framedepth.depthmax, g_frames[g_frameIndex].framedepth.depthmode,0);
		}
	
		for(j=0;j<=g_frames[g_frameIndex].LayerVisibleNum;j++)
		{
			int lb = g_frameWidth*g_frameHeight + (1<<j);
			if(g_frames[g_frameIndex].layerdepth[j].setflag)
			{
				if(g_frameResizeFlag==1)
				{
					set_depth_value(g_frameIndex, lb, g_frames[g_frameIndex].layerdepth[j].depthmin, g_frames[g_frameIndex].layerdepth[j].depthmax, g_frames[g_frameIndex].layerdepth[j].depthmode,1);
					yresize(g_SegDepths0,g_SegDepths,g_frameInWidth,g_frameInHeight,g_frameWidth,g_frameHeight);
				}
				else
					set_depth_value(g_frameIndex, lb, g_frames[g_frameIndex].layerdepth[j].depthmin, g_frames[g_frameIndex].layerdepth[j].depthmax, g_frames[g_frameIndex].layerdepth[j].depthmode,0);
			}
			else
			{
				get_depth_default_value(lb, &depthmin, &depthmax, &depthavg);
				if(g_frameResizeFlag==1)
				{
					set_depth_value(g_frameIndex, lb, depthmin, depthmax, 1,1);
					yresize(g_SegDepths0,g_SegDepths,g_frameInWidth,g_frameInHeight,g_frameWidth,g_frameHeight);
				}
				else
					set_depth_value(g_frameIndex, lb, depthmin, depthmax, 1,0);
			}
		}

		ShowDep(g_frames[g_frameIndex].SegLabelMask);
	}
	
	return ;
}

void depthimginfe(int srcidx, int dstidx)
{
	int j;
	char filename[256];

	if(g_frameMaxNum>0)
	{
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

		// 目的帧信息载入
		if(g_keyframeflag)
			sprintf(filename,"./%s/info/%d_infosnap_%d.dat",cfgparams.projectname,g_sectioncur,g_keyframenumarr[dstidx]+1);
		else
			sprintf(filename,"./%s/info/%d_infosnap_%d.dat",cfgparams.projectname,g_sectioncur,dstidx+1);
		if((g_frames[dstidx].finfo=fopen(filename,"rb"))!=NULL)
		{
			// load info file
			loadinfofile(g_frames[dstidx].finfo, dstidx, 0);
			fclose(g_frames[dstidx].finfo);
		}
		
		if(g_frames[srcidx].SegFlag==1&&g_frames[dstidx].SegFlag==1)
		{
			
			g_frames[dstidx].framedepth.setflag=1;
			if(g_frameResizeFlag==1)
			{
				set_depth_value(dstidx, 0, g_frames[srcidx].framedepth.depthmin, g_frames[srcidx].framedepth.depthmax, g_frames[srcidx].framedepth.depthmode,1);
			}
			else
				set_depth_value(dstidx, 0, g_frames[srcidx].framedepth.depthmin, g_frames[srcidx].framedepth.depthmax, g_frames[srcidx].framedepth.depthmode,0);

			for(j=0;j<=g_frames[dstidx].LayerVisibleNum;j++)
			{
				int lb = g_frameWidth*g_frameHeight + (1<<j);
				g_frames[dstidx].layerdepth[j].setflag=1;
				if(g_frameResizeFlag==1)
				{
					set_depth_value(dstidx, lb, g_frames[srcidx].layerdepth[j].depthmin, g_frames[srcidx].layerdepth[j].depthmax, g_frames[srcidx].layerdepth[j].depthmode,1);
				}
				else
					set_depth_value(dstidx, lb, g_frames[srcidx].layerdepth[j].depthmin, g_frames[srcidx].layerdepth[j].depthmax, g_frames[srcidx].layerdepth[j].depthmode,0);
			}

			// 目的帧信息保存
			if(g_frames[dstidx].SegFlag)// &&g_frames[g_frameIndex].ChangeFlag==1) // TODO 可优化，判断是否修改
			{
				if(g_keyframeflag)
					sprintf(filename,"./%s/info/%d_infosnap_%d.dat",cfgparams.projectname,g_sectioncur,g_keyframenumarr[dstidx]+1);
				else
					sprintf(filename,"./%s/info/%d_infosnap_%d.dat",cfgparams.projectname,g_sectioncur,dstidx+1);
				if((g_frames[dstidx].finfo=fopen(filename,"wb"))!=NULL)
				{
					// save info file
					saveinfofile(g_frames[dstidx].finfo, dstidx);
					fclose(g_frames[dstidx].finfo);
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
	}
	else
	{
		//messagebox("Please open a file first.");
	}
	
	return ;
}

void depthlayerinfe(int layernum, int first, int last, int firstmin, int firstmax, int lastmin, int lastmax, int depthmode)
{
	int i;
	char filename[256];

	if(g_frameMaxNum>0)
	{
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
				
		for(i=first;i<=last;i++)
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
		
			if(g_frames[i].SegFlag==1&&layernum<=g_frames[i].LayerVisibleNum)
			{
				int lb = g_frameWidth*g_frameHeight + (1<<layernum);
				g_frames[i].layerdepth[layernum].setflag=1;
				if(last!=first)
				{
					g_frames[i].layerdepth[layernum].depthmin = (lastmin-firstmin)*(i-first)/(last-first)+firstmin;
					g_frames[i].layerdepth[layernum].depthmax = (lastmax-firstmax)*(i-first)/(last-first)+firstmax;
				}
				else
				{
					g_frames[i].layerdepth[layernum].depthmin = firstmin;
					g_frames[i].layerdepth[layernum].depthmax = firstmax;
				}
				g_frames[i].layerdepth[layernum].depthmode = depthmode;
				if(g_frameResizeFlag==1)
					set_depth_value(i, lb, g_frames[i].layerdepth[layernum].depthmin, g_frames[i].layerdepth[layernum].depthmax, g_frames[i].layerdepth[layernum].depthmode,1);
				else
					set_depth_value(i, lb, g_frames[i].layerdepth[layernum].depthmin, g_frames[i].layerdepth[layernum].depthmax, g_frames[i].layerdepth[layernum].depthmode,0);

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
	}
	else
	{
		//messagebox("Please open a file first.");
	}
	
	return ;
}

#define DISTH 100
#define DISDVI 2

typedef struct
{
	int cnt;			// 当前帧该区域像素数
	int avgx;			// 当前帧该区域中心点坐标x
	int avgy;			// 当前帧该区域中心点坐标y

	int avgr;
	int avgg;
	int avgb;
	
	int avgd;

	int matchlabel;
	
}SRegionDepInfeInfo;

static int ARG_DELTA = 9;
static double ARG_ALPHA = 0.125;


void GetRegionInfo(SRegionDepInfeInfo * region, unsigned char * inrgb, int *labels, unsigned char * indep, int width, int height, int numlabels)
{	
	int i,j;
	
	for(i=0;i<numlabels+1;i++)
	{
		region[i].cnt = 
		region[i].avgx = 
		region[i].avgy = 
		region[i].avgr = 
		region[i].avgg = 
		region[i].avgb = 
		region[i].avgd = 0;
	}

	if(indep!=NULL)
	{
		for(j=0;j<height;j++)
			for(i=0;i<width;i++)
			{
				region[labels[i+j*width]].avgx +=  i;
				region[labels[i+j*width]].avgy +=  j;
				region[labels[i+j*width]].avgr +=  inrgb[i*3+j*width*3];
				region[labels[i+j*width]].avgg +=  inrgb[i*3+1+j*width*3];
				region[labels[i+j*width]].avgb +=  inrgb[i*3+2+j*width*3];
				region[labels[i+j*width]].avgd +=  indep[i+j*width];
				region[labels[i+j*width]].cnt++;
			}

		for(i=0;i<numlabels+1;i++)
		{
			if(region[i].cnt!=0)
			{
				region[i].avgx = region[i].avgx/region[i].cnt;
				region[i].avgy = region[i].avgy/region[i].cnt;
				region[i].avgr = region[i].avgr/region[i].cnt;
				region[i].avgg = region[i].avgg/region[i].cnt;
				region[i].avgb = region[i].avgb/region[i].cnt;
				region[i].avgd = region[i].avgd/region[i].cnt;
			}
		}
	}
	else
	{
		for(j=0;j<height;j++)
			for(i=0;i<width;i++)
			{
				region[labels[i+j*width]].avgx +=  i;
				region[labels[i+j*width]].avgy +=  j;
				region[labels[i+j*width]].avgr +=  inrgb[i*3+j*width*3];
				region[labels[i+j*width]].avgg +=  inrgb[i*3+1+j*width*3];
				region[labels[i+j*width]].avgb +=  inrgb[i*3+2+j*width*3];
				region[labels[i+j*width]].cnt++;
			}
	
		for(i=0;i<numlabels+1;i++)
		{
			if(region[i].cnt!=0)
			{
				region[i].avgx = region[i].avgx/region[i].cnt;
				region[i].avgy = region[i].avgy/region[i].cnt;
				region[i].avgr = region[i].avgr/region[i].cnt;
				region[i].avgg = region[i].avgg/region[i].cnt;
				region[i].avgb = region[i].avgb/region[i].cnt;
			}
		}
	}
	
	return ;
}

int RegionMatch(SRegionDepInfeInfo * region1, SRegionDepInfeInfo * region2, unsigned char * outdep, int num1, int num2, int width, int height)
{
	int k,l;
	int diffmin;
	int diffdis;
	int diff;
	int diffsum = 0;

	for(k=0;k<num2+1;k++)
	{
		diffmin = 0x7fff;
		for(l=0;l<num1+1;l++)
		{
			diffdis = abs(region1[l].avgx-region2[k].avgx)+abs(region1[l].avgy-region2[k].avgy);
			if(diffdis<=DISTH)
			{
				diff = abs(region1[l].avgr-region2[k].avgr)
						+abs(region1[l].avgg-region2[k].avgg)
						+abs(region1[l].avgb-region2[k].avgb);
				diff = diff + diffdis/DISDVI;
				if(diff<diffmin)
				{
					region2[k].avgd = region1[l].avgd;
					region2[k].matchlabel = l;
					diffmin = diff;
				}
			}
		}
		diffsum += diffmin;
	}

	return diffsum;
}

/* 深度赋值 */
void DepthAssignment(unsigned char * InKeyRGB, unsigned char * InKeyDepth, unsigned char * InRGB, SRegionDepInfeInfo * region1, SRegionDepInfeInfo * region2, unsigned char * outdep, int * labels1, int * labels2, int width, int height, double alpha)
{
	int i,j;
	for(j=0;j<height;j++)
		for(i=0;i<width;i++)
		{
			outdep[i+j*width] = (unsigned char)region2[labels2[i+j*width]].avgd;
		}
	return ;
}

void DepthFilter(unsigned char * InKeyRGB, unsigned char * InKeyDepth, unsigned char * InRGB, SRegionDepInfeInfo * region1, SRegionDepInfeInfo * region2, unsigned char * outdep, int * labels1, int * labels2, int width, int height, int delta, double alpha)
{
	int i,j, ii, jj, m, n, im, jn;
	double weightsum;
	double depthsum;
	double weightdep;
	double weighttmp;
	unsigned char cr, cg, cb, rr, rg, rb;

	for(j=0;j<height;j++)
		for(i=0;i<width;i++)
		{
			weightsum = 0.0;
			depthsum = 0.0;
			cr = InRGB[i*3+j*width*3];
			cg = InRGB[i*3+1+j*width*3];
			cb = InRGB[i*3+2+j*width*3];
			//im = region1[region2[labels2[i+j*width]].matchlabel].avgx;
			//jn = region1[region2[labels2[i+j*width]].matchlabel].avgy;
			im = region1[region2[labels2[i+j*width]].matchlabel].avgx+i-region2[labels2[i+j*width]].avgx;
			jn = region1[region2[labels2[i+j*width]].matchlabel].avgy+j-region2[labels2[i+j*width]].avgy;
			for(n=-1*delta;n<=delta;n++)
				for(m=-1*delta;m<=delta;m++)
				{
					ii = im+m;
					jj = jn+n;
					if(ii>=0&&ii<width&&jj>=0&&jj<height)//&&region2[labels2[i+j*width]].matchlabel==labels1[ii+jj*width])
					{
						rr = InKeyRGB[ii*3+jj*width*3];
						rg = InKeyRGB[ii*3+1+jj*width*3];
						rb = InKeyRGB[ii*3+2+jj*width*3];
						weighttmp = pow(2,-1*alpha*(abs(cr-rr)+abs(cg-rg)+abs(cb-rb)));
						weightsum += weighttmp;
						depthsum += weighttmp*InKeyDepth[ii+jj*width];
					}
				}
			weightdep = depthsum/weightsum;
			if(weightdep>=255.0)
				outdep[i+j*width] = 255;
			else
				outdep[i+j*width] = (unsigned char)weightdep;
			//printf(".");
		}

	return ;

}

void DepthCombine(unsigned char * outdep, unsigned char * dep1, unsigned char * dep2, int idx, int refidx1, int refidx2, int size)
{
	int i;
	int idxdiff = refidx2 - refidx1;

	if(idxdiff<=0)
	{
		printf("DEPTH   ERROR --- DepthCombine index error: %d : %d.\n", refidx1, refidx2);
		return ;
	}
	
	for(i=0;i<size;i++)
	{
		outdep[i] = (uint8_t)((dep1[i]*(idx-refidx1) + dep2[i]*(refidx2-idx))/idxdiff);
	}
	
	return ;
}

/* 根据深度信息推理中间帧深度信息
  first last 需要推理帧序号
  ref0idx 参考帧序号, 为-1代表不参考
  ref1idx 参考帧序号, 为-1代表不参考
  mode   推理模式，0 逐帧推理 1 只对参考帧推理,目前暂不使用
  */
void depthimginfeauto(int first, int last, int ref0idx, int ref1idx, int mode)
{
	int i;
	char filename[256];
	int size = g_frameWidth*g_frameHeight;
	int biflag = 0; // 单向推理1 双向推理2

	int refidx;			
	int numlabels0;
	int numlabels1;
	int numlabels2;
	
	SRegionDepInfeInfo * region0=NULL;
	SRegionDepInfeInfo * region1=NULL;
	SRegionDepInfeInfo * region2=NULL;
	int * labels0=NULL;
	int * labels1=NULL;
	int * labels2=NULL;
	unsigned char * inrgb0=NULL;
	unsigned char * inrgb1=NULL;
	unsigned char * inrgb2=NULL;
	unsigned char * indp=NULL;
	unsigned char * outdp1=NULL;
	unsigned char * outdp2=NULL;
	unsigned char * outdp=NULL;

	if(ref0idx==-1&&ref1idx==-1)
	{
		messagebox(MSGBOX_NRFS);
		return ;
	}

	if(g_frameMaxNum>0)
	{
		/* 保存当前帧信息*/
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

		/* 载入第一个参考帧*/
		if(ref0idx!=-1)
		{
			if(g_keyframeflag)
				sprintf(filename,"./%s/info/%d_infosnap_%d.dat",cfgparams.projectname,g_sectioncur,g_keyframenumarr[ref0idx]+1);
			else
				sprintf(filename,"./%s/info/%d_infosnap_%d.dat",cfgparams.projectname,g_sectioncur,ref0idx+1);
			if((g_frames[ref0idx].finfo=fopen(filename,"rb"))!=NULL)
			{
				// load info file
				loadinfofile(g_frames[ref0idx].finfo, ref0idx, 1);
				fclose(g_frames[ref0idx].finfo);
				//printf("load info file %s\n", filename);
			}
			else
			{
				messagebox(MSGBOX_RF0IERR);
				goto LAB_LOADCURFRAME;
			}

			if(g_frames[ref0idx].SegFlag != 1)
			{
				messagebox(MSGBOX_RF0NSEG);
				goto LAB_LOADCURFRAME;
			}
			
			biflag++;
		}
		/* 载入第二个参考帧*/
		if(ref1idx!=-1)
		{
			if(g_keyframeflag)
				sprintf(filename,"./%s/info/%d_infosnap_%d.dat",cfgparams.projectname,g_sectioncur,g_keyframenumarr[ref1idx]+1);
			else
				sprintf(filename,"./%s/info/%d_infosnap_%d.dat",cfgparams.projectname,g_sectioncur,ref1idx+1);
			if((g_frames[ref1idx].finfo=fopen(filename,"rb"))!=NULL)
			{
				// load info file
				loadinfofile(g_frames[ref1idx].finfo, ref1idx, 2);
				fclose(g_frames[ref1idx].finfo);
				//printf("load info file %s\n", filename);
			}
			else
			{
				messagebox(MSGBOX_RF1IERR);
				goto LAB_LOADCURFRAME;
			}
			
			if(g_frames[ref1idx].SegFlag != 1)
			{
				messagebox(MSGBOX_RF1NSEG);
				goto LAB_LOADCURFRAME;
			}
			biflag++;
		}

		/* 深度推理*/
		for(i=first;i<=last;i++)
		{ 
			printf("\rDEPTH    INFO --- Auto Depth Inference frame index %d from %d to %d ref frame number %d %.2lf%% ", i+1, first+1, last+1, biflag, ((double)(i-first))/(last-first+1)*100);
			/* 载入待处理帧*/
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

			// 获取待推理帧信息
			if(g_frames[i].SegFlag == 0||g_frames[i].AutoSegLabel <= g_segparamnumregion/4)
			{
				if(g_inputfileformat!=INPIMG)
				{
					#ifdef I64
					_fseeki64(g_fileInpYUV, (int64_t)i*IMAGE_SIZE(g_frameInWidth,g_frameInHeight), SEEK_SET);
					#else
					fseek(g_fileInpYUV, (unsigned long)i*IMAGE_SIZE(g_frameInWidth,g_frameInHeight), SEEK_SET);
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

					yuv2rgb(g_yuvinput.y, g_yuvinput.v, g_yuvinput.u, g_frames[i].rgbinput, g_frameWidth, g_frameHeight);
				}
				//SegmentImg(NULL, NULL);
				SegmentImgNoShow(i);
			}
			else
			{
				#ifdef I64
				_fseeki64(g_fileInpYUV, (int64_t)i*IMAGE_SIZE(g_frameInWidth,g_frameInHeight), SEEK_SET);
				#else
				fseek(g_fileInpYUV, (unsigned long)i*IMAGE_SIZE(g_frameInWidth,g_frameInHeight), SEEK_SET);
				#endif
				fread(g_yuvorginput.y,(g_frameInWidth*g_frameInHeight),1,g_fileInpYUV);
				fread(g_yuvorginput.u,(g_frameInWidth*g_frameInHeight)>>2,1,g_fileInpYUV);
				fread(g_yuvorginput.v,(g_frameInWidth*g_frameInHeight)>>2,1,g_fileInpYUV);
				yuv2rgb(g_yuvorginput.y, g_yuvorginput.v, g_yuvorginput.u, g_rgborginput, g_frameInWidth, g_frameInHeight);
			}
			inrgb0 = g_rgborginput;
			labels0 = g_frames[i].OrgLabels0;
			numlabels0 = g_frames[i].AutoSegLabel;
			region0 = new SRegionDepInfeInfo[numlabels0+1];
			GetRegionInfo(region0, inrgb0, labels0, NULL, g_frameInWidth, g_frameInHeight, numlabels0);

			if(biflag==1) // 单向推理
			{
				if(ref0idx!=-1)
					refidx = ref0idx;
				else
					refidx = ref1idx;
				
				// 获取参考帧信息
				#ifdef I64
				_fseeki64(g_fileInpYUV, (int64_t)refidx*IMAGE_SIZE(g_frameInWidth,g_frameInHeight), SEEK_SET);
				#else
				fseek(g_fileInpYUV, (unsigned long)refidx*IMAGE_SIZE(g_frameInWidth,g_frameInHeight), SEEK_SET);
				#endif
				fread(g_yuvorginputref.y,(g_frameInWidth*g_frameInHeight),1,g_fileInpYUV);
				fread(g_yuvorginputref.u,(g_frameInWidth*g_frameInHeight)>>2,1,g_fileInpYUV);
				fread(g_yuvorginputref.v,(g_frameInWidth*g_frameInHeight)>>2,1,g_fileInpYUV);
				yuv2rgb(g_yuvorginputref.y, g_yuvorginputref.v, g_yuvorginputref.u, g_rgborginputref, g_frameInWidth, g_frameInHeight);
				inrgb1 = g_rgborginputref;
				labels1 = g_frames[refidx].OrgLabels0;
				numlabels1 = g_frames[refidx].AutoSegLabel;
				region1 = new SRegionDepInfeInfo[numlabels1+1];
				indp = g_frames[refidx].SegDepths0;
				GetRegionInfo(region1, inrgb1, labels1, indp, g_frameInWidth, g_frameInHeight, numlabels1);
				// 区域匹配
				outdp = g_frames[i].SegDepths0;
				RegionMatch(region1, region0, outdp, numlabels1,numlabels0, g_frameInWidth, g_frameInHeight);
				// 深度滤波
				DepthFilter(inrgb1, indp, inrgb0, region1, region0, outdp, labels1, labels0, g_frameInWidth, g_frameInHeight,ARG_DELTA, ARG_ALPHA);

				delete [] region1;
			}
			else if(biflag==2) // 双向推理
			{
				// 获取参考帧0 信息
				#ifdef I64
				_fseeki64(g_fileInpYUV, (int64_t)ref0idx*IMAGE_SIZE(g_frameInWidth,g_frameInHeight), SEEK_SET);
				#else
				fseek(g_fileInpYUV, (unsigned long)ref0idx*IMAGE_SIZE(g_frameInWidth,g_frameInHeight), SEEK_SET);
				#endif
				fread(g_yuvorginputref.y,(g_frameInWidth*g_frameInHeight),1,g_fileInpYUV);
				fread(g_yuvorginputref.u,(g_frameInWidth*g_frameInHeight)>>2,1,g_fileInpYUV);
				fread(g_yuvorginputref.v,(g_frameInWidth*g_frameInHeight)>>2,1,g_fileInpYUV);
				yuv2rgb(g_yuvorginputref.y, g_yuvorginputref.v, g_yuvorginputref.u, g_rgborginputref, g_frameInWidth, g_frameInHeight);
				inrgb1 = g_rgborginputref;
				labels1 = g_frames[ref0idx].OrgLabels0;
				numlabels1 = g_frames[ref0idx].AutoSegLabel;
				region1 = new SRegionDepInfeInfo[numlabels1+1];
				indp = g_frames[ref0idx].SegDepths0;
				GetRegionInfo(region1, inrgb1, labels1, indp, g_frameInWidth, g_frameInHeight, numlabels1);
				// 区域匹配
				outdp1 = new uint8_t[g_frameInWidth*g_frameInHeight];
				RegionMatch(region1, region0, outdp1, numlabels1,numlabels0, g_frameInWidth, g_frameInHeight);
				// 深度滤波
				DepthFilter(inrgb1, indp, inrgb0, region1, region0, outdp1, labels1, labels0, g_frameInWidth, g_frameInHeight,ARG_DELTA, ARG_ALPHA);
				
				// 获取参考帧1 信息
				#ifdef I64
				_fseeki64(g_fileInpYUV, (int64_t)ref1idx*IMAGE_SIZE(g_frameInWidth,g_frameInHeight), SEEK_SET);
				#else
				fseek(g_fileInpYUV, (unsigned long)ref1idx*IMAGE_SIZE(g_frameInWidth,g_frameInHeight), SEEK_SET);
				#endif
				fread(g_yuvorginputref.y,(g_frameInWidth*g_frameInHeight),1,g_fileInpYUV);
				fread(g_yuvorginputref.u,(g_frameInWidth*g_frameInHeight)>>2,1,g_fileInpYUV);
				fread(g_yuvorginputref.v,(g_frameInWidth*g_frameInHeight)>>2,1,g_fileInpYUV);
				yuv2rgb(g_yuvorginputref.y, g_yuvorginputref.v, g_yuvorginputref.u, g_rgborginputref, g_frameInWidth, g_frameInHeight);
				inrgb2 = g_rgborginputref;
				labels2 = g_frames[ref1idx].OrgLabels0;
				numlabels2 = g_frames[ref1idx].AutoSegLabel;
				region2 = new SRegionDepInfeInfo[numlabels2+1];
				indp = g_frames[ref1idx].SegDepths0;
				GetRegionInfo(region2, inrgb2, labels2, indp, g_frameInWidth, g_frameInHeight, numlabels2);
				// 区域匹配
				outdp2 = new uint8_t[g_frameInWidth*g_frameInHeight];
				RegionMatch(region2, region0, outdp2, numlabels2,numlabels0, g_frameInWidth, g_frameInHeight);
				// 深度滤波
				DepthFilter(inrgb2, indp, inrgb0, region2, region0, outdp2, labels2, labels0, g_frameInWidth, g_frameInHeight,ARG_DELTA, ARG_ALPHA);

				// 深度融合
				outdp = g_frames[i].SegDepths0;
				DepthCombine(outdp, outdp1, outdp2, i, ref0idx, ref1idx,g_frameInWidth*g_frameInHeight);

				
				delete [] region1;
				delete [] region2;
				delete [] outdp1;
				delete [] outdp2;
				
			}
			delete [] region0;

			/* 保存已处理帧*/
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

			/* 保存深度图像*/
			IplImage* pImg_depth; // 输入深度图
			CvSize sz;
			int j,k;
			sz.width = g_frameInWidth;
			sz.height = g_frameInHeight;
			pImg_depth = cvCreateImage(sz, IPL_DEPTH_8U, 1);
			for(j=0;j<g_frameInHeight;j++)
				for(k=0;k<g_frameInWidth;k++)
				{
					pImg_depth->imageData[k+j*pImg_depth->widthStep] = outdp[k+j*g_frameInWidth];
				}
			if(g_keyframeflag)
				sprintf(filename,"./%s/depth/%d_depthsnap_%d.png",cfgparams.projectname, g_sectioncur, g_keyframenumarr[i]+1);
			else
				sprintf(filename,"./%s/depth/%d_depthsnap_%d.png",cfgparams.projectname, g_sectioncur, i+1);
			cvSaveImage(filename, pImg_depth);
			cvReleaseImage( &pImg_depth );
			
			printf("\rDEPTH    INFO --- Auto Depth Inference frame index %d from %d to %d ref frame number %d %.2lf%% ", i+1, first+1, last+1, biflag, ((double)(i-first+1))/(last-first+1)*100);
		}
		printf("\n");

LAB_LOADCURFRAME:
		/* 载入当前帧信息*/
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
	}
	else
	{
		//messagebox("Please open a file first.");
	}
	

	return ;
}



