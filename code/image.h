#ifndef _IMAGE_
#define _IMAGE_

#include"zf_common_headfile.h"

//ͼ��ɼ�������
#define ImageW                 188                      //ѹ�����ͼ���
#define ImageH                 50                       //ѹ�����ͼ���
#define center                 93.5
#define max_size (ImageW * ImageH)

typedef struct
{
    volatile float cam_finalCenterERR[10];//����ƫ��
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
/***************************************��******************************************************/
int find(int x);
void init_union_find(void);
void union_label(int x, int y);
bool two_pass(void);
bool MediumLineCal_2(volatile float* final, uint8 img1_x, uint8 img2_x);
void ImageProcess(void);
float calculate_variance(float average, float num);
void gamma_correction(uint8 img[ImageH][ImageW], float gamma);
#endif


