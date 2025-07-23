#ifndef _IMAGE_
#define _IMAGE_

#include"zf_common_headfile.h"

//图像采集及处理
#define ImageW                 188                      //压缩后的图像宽
#define ImageH                 50                       //压缩后的图像高
#define center                 93.5
#define max_size (ImageW * ImageH)

typedef struct
{
    volatile float cam_finalCenterERR[10];//中线偏差
}Image_Data;

extern Image_Data image;
extern float delta_y;
extern uint16 light;
extern bool Pixle[ImageH][ImageW];
extern bool rst;
extern uint8_t bin_thr;
extern float center_line;
extern float center_y;
extern uint16_t parent[max_size];
extern uint16_t kind[max_size];
extern int max1;
extern int max2;
extern float img1_xx, img1_yy, img2_xx, img2_yy;
extern signed short servo_error;
extern float motorerror;
extern float variance_y;
/***************************************后车******************************************************/
int find(int x);
void init_union_find(void);
void union_label(int x, int y);
bool two_pass(void);
bool MediumLineCal_2(volatile float* final, uint8 img1_x, uint8 img2_x);
void ImageProcess(void);
float calculate_variance(float average, float num);
void gamma_correction(uint8 img[ImageH][ImageW], float gamma);
#endif


