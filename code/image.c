#include "zf_common_headfile.h"

uint8_t bin_thr = 0;
bool rst = 0;
bool Pixle[ImageH][ImageW];
static bool isVisited[ImageH][ImageW];
Image_Data image;
float delta_y;
uint16 light = 180;
uint8 C_Image[ImageH][ImageW];
uint8 image_pross[ImageH][ImageW];
uint8 pixel_min_num = 3;
uint8 pixel_max_num = 1000;
float img1_xx, img1_yy, img2_xx, img2_yy;
signed short servo_error;
float motorerror;
int max1,max2;
float center_line,center_y;
uint16_t parent[max_size] = {0};
uint16_t kind[max_size] = {0};

float variance_y = 0;

void ImageProcess(void)
{
    uint32 sumx1 = 0, sumy1 = 0, count1 = 0;
    uint32 sumx2 = 0, sumy2 = 0, count2 = 0;
    max1 = 0;
    max2 = 0;
    bin_thr = 170;

    uint8_t i, j;
    static uint8 lostTime = 0;

    uint16_t sum_y = 0, average_y = 0, variance_sum_y = 0;
    uint16_t point_y_record[max_size] = {0};
    uint32_t certer_x  = 0;
    uint32_t count = 0;

    for (uint8 j = 0; j < ImageW; j++)
    {
        for (uint8 i = 0; i < ImageH; i++)
        {
            if (mt9v03x_image[40 + i][j] >= bin_thr)
            {
                Pixle[i][j] = true;
            }
            else
            {
                Pixle[i][j] = false;
            }
        }
    }

    rst = two_pass();
    if (rst)
    {
        for (uint8 i = 0; i < ImageH; i++)
        {
             for (uint8 j = 0; j < ImageW; j++)
             {
                 if (image_pross[i][j] == max1)
                 {
                     sumx1 += j;
                     sumy1 += i;
                     count1++;
                  }
                 else if (image_pross[i][j] == max2)
                 {
                     sumx2 += j;
                     sumy2 += i;
                     count2++;
                 }

                 if(image_pross[i][j] == max1 || image_pross[i][j] == max2)
                 {
                      point_y_record[count] = i;
                      count++;
                 }
             }
         }
            if (count1 > 0)
            {
                img1_xx = (float) sumx1 / (float) count1;
                img1_yy = (float) sumy1 / (float) count1;
            }
            if (count2 > 0)
            {
                img2_xx = sumx2 / count2;
                img2_yy = sumy2 / count2;
            }

            certer_x = (img1_xx + img2_xx) / 2.0f;
            center_y = (img1_yy + img2_yy) / 2.0f;

            sum_y = sumy1 + sumy2;
            average_y = sum_y / count;
            for(uint8_t k = 0 ; k < count; k++)
            {
                  variance_sum_y += calculate_variance(average_y,point_y_record[k]);
            }
            variance_y = round((variance_sum_y / count) * 100) / 100 ;            //y的方差

            MediumLineCal_2(image.cam_finalCenterERR, img1_xx, img2_xx);
            servo_error = -(0.7f * image.cam_finalCenterERR[0] + 0.15f * image.cam_finalCenterERR[1]
                           + 0.1f * image.cam_finalCenterERR[2] + 0.05f * image.cam_finalCenterERR[3]);  // 最新偏差
            motorerror = - servo_error;
        }
        else {
            //rst = 1;
            //Flag.Duty_Stop_Flag = 1;
            //Flag.Ui_Stop_Flag = 1;
        }
        delta_y = round( my_abs_float(img1_yy - img2_yy) *100 ) / 100 ;
}


/*
void ImageProcess(void)
{

    uint32_t certer_x  = 0;
    uint32_t count = 0;
    uint32_t white_point_count = 0;
    uint32_t white_point_squared_distance = 0;
    uint32_t white_count_root_mean_square_distance =0;
    uint8_t i, j;
    uint16_t sumx1 = 0, sumy1 = 0, count1 = 0;
    uint16_t sumx2 = 0, sumy2 = 0, count2 = 0;
    uint16_t sum_y = 0, average_y = 0, variance_sum_y = 0;
    uint16_t point_y_record[ImageH * ImageH] = {0};

    static uint8 lostTime = 0;

    variance_y = 0;

    for (i = 0; i < ImageH; i++)
    {
       for (j = 0; j < ImageW; j++)
       {
            C_Image[i][j] = 0;
        }
    }
    for (i = 0; i < ImageH; i++)
    {
        for (j = 0; j < ImageW; j++)
        {
            C_Image[i][j] = mt9v03x_image[2*i][2*j];
        }
    }
    mt9v03x_finish_flag = 0;

    bin_thr = 170;
    for (uint8 j = 0; j < ImageW; j++)
    {
        for (uint8 i = 0; i < ImageH; i++)
        {
            if (C_Image[i][j] >= 150)
            {
                Pixle[i][j] = true;
            }
            else
            {
                Pixle[i][j] = false;
            }
        }
    }
    max1 = 0;
    max2 = 0;
    rst = two_pass();
    if (rst)
    {
        lostTime = 0;
        for (uint8_t i = 0; i < ImageH; i++)
        {
             for (uint8_t j = 0; j < ImageW; j++)
             {
                 if (image_pross[i][j] == max1)
                 {
                     sumx1 += j;
                     sumy1 += i;
                     count1++;
                  }
                 else if (image_pross[i][j] == max2)
                 {
                     sumx2 += j;
                     sumy2 += i;
                     count2++;
                 }

                 if(image_pross[i][j] == max1 && image_pross[i][j] == max2)
                 {
                     point_y_record[count] = i;
                     count++;
                 }
             }
         }
            if (count1 > 0)
            {
                img1_xx = (float) sumx1 / (float) count1;
                img1_yy = (float) sumy1 / (float) count1;
            }
            if (count2 > 0)
            {
                img2_xx = sumx2 / count2;
                img2_yy = sumy2 / count2;
            }

            certer_x = (img1_xx + img2_xx) / 2.0f;
            center_y = (img1_yy + img2_yy) / 2.0f;

            delta_y = round( my_abs_float(img1_yy - img2_yy) *100 ) / 100 ;

            sum_y = sumy1 + sumy2;
            average_y = sum_y / count;
            for(uint8_t k = 0 ; k < count; k++)
            {
                variance_sum_y += calculate_variance(average_y,point_y_record[k]);
            }
            variance_y = round((variance_sum_y / count) * 100) / 100 ;            //y的方差

            MediumLineCal_2(image.cam_finalCenterERR, img1_xx, img2_xx);

        }
        else if (!rst && (lostTime <= 50) || abs(img1_xx - img2_xx) >= 4){
            lostTime++;
        }
        else {
            //rst = 1;
            //Flag.Duty_Stop_Flag = 1;
            //Flag.Ui_Stop_Flag = 1;
        }
}
*/

float calculate_variance(float average, float num)
{
    float out = 0;
    out = (num - average)*(num - average);
    return out;
}

bool MediumLineCal_2(volatile float* final, uint8 img1_xx, uint8 img2_xx)
{
    uint8 i;

    float error;

    for (i = 0; i < 9; i++) {
        final[9 - i] = final[8 - i];
    }

    center_line = (img1_xx + img2_xx) / 2.0f;

    error = (float)(center_line - center);

    final[0] = error;

    return true;
}

int find(int x) {
    int i = x;
    while (0 != parent[i])
        i = parent[i];
    return i;
}

void union_label(int x, int y) {
    int i = x;
    int j = y;
    while (0 != parent[i])
        i = parent[i];
    while (0 != parent[j])
        j = parent[j];
    if (i != j)
        parent[MAX(i,j)] = MIN(i,j);
}

bool two_pass(void)
{
    int i, j;
    uint8 label = 1;

    for (j = 0; j < max_size; j++) {
        parent[j] = 0;
        kind[j] = 0;
    }

    for (i = 0; i < ImageH; i++) {
        for (j = 0; j < ImageW; j++)
            image_pross[i][j] = 0;
    }

    //寻找全部连通域
    for (i = 1; i < ImageH; i++) {
        for (j = 1; j < ImageW; j++) {
            if (Pixle[i][j]) {
                if (!image_pross[i][j - 1] && !image_pross[i - 1][j]) {//左，上都不是白点，新建标签
                    image_pross[i][j] = label++;
                }
                else if (image_pross[i][j - 1] && !image_pross[i - 1][j]) {//上是左不是，继承标签
                    image_pross[i][j] = image_pross[i][j - 1];
                }
                else if (!image_pross[i][j - 1] && image_pross[i - 1][j]) {//左是上不是，继承标签
                    image_pross[i][j] = image_pross[i - 1][j];
                }
                else {//左上都是白点
                    if (image_pross[i][j - 1] == image_pross[i - 1][j]) {//标签相同，继承标签
                        image_pross[i][j] = image_pross[i][j - 1];
                    }
                    else {//标签不同，继承小标签，合并两标签
                        image_pross[i][j] = MIN(image_pross[i][j - 1], image_pross[i - 1][j]);
                        union_label(image_pross[i][j - 1], image_pross[i - 1][j]);
                    }
                }
            }
        }
    }

    //让全部点指向最初点
    for (i = 1; i < ImageH; i++) {
        for (j = 1; j < ImageW; j++) {
            image_pross[i][j] = find(image_pross[i][j]);
        }
    }

    //记录全部连通域点数
    for (i = 0; i < ImageH; i++) {
        for (j = 0; j < ImageW; j++){
            if (image_pross[i][j])
                kind[image_pross[i][j]]++;
        }
    }

    //找到最大两连通域

    for (i = 1; i < max_size; i++) {
        if (kind[i] > kind[max1]) {
            max2 = max1;
            max1 = i;
        } else if (kind[i] > kind[max2]) {
            max2 = i;
        }
    }

    if(!(kind[max1] + kind[max2] >=4 ))//连通域检查
    {
        return false;
    }
    /*
    // 鍙繚鐣� max1 鍜� max2
    for (i = 0; i < ImageH; i++) {
        for (j = 0; j < ImageW; j++) {
            if (image_pross[i][j] == max1 || image_pross[i][j] == max2)
                Pixle[i][j] = true;
            else
                Pixle[i][j] = false;
        }
    }
    */

    return true;
}

void gamma_correction(uint8 image[ImageH][ImageW], float gamma)
{
    // 构建查找表
    uint8 gamma_LUT[256];
    for (int i = 0; i < 256; i++)
    {
        gamma_LUT[i] = (uint8)constrain_int16(powf(i / 255.0, 1.0f / gamma) * 255.0,0,255);
    }

    // 就地修改图像
    for (int i = 0; i < ImageH; i++)
    {
        for (int j = 0; j < ImageW; j++)
        {
            image[i][j] = gamma_LUT[image[i][j]];
        }
    }
}
