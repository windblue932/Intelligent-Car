#include "zf_common_headfile.h"

motor1 motor_l;
motor1 motor_r;

float Inc_L;
float Inc_R;

int16_t count_stop_near = 0;
int16_t count_stop_loss = 0;
int creazy_count = 0;
int stem_count = 0;
float deltaspeed = 0;
float target_speed = 0;
float difference_speed;

//�����ʼ��
void ServoInit(unsigned short freq) {
    pwm_init(Servo_PWM_PIN, freq, Servo_Center_Mid);

}

//�������
void ServoCtrl(uint16_t duty) {

     if (duty >= Servo_Left_Max)                  //���Ʒ�ֵ
     duty = Servo_Left_Max;
     else if (duty <= Servo_Right_Min)            //���Ʒ�ֵ
     duty = Servo_Right_Min;
     CurrentServoDty = duty;
     pwm_set_duty(Servo_PWM_PIN, duty);

}

//��������ʼ��
void Encoder_Init(void){
    //�ұ�����
    encoder_dir_init(TIM6_ENCODER,TIM6_ENCODER_CH1_P20_3,TIM6_ENCODER_CH2_P20_0);
    //�������
    encoder_dir_init(TIM4_ENCODER,TIM4_ENCODER_CH1_P02_8,TIM4_ENCODER_CH2_P00_9);
}

//����������
void Encoder_Data_Get(void){
  motor_r.encoder_raw=encoder_get_count(TIM6_ENCODER);
  motor_r.encoder_speed=motor_r.encoder_raw;
  motor_r.total_encoder+=motor_r.encoder_raw;
  encoder_clear_count(TIM6_ENCODER);

  motor_l.encoder_raw=encoder_get_count(TIM4_ENCODER);
  motor_l.encoder_speed=-motor_l.encoder_raw;
  motor_l.total_encoder+=motor_l.encoder_raw;
  encoder_clear_count(TIM4_ENCODER);
}

//�����ʼ��
void MotorInit(unsigned short freq) {
    gpio_init(Motor_GPIO_Left, GPO, GPIO_HIGH, GPO_PUSH_PULL);
    gpio_init(Motor_GPIO_Right, GPO, GPIO_HIGH, GPO_PUSH_PULL);

    pwm_init(Motor_PWM_Left, freq, 0);
    pwm_init(Motor_PWM_Right, freq, 0);

}

//�������
void Speed_Set(pwm_channel_enum pin1, gpio_pin_enum pin2, int speed ,uint8 plus, uint8 minus){
    if(speed>0)
    {
        pwm_set_duty(pin1, speed);
        gpio_set_level(pin2, plus);
    }
    else
    {
        pwm_set_duty(pin1, -speed);
        gpio_set_level(pin2, minus);
    }
}

//����ٶȱջ�����
void Motor_Control(int Speed_L, int Speed_R){
    motor_l.target_speed=Speed_L;
    motor_r.target_speed=Speed_R;

    Inc_L = PIDIncCtrl(&motor_pid_l,(float)motor_l.encoder_speed,(float)motor_l.target_speed);
    Inc_R = PIDIncCtrl(&motor_pid_r,(float)motor_r.encoder_speed,(float)motor_r.target_speed);

    motor_l.duty=limit_int(-pid_out_limit,motor_l.duty + Inc_L,pid_out_limit);
    motor_r.duty=limit_int(-pid_out_limit,motor_r.duty + Inc_R,pid_out_limit);

    Speed_Set(Motor_PWM_Left,Motor_GPIO_Left,motor_l.duty,0,1);
    Speed_Set(Motor_PWM_Right,Motor_GPIO_Right,motor_r.duty,0,1);
}

//������������
void Motor_ControlL(int Speed_L){
    motor_l.target_speed=Speed_L;

    motor_l.duty=limit_int(-pid_out_limit,motor_l.duty+PIDIncCtrl(&motor_pid_l,(float)motor_l.encoder_speed,(float)motor_l.target_speed),pid_out_limit);

    Speed_Set(Motor_PWM_Left,Motor_GPIO_Left,motor_l.duty,0,1);
}

//�ҵ����������
void Motor_ControlR(int Speed_R){
    motor_r.target_speed=Speed_R;

    motor_r.duty=limit_int(-pid_out_limit,motor_r.duty+PIDIncCtrl(&motor_pid_r,(float)motor_r.encoder_speed,(float)motor_r.target_speed),pid_out_limit);

    Speed_Set(Motor_PWM_Right,Motor_GPIO_Right,motor_r.duty,0,1);
}

//ͣ������
void shut_down(void){

    pwm_set_duty(FAN_PWM_LEFT, 0);
    pwm_set_duty(FAN_PWM_RIGT, 0);

    Speed_Set(Motor_PWM_Left,Motor_GPIO_Left,0,0,1);
    Speed_Set(Motor_PWM_Right,Motor_GPIO_Right,0,0,1);

    pit_all_close();
}

//�����ת���
void motor_creazy_protect(void){
    if(my_abs(motor_l.duty) >= (pid_out_limit - 100) || my_abs(motor_r.duty) >= (pid_out_limit - 100))
        creazy_count++;
    else
        creazy_count = 0;
    if(creazy_count >= 200){
        shut_down();
        Flag.Motor_Creazy = true;
    }

}

//�����ת���
void motor_stem_protect(void){
    if(my_abs(motor_l.encoder_speed) <= 20 || my_abs(motor_r.encoder_speed) <= 20)
        stem_count++;
    else
        stem_count = 0;
    if(creazy_count >= 200 && isr_count >= 2000){
        shut_down();
        Flag.Motor_Stem = true;
    }
}

//���տ���
void backspeedcontrol(float delta,float delta_target,float base_speed,float k1,float motorerror){

    //ң��**P20_11 = A�����������P20_12 = B��ֹͣ�����P20_13 = C����������ѹ���ȣ�
    boolean p20_11 = IfxPort_getPinState(&MODULE_P20, 11);
    boolean p20_12 = IfxPort_getPinState(&MODULE_P20, 12);
    boolean p20_13 = IfxPort_getPinState(&MODULE_P20, 13);

        if(!p20_11 && p20_12){//A
            Flag.GOGOGO = true;
        }
        else if(p20_11 && !p20_12){//B
            Flag.GOGOGO = false;
        }
        else{
            ;
        }

    //�ٶȻ�
    float delta_error = delta - delta_target; //��������
    deltaspeed = PidLocCtrl(&motor_pid_dist,(float)delta_error); //�󳵻����ٶ�PID
    //target_speed = base_speed - k1*deltaspeed + add_speed;
    target_speed = base_speed - k1*deltaspeed;

    //���뻷&&������ʩ
    if(Flag.GOGOGO){

        if((delta >= 18 && abs(motorerror) <= 3) || delta >= 19){ //����ͣ��

            count_stop_near ++;

            count_stop_loss = constrain_int16_LOW(count_stop_loss --, 0);

            Motor_Control(0,0);

            if(count_stop_near >= 200 && isr_count >= 2000) shut_down();

        }
        else if(!rst || abs(img1_xx - img2_xx) >= 4){ //����ͣ��

            count_stop_near = 0;

            count_stop_loss = constrain_int16_MAX(count_stop_loss ++, 30);

            if(count_stop_loss >= 10) shut_down();

        }
        else{ //������ʻ

            count_stop_near = 0;

            count_stop_loss = constrain_int16_LOW(count_stop_loss --, 0);

                if(difference_speed <= 0){

                    Motor_Control(target_speed + KLI_DEC * difference_speed, target_speed - KLO_DEC * difference_speed);
                    //Motor_Control(target_speed + KLI_DEC * output, target_speed - KLO_DEC * output);
                }
                else{

                    Motor_Control(target_speed + KRO_DEC * difference_speed, target_speed - KRI_DEC * difference_speed);
                    //Motor_Control(target_speed + KLI_DEC * output, target_speed - KLO_DEC * output);
                }
        }
    }
    else Motor_Control(0,0);
}
