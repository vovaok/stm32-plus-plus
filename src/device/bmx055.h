#ifndef _BMX055_H
#define _BMX055_H

#include "i2c.h"

//	����� � ID ����:						//
#define	BMA_ADDRESS				(0x19<<1)   //	����� ������� �� ���� I2C
#define	BMA_ID					0xFA		//	ID �������
//	�������� ����:							//											|	���7	|	���6	|	���5	|	���4	|	���3	|	���2	|	���1	|	���0	|��� ���|	�����������
#define	REG_BMA_CHIPID			0x00		//	ID ����									|											ID<7:0>												| 0xFA	|	ID<7:0>		- �������� �������� ������ ���� ����� 0xFA (��������� BMA_ID)
#define REG_BMA_X_LSB			0x02		//	������ �������� ��������� �� ��� X		|					x_lsb<3:0>					| -	- -	- -	- -	- -	- -	- -	- -	- -	|new_data_x	| 0x00	|	x_lsb<3 :0> - ������� ���� ������ ��� X, new_data_x - ���� ����������� �� �� ��� ������ ���� ��������� ����� ���������� ������
#define REG_BMA_X_MSB			0x03		//	������ �������� ��������� �� ��� X		|					x_msb<11:4>																	| 0x00	|	x_msb<11:4> - ������� ���� ������ ��� X
#define REG_BMA_Y_LSB			0x04		//	������ �������� ��������� �� ��� Y		|					y_lsb<3:0>					| -	- -	- -	- -	- -	- -	- -	- -	- -	|new_data_y	| 0x00	|	y_lsb<3 :0> - ������� ���� ������ ��� Y, new_data_y - ���� ����������� �� �� ��� ������ ���� ��������� ����� ���������� ������
#define REG_BMA_Y_MSB			0x05		//	������ �������� ��������� �� ��� Y		|					y_msb<11:4>																	| 0x00	|	y_msb<11:4> - ������� ���� ������ ��� Y
#define REG_BMA_Z_LSB			0x06		//	������ �������� ��������� �� ��� Z		|					z_lsb<3:0>					| -	- -	- -	- -	- -	- -	- -	- -	- -	|new_data_z	| 0x00	|	z_lsb<3 :0> - ������� ���� ������ ��� Z, new_data_z - ���� ����������� �� �� ��� ������ ���� ��������� ����� ���������� ������
#define REG_BMA_Z_MSB			0x07		//	������ �������� ��������� �� ��� Z		|					z_msb<11:4>																	| 0x00	|	z_msb<11:4> - ������� ���� ������ ��� Z
#define REG_BMA_TEMP			0x08		//	������ ����������� ����	� �C			|											temp<7:0>											| 0x00	|	temp<7:0>	- ����� � ������� ���������� �� ���� � ����� 0.5�� � ������� �� 23�� ����. ��� ��������� �������� ���������� ����� ������ ����� �������� �������� �� ��� (0.5��) � ������� � 23�C. �������� 0x00 ������������� ����������� 23�C, �������� 0x40 (64) ������������� ����������� 55�C, �������� 0xC0 (-64) ������������� ����������� -9�C.
#define REG_BMA_INT_ST_0		0x09		//	����� ��������� ����������				|	flat	|  orient	|   s_tap	|   d_tap	|slo_not_mot|	slope	|	high	|	low		| 0x00	|	flat - ������ ���������� � ����������� ����� �� ����, orient - ����� ���������� ��������� (x/y/z), s_tap - ��������� �������, d_tap - ������� �������, slo_not_mot - ��������� �������� ��� �������������, slope - ������, high - ������� ���������, low - ������ ���������
#define REG_BMA_INT_ST_1		0x0A		//	����� ��������� ����������				|	data	|  fifo_wm	| fifo_full	| -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	| 0x00	|	data - ������ ����� ������, fifo_wm - ����� �������� �� ������ watermark, fifo_full - ����� ��������� ��������
#define REG_BMA_INT_ST_2		0x0B		//	����� ��������� ����������				|  tap_sign	|tap_first_z|tap_first_y|tap_first_x|slope_sign	|slopeFirstZ|slopeFirstY|slopeFirstX| 0x00	|	tap_sign - ���� (�����������) ����������/�������� �������, tap_first_z - ���������/������� ������� �� ��� Z, tap_first_y - ���������/������� ������� �� ��� Y, tap_first_x - ���������/������� ������� �� ��� X, slope_sign - ���� (�����������) �������, slopeFirstZ - ������ �� ��� Z, slopeFirstY - ������ �� ��� Y, slopeFirstX - ������ �� ��� X
#define REG_BMA_INT_ST_3		0x0C		//	����� ��������� ����������				|	flat	|			 orient<2:0>			| high_sign	| highFirstZ| highFirstY| highFirstX| 0x00	|	flat - ������ ���������� � ����������� ����� �� ����, orient[2] - ���������� ��������� �� ��� Z (�0� �����, �1� ����), orient<1:0> - ���������� ��������� �� ���� XY (�00� ����������� ������; �01� ����������� �����; �10� ������������� �����; �11� ������������� ������), high_sign ���� (�����������) �������� ���������, highFirstZ - ������� ��������� �� ��� Z, highFirstY - ������� ��������� �� ��� Y, highFirstX - ������� ��������� �� ��� X
#define REG_BMA_FIFO_ST			0x0E		//	������ ������ FIFO						|  overrun	|						fifo_frame_counter<6:0>										| 0x00	|	overrun - ��������� ������������ ������ FIFO, fifo_frame_counter<6:0> - ������ ������ FIFO (���������� ������ �������� � ������)
#define REG_BMA_RANGE			0x0F		//	�������� ����������� �������� ���������	| -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	|					range<3:0>					| 0xFF	|	range<3:0> - �������� ����������� �������� ���������: �0011� �2g; �0101� �4g; �1000� �8g; �1100� �16g
#define REG_BMA_BW				0x10		//	������ ����������� (Bandwidths)			| -	- -	- -	- -	- -	- -	- -	- -	- -	|							bw<4:0>							| 0x0F	|	bw<4:0> - ������ ����������� ������������� ������: �00xxx� 7.81 ��; �01000� 7.81 ��; �01001� 15.63 ��; �01010� 31.25 ��; �01011� 62.5 ��; �01100� 125 ��; �01101� 250 ��; �01110� 500 ��; �01111� 1000 ��; �1xxxx� 1000 ��
#define REG_BMA_LPW				0x11		//	����� �������� ������� �������			|  suspend	|lowpower_en|deep_suspend|				sleep_dur<3:0>					| -	- -	- -	| 0x00	|	suspend - ������ �����, lowpower_en - ����� ����������� ����������������, deep_suspend - ����� �������� ������������. ����� ���������� ������ ���� �� ������ suspend, lowpower_en � deep_suspend, � ����� ���� ��� ������ � 0 ��������� ���������� �����. sleep_dur<3:0> - ����������������� ���� �������� � ������ LOW_POWER (����������� �����������������): �0000�...�0101� 0,5��; �0110� 1��; �0111� 2��; �1000� 4��; �1001� 6��; �1010� 10��; �1011� 25��; �1100� 50��; �1101� 100��; �1110� 500��; �1111� 1�.
#define REG_BMA_LOW_POWER		0x12		//	��������. ������� �������������.		| -	- -	- -	| lowpower	| sleeptimer| -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	| 0x00	|	
#define REG_BMA_D_HBW			0x13		//	����� ���� ������ � �� ���������		|dataHigh_bw| shadow_dis| -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	| 0x00	|	dataHigh_bw: �1� ��������� ��������������� ������; �0� ��������� ������������� ������, shadow_dis: �1� ��������� / �0� �������� �������� ��������� ��������� ������ ������ �������� ���������. (��������� ������������ ����������� ������, �������� ������ ����� ������ � ������� ���� (MSB) ���� �������� ������ ������� ����� (LSB), �� ��� ��� ���� �� ����� ��������� ������� ���� (MSB) ������ �������� ���������).
#define REG_BMA_RESET			0x14		//	����������� ������������				|											softreset<7:0>										| 0x00	|	������ �������� 0xB6 ������� � ������������ � ������ ���������.
#define REG_BMA_INT_EN_0		0x16		//	����� ���������� ����������				|  flat_en	| orient_en	| s_tap_en	| d_tap_en	| -	- -	- -	| slope_en_z| slope_en_y| slope_en_x| 0x00	|	flat_en - ��������� ���������� ������������� ��������� ������� � ����������� ����� �� ����, orient_en - ��������� ���������� �� ����� ���������� ���������, s_tap_en - ��������� ���������� �� ���������� �������, d_tap_en - ��������� ���������� �� �������� �������, slope_en_z - ��������� ���������� ����������� ������ �� ��� z, slope_en_y - ��������� ���������� ����������� ������ �� ��� y, slope_en_x - ��������� ���������� ����������� ������ �� ��� x
#define REG_BMA_INT_EN_1		0x17		//	����� ���������� ����������				| -	- -	- -	|  f_wm_en	| f_full_en	|  data_en	|  low_en	| high_en_z	| high_en_y	| high_en_x	| 0x00	|	f_wm_en - ��������� ���������� �� ���������� ������ fifo �� ������ (watermark), f_full_en - ��������� ���������� �� ������� ���������� ������ fifo, data_en - ��������� ���������� �� ����������� ����� ������, low_en - ��������� ���������� ����������� ������ ���������, high_en_z - ��������� ���������� ����������� ������� ��������� �� ��� z, high_en_y - ��������� ���������� ����������� ������� ��������� �� ��� y, high_en_x - ��������� ���������� ����������� ������� ��������� �� ��� x
#define REG_BMA_INT_EN_2		0x18		//	����� ���������� ����������				| -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	|sloNoMotSel|sloNoMotEnZ|sloNoMotEnY|sloNoMotEnX| 0x00	|	sloNoMotSel - ����� ������ ������ ���������� ������������ ��������� �������� ��� ������������� (�0� ��������� ��������; �1� �������������), sloNoMotEnZ - ��������� ���������� ����������� ��������� �������� ��� ������������� �� ��� z, sloNoMotEnY - ��������� ���������� ����������� ��������� �������� ��� ������������� �� ��� y, sloNoMotEnX - ��������� ���������� ����������� ��������� �������� ��� ������������� �� ��� x
#define REG_BMA_INT_MAP_0		0x19		//	����� ������������ ���������� �� INT1	| int1_flat	|int1_orient| int1_s_tap| int1_d_tap|int1SloNoMot|int1_slope| int1_high	| int1_low	| 0x00	|	int1_flat - ��������� ���������� �� ��������� ������� � ����������� ����� �� ���� �� ����� INT1, int1_orient - ��������� ���������� �� ����� ���������� ��������� �� ����� INT1, int1_s_tap - ��������� ���������� �� ���������� ������� �� ����� INT1, int1_d_tap - ��������� ���������� �� �������� ������� �� ����� INT1, int1SloNoMot - ��������� ���������� �� ���������� �������� ��� ������������� �� ����� INT1, int1_slope - ��������� ���������� �� ������� �� ����� INT1, int1_high - ��������� ���������� �� �������� ��������� �� ����� INT1, int1_low - ��������� ���������� �� ������� ��������� �� ����� INT1
#define REG_BMA_INT_MAP_1		0x1A		//	����� �������-� ���������� �� INT1,INT2	| int2_data	|int2_f_wm	|int2_f_full| -	- -	- -	| -	- -	- -	|int1_f_full| int1_f_wm	| int1_data	| 0x00	|	int2_data - ��������� ���������� �� ���������� ����� ������ �� ����� INT2, int2_f_wm - ��������� ���������� �� ���������� ������ fifo �� ������ (watermark) �� ����� INT2, int2_f_full - ��������� ���������� �� ������������ ������ fifo �� ����� INT2, int1_f_full - ��������� ���������� �� ������������ ������ fifo �� ����� INT1, int1_f_wm - ��������� ���������� �� ���������� ������ fifo �� ������ (watermark) �� ����� INT1, int1_data - ��������� ���������� �� ���������� ����� ������ �� ����� INT1
#define REG_BMA_INT_MAP_2		0x1B		//	����� ������������ ���������� �� INT2	| int2_flat	|int2_orient| int2_s_tap| int2_d_tap|int2SloNoMot|int2_slope| int2_high	| int2_low	| 0x00	|	int2_flat - ��������� ���������� �� ��������� ������� � ����������� ����� �� ���� �� ����� INT2, int2_orient - ��������� ���������� �� ����� ���������� ��������� �� ����� INT2, int2_s_tap - ��������� ���������� �� ���������� ������� �� ����� INT2, int2_d_tap - ��������� ���������� �� �������� ������� �� ����� INT2, int2SloNoMot - ��������� ���������� �� ���������� �������� ��� ������������� �� ����� INT2, int2_slope - ��������� ���������� �� ������� �� ����� INT2, int2_high - ��������� ���������� �� �������� ��������� �� ����� INT2, int2_low - ��������� ���������� �� ������� ��������� �� ����� INT2
#define REG_BMA_INT_SRC			0x1E		//	��� ������ ������������ ��� ����������	| -	- -	- -	- -	- -	- -	| src_data	| src_tap	|srcSloNoMot| src_slope	| src_high	|  src_low	| 0x00	|	���������� ������ ��� ��������������� ���������� �����: �0� ������������� ������; �1� ��������������� ������
#define REG_BMA_INT_OUT_CTRL	0x20		//	����� ��������� ������� INT1 � INT2		| -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	|  int2_od	|  int2_lvl	|  int1_od	| int1_lvl	| 0x05	|	int2_od - ����� ���� ����� ��� ������ INT2 (�0� ��������������� (push-pull); �1� �������� ����), int2_lvl - �������� ������� �� ������ INT2, int1_od - ����� ���� ����� ��� ������ INT1 (�0� ��������������� (push-pull); �1� �������� ����), int1_lvl - �������� ������� �� ������ INT1
#define REG_BMA_INT_RST_LATCH	0x21		//	����� ������ � ����� ����������			| reset_int	| -	- -	- -	- -	- -	- -	- -	- -	- -	|					latch_int<3:0>				| 0x00	|	reset_int - ��������� � �1� ������� � ������ �������� ����������, latch_int<3:0> - ���������� ����� ���������� �������������, �� �������������, ��� ���������.
#define REG_BMA_INT_0			0x22		//	�������� ��� ���������� ����. ���������	|											low_dur<7:0>										| 0x09	|	low_dur<7:0> - ����� �������� ��� ���������� ������������ ������ ��������� = (low_dur<7:0> + 1) * 2��. ���������� ��������� ���� ������ ��������� ����������� ������ ������� ��������. �� ���������� ������� ���������� ��� ����� �������� ����� ���������� �� 2 �� 512 ��., �������� �� ��������� 20 ��.
#define REG_BMA_INT_1			0x23		//	�������� ��� ���������� ����. ���������	|											low_th<7:0>											| 0x30	|	low_th<7:0> - ����� ������������ ��� ���������� ������������ ������ ��������� = low_th<7:0> * 7.81�g. ���������� ��������� ���� ����������� ��������� ���� ���������� ������. �� ���������� ������� ���������� ��� ����� ������������ ����� ���������� �� 0 �� 1,992 g., �������� �� ��������� 375 g.
#define REG_BMA_INT_2			0x24		//	�������� ��� ����. � �����. ���������	|		high_hy<1:0>	| -	- -	- -	- -	- -	- -	- -	- -	- -	| low_mode	|		low_hy<1:0>		| 0x81	|	low_mode - ����� ������ ���������� ������� ���������: �0� ��� ���������� ����������� ������ ������ ��� �� �����������; �1� ��� ���������� ����������� ����� ���� ����, high_hy<1:0> - ���������� ���������� ������������ ������� ��������� = high_hy<1:0> * 62.5�g * (�������� ����������� ��������� range<3:0> � g), low_hy<1:0> - ���������� ���������� ������������ ������ ��������� = low_hy<1:0> * 125�g (���������� �� ��������� ����������� ��������� range<3:0>),
#define REG_BMA_INT_3			0x25		//	�������� ��� ���������� �����. ���������|											high_dur<7:0>										| 0x0F	|	high_dur<7:0> - ����� �������� ��� ���������� ������������ ������� ��������� = (high_dur<7:0> + 1) * 2��. ���������� ��������� ���� ������� ��������� ����������� ������ ������� ��������. �� ���������� ������� ���������� ��� ����� �������� ����� ���������� �� 2 �� 512 ��., �������� �� ��������� 32 ��.
#define REG_BMA_INT_4			0x26		//	�������� ��� ���������� �����. ���������|											high_th<7:0>										| 0xC0	|	high_th<7:0> - ����� ������������ ��� ���������� ������������ ������� ��������� = high_th<7:0> * 3.905�g * (�������� ����������� ���������: range<3:0> � g = �2;�4;�8;�16), ��������� � �g. ���������� ��������� ���� ����������� ��������� ���� ���������� ������.
#define REG_BMA_INT_5			0x27		//	�������� ��� ����������	������� � ...	|							slo_no_mot_dur<5:0>							|	slope_dur<1:0>		| 0x00	|	slope_dur<1:0> - ����� �������� ��� ���������� ������������ ������ = slope_dur <1:0> + 1��, slo_no_mot_dur<5:0> - ������� �� ���� sloNoMotSel: ���� sloNoMotSel=�0� �� slo_no_mot_dur<1:0> (��� ����!) ������ ���� ���� ������ ������������ (slo_no_mot_th<7:0>) ��� ���������� ������������ ��������� ��������; ���� sloNoMotSel=�1� �� slo_no_mot_dur<5:0> ���������� ����� ������������ ��� ���������� ������������ ���������� �������� (���� slo_no_mot_dur<5:4>=�00� �� ����� = slo_no_mot_dur<3:0> + 1��, ���� slo_no_mot_dur<5:4>=�b01� �� ����� = slo_no_mot_dur<3:0> * 4 + 20��, ���� slo_no_mot_dur<5>=�1� �� ����� = slo_no_mot_dur<4:0> * 8 + 88��., 
#define REG_BMA_INT_6			0x28		//	�������� ��� ���������� �������			|											slope_th<7:0>										| 0x14	|	slope_th<7:0> - ����� ������������ ��� ���������� ������������ ������ = slope_th<7:0> * 1.955 �g * (�������� ����������� ���������: range<3:0> � g = �2;�4;�8;�16), ��������� � �g. ���������� ��������� ���� ����������� ������ ���� ���������� ������.
#define REG_BMA_INT_7			0x29		//	�������� ��� ���������� ����. ��������	|										slo_no_mot_th<7:0>										| 0x14	|	slo_no_mot_th<7:0> - ����� ������������ ��� ���������� ���������� �������� = slo_no_mot_th<7:0> * 1.955 �g * (�������� ����������� ���������: range<3:0> � g = �2;�4;�8;�16), ��������� � �g. ���������� ��������� ���� ����������� �������� ���� ���������� ������.
#define REG_BMA_INT_8			0x2A		//	�������� ��� ����������/�������� �������| tap_quiet	| tap_shock	| -	- -	- -	- -	- -	- -	- -	- -	- -	|			tap_dur<2:0>			| 0x04	|	tap_quiet - ������������ ������� ������������ �������: �0� 30��; �1� 20��, tap_shock - ������������ �������� ������������ �������: �0� 50��; �1� 70��, tap_dur<2:0> - ����� �������� ����� ������� �������� = �000b� 50��; �001b� 100��; �010b� 150��; �011b� 200��; �100b� 250��; �101b� 375��; �110b� 500��; �111b� 700��.
#define REG_BMA_INT_9			0x2B		//	�������� ��� ����������/�������� �������|	tap_samp<1:0>		| -	- -	- -	|						tap_th<4:0>							| 0x0A	|	tap_samp<1:0> - ���������� ������� ������ ��� ����������� ��������/���������� ������� � ������ (lowpower) ����������� �����������������: �00b� 2 �������; �01� 4 �������; �10� 8 �������; �11� 16 �������, tap_th<4:0> - ����� ������������ ��� ���������� ������������ �������/��������� ������� = tap_th<3:0> * 31.25 * (�������� ����������� ���������: range<3:0> � g = �2;�4;�8;�16), ��������� � �g. ���������� ��������� ���� ����������� ������� ���� ���������� ������.
#define REG_BMA_INT_A			0x2C		//	�������� ��� ���������� ����������		| -	- -	- -	|			orient_hyst<2:0>		|  orient_blocking<1:0>	|	orient_mode<1:0>	| 0x18	|	
#define REG_BMA_INT_B			0x2D		//	�������� ��� ���������� ����������		| -	- -	- -	|orient_ud_en|							orient_theta<5:0>							| 0x48	|	
#define REG_BMA_INT_C			0x2E		//	�������� ��� ���������� � ����� �����.	| -	- -	- -	- -	- -	- -	|							flat_theta<5:0>								| 0x08	|	
#define REG_BMA_INT_D			0x2F		//	�������� ��� ���������� � ����� �����.	| -	- -	- -	- -	- -	- -	|  flat_hold_time<1:0>	| -	- -	- -	|			flat_hy[2:0]			| 0x11	|	
#define REG_BMA_FIFO_CONFIG_0	0x30		//	������� watermark ��� ������ fifo		| -	- -	- -	- -	- -	- -	|			fifo_water_mark_level_trigger_retain<5:0>					| 0x00	|	fifo_water_mark_level_trigger_retain<5:0> - ������� watermark ��� ������ FIFO. ��� ������ ����� ���������� �� ����� ������ - ��������� ���������� fifo_wm ���� ��� ��������� ������ f_wm_en
#define REG_BMA_PMU_SELF_TEST	0x32		//	��������� � ������ ����������������		| -	- -	- -	- -	- -	- -	- -	- -	- -	|s_test_amp	| -	- -	- -	|s_test_sign|  self_test_axis<1:0>	| 0x00	|	s_test_amp - ���� ������ ��������� ���������������� (� ���������� ������������ ������� ���������): �1� �������; �0� ������, s_test_sign - ���� ������ ����� (����������� ����������� ������������������ ����) ����������������: �1� �������������; �0� �������������, self_test_axis<1:0> - ����� ��� ��� ����������������: �00� ��� (���������������� ���������); �01� ��� X; �10� ��� Y; �11� ��� Z; ����� ������ ��� �������� ���������������� �� ��� ������� ��������� �� ����� 50��.
#define REG_BMA_TRIM_NVM_CTRL	0x33		//	��������� ����������������� ������ NVM	|				nvm_remain<3:0>					| nvm_load	|  nvm_rdy	| nvm_trig	| nvm_mode	| 0xF0	|	nvm_remain<3:0> - ���������� ���������� ������ ������ � NVM (����������� ��������������� ����������������� ������), nvm_load - ���� ����������� ���������� ���� ���������������� ��������� �� NVM ������, nvm_rdy - ���� ��������� ���������� ������ NVM � ������: �0� ����������� ������ (����������) NVM; �1� ������ NVM ������ ������� ����� ������� ��� ������, nvm_trig - ���� ����������� �������� ������ � NVM: �1� ������; �0� ���, nvm_mode - ���� ������������� ������ � NVM: �1� ������ ��������� (��������������); �0� ������ ��������� (�������������).
#define REG_BMA_BGW_SPI3_WDT	0x34		//	��������� ��� �������� �����������		| -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	| i2c_wdt_en|i2c_wdt_sel|	spi3	| 0x00	|	i2c_wdt_en - ���� ��������� ����������� ������� WDT �� ������ SDA ��� ���� I2C: �1� �������; �0� ��������, i2c_wdt_sel - ����� ������� �������� ��� ����������� ������� �� ���� I2C: �0� 1 ��; �1� 50 ��, spi3 - ���� ������ 3-���������� ������ ���� SPI: �1� 3-��������� �����; �0� 4-��������� �����.
#define REG_BMA_OFC_CTRL		0x36		//	��������. ����������� �������. ��������	|offset_reset|	cal_trigger<1:0>	|  cal_rdy	| -	- -	- -	|  hp_z_en	|  hp_y_en	|  hp_x_en	| 0x10	|	offset_reset - ���� ������ �������� ��������� ��������: �1� �������� �������� �������� (0x38 - 0x3A) � ����; �0� ������� �������� ��������� ��������, cal_trigger<1:0> - ��������� ������� ���������� ����������� �������� ���: �01� ��� X; �10� ���Y; �11� ��� Z; �00� �� ��������� ������� ���������� ����������� ��������; (������� ���������� ����������� �������� �� ������ ���������� ��� ���������� ����� cal_rdy), cal_rdy - ���� ����������� � ���������� ���� � ������ �������� ���������� ����������� ��������: �1� �����; �0� �� �����, hp_z_en - ���� ����������� ��������� ���������� ����������� �������� �� ��� Z: �1� ���������; �0� �� ���������, hp_y_en - ���� ����������� ��������� ���������� ����������� �������� �� ��� Y: �1� ���������; �0� �� ���������, hp_x_en - ���� ����������� ��������� ���������� ����������� �������� �� ��� X: �1� ���������; �0� �� ���������.
#define REG_BMA_OFC_SETTING		0x37		//	��������� ���������� �������. ��������	| -	- -	- -	| offset_target_z<1:0>	| offset_target_y<1:0>	| offset_target_x<1:0>	|  cut_off	| 0x00	|	offset_target_z<1:0> - ���������� �������� ����������� ��� ��� Z �: �00� 0g.; �01� +1g.; �10� -1g.; �11� 0g., offset_target_y<1:0> - ���������� �������� ����������� ��� ��� Y �: �00� 0g.; �01� +1g.; �10� -1g.; �11� 0g., offset_target_x<1:0> - ���������� �������� ����������� ��� ��� X �: �00� 0g.; �01� +1g.; �10� -1g.; �11� 0g., cut_off - ��������� ������� ������� ��� ���������� ���������� ����������� ��������: �0� 1��; �1� 10��
#define REG_BMA_OFC_OFFSET_X	0x38		//	�������� ����������� �������� �� ��� X	|										offset_x<7:0>											| 0x00	|	offset_x<7:0> - �������� ����������� �������� ��� ������ �������� ��������� ����������� �� ��� X. �������� �������� ����������� � ���������� ������������� � ��������������� ������ ���������; �������� �������� �������������� � ���� ����� � ���� ���������� ���� �� -128 (-1g) �� +127 (+0,992g) � �� ������� �� ���������� ��������� ���������; ���������� ����� �������� ����� ���� �������� � NVM (�� ���� ����� ������������� ����������������� ����� ������� ��������� ������� ��� ����������� ������������), ��� ����� ���� ��������� (������� ��� ��������� ��������) ��������� ���������� ������������ � ������� ������������� �� ���������� ����������, ��� �������� �������������.
#define REG_BMA_OFC_OFFSET_Y	0x39		//	�������� ����������� �������� �� ��� Y	|										offset_y<7:0>											| 0x00	|	offset_y<7:0> - �������� ����������� �������� ��� ������ �������� ��������� ����������� �� ��� Y. �������� �������� ����������� � ���������� ������������� � ��������������� ������ ���������; �������� �������� �������������� � ���� ����� � ���� ���������� ���� �� -128 (-1g) �� +127 (+0,992g) � �� ������� �� ���������� ��������� ���������; ���������� ����� �������� ����� ���� �������� � NVM (�� ���� ����� ������������� ����������������� ����� ������� ��������� ������� ��� ����������� ������������), ��� ����� ���� ��������� (������� ��� ��������� ��������) ��������� ���������� ������������ � ������� ������������� �� ���������� ����������, ��� �������� �������������.
#define REG_BMA_OFC_OFFSET_Z	0x3A		//	�������� ����������� �������� �� ��� Z	|										offset_z<7:0>											| 0x00	|	offset_z<7:0> - �������� ����������� �������� ��� ������ �������� ��������� ����������� �� ��� Z. �������� �������� ����������� � ���������� ������������� � ��������������� ������ ���������; �������� �������� �������������� � ���� ����� � ���� ���������� ���� �� -128 (-1g) �� +127 (+0,992g) � �� ������� �� ���������� ��������� ���������; ���������� ����� �������� ����� ���� �������� � NVM (�� ���� ����� ������������� ����������������� ����� ������� ��������� ������� ��� ����������� ������������), ��� ����� ���� ��������� (������� ��� ��������� ��������) ��������� ���������� ������������ � ������� ������������� �� ���������� ����������, ��� �������� �������������.
#define REG_BMA_TRIM_GPO		0x3B		//	������� ������ NVM ������ ����������	|											GP0<7:0>											| 0x00	|	GP0<7:0> - ������ ����� �������� ����� ���� �������� � NVM (����������� ��������������� ����������������� ������) � ����� ����������������� �� �� � ���� ������� ��� ������ ��������� ��� ����������� ������������. ������� �� ���� ����� ���� ���������������� � ����� ���� ����������� �������������
#define REG_BMA_TRIM_GP1		0x3C		//	������� ������ NVM ������ ����������	|											GP1<7:0>											| 0x00	|	GP1<7:0> - ������ ����� �������� ����� ���� �������� � NVM (����������� ��������������� ����������������� ������) � ����� ����������������� �� �� � ���� ������� ��� ������ ��������� ��� ����������� ������������. ������� �� ���� ����� ���� ���������������� � ����� ���� ����������� �������������
#define REG_BMA_FIFO_CONFIG_1	0x3E		//	��������� ������������ ������ FIFO		|	fifo_mode<1:0>		| -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	- -	| fifo_data_select<1:0>	| 0x00	|	fifo_mode<1:0> - ����� ������ ������ ������ FIFO: �00� BYPASS (������� ������ 1 ����, ������ ������ ���������); �01� FIFO (���� ������ ������������ ��� ������ ���������� ������ - 32 �����); �10� STREAM (���� ������ ������������ ��� ������ ���������� ������, ������ ����� ���������), fifo_data_select<1:0> - ����� ������ ��� ������ FIFO: �00� ���������� ������ ���� ���� XYZ; �01� ���������� ������ ������ ��� X; �10� ���������� ������ ������ ��� Y; �11� ���������� ������ ������ ��� Z. ��� ������ ������ � ������� FIFO_CONFIG_1, ������ FIFO ������ � ���� fifo-full ���������.
#define REG_BMA_FIFO_DATA		0x3F		//	������� ������ ������ �� ������ FIFO	|										fifo_data<7:0>											| 0x00	|	fifo_data<7:0> - ���� ��� ������ ������ �� ������ FIFO. ������ ������ FIFO �������� ������� (����� ������ ����� 32 �����). ������ ����� ������� �� �������� ����� fifo_data_select<1:0> �������� REG_BMA_FIFO_CONFIG_1: ���� ������� ������ ���� ���� XYZ, �� ������ ���� ����������� �������� � ��������� ������� X-lsb, X-msb, Y-lsb, Y-msb, Z-lsb, Z-msb; ���� ������� ������ ������ ��� X, �� ������ ���� ����������� �������� � ��������� ������� X-lsb, X-msb; ���� ������� ������ ������ ��� Y ��� ������ ��� Z, �� ������ ���� ����������� ��������, ��� � ��� ��� X. ���� ������ ������ ����������� � ���� ������ �� ���������, �� ��� ��������� ������������� ������ � fifo_data<7:0> ����� ���������� ������ ���� ���������� �����.


class BMX055
{
public:
    BMX055(I2c *i2c);
  
    uint8_t readReg(uint8_t reg);
    bool writeReg(uint8_t reg, uint8_t value);
    
    bool isValid() const {return m_valid;}
    bool readAcc();
    
    float accX() const {return m_value[0];}
    float accY() const {return m_value[1];}
    float accZ() const {return m_value[2];}
    
private:
    I2c *m_dev;
    bool m_valid;
    
    void setFastOffset();
    
public:
    int16_t m_acc[3];
    float m_value[3];
    
};


#endif