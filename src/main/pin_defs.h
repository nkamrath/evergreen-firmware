#ifndef PIN_DEFS__H
#define PIN_DEFS__H


#define MOTION_SENSOR_PIN			4
#define MOTION_SENSOR_PIN_MASK		(1ULL<<MOTION_SENSOR_PIN)
#define MOTION_SENSOR_PIN_CONFIG		{					\
	.intr_type = GPIO_INTR_DISABLE,					\
	.mode = GPIO_MODE_INPUT,							\
	.pin_bit_mask = (1ULL << MOTION_SENSOR_PIN),		\
	.pull_down_en = 0,									\
	.pull_up_en = 0										\
}

#define LIGHT1_RELAY_PIN			15ULL
#define LIGHT1_RELAY_PIN_MASK		(1ULL<<LIGHT1_RELAY_PIN)
#define LIGHT1_RELAY_PIN_CONFIG		{					\
	.intr_type = GPIO_INTR_DISABLE,					\
	.mode = GPIO_MODE_OUTPUT,							\
	.pin_bit_mask = (1ULL << LIGHT1_RELAY_PIN),		\
	.pull_down_en = 0,									\
	.pull_up_en = 0										\
}

//debug output leds pins


#define DEBUG_PIN1					4ULL
#define DEBUG_PIN1_MASK				(1ULL<<DEBUG_PIN1)
#define DEBUG_PIN1_CONFIG		{					\
	.intr_type = GPIO_INTR_DISABLE,					\
	.mode = GPIO_MODE_OUTPUT,							\
	.pin_bit_mask = (1ULL << DEBUG_PIN1),		\
	.pull_down_en = 0,									\
	.pull_up_en = 0										\
}

#define DEBUG_PIN2					0ULL
#define DEBUG_PIN2_MASK				(1ULL<<DEBUG_PIN2)
#define DEBUG_PIN2_CONFIG		{					\
	.intr_type = GPIO_INTR_DISABLE,					\
	.mode = GPIO_MODE_OUTPUT,							\
	.pin_bit_mask = (1ULL << DEBUG_PIN2),		\
	.pull_down_en = 0,									\
	.pull_up_en = 0										\
}

#define DEBUG_PIN3					2ULL
#define DEBUG_PIN3_MASK				(1ULL<<DEBUG_PIN3)
#define DEBUG_PIN3_CONFIG		{					\
	.intr_type = GPIO_INTR_DISABLE,					\
	.mode = GPIO_MODE_OUTPUT,							\
	.pin_bit_mask = (1ULL << DEBUG_PIN3),		\
	.pull_down_en = 0,									\
	.pull_up_en = 0										\
}

#endif