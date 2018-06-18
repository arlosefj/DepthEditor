#include "common.h"


void InterpImageBilinear_RowFilter(unsigned char* src, long* dst, int len, int* leftIdx, int* rightIdx, long* weight, int shift)
{
    int i;
    for(i = 0; i < len - 4; i+=4){
        *dst++ = ((1<<shift) - weight[i])*src[leftIdx[i]] + weight[i]*src[rightIdx[i]];
        *dst++ = ((1<<shift) - weight[i+1])*src[leftIdx[i+1]] + weight[i+1]*src[rightIdx[i+1]];
        *dst++ = ((1<<shift) - weight[i+2])*src[leftIdx[i+2]] + weight[i+2]*src[rightIdx[i+2]];
        *dst++ = ((1<<shift) - weight[i+3])*src[leftIdx[i+3]] + weight[i+3]*src[rightIdx[i+3]];
 
    }
    for( ; i < len; ++i){
        *dst++ = ((1<<shift) - weight[i])*src[leftIdx[i]] + weight[i]*src[rightIdx[i]];
    }
}
 
void InterpImageBilinear(unsigned char* src, unsigned char* dst, int src_width, int src_height, int dst_width, int dst_height)
{
    int i, j, k;
    int sw, sh, sstep;
    int dw, dh, dstep;
    unsigned char *sdata, *ddata;
    float horScaleRatio, verScaleRatio;
    long *rowBuf1, *rowBuf2;
    long *upLinePtr, *downLinePtr, *tempPtr;
    long *horWeight;
    int *horLeftIdx, *horRightIdx;
    int preVerUpIdx, preVerDownIdx;
    int shift = 8;
 
    sw = src_width; sh = src_height; sstep = src_width; 
    dw = dst_width; dh = dst_height; dstep = dst_width;
 
    horScaleRatio = sw / (float)(dw);
    verScaleRatio = sh / (float)(dh);
 
    rowBuf1 = new long[dw];
    rowBuf2 = new long[dw];
    horWeight = new long[dw];
    horLeftIdx = new int[dw];
    horRightIdx = new int[dw];
	sdata = new unsigned char [sw*sh];
	ddata = new unsigned char [dw*dh];

	for(k=0;k<3;k++)
	{
		for(j=0;j<sh;j++)
		{
			for(i=0;i<sw;i++)
			{
				sdata[i+j*sw] = src[i*3+k+j*sw*3];
			}
		}
 
	//col interpolation
	    for(i = 0; i < dw; i++){
	        float pos = (i + 0.5f) * horScaleRatio;
	        horLeftIdx[i] = (int)(H3D_MAX(pos - 0.5f, 0));
	        horRightIdx[i] = (int)(H3D_MIN(pos + 0.5f, sw-1));
	        horWeight[i] = (long) (fabs(pos - 0.5f - horLeftIdx[i]) * (1<<shift));
	    }
	 
	    preVerUpIdx = -1;
	    preVerDownIdx = -1;
	    upLinePtr = rowBuf1;
	    downLinePtr = rowBuf2;
	    for(j = 0; j < dh; j++){
	        float pos = (j + 0.5f) * verScaleRatio;
	        int verUpIdx = (int)(H3D_MAX(pos - 0.5f, 0));
	        int verDownIdx = (int)(H3D_MIN(pos + 0.5f, sh-1));
	        long verWeight = (long) (fabs(pos - 0.5f - verUpIdx) * (1<<shift));
	 
	        if(verUpIdx == preVerUpIdx && verDownIdx == preVerDownIdx){
	            ;
	//do nothing
	        }
	        else if(verUpIdx == preVerDownIdx){
	            H3D_SWAP(upLinePtr, downLinePtr, tempPtr);
	            InterpImageBilinear_RowFilter(sdata + sstep*verDownIdx, downLinePtr, dw, horLeftIdx, horRightIdx, horWeight, shift);
	        }else{
	            InterpImageBilinear_RowFilter(sdata + sstep*verUpIdx,   upLinePtr, dw, horLeftIdx, horRightIdx, horWeight, shift);
	            InterpImageBilinear_RowFilter(sdata + sstep*verDownIdx, downLinePtr, dw, horLeftIdx, horRightIdx, horWeight, shift);
	        }        
	 
	        unsigned char* _ptr = ddata + dstep*j;
	        for(i = 0; i < dw-4; i+=4){
	            *_ptr++ = (unsigned char) ( (((1<<shift) - verWeight)*upLinePtr[i] + verWeight*downLinePtr[i]) >> (2*shift) );
	            *_ptr++ = (unsigned char) ( (((1<<shift) - verWeight)*upLinePtr[i+1] + verWeight*downLinePtr[i+1]) >> (2*shift) );
	            *_ptr++ = (unsigned char) ( (((1<<shift) - verWeight)*upLinePtr[i+2] + verWeight*downLinePtr[i+2]) >> (2*shift) );
	            *_ptr++ = (unsigned char) ( (((1<<shift) - verWeight)*upLinePtr[i+3] + verWeight*downLinePtr[i+3]) >> (2*shift) );
	        }
	        for(; i < dw; i++){
	            *_ptr++ = (unsigned char) ( (((1<<shift) - verWeight)*upLinePtr[i] + verWeight*downLinePtr[i]) >> (2*shift) );
	        }
	        preVerUpIdx = verUpIdx;
	        preVerDownIdx = verDownIdx;
	    }

		for(j=0;j<dh;j++)
		{
			for(i=0;i<dw;i++)
			{
				dst[i*3+k+j*dw*3] = ddata[i+j*dw];
			}
		}
	}
	
    delete []rowBuf1;
    delete []rowBuf2;
    delete []horWeight;
    delete []horLeftIdx;
    delete []horRightIdx;
	delete []sdata;
	delete []ddata;

	return ;
}

void InterpYuvBilinear(unsigned char* src, unsigned char* dst, int src_width, int src_height, int dst_width, int dst_height)
{
    int i, j;
    int sw, sh, sstep;
    int dw, dh, dstep;
    unsigned char *sdata, *ddata;
    float horScaleRatio, verScaleRatio;
    long *rowBuf1, *rowBuf2;
    long *upLinePtr, *downLinePtr, *tempPtr;
    long *horWeight;
    int *horLeftIdx, *horRightIdx;
    int preVerUpIdx, preVerDownIdx;
    int shift = 8;
 
    sw = src_width; sh = src_height; sstep = src_width; 
    dw = dst_width; dh = dst_height; dstep = dst_width;
 
    horScaleRatio = sw / (float)(dw);
    verScaleRatio = sh / (float)(dh);
 
    rowBuf1 = new long[dw];
    rowBuf2 = new long[dw];
    horWeight = new long[dw];
    horLeftIdx = new int[dw];
    horRightIdx = new int[dw];
	sdata = new unsigned char [sw*sh];
	ddata = new unsigned char [dw*dh];

	for(j=0;j<sh;j++)
	{
		for(i=0;i<sw;i++)
		{
			sdata[i+j*sw] = src[i+j*sw];
		}
	}

//col interpolation
    for(i = 0; i < dw; i++){
        float pos = (i + 0.5f) * horScaleRatio;
        horLeftIdx[i] = (int)(H3D_MAX(pos - 0.5f, 0));
        horRightIdx[i] = (int)(H3D_MIN(pos + 0.5f, sw-1));
        horWeight[i] = (long) (fabs(pos - 0.5f - horLeftIdx[i]) * (1<<shift));
    }
 
    preVerUpIdx = -1;
    preVerDownIdx = -1;
    upLinePtr = rowBuf1;
    downLinePtr = rowBuf2;
    for(j = 0; j < dh; j++){
        float pos = (j + 0.5f) * verScaleRatio;
        int verUpIdx = (int)(H3D_MAX(pos - 0.5f, 0));
        int verDownIdx = (int)(H3D_MIN(pos + 0.5f, sh-1));
        long verWeight = (long) (fabs(pos - 0.5f - verUpIdx) * (1<<shift));
 
        if(verUpIdx == preVerUpIdx && verDownIdx == preVerDownIdx){
            ;
//do nothing
        }
        else if(verUpIdx == preVerDownIdx){
            H3D_SWAP(upLinePtr, downLinePtr, tempPtr);
            InterpImageBilinear_RowFilter(sdata + sstep*verDownIdx, downLinePtr, dw, horLeftIdx, horRightIdx, horWeight, shift);
        }else{
            InterpImageBilinear_RowFilter(sdata + sstep*verUpIdx,   upLinePtr, dw, horLeftIdx, horRightIdx, horWeight, shift);
            InterpImageBilinear_RowFilter(sdata + sstep*verDownIdx, downLinePtr, dw, horLeftIdx, horRightIdx, horWeight, shift);
        }        
 
        unsigned char* _ptr = ddata + dstep*j;
        for(i = 0; i < dw-4; i+=4){
            *_ptr++ = (unsigned char) ( (((1<<shift) - verWeight)*upLinePtr[i] + verWeight*downLinePtr[i]) >> (2*shift) );
            *_ptr++ = (unsigned char) ( (((1<<shift) - verWeight)*upLinePtr[i+1] + verWeight*downLinePtr[i+1]) >> (2*shift) );
            *_ptr++ = (unsigned char) ( (((1<<shift) - verWeight)*upLinePtr[i+2] + verWeight*downLinePtr[i+2]) >> (2*shift) );
            *_ptr++ = (unsigned char) ( (((1<<shift) - verWeight)*upLinePtr[i+3] + verWeight*downLinePtr[i+3]) >> (2*shift) );
        }
        for(; i < dw; i++){
            *_ptr++ = (unsigned char) ( (((1<<shift) - verWeight)*upLinePtr[i] + verWeight*downLinePtr[i]) >> (2*shift) );
        }
        preVerUpIdx = verUpIdx;
        preVerDownIdx = verDownIdx;
    }

	for(j=0;j<dh;j++)
	{
		for(i=0;i<dw;i++)
		{
			dst[i+j*dw] = ddata[i+j*dw];
		}
	}
	
    delete []rowBuf1;
    delete []rowBuf2;
    delete []horWeight;
    delete []horLeftIdx;
    delete []horRightIdx;
	delete []sdata;
	delete []ddata;

	return ;
}


