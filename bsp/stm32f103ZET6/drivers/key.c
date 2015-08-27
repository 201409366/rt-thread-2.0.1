#include "key.h"

ALIGN(RT_ALIGN_SIZE)

static BUTTON_T s_BtnUp;		/* UP 键 */
static BUTTON_T s_BtnDown;	/* DOWN 键 */
static BUTTON_T s_BtnLeft;	/* LEFT 键 */
static BUTTON_T s_BtnRight;		/* RIGHT键 */
static BUTTON_T s_BtnSelect;		/* SELECT键 */

/* 事件控制块 */
static struct rt_event keyEvent;

/*
	按键口线分配：

	定义4个函数，判断按键是否按下，返回值1 表示按下，0表示未按下
*/

static uint8_t IsUpKeyDown(void) 		{if (GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_11) == Bit_SET) return 0; return 1;}
static uint8_t IsDownKeyDown(void) 	{if (GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_8) == Bit_SET) return 0; return 1;}
static uint8_t IsLeftKeyDown(void) 	{if (GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_13) == Bit_SET) return 0; return 1;}
static uint8_t IsRightKeyDown(void) 		{if (GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_14) == Bit_SET) return 0; return 1;}																						  
static uint8_t IsSelectKeyDown(void) 		{if (GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_7) == Bit_SET) return 0; return 1;}																						  

static void GPIO_Configuration(void) //IO初始化
{
 	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG,ENABLE);
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
}


/*******************************************************************************
	函数名: InitButtonVar
	输  入:
	输  出:
	功能说明：初始化所有的按键变量，这个函数需要在systic中断启动钱调用1次
*/
static void InitButtonVar(void)
{
	/* 初始化add按键变量 */
	s_BtnUp.IsKeyDownFunc = IsUpKeyDown;	/* 判断按键按下的函数 */
	s_BtnUp.FilterTime = BUTTON_FILTER_TIME;	/* 按键滤波时间 */
	s_BtnUp.LongTime = BUTTON_LONG_TIME;		/* 长按时间 */
	s_BtnUp.Count = s_BtnUp.FilterTime / 2;	/* 计数器设置为滤波时间的一半 */
	s_BtnUp.State = 0;						/* 按键缺省状态，0为未按下 */
	s_BtnUp.KeyCodeDown = KEY_DOWN_UP;		/* 按键按下的键值代码 */
	s_BtnUp.KeyCodeUp = 0;			/* 按键弹起的键值代码 */
	s_BtnUp.KeyCodeLong = KEY_HOLD_UP;		/* 按键被持续按下的键值代码 */
	s_BtnUp.RepeatSpeed = BUTTON_FILTER_TIME;   /* 按键连发的速度，0表示不支持连发 */
  s_BtnUp.RepeatCount = 0;                    /* 连发计数器 */                

	/* 初始化dec按键变量 */
	s_BtnDown.IsKeyDownFunc = IsDownKeyDown;	/* 判断按键按下的函数 */
	s_BtnDown.FilterTime = BUTTON_FILTER_TIME;	/* 按键滤波时间 */
	s_BtnDown.LongTime = BUTTON_LONG_TIME;			/* 长按时间, 0表示不检测 */
	s_BtnDown.Count = s_BtnDown.FilterTime / 2;	/* 计数器设置为滤波时间的一半 */
	s_BtnDown.State = 0;							/* 按键缺省状态，0为未按下 */
	s_BtnDown.KeyCodeDown = KEY_DOWN_DOWN;		/* 按键按下的键值代码 */
	s_BtnDown.KeyCodeUp = 0;						/* 按键弹起的键值代码 */
	s_BtnDown.KeyCodeLong = KEY_HOLD_DOWN;		/* 按键被持续按下的键值代码 */
	s_BtnDown.RepeatSpeed = BUTTON_FILTER_TIME;   /* 按键连发的速度，0表示不支持连发 */
  s_BtnDown.RepeatCount = 0;                  /* 连发计数器 */                
	
	/* 初始化auto按键变量 */
	s_BtnLeft.IsKeyDownFunc = IsLeftKeyDown;	/* 判断按键按下的函数 */
	s_BtnLeft.FilterTime = BUTTON_FILTER_TIME;	/* 按键滤波时间 */
	s_BtnLeft.LongTime = BUTTON_LONG_TIME;						/* 长按时间 */
	s_BtnLeft.Count = s_BtnLeft.FilterTime / 2;	/* 计数器设置为滤波时间的一半 */
	s_BtnLeft.State = 0;							/* 按键缺省状态，0为未按下 */
	s_BtnLeft.KeyCodeUp = 0;						/* 按键弹起的键值代码，0表示不检测 */
	s_BtnLeft.KeyCodeDown = KEY_DOWN_LEFT;		/* 按键按下的键值代码 */
	s_BtnLeft.KeyCodeLong = KEY_HOLD_LEFT;					/* 按键被持续按下的键值代码，0表示不检测 */
	s_BtnLeft.RepeatSpeed = BUTTON_FILTER_TIME;                                                /* 按键连发的速度，0表示不支持连发 */
 	s_BtnLeft.RepeatCount = 0;                                                /* 连发计数器 */                

	/* 初始化reset按键变量 */
	s_BtnRight.IsKeyDownFunc = IsRightKeyDown;		/* 判断按键按下的函数 */
	s_BtnRight.FilterTime = BUTTON_FILTER_TIME;	/* 按键滤波时间 */
	s_BtnRight.LongTime = BUTTON_LONG_TIME;						/* 长按时间 */
	s_BtnRight.Count = s_BtnRight.FilterTime / 2;		/* 计数器设置为滤波时间的一半 */
	s_BtnRight.State = 0;							/* 按键缺省状态，0为未按下 */
	s_BtnRight.KeyCodeDown = KEY_DOWN_RIGHT;		/* 按键按下的键值代码 */
	s_BtnRight.KeyCodeUp = 0;						/* 按键弹起的键值代码，0表示不检测 */
	s_BtnRight.KeyCodeLong = KEY_HOLD_RIGHT;					/* 按键被持续按下的键值代码，0表示不检测 */
	s_BtnRight.RepeatSpeed = BUTTON_FILTER_TIME;                                                /* 按键连发的速度，0表示不支持连发 */
 	s_BtnRight.RepeatCount = 0;                                                /* 连发计数器 */                
	
		/* 初始化reset按键变量 */
	s_BtnSelect.IsKeyDownFunc = IsSelectKeyDown;		/* 判断按键按下的函数 */
	s_BtnSelect.FilterTime = BUTTON_FILTER_TIME;	/* 按键滤波时间 */
	s_BtnSelect.LongTime = BUTTON_LONG_TIME;						/* 长按时间 */
	s_BtnSelect.Count = s_BtnSelect.FilterTime / 2;		/* 计数器设置为滤波时间的一半 */
	s_BtnSelect.State = 0;							/* 按键缺省状态，0为未按下 */
	s_BtnSelect.KeyCodeDown = KEY_DOWN_SELECT;		/* 按键按下的键值代码 */
	s_BtnSelect.KeyCodeUp = 0;						/* 按键弹起的键值代码，0表示不检测 */
	s_BtnSelect.KeyCodeLong = KEY_HOLD_SELECT;					/* 按键被持续按下的键值代码，0表示不检测 */
	s_BtnSelect.RepeatSpeed = BUTTON_FILTER_TIME;                                                /* 按键连发的速度，0表示不支持连发 */
 	s_BtnSelect.RepeatCount = 0;                                                /* 连发计数器 */                


}

/*******************************************************************************
	函数名: PutKey
	输  入: 键值
	输  出:
	功能说明：发送对应按键的事件
*/
static void PutKey(uint8_t _KeyCode)
{
	switch(_KeyCode) {
		case KEY_DOWN_UP:
			rt_event_send(&keyEvent, 1);
			break;
		case KEY_DOWN_DOWN:
			rt_event_send(&keyEvent, (1<<1));
			break;
		case KEY_DOWN_LEFT:
			rt_event_send(&keyEvent, (1<<2));
			break;
		case KEY_DOWN_RIGHT:
			rt_event_send(&keyEvent, (1<<3));
			break;
		case KEY_DOWN_SELECT:
			rt_event_send(&keyEvent, (1<<4));
			break;
		default :
			break;
	}
}


/*******************************************************************************
	函数名：DetectButton
	输  入: 按键结构变量指针
	输  出:
	功能说明：检测指定的按键
*/
static void DetectButton(BUTTON_T *_pBtn)
{
	/* 如果没有初始化按键函数，则报错*/
	if (_pBtn->IsKeyDownFunc == 0)
	{
		rt_kprintf("Fault : DetectButton(), _pBtn->IsKeyDownFunc undefine");
		return;
		//printf("Fault : DetectButton(), _pBtn->IsKeyDownFunc undefine");
	}
	

	if (_pBtn->IsKeyDownFunc())	  //IsKeyDownFunc()函数返回1时
	{
		if (_pBtn->Count < _pBtn->FilterTime)	//滤波器计数器 <  滤波时间(最大255,表示2550ms)
		{
			_pBtn->Count = _pBtn->FilterTime;
		}
		else if(_pBtn->Count < 2 * _pBtn->FilterTime)	//???
		{
			_pBtn->Count++;
		}
		else
		{
			if (_pBtn->State == 0)
			{
				_pBtn->State = 1;

				/* 发送按钮按下的消息 */
				if (_pBtn->KeyCodeDown > 0)		//判断是否检测这个按键按下（因为==0时表示不检测）
				{
					/* 键值放入按键FIFO */
					PutKey(_pBtn->KeyCodeDown);
				}
			}

			if (_pBtn->LongTime > 0)	  // 如果需要判断长按键时
			{
				if (_pBtn->LongCount < _pBtn->LongTime)
				{
					/* 发送按钮持续按下的消息 */
					if (++_pBtn->LongCount == _pBtn->LongTime)
					{
						/* 键值放入按键FIFO */
						PutKey(_pBtn->KeyCodeLong);
					}
				}
				else 
				{ 
					if (_pBtn->RepeatSpeed > 0) 
					{ if (++_pBtn->RepeatCount >= _pBtn->RepeatSpeed) 
						{ 
							_pBtn->RepeatCount = 0; /* 常按键后，每隔10ms发送1个按键 */ 
							PutKey(_pBtn->KeyCodeDown); 
						} 
					}
				}
			}
		}
	}
	else
	{
		if(_pBtn->Count > _pBtn->FilterTime)
		{
			_pBtn->Count = _pBtn->FilterTime;
		}
		else if(_pBtn->Count != 0)
		{
			_pBtn->Count--;
		}
		else
		{
			if (_pBtn->State == 1)
			{
				_pBtn->State = 0;

				/* 发送按钮弹起的消息 */
				if (_pBtn->KeyCodeUp > 0)
				{
					/* 键值放入按键FIFO */
					PutKey(_pBtn->KeyCodeUp);
				}
			}
		}

		_pBtn->LongCount = 0;
	}
}

/*******************************************************************************
	函数名：KeyPro
	输  入: 
	输  出:
	功能说明：检测所有的按键，这个函数要被systic的中断服务程序调用
*/
static void KeysDetect(void)
{
	DetectButton(&s_BtnUp);	/* up 键 */
	DetectButton(&s_BtnDown);	/* down 键 */
	DetectButton(&s_BtnLeft);	/* left 键 */
	DetectButton(&s_BtnRight);	/* right键 */
	DetectButton(&s_BtnSelect);	/* Select键 */
}

static void keyDetect_thread_entry(void *parameter)
{
   
    GPIO_Configuration();
		InitButtonVar();
    while (1)
    {
       	KeysDetect();      	
        rt_thread_delay(1);
    }
}

static void keyProcess_thread_entry(void *parameter)
{
    rt_uint32_t e;
    while (1)
    {
        /* receive second event */
        if (rt_event_recv(&keyEvent, 0x1f,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_FOREVER, &e) == RT_EOK)
        {
					rt_kprintf("some key is pressed, event :0x%x\n", e);
        }
    }
}

void rt_hw_key_init(void)
{
    rt_thread_t key_tid;
		rt_thread_t key_tid2;
	
		    /* 初始化事件对象 */
    rt_event_init(&keyEvent, "keyEvent", RT_IPC_FLAG_FIFO);
	
    key_tid = rt_thread_create("keyDetect",
                               keyDetect_thread_entry, RT_NULL,
                               512, USER_NORMAL_THREAD_PRIORITY - 1, 5);
    if (key_tid != RT_NULL) rt_thread_startup(key_tid);
    
    key_tid2 = rt_thread_create("keyProcess",
                               keyProcess_thread_entry, RT_NULL,
                               512, USER_NORMAL_THREAD_PRIORITY - 1, 20);
    if (key_tid2 != RT_NULL) rt_thread_startup(key_tid2);
}
