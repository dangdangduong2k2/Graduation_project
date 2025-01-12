/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "math.h"
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define max 1000
#define min 99
#define PI 3.14159265359
#define dirX_Pin GPIO_PIN_6 // Replace with your actual pin
#define dirY_Pin GPIO_PIN_8 // Replace with your actual pin
#define dirZ_Pin GPIO_PIN_15 // Replace with your actual pin
#define stepX_Pin GPIO_PIN_5 // Replace with your actual pin
#define stepY_Pin GPIO_PIN_9 // Replace with your actual pin
#define stepZ_Pin GPIO_PIN_4 // Replace with your actual pin

#define dirX_GPIO_Port GPIOB // Replace with your actual GPIO port
#define dirY_GPIO_Port GPIOB // Replace with your actual GPIO port
#define dirZ_GPIO_Port GPIOA // Replace with your actual GPIO port
#define stepX_GPIO_Port GPIOB // Replace with your actual GPIO port
#define stepY_GPIO_Port GPIOB // Replace with your actual GPIO port
#define stepZ_GPIO_Port GPIOB // Replace with your actual GPIO port
#define ACCELERATION_STEPS 100 // Replace with your actual GPIO port
#define MAX_SPEED_DELAY 500

uint16_t pwm;
uint8_t run;
float delay_time = 200; // Ð? tr? t?i thi?u trong us
float GX,GY;
float tempx,tempy;
float L1=235;
float L2=212;
int16_t x,y;
int32_t z;
uint8_t stopx,stopy,stopz;
uint8_t data[12];
uint16_t data_size;
uint8_t string[2] ="OK";
void clear_data(uint8_t *arr, int size) 
{
  for (int i = 0; i < size; i++) 
  { 
    arr[i] = 0x00; // Ho?c b?t k? giá tr? nào b?n mu?n d?t l?i 
  } 
}
void start_status(void)
{
  for(int i=0; i<10;i++)
  {
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15,GPIO_PIN_SET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15,GPIO_PIN_RESET);
    HAL_Delay(100);
  }
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15,GPIO_PIN_SET);
}
void delay_us(uint16_t us)
{
    __HAL_TIM_SET_COUNTER(&htim2, 0);  // Ð?t l?i b? d?m Timer v? 0
    HAL_TIM_Base_Start(&htim2);        // B?t d?u Timer
    while (__HAL_TIM_GET_COUNTER(&htim2) < us);  // Ch? cho d?n khi d?m d? s? micro giây
    HAL_TIM_Base_Stop(&htim2);         // D?ng Timer
}
void move(float angelX, float angelY,float*tempx,float*tempy)
{
		if(angelX!=*tempx)
		{
			
			angelX=angelX-(*tempx);
			*tempx=angelX+(*tempx);
			
		}
		else
		{
			angelX=0;
			*tempx=*tempx;
		}
		if(angelY!=*tempy)
		{
			
			angelY=angelY-(*tempy);
			*tempy=angelY+(*tempy);
		}
		else
		{
			angelY=0;
			*tempy=*tempy;
		}
    float pulseX = fabs(2*angelX) * 31.08;
    float pulseY_ro = fabs(2*angelY) *51.771;
    float pulseY_sup = fabs(2*angelX) *51.771 * 0.532;
    float pulseY;
    float higher_stepper;
    float lower_stepper;

    if (angelX < 0){
        HAL_GPIO_WritePin(dirX_GPIO_Port, dirX_Pin, GPIO_PIN_SET);
        if (angelY <= 0){
            HAL_GPIO_WritePin(dirY_GPIO_Port, dirY_Pin, GPIO_PIN_SET);
            pulseY = pulseY_ro + pulseY_sup;
        }
        if (angelY > 0){
            if (pulseY_ro > pulseY_sup){
                HAL_GPIO_WritePin(dirY_GPIO_Port, dirY_Pin, GPIO_PIN_RESET);
                pulseY = pulseY_ro - pulseY_sup;
            } else {
                HAL_GPIO_WritePin(dirY_GPIO_Port, dirY_Pin, GPIO_PIN_SET);
                pulseY = -pulseY_ro + pulseY_sup;
            }
        }
    } else if (angelX > 0){
        HAL_GPIO_WritePin(dirX_GPIO_Port, dirX_Pin, GPIO_PIN_RESET);
        if (angelY >= 0){
            HAL_GPIO_WritePin(dirY_GPIO_Port, dirY_Pin, GPIO_PIN_RESET);
            pulseY = pulseY_ro + pulseY_sup;
        }
        if (angelY < 0){
            if (pulseY_ro > pulseY_sup){
                HAL_GPIO_WritePin(dirY_GPIO_Port, dirY_Pin, GPIO_PIN_SET);
                pulseY = pulseY_ro - pulseY_sup;
            } else {
                HAL_GPIO_WritePin(dirY_GPIO_Port, dirY_Pin, GPIO_PIN_RESET);
                pulseY = -pulseY_ro + pulseY_sup;
            }
        }
    } else {
        if (angelY < 0){
            HAL_GPIO_WritePin(dirY_GPIO_Port, dirY_Pin, GPIO_PIN_SET);
            pulseY = pulseY_ro + pulseY_sup;
        } else if (angelY > 0){
            HAL_GPIO_WritePin(dirY_GPIO_Port, dirY_Pin, GPIO_PIN_RESET);
            pulseY = pulseY_ro + pulseY_sup;
        }
    }

    int higher_steppin;
    int lower_steppin;

    if (pulseX > pulseY){
        higher_stepper = pulseX;
        lower_stepper = pulseY;
        higher_steppin = stepX_Pin;
        lower_steppin = stepY_Pin;
    } else if (pulseX < pulseY){
        higher_stepper = pulseY;
        lower_stepper = pulseX;
        higher_steppin = stepY_Pin;
        lower_steppin = stepX_Pin;
    } else {
        higher_stepper = pulseX;
        lower_stepper = pulseY;
        higher_steppin = stepX_Pin;
        lower_steppin = stepY_Pin;
    }

    float rate = higher_stepper / lower_stepper;
    int temp = 0;
    int steps = (int)higher_stepper;

    for (int i = 0; i <= steps; i++){
        if(i / rate - temp >= 1){
            HAL_GPIO_WritePin(GPIOB, lower_steppin, GPIO_PIN_SET); 
            temp++;
        }
        HAL_GPIO_WritePin(GPIOB, higher_steppin, GPIO_PIN_SET);
        delay_us((uint16_t)delay_time); 
        HAL_GPIO_WritePin(GPIOB, higher_steppin, GPIO_PIN_RESET); 
        HAL_GPIO_WritePin(GPIOB, lower_steppin, GPIO_PIN_RESET);
        delay_us((uint16_t)delay_time);  
    }
	
}
void movez(int pul)
{
  if(pul<0)
  {
    pul=-pul;
    HAL_GPIO_WritePin(dirZ_GPIO_Port, dirZ_Pin, GPIO_PIN_RESET);
  }
  else
  {
    pul=pul;
    HAL_GPIO_WritePin(dirZ_GPIO_Port, dirZ_Pin, GPIO_PIN_SET);
  }
  
  for (int i = 0; i <= pul; i++)
  {
    
        HAL_GPIO_WritePin(stepZ_GPIO_Port, stepZ_Pin, GPIO_PIN_SET);
        delay_us(50); 
        HAL_GPIO_WritePin(stepZ_GPIO_Port, stepZ_Pin, GPIO_PIN_RESET);
        delay_us(50);  
   
   
  }
}
void home(void)
{
  HAL_GPIO_WritePin(dirZ_GPIO_Port, dirZ_Pin, GPIO_PIN_SET);
  for (int i = 0; i <= 1000000; i++)
  {
    if(stopz!=1)
    {
        HAL_GPIO_WritePin(stepZ_GPIO_Port, stepZ_Pin, GPIO_PIN_SET);
        delay_us(20); 
        HAL_GPIO_WritePin(stepZ_GPIO_Port, stepZ_Pin, GPIO_PIN_RESET);
        delay_us(20);  
    }
    else
    {
      break;
    }
  }
  
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
  for (int i = 0; i <= 1000000; i++)
  {
    if(stopy!=1)
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);
        delay_us(100); 
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);
        delay_us(100);  
    }
    else
    {
      break;
    }
  }
  
  delay_time=55;
  tempx=0;
  tempy=0;
  move(0,-168.4,&tempx,&tempy);
  
  
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
  for (int i = 0; i <= 1000000; i++)
  {
    if(stopx!=1)
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
        delay_us(250); 
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
        delay_us(250);  
    }
    else
    {
      break;
    }
  }
   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
  for (int i = 0; i <= 1000000; i++)
  {
    if(stopy!=1)
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);
        delay_us(100); 
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);
        delay_us(100);  
    }
    else
    {
      break;
    }
  }
  tempx=0;
  tempy=0;
  delay_time=55;
  move(23.5,-169,&tempx,&tempy);
  tempx=0;
  tempy=0;
  delay_time=70;
}
void inverse_kinematics(float x, float y, float L1, float L2, float *theta1, float *theta2)
{
    // Tính toán hai nghi?m c?a góc th? hai
    float cos_theta2 = (x * x + y * y - L1 * L1 - L2 * L2) / (2 * L1 * L2);
    cos_theta2 = fmin(fmax(cos_theta2, -1.0), 1.0);  // Gi?i h?n giá tr? c?a cos_theta2 t? -1 d?n 1
    float theta2_1 = acos(cos_theta2);
    float theta2_2 = -theta2_1;

    // Tính toán hai nghi?m c?a góc th? nh?t
    float theta1_1 = atan2(y, x) - asin(fmin(fmax((L2 * sin(theta2_1)) / sqrt(x * x + y * y), -1.0), 1.0));
    float theta1_2 = atan2(y, x) - asin(fmin(fmax((L2 * sin(theta2_2)) / sqrt(x * x + y * y), -1.0), 1.0));

    // Chuy?n d?i góc t? radian sang d?
    float theta1_1_deg = theta1_1 * 180.0 / PI;
    float theta2_1_deg = theta2_1 * 180.0 / PI;
    float theta1_2_deg = theta1_2 * 180.0 / PI;
    float theta2_2_deg = theta2_2 * 180.0 / PI;

    // Gi?i h?n giá tr? c?a theta1 và theta2 theo yêu c?u
    if (-20 <= theta1_1_deg && theta1_1_deg <= 200 && -170 <= theta2_1_deg && theta2_1_deg <= 170) {
        *theta1 = theta1_1_deg;
        *theta2 = theta2_1_deg;
    } else if (-20 <= theta1_2_deg && theta1_2_deg <= 200 && -170 <= theta2_2_deg && theta2_2_deg <= 170) {
        *theta1 = theta1_2_deg;
        *theta2 = theta2_2_deg;
    } else {
        // N?u không tìm du?c giá tr? th?a mãn, gán giá tr? d?c bi?t
        *theta1 = NAN;
        *theta2 = NAN;
    }
}
void check_ipnut(void)
{
  if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_1)==0)
  {
    stopx=1;
  }
  else
  {
    stopx=0;
  }
  if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4)==0)
  {
    stopy=1;
  }
  else
  {
    stopy=0;
  }
  if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==0)
  {
    stopz=1;
  }
  else
  {
    stopz=0;
  }
  if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)==0)
  {
    run=2;
  }
  
}
void robot_check(void)
{
    if(run!=0)
    {
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15,GPIO_PIN_RESET);
    }
    else
    {
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15,GPIO_PIN_SET);
    }
    if(run==1)
    {
        inverse_kinematics(x,y,L1,L2,&GX,&GY);
        move(GX,GY,&tempx,&tempy);
        movez(z);
        run=0;
    }
    else if(run==2)
    {
        home();
        run=0;
    }
}
void convert_xy(char dat1, char dat2, int16_t *x, int16_t *y)
{
  if(dat1=='a')
  {
    if(dat2=='1') { *x=112; *y=345; }
    else if(dat2=='2') { *x=111; *y=311; }
    else if(dat2=='3') { *x=113; *y=279; }
    else if(dat2=='4') { *x=113; *y=247; }
    else if(dat2=='5') { *x=113; *y=214; }
    else if(dat2=='6') { *x=116; *y=184; }
    else if(dat2=='7') { *x=114; *y=149; }
    else if(dat2=='8') { *x=111; *y=115; }
  }
  else if(dat1=='b')
  {
    if(dat2=='1') { *x=76; *y=342; }
    else if(dat2=='2') { *x=79; *y=311; }
    else if(dat2=='3') { *x=81; *y=279; }
    else if(dat2=='4') { *x=80; *y=240; }
    else if(dat2=='5') { *x=81; *y=215; }
    else if(dat2=='6') { *x=82; *y=183; }
    else if(dat2=='7') { *x=81; *y=149; }
    else if(dat2=='8') { *x=80; *y=115; }
  }
  else if(dat1=='c')
  {
    if(dat2=='1') { *x=44; *y=342; }
    else if(dat2=='2') { *x=47; *y=312; }
    else if(dat2=='3') { *x=49; *y=280; }
    else if(dat2=='4') { *x=48; *y=250; }
    else if(dat2=='5') { *x=49; *y=218; }
    else if(dat2=='6') { *x=48; *y=185; }
    else if(dat2=='7') { *x=49; *y=151; }
    else if(dat2=='8') { *x=48; *y=118; }
  }
  else if(dat1=='d')
  {
    if(dat2=='1') { *x=12; *y=344; }
    else if(dat2=='2') { *x=14; *y=312; }
    else if(dat2=='3') { *x=16; *y=282; }
    else if(dat2=='4') { *x=16; *y=250; }
    else if(dat2=='5') { *x=15; *y=218; }
    else if(dat2=='6') { *x=15; *y=184; }
    else if(dat2=='7') { *x=16; *y=155; }
    else if(dat2=='8') { *x=16; *y=119; }
  }
  else if(dat1=='e')
  {
    if(dat2=='1') { *x=-23; *y=344; }
    else if(dat2=='2') { *x=-18; *y=312; }
    else if(dat2=='3') { *x=-16; *y=283; }
    else if(dat2=='4') { *x=-18; *y=249; }
    else if(dat2=='5') { *x=-19; *y=218; }
    else if(dat2=='6') { *x=-18; *y=187; }
    else if(dat2=='7') { *x=-18; *y=156; }
    else if(dat2=='8') { *x=-19; *y=120; }
  }
  else if(dat1=='f')
  {
    if(dat2=='1') { *x=-53; *y=344; }
    else if(dat2=='2') { *x=-52; *y=312; }
    else if(dat2=='3') { *x=-50; *y=282; }
    else if(dat2=='4') { *x=-51; *y=250; }
    else if(dat2=='5') { *x=-51; *y=219; }
    else if(dat2=='6') { *x=-51; *y=189; }
    else if(dat2=='7') { *x=-52; *y=155; }
    else if(dat2=='8') { *x=-51; *y=121; }
  }
  else if(dat1=='g')
  {
    if(dat2=='1') { *x=-87; *y=343; }
    else if(dat2=='2') { *x=-84; *y=313; }
    else if(dat2=='3') { *x=-84; *y=282; }
    else if(dat2=='4') { *x=-83; *y=250; }
    else if(dat2=='5') { *x=-85; *y=218; }
    else if(dat2=='6') { *x=-85; *y=189; }
    else if(dat2=='7') { *x=-86; *y=155; }
    else if(dat2=='8') { *x=-85; *y=120; }
  }
  else if(dat1=='h')
  {
    if(dat2=='1') { *x=-117; *y=344; }
    else if(dat2=='2') { *x=-117; *y=312; }
    else if(dat2=='3') { *x=-117; *y=281; }
    else if(dat2=='4') { *x=-116; *y=249; }
    else if(dat2=='5') { *x=-118; *y=220; }
    else if(dat2=='6') { *x=-118; *y=187; }
    else if(dat2=='7') { *x=-119; *y=157; }
    else if(dat2=='8') { *x=-118; *y=120; }
  }
}
uint16_t convert_pos_z(char dat) 
{
    switch (dat) 
    {
        case 'r' : return 60000;//
        case 'n' : return 60000;
        case 'b' : return 55000;//
        case 'k' : return 46000;//
        case 'q' : return 46000;//
        case 'p' : return 62000;//
        default: return 0;
    }
}
void data_process(void)
{
  if(data[5]=='*')
  {
    /////di toi x,y 1
    convert_xy(data[1],data[2],&x,&y);
    inverse_kinematics(x, y, L1, L2, &GX, &GY);
    move(GX,GY,&tempx,&tempy);
    /////tha ra
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_RESET);

    /////ha xuong
    movez(-convert_pos_z(data[0]));
    /////gap
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_SET);

    /////nang len
    movez(convert_pos_z(data[0]));
    /////di toi x,y 2
    convert_xy(data[3],data[4],&x,&y);
    inverse_kinematics(x, y, L1, L2, &GX, &GY);
    move(GX,GY,&tempx,&tempy);
    /////ha xuong
    movez(-convert_pos_z(data[0]));
    /////tha ra
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_RESET);
    HAL_Delay(1000);
    /////nang len
    movez(convert_pos_z(data[0]));
    //xoa data
    clear_data(data,12);
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_RESET);
    //ve home
    move(0,0,&tempx,&tempy);
    HAL_UART_Transmit(&huart1,string,2,1000);
  }
  else if(data[7]=='*')
  {
    ////di toi x,y 2
    convert_xy(data[5],data[6],&x,&y);
    inverse_kinematics(x, y, L1, L2, &GX, &GY);
    move(GX,GY,&tempx,&tempy);
    /////tha ra
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_RESET);
    /////ha xuong
    movez(-convert_pos_z(data[4]));
    /////gap
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_SET);
    /////nang len
    movez(convert_pos_z(data[4]));
    /////di toi x,y tha co
    move(0,0,&tempx,&tempy);
    /////tha ra
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_RESET);
    ////di toi x,y 1
    convert_xy(data[1],data[2],&x,&y);
    inverse_kinematics(x, y, L1, L2, &GX, &GY);
    move(GX,GY,&tempx,&tempy);
    /////ha xuong
    movez(-convert_pos_z(data[0]));
    /////gap
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_SET);
    /////nang len
    movez(convert_pos_z(data[0]));
    ////di toi x,y 2
    convert_xy(data[5],data[6],&x,&y);
    inverse_kinematics(x, y, L1, L2, &GX, &GY);
    move(GX,GY,&tempx,&tempy);
    /////ha xuong
    movez(-convert_pos_z(data[0]));
    /////tha ra
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_RESET);
    /////nang len
    movez(convert_pos_z(data[0]));
    //xoa data
    clear_data(data,12);
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_RESET);
    //ve home
    move(0,0,&tempx,&tempy);
    HAL_UART_Transmit(&huart1,string,2,1000);
  }
  else if(data[9]=='*')
  {
    /////di toi x,y 1
    convert_xy(data[1],data[2],&x,&y);
    inverse_kinematics(x, y, L1, L2, &GX, &GY);
    move(GX,GY,&tempx,&tempy);
    /////tha ra
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_RESET);
    /////ha xuong
    movez(-convert_pos_z(data[0]));
    /////gap
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_SET);
    /////nang len
    movez(convert_pos_z(data[0]));
    /////di toi x,y 2
    convert_xy(data[3],data[4],&x,&y);
    inverse_kinematics(x, y, L1, L2, &GX, &GY);
    move(GX,GY,&tempx,&tempy);
    /////ha xuong
    movez(-convert_pos_z(data[0]));
    /////tha ra
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_RESET);
    /////nang len
    movez(convert_pos_z(data[0]));
    /////di toi x,y 3
    convert_xy(data[7],data[8],&x,&y);
    inverse_kinematics(x, y, L1, L2, &GX, &GY);
    move(GX,GY,&tempx,&tempy);
    /////tha ra
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_RESET);
    /////ha xuong
    movez(-convert_pos_z(data[6]));
    /////gap
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_SET);
    /////nang len
    movez(convert_pos_z(data[6]));
    /////di toi x,y 4
    convert_xy(data[9],data[10],&x,&y);
    inverse_kinematics(x, y, L1, L2, &GX, &GY);
    move(GX,GY,&tempx,&tempy);
    /////ha xuong
    movez(-convert_pos_z(data[6]));
    /////tha ra
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_RESET);
    /////nang len
    movez(convert_pos_z(data[6]));
    clear_data(data,12);
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_SET);
    //ve home
    move(0,0,&tempx,&tempy);
    HAL_UART_Transmit(&huart1,string,2,1000);
  }
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim->Instance==TIM1)
  {
    check_ipnut();
    
  }
  if(htim->Instance==TIM4)
  {
    
  }
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) 
{ 
  if (huart->Instance == USART1) 
  {  
    HAL_UART_Receive_IT(&huart1,data,12);
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_RESET);
  }
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim1);
  HAL_TIM_Base_Start_IT(&htim4);
  
  HAL_UART_Receive_IT(&huart1,data,12);
  start_status();
 
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
   
   robot_check();
   data_process();
   
   HAL_Delay(50);

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 63;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 9999;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 99;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 0xffff;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 10999;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 999;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 63;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 9999;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6|GPIO_PIN_12|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6
                          |GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA0 PA1 PA4 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PA6 PA12 */
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 PB3 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB15 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB4 PB5 PB6 PB8
                           PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_8
                          |GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
