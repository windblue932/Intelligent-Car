#include "zf_common_headfile.h"

/*图像处理*/
uint8_t bin_thr = 0;
bool Pixle[ImageH][ImageW];       //二值化图像
static bool isVisited[ImageH][ImageW];//像素点是否访问过
Image_Data image;
volatile imageLine_t imageLine; // 图像边线与中线信息

/*图像拐点*/
short LTurnPoint_Row = EFFECTIVE_ROW, LTurnPoint = 0;
short RTurnPoint_Row = EFFECTIVE_ROW, RTurnPoint = 0;
float Kl_up = 0, Bl_up = 0, Kl_down = 0, Bl_down = 0;
float Kr_up = 0, Br_up = 0, Kr_down = 0, Br_down = 0;
uint8_t Error_Cnt_L = 0, Error_Cnt_R = 0;

/*中线误差*/
volatile uint8_t AIM_LINE_SET;    //目标行36
volatile uint8_t RAMP_AIM_DELTA = 10;
volatile uint8_t AIM_LINE;
bool Flag_SpeedDown = false;
int biaos = 0;
#define SHOW_IMAGE                         (0)

uint8 C_Image[ImageH][ImageW];
uint16 threshold = 230;
uint16 light;
uint8 para = 60;
uint8_t poat_num = 0;
bool stop_flag = false;
uint16_t start_time = 0;

//新圆环参数初始化
bool Left_Island_state = false;
bool Left_Island_flag0 = true;
bool Left_Island_flag1 = false;
bool Left_Island_flag2 = false;
bool Left_Island_flag3 = false;
bool Left_Island_flag4 = false;
bool Out_Island_Left_Turn = false;   // 是否进入固定左打角模式
bool Right_Island_state = false;
bool Right_Island_flag0 = true;
bool Right_Island_flag1 = false;
bool Right_Island_flag2 = false;
bool Right_Island_flag3 = false;
bool Right_Island_flag4 = false;
bool Search_In_Down_Point_Flag = false;
bool Out_Island_Right_Turn = false;   // 是否进入固定右打角模式
bool  Out_Island_Transition_Flag = false;  // 是否处于缓慢过渡阶段
int Out_Island_Turn_Counter = 0;      // 打角时间计数器
float k_in=0;
float k_out=0;
int In_Down_Point_Row, In_Down_Point=0;
int In_Up_Point_Row, In_Up_Point=0;
bool In_Up_Point_Flag=false;
int Out_Down_Point_Row, Out_Down_Point = 0;
bool Out_Down_Point_Flag=false;
int Out_Up_Point_Row, Out_Up_Point = 0;
bool Out_Up_Point_Flag=false;
int i, j, k = 0;
int Max_Out_Down_Point_Row=0;
short Inv_Point[3] = {0, 0, 0}, Inv_Point_Row[3] = {0, 0, 0};
float Out_Island_Transition_Ratio = 1.0f;  // 初始打角权重 = 100%固定角
float Normal_Angle = 0.0;
int Lose_Count = 0;
int Left_Second_Difference[45] = {0};
int Left_Second_Difference_Count = 0;
int Right_Second_Difference[45] = {0};
int Right_Second_Difference_Count = 0;
bool In_Down_Flag = false;
bool Mutation_Flag = false;
bool Straight_Flag = false;
bool Turn_Flag = false;
int Left_Mutation_Count = 0;
int Right_Mutation_Count = 0;
int Out_Up_Point2_Row = 0;
int Out_Up_Point2 = 0;
float k_Left = 0;
float k_Right = 0;
int Straight_Count = 0;
uint8_t White_Column[2];
float White_Content;

/************************************图像采集及处理************************************************/
/*图像预处理函数打包*/
void ImageProcess(void)
{
    uint8 i, j;
    //AIM_LINE = AIM_LINE_SET;

    //边线和中心线都不存在
    imageLine.Lost_Center = false;
    imageLine.Lost_Left = false;
    imageLine.Lost_Right = false;

    for (i = 0; i < ImageH; i++)
    {
        //每一行的左右边界点和中心点都不存在
        imageLine.Exist_Left[i] = false;
        imageLine.Exist_Right[i] = false;
        imageLine.Exist_Center[i] = false;

        //边界点和中心点设为初始位置
        imageLine.Point_Left[i] = 1;
        imageLine.Point_Right[i] = ImageW - 1;
        imageLine.Point_Center[i] = ImageW / 2;

        //每一行的白点个数清零
        imageLine.White_Num[i] = 0;

        for (j = 0; j < ImageW; j++)
        {
            isVisited[i][j] = false;//DFS用, 所有点都还没被遍历过
            C_Image[i][j] = mt9v03x_image[i * 2][j * 2];          //  压缩图像

        }
    }
    /*************大津法得到二值化的阈值**********************/
    bin_thr = GetOSTU(C_Image);
    //bin_thr= otsuThreshold(C_Image);
    /******************图像二值化*******************************/
    for (i = 0; i < ImageH; i++)
    {
        for (j = 0; j < ImageW; j++)
        {
            if (C_Image[i][j] > bin_thr)//白点 1；黑点 0
                Pixle[i][j] = true;
            else
                Pixle[i][j] = false;
        }
    }
    // 过滤车头
    for (j = 31 ; j<61 ; j++){
        for (i = 50  ; i < ImageH ; i++){
            Pixle[i][j] = true;
        }
    }
    //Pixle_Filter();             //过滤二值化图像噪点
}

/*获取二值化阈值(大津法)*/
int GetOSTU(uint8 tmImage[ImageH][ImageW])
{
    int16 i, j;
    uint32 Amount = 0;//像素总数
    uint32 PixelBack = 0;//back像素点数
    uint32 PixelIntegralBack = 0;//back灰度值
    uint32 PixelIntegral = 0;//灰度值总数
    int32 PixelIntegralFore = 0;//fore灰度值
    int32 PixelFore = 0;//fore像素点数
    float OmegaBack, OmegaFore, MicroBack, MicroFore, SigmaB, Sigma; // 类间方差;
    int16 MinValue, MaxValue;
    uint8 Threshold = 0;
    uint8 HistoGram[256];              //

    for (j = 0; j < 256; j++)  HistoGram[j] = 0; //初始化灰度直方图

    for (j = 0; j < ImageH; j++)
    {
        for (i = 0; i < ImageW; i++)
        {
            HistoGram[tmImage[j][i]]++; //统计灰度级中每个像素在整幅图像中的个数
        }
    }

    for (MinValue = 0; MinValue < 256 && HistoGram[MinValue] == 0; MinValue++);        //获取最小灰度的值
    for (MaxValue = 255; MaxValue > MinValue && HistoGram[MinValue] == 0; MaxValue--); //获取最大灰度的值

    if (MaxValue == MinValue)     return MaxValue;         // 图像中只有一个颜色
    if (MinValue + 1 == MaxValue)  return MinValue;        // 图像中只有二个颜色

    for (j = MinValue; j <= MaxValue; j++)    Amount += HistoGram[j];        //  像素总数

    PixelIntegral = 0;
    for (j = MinValue; j <= MaxValue; j++)
    {
        PixelIntegral += HistoGram[j] * j;//灰度值总数 j是0-255 表示灰度
    }
    SigmaB = -1;
    for (j = MinValue; j < MaxValue; j++)//从本次图像的最小灰度到最大灰度遍历
    {
        PixelBack = PixelBack + HistoGram[j];   //前景像素点数
        PixelFore = Amount - PixelBack;         //背景像素点数
        OmegaBack = (float)PixelBack / Amount;//前景像素百分比
        OmegaFore = (float)PixelFore / Amount;//背景像素百分比
        PixelIntegralBack += HistoGram[j] * j;  //前景灰度值
        PixelIntegralFore = PixelIntegral - PixelIntegralBack;//背景灰度值
        MicroBack = (float)PixelIntegralBack / PixelBack;   //前景灰度百分比
        MicroFore = (float)PixelIntegralFore / PixelFore;   //背景灰度百分比
        Sigma = OmegaBack * OmegaFore * (MicroBack - MicroFore) * (MicroBack - MicroFore);//计算类间方差
        if (Sigma > SigmaB)                    //遍历最大的类间方差g //找出最大类间方差以及对应的阈值
        {
            SigmaB = Sigma;
            Threshold = j;//j是0-255灰度值
        }
    }
    return Threshold;                        //返回最佳阈值;
    /*
    int16 i, j;
        uint32 Amount = 0;         // 像素总数
        uint32 PixelIntegral = 0;  // 灰度值总和
        uint8 HistoGram[256] = {0}; // 初始化灰度直方图为0

        // 统计直方图并计算总像素和总灰度值
        for (j = 0; j < ImageH; j++) {
            for (i = 0; i < ImageW; i++) {
                uint8 pixel = tmImage[j][i];
                HistoGram[pixel]++;
                Amount++;
                PixelIntegral += pixel;
            }
        }

        // 获取最小和最大灰度值
        int16 MinValue = 0;
        while (MinValue < 256 && HistoGram[MinValue] == 0) MinValue++;
        int16 MaxValue = 255;
        while (MaxValue > MinValue && HistoGram[MaxValue] == 0) MaxValue--;

        if (MaxValue == MinValue)     return MaxValue; // 仅有一个颜色
        if (MinValue + 1 == MaxValue) return MinValue; // 仅有两个颜色

        uint32 PixelBack = 0;         // 背景像素数
        uint32 PixelIntegralBack = 0; // 背景灰度总和
        double BestSigma = -1;        // 最佳类间方差
        uint8 Threshold = MinValue;   // 初始阈值

        // 预计算常量
        const double InvAmount = 1.0 / Amount;
        const double PixelIntegralDouble = (double)PixelIntegral;

        for (j = MinValue; j <= MaxValue; j++) {
            PixelBack += HistoGram[j];
            if (PixelBack == 0) continue; // 跳过无像素的灰度级

            PixelIntegralBack += (uint32)j * HistoGram[j];
            uint32 PixelFore = Amount - PixelBack; // 前景像素数
            if (PixelFore == 0) break; // 前景无像素可提前终止

            // 计算统计量
            double OmegaBack = PixelBack * InvAmount;
            double OmegaFore = 1.0 - OmegaBack;
            double MicroBack = (double)PixelIntegralBack / PixelBack;
            double MicroFore = (PixelIntegralDouble - PixelIntegralBack) / PixelFore;

            // 计算类间方差
            double Sigma = OmegaBack * OmegaFore * (MicroBack - MicroFore) * (MicroBack - MicroFore);

            // 更新最佳阈值
            if (Sigma > BestSigma) {
                BestSigma = Sigma;
                Threshold = j;
            }
        }
        return Threshold;*/
}

/*得到每一行所有白点的个数*/
void Get_White_Num(void)
{
    uint8_t i, j;
    uint8_t white_num = 0;

    for (i = 0; i < ImageH; i++)
    {
        for (j = 0; j < ImageW; j++)
        {
            if (Pixle[i][j])
            {
                white_num++;
            }
        }
        imageLine.White_Num[i] = white_num;
        white_num = 0;
    }
}

/*DFS巡线：深度优先搜索找出连通域内所有白点(DFS不后退)*/
void trackDFS(void)
{
    uint8_t i, j;
    uint8_t roadPointStackX[300];             //DFS堆栈存储寻找X坐标
    uint8_t roadPointStackY[300];             //DFS堆栈存储寻找Y坐标
    short stackTopPos = -1;

    if (!Flag.Left_Ring_Turn && !Flag.Right_Ring_Turn)
    {
        //选择图片下方中点作为起始点
        if (isWhite(ImageH - 2, ImageW / 2))          //若下方中点就是白点，入栈
        {
            stackTopPos++;
            roadPointStackX[stackTopPos] = ImageH - 2;
            roadPointStackY[stackTopPos] = ImageW / 2;
            isVisited[ImageH - 2][ImageW / 2] = true;
        }
        else         //如果下方中点不是白点，就从右往左搜索，如果连续五个点都是白点，则判定为dfs的起点
        {
            for (j = ImageW - 4; j >= 2; j--)//从右往左搜索
            {
                if (isWhite(ImageH - 2, j - 2) && isWhite(ImageH - 2, j - 1) && isWhite(ImageH - 2, j)
                    && isWhite(ImageH - 2, j + 1) && isWhite(ImageH - 2, j + 2))//连续5个白点
                {
                    //搜索到就入栈
                    stackTopPos++;                      //stackTopPos非零就表示栈非空
                    roadPointStackX[stackTopPos] = ImageH - 2;
                    roadPointStackY[stackTopPos] = j;
                    isVisited[ImageH - 2][j] = true;      //标记为该点已经遍历过
                    break;                              //一旦确定起点就break
                }
            }
        }

        i = j = 0;

        while (stackTopPos >= 0)                        //只要找到起点就开始
        {
            //出栈
            i = roadPointStackX[stackTopPos];
            j = roadPointStackY[stackTopPos];
            stackTopPos--;

            //处理出界，直接continue
            if ((i < EFFECTIVE_ROW) || (i > ImageH - 2) || (j < 1) || (j > ImageW - 1))
            {
                continue;
            }

            /*************以下操作原则是：遇白点入栈，遇黑点“初步”判断为边界点(后续还需要修正滤波补线等操作)**************/
            /**************************堆栈存储：...左右(上)左右(上)...******************************/
            /******************一般情况下，上的白点会先出栈(体现为先一直往图像上方搜索)，然后再出左右*****************/

            //向左搜索
            if (!isVisited[i][j - 1])                   //起点的左边一点是否遍历过
            {
                if (isWhite(i, j - 1))
                {
                    //白点入栈
                    stackTopPos++;
                    roadPointStackX[stackTopPos] = i;
                    roadPointStackY[stackTopPos] = j - 1;
                    isVisited[i][j - 1] = true;         //标记为该点已经遍历过
                }
                else                                    //黑点就初步判断为边界点               (xzl: 感觉会出现覆盖的情况)
                {
                    if (isLeftPoint(i, j))
                    {
                        //这个条件得问问
                        if (!imageLine.Exist_Left[i] || (imageLine.Exist_Left[i] && imageLine.Point_Left[i] < j && j < 59))
                        {
                            imageLine.Point_Left[i] = j;    //左边界点的横坐标
                            imageLine.Exist_Left[i] = 1;    //左边界点存在
                        }
                    }
                }
            }

            //向右搜索
            if (!isVisited[i][j + 1])                   //起点的右边一点是否遍历过
            {
                if (isWhite(i, j + 1))
                {
                    //白点入栈
                    stackTopPos++;
                    roadPointStackX[stackTopPos] = i;
                    roadPointStackY[stackTopPos] = j + 1;
                    isVisited[i][j + 1] = true;         //标记为该点已经遍历过
                }
                else                                    //黑点就初步判断为边界点               (xzl: 感觉会出现覆盖的情况)
                {
                    if (isRightPoint(i, j))
                    {
                        if (!imageLine.Exist_Right[i] || (imageLine.Exist_Right[i] && imageLine.Point_Right[i] > j && j > 35))
                        {
                            imageLine.Point_Right[i] = j;//右边界点的横坐标
                            imageLine.Exist_Right[i] = true;
                        }
                    }
                }
            }

            //向上搜索(向上不判断边界点)
            if (!isVisited[i - 1][j])
            {
                if (isWhite(i - 1, j))
                {
                    stackTopPos++;
                    roadPointStackX[stackTopPos] = i - 1;
                    roadPointStackY[stackTopPos] = j;
                    isVisited[i - 1][j] = true;
                }
            }
        }
    }
    else
    {
        for (i = 0; i <= ImageW / 2; i++)//从左往右搜索
        {
            if (Flag.Left_Ring_Turn)
            {
                if (isWhite(ImageH - 2, i - 2) && isWhite(ImageH - 2, i - 1) && isWhite(ImageH - 2, i)
                    && isWhite(ImageH - 2, i + 1) && isWhite(ImageH - 2, i + 2)//连续5个白点
                    )
                {
                    //搜索到就入栈
                    stackTopPos++;//stackTopPos非零就表示栈非空
                    roadPointStackX[stackTopPos] = ImageH - 2;
                    roadPointStackY[stackTopPos] = i;
                    isVisited[ImageH - 2][i] = 1;
                }
            }
            else
            {
                if (isWhite(ImageH - 2, ImageW - i - 2) && isWhite(ImageH - 2, ImageW - i - 1) && isWhite(ImageH - 2, ImageW - i)
                    && isWhite(ImageH - 2, ImageW - i + 1) && isWhite(ImageH - 2, ImageW - i + 2)//连续5个白点
                    )
                {
                    //搜索到就入栈
                    stackTopPos++;//stackTopPos非零就表示栈非空
                    roadPointStackX[stackTopPos] = ImageH - 2;
                    roadPointStackY[stackTopPos] = i;
                    isVisited[ImageH - 2][i] = 1;
                }
            }
        }

        i = j = 0;

        while (stackTopPos >= 0)
        {
            //出栈
            i = roadPointStackX[stackTopPos];
            j = roadPointStackY[stackTopPos];
            stackTopPos--;

            //处理出界，直接continue
            if ((i < EFFECTIVE_ROW) || (i > ImageH - 2) || (j < 1) || (j > ImageW - 2))
            {
                continue;
            }

            /*************以下操作原则是：遇白点入栈，遇黑点初步判断为边界点(后续还需要修正滤波补线等操作)**************/
            //堆栈存储：...左右(上)左右(上)...
            //一般情况下，上的白点会先出栈(体现为先一直往图像上方搜索)，然后再出左右
            //向左搜索
            if (!isVisited[i][j - 1])
            {
                if (isWhite(i, j - 1))
                {
                    //白点入栈
                    stackTopPos++;
                    roadPointStackX[stackTopPos] = i;
                    roadPointStackY[stackTopPos] = j - 1;
                    isVisited[i][j - 1] = 1;
                }
                else
                {
                    //黑点初步判断为边界点(xzl: 感觉会出现覆盖的情况)
                    if (isLeftPoint(i, j))
                    {
                        imageLine.Point_Left[i] = j;//左线轨迹
                        imageLine.Exist_Left[i] = 1;
                        imageLine.Lost_Left = 0;
                    }
                }
            }

            //向右搜索
            if (!isVisited[i][j + 1])
            {
                if (isWhite(i, j + 1))
                {
                    stackTopPos++;
                    roadPointStackX[stackTopPos] = i;
                    roadPointStackY[stackTopPos] = j + 1;
                    isVisited[i][j + 1] = 1;
                }
                else//可能找到右边界
                {
                    if (isRightPoint(i, j))
                    {
                        imageLine.Point_Right[i] = j;//右线轨迹
                        imageLine.Exist_Right[i] = 1;
                        imageLine.Lost_Right = 0;
                    }
                }
            }
            //向上搜索(向上不判断边界点)
            if (!isVisited[i - 1][j])
            {
                if (isWhite(i - 1, j))
                {
                    stackTopPos++;
                    roadPointStackX[stackTopPos] = i - 1;
                    roadPointStackY[stackTopPos] = j;
                    isVisited[i - 1][j] = 1;
                }
            }
        }
    }
}

/****************************************滤波******************************************************/
/*滤除二值化图像噪点*/
void Pixle_Filter(void)
{

  uint8_t nr; //行
    uint8_t nc; //列

    //四领域
    for (nr = ImageH - 2; nr >= 1; nr--)
    {
        for (nc = 1; nc < ImageW - 1; nc++)
        {
            if ((Pixle[nr][nc] == 0) && (Pixle[nr - 1][nc] + Pixle[nr + 1][nc] + Pixle[nr][nc + 1] + Pixle[nr][nc - 1] > 2))
                Pixle[nr][nc] = true;
            else if ((Pixle[nr][nc] == 1) && (Pixle[nr - 1][nc] + Pixle[nr + 1][nc] + Pixle[nr][nc + 1] + Pixle[nr][nc - 1] < 2))
                Pixle[nr][nc] = false;
            //if ((Pixle[nr][nc] == 0) && (Pixle[nr - 1][nc - 1] + Pixle[nr][nc - 1] + Pixle[nr + 1][nc - 1] + Pixle[nr - 1][nc + 1]
            //    + Pixle[nr][nc + 1] + Pixle[nr + 1][nc + 1] > 4))
            //    Pixle[nr][nc] = true;
        }
    }

/*
    uint16_t nr, nc;
    uint8_t tmp[ImageH][ImageW];

    // 创建临时副本时扩展边界（避免边界判断）
    memcpy(tmp, Pixle, sizeof(tmp));

    // 使用八邻域判断+面积阈值
    for (nr = 1; nr < ImageH-1; nr++) {
        for (nc = 1; nc < ImageW-1; nc++) {
            // 八邻域计算（包含四个对角）
            const uint8_t sum_8 = Pixle[nr-1][nc-1] + Pixle[nr-1][nc] + Pixle[nr-1][nc+1]
                                + Pixle[nr][nc-1] + Pixle[nr][nc+1]
                                + Pixle[nr+1][nc-1] + Pixle[nr+1][nc] + Pixle[nr+1][nc+1];

            // 主过滤条件
            if (Pixle[nr][nc] == 1) {
                // 白点过滤规则：
                //1. 八邻域白色像素数 < 3：孤立点直接滤除
                //2. 3 <= 八邻域数 < 5：需要二次验证四邻域
                tmp[nr][nc] = (sum_8 < 3) ? 0 :
                             ((sum_8 < 5) ? ((Pixle[nr-1][nc] + Pixle[nr+1][nc]
                                           + Pixle[nr][nc-1] + Pixle[nr][nc+1]) >= 2)
                                          : 1);
            } else {
                // 黑点填充规则保持原逻辑
                tmp[nr][nc] = (Pixle[nr-1][nc] + Pixle[nr+1][nc]
                             + Pixle[nr][nc-1] + Pixle[nr][nc+1] > 2) ? 1 : 0;
            }
        }
    }

    memcpy(Pixle, tmp, sizeof(tmp));
*/
}

/*深度遍历滤波*/
void Left_Right_Confusion_Filter(void)
{
    int8_t i = 0, j = 0;
    for (i = 59; i >= 0; i--)
    {
        if (imageLine.Exist_Right[i] && imageLine.Exist_Left[i])
        {
            if (imageLine.Point_Left[i] > imageLine.Point_Right[i])
            {
                for (j = imageLine.Point_Left[i] + 1; j < 93; j++)
                {
                    if (!isWhite(i, j + 1))
                    {
                        if (isRightPoint(i, j))
                            imageLine.Point_Right[i] = j;
                        break;
                    }
                }
                for (j = imageLine.Point_Right[i] - 1; j > 0; j--)
                {
                    if (!isWhite(i, j - 1))
                    {
                        if (isLeftPoint(i, j))
                            imageLine.Point_Left[i] = j;
                        break;
                    }
                }
            }
        }
    }
    for (i = 59; i > 0; i--)
    {
        if (imageLine.Exist_Right[i] && imageLine.Exist_Right[i - 1] && (ABS(imageLine.Point_Right[i] - imageLine.Point_Right[i - 1])) > 5)
        {
            for (j = 5; j > 0; j--)
            {
                if (isRightPoint(i - 1, imageLine.Point_Right[i] + j))
                {
                    imageLine.Point_Right[i - 1] = imageLine.Point_Right[i - 1] + j;
                    break;
                }

                if (isRightPoint(i - 1, imageLine.Point_Right[i - 1] - j))
                {
                    imageLine.Point_Right[i - 1] = imageLine.Point_Right[i - 1] - j;
                    break;
                }
            }
        }
        if (imageLine.Exist_Left[i] && imageLine.Exist_Left[i - 1] && (ABS(imageLine.Exist_Left[i] - imageLine.Exist_Left[i - 1])) > 5)
        {
            for (j = 5; j > 0; j--)
            {
                if (isLeftPoint(i - 1, imageLine.Point_Left[i] + j))
                {
                    imageLine.Point_Left[i - 1] = imageLine.Point_Left[i - 1] + j;
                    break;
                }

                if (isLeftPoint(i - 1, imageLine.Point_Left[i - 1] - j))
                {
                    imageLine.Point_Left[i - 1] = imageLine.Point_Left[i - 1] - j;
                    break;
                }
            }
        }
    }

}
/*filter函数打包*/
void doFilter(void)
{
    if (!Flag.CrossRoad_Find || Flag.crossLeft || Flag.crossRight)
    {
        TurnFliter(LTurnPoint_Row, LTurnPoint, RTurnPoint_Row, RTurnPoint, 3, 3,
            Kl_up, Bl_up, Kl_down, Bl_down, Kr_up, Br_up, Kr_down, Br_down, Error_Cnt_L, Error_Cnt_R);
    }
    left_right_Limit();    //边界点越界限制
    lineChangeLimit();     //边界斜率限制
    singlePoint_Filter();  //单独点滤去
    lostLine_Filter();    //无效行过多滤去(丢边线判断)
}
/*无效行过多滤去(丢边线判断)*/
void lostLine_Filter(void)
{
    //对于左边界线的判断--------------------
    short count = 0;
    int8_t i = 0;

    for (i = 0; i < ImageH; i++)        //从上到下搜索
    {
        if (imageLine.Exist_Left[i])
            count++;
    }

    if (count < VALID_LINE_THRESHOLE)        //如果无效行超过阈值认为该边界线丢失
    {
        imageLine.Lost_Left = 1;
        for (i = 0; i < ImageH; i++)        //从上到下搜索
        {
            imageLine.Exist_Left[i] = 0;
        }
    }
    else
        imageLine.Lost_Left = 0;

    //对于右边界线的判断--------------------
    count = 0;
    for (i = 0; i < ImageH; i++)
    {
        if (imageLine.Exist_Right[i])
            count++;
    }

    if (count < VALID_LINE_THRESHOLE)        //如果无效行超过阈值认为该边界线丢失
    {
        imageLine.Lost_Right = 1;
        for (i = 0; i < ImageH; i++)        //从上到下搜索
        {
            imageLine.Exist_Right[i] = 0;
        }
    }
    else
        imageLine.Lost_Right = 0;
}
/*左右边界点越界滤波*/
void left_right_Limit(void)
{
    int8_t i = 0;

    for (i = 1; i < ImageH - 1; i++)//从左上到右下
    {
        if (imageLine.Exist_Left[i] && imageLine.Exist_Right[i])
        {
            if (imageLine.Point_Left[i] > imageLine.Point_Right[i])
            {
                if (Flag.Right_Ring_Out || Flag.Left_Ring_Find || Flag.Left_Ring_Out_Mend2)
                {
                    imageLine.Exist_Right[i] = 0;
                    continue;
                }
                if (Flag.Left_Ring_Out || Flag.Right_Ring_Find || Flag.Right_Ring_Out_Mend2)
                {
                    imageLine.Exist_Left[i] = 0;
                    continue;
                }
                imageLine.Exist_Right[i] = 0;
                imageLine.Exist_Left[i] = 0;
            }
        }
    }
}
/*边界点不突变*/
void lineChangeLimit(void)
{
    int8_t i = 0, j = 0;
    float leftK = 0, rightK = 0;

    //左边界相邻两有效点斜率检测
    for (i = ImageH - 2; i > 0; i--)        //从下往上
    {
        if (imageLine.Exist_Left[i])        //先找到第一个有效点
        {
            for (j = i + 1; j < ImageH; j++)        //再向下找临近有效点
            {
                if (imageLine.Exist_Left[j])
                {
                    leftK = getLineK(i, imageLine.Point_Left[i], j, imageLine.Point_Left[j]);

                    if (ABS(leftK) > K_MAX_THRESHOLD)
                    {
                        imageLine.Exist_Left[i] = 0;
                        //imageLine.Exist_Left[j] = 0;
                    }
                    break;        //只要找到一个临近有效点，检测后就break到下一个i
                }
            }
        }
    }

    //右边界相邻两有效点斜率检测
    for (i = ImageH - 2; i > 0; i--)        //从下往上
    {
        if (imageLine.Exist_Right[i])        //先找到第一个有效点
        {
            for (j = i + 1; j < ImageH; j++)        //再向下找临近有效点
            {
                if (imageLine.Exist_Right[j])
                {
                    rightK = getLineK(i, imageLine.Point_Right[i], j, imageLine.Point_Right[j]);

                    if (ABS(rightK) > K_MAX_THRESHOLD)
                    {
                        imageLine.Exist_Right[i] = 0;
                        //imageLine.Exist_Right[j] = 0;
                    }
                    break;        //只要找到一个临近有效点，检测后就break到下一个i
                }
            }
        }
    }
}
/*单独边界点滤除*/
void singlePoint_Filter(void)
{
    uint8_t i = 0;

    for (i = EFFECTIVE_ROW; i < ImageH - 1; i++)
    {
        if (!imageLine.Exist_Left[i - 1] && imageLine.Exist_Left[i] && !imageLine.Exist_Left[i + 1])
        {
            imageLine.Exist_Left[i] = false;
        }
        if (!imageLine.Exist_Right[i - 1] && imageLine.Exist_Right[i] && !imageLine.Exist_Right[i + 1])
        {
            imageLine.Exist_Right[i] = false;
        }
    }
    for (i = EFFECTIVE_ROW; i < ImageH - 2; i++)
    {
        if (!imageLine.Exist_Left[i - 1] && imageLine.Exist_Left[i] && imageLine.Exist_Left[i + 1] && !imageLine.Exist_Left[i + 2])
        {
            imageLine.Exist_Left[i] = false;
            imageLine.Exist_Left[i + 1] = false;
        }
        if (!imageLine.Exist_Right[i - 1] && imageLine.Exist_Right[i] && imageLine.Exist_Right[i + 1] && !imageLine.Exist_Right[i + 2])
        {
            imageLine.Exist_Right[i] = false;
            imageLine.Exist_Right[i + 1] = false;
        }
    }
}
/****************************************补线******************************************************/
/*给中点赋值，并认为该行中线有效*/
void amplitudeLIMIT(uint16_t i, int16_t amp)
{
    imageLine.Exist_Center[i] = true;
    imageLine.Point_Center[i] = amp;
}
/*对得出的中线进行突变限幅*/
void limitCenter(void)
{
    for (int8_t i = ImageH - 2; i >= 1; i--)        //从下往上
    {
        if (imageLine.Exist_Center[i] && imageLine.Exist_Center[i + 1])
        {
            if (ABS(imageLine.Point_Center[i] - imageLine.Point_Center[i + 1]) > 6)
            {
                if ((imageLine.Exist_Center[i]) && (!imageLine.Exist_Center[i - 1]))
                    imageLine.Exist_Center[i] = 0;
            }
        }
        if (imageLine.Exist_Center[i] && imageLine.Exist_Center[i - 1])
        {
            if (ABS(imageLine.Point_Center[i] - imageLine.Point_Center[i - 1]) > 6)
            {
                if ((imageLine.Exist_Center[i]) && (!imageLine.Exist_Center[i + 1]))
                    imageLine.Exist_Center[i] = 0;
            }
        }
        if (imageLine.Exist_Center[i] && !imageLine.Exist_Center[i - 1] && !imageLine.Exist_Center[i + 1])
        {
            imageLine.Exist_Center[i] = 0;
        }
    }
}
/*补中线合集*/
void doMend(void)
{
    trackMend_startPart();
    //new_trackMend_startPart();
    trackMend_HalfPart();//丢边线半宽补，不丢线直接计算
}
/*元素外拐点滤波补线*/
void TurnFliter(short Lturnpoint_row, short Lturnpoint, short Rturnpoint_row, short Rturnpoint, float limitKl, float limitKr,
    float kl_up, float bl_up, float kl_down, float bl_down, float kr_up, float br_up, float kr_down, float br_down, uint8_t error_cnt_l, uint8_t error_cnt_r)
{
    short i = 0, j = 0;
    float DeltaKl = kl_up - kl_down, DeltaKr = kr_up - kr_down;
    bool flag_fliterL = false, flag_fliterR = false, count_flag = false;

    if (Flag.Left_Ring || Flag.Right_Ring)
    {
        Flag.CrossRoad_Find = false;
        Flag.CrossRoad_Block = false;
        return;
    }

    if (Lturnpoint_row != EFFECTIVE_ROW && Lturnpoint_row >= 16 && Lturnpoint_row <= 50)
    {
        short Endline_R = ImageH - 1;
        for (i = ImageH - 1; i >= EFFECTIVE_ROW; i--)
        {
            if ((i <= Lturnpoint_row + 5) && (!imageLine.Exist_Right[i] && !imageLine.Exist_Right[i - 1] && !imageLine.Exist_Right[i - 2]
                && !imageLine.Exist_Right[i - 3] && !imageLine.Exist_Right[i - 4] && !imageLine.Exist_Right[i - 5]))
                break;
            if (imageLine.Exist_Right[i] && imageLine.Exist_Right[i - 1] &&
                ABS(imageLine.Point_Right[i - 1] - imageLine.Point_Right[i]) <= 10)
            {
                for (j = i; j >= EFFECTIVE_ROW; j--)
                {
                    if (imageLine.Exist_Right[j] && imageLine.Exist_Right[j - 1] &&
                        ABS(imageLine.Point_Right[j - 1] - imageLine.Point_Right[j]) <= 10)
                        Endline_R = j - 1;
                    else
                        break;
                    if (Endline_R < Lturnpoint_row && imageLine.Point_Right[Endline_R] < Lturnpoint + (roadK * i / 3 + roadB / 3))
                        break;
                }
            }
            if (Endline_R != ImageH - 1)
                break;
        }
        if (!(Endline_R < Lturnpoint_row && imageLine.Point_Right[Endline_R] < Lturnpoint + roadK * i / 3 + roadB / 3))
        {
            if (kl_up > 0)
            {
                if (ABS(DeltaKl) >= limitKl)
                    flag_fliterL = true;
                if (ABS(DeltaKl) <= 5)
                {
                    if ((error_cnt_l >= 5) && (Lturnpoint < ImageW - 15))
                        flag_fliterL = false;
                }
            }
            if (kl_up == 0)
            {
                if (kl_down < 0 && Lturnpoint >= 5)
                    flag_fliterL = true;
            }
        }
    }
    if (Rturnpoint_row != EFFECTIVE_ROW && Rturnpoint_row >= 16 && Rturnpoint_row <= 50)
    {
        short Endline_L = ImageH - 1;
        for (i = ImageH - 10; i >= EFFECTIVE_ROW; i--)
        {
            if ((i <= Rturnpoint_row + 5) && (!imageLine.Exist_Left[i] && !imageLine.Exist_Left[i - 1] && !imageLine.Exist_Left[i - 2]
                && !imageLine.Exist_Left[i - 3] && !imageLine.Exist_Left[i - 4] && !imageLine.Exist_Left[i - 5]))
                break;
            if (imageLine.Exist_Left[i] && imageLine.Exist_Left[i - 1] &&
                ABS(imageLine.Point_Left[i - 1] - imageLine.Point_Left[i]) <= 10)
            {
                for (j = i; j >= EFFECTIVE_ROW; j--)
                {
                    if (imageLine.Exist_Left[j] && imageLine.Exist_Left[j - 1] &&
                        ABS(imageLine.Point_Left[j - 1] - imageLine.Point_Left[j]) <= 10)
                        Endline_L = j - 1;
                    else
                        break;
                    if (Endline_L < Rturnpoint_row && imageLine.Point_Left[Endline_L] > Rturnpoint - (roadK * i / 3 + roadB / 3))
                        break;
                }
            }
            if (Endline_L != ImageH - 1)
                break;
        }
        if (!(Endline_L < Rturnpoint_row && imageLine.Point_Left[Endline_L] > Rturnpoint - (roadK * i / 3 + roadB / 3)))
        {
            if (kr_up < 0)
            {
                if (ABS(DeltaKr) >= limitKr)
                    flag_fliterR = true;
                if (ABS(DeltaKr) <= 5)
                {
                    if ((error_cnt_r >= 5) && (Rturnpoint > 15))
                        flag_fliterR = false;
                }
            }
            if (kr_up == 0)
            {
                if (kr_down > 0 && Rturnpoint <= ImageW - 5)
                    flag_fliterR = true;
            }
        }
    }

    if (Lturnpoint_row != EFFECTIVE_ROW && Rturnpoint_row != EFFECTIVE_ROW
        && Lturnpoint_row >= 18 && Rturnpoint_row >= 18
        && Lturnpoint_row <= 45 && Rturnpoint_row <= 45)
    {
        if (ABS(Lturnpoint_row - Rturnpoint_row) <= 20 && ABS(Lturnpoint - Rturnpoint) >= 30)
        {
            for (i = Lturnpoint_row; i > EFFECTIVE_ROW; i--)
            {
                if (imageLine.Exist_Left[i] && imageLine.Point_Left[i] < Lturnpoint)
                    imageLine.Exist_Left[i] = false;
                if (!isWhite(i, Lturnpoint))
                    break;
            }
            for (i = Rturnpoint_row; i > EFFECTIVE_ROW; i--)
            {
                if (imageLine.Exist_Right[i] && imageLine.Point_Right[i] > Rturnpoint)
                    imageLine.Exist_Right[i] = false;
                if (!isWhite(i, Rturnpoint))
                    break;
            }
        }
        if (kl_down >= 0 && kr_down <= 0)
            return;
        if (kl_down < 0 && kr_down <= 0)
        {
            if ((Rturnpoint < ImageW - 10) && (kl_down > kr_down))
            {
                if ((Lturnpoint_row < Rturnpoint_row))
                {
                    flag_fliterR = true;
                }
            }
        }
        if (kl_down >= 0 && kr_down > 0)
        {
            if ((Lturnpoint > 10) && (kl_down < kr_down))
            {
                if ((Rturnpoint_row < Lturnpoint_row))
                {
                    flag_fliterL = true;
                }
            }
        }
        if (kl_down < 0 && kr_down > 0)
        {
            if (ABS(Lturnpoint_row - Rturnpoint_row) <= 10 && ABS(Lturnpoint - Rturnpoint) >= 30)
            {
                flag_fliterL = true;
                flag_fliterR = true;
            }
        }
        for (i = AIM_LINE + 1; i > AIM_LINE - 10; i--)
        {
            if (!(imageLine.Exist_Left[i] && imageLine.Exist_Right[i]))
            {
                count_flag = true;
                break;
            }
        }
        if (!count_flag)
        {
            flag_fliterL = false;
            flag_fliterR = false;
        }
    }
    //滤波补线
    if (flag_fliterL)
    {
        for (i = Lturnpoint_row - 1; i >= 0; i--)
        {
            imageLine.Exist_Left[i] = true;
            imageLine.Point_Left[i] = getLineValue(i, kl_down, bl_down);
            if (imageLine.Point_Right[i] < imageLine.Point_Left[i] + (roadK * i / 2 + roadB / 2) && !flag_fliterR)
                imageLine.Exist_Right[i] = false;
        }
    }
    if (flag_fliterR)
    {
        for (i = Rturnpoint_row - 1; i >= 0; i--)
        {
            imageLine.Exist_Right[i] = true;
            imageLine.Point_Right[i] = getLineValue(i, kr_down, br_down);
            if (imageLine.Point_Left[i] > imageLine.Point_Right[i] - (roadK * i / 2 + roadB / 2) && !flag_fliterL)
                imageLine.Exist_Left[i] = false;
        }
    }
}

/*计算中线*/
void trackMend_startPart(void)
{
    int leftLine_startPoint = 0;
    int rightLine_startPoint = 0;

    int leftLine_stopPoint = 0;
    int rightLine_stopPoint = 0;

    int i;

    float k_left, b_left;
    float k_right, b_right;

    short MendBasis_left[2][5];
    short MendBasis_right[2][5];
    int count = 0;

    //------------------------补左线-------------------------------
    //找左线起始点
    for (i = ImageH - 1; i >= (ImageH_6 ); i--)
    {
        if (imageLine.Exist_Left[i])
        {
            leftLine_startPoint = i;
            break;
        }
    }

    //------------------------补右线-------------------------------
    //找右线起始点
    for (i = ImageH - 1; i >= (ImageH_6 ); i--)
    {
        if (imageLine.Exist_Right[i])
        {
            rightLine_startPoint = i;
            break;
        }
    }

    if (imageLine.Point_Right[rightLine_startPoint] - imageLine.Point_Left[leftLine_startPoint] >= 5)    //防止在三叉的位置补下部分线
    {
        //当左起始超过2/3行时(图像底部看不到边线的部分有点多啦),补线
        if (leftLine_startPoint >10)
        {
            for (i = leftLine_startPoint; i > 0; i--)
            {
                if (imageLine.Exist_Left[i])
                {
                    MendBasis_left[0][count] = (short)i;
                    MendBasis_left[1][count] = (short)imageLine.Point_Left[i];
                    count++;
                }
                if (count == 5)    //有5个点即可开始补线
                {
                    leastSquareMethod(MendBasis_left[0], MendBasis_left[1], 5, &k_left, &b_left);
                    if (ABS(k_left) > 2 && i > ImageH_6)
                    {
                        count = 0;
                        continue;
                    }
                    //开始补线
                    for (i=i+5; i < ImageH; i++)
                    {
                         imageLine.Point_Left[i] = constrain_int16(getLineValue(i, k_left, b_left), 0, ImageW - 1);
                         imageLine.Exist_Left[i] = 1;
                    }
                    break;
                }
            }

        }
        //当右起始超过2/3行时(图像底部看不到边线的部分有点多啦),补线
        count = 0;
        if (rightLine_startPoint >10)
        {
            for (i = rightLine_startPoint; i > 0; i--)
            {
                if (imageLine.Exist_Right[i])
                {
                    MendBasis_right[0][count] = (short)i;
                    MendBasis_right[1][count] = (short)imageLine.Point_Right[i];
                    count++;
                }
                if (count == 5)    //有5个点即可开始补线
                {
                    leastSquareMethod(MendBasis_right[0], MendBasis_right[1], 5, &k_right, &b_right);
                    if (ABS(k_right) > 2 && i < ImageH_6)
                    {
                        count = 0;
                        continue;
                    }
                    //开始补线
                    for (i = i + 5; i < ImageH; i++)
                    {
                        imageLine.Point_Right[i] = constrain_int16(getLineValue(i, k_right, b_right), 0, ImageW - 1);
                        imageLine.Exist_Right[i] = 1;
                    }
                    break;
                }
            }

        }
    }
    //补十字
    for (i = leftLine_startPoint; i >= 4; i--)
   {
        if (!imageLine.Exist_Left[i])
        {
            leftLine_stopPoint = i+1;
            break;
        }
    }
    for (i = rightLine_startPoint; i >= 4; i--)
    {
        if (!imageLine.Exist_Right[i])
        {
            rightLine_stopPoint = i+1;
            break;
        }
    }
    if (leftLine_stopPoint > 8)
    {
        for (i = leftLine_stopPoint ;i > 4;i--)
        {
            if (RoadLeft(i) > 90 && RoadLeft(i + 1) > 90 && RoadLeft(i + 2) > 90)//至少三个全白行
                break;
        }
        if (i != 4)
        {
            for (i = leftLine_stopPoint - 1; i > 0; i--)
            {
                if (imageLine.Exist_Left[i] && imageLine.Exist_Left[i -1] && (ABS(imageLine.Exist_Left[i] - imageLine.Exist_Left[i-1]) <3))
                {
                    leftLine_stopPoint = i;
                    break;
                }
            }
            if (i != 0)
            {
                float k_Left = 0, b_Left = 0;
                k_Left = ((float)imageLine.Point_Left[leftLine_stopPoint] - (float)imageLine.Point_Left[leftLine_startPoint])
                    / ((float)leftLine_stopPoint - (float)leftLine_startPoint);
                b_Left = (float)imageLine.Point_Left[leftLine_startPoint] - k_Left * (leftLine_startPoint);
                for (i = leftLine_startPoint; i >= leftLine_stopPoint; i--)
                {
                    imageLine.Exist_Left[i] = true;
                    imageLine.Point_Left[i] = getLineValue(i, k_Left, b_Left);
                }
            }
        }

    }

    if (rightLine_stopPoint > 8)
    {
        for (i = rightLine_stopPoint;i > 4;i--)
        {
            if (RoadRight(i) > 90 && RoadRight(i + 1) > 90 && RoadRight(i + 2) > 90)//至少三个全白行
                break;
        }
        if (i != 4)
        {
            for (i = rightLine_stopPoint - 1; i > 0; i--)
            {
                if (imageLine.Exist_Right[i] && imageLine.Exist_Right[i -1] && (ABS(imageLine.Exist_Right[i] - imageLine.Exist_Right[i-1]) <3))
                {
                    rightLine_stopPoint = i;
                    break;
                }
            }
            if (i != 0)
            {

                float k_Right = 0, b_Right = 0;
                k_Right = ((float)imageLine.Point_Right[rightLine_stopPoint] - (float)imageLine.Point_Right[rightLine_startPoint])
                    / ((float)rightLine_stopPoint - (float)rightLine_startPoint);
                b_Right = (float)imageLine.Point_Right[rightLine_startPoint] - k_Right * (rightLine_startPoint);
                for (i = rightLine_startPoint; i >= rightLine_stopPoint; i--)
                {
                    imageLine.Exist_Right[i] = true;
                    imageLine.Point_Right[i] = getLineValue(i, k_Right, b_Right);
                }
            }
        }

    }
}

void trackMend_HalfPart(void)
{
    int8_t i = 0, j = 0;
    float err = 0, aveErr = 0, k_l = 0, b_l = 0, k_r = 0, b_r = 0;
    uint8_t count = 0;
    uint8_t centerCompensation = 0;
    bool loseL_flag = false, loseR_flag = false, loseL_First = true, loseR_First = true, recordL = true, recordR = true;
    short basis_length = 0, basis_row = 0, basis_length_2 = 0, basis_row_2 = 0;

    //(一) 两边丢线(没救了)
    if (imageLine.Lost_Left && imageLine.Lost_Right)
    {
        imageLine.Lost_Center = true;
    }

    //(二) 只丢左边线
    else if (imageLine.Lost_Left)
    {
        imageLine.Lost_Center = false;
        //1. 计算中线补偿
        for (i = ImageH - 1; i >= 0; i--)
        {
            if (imageLine.Exist_Right[i])
            {
                err += (((ImageW) / 2 + roadK * i / 2 + roadB / 2) - imageLine.Point_Right[i]);
                count++;
            }
        }
        //2. 计算平均误差
        aveErr = (float)(err / count);
        //3. 根据右边线补中线(路宽+中线补偿)
        if (count >= 5 && aveErr > 0)//点数足够且确定右边界线往左倾斜
            centerCompensation = LIMIT2MIN(aveErr, SingleLineLeanAveERR_MAX) / SingleLineLeanAveERR_MAX * SingleLineLeanK / 2;//补偿计算

        for (i = ImageH - 1; i >= 0; i--)
        {
            if (imageLine.Exist_Right[i])
            {
                //计算中线并限幅
                imageLine.Exist_Center[i] = true;
                amplitudeLIMIT(i, imageLine.Point_Right[i] - centerCompensation - roadK * i / 2 - roadB / 2 - (59 - i) * 0.15);
            }
        }
        limitCenter();//对得出的中线进行突变限幅
    }
    //(三) 只丢右边线
    else if (imageLine.Lost_Right)
    {
        imageLine.Lost_Center = false;
        //1. 计算中线补偿
        for (i = ImageH - 1; i >= 0; i--)
        {
            if (imageLine.Exist_Left[i])
            {
                err += (imageLine.Point_Left[i] - (ImageW / 2 - roadK * i / 2 - roadB / 2));
                count++;
            }
        }

        //2. 计算平均误差
        aveErr = (float)(err / count);
        //3. 根据左边线补中线(路宽+中线补偿)
        if (count >= 5 && aveErr > 0)//点数足够且确定右边界线往左倾斜
            centerCompensation = LIMIT2MIN(aveErr, SingleLineLeanAveERR_MAX) / SingleLineLeanAveERR_MAX * SingleLineLeanK / 2;//补偿计算

        for (i = ImageH - 1; i >= 0; i--)
        {
            if (imageLine.Exist_Left[i])
            {
                //计算中线并限幅
                imageLine.Exist_Center[i] = true;
                amplitudeLIMIT(i, imageLine.Point_Left[i] + centerCompensation + roadK * i / 2 + roadB / 2 + (59 - i) * 0.15);
            }
        }
        limitCenter();//对得出的中线进行突变限幅
    }
    //(四)两边都没有丢线
    else if (!imageLine.Lost_Left && !imageLine.Lost_Right)
    {
        imageLine.Lost_Center = false;
        for (i = ImageH - 1; i >= 0; i--)
        {
            if (imageLine.Exist_Left[i] && imageLine.Exist_Right[i])
            {
                if (imageLine.Point_Right[i] - imageLine.Point_Left[i] < MINRoadLen)
                    continue;
                else
                {
                    loseL_flag = false; loseR_flag = false; loseL_First = true;
                    loseR_First = true; recordL = true; recordR = true;
                    imageLine.Exist_Center[i] = true;
                    amplitudeLIMIT(i, (imageLine.Point_Left[i] + imageLine.Point_Right[i]) / 2);
                }
            }
            else if ((!imageLine.Exist_Left[i] && imageLine.Exist_Right[i]))       // && i < 40 ImageH / 2
            {
                if (loseL_First)
                {
                    if (imageLine.Exist_Left[MIN(i + 1, ImageH - 1)])
                    {
                        for (j = i - 1; j >= 2; j--)
                        {
                            if (imageLine.Exist_Left[j] && imageLine.Exist_Right[j] && imageLine.Exist_Right[j - 1]
                                && ABS(imageLine.Point_Left[j] - imageLine.Point_Left[j - 1]) < 5)
                            {
                                loseL_flag = false;
                                break;
                            }
                            loseL_flag = true;
                        }
                        loseL_First = false;
                    }
                }
                if (!loseL_First && loseL_flag)
                {
                    if (recordL)
                    {
                        basis_length = imageLine.Point_Right[i + 1] - imageLine.Point_Center[i + 1];
                        basis_row = i + 1;
                        recordL = false;
                    }
                    imageLine.Exist_Center[i] = true;
                    amplitudeLIMIT(i, (imageLine.Point_Right[i] - basis_length + (basis_row - i) * 0.3));
                }
                else if (!loseL_First && !loseL_flag)
                {
                    if (recordL)
                    {
                        basis_length = imageLine.Point_Right[i + 1] - imageLine.Point_Center[i + 1];
                        basis_row = i + 1;
                        basis_length_2 = imageLine.Point_Right[j] - (imageLine.Point_Left[j] + imageLine.Point_Right[j]) / 2;
                        basis_row_2 = j;
                        k_l = getLineK(basis_row, basis_length, basis_row_2, basis_length_2);
                        b_l = basis_length - k_l * basis_row;
                        recordL = false;
                    }
                    imageLine.Exist_Center[i] = true;
                    amplitudeLIMIT(i, imageLine.Point_Right[i] - (k_l * i + b_l));
                }
                else
                    amplitudeLIMIT(i, (imageLine.Point_Right[i] - (roadK * i + roadB) / 2));
                if (ABS(imageLine.Point_Center[i] - center) > 50)
                    break;
            }
            else if ((imageLine.Exist_Left[i] && !imageLine.Exist_Right[i]))       // && i < ImageH / 2
            {
                if (loseR_First)
                {
                    if (imageLine.Exist_Right[MIN(i + 1, ImageH - 1)])
                    {
                        for (j = i - 1; j >= 2; j--)
                        {
                            if (imageLine.Exist_Left[j] && imageLine.Exist_Right[j] &&
                                imageLine.Exist_Left[j - 1] && imageLine.Exist_Right[j - 1] &&
                                ABS(imageLine.Point_Right[j] - imageLine.Point_Right[j - 1]) < 5)
                            {
                                loseR_flag = false;
                                break;
                            }
                            loseR_flag = true;
                        }
                        loseR_First = false;
                    }
                }
                if (!loseR_First && loseR_flag)
                {
                    if (recordR)
                    {
                        basis_length = imageLine.Point_Center[i + 1] - imageLine.Point_Left[i + 1];
                        basis_row = i + 1;
                        recordR = false;
                    }
                    imageLine.Exist_Center[i] = true;
                    amplitudeLIMIT(i, (imageLine.Point_Left[i] + basis_length - (basis_row - i) * 0.3));
                }
                else if (!loseR_First && !loseR_flag)
                {
                    if (recordR)
                    {
                        basis_length = imageLine.Point_Center[i + 1] - imageLine.Point_Left[i + 1];
                        basis_row = i + 1;
                        basis_length_2 = (imageLine.Point_Left[j] + imageLine.Point_Right[j]) / 2 - imageLine.Point_Left[j];
                        basis_row_2 = j;
                        k_r = getLineK(basis_row, basis_length, basis_row_2, basis_length_2);
                        b_r = basis_length - k_r * basis_row;
                        recordR = false;
                    }
                    imageLine.Exist_Center[i] = true;
                    amplitudeLIMIT(i, imageLine.Point_Left[i] + (k_r * i + b_r));
                }
                else
                    amplitudeLIMIT(i, (imageLine.Point_Left[i] + (roadK * i + roadB) / 2));
                if (ABS(imageLine.Point_Center[i] - center) > 50)
                    break;
            }
            else continue;
        }
        limitCenter();
    }
}
/**************************************底层函数****************************************************/
/*判断是否为白点*/
bool isWhite(uint16_t row, uint16_t line)
{
    //出界判断
    if (!(row >= 0 && row < ImageH && line >= 0 && line < ImageW))
        return false;

    //判断白点黑点
    if (Pixle[row][line])
        return true;
    else
        return false;
}
/*判断是否为左右边界点*/
bool isLeftPoint(uint16_t i, uint16_t j)
{
    if (j < 2 || j >= ImageW - 3 || i<0 || i>ImageH - 1)//图像边缘
        return false;
    //右边一定不能出现蓝布
    if (((!isWhite(i, j)) || (!isWhite(i, j + 1)) || (!isWhite(i, j + 2)) || (!isWhite(i, j + 3)) || (!isWhite(i, j + 4)) || (!isWhite(i, j + 5))))
        return false;
    //左边一定不能出现路
    if (isWhite(i, j - 1) || isWhite(i, j - 2) || isWhite(i, j - 3) || isWhite(i, j - 4) || isWhite(i, j - 5))
        return false;
    if (j < 0 || j >= ImageW)// 列索引超出范围，直接返回无效
        return false;

    return true;
}
bool isRightPoint(uint16_t i, uint16_t j)
{
    if (j < 2 || j >= ImageW - 3 || i<0 || i>ImageH - 1)//图像边缘
        return false;
    //左边一定不能出现蓝布
    if (((!isWhite(i, j)) || (!isWhite(i, j - 1)) || (!isWhite(i, j - 2)) || (!isWhite(i, j - 3)) || (!isWhite(i, j - 4)) || (!isWhite(i, j - 5))))
        return false;
    //右边一定不能出现路
    if (isWhite(i, j + 1) || isWhite(i, j + 2) || isWhite(i, j + 3) || isWhite(i, j + 4) || isWhite(i, j + 5))
        return false;
    if (j < 0 || j >= ImageW)// 列索引超出范围，直接返回无效
        return false;

    return true;
}
/*边界线是否为直线 (EndLine 判断结束行)*/
bool LeftLine_Check(uint16_t EndLine)
{
    short i = 0, j = 0;
    short StartLine = 0;
    //检查拐点
    if (LTurnPoint_Row >= EndLine)
        return false;
    //找到左边线起始点
    for (i = ImageH - 1; i >= ImageH / 2; i--)
    {
        if (imageLine.Exist_Left[i])
        {
            StartLine = i;
            break;
        }
    }
    if (StartLine == 0)
        return false;
    //判断左线位置是否正常
    short LeftLineMAX = 0;
    for (i = StartLine; i > EndLine; i--)
    {
        if (imageLine.Exist_Left[i] && imageLine.Exist_Left[i - 1] && imageLine.Exist_Left[i - 2] &&
            (ABS((imageLine.Point_Left[i] + imageLine.Point_Left[i - 2]) / 2 - imageLine.Point_Left[i - 1]) < 5))
        {
            if (imageLine.Point_Left[i - 2] > LeftLineMAX)
                LeftLineMAX = imageLine.Point_Left[i - 2];
        }
        else
        {
            for (j = i - 3; j > EndLine; j--)
            {
                if (imageLine.Exist_Left[j] && (ABS(imageLine.Point_Left[j] - imageLine.Point_Left[i - 1]) < 10))
                {
                    i = j;
                    break;
                }
            }
        }
    }
    if (LeftLineMAX < 10 || LeftLineMAX >ImageW_2_3)
        return false;
    //判断左边界点的数量是否正常
    short LeftLine_Cnt = 0, e_cnt = 0;
    for (i = StartLine; i > EndLine; i--)
    {
        if (imageLine.Exist_Left[i] && imageLine.Exist_Left[i - 1] &&
            ABS(imageLine.Point_Left[i - 1] - imageLine.Point_Left[i]) <= 5)
            LeftLine_Cnt++;
        else
        {
            e_cnt++;
            if (e_cnt > 5)
                return false;
        }
    }
    if (LeftLine_Cnt < 4 * (StartLine - EndLine) / 5)
        return false;
    //判断左边线斜率是否正常
    float k_left = 0, b_left = 0;
    short Basis_left[2][5];
    short count = 0;
    for (i = EndLine; i < StartLine; i++)
    {
        if (imageLine.Exist_Left[i])
        {
            Basis_left[0][count] = (short)i;
            Basis_left[1][count] = (short)imageLine.Point_Left[i];
            count++;
        }
        if (count == 5)
            break;
    }
    if (count == 5)
    {
        leastSquareMethod(Basis_left[0], Basis_left[1], 5, &k_left, &b_left);
        if (k_left < -1.5 || k_left > 0.5)
            return false;
        else
            return true;
    }
    else
        return false;
}
bool RightLine_Check(uint16_t EndLine)
{
    short i = 0, j = 0;
    short StartLine = 0;
    //检查拐点
    if (RTurnPoint_Row >= EndLine)
        return false;
    //找到右边线起始点
    for (i = ImageH - 1; i >= ImageH / 2; i--)
    {
        if (imageLine.Exist_Right[i])
        {
            StartLine = i;
            break;
        }
    }
    if (StartLine == 0)
        return false;
    //判断右线位置是否正常
    short rightLineMAX = 94;
    for (i = StartLine; i > EndLine; i--)
    {
        if (imageLine.Exist_Right[i] && imageLine.Exist_Right[i - 1] && imageLine.Exist_Right[i - 2] &&
            (ABS((imageLine.Point_Right[i] + imageLine.Point_Right[i - 2]) / 2 - imageLine.Point_Right[i - 1]) < 5))
        {
            if (imageLine.Point_Right[i - 2] < rightLineMAX)
                rightLineMAX = imageLine.Point_Right[i - 2];
        }
        else
        {
            for (j = i - 3; j > EndLine; j--)
            {
                if (imageLine.Exist_Right[j] && (ABS(imageLine.Point_Right[j] - imageLine.Point_Right[i - 1]) < 10))
                {
                    i = j;
                    break;
                }
            }
        }
    }
    if (rightLineMAX < ImageW_3 || rightLineMAX > 84)
        return false;
    //判断右边界点的数量是否正常
    short RightLine_Cnt = 0, e_cnt = 0;
    for (i = StartLine; i > EndLine; i--)
    {
        if (imageLine.Exist_Right[i] && imageLine.Exist_Right[i - 1] &&
            ABS(imageLine.Point_Right[i - 1] - imageLine.Point_Right[i]) <= 5)
            RightLine_Cnt++;
        else
        {
            e_cnt++;
            if (e_cnt > 5)
                return false;
        }
    }
    if (RightLine_Cnt < 4 * (StartLine - EndLine) / 5)
        return false;
    //判断右边线斜率是否正常
    float k_right = 0, b_right = 0;
    short Basis_right[2][5];
    short count = 0;
    for (i = EndLine; i < StartLine; i++)
    {
        if (imageLine.Exist_Right[i])
        {
            Basis_right[0][count] = (short)i;
            Basis_right[1][count] = (short)imageLine.Point_Right[i];
            count++;
        }
        if (count == 5)
            break;
    }
    if (count == 5)
    {
        leastSquareMethod(Basis_right[0], Basis_right[1], 5, &k_right, &b_right);
        if (k_right > 1.5 || k_right < -0.5)
            return false;
        else
            return true;
    }
    else
        return false;
}
/*图像找拐点*/
void TurnPointL_Find(short* TurnPoint_Row, short* TurnPoint, float* kl_up, float* bl_up, float* kl_down, float* bl_down, uint8_t* error_cnt_l)
{
    uint8_t err_cnt = 0, StartLineL = 0;
    short i = 0, j = 0, k = 0;
    short BasisLine_Up[3] = { 0,0,0 }, BasisPoint_Up[3] = { 0,0,0 };
    short BasisLine_Down[3] = { 0,0,0 }, BasisPoint_Down[3] = { 0,0,0 };
    short BreakLine[2] = { 0,0 }, BreakPoint[2] = { 0,0 };
    bool flag_left2 = false, flag_enough = false, flag_right1 = false, flag_break = false, flag_err1 = false, flag_err2 = false;

    singlePoint_Filter();  //单独点滤去
    *TurnPoint_Row = EFFECTIVE_ROW; *TurnPoint = 0;
    *kl_up = 0; *bl_up = 0; *kl_down = 0; *bl_down = 0;
    *error_cnt_l = 0;
    if (!Flag.Left_Ring && !Flag.Right_Ring)
    {
        for (i = ImageH_6; i < ImageH; i++)
        {
            if (imageLine.Exist_Left[i])
            {
                StartLineL = i;
                break;
            }
        }
    }
    for (i = ImageH - 2; i >= ImageH_6; i--)
    {
        //45行以上连续6行没有边界点直接退出
        if ((i <= ImageH - 20) && (!imageLine.Exist_Left[i] && !imageLine.Exist_Left[i - 1] && !imageLine.Exist_Left[i - 2]
            && !imageLine.Exist_Left[i - 3] && !imageLine.Exist_Left[i - 4] && !imageLine.Exist_Left[i - 5]))
            break;
        //状态1：未找到逆趋势点。
        //任务：找到3个逆趋势点，并记录下来用于斜率判别
        //      或者找到断点
        if (!flag_break && !flag_left2 && imageLine.Exist_Left[i])
        {
            //先找1个逆趋势点
            for (j = i - 1; j >= MAX(i - 3, ImageH_6); j--)
            {
                if (!imageLine.Exist_Left[j] && !imageLine.Exist_Left[j - 1] && !imageLine.Exist_Left[j - 2])
                    break;
                if (imageLine.Exist_Left[j])
                {
                    if (imageLine.Point_Left[j] < imageLine.Point_Left[i])
                    {
                        BasisLine_Up[0] = i;
                        BasisPoint_Up[0] = imageLine.Point_Left[i];
                        BasisLine_Up[1] = j;
                        BasisPoint_Up[1] = imageLine.Point_Left[j];
                    }
                    break;
                }
            }
            //找到1个逆趋势点后，向上再找两个
            if (BasisLine_Up[0] != 0)
            {
                for (j = BasisLine_Up[0] - 1; j >= MAX(BasisLine_Up[0] - 7, ImageH_6); j--)
                {
                    if (!imageLine.Exist_Left[j] && !imageLine.Exist_Left[j - 1] && !imageLine.Exist_Left[j - 2])
                        break;
                    if (imageLine.Exist_Left[j])
                    {
                        for (k = j - 1; k >= MAX(j - 3, ImageH_6); k--)
                        {
                            if (imageLine.Exist_Left[k])
                            {
                                if (imageLine.Point_Left[k] < imageLine.Point_Left[j])
                                {
                                    BasisLine_Up[1] = j;
                                    BasisPoint_Up[1] = imageLine.Point_Left[j];
                                    BasisLine_Up[2] = k;
                                    BasisPoint_Up[2] = imageLine.Point_Left[k];
                                    flag_left2 = true;
                                }
                                if (imageLine.Point_Left[k] > imageLine.Point_Left[j])//找到3个逆趋势点之前出现趋势逆转，判定为假拐点
                                {
                                    flag_err1 = true;
                                    flag_left2 = false;
                                }
                                break;
                            }
                        }
                        if (flag_left2 || flag_err1)
                            break;
                    }
                }
                if (flag_left2)
                {
                    //检验逆趋势点上方是否为白
                    for (j = BasisLine_Up[0] - 1; j >= BasisLine_Up[0] - 3; j--)
                    {
                        if (!isWhite(j, BasisPoint_Up[0]))
                        {
                            flag_left2 = false;
                            break;
                        }
                    }
                }
                else
                {
                    if (flag_err1)
                    {
                        i = k + 1;
                        BasisLine_Up[0] = 0; BasisLine_Up[1] = 0; BasisLine_Up[2] = 0;
                        BasisPoint_Up[0] = 0; BasisPoint_Up[1] = 0; BasisPoint_Up[2] = 0;
                        flag_err1 = false; flag_left2 = false;
                        continue;
                    }
                    else
                    {
                        for (j = BasisLine_Up[1] - 1; j >= MAX(BasisLine_Up[1] - 10, ImageH_6); j--)
                        {
                            if (!imageLine.Exist_Left[j] && !imageLine.Exist_Left[j - 1] && !imageLine.Exist_Left[j - 2])
                            {
                                if (imageLine.Point_Left[i] < ImageW_2_3)
                                {
                                    BreakLine[1] = BasisLine_Up[0];
                                    BreakPoint[1] = BasisPoint_Up[0];
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            //没有找到逆趋势点，检查是否为断点
            else if (imageLine.Point_Left[i] < ImageW_2_3 && !imageLine.Exist_Left[i - 1] &&
                !imageLine.Exist_Left[i - 2] && !imageLine.Exist_Left[i - 3])
            {
                BreakLine[1] = i;
                BreakPoint[1] = imageLine.Point_Left[i];
            }
            //找到1个断点后，向上判断是否是白的
            if (BreakLine[1] != 0)
            {
                if (BreakLine[1] < StartLineL + 3)
                {
                    BreakLine[1] = 0;
                    BreakPoint[1] = 0;
                }
                if (BreakLine[1] != 0)
                {
                    for (j = BreakLine[1] - 1; j >= BreakLine[1] - 4; j--)
                    {
                        if (!isWhite(j, BreakPoint[1] + 1))
                        {
                            BreakLine[1] = 0;
                            BreakPoint[1] = 0;
                            break;
                        }
                    }
                }
                if (BreakLine[1] != 0)
                {
                    for (k = BreakLine[1]; k <= ImageH - 1; k++)
                    {
                        if (imageLine.Point_Left[k] == BreakPoint[1])
                        {
                            BreakLine[0] = k;
                            BreakPoint[0] = imageLine.Point_Left[k];
                        }
                        else
                            break;
                    }
                    flag_break = true;
                }
            }
        }
        //状态2：找到3个逆趋势点。
        //任务：在逆趋势点下方找到3个正趋势点，并记录下来用于斜率判别
        if (flag_left2 && !flag_right1)
        {
            //先找逆趋势点下方最下点
            for (j = BasisLine_Up[0]; j <= ImageH - 1; j++)
            {
                if (imageLine.Exist_Left[j])
                {
                    for (k = j + 1; k <= MIN(j + 3, ImageH - 1); k++)
                    {
                        if (imageLine.Exist_Left[k])
                        {
                            if (imageLine.Point_Left[k] < imageLine.Point_Left[j])
                            {
                                BasisLine_Down[0] = j;
                                BasisPoint_Down[0] = imageLine.Point_Left[j];
                                BasisLine_Down[1] = k;
                                BasisPoint_Down[1] = imageLine.Point_Left[k];
                            }
                            break;
                        }
                    }
                    if (BasisLine_Down[0] != 0)
                        break;
                }
            }
            //找到最下点后，向下找2个正趋势点
            if (BasisLine_Down[0] != 0)
            {
                for (j = BasisLine_Down[0] + 1; j <= MIN(BasisLine_Down[0] + 7, ImageH - 1); j++)
                {
                    if (!imageLine.Exist_Left[j] && !imageLine.Exist_Left[j - 1] && !imageLine.Exist_Left[j - 2])
                        break;
                    if (imageLine.Exist_Left[j])
                    {
                        for (k = j + 1; k <= MIN(j + 3, ImageH - 1); k++)
                        {
                            if (imageLine.Exist_Left[k])
                            {
                                if (imageLine.Point_Left[k] < imageLine.Point_Left[j])
                                {
                                    BasisLine_Down[1] = j;
                                    BasisPoint_Down[1] = imageLine.Point_Left[j];
                                    BasisLine_Down[2] = k;
                                    BasisPoint_Down[2] = imageLine.Point_Left[k];
                                    flag_right1 = true;
                                }
                                if (imageLine.Point_Left[k] > imageLine.Point_Left[j])//找到3个正趋势点之前出现趋势逆转，判定为假拐点
                                {
                                    flag_err2 = true;
                                    flag_right1 = false;
                                }
                                break;
                            }
                        }
                        if (flag_right1 || flag_err2)
                            break;
                    }
                }
            }
            if (!flag_right1)//没有找到3个正趋势点，清缓存，继续找
            {
                i = BasisLine_Up[2] + 1;
                BasisLine_Up[0] = 0; BasisLine_Up[1] = 0; BasisLine_Up[2] = 0;
                BasisPoint_Up[0] = 0; BasisPoint_Up[1] = 0; BasisPoint_Up[2] = 0;
                BasisLine_Down[0] = 0; BasisLine_Down[1] = 0; BasisLine_Down[2] = 0;
                BasisPoint_Down[0] = 0; BasisPoint_Down[1] = 0; BasisPoint_Down[2] = 0;
                flag_err1 = false; flag_err2 = false; flag_left2 = false; flag_right1 = false;
                continue;
            }
        }
        //状态3：找到断点。
        //任务：在断点下方找到3个正趋势点。
        if (flag_break && !flag_right1)
        {
            if (BreakLine[0] - BreakLine[1] > 5)
            {
                BasisLine_Down[0] = BreakLine[1];
                BasisPoint_Down[0] = BreakPoint[1];
            }
            else
            {
                BasisLine_Down[0] = BreakLine[0];
                BasisPoint_Down[0] = BreakPoint[0];
            }
            for (j = BreakLine[0] + 1; j <= ImageH - 1; j++)
            {
                if (!imageLine.Exist_Left[j] && !imageLine.Exist_Left[j - 1] && !imageLine.Exist_Left[j - 2])
                {
                    flag_err2 = true;
                    break;
                }
                if (imageLine.Exist_Left[j])
                {
                    BasisLine_Down[1] = j;
                    BasisPoint_Down[1] = imageLine.Point_Left[j];
                    break;
                }
            }
            if (BasisLine_Down[1] != 0)
            {
                for (j = BasisLine_Down[1]; j <= ImageH - 1; j++)
                {
                    if (!imageLine.Exist_Left[j] && !imageLine.Exist_Left[j - 1] && !imageLine.Exist_Left[j - 2] && !imageLine.Exist_Left[j - 3])
                    {
                        flag_err2 = true;
                        break;
                    }
                    if (imageLine.Exist_Left[j])
                    {
                        for (k = j + 1; k <= ImageH - 1; k++)
                        {
                            if (!imageLine.Exist_Left[k] && !imageLine.Exist_Left[k - 1] && !imageLine.Exist_Left[k - 2] && !imageLine.Exist_Left[k - 3])
                                break;
                            if (imageLine.Exist_Left[k])
                            {
                                if (BasisPoint_Down[1] > BasisPoint_Down[0])
                                {
                                    if (imageLine.Point_Left[k] > imageLine.Point_Left[j])
                                    {
                                        BasisLine_Down[2] = k;
                                        BasisPoint_Down[2] = imageLine.Point_Left[k];
                                        flag_enough = true;
                                    }
                                    if (imageLine.Point_Left[k] < imageLine.Point_Left[j])
                                        flag_err2 = true;
                                }
                                else
                                {
                                    if (imageLine.Point_Left[k] < imageLine.Point_Left[j])
                                    {
                                        BasisLine_Down[2] = k;
                                        BasisPoint_Down[2] = imageLine.Point_Left[k];
                                        flag_enough = true;
                                    }
                                    if (imageLine.Point_Left[k] > imageLine.Point_Left[j])
                                        flag_err2 = true;
                                }
                                break;
                            }
                        }
                        if (flag_enough || flag_err2)
                            break;
                    }
                }
            }
            else
            {
                BasisLine_Down[1] = BreakLine[0];
                BasisPoint_Down[1] = BreakPoint[0];
                BasisLine_Down[2] = BreakLine[0];
                BasisPoint_Down[2] = BreakPoint[0];
                flag_enough = true;
            }
            if (!flag_enough)
            {
                if (!flag_err2)
                {
                    BasisLine_Down[2] = BasisLine_Down[1];
                    BasisPoint_Down[2] = BasisPoint_Down[1];
                    flag_enough = true;
                }
                else
                    return;
            }
        }
        //状态4：找到3个逆趋势点，且其下方有3个正趋势点。
        //任务：返回上、下拐点(或断点)、k、b
        if (flag_left2 && flag_right1)
        {
            *TurnPoint_Row = BasisLine_Down[0];
            *TurnPoint = BasisPoint_Down[0];
            for (j = BasisLine_Up[1]; j > MAX(BasisLine_Up[1] - 10, ImageH_6); j--)
            {
                if (imageLine.Exist_Left[j])
                {
                    if (imageLine.Point_Left[j] == BasisPoint_Up[1])
                        BasisLine_Up[1] = j;
                    else
                        break;
                }
            }
            for (j = BasisLine_Up[2]; j > MAX(BasisLine_Up[2] - 10, ImageH_6); j--)
            {
                if (imageLine.Exist_Left[j])
                {
                    if (imageLine.Point_Left[j] == BasisPoint_Up[2])
                        BasisLine_Up[2] = j;
                    else
                        break;
                }
            }
            for (j = BasisLine_Down[1]; j < MIN(BasisLine_Down[1] + 10, ImageW); j++)
            {
                if (imageLine.Exist_Left[j])
                {
                    if (imageLine.Point_Left[j] == BasisPoint_Down[1])
                        BasisLine_Down[1] = j;
                    else
                        break;
                }
            }
            for (j = BasisLine_Down[2]; j < MIN(BasisLine_Down[2] + 10, ImageW); j++)
            {
                if (imageLine.Exist_Left[j])
                {
                    if (imageLine.Point_Left[j] == BasisPoint_Down[2])
                        BasisLine_Down[2] = j;
                    else
                        break;
                }
            }
            leastSquareMethod(BasisLine_Up, BasisPoint_Up, 3, kl_up, bl_up);
            leastSquareMethod(BasisLine_Down, BasisPoint_Down, 3, kl_down, bl_down);
            for (j = BasisLine_Down[2]; j > BasisLine_Up[2]; j--)
            {
                if (imageLine.Exist_Left[j] && imageLine.Exist_Left[j - 1] &&
                    ABS(imageLine.Point_Left[j] - imageLine.Point_Left[j - 1]) < 2)
                    err_cnt++;
            }
            *error_cnt_l = err_cnt;
            return;
        }
        //状态5：找到断点。
        //任务：返回断点、k、b
        if (flag_break && flag_enough)
        {
            if (BasisLine_Down[1] != BreakLine[0])
            {
                *TurnPoint_Row = BreakLine[1];
                *TurnPoint = BreakPoint[1];
                for (j = BasisLine_Down[1]; j < MIN(BasisLine_Down[1] + 10, ImageH); j++)
                {
                    if (imageLine.Exist_Left[j])
                    {
                        if (imageLine.Point_Left[j] == BasisPoint_Down[1])
                            BasisLine_Down[1] = j;
                        else
                            break;
                    }
                }
                for (j = BasisLine_Down[2]; j < MIN(BasisLine_Down[2] + 10, ImageH); j++)
                {
                    if (imageLine.Exist_Left[j])
                    {
                        if (imageLine.Point_Left[j] == BasisPoint_Down[2])
                            BasisLine_Down[2] = j;
                        else
                            break;
                    }
                }
                *kl_up = 0; *bl_up = 0;
                leastSquareMethod(BasisLine_Down, BasisPoint_Down, 3, kl_down, bl_down);
            }
            else
            {
                *TurnPoint_Row = BreakLine[1];
                *TurnPoint = BreakPoint[1];
                *kl_up = 0; *bl_up = 0; *kl_down = 0; *bl_down = BreakPoint[1];
            }
            return;
        }
    }
}

void TurnPointR_Find(short* TurnPoint_Row, short* TurnPoint, float* kr_up, float* br_up, float* kr_down, float* br_down, uint8_t* error_cnt_r)
{
    uint8_t err_cnt = 0, StartLineR = 0;
    short i = 0, j = 0, k = 0;
    short BasisLine_Up[3] = { 0,0,0 }, BasisPoint_Up[3] = { 0,0,0 };
    short BasisLine_Down[3] = { 0,0,0 }, BasisPoint_Down[3] = { 0,0,0 };
    short BreakLine[2] = { 0,0 }, BreakPoint[2] = { 0,0 };
    bool flag_left1 = false, flag_enough = false, flag_right2 = false, flag_break = false, flag_err1 = false, flag_err2 = false;

    singlePoint_Filter();  //单独点滤去
    *TurnPoint_Row = EFFECTIVE_ROW; *TurnPoint = 0;
    *kr_up = 0; *br_up = 0; *kr_down = 0; *br_down = 0;
    *error_cnt_r = 0;
    if (!Flag.Left_Ring && !Flag.Right_Ring)
    {
        for (i = ImageH_6; i < ImageH; i++)
        {
            if (imageLine.Exist_Right[i])
            {
                StartLineR = i;
                break;
            }
        }
    }
    for (i = ImageH - 2; i >= ImageH_6; i--)
    {
        //45行以上连续6行没有边界点直接退出
        if ((i <= ImageH - 20) && (!imageLine.Exist_Right[i] && !imageLine.Exist_Right[i - 1] && !imageLine.Exist_Right[i - 2]
            && !imageLine.Exist_Right[i - 3] && !imageLine.Exist_Right[i - 4] && !imageLine.Exist_Right[i - 5]))
            break;
        //状态1：未找到逆趋势点。
        //任务：找到3个逆趋势点，并记录下来用于斜率判别
        //      或者找到断点
        if (!flag_break && !flag_right2 && imageLine.Exist_Right[i])
        {
            //先找1个逆趋势点
            for (j = i - 1; j >= MAX(i - 7, ImageH_6); j--)
            {
                if (!imageLine.Exist_Right[j] && !imageLine.Exist_Right[j - 1] && !imageLine.Exist_Right[j - 2])
                    break;
                if (imageLine.Exist_Right[j])
                {
                    if (imageLine.Point_Right[j] > imageLine.Point_Right[i])
                    {
                        BasisLine_Up[0] = i;
                        BasisPoint_Up[0] = imageLine.Point_Right[i];
                        BasisLine_Up[1] = j;
                        BasisPoint_Up[1] = imageLine.Point_Right[j];
                    }
                    break;
                }
            }
            //找到1个逆趋势点后，向上再找两个
            if (BasisLine_Up[0] != 0)
            {
                for (j = BasisLine_Up[0] - 1; j >= MAX(BasisLine_Up[0] - 7, ImageH_6); j--)
                {
                    if (!imageLine.Exist_Right[j] && !imageLine.Exist_Right[j - 1] && !imageLine.Exist_Right[j - 2])
                        break;
                    if (imageLine.Exist_Right[j])
                    {
                        for (k = j - 1; k >= MAX(j - 3, ImageH_6); k--)
                        {
                            if (imageLine.Exist_Right[k])
                            {
                                if (imageLine.Point_Right[k] > imageLine.Point_Right[j])
                                {
                                    BasisLine_Up[1] = j;
                                    BasisPoint_Up[1] = imageLine.Point_Right[j];
                                    BasisLine_Up[2] = k;
                                    BasisPoint_Up[2] = imageLine.Point_Right[k];
                                    flag_right2 = true;
                                }
                                if (imageLine.Point_Right[k] < imageLine.Point_Right[j])//找到3个逆趋势点之前出现趋势逆转，判定为假拐点
                                {
                                    flag_err1 = true;
                                    flag_right2 = false;
                                }
                                break;
                            }
                        }
                        if (flag_right2 || flag_err1)
                            break;
                    }
                }
                if (flag_right2)
                {
                    for (j = BasisLine_Up[0] - 1; j >= BasisLine_Up[0] - 3; j--)
                    {
                        if (!isWhite(j, BasisPoint_Up[0]))
                        {
                            flag_right2 = false;
                            break;
                        }
                    }
                }
                else//没有找到三个逆趋势点，判断是否存在断点，如果没有则清缓存，继续找
                {
                    if (flag_err1)
                    {
                        i = k + 1;
                        BasisLine_Up[0] = 0; BasisLine_Up[1] = 0; BasisLine_Up[2] = 0;
                        BasisPoint_Up[0] = 0; BasisPoint_Up[1] = 0; BasisPoint_Up[2] = 0;
                        flag_err1 = false; flag_right2 = false;
                        continue;
                    }
                    else
                    {
                        for (j = BasisLine_Up[1] - 1; j >= MAX(BasisLine_Up[1] - 10, ImageH_6); j--)
                        {
                            if (!imageLine.Exist_Right[j] && !imageLine.Exist_Right[j - 1] && !imageLine.Exist_Right[j - 2])
                            {
                                if (imageLine.Point_Right[i] > ImageW_3)
                                {
                                    BreakLine[1] = i;
                                    BreakPoint[1] = imageLine.Point_Right[i];
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            //没有找到逆趋势点，检查是否为断点
            else if (imageLine.Point_Right[i] > ImageW_3 && !imageLine.Exist_Right[i - 1] &&
                !imageLine.Exist_Right[i - 2] && !imageLine.Exist_Right[i - 3])
                 {
                      BreakLine[1] = i;
                      BreakPoint[1] = imageLine.Point_Right[i];
                 }
            //找到1个断点后，向上判断是否是白的
            if (BreakLine[1] != 0)
            {
                if (BreakLine[1] < StartLineR + 3)
                {
                    BreakLine[1] = 0;
                    BreakPoint[1] = 0;
                }
                if (BreakLine[1] != 0)
                {
                    for (j = BreakLine[1] - 1; j >= BreakLine[1] - 4; j--)
                    {
                        if (!isWhite(j, BreakPoint[1] - 1))
                        {
                            BreakLine[1] = 0;
                            BreakPoint[1] = 0;
                            break;
                        }
                    }
                }
                if (BreakLine[1] != 0)
                {
                    for (k = BreakLine[1]; k <= ImageH - 1; k++)
                    {
                        if (imageLine.Point_Right[k] == BreakPoint[1])
                        {
                            BreakLine[0] = k;
                            BreakPoint[0] = imageLine.Point_Right[k];
                        }
                        else
                            break;
                    }
                    flag_break = true;
                }
            }
        }
        //状态2：找到3个逆趋势点。
        //任务：在逆趋势点下方找到3个正趋势点，并记录下来用于斜率判别
        if (flag_right2 && !flag_left1)
        {
            //先找逆趋势点下方最下点
            for (j = BasisLine_Up[0]; j <= ImageH - 1; j++)
            {
                if (imageLine.Exist_Right[j])
                {
                    for (k = j + 1; k <= MIN(j + 3, ImageH - 1); k++)
                    {
                        if (imageLine.Exist_Right[k])
                        {
                            if (imageLine.Point_Right[k] > imageLine.Point_Right[j])
                            {
                                BasisLine_Down[0] = j;
                                BasisPoint_Down[0] = imageLine.Point_Right[j];
                                BasisLine_Down[1] = k;
                                BasisPoint_Down[1] = imageLine.Point_Right[k];
                            }
                            break;
                        }
                    }
                    if (BasisLine_Down[0] != 0)
                        break;
                }
            }
            if (BasisLine_Down[0] != 0)
            {
                //找到最下点后，向下找2个正趋势点
                for (j = BasisLine_Down[0] + 1; j <= MIN(BasisLine_Down[0] + 7, ImageH - 1); j++)
                {
                    if (!imageLine.Exist_Right[j] && !imageLine.Exist_Right[j - 1] && !imageLine.Exist_Right[j - 2])
                        break;
                    if (imageLine.Exist_Right[j])
                    {
                        for (k = j + 1; k <= MIN(j + 3, ImageH - 1); k++)
                        {
                            if (imageLine.Exist_Right[k])
                            {
                                if (imageLine.Point_Right[k] > imageLine.Point_Right[j])
                                {
                                    BasisLine_Down[1] = j;
                                    BasisPoint_Down[1] = imageLine.Point_Right[j];
                                    BasisLine_Down[2] = k;
                                    BasisPoint_Down[2] = imageLine.Point_Right[k];
                                    flag_left1 = true;
                                }
                                if (imageLine.Point_Right[k] < imageLine.Point_Right[j])//找到3个正趋势点之前出现趋势逆转，判定为假拐点
                                {
                                    flag_err2 = true;
                                    flag_left1 = false;
                                }
                                break;
                            }
                        }
                        if (flag_left1 || flag_err2)
                            break;
                    }
                }
            }
            if (!flag_left1)//没有找到3个正趋势点，清缓存，继续找
            {
                i = BasisLine_Up[2] + 1;
                BasisLine_Up[0] = 0; BasisLine_Up[1] = 0; BasisLine_Up[2] = 0;
                BasisPoint_Up[0] = 0; BasisPoint_Up[1] = 0; BasisPoint_Up[2] = 0;
                BasisLine_Down[0] = 0; BasisLine_Down[1] = 0; BasisLine_Down[2] = 0;
                BasisPoint_Down[0] = 0; BasisPoint_Down[1] = 0; BasisPoint_Down[2] = 0;
                flag_err1 = false; flag_err2 = false; flag_right2 = false; flag_left1 = false;
                continue;
            }
        }
        //状态3：找到断点。
        //任务：在断点下方找到3个正趋势点。
        if (flag_break && !flag_left1)
        {
            if (BreakLine[0] - BreakLine[1] > 5)
            {
                BasisLine_Down[0] = BreakLine[1];
                BasisPoint_Down[0] = BreakPoint[1];
            }
            else
            {
                BasisLine_Down[0] = BreakLine[0];
                BasisPoint_Down[0] = BreakPoint[0];
            }
            for (j = BreakLine[0] + 1; j <= ImageH - 1; j++)
            {
                if (!imageLine.Exist_Right[j] && !imageLine.Exist_Right[j - 1] && !imageLine.Exist_Right[j - 2])
                {
                    flag_err2 = true;
                    break;
                }
                if (imageLine.Exist_Right[j])
                {
                    BasisLine_Down[1] = j;
                    BasisPoint_Down[1] = imageLine.Point_Right[j];
                    break;
                }
            }
            if (BasisLine_Down[1] != 0)
            {
                for (j = BasisLine_Down[1]; j <= ImageH - 1; j++)
                {
                    if (!imageLine.Exist_Right[j] && !imageLine.Exist_Right[j - 1] && !imageLine.Exist_Right[j - 2])
                    {
                        flag_err2 = true;
                        break;
                    }
                    if (imageLine.Exist_Right[j])
                    {
                        for (k = j + 1; k <= ImageH - 1; k++)
                        {
                            if (!imageLine.Exist_Right[k] && !imageLine.Exist_Right[k - 1] && !imageLine.Exist_Right[k - 2])
                                break;
                            if (imageLine.Exist_Right[k])
                            {
                                if (BasisPoint_Down[1] > BasisPoint_Down[0])
                                {
                                    if (imageLine.Point_Right[k] > imageLine.Point_Right[j])
                                    {
                                        BasisLine_Down[2] = k;
                                        BasisPoint_Down[2] = imageLine.Point_Right[k];
                                        flag_enough = true;
                                    }
                                    if (imageLine.Point_Right[k] < imageLine.Point_Right[j])
                                        flag_err2 = true;
                                }
                                else
                                {
                                    if (imageLine.Point_Right[k] < imageLine.Point_Right[j])
                                    {
                                        BasisLine_Down[2] = k;
                                        BasisPoint_Down[2] = imageLine.Point_Right[k];
                                        flag_enough = true;
                                    }
                                    if (imageLine.Point_Right[k] > imageLine.Point_Right[j])
                                        flag_err2 = true;
                                }
                                break;
                            }
                        }
                        if (flag_enough || flag_err2)
                            break;
                    }
                }
            }
            else
            {
                BasisLine_Down[1] = BreakLine[0];
                BasisPoint_Down[1] = BreakPoint[0];
                BasisLine_Down[2] = BreakLine[0];
                BasisPoint_Down[2] = BreakPoint[0];
                flag_enough = true;
            }
            if (!flag_enough)
            {
                if (!flag_err2)
                {
                    BasisLine_Down[2] = BasisLine_Down[1];
                    BasisPoint_Down[2] = BasisPoint_Down[1];
                    flag_enough = true;
                }
                else
                    return;
            }
        }
        //状态4：找到3个逆趋势点，且其下方有3个正趋势点。
        //任务：返回上、下拐点、k、b
        if (flag_right2 && flag_left1)
        {
            *TurnPoint_Row = BasisLine_Down[0];
            *TurnPoint = BasisPoint_Down[0];
            for (j = BasisLine_Up[1]; j > MAX(BasisLine_Up[1] - 10, ImageH_6); j--)
            {
                if (imageLine.Exist_Right[j])
                {
                    if (imageLine.Point_Right[j] == BasisPoint_Up[1])
                        BasisLine_Up[1] = j;
                    else
                        break;
                }
            }
            for (j = BasisLine_Up[2]; j > MAX(BasisLine_Up[2] - 10, ImageH_6); j--)
            {
                if (imageLine.Exist_Right[j])
                {
                    if (imageLine.Point_Right[j] == BasisPoint_Up[2])
                        BasisLine_Up[2] = j;
                    else
                        break;
                }
            }
            for (j = BasisLine_Down[1]; j < MIN(BasisLine_Down[1] + 10, ImageW); j++)
            {
                if (imageLine.Exist_Right[j])
                {
                    if (imageLine.Point_Right[j] == BasisPoint_Down[1])
                        BasisLine_Down[1] = j;
                    else
                        break;
                }
            }
            for (j = BasisLine_Down[2]; j < MIN(BasisLine_Down[2] + 10, ImageW); j++)
            {
                if (imageLine.Exist_Right[j])
                {
                    if (imageLine.Point_Right[j] == BasisPoint_Down[2])
                        BasisLine_Down[2] = j;
                    else
                        break;
                }
            }
            leastSquareMethod(BasisLine_Up, BasisPoint_Up, 3, kr_up, br_up);
            leastSquareMethod(BasisLine_Down, BasisPoint_Down, 3, kr_down, br_down);
            for (j = BasisLine_Down[2]; j > BasisLine_Up[2]; j--)
            {
                if (imageLine.Exist_Right[j] && imageLine.Exist_Right[j - 1] &&
                    ABS(imageLine.Point_Right[j] - imageLine.Point_Right[j - 1]) < 2)
                    err_cnt++;
            }
            *error_cnt_r = err_cnt;
            return;
        }
        //状态5：找到断点。
        //任务：返回断点、k、b
        if (flag_break && flag_enough)
        {
            if (BasisLine_Down[1] != BreakLine[0])
            {
                *TurnPoint_Row = BreakLine[1];
                *TurnPoint = BreakPoint[1];
                for (j = BasisLine_Down[1]; j < MIN(BasisLine_Down[1] + 10, ImageH); j++)
                {
                    if (imageLine.Exist_Right[j])
                    {
                        if (imageLine.Point_Right[j] == BasisPoint_Down[1])
                            BasisLine_Down[1] = j;
                        else
                            break;
                    }
                }
                for (j = BasisLine_Down[2]; j < MIN(BasisLine_Down[2] + 10, ImageH); j++)
                {
                    if (imageLine.Exist_Right[j])
                    {
                        if (imageLine.Point_Right[j] == BasisPoint_Down[2])
                            BasisLine_Down[2] = j;
                        else
                            break;
                    }
                }
                *kr_up = 0; *br_up = 0;
                leastSquareMethod(BasisLine_Down, BasisPoint_Down, 3, kr_down, br_down);
            }
            else
            {
                *TurnPoint_Row = BreakLine[1];
                *TurnPoint = BreakPoint[1];
                *kr_up = 0; *br_up = 0; *kr_down = 0; *br_down = BreakPoint[1];
            }
            return;
        }
    }
}


// **************************************************************十字**************************************
/*返回某行右半边或左半边的白点数*/
uint8_t road_right(uint8_t row)
{
    uint8_t i, white = 0;
    for (i = 47; i < ImageW; i++)
    {
        if (isWhite(row, i))
        {
            white++;
        }
        else
        {
            break;
        }
    }
    return white;
}
uint8_t road_left(uint8_t row)
{
    uint8_t i, white = 0;
    for (i = 47; i > 0; i--)
    {
        if (isWhite(row, i))
        {
            white++;
        }
        else
        {
            break;
        }
    }
    return white;
}


uint8_t Point_LeftRight(uint8_t row)//从右边开始找第一个左边界
{
    uint8 i = 0, j = 0;
    for (i = ImageW - 1;i > 0;i--)
    {
        if (isWhite(row, i))//右边第一个白点
            break;
    }
    for (j = i - 1;j < ImageW;j--)
    {
        if (!isWhite(row, j))
            break;

    }
    return j + 1;
}
uint8_t Point_RightLeft(uint8_t row)//从左边开始找第一个右边界
{
    uint8 white = 0, i = 0, j = 0;
    for (i = 0;i < ImageW;i++)
    {

        if (isWhite(row, i))//左边第一个白点
            break;
    }
    for (j = i + 1;j < ImageW;j++)
    {
        if (isWhite(row, j))
        {
            white++;
        }
        else
        {
            break;
        }
    }
    return white + i;
}
/***************************************************************************
 * 函数名称：RoadRight
 * 函数说明：从右边第一个黑点开始计算的宽度
 * 参数说明：row:行数
 * 函数返回：该行的右边路宽
 * 函数备注：*
 ***************************************************************************/

uint8_t RoadRight(uint8_t row)
{
    uint8 white = 0, i = 0, j = 0;
    for (i = ImageW - 1;i > 0;i--)
    {
        if (isWhite(row, i))//右边第一个白点
            break;
    }
    for (j = i;j < ImageW;j--)
    {
        if (isWhite(row, j))
        {
            white++;
        }
        else
        {
            break;
        }
    }
    return white;
}

/***************************************************************************
 * 函数名称：RoadLeft
 * 函数说明：从左边第一个白点开始计算的宽度
 * 参数说明：row:行数
 * 函数返回：该行的左边路宽
 * 函数备注：*
 ***************************************************************************/
int8_t RoadLeft(uint8_t row)
{
    uint8 white = 0, i = 0, j = 0;
    for (i = 0;i < ImageW;i++)
    {
        if (isWhite(row, i))//左边第一个白点
            break;
    }
    for (j = i;j < ImageW;j++)
    {
        if (isWhite(row, j))
        {
            white++;
        }
        else
        {
            break;
        }
    }
    return white;
}


/****************************************圆环******************************************************/
/****************************************新圆环******************************************************/
void Second_Defference(void) //二次差分判断是否有左、右弯道
{
    Right_Second_Difference_Count = 0;
    Right_Mutation_Count = 0;
    Mutation_Flag = false;
    Left_Second_Difference_Count = 0;
    Left_Mutation_Count = 0;
    Turn_Flag = false;
    Straight_Flag = false;
    for(i = 5; i < 45; i++)
    {
        Left_Second_Difference[i - 5] = imageLine.Point_Left[i + 10] - 2 * imageLine.Point_Left[i + 5] + imageLine.Point_Left[i];
        Right_Second_Difference[i - 5] = imageLine.Point_Right[i + 10] - 2 * imageLine.Point_Right[i + 5] + imageLine.Point_Right[i];
    }
    for(i = 5; i < 45; i++)
    {
        if(my_abs(Left_Second_Difference[i - 5]) >= 2)
        {
            Left_Second_Difference_Count++;
        }
        if(my_abs(Left_Second_Difference[i - 5]) >= 3)
        {
            Left_Mutation_Count ++;
        }
        if(my_abs(Right_Second_Difference[i - 5]) >= 2)
        {
            Right_Second_Difference_Count++;
        }
        if(my_abs(Right_Second_Difference[i - 5]) >= 3)
        {
            Right_Mutation_Count ++;
        }
     }
}

void Left_Island_detect()  //最终版的左圆环detect
{
    if (Right_Island_state || Left_Island_state)
    {
        return;
    }
    if (Left_Island_flag0)
    {
        In_Down_Point_Row = 0;
        In_Down_Point = 0;
        Inv_Point_Row[0] = 0;
        Inv_Point[0] = 0;
        Inv_Point_Row[1] = 0;
        Inv_Point[1] = 0;
        In_Down_Flag = false;
        if (Right_Second_Difference_Count <= 5 && Right_Mutation_Count <= 3 && (!right_inexist_number(40,35))){
            for (i = 50; i >= 4; i--)
            {
                //右边线连续 且出现下角点 且下角点上方连续丢左线
                for (j = i - 1; j > i - 3; j--)
                {
                    if (imageLine.Exist_Left[i]
                        && Rectangle_isWhite(imageLine.Point_Left[i], i - 3, 10, 5) //***********************************误判圆环了改**************************************
                        && (imageLine.Point_Left[i] >= imageLine.Point_Left[i + 1])
                        && (imageLine.Point_Left[i] > imageLine.Point_Left[j] + 8)
                        && (imageLine.Point_Left[j] > imageLine.Point_Left[j - 1]))
                    {
                        In_Down_Point_Row = i;
                        In_Down_Point = imageLine.Point_Left[i];
                        In_Down_Flag = true;
                        //printf("In_Down_Flag\n");
                        break;
                    }
                }
                if(In_Down_Flag)
                {
                    break;
                }
            }
        }
        if (In_Down_Flag)
        {
            for (i = In_Down_Point_Row - 2; i >= 4; i--)   //*********   In_Down_Point_Row - 4待修改
            {
                for (j = i - 1; j > i - 3; j--) // 找2个逆趋势点
                {
                    if (imageLine.Exist_Left[i] && imageLine.Exist_Left[j] && imageLine.Exist_Left[j - 1]
                        && (2 * imageLine.Point_Left[i] > imageLine.Point_Left[i+1] + imageLine.Point_Left[i+2])
                        && (imageLine.Point_Left[i] > imageLine.Point_Left[j])
                        && (imageLine.Point_Left[j] > imageLine.Point_Left[j - 1]))
                    {
                        Left_Island_flag1 = true;
                        Left_Island_flag0 = false;
                        Left_Island_state = true;
                        Search_In_Down_Point_Flag = true;
                        //printf("Left_island_flag1, Left_Island_state\n");
                        break;
                    }else{
                        In_Down_Flag = false;
                    }
                }
                if (Left_Island_flag1 && Left_Island_state)
                {
                    break;
                }
            }
        }
    }
}
void Left_Island_logic()   //最终版的左圆环logic
{
    if(Left_Island_flag1 && Left_Island_state)
    {
        if(Search_In_Down_Point_Flag){
            for(i = 57; i >= 6; i--)
            {
                In_Down_Point_Row = 0;
                In_Down_Point = 0;
                In_Down_Flag = false;
                for (j = i - 1; j > i - 3; j--) // 跟踪下角点
                {
                    if (imageLine.Exist_Left[i]
                        && (left_inexist_number(j, 10) >= 5)
                        && (imageLine.Point_Left[i] - imageLine.Point_Left[j] > 5)
                        && (imageLine.Point_Left[j] >= imageLine.Point_Left[j - 1]))
                    {
                        In_Down_Point_Row = i;
                        In_Down_Point = imageLine.Point_Left[i];
                        if (In_Down_Point_Row > 50)  //40
                        {
                            In_Down_Flag = false;
                            Search_In_Down_Point_Flag = false;
                            break;
                        }else{
                            In_Down_Flag = true;
                            Search_In_Down_Point_Flag = true;
                        }
                        break;
                    }else{
                        In_Down_Flag = false;
                        Search_In_Down_Point_Flag = true;
                    }
                }
                if(In_Down_Point_Row == i) break;
                //if(In_Down_Flag || !Search_In_Down_Point_Flag) break;
            }
        }
        else if(!Search_In_Down_Point_Flag)
        {
            for (i = In_Down_Point_Row - 10; i >= 15; i--)  //i的范围（49+）->15 （魔数49:下角点丢失的点 魔数15：防止逆趋势点找到远处，同时保证找到逆趋势点）
            {
                for (j = i - 1; j > i - 3; j--) // 找2个逆趋势点
                {
                    if (imageLine.Exist_Left[i] && imageLine.Exist_Left[j] && imageLine.Exist_Left[j - 1]
                        && (imageLine.Point_Left[i] > imageLine.Point_Left[j])
                        && (imageLine.Point_Left[j] > imageLine.Point_Left[j - 1]) + 1)
                    {
                        Inv_Point_Row[0] = i;
                        Inv_Point[0] = imageLine.Point_Left[i];
                        Inv_Point_Row[1] = j;
                        Inv_Point[1] = imageLine.Point_Left[j];
                        break;
                    }
                }
                if(Inv_Point_Row[0] == i) break;
            }
/*
            if(Inv_Point_Row[0])
            {
                In_Up_Point_Row = 0;
                In_Up_Point = 0;
                In_Up_Point_Flag = false;
                for (j = Inv_Point_Row[0] - 1; j >= 13; j--) //找上角点
                {
                    if (imageLine.Exist_Left[j] && imageLine.Exist_Left[j - 1]
                        && imageLine.Point_Left[j] > (Inv_Point[0] + 8)
                        && imageLine.Point_Left[j - 1] >= imageLine.Point_Left[j]
                        && imageLine.Point_Left[j] > (imageLine.Point_Left[j + 1] + 5))
                    {
                        In_Up_Point_Row = j;
                        In_Up_Point = imageLine.Point_Left[j];
                        In_Up_Point_Flag = true;
                        if(In_Up_Point_Flag && (In_Up_Point_Row > 45 || In_Up_Point > 75))
                        {
                            Left_Island_flag2 = true;
                            Left_Island_flag1 = false;
                            In_Up_Point_Flag = false;
                            //printf("Left_Island_flag2\n");
                            break;
                        }
                        break;
                    }
                }
                */
            if(Inv_Point_Row[0])
            {
                 for (j = Inv_Point_Row[0] - 10; j >= 13; j--) // 找上角点
                 {
                      if (imageLine.Point_Left[j] > (Inv_Point[0] + 7)
                         && imageLine.Point_Left[j - 1] >= imageLine.Point_Left[j]
                         && imageLine.Point_Left[j - 2] >= imageLine.Point_Left[j - 1])
                      {
                         In_Up_Point_Row = j;
                         In_Up_Point = imageLine.Point_Left[j];
                         In_Up_Point_Flag = true;
                          break;
                       }else
                       {
                           In_Up_Point_Flag = false;
                       }
                  }
            }
            if(In_Up_Point_Flag && (In_Up_Point_Row > 45 || In_Up_Point > 75))
            {
                  Left_Island_flag2 = true;
                  Left_Island_flag1 = false;
                  //printf("Left_Island_flag2\n");
            }
        }
}

    else if(Left_Island_flag2 && Left_Island_state)
    {
        for(i = 57; i >= 5; i--)
        {
            //if((imageLine.Point_Right[i] - imageLine.Point_Right[i+1] >=2) && (imageLine.Point_Right[i-1] - imageLine.Point_Right[i] >=1) && imageLine.Exist_Right[i] && imageLine.Exist_Right[i-1] && imageLine.Exist_Right[i+1])
            //if((imageLine.Point_Right[j] > imageLine.Point_Right[i]) &&(imageLine.Point_Right[j-1] >= imageLine.Point_Right[j])&&(imageLine.Point_Right[j-2] >= imageLine.Point_Right[j-1])&&(imageLine.Point_Right[j-3] >= imageLine.Point_Right[j-2])&&(imageLine.Point_Right[j-4] >= imageLine.Point_Right[j-3])) //imageLine.Exist_Right[j] &&
            if(imageLine.Exist_Right[i] && imageLine.Exist_Right[i-1] && imageLine.Exist_Right[i-2] && (right_inexist_number(i,20)>= 1)
               && (imageLine.Point_Right[i-1] - imageLine.Point_Right[i] >=2)
               && (imageLine.Point_Right[i+1] >= imageLine.Point_Right[i])
               && (imageLine.Point_Right[i-2] >= imageLine.Point_Right[i-1]))
            {
                Out_Down_Point_Row = i;
                Out_Down_Point = imageLine.Point_Right[i];
                Out_Down_Point_Flag = true;
                break;
            }
            if(Out_Down_Point_Flag && (right_inexist_number(59,4) >= 3))
                //if(Out_Down_Point_Flag && (Out_Down_Point_Row > 45 || Out_Down_Point > 70))
            {
                Left_Island_flag3 = true;
                Left_Island_flag2 = false;
                Out_Island_Left_Turn = true;//出圆环打角
                Out_Island_Turn_Counter = 0;
                printf("Left_Island_flag3\n");
                break;
            }
        }
    }
    else if(Left_Island_flag3 && Left_Island_state)
    {
        if(Out_Island_Transition_Ratio < 1 && Out_Island_Transition_Flag == false)
        {
            Left_Island_flag3 = false;
            //Left_Island_flag4 = true;
            Left_Island_state = false;
            island_flag_init();
            printf("Left_Island_flag4\n");
        }
    }
    else if(Left_Island_flag4 && Left_Island_state)
    {
        for(i = 30; i > 8; i--)  //8防止找到上面逆趋势点
        {
            for(j = i-1; j > i-2; j--)
            {
                if(imageLine.Exist_Left[i] && imageLine.Exist_Left[j])
                {
                    if(imageLine.Point_Left[j] <= imageLine.Point_Left[i]
                       && imageLine.Point_Left[j-1] <= imageLine.Point_Left[j]
                       && imageLine.Point_Left[i] <= imageLine.Point_Left[i + 1])
                    {
                        Out_Up_Point_Row = i;
                        Out_Up_Point = imageLine.Point_Left[i];
                        Out_Up_Point2_Row = i-4;
                        Out_Up_Point2 = imageLine.Point_Left[i-4];
                        Out_Up_Point_Flag = true;
                        break;
                    }
                }
            }
            if(Out_Up_Point_Flag && Out_Up_Point_Row > 40) {
                Left_Island_flag4 = false;
                printf("Left_Island_state = false\n");
                break;
            }
            if(Out_Up_Point_Row == i && Out_Up_Point == imageLine.Point_Left[i])
            {
                break;
            }
        }
    }

}
void Left_Island_update()  //最终版的左圆环update
{
    if(Left_Island_flag1 && Left_Island_state)
    {
        k_in = 0.75;
        if(In_Down_Flag && !In_Up_Point_Flag)
        {
            float k,b;
            short In_Down_Point_x[4] = {In_Down_Point_Row,In_Down_Point_Row + 1, In_Down_Point_Row + 2, In_Down_Point_Row + 3};
            short In_Down_Point_y[4] = {imageLine.Point_Left[In_Down_Point_Row],imageLine.Point_Left[In_Down_Point_Row  + 1], imageLine.Point_Left[In_Down_Point_Row  + 2], imageLine.Point_Left[In_Down_Point_Row  + 3]};
            leastSquareMethod(In_Down_Point_x, In_Down_Point_y, 4, &k, &b);
            for(i = In_Down_Point_Row; i >= 4; i--)
            {
                imageLine.Point_Left[i] = (int) constrain_int16(k * (i - In_Down_Point_Row) + In_Down_Point, 0, imageLine.Point_Right[i]);
            }
            for(j = ImageH-2; j >= 4; j--)
            {
                imageLine.Point_Center[j] = 0.5 * imageLine.Point_Left[j] + 0.5 * imageLine.Point_Right[j];   //0.5，0.5时入环前会靠左
                imageLine.Exist_Center[j] = true;
            }
            updateMediumLine();
        }
        else if(!In_Down_Flag && !In_Up_Point_Flag)
        {
            k_Left = ((float)10 - (float)Inv_Point[0]) / ((float)(ImageH - 1) - (float)Inv_Point_Row[0]);
            if(Inv_Point_Row[0] > 3)
            {
                for (i = Inv_Point_Row[0]; i <= ImageH - 1; i++)
                {
                    imageLine.Point_Left[i] = (int) constrain_int16(k_Left * (i - Inv_Point_Row[0]) + Inv_Point[0], 0, imageLine.Point_Right[i]);
                }
                for(j = ImageH-2; j > 10; j--)
                {
                    imageLine.Point_Center[j] = 0.5 * imageLine.Point_Left[j] + 0.5 * imageLine.Point_Right[j];
                    imageLine.Exist_Center[j] = true;
                }
            }
            updateMediumLine();
        }
        else if(In_Up_Point_Flag && !In_Down_Flag)
        {
            for(j = In_Up_Point_Row; j < ImageH; j++)
            {
                imageLine.Point_Right[j] = (int) constrain_int16(k_in * (j - In_Up_Point_Row) + In_Up_Point, imageLine.Point_Left[j], ImageW-1);
            }
            for(j = ImageH - 1; j >= 0; j--)
            {
                imageLine.Point_Left[j] = 0;
                imageLine.Exist_Left[j] = false;
            }
            for(j = ImageH-2; j > 10; j--)
            {
                imageLine.Point_Center[j] = 0.5 * imageLine.Point_Left[j] + 0.5 * imageLine.Point_Right[j];
                imageLine.Exist_Center[j] = true;
            }
            updateMediumLine();
        }
        else
        {
            doMend();
            updateMediumLine();
        }
    }
    else if(Left_Island_flag2 && Left_Island_state)
    {
        k_out = 2.5;
        if(Out_Down_Point_Flag)
        {
            for(j = Out_Down_Point_Row; j >= 0; j--)
            {
                imageLine.Point_Right[j] = (int) constrain_int16(k_out * (j - Out_Down_Point_Row) + Out_Down_Point, imageLine.Point_Left[j], ImageW-1);
            }
            for(j = ImageH-2; j > 10; j--)
            {
                imageLine.Point_Center[j] = 0.5 * imageLine.Point_Left[j] + 0.5 * imageLine.Point_Right[j];
                imageLine.Exist_Center[j] = true;
            }
            for(j = ImageH - 1; j >= 0; j--)
            {
                imageLine.Point_Left[j] = 0;
                imageLine.Exist_Left[j] = false;
            }
            updateMediumLine();
        }
        else
        {
            for(j = ImageH - 1; j >= 0; j--)
            {
                imageLine.Point_Left[j] = 0;
                imageLine.Exist_Left[j] = false;
            }
            for(j = ImageH-2; j > 10; j--)
            {
                imageLine.Point_Center[j] = 0.5 * imageLine.Point_Left[j] + 0.5 * imageLine.Point_Right[j];
                imageLine.Exist_Center[j] = true;
            }
            updateMediumLine();
        }
    }
    else if(Left_Island_flag3 && Left_Island_state)
    {
        if(Out_Island_Left_Turn)
        {
            servo_param.Servo_filter = FIXED_LEFT_ANGLE;//固定打角
            Out_Island_Turn_Counter++;
            if(Out_Island_Turn_Counter > OUT_ISLAND_TURN_TIME)
            {
                Out_Island_Transition_Flag = false;
                Out_Island_Left_Turn = false;
                Out_Island_Transition_Ratio = 0.0f;
            }
        }
    }
    else if(Left_Island_flag4 && Left_Island_state)
    {
        k_Left = ((float)Out_Up_Point2 - (float)Out_Up_Point) / ((float)(Out_Up_Point2_Row - (float)Out_Up_Point_Row));
        if(Out_Up_Point_Flag)
        {
            for (i = Out_Up_Point_Row; i <= ImageH - 1; i++)
            {
                imageLine.Point_Left[i] = (int) constrain_int16(k_Left * (i - Out_Up_Point_Row)  + Out_Up_Point, 0, imageLine.Point_Right[i]);
            }
            for(j = ImageH-2; j > 10; j--)
            {
                imageLine.Point_Center[j] = 0.5 * imageLine.Point_Left[j] + 0.5 * imageLine.Point_Right[j];
                imageLine.Exist_Center[j] = true;
            }
            updateMediumLine();
        }
        else
        {
            doMend();
            updateMediumLine();
        }
    }
}

void Right_Island_detect1()
{
    if (Right_Island_state || Left_Island_state)
    {
         return;
    }
    if (Right_Island_flag0)
    {
         In_Down_Point_Row = 0;
         In_Down_Point = 0;
         Inv_Point_Row[0] = 0;
         Inv_Point[0] = 0;
         Inv_Point_Row[1] = 0;
         Inv_Point[1] = 0;
         In_Down_Flag = false;
         if(Left_Second_Difference_Count <= 5 && Left_Mutation_Count <= 3 && (!left_inexist_number(40,35)))
         {
             for (i = 50; i >= 4; i--)
             {
                 //左边线连续 且出现下角点 且下角点上方连续丢左线
                 for (j = i - 1; j > i - 3; j--)
                 {
                      if (imageLine.Exist_Right[i]
                          && Rectangle_isWhite(imageLine.Point_Right[i] + 10, i - 3, 10, 5)
                          && (imageLine.Point_Right[i + 1] >= imageLine.Point_Right[i])
                          && (imageLine.Point_Right[i] < (imageLine.Point_Right[j] - 8))  //与左不同
                          && (imageLine.Point_Right[j] < imageLine.Point_Right[j - 1]))
                      {
                          In_Down_Point_Row = i;
                          In_Down_Point = imageLine.Point_Right[i];
                          In_Down_Flag = true;
                          printf("In_Down_Flag\n");
                          break;
                      }
                 }
                 if(In_Down_Flag)
                 {
                     break;
                 }
             }
         }
         if (In_Down_Flag)
         {
              for (i = In_Down_Point_Row - 2; i >= 4; i--)   //*********   In_Down_Point_Row - 4待修改
              {
                  for (j = i - 1; j > i - 3; j--) // 找2个逆趋势点
                  {
                       if (imageLine.Exist_Right[i + 1] && imageLine.Exist_Right[i] && !imageLine.Exist_Right[j]
                           && (imageLine.Point_Right[i] <= imageLine.Point_Right[i + 1])
                           && (imageLine.Point_Right[i] < (imageLine.Point_Right[j] - 5)))
                       {
                           Right_Island_flag1 = true;
                           Right_Island_flag0 = false;
                           Right_Island_state = true;
                           Search_In_Down_Point_Flag = true;
                           printf("Left_island_flag1, Left_Island_state\n");
                           break;
                        }else{
                            In_Down_Flag = false;
                        }
                  }
                  if (Right_Island_flag1 && Right_Island_state)
                  {
                       break;
                  }
              }
         }
    }
}
void Right_Island_logic1()
{
    if(Right_Island_flag1 && Right_Island_state)
    {
        if (Search_In_Down_Point_Flag)
        {
            for(i = ImageH - 2; i >= 4; i--)
            {
                In_Down_Point_Row = 0;
                In_Down_Point = 0;
                In_Down_Flag = false;
                for (j = i - 1; j > i - 5; j--) // 跟踪下角点
                {
                     if (imageLine.Exist_Right[i] && !imageLine.Exist_Right[j-3] && !imageLine.Exist_Right[j - 4]
                         && (right_inexist_number(j, 10) >= 5)
                         && imageLine.Point_Right[i] < (imageLine.Point_Right[j] - 5)
                         && imageLine.Point_Right[j] <= imageLine.Point_Right[j - 1])
                     {
                         In_Down_Point_Row = i;
                         In_Down_Point = imageLine.Point_Right[i];
                         In_Down_Flag = true;
                         if (In_Down_Point_Row > 53)          //*********53待修改
                         {
                             In_Down_Flag = false;
                             Search_In_Down_Point_Flag = false;
                             break;
                         }
                         break;
                      }
                  }
                  if(In_Down_Flag) break;
              }
          }
          if(!In_Down_Flag)
          {
               if(!In_Up_Point_Flag)   //如果没有找到上角点，则继续找逆趋势点
               {
                    for (i = In_Down_Point_Row - 1; i >= 15; i--)
                    {
                        for(j = i - 1; j > i - 3; j--)
                        {
                            if(imageLine.Exist_Right[i]
                               &&(imageLine.Point_Right[i] <= imageLine.Point_Right[j])
                               && (imageLine.Point_Right[j] <= imageLine.Point_Right[j-1]))
                            {
                                Inv_Point_Row[0] = i;
                                Inv_Point[0] = imageLine.Point_Right[i];
                                Inv_Point_Row[1] = j;
                                Inv_Point[1] = imageLine.Point_Right[j];
                                break;
                             }
                        }
                        if(Inv_Point_Row[0] == i) break;
                     }
               }
               if(Inv_Point_Row[0])   //如果找到逆趋势点，则找上角点
               {
                   In_Up_Point_Row = 0;
                   In_Up_Point = 0;
                   In_Up_Point_Flag = false;
                   for(j = Inv_Point_Row[0] - 1; j >= 12; j--)   //找上角点， 魔数12：决定上角点找的时机，越大越晚看到
                   {
                        if(imageLine.Exist_Right[j] && imageLine.Exist_Right[j - 1]
                           && imageLine.Point_Right[j] < (Inv_Point[0] - 5)
                           && imageLine.Point_Right[j-1] <= imageLine.Point_Right[j]
                           && imageLine.Point_Right[j-2] <= imageLine.Point_Right[j-1])
                        {
                           In_Up_Point_Row = j;
                           In_Up_Point = imageLine.Point_Right[j];
                           In_Up_Point_Flag = true;
                           if(In_Up_Point_Flag && (In_Up_Point_Row > 45 || In_Up_Point < 14))
                           {
                                Right_Island_flag2 = true;
                                Right_Island_flag1 = false;
                                printf("Right_Island_flag2\n");
                                break;
                            }
                            break;
                        }
                        else
                        {
                            In_Up_Point_Flag = false;
                        }
                    }
                }
            }
        }
    else if(Right_Island_flag2 && Right_Island_state)
    {
          for(i = ImageH - 2; i >= 0; i--)
          {
                    /*if(imageLine.Point_Left[i] && imageLine.Point_Left[i-1] && imageLine.Point_Left[i-2] && (imageLine.Exist_Left[4] + imageLine.Exist_Left[5] + imageLine.Exist_Left[6] == 0)
                        && (imageLine.Point_Left[i] - imageLine.Point_Left[i-1] >= 2)
                        && (imageLine.Point_Left[i] - imageLine.Point_Left[i+1] >= 0)
                        && (imageLine.Point_Left[i-1] >= imageLine.Point_Left[i-2])
                        && (2*imageLine.Point_Left[i+1] - imageLine.Point_Left[i+2]- imageLine.Point_Left[i+3] >= 2)
                        && (2*imageLine.Point_Left[i] - imageLine.Point_Left[i-1] - imageLine.Point_Left[i+1] >= 2)
                        )*/
               if(imageLine.Exist_Left[i] && imageLine.Exist_Left[i-1] && imageLine.Exist_Left[i-2] && (left_inexist_number(i,20) >= 1)
                  && (imageLine.Point_Left[i] >= (imageLine.Point_Left[i-1] + 2))
                  && (imageLine.Point_Left[i] >= imageLine.Point_Left[i+1])  //角点下方斜率
                  && (imageLine.Point_Left[i-1] >= imageLine.Point_Left[i-2])) //角点上方平锯齿
               {
                    Out_Down_Point_Row = i;
                    Out_Down_Point = imageLine.Point_Left[i];
                    Out_Down_Point_Flag = true;
                    break;
               }
               if(Out_Down_Point_Flag &&(left_inexist_number(59,15)>= 3))
                  //if(Out_Down_Point_Flag && (Out_Down_Point_Row > 45 || Out_Down_Point < 15))
                  //if(Out_Down_Point_Flag && (imageLine.Exist_Left[ImageH - 1] + imageLine.Exist_Left[ImageH - 2] + imageLine.Exist_Left[ImageH - 3]+ imageLine.Exist_Left[ImageH - 4]+ imageLine.Exist_Left[ImageH - 5] == 0) )
               {
                     Right_Island_flag3 = true;
                     Right_Island_flag2 = false;
                     Out_Island_Right_Turn = true;
                     Out_Island_Turn_Counter = 0;
                     printf("Right_Island_flag3\n");
                     break;
               }
            }
    }
    else if(Right_Island_flag3 && Right_Island_state)
    {
         if(Out_Island_Transition_Ratio < 1 && Out_Island_Transition_Flag == false)
         {
               Right_Island_flag3 = false;
               //Right_Island_flag4 = true;
               Right_Island_state = false;
               island_flag_init();
               printf("Right_Island_flag4\n");
          }
    }
    else if(Right_Island_flag4 && Right_Island_state)
    {
            for(i = 30; i > 8; i--)
            {
                for(j = i - 1; j > i-2; j--)
                {
                    if(imageLine.Exist_Right[i] && imageLine.Exist_Right[j])
                    {
                        if(imageLine.Point_Right[j] >= imageLine.Point_Right[i]
                           && imageLine.Point_Right[j - 1] >= imageLine.Point_Right[j]
                           && imageLine.Point_Right[i] >= imageLine.Point_Right[i + 1])
                        {
                            Out_Up_Point_Row = i;
                            Out_Up_Point = imageLine.Point_Right[i];
                            Out_Up_Point2_Row = i-4;
                            Out_Up_Point2 = imageLine.Point_Right[i-4];
                            Out_Up_Point_Flag = true;
                            break;
                        }
                    }
                }
                if(Out_Up_Point_Flag && Out_Up_Point_Row > 40)
                {
                    Right_Island_flag4 = false;
                    printf("Right_Island_state = false\n");
                    break;
                }
                if(Out_Up_Point_Row == i && Out_Up_Point == imageLine.Point_Right[i])
                {
                    break;
                }
            }
        }
     }
void Right_Island_update1()
    {
        if(Right_Island_flag1 && Right_Island_state)
        {
            if(In_Down_Flag && !In_Up_Point_Flag)
            {
                k_Right = ((float)imageLine.Point_Right[In_Down_Point_Row] - (float)imageLine.Point_Right[In_Down_Point_Row + 5])
                          / ((float)In_Down_Point_Row - (float)(In_Down_Point_Row + 5));
                for(i = In_Down_Point_Row; i > 6; i--)
                {
                    imageLine.Point_Right[i] = (int) constrain_int16(k_Right * (i - In_Down_Point_Row) + In_Down_Point, imageLine.Point_Left[i], ImageW - 1);
                }
            }
            else if(!In_Down_Flag && !In_Up_Point_Flag)
            {
                k_Right = ((float)84 - (float)Inv_Point[0])
                          / ((float)(ImageH - 1) - (float)Inv_Point_Row[0]);
                if(Inv_Point_Row[0] > 3)
                {
                    for (i = Inv_Point_Row[0]; i <= ImageH - 1; i++)
                    {
                        imageLine.Point_Right[i] = (int) constrain_int16(k_Right * (i - Inv_Point_Row[0]) + Inv_Point[0], imageLine.Point_Left[i], ImageW - 1);
                    }
                }
            }
            k_in = -0.75;
            if(In_Up_Point_Flag && !In_Down_Flag)
            {
                for(j = In_Up_Point_Row; j < ImageH; j++)
                {
                    imageLine.Point_Left[j] = (int) constrain_int16(k_in * (j - In_Up_Point_Row) + In_Up_Point, 0, imageLine.Point_Right[j]);
                }
                for(j = ImageH - 1; j >= 0; j--)
                {
                    imageLine.Point_Right[j] = 93;
                    imageLine.Exist_Right[j] = false;
                }
                for(j = ImageH-2; j > 10; j--)
                {
                    imageLine.Point_Center[j] = 0.5 * imageLine.Point_Right[j] + 0.5 * imageLine.Point_Left[j];
                    imageLine.Exist_Center[j] = true;
                }
                updateMediumLine();
            }
            else
            {
                doMend();
                updateMediumLine();
            }
        }
        else if(Right_Island_flag2 && Right_Island_state)
        {
            k_out = -2.5;
            if(Out_Down_Point_Flag)
            {
                for(j = Out_Down_Point_Row; j >= 0; j--)
                {
                    imageLine.Point_Left[j] = (int) constrain_int16(k_out * (j - Out_Down_Point_Row) + Out_Down_Point, 0, imageLine.Point_Right[j]);
                }
                for(j = ImageH-2; j > 10; j--)
                {
                    imageLine.Point_Center[j] = 0.5 * imageLine.Point_Right[j] + 0.5 * imageLine.Point_Left[j];
                    imageLine.Exist_Center[j] = true;
                }
                for(j = ImageH - 1; j >= 0; j--)
                {
                    imageLine.Point_Right[j] = 93;
                    imageLine.Exist_Right[j] = false;
                }
                updateMediumLine();
            }
            else
            {
                for(j = ImageH - 1; j >= 0; j--)
                {
                    imageLine.Point_Right[j] = 93;
                    imageLine.Exist_Right[j] = false;
                }
                for(j = ImageH-2; j > 10; j--)
                {
                    imageLine.Point_Center[j] = 0.5 * imageLine.Point_Left[j] + 0.5 * imageLine.Point_Right[j];
                    imageLine.Exist_Center[j] = true;
                }
                updateMediumLine();
            }
        }
        else if(Right_Island_flag3 && Right_Island_state)
        {
            if(Out_Island_Right_Turn)
            {
                servo_param.Servo_filter = FIXED_RIGHT_ANGLE;
                Out_Island_Turn_Counter++;
                if(Out_Island_Turn_Counter > OUT_ISLAND_TURN_TIME)
                {
                    Out_Island_Right_Turn = false;
                    Out_Island_Transition_Ratio = 0.0f;
                    Out_Island_Transition_Flag = false;
                }
            }
        }
        else if(Left_Island_flag4 && Left_Island_state)
        {
            k_Right = ((float)Out_Up_Point2 - (float)Out_Up_Point) / ((float)(Out_Up_Point2_Row - (float)Out_Up_Point_Row));
            if(Out_Up_Point_Flag)
            {
                for (i = Out_Up_Point_Row; i <= ImageH - 1; i++)
                {
                    imageLine.Point_Right[i] = (int) constrain_int16(k_Right * (i - Out_Up_Point_Row)  + Out_Up_Point, imageLine.Point_Left[i], ImageW-1);
                }
                for(j = ImageH-2; j > 10; j--)
                {
                    imageLine.Point_Center[j] = 0.5 * imageLine.Point_Left[j] + 0.5 * imageLine.Point_Right[j];
                    imageLine.Exist_Center[j] = true;
                }
                updateMediumLine();
            }
            else
            {
                doMend();
                updateMediumLine();
            }
        }

    }

void Right_Island_detect()  //最终版的右圆环detect
{
    if (Right_Island_state || Left_Island_state)
    {
        return;
    }
    if (Right_Island_flag0)
    {
        In_Down_Point_Row = 0;
        In_Down_Point = 0;
        Inv_Point_Row[0] = 0;
        Inv_Point[0] = 0;
        Inv_Point_Row[1] = 0;
        Inv_Point[1] = 0;
        In_Down_Flag = false;
        if(Left_Second_Difference_Count <= 5 && Left_Mutation_Count <= 3 && (!left_inexist_number(40,35)))
        {
            for (i = 50; i >= 4; i--)
            {
                //左边线连续 且出现下角点 且下角点上方连续丢左线
                for (j = i - 1; j > i - 3; j--)
                {
                    if (imageLine.Exist_Right[i]
                        && Rectangle_isWhite(imageLine.Point_Right[i] + 10, i - 3, 10, 5)
                        && (imageLine.Point_Right[i + 1] >= imageLine.Point_Right[i])
                        && (imageLine.Point_Right[i] < (imageLine.Point_Right[j] - 8))  //与左不同
                        && (imageLine.Point_Right[j] < imageLine.Point_Right[j - 1]))
                    {
                        In_Down_Point_Row = i;
                        In_Down_Point = imageLine.Point_Right[i];
                        In_Down_Flag = true;
                        printf("In_Down_Flag\n");
                        break;
                    }
                }
                if(In_Down_Flag)
                {
                    break;
                }
            }
        }
        if (In_Down_Flag)
        {
            for (i = In_Down_Point_Row - 2; i >= 4; i--)   //*********   In_Down_Point_Row - 4待修改
            {
                for (j = i - 1; j > i - 3; j--) // 找2个逆趋势点
                {
                    if (imageLine.Exist_Right[i + 1] && imageLine.Exist_Right[i] && !imageLine.Exist_Right[j]
                        && (imageLine.Point_Right[i] <= imageLine.Point_Right[i + 1])
                        && (imageLine.Point_Right[i] < (imageLine.Point_Right[j] - 5)))
                    {
                        Right_Island_flag1 = true;
                        Right_Island_flag0 = false;
                        Right_Island_state = true;
                        Search_In_Down_Point_Flag = true;
                        printf("Left_island_flag1, Left_Island_state\n");
                        break;
                    }else{
                        In_Down_Flag = false;
                    }
                }
                if (Right_Island_flag1 && Right_Island_state)
                {
                    break;
                }
            }
        }
    }
}
void Right_Island_logic()   //最终版的右圆环logic
{
    if(Right_Island_flag1 && Right_Island_state)
    {
        if (Search_In_Down_Point_Flag)
        {
            for (i = 57; i > 6; i--)
            {
                for (j = i - 1; j > i - 3; j--) // 跟踪下角点
                {
                    if (imageLine.Exist_Right[i]
                        && Rectangle_isWhite(imageLine.Point_Right[i] + 10, i - 3, 10, 5)
                        && imageLine.Point_Right[i] < (imageLine.Point_Right[j] - 5)
                        && imageLine.Point_Right[j] <= imageLine.Point_Right[j - 1])
                    {
                        In_Down_Point_Row = i;
                        In_Down_Point = imageLine.Point_Right[i];
                        if (In_Down_Point_Row > 50)
                        {
                            In_Down_Flag = false;
                            Search_In_Down_Point_Flag = false;
                            break;
                        }else {
                            In_Down_Flag = true;
                            Search_In_Down_Point_Flag = true;
                            printf("what the hell\n");
                            break;              //break位置与左不同
                        }
                    } else{
                        In_Down_Flag = false;
                        Search_In_Down_Point_Flag = true;
                    }
                }
                if(In_Down_Point_Row == i) break;
            }
        } else if (!Search_In_Down_Point_Flag){
            for (i = In_Down_Point_Row - 10; i >= 15; i--)   //********与左不一样
            {
                for (j = i - 1; j > i - 3; j--)            // 找2个逆趋势点
                {
                    if (imageLine.Exist_Right[i] && imageLine.Exist_Right[j] && imageLine.Exist_Right[j - 1]
                        && (imageLine.Point_Right[i] < imageLine.Point_Right[j])
                        && (imageLine.Point_Right[j] + 1 < imageLine.Point_Right[j - 1])) {
                        Inv_Point_Row[0] = i;
                        Inv_Point[0] = imageLine.Point_Right[i];
                        Inv_Point_Row[1] = j;
                        Inv_Point[1] = imageLine.Point_Right[j];
                        break;
                    }
                }
                if (Inv_Point_Row[0] == i) break;
            }
            if (Inv_Point_Row[0])
            {
                for (j = Inv_Point_Row[0] - 10; j >= 13; j--) {
                    if (imageLine.Point_Right[j] < (Inv_Point[0] - 5)
                        && imageLine.Point_Right[j - 1] <= imageLine.Point_Right[j]
                        && imageLine.Point_Right[j - 2] <= imageLine.Point_Right[j - 1]) {
                        In_Up_Point_Row = j;
                        In_Up_Point = imageLine.Point_Right[j];
                        In_Up_Point_Flag = true;
                        break;
                    } else {
                        In_Up_Point_Flag = false;
                    }
                }
            }
            if (In_Up_Point_Flag && (In_Up_Point_Row > 45 || In_Up_Point < 14))
            {
                Right_Island_flag2 = true;
                Right_Island_flag1 = false;
                printf("Right_Island_flag2\n");
            }
        }
    }
    else if(Right_Island_flag2 && Right_Island_state)
    {
        for(i = 57; i >= 4; i--)
        {
            /*if(imageLine.Point_Left[i] && imageLine.Point_Left[i-1] && imageLine.Point_Left[i-2] && (imageLine.Exist_Left[4] + imageLine.Exist_Left[5] + imageLine.Exist_Left[6] == 0)
                && (imageLine.Point_Left[i] - imageLine.Point_Left[i-1] >= 2)
                && (imageLine.Point_Left[i] - imageLine.Point_Left[i+1] >= 0)
                && (imageLine.Point_Left[i-1] >= imageLine.Point_Left[i-2])
                && (2*imageLine.Point_Left[i+1] - imageLine.Point_Left[i+2]- imageLine.Point_Left[i+3] >= 2)
                && (2*imageLine.Point_Left[i] - imageLine.Point_Left[i-1] - imageLine.Point_Left[i+1] >= 2)
                )*/
            if(imageLine.Exist_Left[i] && imageLine.Exist_Left[i-1] && imageLine.Exist_Left[i-2] && (left_inexist_number(i,20) >= 1)
               && (imageLine.Point_Left[i] >= (imageLine.Point_Left[i-1] + 2))
               && (imageLine.Point_Left[i-1] >= imageLine.Point_Left[i-2]) //角点上方平锯齿
               && (2*imageLine.Point_Left[i] >= imageLine.Point_Left[i+1]))  //角点下方斜率
            {
                Out_Down_Point_Row = i;
                Out_Down_Point = imageLine.Point_Left[i];
                Out_Down_Point_Flag = true;
                break;
            }
            if(Out_Down_Point_Flag &&(left_inexist_number(59,4)>= 3))
            {
                Right_Island_flag3 = true;
                Right_Island_flag2 = false;
                Out_Island_Right_Turn = true;
                Out_Island_Turn_Counter = 0;
                printf("Right_Island_flag3\n");
                break;
            }
            //if(Out_Down_Point_Row == i && Out_Down_Point == imageLine.Point_Right[i]) break;
            //if(Right_Island_flag3) break;
        }
    }
    else if(Right_Island_flag3 && Right_Island_state)
    {
        if(Out_Island_Transition_Ratio < 1 && Out_Island_Transition_Flag == false)
        {
            Right_Island_flag3 = false;
            //Right_Island_flag4 = true;
            Right_Island_state = false;
            island_flag_init();
            printf("Right_Island_flag4\n");
        }
    }
    else if(Right_Island_flag4 && Right_Island_state)
    {
        for(i = 30; i > 8; i--)
        {
            for(j = i - 1; j > i-2; j--)
            {
                if(imageLine.Exist_Right[i] && imageLine.Exist_Right[j])
                {
                    if(imageLine.Point_Right[j] >= imageLine.Point_Right[i]
                       && imageLine.Point_Right[j - 1] >= imageLine.Point_Right[j]
                       && imageLine.Point_Right[i] >= imageLine.Point_Right[i + 1])
                    {
                        Out_Up_Point_Row = i;
                        Out_Up_Point = imageLine.Point_Right[i];
                        Out_Up_Point2_Row = i-4;
                        Out_Up_Point2 = imageLine.Point_Right[i-4];
                        Out_Up_Point_Flag = true;
                        break;
                    }
                }
            }
            if(Out_Up_Point_Flag && Out_Up_Point_Row > 40)
            {
                Right_Island_flag4 = false;
                printf("Right_Island_state = false\n");
                break;
            }
            if(Out_Up_Point_Row == i && Out_Up_Point == imageLine.Point_Right[i])
            {
                break;
            }
        }
    }
} //最终版的右圆环logic
void Right_Island_update()  //最终版的右圆环update
{
    if(Right_Island_flag1 && Right_Island_state)
    {
        k_in = -0.75;
        if(In_Down_Flag && !In_Up_Point_Flag)
        {
            k_Right = ((float)imageLine.Point_Right[In_Down_Point_Row] - (float)imageLine.Point_Right[In_Down_Point_Row + 5])
                      / ((float)In_Down_Point_Row - (float)(In_Down_Point_Row + 5));
            for(i = In_Down_Point_Row; i > 4; i--)
            {
                imageLine.Point_Right[i] = (int) constrain_int16(k_Right * (i - In_Down_Point_Row) + In_Down_Point, imageLine.Point_Left[i], ImageW - 1);
            }
            for(j = ImageH-2; j >= 4; j--)
            {
                imageLine.Point_Center[j] = 0.5 * imageLine.Point_Left[j] + 0.5 * imageLine.Point_Right[j];   //0.5，0.5时入环前会靠左
                imageLine.Exist_Center[j] = true;
            }
            updateMediumLine();
        }
        else if(!In_Down_Flag && !In_Up_Point_Flag)
        {
            k_Right = ((float)84 - (float)Inv_Point[0])
                      / ((float)(ImageH - 1) - (float)Inv_Point_Row[0]);
            if(Inv_Point_Row[0] > 3)
            {
                for (i = Inv_Point_Row[0]; i <= ImageH - 1; i++)
                {
                    imageLine.Point_Right[i] = (int) constrain_int16(k_Right * (i - Inv_Point_Row[0]) + Inv_Point[0], imageLine.Point_Left[i], ImageW - 1);
                }
                for(j = ImageH-2; j > 10; j--)
                {
                    imageLine.Point_Center[j] = 0.5 * imageLine.Point_Left[j] + 0.5 * imageLine.Point_Right[j];
                    imageLine.Exist_Center[j] = true;
                }
            }
            updateMediumLine();
        }
        else if(In_Up_Point_Flag && !In_Down_Flag)
        {
            for(j = In_Up_Point_Row; j < ImageH; j++)
            {
                imageLine.Point_Left[j] = (int) constrain_int16(k_in * (j - In_Up_Point_Row) + In_Up_Point, 0, imageLine.Point_Right[j]);
            }
            for(j = ImageH - 1; j >= 0; j--)
            {
                imageLine.Point_Right[j] = 93;
                imageLine.Exist_Right[j] = false;
            }
            for(j = ImageH-2; j > 10; j--)
            {
                imageLine.Point_Center[j] = 0.5 * imageLine.Point_Right[j] + 0.5 * imageLine.Point_Left[j];
                imageLine.Exist_Center[j] = true;
            }
            updateMediumLine();
        }
        else
        {
            doMend();
            updateMediumLine();
        }
    }
    else if(Right_Island_flag2 && Right_Island_state)
    {
        k_out = -2.5;
        if(Out_Down_Point_Flag)
        {
            for(j = Out_Down_Point_Row; j >= 0; j--)
            {
                imageLine.Point_Left[j] = (int) constrain_int16(k_out * (j - Out_Down_Point_Row) + Out_Down_Point, 0, imageLine.Point_Right[j]);
            }
            for(j = ImageH-2; j > 10; j--)
            {
                imageLine.Point_Center[j] = 0.5 * imageLine.Point_Right[j] + 0.5 * imageLine.Point_Left[j];
                imageLine.Exist_Center[j] = true;
            }
            for(j = ImageH - 1; j >= 0; j--)
            {
                imageLine.Point_Right[j] = 93;
                imageLine.Exist_Right[j] = false;
            }
            updateMediumLine();
        }
        else
        {
            for(j = ImageH - 1; j >= 0; j--)
            {
                imageLine.Point_Right[j] = 93;
                imageLine.Exist_Right[j] = false;
            }
            for(j = ImageH-2; j > 10; j--)
            {
                imageLine.Point_Center[j] = 0.5 * imageLine.Point_Left[j] + 0.5 * imageLine.Point_Right[j];
                imageLine.Exist_Center[j] = true;
            }
            updateMediumLine();
        }
    }
    else if(Right_Island_flag3 && Right_Island_state)
    {
        if(Out_Island_Right_Turn)
        {
            servo_param.Servo_filter = FIXED_RIGHT_ANGLE;
            Out_Island_Turn_Counter++;
            if(Out_Island_Turn_Counter > OUT_ISLAND_TURN_TIME)
            {
                Out_Island_Right_Turn = false;
                Out_Island_Transition_Ratio = 0.0f;
                Out_Island_Transition_Flag = false;
            }
        }
    }
    else if(Left_Island_flag4 && Left_Island_state)
    {
        k_Right = ((float)Out_Up_Point2 - (float)Out_Up_Point) / ((float)(Out_Up_Point2_Row - (float)Out_Up_Point_Row));
        if(Out_Up_Point_Flag)
        {
            for (i = Out_Up_Point_Row; i <= ImageH - 1; i++)
            {
                imageLine.Point_Right[i] = (int) constrain_int16(k_Right * (i - Out_Up_Point_Row)  + Out_Up_Point, imageLine.Point_Left[i], ImageW-1);
            }
            for(j = ImageH-2; j > 10; j--)
            {
                imageLine.Point_Center[j] = 0.5 * imageLine.Point_Left[j] + 0.5 * imageLine.Point_Right[j];
                imageLine.Exist_Center[j] = true;
            }
            updateMediumLine();
        }
        else
        {
            doMend();
            updateMediumLine();
        }
    }

} //最终版的右圆环update
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     根据一个定点，向左选取left_search_colume列数，向上选取up_search_row行数，形成矩形，
//              若矩形内全为白点，则返回true，存在黑点则返回false
// 参数说明     定点的列数：x，定点的行数：y，向左找的列数left_search_colume，向上找的行数up_search_row
// 返回参数     bool量
// 使用示例     Rectangle_isWhite(imageLine.Point_Left[j], j - 2, 8, 5)
// 备注信息     用于代替圆环丢线判断
//-------------------------------------------------------------------------------------------------------------------
bool Rectangle_isWhite(uint8_t x, uint8_t y, uint8_t left_search_colume, uint8_t up_search_row)
{
    constrain_int16(x, 0, ImageW - 1);
    constrain_int16(y, 0, ImageH - 1);
    constrain_int16(left_search_colume, 0, ImageW - 1);
    constrain_int16(up_search_row, 0, ImageH - 1);
    constrain_int16(x - left_search_colume, 0, ImageW - 1);
    constrain_int16(y - up_search_row, 0, ImageH - 1);

    for(uint8_t i = y ; i >=  y - up_search_row; i--)
    {
        for(uint8_t j = x ; j >=  x - left_search_colume; j--)
        {
            if(!isWhite(i,j)){ return false; }
        }
    }
    return true;
}

/**************************************中线误差****************************************************/
/*计算中线打角*/
bool MediumLineCal(volatile float* final)
{
    uint8 i;
    static float lastCenter = 47;
    short temp_dev = 0;


    //直道、弯道 目标行、速度转换逻辑
    if(Left_Island_state || Right_Island_state)
    {
        AIM_LINE = AIM_LINE_MUTATION;
        Purpost_Speed = SPEED_MUTATION;
        pid_down_left_limit = 10000;  //左轮下限
        pid_up_left_limit = 7000;    //左轮上限
        pid_down_right_limit = 10000; //右轮下限
        pid_up_right_limit = 7000;   //右轮上限
    }
    else
    {
        if(Mutation_Flag)
        {
            AIM_LINE = AIM_LINE_MUTATION;
            Purpost_Speed = SPEED_MUTATION;
            pid_down_left_limit = 10000;  //左轮下限
            pid_up_left_limit = 7000;    //左轮上限
            pid_down_right_limit = 10000; //右轮下限
            pid_up_right_limit = 7000;   //右轮上限
        }
        else if(Turn_Flag)
        {
            AIM_LINE = AIM_LINE_TURN;
            Purpost_Speed = SPEED_TURN;
            pid_down_left_limit = 10000;  //左轮下限
            pid_up_left_limit = 7000;    //左轮上限
            pid_down_right_limit = 10000; //右轮下限
            pid_up_right_limit = 7000;   //右轮上限
        }
        else if(Straight_Flag)
        {
            AIM_LINE = AIM_LINE_STRAIGHT;
            Purpost_Speed = SPEED_STRAIGHT;
            pid_down_left_limit = 5000;  //左轮下限
            pid_up_left_limit = 7000;    //左轮上限
            pid_down_right_limit = 5000; //右轮下限
            pid_up_right_limit = 7000;   //右轮上限
        }
    }

    while(!imageLine.Exist_Center[AIM_LINE])
    {
        AIM_LINE++;
    }

    //选择AIM_LINE
    /*
    if (!imageLine.Exist_Center[AIM_LINE] && imageLine.Lost_Center)
    {
        uint8_t down_cnt = 0, up_cnt = 0;
        int new_line_down = -1, new_line_up = -1;
        int i;

        // 向下搜索（行号增大）
        for (i = AIM_LINE + 1; i < AIM_LINE + 15 && i < ImageH; i++)
        {
            if (imageLine.Exist_Center[i])
            {
                new_line_down = i;
                break;
            }
            down_cnt++;
        }

        // 向上搜索（行号减小）
        for (i = AIM_LINE - 1; i >= 0 && i > AIM_LINE - 10; i--)
        {
            if (imageLine.Exist_Center[i])
            {
                new_line_up = i;
                break;
            }
            up_cnt++;
        }

        // 选择距离更近的有效行
        if (new_line_down != -1 || new_line_up != -1)
        {
            if (new_line_up != -1 && (up_cnt < down_cnt || new_line_down == -1))
                AIM_LINE = new_line_up;
            else
                AIM_LINE = new_line_down;

            // 可选的行号调整（按需）
            if (AIM_LINE <= AIM_LINE_STRAIGHT) AIM_LINE = AIM_LINE_STRAIGHT;
            else if (AIM_LINE >= AIM_LINE_TURN) AIM_LINE = AIM_LINE_TURN;
        }
        else
        {
            imageLine.Lost_Center = true;
        }
    }*/

    //更新ERR
    if (!imageLine.Lost_Center)
    {
        if (imageLine.Exist_Center[AIM_LINE])
        {
            if (my_abs_float(lastCenter - (float)imageLine.Point_Center[AIM_LINE]) > 100.0f)
            {
                lastCenter = (imageLine.Point_Center[AIM_LINE] + imageLine.Point_Center[AIM_LINE - 1] + imageLine.Point_Center[AIM_LINE + 1]) / 3;
                return true;
            }
            else
            {
                //更新中线误差
                for (i = 0; i < 10; i++)
                {
                    *(final + 10 - i) = *(final + 9 - i);
                }
                *(final) = ((imageLine.Point_Center[AIM_LINE] + imageLine.Point_Center[AIM_LINE - 1] + imageLine.Point_Center[AIM_LINE + 1]) / 3 - center);
                lastCenter = (imageLine.Point_Center[AIM_LINE] + imageLine.Point_Center[AIM_LINE - 1] + imageLine.Point_Center[AIM_LINE + 1]) / 3;
                return true;
            }
        }
    }
    else
    {
        *(final) = *(final + 1);        // 丢线则保持上次的误差
        //Purpost_Speed = 0;
        return false;
    }
    return false;
}

void island_flag_init()
{
    Search_In_Down_Point_Flag = false;
    Out_Island_Transition_Flag = false;  // 是否处于缓慢过渡阶段
    Out_Island_Turn_Counter = 0;         // 打角时间计数器

    In_Down_Point_Row = 0; In_Down_Point=0;
    In_Up_Point_Row = 0;   In_Up_Point=0;
    In_Up_Point_Flag=false;

    Out_Down_Point_Row = 0; Out_Down_Point = 0;
    Out_Up_Point_Row =0 ;   Out_Up_Point = 0;
    Out_Down_Point_Flag=false;
    Out_Up_Point_Flag=false;

}


static uint8_t longStraightRoadJudge(void)
{
    short i;
    for(i = 20; i < 50; i++)
    {
        if((imageLine.Exist_Center[i] && ABS(imageLine.Point_Center[i] - center) > 0.3*i) || !imageLine.Exist_Center[i])
            return 0;
    }
    return 1;
}

static uint8_t longStraightRoadClear(void)
{
    short i;
    for(i = 15; i < 50; i++)
    {
        if((imageLine.Exist_Center[i] && ABS(imageLine.Point_Center[i] - center) > 0.3*i) || !imageLine.Exist_Center[i])
            return 1;
    }
    return 0;
}

/*更新中线打角*/
void updateMediumLine(void)
{
    bool rst = true;
    static uint8_t lostTime = 0;


    /************************计算中线打角*****************************/
    rst = MediumLineCal(image.cam_finalCenterERR);

    servo_param.Servo_filter = image.cam_finalCenterERR[0];
    /*0.8f * image.cam_finalCenterERR[0] + 0.1f * image.cam_finalCenterERR[1]
        + 0.05f * image.cam_finalCenterERR[2] + 0.05f * image.cam_finalCenterERR[3];*/

    servo_param.cam_servo_temp_1 = servo_param.Servo_filter;
}


void show_line(){
    uint8_t i;
    uint8_t leftLine, rightLine, centerLine;
    for(i = 10 ; i < 10 + ImageH ; i++){
        if(imageLine.Point_Left[i] > ImageW ){
            leftLine = ImageW ;
        }
        else if (imageLine.Point_Left[i] < 0 ){
            leftLine = 0 ;
        }
        else{
            leftLine = imageLine.Point_Left[i];
        }

        if(imageLine.Point_Right[i] > ImageW ){
            rightLine = ImageW ;
        }
        else if (imageLine.Point_Right[i] < 0 ){
            rightLine = 0 ;
        }
        else{
            rightLine = imageLine.Point_Right[i];
        }

        if(imageLine.Point_Center[i] > ImageW ){
            centerLine = ImageW ;
        }
        else if (imageLine.Point_Center[i] < 0 ){
            centerLine = 0 ;
        }
        else{
            centerLine = imageLine.Point_Center[i];
        }
        ips200_draw_point(24 + leftLine, i, RGB565_RED);
        ips200_draw_point(25 + leftLine, i, RGB565_RED);
        ips200_draw_point(26 + leftLine, i, RGB565_RED);

        ips200_draw_point(24 + rightLine, i, RGB565_GREEN);
        ips200_draw_point(25 + rightLine, i, RGB565_GREEN);
        ips200_draw_point(26 + rightLine, i, RGB565_GREEN);

        ips200_draw_point(24 + centerLine, i, RGB565_BLUE);
        ips200_draw_point(25 + centerLine, i, RGB565_BLUE);
        ips200_draw_point(26 + centerLine, i, RGB565_BLUE);
        i++;
    }
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
/*返回丢点行的个数*/
uint8_t left_inexist_number(uint8_t begin_row,uint8_t search_row)
{
    uint8_t i;
    uint8_t count = 0;
    uint8_t end_row = constrain_int16(begin_row - search_row, 4, ImageH - 1);
    for(i=begin_row;i>end_row;i--)
    {
        if(!imageLine.Exist_Left[i])count++;
    }
    return count;
}

uint8_t right_inexist_number(uint8_t begin_row,uint8_t search_row)
{
    uint8_t i;
    uint8_t count = 0;
    uint8_t end_row = constrain_int16(begin_row - search_row, 4, ImageH - 1);
    for(i=begin_row;i>end_row;i--)
    {
        if(!imageLine.Exist_Right[i])count++;
    }
    return count;
}


void zebra_found_zz(void)
{
    uint8_t zebra_row_count = 0;
    uint8_t i, j;
    uint8_t edge_count = 0;
    if(isr_count <= 3000) return;
    for(i = ImageH_2; i > 4; i--)
    {
        for(j = imageLine.Point_Left[i]; j <= imageLine.Point_Right[i]; j++)
        {
            if(Pixle[i][j] != Pixle[i][j - 1])
            {
                edge_count++;
            }
        }
        if(edge_count >= 5)
        {
            zebra_row_count++;
            if(zebra_row_count >= 5)
            {
                Flag.Garage_Find = true;
                if(!start_time)
                {
                    start_time = isr_count;
                }
            }
        }
        edge_count = 0;
    }
}

void park(void)
{
    uint8_t blackpoint_count = 0;
    uint8_t blackrow_count = 0;
    for(i = 50; i > 40; i--)
        {
            for(j = 0; j <= ImageW - 1; j++)
            {
                if(Pixle[i][j] == 0)
                {
                    blackpoint_count++;
                    if(blackpoint_count >= 90)
                    {
                        blackrow_count++;
                        break;
                    }
                }
            }
            blackpoint_count =0;
            if(blackrow_count >= 5)
            {
                Flag.Park = true;
                break;
            }
        }
}

void Longest_White_Column(void)
{
    uint8_t i, j = 0;
    Straight_Flag = false;
    Turn_Flag = false;
    Mutation_Flag = false;

    //清零
    for(i = 0; i <= 1; i++)
    {
        White_Column[i] = 0;
    }


    //圆环不调用
    if(Right_Island_state || Left_Island_state)
    {
        return;
    }


    //从下往上得到第46、47列的白点数目
    for(j = 46; j <= 47; j++)
    {
        for(i = ImageH - 1; i >= 0; i--)
        {
            if(!isWhite(i, j) && !isWhite(i - 1, j) && !isWhite(i - 2, j))
                break;
            else
                White_Column[j - 46]++;
        }
    }

    //根据白点含量决定赛道类型
    if(White_Column[0] >= 58 && White_Column[1] >= 58)
    {
        Straight_Flag = true;
    }
    else if(White_Column[0] <= 44 && White_Column[1] <= 44)
    {
        if(Left_Mutation_Count >= 1 || Right_Mutation_Count >= 1){
             Mutation_Flag = true;
         }else Straight_Flag = true;
    }
    else
    {
        if(Left_Mutation_Count >= 1 || Right_Mutation_Count >= 1){
            Turn_Flag = true;
        }else Straight_Flag = true;
    }
}

