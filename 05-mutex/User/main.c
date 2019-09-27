#include "stm32f10x.h"
#include "bsp_usart.h"
#include "tos.h"


k_mutex_t test_mutex_00;

k_task_t task1;
k_task_t task2;
k_stack_t task_stack1[1024];
k_stack_t task_stack2[1024];


void test_task1(void *Parameter)
{
    k_err_t err;
    printf("task1 running\r\n");
    while(1)
    {
        // hold the mutex first
        err = tos_mutex_pend(&test_mutex_00);
        if (err == K_ERR_NONE)
            printf("task1 mutex pend success\r\n");
        
        if (err == K_ERR_PEND_DESTROY)
        {
            printf("queue is destroy\r\n");
            tos_task_delay(TOS_TIME_FOREVER - 1);
        }
        
        // yeild cpu to task2
        tos_task_delay(2000);  
        
        err = tos_mutex_post(&test_mutex_00);
        if (err == K_ERR_NONE)
            printf("task1 mutex post success\r\n");
        tos_task_delay(2000);
    }
}

void test_task2(void *Parameter)
{
    k_err_t err;
    printf("task2 running\r\n");
    while(1)
    {
        // hold the mutex first, time is 0
        err = tos_mutex_pend_timed(&test_mutex_00, 0);
        if (err == K_ERR_NONE)
            printf("task2 mutex pend success\r\n");

        if (err != K_ERR_NONE)
            printf("task2 mutex pend fail! code : %d \r\n",err);
        
        err = tos_mutex_post(&test_mutex_00);
        if (err == K_ERR_NONE)
            printf("task1 mutex post success\r\n");
        
        tos_task_delay(1000);       
    }
}
/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
    k_err_t err;
    
    /*初始化USART 配置模式为 115200 8-N-1，中断接收*/
    USART_Config();

    printf("Welcome to TencentOS tiny\r\n");

    tos_knl_init(); // TOS Tiny kernel initialize
    
    tos_robin_config(TOS_ROBIN_STATE_ENABLED, (k_timeslice_t)500u);
    
    printf("create test_mutex_00\r\n");
    err = tos_mutex_create(&test_mutex_00);
    if(err != K_ERR_NONE)
        printf("TencentOS Create test_mutex_00 fail! code : %d \r\n",err);
    
    printf("create task1\r\n");
    err = tos_task_create(&task1, 
                          "task1", 
                          test_task1,
                          NULL, 
                          3, 
                          task_stack1,
                          1024,
                          20);
    if(err != K_ERR_NONE)
        printf("TencentOS Create task1 fail! code : %d \r\n",err);
    
    printf("create task2\r\n");
    err = tos_task_create(&task2, 
                          "task2", 
                          test_task2,
                          NULL, 
                          4, 
                          task_stack2,
                          1024,
                          20);
    if(err != K_ERR_NONE)
        printf("TencentOS Create task2 fail! code : %d \r\n",err);
    
    tos_knl_start(); // Start TOS Tiny

}

