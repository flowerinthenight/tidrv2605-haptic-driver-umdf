#pragma once

//
// Register interfaces
//
#define DRV2605_REG_STATUS								0x00
#define DRV2605_REG_MODE								0x01
#define DRV2605_REG_REALTIME_PLAYBACK_IN				0x02
#define DRV2605_REG_LIBRARY_SELECTION					0x03
#define DRV2605_REG_WAVEFORM_SEQUENCER_00				0x04 // 0x04 --> 0x0B
#define DRV2605_REG_WAVEFORM_SEQUENCER_01				0x05 // 0x04 --> 0x0B
#define DRV2605_REG_GO									0x0C
#define DRV2605_REG_OVERDRIVE_TIME_OFFSET				0x0D
#define DRV2605_REG_SUSTAIN_TIME_OFFSET_POSITIVE		0x0E
#define DRV2605_REG_SUSTAIN_TIME_OFFSET_NEGATIVE		0x0F
#define DRV2605_REG_BRAKETIME_OFFSET					0x10
#define DRV2605_REG_AUDIO_TO_HAPTICS_CTRL				0x11
#define DRV2605_REG_AUDIO_TO_HAPTICS_MIN_IN_LVL			0x12
#define DRV2605_REG_AUDIO_TO_HAPTICS_MAX_IN_LVL			0x13
#define DRV2605_REG_AUDIO_TO_HAPTICS_MIN_OUT_DRIVE		0x14
#define DRV2605_REG_AUDIO_TO_HAPTICS_MAX_OUT_DRIVE		0x15
#define DRV2605_REG_RATED_VOLTAGE						0x16
#define DRV2605_REG_OVERDRIVE_CLAMP_VOLTAGE				0x17
#define DRV2605_REG_AUTO_CALIB_COMPENSATION_RESULT		0x18
#define DRV2605_REG_AUTO_CALIB_BACK_EMF_RESULT			0x19
#define DRV2605_REG_FEEDBACK_CTRL						0x1A
#define DRV2605_REG_CTRL_1								0x1B
#define DRV2605_REG_CTRL_2								0x1C
#define DRV2605_REG_CTRL_3								0x1D
#define DRV2605_REG_AUTO_CALIB_MEM_IFACE				0x1E
#define DRV2605_REG_VBAT_VOLTAGE_MONITOR				0x21
#define DRV2605_REG_LRA_RESONANCE_PERIOD				0x22

#define DRV2605_CMD_GO									0x01

//
// Waveform library effects
//
typedef enum WaveformLibraryEffect
{
	WAVEFORM_NULL = 0,
	WAVEFORM_STRONG_CLICK_100,
	WAVEFORM_STRONG_CLICK_60,
	WAVEFORM_STRONG_CLICK_30,
	WAVEFORM_SHARP_CLICK_100,
	WAVEFORM_SHARP_CLICK_60,
	WAVEFORM_SHARP_CLICK_30,
	WAVEFORM_SOFT_BUMP_100,
	WAVEFORM_SOFT_BUMP_60,
	WAVEFORM_SOFT_BUMP_30,
	WAVEFORM_DOUBLECLICK_100,
	WAVEFORM_DOUBLECLICK_60,
	WAVEFORM_TRIPLECLICK_100,
	WAVEFORM_SOFT_FUZZ_60,
	WAVEFORM_STRONG_BUZZ_100,
	WAVEFORM_750MS_ALERT_100,
	WAVEFORM_1000MS_ALERT_100,
	WAVEFORM_STRONG_CLICK_1_100,
	WAVEFORM_STRONG_CLICK_2_80,
	WAVEFORM_STRONG_CLICK_3_60,
	WAVEFORM_STRONG_CLICK_4_30,
	WAVEFORM_MEDIUM_CLICK_1_100,
	WAVEFORM_MEDIUM_CLICK_2_80,
	WAVEFORM_MEDIUM_CLICK_3_60,
	WAVEFORM_SHARP_TICK_1_100,
	WAVEFORM_SHARP_TICK_2_80,
	WAVEFORM_SHARP_TICK_3_60,
	WAVEFORM_SHORT_DOUBLECLICK_STRONG_1_100,
	WAVEFORM_SHORT_DOUBLECLICK_STRONG_2_80,
	WAVEFORM_SHORT_DOUBLECLICK_STRONG_3_60,
	WAVEFORM_SHORT_DOUBLECLICK_STRONG_4_30,
	WAVEFORM_SHORT_DOUBLECLICK_MEDIUM_1_100,
	WAVEFORM_SHORT_DOUBLECLICK_MEDIUM_2_80,
	WAVEFORM_SHORT_DOUBLECLICK_MEDIUM_3_60,
	WAVEFORM_SHORT_DOUBLE_SHARP_TICK_1_100,
	WAVEFORM_SHORT_DOUBLE_SHARP_TICK_2_80,
	WAVEFORM_SHORT_DOUBLE_SHARP_TICK_3_60,
	WAVEFORM_LONG_DOUBLE_SHARP_CLICK_STRONG_1_100,
	WAVEFORM_LONG_DOUBLE_SHARP_CLICK_STRONG_2_80,
	WAVEFORM_LONG_DOUBLE_SHARP_CLICK_STRONG_3_60,
	WAVEFORM_LONG_DOUBLE_SHARP_CLICK_STRONG_4_30,
	WAVEFORM_LONG_DOUBLE_SHARP_CLICK_MEDIUM_1_100,
	WAVEFORM_LONG_DOUBLE_SHARP_CLICK_MEDIUM_2_80,
	WAVEFORM_LONG_DOUBLE_SHARP_CLICK_MEDIUM_3_60,
	WAVEFORM_LONG_DOUBLE_SHARP_TICK_1_100,
	WAVEFORM_LONG_DOUBLE_SHARP_TICK_2_80,
	WAVEFORM_LONG_DOUBLE_SHARP_TICK_3_60,
	WAVEFORM_BUZZ_1_100,
	WAVEFORM_BUZZ_2_80,
	WAVEFORM_BUZZ_3_60,
	WAVEFORM_BUZZ_4_40,
	WAVEFORM_BUZZ_5_20,
	WAVEFORM_PULSING_STRONG_1_100,
	WAVEFORM_PULSING_STRONG_2_60,
	WAVEFORM_PULSING_MEDIUM_1_100,
	WAVEFORM_PULSING_MEDIUM_2_60,
	WAVEFORM_PULSING_SHARP_1_100,
	WAVEFORM_PULSING_SHARP_2_60,
	WAVEFORM_TRANSITION_CLICK_1_100,
	WAVEFORM_TRANSITION_CLICK_2_80,
	WAVEFORM_TRANSITION_CLICK_3_60,
	WAVEFORM_TRANSITION_CLICK_4_40,
	WAVEFORM_TRANSITION_CLICK_5_20,
	WAVEFORM_TRANSITION_CLICK_6_10,
	WAVEFORM_TRANSITION_HUM_1_100,
	WAVEFORM_TRANSITION_HUM_2_80,
	WAVEFORM_TRANSITION_HUM_3_60,
	WAVEFORM_TRANSITION_HUM_4_40,
	WAVEFORM_TRANSITION_HUM_5_20,
	WAVEFORM_TRANSITION_HUM_6_10,
	WAVEFORM_TRANSITION_RAMP_DOWN_LONG_SMOOTH_1_100_TO_0,
	WAVEFORM_TRANSITION_RAMP_DOWN_LONG_SMOOTH_2_100_TO_0,
	WAVEFORM_TRANSITION_RAMP_DOWN_MEDIUM_SMOOTH_1_100_TO_0,
	WAVEFORM_TRANSITION_RAMP_DOWN_MEDIUM_SMOOTH_2_100_TO_0,
	WAVEFORM_TRANSITION_RAMP_DOWN_SHORT_SMOOTH_1_100_TO_0,
	WAVEFORM_TRANSITION_RAMP_DOWN_SHORT_SMOOTH_2_100_TO_0,
	WAVEFORM_TRANSITION_RAMP_DOWN_LONG_SHARP_1_100_TO_0,
	WAVEFORM_TRANSITION_RAMP_DOWN_LONG_SHARP_2_100_TO_0,
	WAVEFORM_TRANSITION_RAMP_DOWN_MEDIUM_SHARP_1_100_TO_0,
	WAVEFORM_TRANSITION_RAMP_DOWN_MEDIUM_SHARP_2_100_TO_0,
	WAVEFORM_TRANSITION_RAMP_DOWN_SHORT_SHARP_1_100_TO_0,
	WAVEFORM_TRANSITION_RAMP_DOWN_SHORT_SHARP_2_100_TO_0,
	WAVEFORM_TRANSITION_RAMP_UP_LONG_SMOOTH_1_0_TO_100,
	WAVEFORM_TRANSITION_RAMP_UP_LONG_SMOOTH_2_0_TO_100,
	WAVEFORM_TRANSITION_RAMP_UP_MEDIUM_SMOOTH_1_0_TO_100,
	WAVEFORM_TRANSITION_RAMP_UP_MEDIUM_SMOOTH_2_0_TO_100,
	WAVEFORM_TRANSITION_RAMP_UP_SHORT_SMOOTH_1_0_TO_100,
	WAVEFORM_TRANSITION_RAMP_UP_SHORT_SMOOTH_2_0_TO_100,
	WAVEFORM_TRANSITION_RAMP_UP_LONG_SHARP_1_0_TO_100,
	WAVEFORM_TRANSITION_RAMP_UP_LONG_SHARP_2_0_TO_100,
	WAVEFORM_TRANSITION_RAMP_UP_MEDIUM_SHARP_1_0_TO_100,
	WAVEFORM_TRANSITION_RAMP_UP_MEDIUM_SHARP_2_0_TO_100,
	WAVEFORM_TRANSITION_RAMP_UP_SHORT_SHARP_1_0_TO_100,
	WAVEFORM_TRANSITION_RAMP_UP_SHORT_SHARP_2_0_TO_100,
	WAVEFORM_TRANSITION_RAMP_DOWN_LONG_SMOOTH_1_50_TO_0,
	WAVEFORM_TRANSITION_RAMP_DOWN_LONG_SMOOTH_2_50_TO_0,
	WAVEFORM_TRANSITION_RAMP_DOWN_MEDIUM_SMOOTH_1_50_TO_0,
	WAVEFORM_TRANSITION_RAMP_DOWN_MEDIUM_SMOOTH_2_50_TO_0,
	WAVEFORM_TRANSITION_RAMP_DOWN_SHORT_SMOOTH_1_50_TO_0,
	WAVEFORM_TRANSITION_RAMP_DOWN_SHORT_SMOOTH_2_50_TO_0,
	WAVEFORM_TRANSITION_RAMP_DOWN_LONG_SHARP_1_50_TO_0,
	WAVEFORM_TRANSITION_RAMP_DOWN_LONG_SHARP_2_50_TO_0,
	WAVEFORM_TRANSITION_RAMP_DOWN_MEDIUM_SHARP_1_50_TO_0,
	WAVEFORM_TRANSITION_RAMP_DOWN_MEDIUM_SHARP_2_50_TO_0,
	WAVEFORM_TRANSITION_RAMP_DOWN_SHORT_SHARP_1_50_TO_0,
	WAVEFORM_TRANSITION_RAMP_DOWN_SHORT_SHARP_2_50_TO_0,
	WAVEFORM_TRANSITION_RAMP_UP_LONG_SMOOTH_1_0_TO_50,
	WAVEFORM_TRANSITION_RAMP_UP_LONG_SMOOTH_2_0_TO_50,
	WAVEFORM_TRANSITION_RAMP_UP_MEDIUM_SMOOTH_1_0_TO_50,
	WAVEFORM_TRANSITION_RAMP_UP_MEDIUM_SMOOTH_2_0_TO_50,
	WAVEFORM_TRANSITION_RAMP_UP_SHORT_SMOOTH_1_0_TO_50,
	WAVEFORM_TRANSITION_RAMP_UP_SHORT_SMOOTH_2_0_TO_50,
	WAVEFORM_TRANSITION_RAMP_UP_LONG_SHARP_1_0_TO_50,
	WAVEFORM_TRANSITION_RAMP_UP_LONG_SHARP_2_0_TO_50,
	WAVEFORM_TRANSITION_RAMP_UP_MEDIUM_SHARP_1_0_TO_50,
	WAVEFORM_TRANSITION_RAMP_UP_MEDIUM_SHARP_2_0_TO_50,
	WAVEFORM_TRANSITION_RAMP_UP_SHORT_SHARP_1_0_TO_50,
	WAVEFORM_TRANSITION_RAMP_UP_SHORT_SHARP_2_0_TO_50,
	WAVEFORM_LONG_BUZZ_FOR_PROGRAMMATIC_STOPPING_100,
	WAVEFORM_SMOOTH_HUM_1_NO_KICK_OR_BRAKE_PULSE_50,
	WAVEFORM_SMOOTH_HUM_2_NO_KICK_OR_BRAKE_PULSE_40,
	WAVEFORM_SMOOTH_HUM_3_NO_KICK_OR_BRAKE_PULSE_30,
	WAVEFORM_SMOOTH_HUM_4_NO_KICK_OR_BRAKE_PULSE_20,
	WAVEFORM_SMOOTH_HUM_5_NO_KICK_OR_BRAKE_PULSE_10,
};

const unsigned short DRV2605_BUS_ADDRESS[] = L"5A";