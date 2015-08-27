#ifndef __KEY_H
#define __KEY_H	 

#include <rtthread.h>
#include <board.h>

/* 按键滤波时间50ms, 单位10ms
 只有连续检测到50ms状态不变才认为有效，包括弹起和按下两种事件
*/
#define BUTTON_FILTER_TIME 	5
#define BUTTON_LONG_TIME 	100		/* 持续1秒，认为长按事件 */

/*
	每个按键对应1个全局的结构体变量。
	其成员变量是实现滤波和多种按键状态所必须的
*/
typedef struct
{
	/* 下面是一个函数指针，指向判断按键手否按下的函数 */
	uint8_t (*IsKeyDownFunc)(void); /* 按键按下的判断函数,1表示按下 */

	uint8_t Count;			/* 滤波器计数器 */
	uint8_t FilterTime;		/* 滤波时间(最大255,表示2550ms) */
	uint16_t LongCount;		/* 长按计数器 */
	uint16_t LongTime;		/* 按键按下持续时间, 0表示不检测长按 */
	uint8_t  State;			/* 按键当前状态（按下还是弹起） */
	uint8_t KeyCodeUp;		/* 按键弹起的键值代码, 0表示不检测按键弹起 */
	uint8_t KeyCodeDown;	/* 按键按下的键值代码, 0表示不检测按键按下 */
	uint8_t KeyCodeLong;	/* 按键长按的键值代码, 0表示不检测长按 */
	uint8_t RepeatSpeed;        /* 连续按键周期 */
  uint8_t RepeatCount;        /* 连续按键计数器 */
}BUTTON_T;

/* 定义键值代码
	推荐使用enum, 不用#define，原因：
	(1) 便于新增键值,方便调整顺序，使代码看起来舒服点
	(2)	编译器可帮我们避免键值重复。
*/
typedef enum
{
	KEY_NONE = 0,			/* 0 表示按键事件 */

	/* 为了演示，需要检测ADD键弹起事件和长按事件 */
	KEY_DOWN_UP,			/* ADD键按下 */
	KEY_HOLD_UP,			/* ADD键弹起 */
	
	KEY_DOWN_DOWN,		/* DEC键按下 */
	KEY_HOLD_DOWN,
	
	KEY_DOWN_LEFT,		/* AUTO键按下 */
	KEY_HOLD_LEFT,			/* ADD键按下 */
	
	KEY_DOWN_RIGHT,		/* 摇杆RESET键按下 */
	KEY_HOLD_RIGHT,			/* ADD键按下 */
	
	KEY_DOWN_SELECT,		/* 摇杆RESET键按下 */
	KEY_HOLD_SELECT,			/* ADD键按下 */
	
}KEY_ENUM;

//void rt_hw_key_init(void);
#endif
