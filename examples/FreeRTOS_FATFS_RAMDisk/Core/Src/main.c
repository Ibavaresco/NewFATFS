/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <ffs.h>
#include <ram_diskio.h>
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
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
void StartDefaultTask(void *argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/*============================================================================*/
void LEDOn( void )
	{
	HAL_GPIO_WritePin(LED_WORK_GPIO_Port, LED_WORK_Pin, GPIO_PIN_SET );
	}
/*============================================================================*/
void LEDOff( void )
	{
	HAL_GPIO_WritePin(LED_WORK_GPIO_Port, LED_WORK_Pin, GPIO_PIN_RESET );
	}
/*============================================================================*/
/* USER CODE END 0 */

static int format( const char *path )
	{
	uint8_t	Buffer[512];

	mkfs( "ram", NULL, Buffer, sizeof Buffer );

	return 0;
	}

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
  FFS_AttachDrive( "ram", NULL, &RAMDisk_Driver );
  format( "ram" );
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while( 1 )
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 8;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}


/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_WORK_GPIO_Port, LED_WORK_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_WORK_Pin */
  GPIO_InitStruct.Pin = LED_WORK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_WORK_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

#define try			int __excode = 0; do
#define	throw(e)	(__excode=(e),({break;}))
#define catch(e)	while(0);if(__excode!=0)

static void Error( int n )
	{
	int	i;

	for( i = 0; i < 3; i++ )
		{
		for( int i = n; i; i-- )
			{
			LEDOn();
			vTaskDelay( 50 );
			LEDOff();
			vTaskDelay( 450 );
			}
		vTaskDelay( 1000 );
		}
	}

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */

void StartDefaultTask( void *argument )
	{
	/* USER CODE BEGIN 5 */
	int		MyFile;		/* File object */
	int		byteswritten, bytesread;									/* File write/read counts */
	char	wtext[] = "This is an example of RAMDisk with NewFATFS";	/* File write buffer */
	char	rtext[100];													/* File read buffer */
	char	DirObj[SizeOfDirObject()];
	ffs_fileinfo_t	fno;

	/* Infinite loop */
	while( 1 )
		{
#if 1
		while( 1 )
			{
			try
				{
				if( mount( "ram", "A:" ) != 0 )
					throw( 1 );

				setlabel( "A:RAMDISK" );
				getlabel( "A:", rtext, NULL );

				mkdir( "/xyz", 0 );

				/*##-4- Create and Open a new text file object with write access #####*/
				if(( MyFile = open( "Test.txt", O_CREAT | O_WRONLY | O_APPEND )) == -1 )
					throw( 3 );

				mkdir( "/abc", 0 );

				lseek64( MyFile, 0, SEEK_SET );

				//lseek( MyFile, 0, SEEK_END );

				//open( "X.TXT", O_CREAT | O_WRONLY );

				/*##-5- Write data to the text file ################################*/
				byteswritten = write( MyFile, wtext, strlen( wtext ));

				/*##-6- Close the open text file #################################*/
				close( MyFile );

				mkdir( "/xyz/uvw", 0 );

				if( byteswritten <= 0 )
					throw( 4 );

				/*##-7- Open the text file object with read access ###############*/
				if(( MyFile = open( "Test.txt", O_RDONLY )) == -1 )
					throw( 5 );

				/*##-8- Read data from the text file ###########################*/
				bytesread = read( MyFile, rtext, sizeof( rtext ));

				/*##-9- Close the open text file #############################*/
				close( MyFile );

				if( bytesread <= 0 )
					throw( 6 );

				opendir( &DirObj, "A:\\" );
				while( readdir( &DirObj, &fno ) > 0 )
					{}
				closedir( &DirObj );

				unlink( "A:Test.bak" );
				rename( "A:Test.txt", "Test.bak" );

				/*##-10- Compare read data with the expected data ############*/
				if( bytesread != byteswritten )
					throw( 7 );
				}
			catch( int __excode )
				{
				Error( __excode );
				}

			umount( "A:", 0 );
			}

#else
		(void)rtext;
		(void)wtext;
		(void)bytesread;
		(void)byteswritten;
		(void)MyFile;
		try
			{
			if( mount( "ram", "A:" ) != 0 )
				throw( 1 );

			FILE	*f;
			if(( f = fopen( "TestFile.txt", "rb+" )) == NULL )
				throw( 2 );

			int	v = 0;
			while( fscanf( f, "%d", &v ) > 0 )
				{
				fscanf( f, "," );
				asm volatile( "nop" );
				}

			if( fprintf( f, "%d,", v + 1 ) == 0 )
				throw( 3 );

			fclose( f );
			}
		catch( int __excode )
			{
			Error( __excode );
			}
#endif

		umount( "A:", 0 );
		}

	/* USER CODE END 5 */
	}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM14 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM14) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  asm volatile( "bkpt" );
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
