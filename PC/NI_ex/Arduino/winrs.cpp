#include "winrs.h"
#include "string.h"
#include <cstdio>
#include <cstring>
#include <Windows.h>

std::set<int> WinRS::portSet;

WinRS::WinRS(int addr, int bps, ifLine::delim delim, char *mode, bool rsFlow) throw (portDuplex)
: ifLine(addr, rsline,  delim)
{
   myAddr = addr; // デストラクタで使う
   myRSFlow = rsFlow;
   if (portSet.find(addr) == portSet.end())
   { // このポートはまだ open されていない
     init(addr, bps, mode);
     if (ifaceValid) portSet.insert(addr);
   }
   else  // 多重オープンには対応できない
     throw portDuplex(addr);

}


int WinRS::init(int addr, int bps, char* mode)
{

#if defined(_UNICODE)
   #define PRAMA_CHAR wchar_t
   #define L_Suffix(x) L##x
   #define SPRINTF std::swprintf
#else
   #define PRAMA_CHAR char
   #define L_Suffix(x) x
   #define SPRINTF std::sprintf
#endif

	PRAMA_CHAR paraBuff[256];

	SPRINTF(paraBuff, L_Suffix("\\\\.\\COM%d"), addr);
    handle = CreateFile(paraBuff,
    GENERIC_READ|GENERIC_WRITE,
    0,
    NULL,
    OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL,
    NULL);

    if (handle !=  INVALID_HANDLE_VALUE) ifaceValid = true;

    PurgeComm(handle, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
    SetupComm(handle,BUFFERSIZE,BUFFERSIZE);

    GetCommState(handle, & masterDCB);
    dcb1 = masterDCB;

    GetCommTimeouts(handle, &masterTimeOut);
    timeout1 = masterTimeOut;

    if (std::strchr("78",  mode[0]) == 0) mode[0] = '8';
    if (std::strchr("OEN", mode[1]) == 0) mode[1] = 'N';
    if (std::strchr("12",  mode[2]) == 0) mode[2] = '1';

    SPRINTF(paraBuff, L_Suffix("baud=%d parity=%c data=%c stop=%c"), bps, mode[1], mode[0], mode[2]);
    BuildCommDCB(paraBuff, &dcb1);

    if (myRSFlow)
    {
      dcb1.fRtsControl = RTS_CONTROL_HANDSHAKE;
      dcb1.fOutxCtsFlow = TRUE;
    }
    else
    {
      dcb1.fRtsControl = RTS_CONTROL_ENABLE;
      dcb1.fOutxCtsFlow = FALSE;
    }

    dcb1.fDtrControl  = DTR_CONTROL_ENABLE;
    
	dcb1.fOutxDsrFlow = FALSE;
    dcb1.fOutxCtsFlow = FALSE;
	dcb1.fDsrSensitivity = FALSE;
    dcb1.fAbortOnError = FALSE;

    timeout1.WriteTotalTimeoutConstant = 0;
    timeout1.WriteTotalTimeoutMultiplier = 0;

    timeout1.ReadIntervalTimeout = 0;
    timeout1.ReadTotalTimeoutConstant = 0;
    timeout1.ReadTotalTimeoutMultiplier = 0;

    SetCommState(handle,&dcb1);
    SetCommTimeouts(handle,&timeout1);

    return 0;
}

unsigned int WinRS::loc()
{
  ClearCommError(handle, &ErrorMask, &comstat1);
  return comstat1.cbInQue;
}

char WinRS::getc1()
{
  char rBuff;
  unsigned long len;

  ClearCommError(handle, &ErrorMask, &comstat1);

  int ic;
  for (ic = 0; ic < 100; ic++)
  {
     if (loc()) break;
     Sleep(60);
  }

  if (ic >= 100) throw ifTimeOver();

  len = 1;
  ReadFile(handle, &rBuff, len, &len, NULL);
  return rBuff;
}

ifLine::stat WinRS::listen(char *buff, unsigned int &len)
{
  char rBuff;
  int  is_ready = 0;
  unsigned int  rLen = 0;

  switch (myDelim & 7)
  {  case  crlf  : is_ready = 0; break;
     case  cr    : is_ready = 2; break; // already get lf
     case  lf    : is_ready = 1; break; // already get cr
  }

  ClearCommError(handle, &ErrorMask, &comstat1);

  while (is_ready != 3)
  {
    rBuff = getc1();
    if      ( rBuff == '\x0d') is_ready |= 1;
    else if ( rBuff == '\x0a') is_ready |= 2;
    else if ( rLen >= (len - 1) ) continue;
    else
    {  rLen ++;
       *(buff++) = rBuff;
    }
  }
  *buff = '\0';
  len = rLen + 1;
  return ifLine::normalEnd;

}



ifLine::stat WinRS::talk(char *pMess)
{
  unsigned long dLen;
  char *mess;

  dLen = strlen(pMess);
  mess = new char[dLen + 4];
  strcpy(mess, pMess);
  
  switch (myDelim & 7)
  {  case crlf : 
                 mess[dLen]     = '\x0d';
                 mess[dLen + 1] = '\x0a';
                 mess[dLen + 2] = '\0';
                 dLen += 2;
                 break;

     case cr   : mess[dLen]     = '\x0d';
                 mess[dLen + 1] = '\0';
                 dLen++;
                 break;

     case lf   : mess[dLen]     = '\x0a';
                 mess[dLen + 1] = '\0';
                 dLen++;
                 break;
  }

  WriteFile(handle, mess, dLen, &dLen, NULL);
  FlushFileBuffers(handle);
  delete [] mess;
  return ifLine::normalEnd;
}

ifLine::stat WinRS::putN(char *mess, int len)
{
  unsigned long dLen = len;
  WriteFile(handle, mess, dLen, &dLen, NULL);
  FlushFileBuffers(handle);
  return ifLine::normalEnd;
}


ifLine::stat WinRS::putc1(char c)
{
  unsigned long  dLen;
  char wBuff[16];
  wBuff[0] = c;
  wBuff[1] = '\0';
//  WriteFile(handle, wBuff, strlen(wBuff),&dLen, NULL);
  WriteFile(handle, wBuff, 1,&dLen, NULL);
  return ifLine::normalEnd;
}

WinRS::~WinRS()
{
  if (portSet.find(myAddr) != portSet.end())
  {
    portSet.erase(portSet.find(myAddr));
    PurgeComm(handle, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
    SetCommState(handle, &masterDCB);
    SetCommTimeouts(handle,&masterTimeOut);
    CloseHandle(handle);
  }
}
