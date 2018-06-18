
#include "common.h"

int32_t RGB_Y_tab[256];
int32_t B_U_tab[256];
int32_t G_U_tab[256];
int32_t G_V_tab[256];
int32_t R_V_tab[256];

/*	rgb -> yuv def's

	this following constants are "official spec"
	Video Demystified" (ISBN 1-878707-09-4)

	rgb<->yuv _is_ lossy, since most programs do the conversion differently
		
	SCALEBITS/FIX taken from  ffmpeg
*/

#define Y_R_IN			0.257
#define Y_G_IN			0.504
#define Y_B_IN			0.098
#define Y_ADD_IN		16

#define U_R_IN			0.148
#define U_G_IN			0.291
#define U_B_IN			0.439
#define U_ADD_IN		128

#define V_R_IN			0.439
#define V_G_IN			0.368
#define V_B_IN			0.071
#define V_ADD_IN		128

#define SCALEBITS_IN	8
#define FIX_IN(x)		((uint16_t) ((x) * (1L<<SCALEBITS_IN) + 0.5))





/*	rgb24 -> yuv 4:2:0 planar 

	NOTE: always flips.
*/

void rgb2yuv(uint8_t * src, uint8_t * dsty, uint8_t * dstu, uint8_t * dstv, int width, int height)
{
	int32_t y_stride = width;
	int32_t src_stride = width * 3;
	uint32_t y_dif = y_stride - width;
	uint32_t uv_dif = (y_stride - width) / 2;
	uint32_t x, y;
	uint8_t * y_out = dsty;
	uint8_t * u_out = dstu;
	uint8_t * v_out = dstv;

	if (height < 0) {
		height = -height;
		src += (height - 1) * src_stride;
		src_stride = -src_stride;
	}


	for (y = height / 2; y; y--) {
		/* process one 2x2 block per iteration */
		for (x = 0; x < (uint32_t) width; x += 2) {
			int r, g, b, r4, g4, b4;

			//rgb = *(uint16_t *) (src + x * 2);
			b4 = b = *(src+x*3+0);
			g4 = g = *(src+x*3+1);
			r4 = r = *(src+x*3+2);
			y_out[0] =
				(uint8_t) ((FIX_IN(Y_R_IN) * r + FIX_IN(Y_G_IN) * g +
							FIX_IN(Y_B_IN) * b) >> SCALEBITS_IN) + Y_ADD_IN;

			//rgb = *(uint16_t *) (src + x * 2 + src_stride);
			b4 += b = *(src+x*3+0+src_stride);
			g4 += g = *(src+x*3+1+src_stride);
			r4 += r = *(src+x*3+2+src_stride);
			y_out[y_stride] =
				(uint8_t) ((FIX_IN(Y_R_IN) * r + FIX_IN(Y_G_IN) * g +
							FIX_IN(Y_B_IN) * b) >> SCALEBITS_IN) + Y_ADD_IN;

			//rgb = *(uint16_t *) (src + x * 2 + 2);
			b4 += b = *(src+x*3+0+3);
			g4 += g = *(src+x*3+1+3);
			r4 += r = *(src+x*3+2+3);
			y_out[1] =
				(uint8_t) ((FIX_IN(Y_R_IN) * r + FIX_IN(Y_G_IN) * g +
							FIX_IN(Y_B_IN) * b) >> SCALEBITS_IN) + Y_ADD_IN;

			//rgb = *(uint16_t *) (src + x * 2 + src_stride + 2);
			b4 += b = *(src+x*3+0+3+src_stride);
			g4 += g = *(src+x*3+1+3+src_stride);
			r4 += r = *(src+x*3+2+3+src_stride);
			y_out[y_stride + 1] =
				(uint8_t) ((FIX_IN(Y_R_IN) * r + FIX_IN(Y_G_IN) * g +
							FIX_IN(Y_B_IN) * b) >> SCALEBITS_IN) + Y_ADD_IN;

			*u_out++ =
				(uint8_t) ((-FIX_IN(U_R_IN) * r4 - FIX_IN(U_G_IN) * g4 +
							FIX_IN(U_B_IN) * b4) >> (SCALEBITS_IN + 2)) +
				U_ADD_IN;


			*v_out++ =
				(uint8_t) ((FIX_IN(V_R_IN) * r4 - FIX_IN(V_G_IN) * g4 -
							FIX_IN(V_B_IN) * b4) >> (SCALEBITS_IN + 2)) +
				V_ADD_IN;

			y_out += 2;
		}
		src += src_stride * 2;
		y_out += y_dif + y_stride;
		u_out += uv_dif;
		v_out += uv_dif;
	}
}

void rgb2yuv_pixel(uint8_t *rgb, uint8_t * yuv)
{
	int r, g, b, r4, g4, b4;

	//rgb = *(uint16_t *) (src + x * 2);
	b = rgb[2];
	b4 = b<<2;
	g = rgb[1];
	g4 = g<<2;
	r = rgb[0];
	r4 = r<<2;
	yuv[0] =
		(uint8_t) ((FIX_IN(Y_R_IN) * r + FIX_IN(Y_G_IN) * g +
					FIX_IN(Y_B_IN) * b) >> SCALEBITS_IN) + Y_ADD_IN;
	yuv[1] =
		(uint8_t) ((-FIX_IN(U_R_IN) * r4 - FIX_IN(U_G_IN) * g4 +
					FIX_IN(U_B_IN) * b4) >> (SCALEBITS_IN + 2)) +	U_ADD_IN;

	yuv[2] =
		(uint8_t) ((FIX_IN(V_R_IN) * r4 - FIX_IN(V_G_IN) * g4 -
					FIX_IN(V_B_IN) * b4) >> (SCALEBITS_IN + 2)) +	V_ADD_IN;
}

/*	yuv -> rgb def's */

#define RGB_Y_OUT		1.164
#define B_U_OUT			2.018
#define Y_ADD_OUT		16

#define G_U_OUT			0.391
#define G_V_OUT			0.813
#define U_ADD_OUT		128

#define R_V_OUT			1.596
#define V_ADD_OUT		128


#define SCALEBITS_OUT	13
#define FIX_OUT(x)		((uint16_t) ((x) * (1L<<SCALEBITS_OUT) + 0.5))

/* initialize rgb lookup tables */

void
colorspace_init(void)
{
	int32_t i;

	for (i = 0; i < 256; i++) {
		RGB_Y_tab[i] = FIX_OUT(RGB_Y_OUT) * (i - Y_ADD_OUT);
		B_U_tab[i] = FIX_OUT(B_U_OUT) * (i - U_ADD_OUT);
		G_U_tab[i] = FIX_OUT(G_U_OUT) * (i - U_ADD_OUT);
		G_V_tab[i] = FIX_OUT(G_V_OUT) * (i - V_ADD_OUT);
		R_V_tab[i] = FIX_OUT(R_V_OUT) * (i - V_ADD_OUT);
	}
}

#define YUV2RGB(y, u, v, r, g, b)\
  r = y + ((v*1436) >>10);\
  g = y - ((u*352 + v*731) >> 10);\
  b = y + ((u*1814) >> 10);\
  r = r < 0 ? 0 : r;\
  g = g < 0 ? 0 : g;\
  b = b < 0 ? 0 : b;\
  r = r > 255 ? 255 : r;\
  g = g > 255 ? 255 : g;\
  b = b > 255 ? 255 : b

void yv12_to_rgb24(unsigned char *src, unsigned char *dest, int width, int height)
{
	register int i,j;
	register int y0, y1, u, v;
	register int r, g, b;
	register unsigned char *s[3];
	s[0] = src;
	s[1] = s[0] + width*height;
	s[2] = s[1] + width*height/4;
	
	for (i = 0; i < height; i++) {
		for (j = 0; j < width/2; j++) {
			y0 = *(s[0])++;
			y1 = *(s[0])++;
			if (i % 2 == 0 ) {
			u  = *(s[1])++ - 128;
			v  = *(s[2])++ - 128;
			}
			YUV2RGB (y0, u, v, r, g, b);
			*dest++ = r;
			*dest++ = g;
			*dest++ = b;
			YUV2RGB (y1, u, v, r, g, b);
			*dest++ = r;
			*dest++ = g;
			*dest++ = b;
		}
	}
}


/* yuv 4:2:0 planar -> rgb24 */
void yuv2rgb(uint8_t * srcy, uint8_t * srcu, uint8_t * srcv, uint8_t * dst, int width, int height)
{
#if 1
	int dst_stride = width;
	int y_stride = width;
	int uv_stride = width>>1;
	uint8_t * y_src = srcy;
	uint8_t * u_src = srcu;
	uint8_t * v_src = srcv;
	const uint32_t dst_dif = 6 * dst_stride - 3 * width;
	int32_t y_dif = 2 * y_stride - width;

	uint8_t *dst2 = dst + 3 * dst_stride;
	uint8_t *y_src2 = y_src + y_stride;
	uint32_t x, y;

	if (height < 0) {			/* flip image? */
		height = -height;
		y_src += (height - 1) * y_stride;
		y_src2 = y_src - y_stride;
		u_src += (height / 2 - 1) * uv_stride;
		v_src += (height / 2 - 1) * uv_stride;
		y_dif = -width - 2 * y_stride;
		uv_stride = -uv_stride;
	}
	for (y = height / 2; y; y--) {
		/* process one 2x2 block per iteration */
		for (x = 0; x < (uint32_t) width / 2; x++) {
			int u, v;
			int b_u, g_uv, r_v, rgb_y;
			int r, g, b;

			u = u_src[x];
			v = v_src[x];
			
			b_u = B_U_tab[u];
			g_uv = G_U_tab[u] + G_V_tab[v];
			r_v = R_V_tab[v];

			rgb_y = RGB_Y_tab[*y_src];
			b = (rgb_y + b_u) >> SCALEBITS_OUT;
			g = (rgb_y - g_uv) >> SCALEBITS_OUT;
			r = (rgb_y + r_v) >> SCALEBITS_OUT;
			dst[0] = H3D_MAX(0, H3D_MIN(255, b));
			dst[1] = H3D_MAX(0, H3D_MIN(255, g));
			dst[2] = H3D_MAX(0, H3D_MIN(255, r));
			
			y_src++;
			rgb_y = RGB_Y_tab[*y_src];
			b = (rgb_y + b_u) >> SCALEBITS_OUT;
			g = (rgb_y - g_uv) >> SCALEBITS_OUT;
			r = (rgb_y + r_v) >> SCALEBITS_OUT;
			dst[3] = H3D_MAX(0, H3D_MIN(255, b));
			dst[4] = H3D_MAX(0, H3D_MIN(255, g));
			dst[5] = H3D_MAX(0, H3D_MIN(255, r));
			y_src++;

			rgb_y = RGB_Y_tab[*y_src2];
			b = (rgb_y + b_u) >> SCALEBITS_OUT;
			g = (rgb_y - g_uv) >> SCALEBITS_OUT;
			r = (rgb_y + r_v) >> SCALEBITS_OUT;
			dst2[0] = H3D_MAX(0, H3D_MIN(255, b));
			dst2[1] = H3D_MAX(0, H3D_MIN(255, g));
			dst2[2] = H3D_MAX(0, H3D_MIN(255, r));
			y_src2++;

			rgb_y = RGB_Y_tab[*y_src2];
			b = (rgb_y + b_u) >> SCALEBITS_OUT;
			g = (rgb_y - g_uv) >> SCALEBITS_OUT;
			r = (rgb_y + r_v) >> SCALEBITS_OUT;
			dst2[3] = H3D_MAX(0, H3D_MIN(255, b));
			dst2[4] = H3D_MAX(0, H3D_MIN(255, g));
			dst2[5] = H3D_MAX(0, H3D_MIN(255, r));
			y_src2++;

			dst += 6;
			dst2 += 6;

		}

		dst += dst_dif;
		dst2 += dst_dif;

		y_src += y_dif;
		y_src2 += y_dif;

		u_src += uv_stride;
		v_src += uv_stride;
	}
#else
int dst_stride = width;
int y_stride = width;
int uv_stride = width>>1;
uint8_t * y_src = srcy;
uint8_t * u_src = srcu;
uint8_t * v_src = srcv;
const uint32_t dst_dif = 6 * dst_stride - 3 * width;
int32_t y_dif = 2 * y_stride - width;

uint8_t *dst2 = dst + 3 * dst_stride;
uint8_t *y_src2 = y_src + y_stride;
uint32_t x, y;

if (height < 0) {			/* flip image? */
	height = -height;
	y_src += (height - 1) * y_stride;
	y_src2 = y_src - y_stride;
	u_src += (height / 2 - 1) * uv_stride;
	v_src += (height / 2 - 1) * uv_stride;
	y_dif = -width - 2 * y_stride;
	uv_stride = -uv_stride;
}

for (y = height / 2; y; y--) {
	/* process one 2x2 block per iteration */
	for (x = 0; x < (uint32_t) width / 2; x++) {
		int u, v;
		int b_u, g_uv, r_v, rgb_y;
		int r, g, b;

		u = u_src[x];
		v = v_src[x];

		b_u = B_U_tab[u];
		g_uv = G_U_tab[u] + G_V_tab[v];
		r_v = R_V_tab[v];

		rgb_y = RGB_Y_tab[*y_src];
		b = (rgb_y + b_u) >> SCALEBITS_OUT;
		g = (rgb_y - g_uv) >> SCALEBITS_OUT;
		r = (rgb_y + r_v) >> SCALEBITS_OUT;
		dst[0] = H3D_MAX(0, H3D_MIN(255, b));
		dst[1] = 0;//H3D_MAX(0, H3D_MIN(255, g));
		dst[2] = 0;//H3D_MAX(0, H3D_MIN(255, r));

		y_src++;
		rgb_y = RGB_Y_tab[*y_src];
		b = (rgb_y + b_u) >> SCALEBITS_OUT;
		g = (rgb_y - g_uv) >> SCALEBITS_OUT;
		r = (rgb_y + r_v) >> SCALEBITS_OUT;
		dst[3] = H3D_MAX(0, H3D_MIN(255, b));
		dst[4] = 0;//H3D_MAX(0, H3D_MIN(255, g));
		dst[5] = 0;//H3D_MAX(0, H3D_MIN(255, r));
		y_src++;

		rgb_y = RGB_Y_tab[*y_src2];
		b = (rgb_y + b_u) >> SCALEBITS_OUT;
		g = (rgb_y - g_uv) >> SCALEBITS_OUT;
		r = (rgb_y + r_v) >> SCALEBITS_OUT;
		dst2[0] = H3D_MAX(0, H3D_MIN(255, b));
		dst2[1] = 0;//H3D_MAX(0, H3D_MIN(255, g));
		dst2[2] = 0;//H3D_MAX(0, H3D_MIN(255, r));
		y_src2++;

		rgb_y = RGB_Y_tab[*y_src2];
		b = (rgb_y + b_u) >> SCALEBITS_OUT;
		g = (rgb_y - g_uv) >> SCALEBITS_OUT;
		r = (rgb_y + r_v) >> SCALEBITS_OUT;
		dst2[3] = H3D_MAX(0, H3D_MIN(255, b));
		dst2[4] = 0;//H3D_MAX(0, H3D_MIN(255, g));
		dst2[5] = 0;//H3D_MAX(0, H3D_MIN(255, r));
		y_src2++;

		dst += 6;
		dst2 += 6;
	}

	dst += dst_dif;
	dst2 += dst_dif;

	y_src += y_dif;
	y_src2 += y_dif;

	u_src += uv_stride;
	v_src += uv_stride;
}


#endif
}

void yuv2rgb_pixel(uint8_t *yuv, uint8_t * rgb)
{
	uint8_t y = yuv[0];
	uint8_t u = yuv[1];
	uint8_t v = yuv[2];
	
	int rgb_y, rgb_u, rgb_v;
	int b_u, g_uv, r_v;
	int r, g, b;

	rgb_u = u;
	rgb_v = v;

	b_u = B_U_tab[u];
	g_uv = G_U_tab[u] + G_V_tab[v];
	r_v = R_V_tab[v];

	rgb_y = RGB_Y_tab[y];
	b = (rgb_y + b_u) >> SCALEBITS_OUT;
	g = (rgb_y - g_uv) >> SCALEBITS_OUT;
	r = (rgb_y + r_v) >> SCALEBITS_OUT;
	rgb[2] = (uint8_t)H3D_MAX(0, H3D_MIN(255, b));
	rgb[1] = (uint8_t)H3D_MAX(0, H3D_MIN(255, g));
	rgb[0] = (uint8_t)H3D_MAX(0, H3D_MIN(255, r));

}


