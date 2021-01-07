#include "Print.h"
#include "includes.h"

  LISTITEMS printListItems = {
  // title
  LABEL_BACKGROUND,
  // icon                 ItemType      Item Title        item value text(only for custom value)
  {
    {ICONCHAR_BACKGROUND, LIST_LABEL, LABEL_BACKGROUND, LABEL_BACKGROUND},
    {ICONCHAR_BACKGROUND, LIST_LABEL, LABEL_BACKGROUND, LABEL_BACKGROUND},
    {ICONCHAR_BACKGROUND, LIST_LABEL, LABEL_BACKGROUND, LABEL_BACKGROUND},
    {ICONCHAR_BACKGROUND, LIST_LABEL, LABEL_BACKGROUND, LABEL_BACKGROUND},
    {ICONCHAR_BACKGROUND, LIST_LABEL, LABEL_BACKGROUND, LABEL_BACKGROUND},
    {ICONCHAR_BACKGROUND, LIST_LABEL, LABEL_BACKGROUND, LABEL_BACKGROUND},
    {ICONCHAR_BACKGROUND, LIST_LABEL, LABEL_BACKGROUND, LABEL_BACKGROUND},
    {ICONCHAR_BACK,       LIST_LABEL, LABEL_BACKGROUND, LABEL_BACKGROUND},}
  };

// File list number per page
#define NUM_PER_PAGE	5
SCROLL   titleScroll;
const GUI_RECT titleRect={10, (TITLE_END_Y - BYTE_HEIGHT) / 2, LCD_WIDTH-10, (TITLE_END_Y - BYTE_HEIGHT) / 2 + BYTE_HEIGHT};

SCROLL   gcodeScroll;
bool icon_pre = false;

const GUI_RECT gcodeRect[NUM_PER_PAGE] = {
  {BYTE_WIDTH/2+0*SPACE_X_PER_ICON,  1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y+(SPACE_Y-BYTE_HEIGHT)/2,
  1*SPACE_X_PER_ICON-BYTE_WIDTH/2,  1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y+(SPACE_Y-BYTE_HEIGHT)/2+BYTE_HEIGHT},

  {BYTE_WIDTH/2+1*SPACE_X_PER_ICON,  1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y+(SPACE_Y-BYTE_HEIGHT)/2,
  2*SPACE_X_PER_ICON-BYTE_WIDTH/2,  1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y+(SPACE_Y-BYTE_HEIGHT)/2+BYTE_HEIGHT},

  {BYTE_WIDTH/2+2*SPACE_X_PER_ICON,  1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y+(SPACE_Y-BYTE_HEIGHT)/2,
  3*SPACE_X_PER_ICON-BYTE_WIDTH/2,  1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y+(SPACE_Y-BYTE_HEIGHT)/2+BYTE_HEIGHT},

  {BYTE_WIDTH/2+3*SPACE_X_PER_ICON,  1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y+(SPACE_Y-BYTE_HEIGHT)/2,
  4*SPACE_X_PER_ICON-BYTE_WIDTH/2,  1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y+(SPACE_Y-BYTE_HEIGHT)/2+BYTE_HEIGHT},

  {BYTE_WIDTH/2+0*SPACE_X_PER_ICON,  2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y+(SPACE_Y-BYTE_HEIGHT)/2,
  1*SPACE_X_PER_ICON-BYTE_WIDTH/2,  2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y+(SPACE_Y-BYTE_HEIGHT)/2+BYTE_HEIGHT},
};

void scrollFileNameCreate(u8 i)
{
  u8 num=infoFile.cur_page * NUM_PER_PAGE + i;

  if(infoFile.F_num + infoFile.f_num==0)
  {
    memset(&gcodeScroll,0,sizeof(SCROLL));
    return;
  }
  if(num<infoFile.F_num)
  {
    Scroll_CreatePara(&gcodeScroll, (u8* )infoFile.folder[num],&gcodeRect[i]);
  }
  else if(num<infoFile.F_num+infoFile.f_num)
  {
    Scroll_CreatePara(&gcodeScroll, (u8* )(infoFile.Longfile[num-infoFile.F_num]), &gcodeRect[i]);
  }
}

void normalNameDisp(const GUI_RECT *rect, u8 *name)
{
  if(name == NULL) return;

  GUI_ClearPrect(rect);
  GUI_SetRange(rect->x0, rect->y0, rect->x1, rect->y1);
  GUI_DispStringInPrect(rect, name);
  GUI_CancelRange();
}

bool get_Pre_Icon(void)
{
  return icon_pre;
}




void gocdeListDraw(void)
{
  u8 i = 0;

  Scroll_CreatePara(&titleScroll, (u8 *)infoFile.title, &titleRect);
  printListItems.title.address = (u8 *)infoFile.title;
  GUI_SetBkColor(TITLE_BACKGROUND_COLOR);
  GUI_ClearRect(0, 0, LCD_WIDTH, TITLE_END_Y);
  GUI_SetBkColor(BACKGROUND_COLOR);

  for (i = 0; (i + infoFile.cur_page * NUM_PER_PAGE < infoFile.F_num) && (i < NUM_PER_PAGE); i++) // folder
  {
    printListItems.items[i].icon = ICONCHAR_FOLDER;
    setDynamicLabel(i, infoFile.folder[i + infoFile.cur_page * NUM_PER_PAGE]);
    printListItems.items[i].titlelabel.index = LABEL_DYNAMIC;
    menuDrawListItem(&printListItems.items[i], i);
  }
  for (; (i + infoFile.cur_page * NUM_PER_PAGE < infoFile.f_num + infoFile.F_num) && (i < NUM_PER_PAGE); i++) // gcode file
  {
    printListItems.items[i].icon = ICONCHAR_FILE;
    setDynamicLabel(i, (infoFile.source == BOARD_SD) ? infoFile.Longfile[i + infoFile.cur_page * NUM_PER_PAGE - infoFile.F_num] : infoFile.file[i + infoFile.cur_page * NUM_PER_PAGE - infoFile.F_num]);
    printListItems.items[i].titlelabel.index = LABEL_DYNAMIC;
    menuDrawListItem(&printListItems.items[i], i);

  }

  for (; (i < NUM_PER_PAGE); i++) //background
  {
    printListItems.items[i].icon = ICONCHAR_BACKGROUND;
    printListItems.items[i].titlelabel.index = LABEL_BACKGROUND;
    menuDrawListItem(&printListItems.items[i], i);
  }
    // set page up down button according to page count and current page
    int t_pagenum = (infoFile.F_num+infoFile.f_num+(LISTITEM_PER_PAGE-1))/LISTITEM_PER_PAGE;
    if ((infoFile.F_num+infoFile.f_num) <= LISTITEM_PER_PAGE)
    {
      printListItems.items[5].icon = ICONCHAR_BACKGROUND;
      printListItems.items[6].icon = ICONCHAR_BACKGROUND;
    }
    else
    {
      if(infoFile.cur_page == 0){
        printListItems.items[5].icon = ICONCHAR_BACKGROUND;
        printListItems.items[6].icon = ICONCHAR_PAGEDOWN;
      }
      else if(infoFile.cur_page == (t_pagenum-1)){
        printListItems.items[5].icon = ICONCHAR_PAGEUP;
        printListItems.items[6].icon = ICONCHAR_BACKGROUND;
      }
      else
      {
        printListItems.items[5].icon = ICONCHAR_PAGEUP;
        printListItems.items[6].icon = ICONCHAR_PAGEDOWN;
      }
    }
    menuDrawListItem(&printListItems.items[5],5);
    menuDrawListItem(&printListItems.items[6],6);
}


const int16_t labelVolumeError[] = {LABEL_READ_TFTSD_ERROR, LABEL_READ_U_DISK_ERROR, LABEL_READ_ONBOARDSD_ERROR};

void menuPrintFromSource(void)
{
  KEY_VALUES key_num = KEY_IDLE;

  u8 update=0;

  GUI_Clear(BACKGROUND_COLOR);
  GUI_DispStringInRect(0, 0, LCD_WIDTH, LCD_HEIGHT, textSelect(LABEL_LOADING));

  if (scanPrintFiles() == true)
  {
    menuDrawListPage(&printListItems);
    gocdeListDraw();
  }
  else
  {
    GUI_DispStringInRect(0, 0, LCD_WIDTH, LCD_HEIGHT, textSelect(labelVolumeError[infoFile.source]));
    Delay_ms(1000);
    infoMenu.cur--;
  }

  while(infoMenu.menu[infoMenu.cur] == menuPrintFromSource)
  {
    GUI_SetBkColor(TITLE_BACKGROUND_COLOR);
    Scroll_DispString(&titleScroll, LEFT);    //
    GUI_SetBkColor(BACKGROUND_COLOR);

    key_num = menuKeyGetValue();

    switch(key_num)
    {
      case KEY_ICON_5:
        if(infoFile.cur_page > 0)
        {
          infoFile.cur_page--;
          update=1;
        }
        break;

      case KEY_ICON_6:
        if(infoFile.cur_page+1 < (infoFile.F_num+infoFile.f_num+(NUM_PER_PAGE-1))/NUM_PER_PAGE)
        {
          infoFile.cur_page++;
          update=1;
        }
        break;

      case KEY_ICON_7:
        infoFile.cur_page = 0;
        if(IsRootDir() == true)
        {
          clearInfoFile();
          infoMenu.cur--;
          break;
        }
        else
        {
          ExitDir();
          scanPrintFiles();
          update = 1;
        }
        break;

      case KEY_IDLE:
        break;

      default:
        if(key_num <= KEY_ICON_4)
        {
          u16 start = infoFile.cur_page * NUM_PER_PAGE;
          if(key_num + start < infoFile.F_num)						//folder
          {
            if(EnterDir(infoFile.folder[key_num + start]) == false)  break;
            scanPrintFiles();
            update=1;
            infoFile.cur_page=0;
          }
          else if(key_num+start < infoFile.F_num+infoFile.f_num)	//gcode
          {
            if(infoHost.connected !=true) break;
            if(EnterDir(infoFile.file[key_num + start - infoFile.F_num]) == false) break;

            infoMenu.menu[++infoMenu.cur] = menuStartPrinting;
          }
        }

        break;
    }

    if(update)
    {
      update=0;

      gocdeListDraw();
    }

    #ifdef SD_CD_PIN
    if(isVolumeExist(infoFile.source) != true)
    {
      resetInfoFile();
      infoMenu.cur--;
    }
    #endif
    loopProcess();
  }
}

MENUITEMS sourceSelItems = {
//  title
LABEL_PRINT,
// icon                       label
 {
  {ICON_ONBOARD_SD,           LABEL_ONBOARDSD},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACK,                 LABEL_BACK},}
};
