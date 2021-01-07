#include "gcode.h"
#include "includes.h"

REQUEST_COMMAND_INFO requestCommandInfo;
bool WaitingGcodeResponse = 0;
u16 find_index;

static bool
find_part(const char *str)
{
  // u16 i;
  u16 y;
  for (find_index = 0; find_index < CMD_MAX_REV && requestCommandInfo.cmd_rev_buf[find_index] != 0; find_index++)
  {
    for (y = 0; str[y] != 0 && requestCommandInfo.cmd_rev_buf[find_index + 1] != 0 && requestCommandInfo.cmd_rev_buf[find_index + 1] == str[y]; y++)
    {
      /* code */
    }
    if (str[y] == 0)
    {
      find_index += y;
      return true;
    }
    /* code */
  }
  return false;
}

static void
resetRequestCommandInfo(void)
{
  requestCommandInfo.cmd_rev_buf = malloc(CMD_MAX_REV);
  while (!requestCommandInfo.cmd_rev_buf)
    ; // malloc failed
  memset(requestCommandInfo.cmd_rev_buf, 0, CMD_MAX_REV);
  requestCommandInfo.inWaitResponse = true;
  requestCommandInfo.inResponse = false;
  requestCommandInfo.done = false;
  requestCommandInfo.inError = false;
}

bool RequestCommandInfoIsRunning(void)
{
  return WaitingGcodeResponse; //i try to use requestCommandInfo.done but does not work as expected ...
}

void clearRequestCommandInfo(void)
{
  free(requestCommandInfo.cmd_rev_buf);
}

/*
  * SENDING:M20
  * Begin file list
  * PI3MK2~1.GCO 11081207
  * /YEST~1/TEST2/PI3MK2~1.GCO 11081207
  * /YEST~1/TEST2/PI3MK2~3.GCO 11081207
  * /YEST~1/TEST2/PI3MK2~2.GCO 11081207
  * /YEST~1/TEST2/PI3MK2~4.GCO 11081207
  * /YEST~1/TEST2/PI3MK2~5.GCO 11081207
  * /YEST~1/PI3MK2~1.GCO 11081207
  * /YEST~1/PI3MK2~3.GCO 11081207
  * /YEST~1/PI3MK2~2.GCO 11081207
  * End file list
*/
char *request_M20(char *nextdir)
{
  // uint32_t timeout = ((uint32_t)0x000FFFFF);
  infoHost.pauseGantry = true;
  if (nextdir == NULL)
  {
    strcpy(requestCommandInfo.command, "M20 S2\n\n");
  }
  else
  {
    sprintf(requestCommandInfo.command, "M20 S2 P\"/gcodes/%s\"\n\n", nextdir);
  }
  send_and_wait_M20();
  infoHost.pauseGantry = false;
  return requestCommandInfo.cmd_rev_buf;
}

/*
 * M33 retrieve long filename from short file name
 *   M33 miscel~1/armchair/armcha~1.gco
 * Output:
 *   /Miscellaneous/Armchair/Armchair.gcode
*/
char *request_M33(char *filename)
{
  sprintf(requestCommandInfo.command, "M33 %s\n", filename);
  strcpy(requestCommandInfo.startMagic, "/"); //a character that is in the line to be treated
  strcpy(requestCommandInfo.stopMagic, "ok");
  strcpy(requestCommandInfo.errorMagic, "Cannot open subdir");
  resetRequestCommandInfo();
  mustStoreCmd(requestCommandInfo.command);
  // Wait for response
  WaitingGcodeResponse = 1;
  while (!requestCommandInfo.done)
  {
    loopProcess();
  }
  WaitingGcodeResponse = 0;
  //clearRequestCommandInfo(); //shall be call after copying the buffer ...
  return requestCommandInfo.cmd_rev_buf;
}

/**
 * Select the file to print
 *
 * >>> m23 YEST~1/TEST2/PI3MK2~5.GCO
 * SENDING:M23 YEST~1/TEST2/PI3MK2~5.GCO
 * echo:Now fresh file: YEST~1/TEST2/PI3MK2~5.GCO
 * File opened: PI3MK2~5.GCO Size: 11081207
 * File selected
 * RRF3 не отдаёт в ответ ничего
 **/
bool request_M23(char *filename)
{
  char command[100];
  sprintf(command, "M23 %s\n", filename);
  mustStoreCmd(command);

  return true;
}

/**
 * Start o resume print
 **/
bool request_M24(int pos)
{
  if (pos == 0)
  {
    mustStoreCmd("M24\n");
  }
  else
  {
    char command[100];
    sprintf(command, "M24 S%d\n", pos);
    mustStoreCmd(command);
  }
  return true;
}

/**
 * Abort print
 **/
bool request_M524(void)
{
  request_M25();
  mustStoreCmd("M0 H1\n");
  return true;
}
/**
 * Pause print
 **/
bool request_M25(void)
{
  mustStoreCmd("M25\n");
  return true;
}
/* help function for cleanup serial port*/
void waitPortReady(void)
{
  TCHAR command[30];
  sprintf(command, "Wait for Port");
  uint32_t timeout = ((uint32_t)0x000FFFFF);
  while ((requestCommandInfo.inWaitResponse || requestCommandInfo.inResponse || infoHost.wait || dmaL1NotEmpty(SERIAL_PORT)) && (timeout > 0x00))
  {
    GUI_DispString(5, 10, (u8 *)"Wait for Port");
    if (dmaL1NotEmpty(SERIAL_PORT) && !infoHost.rx_ok[SERIAL_PORT])
      infoHost.rx_ok[SERIAL_PORT] = true;
    loopBackEnd();
    timeout--;
  }
}
char *request_M20_macros(char *nextdir)
{
  // set pause Flag
  infoHost.pauseGantry = true;
  // waitPortReady();
  clearRequestCommandInfo();
  if ((nextdir == NULL) || strchr(nextdir, '/') == NULL)
  {
    strcpy(requestCommandInfo.command, "M20 S2 P\"/macros\"\n");
  }
  else
  {
    sprintf(requestCommandInfo.command, "M20 S2 P\"/macros/\"%s\n\n", nextdir);
  }
  // Send GCode and wait for responce
  send_and_wait_M20();
  // reset pause Flag
  infoHost.pauseGantry = false;
  GUI_Clear(BACKGROUND_COLOR);
  return requestCommandInfo.cmd_rev_buf;
}

void send_and_wait_M20(void)
{
  uint32_t timeout = ((uint32_t)0x000FFFFF);
  uint32_t waitloops = ((uint32_t)0x00000006);

  strcpy(requestCommandInfo.startMagic, "{");
  strcpy(requestCommandInfo.stopMagic, "}");
  strcpy(requestCommandInfo.errorMagic, "Error:");

  resetRequestCommandInfo();
  mustStoreCmd(requestCommandInfo.command);
  while ((strstr(requestCommandInfo.cmd_rev_buf, "dir") == NULL) && (waitloops > 0x00)) //(!find_part("dir"))
  {
    waitloops--;
    timeout = ((uint32_t)0x0000FFFF);
    WaitingGcodeResponse = 1;
    while ((!requestCommandInfo.done) && (timeout > 0x00))
    {
      loopBackEnd();
      timeout--;
    }
    WaitingGcodeResponse = 0;
    if (timeout <= 0x00)
    {
      uint16_t wIndex = (dmaL1Data[SERIAL_PORT].wIndex == 0) ? DMA_TRANS_LEN : dmaL1Data[SERIAL_PORT].wIndex;
      if (dmaL1Data[SERIAL_PORT].cache[wIndex - 1] == '}') // \n fehlt
      {
        BUZZER_PLAY(sound_notify); // for DEBUG
        dmaL1Data[SERIAL_PORT].cache[wIndex] = '\n';
        dmaL1Data[SERIAL_PORT].cache[wIndex + 1] = 0;
        dmaL1Data[SERIAL_PORT].wIndex++;
        infoHost.rx_ok[SERIAL_PORT] = true;
      }
    }
    if (dmaL1NotEmpty(SERIAL_PORT) && !infoHost.rx_ok[SERIAL_PORT])
    {
      infoHost.rx_ok[SERIAL_PORT] = true;
    }
    if (strstr(requestCommandInfo.cmd_rev_buf, "dir") == NULL)
    {
      clearRequestCommandInfo();
      resetRequestCommandInfo();
      mustStoreCmd("\n");
    }
  }
  return; //  requestCommandInfo.cmd_rev_buf;
}

bool request_M98(char *filename)
{
  sprintf(requestCommandInfo.command, "M98 P/macros/%s\n", filename);
  strcpy(requestCommandInfo.startMagic, "{"); //a character that is in the line to be treated
  strcpy(requestCommandInfo.stopMagic, "}");
  strcpy(requestCommandInfo.errorMagic, "Warning:");
  resetRequestCommandInfo();
  mustStoreCmd(requestCommandInfo.command);
  // Wait for response
  WaitingGcodeResponse = 1;
  while (!requestCommandInfo.done)
  {
    loopProcess();
  }
  WaitingGcodeResponse = 0;
  return true;
}