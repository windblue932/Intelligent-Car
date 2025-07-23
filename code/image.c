#include "zf_common_headfile.h"

/*ͼ����*/
uint8_t bin_thr = 0;
bool Pixle[ImageH][ImageW];       //��ֵ��ͼ��
static bool isVisited[ImageH][ImageW];//���ص��Ƿ���ʹ�
Image_Data image;
volatile imageLine_t imageLine; // ͼ�������������Ϣ

/*ͼ��յ�*/
short LTurnPoint_Row = EFFECTIVE_ROW, LTurnPoint = 0;
short RTurnPoint_Row = EFFECTIVE_ROW, RTurnPoint = 0;
float Kl_up = 0, Bl_up = 0, Kl_down = 0, Bl_down = 0;
float Kr_up = 0, Br_up = 0, Kr_down = 0, Br_down = 0;
uint8_t Error_Cnt_L = 0, Error_Cnt_R = 0;

/*�������*/
volatile uint8_t AIM_LINE_SET;    //Ŀ����36
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

//��Բ��������ʼ��
bool Left_Island_state = false;
bool Left_Island_flag0 = true;
bool Left_Island_flag1 = false;
bool Left_Island_flag2 = false;
bool Left_Island_flag3 = false;
bool Left_Island_flag4 = false;
bool Out_Island_Left_Turn = false;   // �Ƿ����̶�����ģʽ
bool Right_Island_state = false;
bool Right_Island_flag0 = true;
bool Right_Island_flag1 = false;
bool Right_Island_flag2 = false;
bool Right_Island_flag3 = false;
bool Right_Island_flag4 = false;
bool Search_In_Down_Point_Flag = false;
bool Out_Island_Right_Turn = false;   // �Ƿ����̶��Ҵ��ģʽ
bool  Out_Island_Transition_Flag = false;  // �Ƿ��ڻ������ɽ׶�
int Out_Island_Turn_Counter = 0;      // ���ʱ�������
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
float Out_Island_Transition_Ratio = 1.0f;  // ��ʼ���Ȩ�� = 100%�̶���
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

/************************************ͼ��ɼ�������************************************************/
/*ͼ��Ԥ���������*/
void ImageProcess(void)
{
    uint8 i, j;
    //AIM_LINE = AIM_LINE_SET;

    //���ߺ������߶�������
    imageLine.Lost_Center = false;
    imageLine.Lost_Left = false;
    imageLine.Lost_Right = false;

    for (i = 0; i < ImageH; i++)
    {
        //ÿһ�е����ұ߽������ĵ㶼������
        imageLine.Exist_Left[i] = false;
        imageLine.Exist_Right[i] = false;
        imageLine.Exist_Center[i] = false;

        //�߽������ĵ���Ϊ��ʼλ��
        imageLine.Point_Left[i] = 1;
        imageLine.Point_Right[i] = ImageW - 1;
        imageLine.Point_Center[i] = ImageW / 2;

        //ÿһ�еİ׵��������
        imageLine.White_Num[i] = 0;

        for (j = 0; j < ImageW; j++)
        {
            isVisited[i][j] = false;//DFS��, ���е㶼��û��������
            C_Image[i][j] = mt9v03x_image[i * 2][j * 2];          //  ѹ��ͼ��

        }
    }
    /*************��򷨵õ���ֵ������ֵ**********************/
    bin_thr = GetOSTU(C_Image);
    //bin_thr= otsuThreshold(C_Image);
    /******************ͼ���ֵ��*******************************/
    for (i = 0; i < ImageH; i++)
    {
        for (j = 0; j < ImageW; j++)
        {
            if (C_Image[i][j] > bin_thr)//�׵� 1���ڵ� 0
                Pixle[i][j] = true;
            else
                Pixle[i][j] = false;
        }
    }
    // ���˳�ͷ
    for (j = 31 ; j<61 ; j++){
        for (i = 50  ; i < ImageH ; i++){
            Pixle[i][j] = true;
        }
    }
    //Pixle_Filter();             //���˶�ֵ��ͼ�����
}

/*��ȡ��ֵ����ֵ(���)*/
int GetOSTU(uint8 tmImage[ImageH][ImageW])
{
    int16 i, j;
    uint32 Amount = 0;//��������
    uint32 PixelBack = 0;//back���ص���
    uint32 PixelIntegralBack = 0;//back�Ҷ�ֵ
    uint32 PixelIntegral = 0;//�Ҷ�ֵ����
    int32 PixelIntegralFore = 0;//fore�Ҷ�ֵ
    int32 PixelFore = 0;//fore���ص���
    float OmegaBack, OmegaFore, MicroBack, MicroFore, SigmaB, Sigma; // ��䷽��;
    int16 MinValue, MaxValue;
    uint8 Threshold = 0;
    uint8 HistoGram[256];              //

    for (j = 0; j < 256; j++)  HistoGram[j] = 0; //��ʼ���Ҷ�ֱ��ͼ

    for (j = 0; j < ImageH; j++)
    {
        for (i = 0; i < ImageW; i++)
        {
            HistoGram[tmImage[j][i]]++; //ͳ�ƻҶȼ���ÿ������������ͼ���еĸ���
        }
    }

    for (MinValue = 0; MinValue < 256 && HistoGram[MinValue] == 0; MinValue++);        //��ȡ��С�Ҷȵ�ֵ
    for (MaxValue = 255; MaxValue > MinValue && HistoGram[MinValue] == 0; MaxValue--); //��ȡ���Ҷȵ�ֵ

    if (MaxValue == MinValue)     return MaxValue;         // ͼ����ֻ��һ����ɫ
    if (MinValue + 1 == MaxValue)  return MinValue;        // ͼ����ֻ�ж�����ɫ

    for (j = MinValue; j <= MaxValue; j++)    Amount += HistoGram[j];        //  ��������

    PixelIntegral = 0;
    for (j = MinValue; j <= MaxValue; j++)
    {
        PixelIntegral += HistoGram[j] * j;//�Ҷ�ֵ���� j��0-255 ��ʾ�Ҷ�
    }
    SigmaB = -1;
    for (j = MinValue; j < MaxValue; j++)//�ӱ���ͼ�����С�Ҷȵ����Ҷȱ���
    {
        PixelBack = PixelBack + HistoGram[j];   //ǰ�����ص���
        PixelFore = Amount - PixelBack;         //�������ص���
        OmegaBack = (float)PixelBack / Amount;//ǰ�����ذٷֱ�
        OmegaFore = (float)PixelFore / Amount;//�������ذٷֱ�
        PixelIntegralBack += HistoGram[j] * j;  //ǰ���Ҷ�ֵ
        PixelIntegralFore = PixelIntegral - PixelIntegralBack;//�����Ҷ�ֵ
        MicroBack = (float)PixelIntegralBack / PixelBack;   //ǰ���ҶȰٷֱ�
        MicroFore = (float)PixelIntegralFore / PixelFore;   //�����ҶȰٷֱ�
        Sigma = OmegaBack * OmegaFore * (MicroBack - MicroFore) * (MicroBack - MicroFore);//������䷽��
        if (Sigma > SigmaB)                    //����������䷽��g //�ҳ������䷽���Լ���Ӧ����ֵ
        {
            SigmaB = Sigma;
            Threshold = j;//j��0-255�Ҷ�ֵ
        }
    }
    return Threshold;                        //���������ֵ;
    /*
    int16 i, j;
        uint32 Amount = 0;         // ��������
        uint32 PixelIntegral = 0;  // �Ҷ�ֵ�ܺ�
        uint8 HistoGram[256] = {0}; // ��ʼ���Ҷ�ֱ��ͼΪ0

        // ͳ��ֱ��ͼ�����������غ��ܻҶ�ֵ
        for (j = 0; j < ImageH; j++) {
            for (i = 0; i < ImageW; i++) {
                uint8 pixel = tmImage[j][i];
                HistoGram[pixel]++;
                Amount++;
                PixelIntegral += pixel;
            }
        }

        // ��ȡ��С�����Ҷ�ֵ
        int16 MinValue = 0;
        while (MinValue < 256 && HistoGram[MinValue] == 0) MinValue++;
        int16 MaxValue = 255;
        while (MaxValue > MinValue && HistoGram[MaxValue] == 0) MaxValue--;

        if (MaxValue == MinValue)     return MaxValue; // ����һ����ɫ
        if (MinValue + 1 == MaxValue) return MinValue; // ����������ɫ

        uint32 PixelBack = 0;         // ����������
        uint32 PixelIntegralBack = 0; // �����Ҷ��ܺ�
        double BestSigma = -1;        // �����䷽��
        uint8 Threshold = MinValue;   // ��ʼ��ֵ

        // Ԥ���㳣��
        const double InvAmount = 1.0 / Amount;
        const double PixelIntegralDouble = (double)PixelIntegral;

        for (j = MinValue; j <= MaxValue; j++) {
            PixelBack += HistoGram[j];
            if (PixelBack == 0) continue; // ���������صĻҶȼ�

            PixelIntegralBack += (uint32)j * HistoGram[j];
            uint32 PixelFore = Amount - PixelBack; // ǰ��������
            if (PixelFore == 0) break; // ǰ�������ؿ���ǰ��ֹ

            // ����ͳ����
            double OmegaBack = PixelBack * InvAmount;
            double OmegaFore = 1.0 - OmegaBack;
            double MicroBack = (double)PixelIntegralBack / PixelBack;
            double MicroFore = (PixelIntegralDouble - PixelIntegralBack) / PixelFore;

            // ������䷽��
            double Sigma = OmegaBack * OmegaFore * (MicroBack - MicroFore) * (MicroBack - MicroFore);

            // ���������ֵ
            if (Sigma > BestSigma) {
                BestSigma = Sigma;
                Threshold = j;
            }
        }
        return Threshold;*/
}

/*�õ�ÿһ�����а׵�ĸ���*/
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

/*DFSѲ�ߣ�������������ҳ���ͨ�������а׵�(DFS������)*/
void trackDFS(void)
{
    uint8_t i, j;
    uint8_t roadPointStackX[300];             //DFS��ջ�洢Ѱ��X����
    uint8_t roadPointStackY[300];             //DFS��ջ�洢Ѱ��Y����
    short stackTopPos = -1;

    if (!Flag.Left_Ring_Turn && !Flag.Right_Ring_Turn)
    {
        //ѡ��ͼƬ�·��е���Ϊ��ʼ��
        if (isWhite(ImageH - 2, ImageW / 2))          //���·��е���ǰ׵㣬��ջ
        {
            stackTopPos++;
            roadPointStackX[stackTopPos] = ImageH - 2;
            roadPointStackY[stackTopPos] = ImageW / 2;
            isVisited[ImageH - 2][ImageW / 2] = true;
        }
        else         //����·��е㲻�ǰ׵㣬�ʹ������������������������㶼�ǰ׵㣬���ж�Ϊdfs�����
        {
            for (j = ImageW - 4; j >= 2; j--)//������������
            {
                if (isWhite(ImageH - 2, j - 2) && isWhite(ImageH - 2, j - 1) && isWhite(ImageH - 2, j)
                    && isWhite(ImageH - 2, j + 1) && isWhite(ImageH - 2, j + 2))//����5���׵�
                {
                    //����������ջ
                    stackTopPos++;                      //stackTopPos����ͱ�ʾջ�ǿ�
                    roadPointStackX[stackTopPos] = ImageH - 2;
                    roadPointStackY[stackTopPos] = j;
                    isVisited[ImageH - 2][j] = true;      //���Ϊ�õ��Ѿ�������
                    break;                              //һ��ȷ������break
                }
            }
        }

        i = j = 0;

        while (stackTopPos >= 0)                        //ֻҪ�ҵ����Ϳ�ʼ
        {
            //��ջ
            i = roadPointStackX[stackTopPos];
            j = roadPointStackY[stackTopPos];
            stackTopPos--;

            //������磬ֱ��continue
            if ((i < EFFECTIVE_ROW) || (i > ImageH - 2) || (j < 1) || (j > ImageW - 1))
            {
                continue;
            }

            /*************���²���ԭ���ǣ����׵���ջ�����ڵ㡰�������ж�Ϊ�߽��(��������Ҫ�����˲����ߵȲ���)**************/
            /**************************��ջ�洢��...����(��)����(��)...******************************/
            /******************һ������£��ϵİ׵���ȳ�ջ(����Ϊ��һֱ��ͼ���Ϸ�����)��Ȼ���ٳ�����*****************/

            //��������
            if (!isVisited[i][j - 1])                   //�������һ���Ƿ������
            {
                if (isWhite(i, j - 1))
                {
                    //�׵���ջ
                    stackTopPos++;
                    roadPointStackX[stackTopPos] = i;
                    roadPointStackY[stackTopPos] = j - 1;
                    isVisited[i][j - 1] = true;         //���Ϊ�õ��Ѿ�������
                }
                else                                    //�ڵ�ͳ����ж�Ϊ�߽��               (xzl: �о�����ָ��ǵ����)
                {
                    if (isLeftPoint(i, j))
                    {
                        //�������������
                        if (!imageLine.Exist_Left[i] || (imageLine.Exist_Left[i] && imageLine.Point_Left[i] < j && j < 59))
                        {
                            imageLine.Point_Left[i] = j;    //��߽��ĺ�����
                            imageLine.Exist_Left[i] = 1;    //��߽�����
                        }
                    }
                }
            }

            //��������
            if (!isVisited[i][j + 1])                   //�����ұ�һ���Ƿ������
            {
                if (isWhite(i, j + 1))
                {
                    //�׵���ջ
                    stackTopPos++;
                    roadPointStackX[stackTopPos] = i;
                    roadPointStackY[stackTopPos] = j + 1;
                    isVisited[i][j + 1] = true;         //���Ϊ�õ��Ѿ�������
                }
                else                                    //�ڵ�ͳ����ж�Ϊ�߽��               (xzl: �о�����ָ��ǵ����)
                {
                    if (isRightPoint(i, j))
                    {
                        if (!imageLine.Exist_Right[i] || (imageLine.Exist_Right[i] && imageLine.Point_Right[i] > j && j > 35))
                        {
                            imageLine.Point_Right[i] = j;//�ұ߽��ĺ�����
                            imageLine.Exist_Right[i] = true;
                        }
                    }
                }
            }

            //��������(���ϲ��жϱ߽��)
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
        for (i = 0; i <= ImageW / 2; i++)//������������
        {
            if (Flag.Left_Ring_Turn)
            {
                if (isWhite(ImageH - 2, i - 2) && isWhite(ImageH - 2, i - 1) && isWhite(ImageH - 2, i)
                    && isWhite(ImageH - 2, i + 1) && isWhite(ImageH - 2, i + 2)//����5���׵�
                    )
                {
                    //����������ջ
                    stackTopPos++;//stackTopPos����ͱ�ʾջ�ǿ�
                    roadPointStackX[stackTopPos] = ImageH - 2;
                    roadPointStackY[stackTopPos] = i;
                    isVisited[ImageH - 2][i] = 1;
                }
            }
            else
            {
                if (isWhite(ImageH - 2, ImageW - i - 2) && isWhite(ImageH - 2, ImageW - i - 1) && isWhite(ImageH - 2, ImageW - i)
                    && isWhite(ImageH - 2, ImageW - i + 1) && isWhite(ImageH - 2, ImageW - i + 2)//����5���׵�
                    )
                {
                    //����������ջ
                    stackTopPos++;//stackTopPos����ͱ�ʾջ�ǿ�
                    roadPointStackX[stackTopPos] = ImageH - 2;
                    roadPointStackY[stackTopPos] = i;
                    isVisited[ImageH - 2][i] = 1;
                }
            }
        }

        i = j = 0;

        while (stackTopPos >= 0)
        {
            //��ջ
            i = roadPointStackX[stackTopPos];
            j = roadPointStackY[stackTopPos];
            stackTopPos--;

            //������磬ֱ��continue
            if ((i < EFFECTIVE_ROW) || (i > ImageH - 2) || (j < 1) || (j > ImageW - 2))
            {
                continue;
            }

            /*************���²���ԭ���ǣ����׵���ջ�����ڵ�����ж�Ϊ�߽��(��������Ҫ�����˲����ߵȲ���)**************/
            //��ջ�洢��...����(��)����(��)...
            //һ������£��ϵİ׵���ȳ�ջ(����Ϊ��һֱ��ͼ���Ϸ�����)��Ȼ���ٳ�����
            //��������
            if (!isVisited[i][j - 1])
            {
                if (isWhite(i, j - 1))
                {
                    //�׵���ջ
                    stackTopPos++;
                    roadPointStackX[stackTopPos] = i;
                    roadPointStackY[stackTopPos] = j - 1;
                    isVisited[i][j - 1] = 1;
                }
                else
                {
                    //�ڵ�����ж�Ϊ�߽��(xzl: �о�����ָ��ǵ����)
                    if (isLeftPoint(i, j))
                    {
                        imageLine.Point_Left[i] = j;//���߹켣
                        imageLine.Exist_Left[i] = 1;
                        imageLine.Lost_Left = 0;
                    }
                }
            }

            //��������
            if (!isVisited[i][j + 1])
            {
                if (isWhite(i, j + 1))
                {
                    stackTopPos++;
                    roadPointStackX[stackTopPos] = i;
                    roadPointStackY[stackTopPos] = j + 1;
                    isVisited[i][j + 1] = 1;
                }
                else//�����ҵ��ұ߽�
                {
                    if (isRightPoint(i, j))
                    {
                        imageLine.Point_Right[i] = j;//���߹켣
                        imageLine.Exist_Right[i] = 1;
                        imageLine.Lost_Right = 0;
                    }
                }
            }
            //��������(���ϲ��жϱ߽��)
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

/****************************************�˲�******************************************************/
/*�˳���ֵ��ͼ�����*/
void Pixle_Filter(void)
{

  uint8_t nr; //��
    uint8_t nc; //��

    //������
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

    // ������ʱ����ʱ��չ�߽磨����߽��жϣ�
    memcpy(tmp, Pixle, sizeof(tmp));

    // ʹ�ð������ж�+�����ֵ
    for (nr = 1; nr < ImageH-1; nr++) {
        for (nc = 1; nc < ImageW-1; nc++) {
            // ��������㣨�����ĸ��Խǣ�
            const uint8_t sum_8 = Pixle[nr-1][nc-1] + Pixle[nr-1][nc] + Pixle[nr-1][nc+1]
                                + Pixle[nr][nc-1] + Pixle[nr][nc+1]
                                + Pixle[nr+1][nc-1] + Pixle[nr+1][nc] + Pixle[nr+1][nc+1];

            // ����������
            if (Pixle[nr][nc] == 1) {
                // �׵���˹���
                //1. �������ɫ������ < 3��������ֱ���˳�
                //2. 3 <= �������� < 5����Ҫ������֤������
                tmp[nr][nc] = (sum_8 < 3) ? 0 :
                             ((sum_8 < 5) ? ((Pixle[nr-1][nc] + Pixle[nr+1][nc]
                                           + Pixle[nr][nc-1] + Pixle[nr][nc+1]) >= 2)
                                          : 1);
            } else {
                // �ڵ������򱣳�ԭ�߼�
                tmp[nr][nc] = (Pixle[nr-1][nc] + Pixle[nr+1][nc]
                             + Pixle[nr][nc-1] + Pixle[nr][nc+1] > 2) ? 1 : 0;
            }
        }
    }

    memcpy(Pixle, tmp, sizeof(tmp));
*/
}

/*��ȱ����˲�*/
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
/*filter�������*/
void doFilter(void)
{
    if (!Flag.CrossRoad_Find || Flag.crossLeft || Flag.crossRight)
    {
        TurnFliter(LTurnPoint_Row, LTurnPoint, RTurnPoint_Row, RTurnPoint, 3, 3,
            Kl_up, Bl_up, Kl_down, Bl_down, Kr_up, Br_up, Kr_down, Br_down, Error_Cnt_L, Error_Cnt_R);
    }
    left_right_Limit();    //�߽��Խ������
    lineChangeLimit();     //�߽�б������
    singlePoint_Filter();  //��������ȥ
    lostLine_Filter();    //��Ч�й�����ȥ(�������ж�)
}
/*��Ч�й�����ȥ(�������ж�)*/
void lostLine_Filter(void)
{
    //������߽��ߵ��ж�--------------------
    short count = 0;
    int8_t i = 0;

    for (i = 0; i < ImageH; i++)        //���ϵ�������
    {
        if (imageLine.Exist_Left[i])
            count++;
    }

    if (count < VALID_LINE_THRESHOLE)        //�����Ч�г�����ֵ��Ϊ�ñ߽��߶�ʧ
    {
        imageLine.Lost_Left = 1;
        for (i = 0; i < ImageH; i++)        //���ϵ�������
        {
            imageLine.Exist_Left[i] = 0;
        }
    }
    else
        imageLine.Lost_Left = 0;

    //�����ұ߽��ߵ��ж�--------------------
    count = 0;
    for (i = 0; i < ImageH; i++)
    {
        if (imageLine.Exist_Right[i])
            count++;
    }

    if (count < VALID_LINE_THRESHOLE)        //�����Ч�г�����ֵ��Ϊ�ñ߽��߶�ʧ
    {
        imageLine.Lost_Right = 1;
        for (i = 0; i < ImageH; i++)        //���ϵ�������
        {
            imageLine.Exist_Right[i] = 0;
        }
    }
    else
        imageLine.Lost_Right = 0;
}
/*���ұ߽��Խ���˲�*/
void left_right_Limit(void)
{
    int8_t i = 0;

    for (i = 1; i < ImageH - 1; i++)//�����ϵ�����
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
/*�߽�㲻ͻ��*/
void lineChangeLimit(void)
{
    int8_t i = 0, j = 0;
    float leftK = 0, rightK = 0;

    //��߽���������Ч��б�ʼ��
    for (i = ImageH - 2; i > 0; i--)        //��������
    {
        if (imageLine.Exist_Left[i])        //���ҵ���һ����Ч��
        {
            for (j = i + 1; j < ImageH; j++)        //���������ٽ���Ч��
            {
                if (imageLine.Exist_Left[j])
                {
                    leftK = getLineK(i, imageLine.Point_Left[i], j, imageLine.Point_Left[j]);

                    if (ABS(leftK) > K_MAX_THRESHOLD)
                    {
                        imageLine.Exist_Left[i] = 0;
                        //imageLine.Exist_Left[j] = 0;
                    }
                    break;        //ֻҪ�ҵ�һ���ٽ���Ч�㣬�����break����һ��i
                }
            }
        }
    }

    //�ұ߽���������Ч��б�ʼ��
    for (i = ImageH - 2; i > 0; i--)        //��������
    {
        if (imageLine.Exist_Right[i])        //���ҵ���һ����Ч��
        {
            for (j = i + 1; j < ImageH; j++)        //���������ٽ���Ч��
            {
                if (imageLine.Exist_Right[j])
                {
                    rightK = getLineK(i, imageLine.Point_Right[i], j, imageLine.Point_Right[j]);

                    if (ABS(rightK) > K_MAX_THRESHOLD)
                    {
                        imageLine.Exist_Right[i] = 0;
                        //imageLine.Exist_Right[j] = 0;
                    }
                    break;        //ֻҪ�ҵ�һ���ٽ���Ч�㣬�����break����һ��i
                }
            }
        }
    }
}
/*�����߽���˳�*/
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
/****************************************����******************************************************/
/*���е㸳ֵ������Ϊ����������Ч*/
void amplitudeLIMIT(uint16_t i, int16_t amp)
{
    imageLine.Exist_Center[i] = true;
    imageLine.Point_Center[i] = amp;
}
/*�Եó������߽���ͻ���޷�*/
void limitCenter(void)
{
    for (int8_t i = ImageH - 2; i >= 1; i--)        //��������
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
/*�����ߺϼ�*/
void doMend(void)
{
    trackMend_startPart();
    //new_trackMend_startPart();
    trackMend_HalfPart();//�����߰����������ֱ�Ӽ���
}
/*Ԫ����յ��˲�����*/
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
    //�˲�����
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

/*��������*/
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

    //------------------------������-------------------------------
    //��������ʼ��
    for (i = ImageH - 1; i >= (ImageH_6 ); i--)
    {
        if (imageLine.Exist_Left[i])
        {
            leftLine_startPoint = i;
            break;
        }
    }

    //------------------------������-------------------------------
    //��������ʼ��
    for (i = ImageH - 1; i >= (ImageH_6 ); i--)
    {
        if (imageLine.Exist_Right[i])
        {
            rightLine_startPoint = i;
            break;
        }
    }

    if (imageLine.Point_Right[rightLine_startPoint] - imageLine.Point_Left[leftLine_startPoint] >= 5)    //��ֹ�������λ�ò��²�����
    {
        //������ʼ����2/3��ʱ(ͼ��ײ����������ߵĲ����е����),����
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
                if (count == 5)    //��5���㼴�ɿ�ʼ����
                {
                    leastSquareMethod(MendBasis_left[0], MendBasis_left[1], 5, &k_left, &b_left);
                    if (ABS(k_left) > 2 && i > ImageH_6)
                    {
                        count = 0;
                        continue;
                    }
                    //��ʼ����
                    for (i=i+5; i < ImageH; i++)
                    {
                         imageLine.Point_Left[i] = constrain_int16(getLineValue(i, k_left, b_left), 0, ImageW - 1);
                         imageLine.Exist_Left[i] = 1;
                    }
                    break;
                }
            }

        }
        //������ʼ����2/3��ʱ(ͼ��ײ����������ߵĲ����е����),����
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
                if (count == 5)    //��5���㼴�ɿ�ʼ����
                {
                    leastSquareMethod(MendBasis_right[0], MendBasis_right[1], 5, &k_right, &b_right);
                    if (ABS(k_right) > 2 && i < ImageH_6)
                    {
                        count = 0;
                        continue;
                    }
                    //��ʼ����
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
    //��ʮ��
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
            if (RoadLeft(i) > 90 && RoadLeft(i + 1) > 90 && RoadLeft(i + 2) > 90)//��������ȫ����
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
            if (RoadRight(i) > 90 && RoadRight(i + 1) > 90 && RoadRight(i + 2) > 90)//��������ȫ����
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

    //(һ) ���߶���(û����)
    if (imageLine.Lost_Left && imageLine.Lost_Right)
    {
        imageLine.Lost_Center = true;
    }

    //(��) ֻ�������
    else if (imageLine.Lost_Left)
    {
        imageLine.Lost_Center = false;
        //1. �������߲���
        for (i = ImageH - 1; i >= 0; i--)
        {
            if (imageLine.Exist_Right[i])
            {
                err += (((ImageW) / 2 + roadK * i / 2 + roadB / 2) - imageLine.Point_Right[i]);
                count++;
            }
        }
        //2. ����ƽ�����
        aveErr = (float)(err / count);
        //3. �����ұ��߲�����(·��+���߲���)
        if (count >= 5 && aveErr > 0)//�����㹻��ȷ���ұ߽���������б
            centerCompensation = LIMIT2MIN(aveErr, SingleLineLeanAveERR_MAX) / SingleLineLeanAveERR_MAX * SingleLineLeanK / 2;//��������

        for (i = ImageH - 1; i >= 0; i--)
        {
            if (imageLine.Exist_Right[i])
            {
                //�������߲��޷�
                imageLine.Exist_Center[i] = true;
                amplitudeLIMIT(i, imageLine.Point_Right[i] - centerCompensation - roadK * i / 2 - roadB / 2 - (59 - i) * 0.15);
            }
        }
        limitCenter();//�Եó������߽���ͻ���޷�
    }
    //(��) ֻ���ұ���
    else if (imageLine.Lost_Right)
    {
        imageLine.Lost_Center = false;
        //1. �������߲���
        for (i = ImageH - 1; i >= 0; i--)
        {
            if (imageLine.Exist_Left[i])
            {
                err += (imageLine.Point_Left[i] - (ImageW / 2 - roadK * i / 2 - roadB / 2));
                count++;
            }
        }

        //2. ����ƽ�����
        aveErr = (float)(err / count);
        //3. ��������߲�����(·��+���߲���)
        if (count >= 5 && aveErr > 0)//�����㹻��ȷ���ұ߽���������б
            centerCompensation = LIMIT2MIN(aveErr, SingleLineLeanAveERR_MAX) / SingleLineLeanAveERR_MAX * SingleLineLeanK / 2;//��������

        for (i = ImageH - 1; i >= 0; i--)
        {
            if (imageLine.Exist_Left[i])
            {
                //�������߲��޷�
                imageLine.Exist_Center[i] = true;
                amplitudeLIMIT(i, imageLine.Point_Left[i] + centerCompensation + roadK * i / 2 + roadB / 2 + (59 - i) * 0.15);
            }
        }
        limitCenter();//�Եó������߽���ͻ���޷�
    }
    //(��)���߶�û�ж���
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
/**************************************�ײ㺯��****************************************************/
/*�ж��Ƿ�Ϊ�׵�*/
bool isWhite(uint16_t row, uint16_t line)
{
    //�����ж�
    if (!(row >= 0 && row < ImageH && line >= 0 && line < ImageW))
        return false;

    //�жϰ׵�ڵ�
    if (Pixle[row][line])
        return true;
    else
        return false;
}
/*�ж��Ƿ�Ϊ���ұ߽��*/
bool isLeftPoint(uint16_t i, uint16_t j)
{
    if (j < 2 || j >= ImageW - 3 || i<0 || i>ImageH - 1)//ͼ���Ե
        return false;
    //�ұ�һ�����ܳ�������
    if (((!isWhite(i, j)) || (!isWhite(i, j + 1)) || (!isWhite(i, j + 2)) || (!isWhite(i, j + 3)) || (!isWhite(i, j + 4)) || (!isWhite(i, j + 5))))
        return false;
    //���һ�����ܳ���·
    if (isWhite(i, j - 1) || isWhite(i, j - 2) || isWhite(i, j - 3) || isWhite(i, j - 4) || isWhite(i, j - 5))
        return false;
    if (j < 0 || j >= ImageW)// ������������Χ��ֱ�ӷ�����Ч
        return false;

    return true;
}
bool isRightPoint(uint16_t i, uint16_t j)
{
    if (j < 2 || j >= ImageW - 3 || i<0 || i>ImageH - 1)//ͼ���Ե
        return false;
    //���һ�����ܳ�������
    if (((!isWhite(i, j)) || (!isWhite(i, j - 1)) || (!isWhite(i, j - 2)) || (!isWhite(i, j - 3)) || (!isWhite(i, j - 4)) || (!isWhite(i, j - 5))))
        return false;
    //�ұ�һ�����ܳ���·
    if (isWhite(i, j + 1) || isWhite(i, j + 2) || isWhite(i, j + 3) || isWhite(i, j + 4) || isWhite(i, j + 5))
        return false;
    if (j < 0 || j >= ImageW)// ������������Χ��ֱ�ӷ�����Ч
        return false;

    return true;
}
/*�߽����Ƿ�Ϊֱ�� (EndLine �жϽ�����)*/
bool LeftLine_Check(uint16_t EndLine)
{
    short i = 0, j = 0;
    short StartLine = 0;
    //���յ�
    if (LTurnPoint_Row >= EndLine)
        return false;
    //�ҵ��������ʼ��
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
    //�ж�����λ���Ƿ�����
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
    //�ж���߽��������Ƿ�����
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
    //�ж������б���Ƿ�����
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
    //���յ�
    if (RTurnPoint_Row >= EndLine)
        return false;
    //�ҵ��ұ�����ʼ��
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
    //�ж�����λ���Ƿ�����
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
    //�ж��ұ߽��������Ƿ�����
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
    //�ж��ұ���б���Ƿ�����
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
/*ͼ���ҹյ�*/
void TurnPointL_Find(short* TurnPoint_Row, short* TurnPoint, float* kl_up, float* bl_up, float* kl_down, float* bl_down, uint8_t* error_cnt_l)
{
    uint8_t err_cnt = 0, StartLineL = 0;
    short i = 0, j = 0, k = 0;
    short BasisLine_Up[3] = { 0,0,0 }, BasisPoint_Up[3] = { 0,0,0 };
    short BasisLine_Down[3] = { 0,0,0 }, BasisPoint_Down[3] = { 0,0,0 };
    short BreakLine[2] = { 0,0 }, BreakPoint[2] = { 0,0 };
    bool flag_left2 = false, flag_enough = false, flag_right1 = false, flag_break = false, flag_err1 = false, flag_err2 = false;

    singlePoint_Filter();  //��������ȥ
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
        //45����������6��û�б߽��ֱ���˳�
        if ((i <= ImageH - 20) && (!imageLine.Exist_Left[i] && !imageLine.Exist_Left[i - 1] && !imageLine.Exist_Left[i - 2]
            && !imageLine.Exist_Left[i - 3] && !imageLine.Exist_Left[i - 4] && !imageLine.Exist_Left[i - 5]))
            break;
        //״̬1��δ�ҵ������Ƶ㡣
        //�����ҵ�3�������Ƶ㣬����¼��������б���б�
        //      �����ҵ��ϵ�
        if (!flag_break && !flag_left2 && imageLine.Exist_Left[i])
        {
            //����1�������Ƶ�
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
            //�ҵ�1�������Ƶ��������������
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
                                if (imageLine.Point_Left[k] > imageLine.Point_Left[j])//�ҵ�3�������Ƶ�֮ǰ����������ת���ж�Ϊ�ٹյ�
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
                    //���������Ƶ��Ϸ��Ƿ�Ϊ��
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
            //û���ҵ������Ƶ㣬����Ƿ�Ϊ�ϵ�
            else if (imageLine.Point_Left[i] < ImageW_2_3 && !imageLine.Exist_Left[i - 1] &&
                !imageLine.Exist_Left[i - 2] && !imageLine.Exist_Left[i - 3])
            {
                BreakLine[1] = i;
                BreakPoint[1] = imageLine.Point_Left[i];
            }
            //�ҵ�1���ϵ�������ж��Ƿ��ǰ׵�
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
        //״̬2���ҵ�3�������Ƶ㡣
        //�����������Ƶ��·��ҵ�3�������Ƶ㣬����¼��������б���б�
        if (flag_left2 && !flag_right1)
        {
            //���������Ƶ��·����µ�
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
            //�ҵ����µ��������2�������Ƶ�
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
                                if (imageLine.Point_Left[k] > imageLine.Point_Left[j])//�ҵ�3�������Ƶ�֮ǰ����������ת���ж�Ϊ�ٹյ�
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
            if (!flag_right1)//û���ҵ�3�������Ƶ㣬�建�棬������
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
        //״̬3���ҵ��ϵ㡣
        //�����ڶϵ��·��ҵ�3�������Ƶ㡣
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
        //״̬4���ҵ�3�������Ƶ㣬�����·���3�������Ƶ㡣
        //���񣺷����ϡ��¹յ�(��ϵ�)��k��b
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
        //״̬5���ҵ��ϵ㡣
        //���񣺷��ضϵ㡢k��b
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

    singlePoint_Filter();  //��������ȥ
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
        //45����������6��û�б߽��ֱ���˳�
        if ((i <= ImageH - 20) && (!imageLine.Exist_Right[i] && !imageLine.Exist_Right[i - 1] && !imageLine.Exist_Right[i - 2]
            && !imageLine.Exist_Right[i - 3] && !imageLine.Exist_Right[i - 4] && !imageLine.Exist_Right[i - 5]))
            break;
        //״̬1��δ�ҵ������Ƶ㡣
        //�����ҵ�3�������Ƶ㣬����¼��������б���б�
        //      �����ҵ��ϵ�
        if (!flag_break && !flag_right2 && imageLine.Exist_Right[i])
        {
            //����1�������Ƶ�
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
            //�ҵ�1�������Ƶ��������������
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
                                if (imageLine.Point_Right[k] < imageLine.Point_Right[j])//�ҵ�3�������Ƶ�֮ǰ����������ת���ж�Ϊ�ٹյ�
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
                else//û���ҵ����������Ƶ㣬�ж��Ƿ���ڶϵ㣬���û�����建�棬������
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
            //û���ҵ������Ƶ㣬����Ƿ�Ϊ�ϵ�
            else if (imageLine.Point_Right[i] > ImageW_3 && !imageLine.Exist_Right[i - 1] &&
                !imageLine.Exist_Right[i - 2] && !imageLine.Exist_Right[i - 3])
                 {
                      BreakLine[1] = i;
                      BreakPoint[1] = imageLine.Point_Right[i];
                 }
            //�ҵ�1���ϵ�������ж��Ƿ��ǰ׵�
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
        //״̬2���ҵ�3�������Ƶ㡣
        //�����������Ƶ��·��ҵ�3�������Ƶ㣬����¼��������б���б�
        if (flag_right2 && !flag_left1)
        {
            //���������Ƶ��·����µ�
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
                //�ҵ����µ��������2�������Ƶ�
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
                                if (imageLine.Point_Right[k] < imageLine.Point_Right[j])//�ҵ�3�������Ƶ�֮ǰ����������ת���ж�Ϊ�ٹյ�
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
            if (!flag_left1)//û���ҵ�3�������Ƶ㣬�建�棬������
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
        //״̬3���ҵ��ϵ㡣
        //�����ڶϵ��·��ҵ�3�������Ƶ㡣
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
        //״̬4���ҵ�3�������Ƶ㣬�����·���3�������Ƶ㡣
        //���񣺷����ϡ��¹յ㡢k��b
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
        //״̬5���ҵ��ϵ㡣
        //���񣺷��ضϵ㡢k��b
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


// **************************************************************ʮ��**************************************
/*����ĳ���Ұ�߻����ߵİ׵���*/
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


uint8_t Point_LeftRight(uint8_t row)//���ұ߿�ʼ�ҵ�һ����߽�
{
    uint8 i = 0, j = 0;
    for (i = ImageW - 1;i > 0;i--)
    {
        if (isWhite(row, i))//�ұߵ�һ���׵�
            break;
    }
    for (j = i - 1;j < ImageW;j--)
    {
        if (!isWhite(row, j))
            break;

    }
    return j + 1;
}
uint8_t Point_RightLeft(uint8_t row)//����߿�ʼ�ҵ�һ���ұ߽�
{
    uint8 white = 0, i = 0, j = 0;
    for (i = 0;i < ImageW;i++)
    {

        if (isWhite(row, i))//��ߵ�һ���׵�
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
 * �������ƣ�RoadRight
 * ����˵�������ұߵ�һ���ڵ㿪ʼ����Ŀ��
 * ����˵����row:����
 * �������أ����е��ұ�·��
 * ������ע��*
 ***************************************************************************/

uint8_t RoadRight(uint8_t row)
{
    uint8 white = 0, i = 0, j = 0;
    for (i = ImageW - 1;i > 0;i--)
    {
        if (isWhite(row, i))//�ұߵ�һ���׵�
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
 * �������ƣ�RoadLeft
 * ����˵��������ߵ�һ���׵㿪ʼ����Ŀ��
 * ����˵����row:����
 * �������أ����е����·��
 * ������ע��*
 ***************************************************************************/
int8_t RoadLeft(uint8_t row)
{
    uint8 white = 0, i = 0, j = 0;
    for (i = 0;i < ImageW;i++)
    {
        if (isWhite(row, i))//��ߵ�һ���׵�
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


/****************************************Բ��******************************************************/
/****************************************��Բ��******************************************************/
void Second_Defference(void) //���β���ж��Ƿ����������
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

void Left_Island_detect()  //���հ����Բ��detect
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
                //�ұ������� �ҳ����½ǵ� ���½ǵ��Ϸ�����������
                for (j = i - 1; j > i - 3; j--)
                {
                    if (imageLine.Exist_Left[i]
                        && Rectangle_isWhite(imageLine.Point_Left[i], i - 3, 10, 5) //***********************************����Բ���˸�**************************************
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
            for (i = In_Down_Point_Row - 2; i >= 4; i--)   //*********   In_Down_Point_Row - 4���޸�
            {
                for (j = i - 1; j > i - 3; j--) // ��2�������Ƶ�
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
void Left_Island_logic()   //���հ����Բ��logic
{
    if(Left_Island_flag1 && Left_Island_state)
    {
        if(Search_In_Down_Point_Flag){
            for(i = 57; i >= 6; i--)
            {
                In_Down_Point_Row = 0;
                In_Down_Point = 0;
                In_Down_Flag = false;
                for (j = i - 1; j > i - 3; j--) // �����½ǵ�
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
            for (i = In_Down_Point_Row - 10; i >= 15; i--)  //i�ķ�Χ��49+��->15 ��ħ��49:�½ǵ㶪ʧ�ĵ� ħ��15����ֹ�����Ƶ��ҵ�Զ����ͬʱ��֤�ҵ������Ƶ㣩
            {
                for (j = i - 1; j > i - 3; j--) // ��2�������Ƶ�
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
                for (j = Inv_Point_Row[0] - 1; j >= 13; j--) //���Ͻǵ�
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
                 for (j = Inv_Point_Row[0] - 10; j >= 13; j--) // ���Ͻǵ�
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
                Out_Island_Left_Turn = true;//��Բ�����
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
        for(i = 30; i > 8; i--)  //8��ֹ�ҵ����������Ƶ�
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
void Left_Island_update()  //���հ����Բ��update
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
                imageLine.Point_Center[j] = 0.5 * imageLine.Point_Left[j] + 0.5 * imageLine.Point_Right[j];   //0.5��0.5ʱ�뻷ǰ�῿��
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
            servo_param.Servo_filter = FIXED_LEFT_ANGLE;//�̶����
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
                 //��������� �ҳ����½ǵ� ���½ǵ��Ϸ�����������
                 for (j = i - 1; j > i - 3; j--)
                 {
                      if (imageLine.Exist_Right[i]
                          && Rectangle_isWhite(imageLine.Point_Right[i] + 10, i - 3, 10, 5)
                          && (imageLine.Point_Right[i + 1] >= imageLine.Point_Right[i])
                          && (imageLine.Point_Right[i] < (imageLine.Point_Right[j] - 8))  //����ͬ
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
              for (i = In_Down_Point_Row - 2; i >= 4; i--)   //*********   In_Down_Point_Row - 4���޸�
              {
                  for (j = i - 1; j > i - 3; j--) // ��2�������Ƶ�
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
                for (j = i - 1; j > i - 5; j--) // �����½ǵ�
                {
                     if (imageLine.Exist_Right[i] && !imageLine.Exist_Right[j-3] && !imageLine.Exist_Right[j - 4]
                         && (right_inexist_number(j, 10) >= 5)
                         && imageLine.Point_Right[i] < (imageLine.Point_Right[j] - 5)
                         && imageLine.Point_Right[j] <= imageLine.Point_Right[j - 1])
                     {
                         In_Down_Point_Row = i;
                         In_Down_Point = imageLine.Point_Right[i];
                         In_Down_Flag = true;
                         if (In_Down_Point_Row > 53)          //*********53���޸�
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
               if(!In_Up_Point_Flag)   //���û���ҵ��Ͻǵ㣬������������Ƶ�
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
               if(Inv_Point_Row[0])   //����ҵ������Ƶ㣬�����Ͻǵ�
               {
                   In_Up_Point_Row = 0;
                   In_Up_Point = 0;
                   In_Up_Point_Flag = false;
                   for(j = Inv_Point_Row[0] - 1; j >= 12; j--)   //���Ͻǵ㣬 ħ��12�������Ͻǵ��ҵ�ʱ����Խ��Խ����
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
                  && (imageLine.Point_Left[i] >= imageLine.Point_Left[i+1])  //�ǵ��·�б��
                  && (imageLine.Point_Left[i-1] >= imageLine.Point_Left[i-2])) //�ǵ��Ϸ�ƽ���
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

void Right_Island_detect()  //���հ����Բ��detect
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
                //��������� �ҳ����½ǵ� ���½ǵ��Ϸ�����������
                for (j = i - 1; j > i - 3; j--)
                {
                    if (imageLine.Exist_Right[i]
                        && Rectangle_isWhite(imageLine.Point_Right[i] + 10, i - 3, 10, 5)
                        && (imageLine.Point_Right[i + 1] >= imageLine.Point_Right[i])
                        && (imageLine.Point_Right[i] < (imageLine.Point_Right[j] - 8))  //����ͬ
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
            for (i = In_Down_Point_Row - 2; i >= 4; i--)   //*********   In_Down_Point_Row - 4���޸�
            {
                for (j = i - 1; j > i - 3; j--) // ��2�������Ƶ�
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
void Right_Island_logic()   //���հ����Բ��logic
{
    if(Right_Island_flag1 && Right_Island_state)
    {
        if (Search_In_Down_Point_Flag)
        {
            for (i = 57; i > 6; i--)
            {
                for (j = i - 1; j > i - 3; j--) // �����½ǵ�
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
                            break;              //breakλ������ͬ
                        }
                    } else{
                        In_Down_Flag = false;
                        Search_In_Down_Point_Flag = true;
                    }
                }
                if(In_Down_Point_Row == i) break;
            }
        } else if (!Search_In_Down_Point_Flag){
            for (i = In_Down_Point_Row - 10; i >= 15; i--)   //********����һ��
            {
                for (j = i - 1; j > i - 3; j--)            // ��2�������Ƶ�
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
               && (imageLine.Point_Left[i-1] >= imageLine.Point_Left[i-2]) //�ǵ��Ϸ�ƽ���
               && (2*imageLine.Point_Left[i] >= imageLine.Point_Left[i+1]))  //�ǵ��·�б��
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
} //���հ����Բ��logic
void Right_Island_update()  //���հ����Բ��update
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
                imageLine.Point_Center[j] = 0.5 * imageLine.Point_Left[j] + 0.5 * imageLine.Point_Right[j];   //0.5��0.5ʱ�뻷ǰ�῿��
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

} //���հ����Բ��update
//-------------------------------------------------------------------------------------------------------------------
// �������     ����һ�����㣬����ѡȡleft_search_colume����������ѡȡup_search_row�������γɾ��Σ�
//              ��������ȫΪ�׵㣬�򷵻�true�����ںڵ��򷵻�false
// ����˵��     �����������x�������������y�������ҵ�����left_search_colume�������ҵ�����up_search_row
// ���ز���     bool��
// ʹ��ʾ��     Rectangle_isWhite(imageLine.Point_Left[j], j - 2, 8, 5)
// ��ע��Ϣ     ���ڴ���Բ�������ж�
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

/**************************************�������****************************************************/
/*�������ߴ��*/
bool MediumLineCal(volatile float* final)
{
    uint8 i;
    static float lastCenter = 47;
    short temp_dev = 0;


    //ֱ������� Ŀ���С��ٶ�ת���߼�
    if(Left_Island_state || Right_Island_state)
    {
        AIM_LINE = AIM_LINE_MUTATION;
        Purpost_Speed = SPEED_MUTATION;
        pid_down_left_limit = 10000;  //��������
        pid_up_left_limit = 7000;    //��������
        pid_down_right_limit = 10000; //��������
        pid_up_right_limit = 7000;   //��������
    }
    else
    {
        if(Mutation_Flag)
        {
            AIM_LINE = AIM_LINE_MUTATION;
            Purpost_Speed = SPEED_MUTATION;
            pid_down_left_limit = 10000;  //��������
            pid_up_left_limit = 7000;    //��������
            pid_down_right_limit = 10000; //��������
            pid_up_right_limit = 7000;   //��������
        }
        else if(Turn_Flag)
        {
            AIM_LINE = AIM_LINE_TURN;
            Purpost_Speed = SPEED_TURN;
            pid_down_left_limit = 10000;  //��������
            pid_up_left_limit = 7000;    //��������
            pid_down_right_limit = 10000; //��������
            pid_up_right_limit = 7000;   //��������
        }
        else if(Straight_Flag)
        {
            AIM_LINE = AIM_LINE_STRAIGHT;
            Purpost_Speed = SPEED_STRAIGHT;
            pid_down_left_limit = 5000;  //��������
            pid_up_left_limit = 7000;    //��������
            pid_down_right_limit = 5000; //��������
            pid_up_right_limit = 7000;   //��������
        }
    }

    while(!imageLine.Exist_Center[AIM_LINE])
    {
        AIM_LINE++;
    }

    //ѡ��AIM_LINE
    /*
    if (!imageLine.Exist_Center[AIM_LINE] && imageLine.Lost_Center)
    {
        uint8_t down_cnt = 0, up_cnt = 0;
        int new_line_down = -1, new_line_up = -1;
        int i;

        // �����������к�����
        for (i = AIM_LINE + 1; i < AIM_LINE + 15 && i < ImageH; i++)
        {
            if (imageLine.Exist_Center[i])
            {
                new_line_down = i;
                break;
            }
            down_cnt++;
        }

        // �����������кż�С��
        for (i = AIM_LINE - 1; i >= 0 && i > AIM_LINE - 10; i--)
        {
            if (imageLine.Exist_Center[i])
            {
                new_line_up = i;
                break;
            }
            up_cnt++;
        }

        // ѡ������������Ч��
        if (new_line_down != -1 || new_line_up != -1)
        {
            if (new_line_up != -1 && (up_cnt < down_cnt || new_line_down == -1))
                AIM_LINE = new_line_up;
            else
                AIM_LINE = new_line_down;

            // ��ѡ���кŵ��������裩
            if (AIM_LINE <= AIM_LINE_STRAIGHT) AIM_LINE = AIM_LINE_STRAIGHT;
            else if (AIM_LINE >= AIM_LINE_TURN) AIM_LINE = AIM_LINE_TURN;
        }
        else
        {
            imageLine.Lost_Center = true;
        }
    }*/

    //����ERR
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
                //�����������
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
        *(final) = *(final + 1);        // �����򱣳��ϴε����
        //Purpost_Speed = 0;
        return false;
    }
    return false;
}

void island_flag_init()
{
    Search_In_Down_Point_Flag = false;
    Out_Island_Transition_Flag = false;  // �Ƿ��ڻ������ɽ׶�
    Out_Island_Turn_Counter = 0;         // ���ʱ�������

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

/*�������ߴ��*/
void updateMediumLine(void)
{
    bool rst = true;
    static uint8_t lostTime = 0;


    /************************�������ߴ��*****************************/
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
    // �������ұ�
    uint8 gamma_LUT[256];
    for (int i = 0; i < 256; i++)
    {
        gamma_LUT[i] = (uint8)constrain_int16(powf(i / 255.0, 1.0f / gamma) * 255.0,0,255);
    }

    // �͵��޸�ͼ��
    for (int i = 0; i < ImageH; i++)
    {
        for (int j = 0; j < ImageW; j++)
        {
            image[i][j] = gamma_LUT[image[i][j]];
        }
    }
}
/*���ض����еĸ���*/
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

    //����
    for(i = 0; i <= 1; i++)
    {
        White_Column[i] = 0;
    }


    //Բ��������
    if(Right_Island_state || Left_Island_state)
    {
        return;
    }


    //�������ϵõ���46��47�еİ׵���Ŀ
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

    //���ݰ׵㺬��������������
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

