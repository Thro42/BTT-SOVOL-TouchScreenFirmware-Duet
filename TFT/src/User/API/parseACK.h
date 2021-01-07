#ifndef _PARSEACK_H_
#define _PARSEACK_H_

#include "stdint.h"
#include "Configuration.h"

static const char errormagic[] = "Error:";
static const char warningmagic[] = "Warning:";
static const char echomagic[] = "echo:";
static const char unknowmagic[] = "Unknown command:";

#define ACK_MAX_SIZE 2048
extern int MODEselect;

void setCurrentAckSrc(uint8_t src);
void parseACK(void);
void parseRcvGcode(void);
bool dmaL1NotEmpty(uint8_t port);

#endif
