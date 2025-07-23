#ifndef _IMAGE_
#define _IMAGE_

#include"zf_common_headfile.h"

extern volatile uint8_t AIM_LINE_SET;                  //目标行
extern volatile uint8_t RAMP_AIM_DELTA;
extern volatile uint8_t AIM_LINE;
extern int biaos;
extern bool Flag_SpeedDown;

//图像采集及处理
#define ImageW                 94                      //压缩后的图像宽
#define ImageW_2               47                      
#define ImageW_3               31
#define ImageW_2_3             62
#define ImageH                 60                      //压缩后的图像高
#define ImageH_2               30
#define ImageH_3               20
#define ImageH_4               15
#define ImageH_6               10
#define ImageH_2_3             40
#define ImageH_5_6             50
#define EFFECTIVE_ROW          4                       //定义图片最上层的有效行
#define TOTAL_POINT            ImageW*ImageH
#define center                 46.5

#define Shield_H               44
#define Shield_w1              15
#define Shield_w2               80
//整体滤波补线
#define K_MAX_THRESHOLD          10                      //两有效点间最大斜率
#define LOST_LINE_THRESHOLD      (60-EFFECTIVE_ROW )                 //丢边行最大值
#define VALID_LINE_THRESHOLE     (60-LOST_LINE_THRESHOLD)  //有效行最小值
#define MINRoadLen               5                       //最小路宽
#define roadK                   (1.2653)                 //直赛道K  1.274
#define roadB                   (1.347)                  //直赛道B  1.789
#define SingleLineLeanAveERR_MAX 23                      //一边丢线，未丢线一边最大平均倾斜误差
#define SingleLineLeanK          25                      //一边丢线，未丢线一边补偿系数(理论距离实际中线的距离)

extern uint8 para;                      // 图像上部亮度矫正参数
extern uint8 C_Image[ImageH][ImageW];
extern uint16 threshold;
extern uint16 light;
extern uint16 stop_count;  // 停车计数器
extern uint16 find_count;  // 查找斑马线计数器
extern uint8_t poat_num;
extern bool stop_flag;
extern uint16_t start_time;
//圆环参数
#define FIXED_LEFT_ANGLE  (-20.0f)    // 固定左打角角度（负号左打）
#define FIXED_RIGHT_ANGLE  (20.0f)    // 固定右打角角度（正号右打）
#define OUT_ISLAND_TURN_TIME  10      // 打角时间，比如50次周期（周期20ms就是1秒）
extern bool Left_Island_state;
extern bool Left_Island_flag0;
extern bool Left_Island_flag1;
extern bool Left_Island_flag2;
extern bool Left_Island_flag3;
extern bool Left_Island_flag4;
extern bool Out_Island_Left_Turn;   // 是否进入固定左打角模式
extern bool Right_Island_state;
extern bool Right_Island_flag0;
extern bool Right_Island_flag1;
extern bool Right_Island_flag2;
extern bool Right_Island_flag3;
extern bool Right_Island_flag4;
extern bool Search_In_Down_Point_Flag;
extern bool Out_Island_Right_Turn;   // 是否进入固定右打角模式
extern bool Out_Island_Transition_Flag;  // 是否处于缓慢过渡阶段
extern float k_in;
extern float k_out;
extern int In_Down_Point_Row, In_Down_Point;
extern int In_Up_Point_Row, In_Up_Point;
extern bool In_Up_Point_Flag;
extern int Out_Down_Point_Row, Out_Down_Point;
extern bool Out_Down_Point_Flag;
extern int Out_Up_Point_Row, Out_Up_Point;
extern bool Out_Up_Point_Flag;
extern short Inv_Point[3], Inv_Point_Row[3];
extern float Out_Island_Transition_Ratio;  // 初始打角权重 = 100%固定角
extern float Normal_Angle;
extern int Lose_Count;
extern int Left_Second_Difference[45];
extern int Left_Second_Difference_Count;
extern int Right_Second_Difference[45];
extern int Right_Second_Difference_Count;
extern bool In_Down_Flag;
extern bool Mutation_Flag;
extern bool Straight_Flag;
extern int Left_Mutation_Count;
extern int Right_Mutation_Count;
extern bool Turn_Flag;
extern int Out_Up_Point2_Row;
extern int Out_Up_Point2;
extern float k_Left;
extern float k_Right;
extern int Straight_Count;
extern uint8_t White_Column[2]; //只需要46、47两列

//结构体定义
typedef struct
{
    volatile bool Exist_Left[ImageH];                 //某一行左边边界点存在
    volatile bool Exist_Right[ImageH];                //某一行右边边界点存在
    volatile bool Exist_Center[ImageH];               //某一行中心点存在

    volatile bool Lost_Center;                           //是否丢了中线
    volatile bool Lost_Left;                             //是否丢了左线
    volatile bool Lost_Right;                            //是否丢了右线

    volatile short Point_Left[ImageH];                //存放某一行的左边界点
    volatile short Point_Right[ImageH];               //存放某一行的右边界点
    volatile short Point_Center[ImageH];              //存放某一行的中心点

    volatile short White_Num[ImageH];                 //某一行的白点个数
}imageLine_t;


typedef struct
{
    volatile float cam_finalCenterERR[10];//中线偏差
}Image_Data;

/************************************图像采集及处理************************************************/
extern uint8 bin_thr;
extern bool Pixle[ImageH][ImageW];
extern volatile imageLine_t imageLine;
extern Image_Data image;
void ImageProcess(void);
int GetOSTU(uint8 tmImage[ImageH][ImageW]);
void trackDFS(void);
void Get_White_Num(void);

/****************************************滤波******************************************************/
void Pixle_Filter(void);
void Left_Right_Confusion_Filter(void);
void doFilter(void);
void left_right_Limit(void);
void lineChangeLimit(void);
void singlePoint_Filter(void);
void lostLine_Filter(void);
/****************************************补线******************************************************/
void amplitudeLIMIT(uint16_t i, int16_t amp);
void limitCenter(void);
void TurnFliter(short Lturnpoint_row, short Lturnpoint, short Rturnpoint_row, short Rturnpoint, float limitKl, float limitKr,
    float kl_up, float bl_up, float kl_down, float bl_down, float kr_up, float br_up, float kr_down, float br_down, uint8_t error_cnt_l, uint8_t error_cnt_r);
void doMend(void);
void new_trackMend_startPart(void);
void trackMend_startPart(void);
void trackMend_HalfPart(void);
/**************************************底层函数****************************************************/
extern short LTurnPoint_Row, LTurnPoint;
extern short RTurnPoint_Row, RTurnPoint;
extern float Kl_up, Bl_up, Kl_down, Bl_down;
extern float Kr_up, Br_up, Kr_down, Br_down;
extern uint8_t Error_Cnt_L, Error_Cnt_R;
bool isWhite(uint16_t row, uint16_t line);

bool isLeftPoint(uint16_t i, uint16_t j);
bool isRightPoint(uint16_t i, uint16_t j);
bool LeftLine_Check(uint16_t EndLine);
bool RightLine_Check(uint16_t EndLine);
void TurnPointL_Find(short* TurnPoint_Row, short* TurnPoint, float* Kl_up, float* Bl_up, float* Kl_down, float* Bl_down, uint8_t* error_cnt_l);
void TurnPointR_Find(short* TurnPoint_Row, short* TurnPoint, float* Kr_up, float* Br_up, float* Kr_down, float* Br_down, uint8_t* error_cnt_r);
uint8_t road_right(uint8_t row);
uint8_t road_left(uint8_t row);
uint8_t RoadRight(uint8_t row);
int8_t RoadLeft(uint8_t row);

/****************************************圆环******************************************************/
//新圆环函数
void Second_Difference(void);
void Left_Island_detect(void);
void Left_Island_logic(void);
void Left_Island_update(void);
void Right_Island_detect(void);
void Right_Island_logic(void);
void Right_Island_update(void);

uint8_t left_inexist_number(uint8_t begin_row,uint8_t search_row);
uint8_t right_inexist_number(uint8_t begin_row,uint8_t search_row);
bool Rectangle_isWhite(uint8_t x, uint8_t y, uint8_t left_search_colume, uint8_t up_search_row);

/***************************************断路******************************************************/
void zebra_found_zz(void);
void park(void);

void updateMediumLine(void);//更新中线打角
bool MediumLineCal(volatile float* final);//计算中线打角
void show_line();
void gamma_correction(uint8 img[ImageH][ImageW], float gamma);
void island_flag_init(void);
void Longest_White_Column(void);

#endif
