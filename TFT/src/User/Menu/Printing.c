#include "Printing.h"
#include "includes.h"

const GUI_POINT printinfo_points[6] = {
{START_X + PICON_LG_WIDTH*0 + PICON_SPACE_X*0, ICON_START_Y + PICON_HEIGHT*0 + PICON_SPACE_Y*0},
{START_X + PICON_LG_WIDTH*1 + PICON_SPACE_X*1, ICON_START_Y + PICON_HEIGHT*0 + PICON_SPACE_Y*0},
{START_X + PICON_LG_WIDTH*2 + PICON_SPACE_X*2, ICON_START_Y + PICON_HEIGHT*0 + PICON_SPACE_Y*0},
{START_X + PICON_LG_WIDTH*0 + PICON_SPACE_X*0, ICON_START_Y + PICON_HEIGHT*1 + PICON_SPACE_Y*1},
{START_X + PICON_LG_WIDTH*1 + PICON_SPACE_X*1, ICON_START_Y + PICON_HEIGHT*1 + PICON_SPACE_Y*1},
{START_X + PICON_LG_WIDTH*2 + PICON_SPACE_X*2, ICON_START_Y + PICON_HEIGHT*1 + PICON_SPACE_Y*1},
};

const GUI_RECT printinfo_val_rect[6] = {
  {START_X + PICON_LG_WIDTH*0 + PICON_SPACE_X*0 + PICON_VAL_X,              ICON_START_Y + PICON_HEIGHT*0 + PICON_SPACE_Y*0 + PICON_VAL_Y,
        START_X + PICON_LG_WIDTH*0 + PICON_SPACE_X*0 + PICON_VAL_LG_EX,     ICON_START_Y + PICON_HEIGHT*0 + PICON_SPACE_Y*0 + PICON_VAL_Y + BYTE_HEIGHT},

  {START_X + PICON_LG_WIDTH*1 + PICON_SPACE_X*1 + PICON_VAL_X,              ICON_START_Y + PICON_HEIGHT*0 + PICON_SPACE_Y*0 + PICON_VAL_Y,
        START_X + PICON_LG_WIDTH*1 + PICON_SPACE_X*1 + PICON_VAL_LG_EX,     ICON_START_Y + PICON_HEIGHT*0 + PICON_SPACE_Y*0 + PICON_VAL_Y + BYTE_HEIGHT},

  {START_X + PICON_LG_WIDTH*2 + PICON_SPACE_X*2 + PICON_VAL_X,              ICON_START_Y + PICON_HEIGHT*0 + PICON_SPACE_Y*0 + PICON_VAL_Y,
        START_X + PICON_LG_WIDTH*2 + PICON_SPACE_X*2 + PICON_VAL_SM_EX,     ICON_START_Y + PICON_HEIGHT*0 + PICON_SPACE_Y*0 + PICON_VAL_Y + BYTE_HEIGHT},

  {START_X + PICON_LG_WIDTH*0 + PICON_SPACE_X*0 + PICON_VAL_X,              ICON_START_Y + PICON_HEIGHT*1 + PICON_SPACE_Y*1 + PICON_VAL_Y,
        START_X + PICON_LG_WIDTH*0 + PICON_SPACE_X*0 + PICON_VAL_LG_EX,     ICON_START_Y + PICON_HEIGHT*1 + PICON_SPACE_Y*1 + PICON_VAL_Y + BYTE_HEIGHT},

  {START_X + PICON_LG_WIDTH*1 + PICON_SPACE_X*1 + PICON_VAL_X,              ICON_START_Y + PICON_HEIGHT*1 + PICON_SPACE_Y*1 + PICON_VAL_Y,
        START_X + PICON_LG_WIDTH*1 + PICON_SPACE_X*1 + PICON_VAL_LG_EX,     ICON_START_Y + PICON_HEIGHT*1 + PICON_SPACE_Y*1 + PICON_VAL_Y + BYTE_HEIGHT},

  {START_X + PICON_LG_WIDTH*2 + PICON_SPACE_X*2 + PICON_VAL_X,               ICON_START_Y + PICON_HEIGHT*1 + PICON_SPACE_Y*1 + PICON_VAL_Y,
        START_X + PICON_LG_WIDTH*2 + PICON_SPACE_X*2 + PICON_VAL_SM_EX,     ICON_START_Y + PICON_HEIGHT*1 + PICON_SPACE_Y*1 + PICON_VAL_Y + BYTE_HEIGHT},
};

static u32 nextTime = 0;
static u32 toggle_time = 2000; // 1 seconds is 1000
TOOL c_Ext = NOZZLE0;
static int c_fan = 0;
static int c_speedID = 0;
static int key_pause = 4;
const char* Speed_ID[2] = {"Speed","Flow"};
#define LAYER_TITLE "Layer"
#define EXT_ICON_POS  0
#define BED_ICON_POS  1
#define FAN_ICON_POS  2
#define TIM_ICON_POS  3
#define Z_ICON_POS    4
#define SPD_ICON_POS  5

#ifdef RAPID_SERIAL_COMM
#define rapid_serial_loop()  loopBackEnd()
#else
#define rapid_serial_loop()
#endif
//1title, ITEM_PER_PAGE item(icon + label)
MENUITEMS printingItems = {
//  title
LABEL_BACKGROUND,
// icon                       label
 {{ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_PAUSE,                LABEL_PAUSE},
  {ICON_BABYSTEP,             LABEL_BABYSTEP},
  {ICON_MORE,                 LABEL_MORE},
  {ICON_STOP,                 LABEL_STOP},}
};
const ITEM itemBlank      = {ICON_BACKGROUND, LABEL_BACKGROUND};
const ITEM itemBabyStep   = {ICON_BABYSTEP, LABEL_BABYSTEP};
const ITEM itemIsPause[2] = {
// icon                       label
  {ICON_PAUSE,                LABEL_PAUSE},
  {ICON_RESUME,               LABEL_RESUME},
};

static PRINTING infoPrinting;

//
bool isPrinting(void)
{
  return infoPrinting.printing;
}

//only return gcode file name except path
//for example:"SD:/test/123.gcode"
//only return "123.gcode"
u8 *getCurGcodeName(char *path)
{
  int i=strlen(path);
  for(; path[i]!='/'&& i>0; i--)
  {}
  return (u8* )(&path[i+1]);
}

void reDrawFileName(void)
{
  printingItems.title.address = getCurGcodeName(infoFile.title);
  menuDrawPage(&printingItems);
}

void setPrinting(bool print)
{
  if (print) 
  {
    memset(&infoPrinting,0,sizeof(PRINTING));
    printingItems.items[KEY_ICON_7].icon = ICON_STOP;
    printingItems.items[KEY_ICON_7].label.index = LABEL_STOP;
  } 
  else
  {
    infoPrinting.printing = infoPrinting.pause = false;
    printingItems.items[KEY_ICON_7].icon = ICON_BACK;
    printingItems.items[KEY_ICON_7].label.index = LABEL_BACK;
  }
  
  infoPrinting.printing = print;
  // if (infoMenu.menu[infoMenu.cur] == menuPrinting)
  //   menuDrawItem(&printingItems.items[KEY_ICON_7], KEY_ICON_7);
}

void setPause(bool is_pause)
{
  infoPrinting.pause = is_pause;
  resumeToPause(is_pause);
}

bool isPause(void)
{
  return infoPrinting.pause;
}

bool isM0_Pause(void)
{
return infoPrinting.m0_pause;
}

//
void setPrintingTime(u32 RTtime)
{
  if(RTtime%1000 == 0)
  {
    if(isPrinting() && !isPause())
    {
      infoPrinting.time++;
    }
  }
}

//
void setPrintSize(u32 size)
{
  infoPrinting.size = size;
}

//
void setPrintCur(u32 cur)
{
  infoPrinting.cur = cur;
}

u8 getPrintProgress(void)
{
  return infoPrinting.progress;
}
u32 getPrintTime(void)
{
  return infoPrinting.time;
}

void printerGotoIdle(void)
{
  // disable all heater
  for(TOOL i = BED; i < HEATER_NUM; i++) {
    mustStoreCmd("%s S0\n", heatCmd[i]);
  }
  // disable all fan
  for(u8 i = 0; i < FAN_NUM; i++) {
    mustStoreCmd("%s S0\n", fanCmd[i]);
  }
  // disable all stepper
  mustStoreCmd("M18\n");
}



void menuBeforePrinting(void)
{
  request_M23(infoFile.title+3);
  request_M24(0);
  infoPrinting.printing = true;
  infoMenu.menu[infoMenu.cur] = menuPrinting;
  printingItems.title.address = getCurGcodeName(infoFile.title);
  printingItems.items[KEY_ICON_7].icon = ICON_STOP;
  printingItems.items[KEY_ICON_7].label.index = LABEL_STOP;
}


void resumeToPause(bool is_pause)
{
  if(infoMenu.menu[infoMenu.cur] != menuPrinting) return;
  printingItems.items[key_pause] = itemIsPause[is_pause];
  menuDrawItem(&printingItems.items[key_pause],key_pause);
}

void setM0Pause(bool m0_pause){
  infoPrinting.m0_pause = m0_pause;
}

bool setPrintPause(bool is_pause, bool is_m0pause)
{
  static bool pauseLock = false;
  if(pauseLock)                      return false;
  if(!isPrinting())                  return false;
  if(infoPrinting.pause == is_pause) return false;

  pauseLock = true;
  infoPrinting.pause = is_pause;

  if (is_pause){
    request_M25();
  } else {
    request_M24(0);
  }

  resumeToPause(is_pause);
  pauseLock = false;
  return true;
}

const GUI_RECT progressRect = {1*SPACE_X_PER_ICON, 0*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y + ICON_HEIGHT/4,
                               3*SPACE_X_PER_ICON, 0*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y + ICON_HEIGHT*3/4};

#define BED_X  (progressRect.x1 - 9 * BYTE_WIDTH)
#define TEMP_Y (progressRect.y1 + 3)
#define TIME_Y (TEMP_Y + 1 * BYTE_HEIGHT + 3)

void reValueNozzle(int icon_pos)
{
  char tempstr[10];
  my_sprintf(tempstr, "%d/%d", heatGetCurrentTemp(c_Ext), heatGetTargetTemp(c_Ext));

  GUI_SetTextMode(GUI_TEXTMODE_TRANS);

  ICON_CustomReadDisplay(printinfo_points[icon_pos].x,printinfo_points[icon_pos].y,PICON_LG_WIDTH,PICON_HEIGHT,ICON_ADDR(ICON_PRINTING_NOZZLE));
  GUI_DispString(printinfo_points[icon_pos].x + PICON_TITLE_X, printinfo_points[icon_pos].y + PICON_TITLE_Y, (u8* )heatDisplayID[c_Ext]);
  GUI_DispStringInPrect(&printinfo_val_rect[icon_pos], (u8 *)tempstr);

  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
}

void reValueBed(int icon_pos)
{
  char tempstr[10];
  my_sprintf(tempstr, "%d/%d", heatGetCurrentTemp(BED), heatGetTargetTemp(BED));

  GUI_SetTextMode(GUI_TEXTMODE_TRANS);

  ICON_CustomReadDisplay(printinfo_points[icon_pos].x,printinfo_points[icon_pos].y,PICON_LG_WIDTH,PICON_HEIGHT,ICON_ADDR(ICON_PRINTING_BED));
  GUI_DispString(printinfo_points[icon_pos].x + PICON_TITLE_X, printinfo_points[icon_pos].y + PICON_TITLE_Y, (u8* )heatDisplayID[BED]);
  GUI_DispStringInPrect(&printinfo_val_rect[icon_pos], (u8 *)tempstr);

  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
}

void reDrawFan(int icon_pos)
{
  char tempstr[10];
  u8 fs;
  #ifdef SHOW_FAN_PERCENTAGE
    fs = (fanGetSpeed(c_fan)*100)/255;
    my_sprintf(tempstr, "%d%%", fs);
  #else
    fs = fanGetSpeed(c_fan);
    my_sprintf(tempstr, "%d", fs);
  #endif

  GUI_SetTextMode(GUI_TEXTMODE_TRANS);

  ICON_CustomReadDisplay(printinfo_points[icon_pos].x,printinfo_points[icon_pos].y,PICON_SM_WIDTH,PICON_HEIGHT,ICON_ADDR(ICON_PRINTING_FAN));
  GUI_DispString(printinfo_points[icon_pos].x + PICON_TITLE_X, printinfo_points[icon_pos].y + PICON_TITLE_Y, (u8*)fanID[c_fan]);
  GUI_DispStringInPrect(&printinfo_val_rect[icon_pos], (u8 *)tempstr);

  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
}

void reDrawSpeed(int icon_pos)
{
  char tempstr[10];
  GUI_SetTextMode(GUI_TEXTMODE_TRANS);

  my_sprintf(tempstr, "%d%%", speedGetPercent(c_speedID) );

  if(c_speedID == 0){
  ICON_CustomReadDisplay(printinfo_points[icon_pos].x,printinfo_points[icon_pos].y,PICON_SM_WIDTH,PICON_HEIGHT,ICON_ADDR(ICON_PRINTING_SPEED));
  }
  else{
  ICON_CustomReadDisplay(printinfo_points[icon_pos].x,printinfo_points[icon_pos].y,PICON_SM_WIDTH,PICON_HEIGHT,ICON_ADDR(ICON_PRINTING_FLOW));
  }
  GUI_DispString(printinfo_points[icon_pos].x + PICON_TITLE_X, printinfo_points[icon_pos].y + PICON_TITLE_Y, (u8 *)Speed_ID[c_speedID]);
  GUI_DispStringInPrect(&printinfo_val_rect[icon_pos], (u8 *)tempstr);

  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
}

void reDrawTime(int icon_pos)
{
  u8  hour = infoPrinting.time/3600,
      min = infoPrinting.time%3600/60,
      sec = infoPrinting.time%60;

  GUI_SetNumMode(GUI_NUMMODE_ZERO);
  GUI_SetTextMode(GUI_TEXTMODE_TRANS);
  char tempstr[10];
  sprintf(tempstr, "%02d:%02d:%02d", hour,min,sec);
  ICON_CustomReadDisplay(printinfo_points[icon_pos].x,printinfo_points[icon_pos].y,PICON_LG_WIDTH,PICON_HEIGHT,ICON_ADDR(ICON_PRINTING_TIMER));
  GUI_DispStringInPrect(&printinfo_val_rect[icon_pos], (u8 *)tempstr);

  GUI_SetNumMode(GUI_NUMMODE_SPACE);
  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
}

void reDrawProgress(int icon_pos)
{
  char buf[6];
  my_sprintf(buf, "%d%%", infoPrinting.progress);

  GUI_SetTextMode(GUI_TEXTMODE_TRANS);

  GUI_DispString(printinfo_points[3].x + PICON_TITLE_X, printinfo_points[3].y + PICON_TITLE_Y, (u8 *)buf);

  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
}

void reDrawLayer(int icon_pos)
{
  char tempstr[10];
  my_sprintf(tempstr, "%.2fMM",coordinateGetAxisTarget(Z_AXIS));

  GUI_SetTextMode(GUI_TEXTMODE_TRANS);

  ICON_CustomReadDisplay(printinfo_points[icon_pos].x,printinfo_points[icon_pos].y,PICON_LG_WIDTH,PICON_HEIGHT,ICON_ADDR(ICON_PRINTING_ZLAYER));
  GUI_DispString(printinfo_points[icon_pos].x + PICON_TITLE_X, printinfo_points[icon_pos].y + PICON_TITLE_Y, (u8* )LAYER_TITLE);
  GUI_DispStringInPrect(&printinfo_val_rect[icon_pos], (u8 *)tempstr);

  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
}

void toggleinfo(void)
{
  if (OS_GetTimeMs() > nextTime)
  {
    if (EXTRUDER_NUM > 1)
    {
      c_Ext = (TOOL)((c_Ext + 1) % HEATER_NUM);
      if (c_Ext == 0)
      {
        c_Ext += 1;
      }
      rapid_serial_loop();	 //perform backend printing loop before drawing to avoid printer idling
      reValueNozzle(EXT_ICON_POS);
    }

    if (FAN_NUM > 1)
    {
      c_fan = (c_fan + 1) % FAN_NUM;
      rapid_serial_loop();	 //perform backend printing loop before drawing to avoid printer idling
      reDrawFan(FAN_ICON_POS);
    }

    c_speedID = (c_speedID + 1) % 2;
    nextTime = OS_GetTimeMs() + toggle_time;
    rapid_serial_loop();	 //perform backend printing loop before drawing to avoid printer idling
    reDrawSpeed(SPD_ICON_POS);
  }
}

//extern SCROLL   titleScroll;
//extern GUI_RECT titleRect;


void printingDrawPage(void)
{
  //	Scroll_CreatePara(&titleScroll, infoFile.title,&titleRect);  //
  if(get_Pre_Icon() == true){
    key_pause = 5;
    //printingItems.items[key_pause - 1] = itemBlank;
    printingItems.items[key_pause - 1].icon = ICON_PREVIEW;
    printingItems.items[key_pause - 1].label.index = LABEL_BACKGROUND;
  }
  else{
    key_pause = 4;
    printingItems.items[key_pause+1] = itemBabyStep;
  }

    printingItems.items[key_pause] = itemIsPause[isPause()];

  menuDrawPage(&printingItems);
  reValueNozzle(EXT_ICON_POS);
  reValueBed(BED_ICON_POS);
  reDrawFan(FAN_ICON_POS);
  reDrawTime(TIM_ICON_POS);
  reDrawProgress(TIM_ICON_POS);
  reDrawLayer(Z_ICON_POS);
  reDrawSpeed(SPD_ICON_POS);
}


void menuPrinting(void)
{
  KEY_VALUES  key_num = KEY_IDLE;
  u32         time = 0;
  HEATER      nowHeat;
  float       curLayer = 0;
  u8          nowFan[FAN_NUM] = {0};
  uint16_t    curspeed[2] = {0};
  memset(&nowHeat, 0, sizeof(HEATER));

  printingDrawPage();
  printingItems.items[key_pause] = itemIsPause[infoPrinting.pause];


  while(infoMenu.menu[infoMenu.cur] == menuPrinting)
  {
//    Scroll_DispString(&titleScroll, LEFT); //Scroll display file name will take too many CPU cycles

    //check nozzle temp change
      if (nowHeat.T[c_Ext].current != heatGetCurrentTemp(c_Ext) || nowHeat.T[c_Ext].target != heatGetTargetTemp(c_Ext))
      {
        nowHeat.T[c_Ext].current = heatGetCurrentTemp(c_Ext);
        nowHeat.T[c_Ext].target = heatGetTargetTemp(c_Ext);
        rapid_serial_loop();	 //perform backend printing loop before drawing to avoid printer idling
        reValueNozzle(EXT_ICON_POS);
      }

    //check bed temp change
    if (nowHeat.T[BED].current != heatGetCurrentTemp(BED) || nowHeat.T[BED].target != heatGetTargetTemp(BED))
    {
      nowHeat.T[BED].current = heatGetCurrentTemp(BED);
      nowHeat.T[BED].target = heatGetTargetTemp(BED);
      rapid_serial_loop();	 //perform backend printing loop before drawing to avoid printer idling
      reValueBed(BED_ICON_POS);
    }

    //check Fan speed change
    if (nowFan[c_fan] != fanGetSpeed(c_fan))
    {
      nowFan[c_fan] = fanGetSpeed(c_fan);
      rapid_serial_loop();  //perform backend printing loop before drawing to avoid printer idling
      reDrawFan(FAN_ICON_POS);
    }

   
    //check print time change
    if(time!=infoPrinting.time || infoPrinting.progress!=limitValue(0,(uint64_t)infoPrinting.cur,100))
    {
      time=infoPrinting.time;
      infoPrinting.progress=limitValue(0,(uint64_t)infoPrinting.cur,100);
      rapid_serial_loop();  //perform backend printing loop before drawing to avoid printer idling
      reDrawTime(TIM_ICON_POS);
      reDrawProgress(TIM_ICON_POS);
    }

    //Z_AXIS coordinate
    if(curLayer != coordinateGetAxisTarget(Z_AXIS)){
      curLayer = coordinateGetAxisTarget(Z_AXIS);
      rapid_serial_loop();  //perform backend printing loop before drawing to avoid printer idling
      reDrawLayer(Z_ICON_POS);
    }

    //check change in speed or flow
    if(curspeed[c_speedID] != speedGetPercent(c_speedID)){
      curspeed[c_speedID] = speedGetPercent(c_speedID);
      rapid_serial_loop();  //perform backend printing loop before drawing to avoid printer idling
      reDrawSpeed(SPD_ICON_POS);
    }

    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_4:
        if(get_Pre_Icon() != true){
        setPrintPause(!isPause(),false);
        }
        break;

      case KEY_ICON_5:
        if(get_Pre_Icon() == true){
        setPrintPause(!isPause(),false);
        }
        else{
        infoMenu.menu[++infoMenu.cur] = menuBabyStep;
        }
        break;

      case KEY_ICON_6:
        infoMenu.menu[++infoMenu.cur] = menuMore;
        break;

      case KEY_ICON_7:
        if(isPrinting())
          infoMenu.menu[++infoMenu.cur] = menuStopPrinting;
        else
        {
          exitPrinting();
          infoMenu.cur--;
        }
        break;

      default :break;
    }
    loopProcess();
    toggleinfo();
  }
}

void exitPrinting(void)
{
  memset(&infoPrinting,0,sizeof(PRINTING));
  ExitDir();
}

void endPrinting(void)
{
  infoPrinting.printing = infoPrinting.pause = false;
  // powerFailedClose();
  // powerFailedDelete();
  if(infoSettings.send_end_gcode == 1){
    mustStoreCmd(PRINT_END_GCODE);
  }
}


void completePrinting(void)
{
  setPrinting(false);
  // if (infoMenu.menu[infoMenu.cur] == menuPrinting)
  //   menuDrawItem(&printingItems.items[KEY_ICON_7], KEY_ICON_7);

  BUZZER_PLAY(sound_success);

  infoMenu.cur--;

  u8  hour = infoPrinting.time/3600,
      min = infoPrinting.time%3600/60,
      sec = infoPrinting.time%60;

  char tempstr[25];
  sprintf(tempstr, "Print time was %02d:%02d:%02d", hour,min,sec);
  // statusScreen_setMsg((u8*)"Complete",(u8*)tempstr);
  popupReminder((u8*)"Complete",(u8*)tempstr);

  // if(infoSettings.auto_off) // Auto shut down after printing
  // {
	// 	infoMenu.menu[++infoMenu.cur] = menuShutDown;
  // }
  exitPrinting();
  
}

void abortPrinting(void)
{
  request_M524();

  heatClearIsWaiting();

  if(infoSettings.send_cancel_gcode == 1){
    mustStoreCmd(PRINT_CANCEL_GCODE);
  }

  endPrinting();
  printerGotoIdle();
  exitPrinting();
}

void menuStopPrinting(void)
{
  u16 key_num = IDLE_TOUCH;

  popupDrawPage(bottomDoubleBtn, textSelect(LABEL_WARNING), textSelect(LABEL_STOP_PRINT), textSelect(LABEL_CONFIRM), textSelect(LABEL_CANNEL));

  while(infoMenu.menu[infoMenu.cur] == menuStopPrinting)
  {
    key_num = KEY_GetValue(2, doubleBtnRect);
    switch(key_num)
    {
      case KEY_POPUP_CONFIRM:
        abortPrinting();
        infoMenu.cur-=2;
        break;

      case KEY_POPUP_CANCEL:
        infoMenu.cur--;
        break;
    }
    loopProcess();
  }
}

void menuStartPrinting(void)
{
  u16 key_num = IDLE_TOUCH;

  char buf[89];
  sprintf(buf, "Do you want to start %.65s?\n", infoFile.title);

  popupDrawPage(bottomDoubleBtn, textSelect(LABEL_INFO), (u8*)buf, textSelect(LABEL_CONFIRM), textSelect(LABEL_CANNEL));

  while(infoMenu.menu[infoMenu.cur] == menuStartPrinting)
  {
    key_num = KEY_GetValue(2, doubleBtnRect);
    switch(key_num)
    {
      case KEY_POPUP_CONFIRM:
        infoMenu.menu[infoMenu.cur] = menuBeforePrinting;
        break;

      case KEY_POPUP_CANCEL:
        ExitDir();
        infoMenu.cur--;
        break;
    }
    loopProcess();
  }
}

// Shut down menu, when the hotend temperature is higher than "AUTO_SHUT_DOWN_MAXTEMP"
// wait for cool down, in the meantime, you can shut down by force
void menuShutDown(void)
{
  bool tempIsLower;
  u16 key_num = IDLE_TOUCH;

  popupDrawPage(bottomDoubleBtn, textSelect(LABEL_SHUT_DOWN), textSelect(LABEL_WAIT_TEMP_SHUT_DOWN), textSelect(LABEL_FORCE_SHUT_DOWN), textSelect(LABEL_CANNEL));

  for(u8 i = 0; i < FAN_NUM; i++)
  {
    mustStoreCmd("%s S255\n", fanCmd[i]);
  }
  while (infoMenu.menu[infoMenu.cur] == menuShutDown)
  {
    key_num = KEY_GetValue(2, doubleBtnRect);
    switch(key_num)
    {
      case KEY_POPUP_CONFIRM:
        goto shutdown;

      case KEY_POPUP_CANCEL:
        infoMenu.cur--;
        break;
    }
    tempIsLower = true;
    for (TOOL i = NOZZLE0; i < HEATER_NUM; i++)
    {
      if(heatGetCurrentTemp(NOZZLE0) >= AUTO_SHUT_DOWN_MAXTEMP)
        tempIsLower = false;
    }
    if(tempIsLower)
    {
      shutdown:
        for(u8 i = 0; i < FAN_NUM; i++)
        {
          mustStoreCmd("%s S0\n", fanCmd[i]);
        }
        mustStoreCmd("M81\n");
        infoMenu.cur--;
        popupReminder(textSelect(LABEL_SHUT_DOWN), textSelect(LABEL_SHUTTING_DOWN));
    }
    loopProcess();
  }
}

