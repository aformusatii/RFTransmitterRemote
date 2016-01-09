/********************************************************************************
    Macros and Defines
********************************************************************************/
#define BT_POWER_TV case 2: return 1;
#define BT_POWER_DVD case 0: return 60;
#define BT_POWER_BD case 145: return 90;

#define BT_INPUT_TV case 1: return 2;

#define BT_DIMMER_TV case 139: return 3;

#define BT_1_TV case 4: return 4;
#define BT_1_DVD case 2: return 61;
#define BT_1_BD case 129: return 91;

#define BT_2_TV case 5: return 5;
#define BT_2_DVD case 3: return 62;
#define BT_2_BD case 130: return 92;

#define BT_3_TV case 6: return 6;
#define BT_3_DVD case 4: return 63;
#define BT_3_BD case 131: return 93;

#define BT_4_TV case 8: return 7;
#define BT_4_DVD case 5: return 64;
#define BT_4_BD case 132: return 94;

#define BT_5_TV case 9: return 8;
#define BT_5_DVD case 6: return 65;
#define BT_5_BD case 133: return 95;

#define BT_6_TV case 10: return 9;
#define BT_6_DVD case 7: return 66;
#define BT_6_BD case 134: return 96;

#define BT_7_TV case 12: return 10;
#define BT_7_DVD case 8: return 67;
#define BT_7_BD case 135: return 97;

#define BT_8_TV case 13: return 11;
#define BT_8_DVD case 9: return 68;
#define BT_8_BD case 136: return 98;

#define BT_9_TV case 14: return 12;
#define BT_9_DVD case 10: return 69;
#define BT_9_BD case 137: return 99;

#define BT_PROLOGIC_TV case 35: return 13;

#define BT_0_TV case 17: return 14;
#define BT_0_DVD case 11: return 70;
#define BT_0_BD case 128: return 111; // 100 - reserved by temperature sensor

#define BT_DSP_TV case 19: return 15;

#define BT_PREV_TV case 0: return 16;
#define BT_PREV_DVD case 13: return 71;
#define BT_PREV_BD case 159: return 101;

#define BT_NEXT_TV case 115: return 17;
#define BT_NEXT_DVD case 17: return 72;
#define BT_NEXT_BD case 161: return 102;

#define BT_SKIP_PREV_TV case 69: return 18;
#define BT_SKIP_PREV_DVD case 18: return 73;
#define BT_SKIP_PREV_BD case 156: return 103;

#define BT_SKIP_NEXT_TV case 72: return 19;
#define BT_SKIP_NEXT_DVD case 21: return 74;
#define BT_SKIP_NEXT_BD case 158: return 104;

#define BT_STOP_TV case 70: return 20;
#define BT_STOP_DVD case 19: return 75;
#define BT_STOP_BD case 162: return 105;

#define BT_PLAY_TV case 71: return 21;
#define BT_PLAY_DVD case 20: return 76;
#define BT_PLAY_BD case 160: return 106;

#define BT_PAUSE_TV case 74: return 23;
#define BT_PAUSE_DVD case 50: return 77;
#define BT_PAUSE_BD case 240: return 107;

#define BT_VOLP_TV case 7: return 24;

#define BT_VOLM_TV case 11: return 25;

#define BT_MUTE_TV case 15: return 26;

#define BT_TUNINGP_TV case 18: return 27;

#define BT_TUNINGM_TV case 16: return 28;

#define BT_ASC_TV case 107: return 29;

#define BT_TUNER_MEM_TV case 121: return 30;

#define BT_SUB_WOOFER_TV case 75: return 31;

#define BT_MO_ST_TV case 159: return 32;

#define BT_MENU_TV case 26: return 33;
#define BT_MENU_DVD case 22: return 78;
#define BT_MENU_BD case 139: return 108;

#define BT_INFO_TV case 31: return 34;
#define BT_INFO_DVD case 30: return 79;
#define BT_INFO_BD case 143: return 109;

#define BT_RETURN_TV case 88: return 35;
#define BT_RETURN_DVD case 23: return 80;
#define BT_RETURN_BD case 149: return 110;

#define BT_EXIT_TV case 45: return 36;
#define BT_EXIT_DVD case 43: return 81;
#define BT_EXIT_BD case 181: return 112;

#define BT_UP_TV case 96: return 37;
#define BT_UP_DVD case 24: return 82;
#define BT_UP_BD case 151: return 113;

#define BT_DOWN_TV case 97: return 38;
#define BT_DOWN_DVD case 25: return 83;
#define BT_DOWN_BD case 152: return 114;

#define BT_LEFT_TV case 101: return 39;
#define BT_LEFT_DVD case 27: return 84;
#define BT_LEFT_BD case 154: return 115;

#define BT_RIGHT_TV case 98: return 40;
#define BT_RIGHT_DVD case 26: return 85;
#define BT_RIGHT_BD case 153: return 116;

#define BT_OK_TV case 104: return 41;
#define BT_OK_DVD case 28: return 86;
#define BT_OK_BD case 155: return 117;

/********************************************************************************
    Function Prototypes
********************************************************************************/
int mapTVKey(int irValue);
int mapBDKey(int irValue);
int mapDVDKey(int irValue);
