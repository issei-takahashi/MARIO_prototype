#if !defined (_BCL_RsLine_WinRS)
#define _BCL_RsLine_WinRS

#include <windows.h>
#include <winbase.h>

#include "ifline.h"
#include <set>

#define TIMEOUT_MUL 10
#define BUFFERSIZE  1024

class ifTimeOver
{ };

class WinRS : public ifLine
{
   private:
         static std::set<int> portSet;

	 int    init(int addr, int bps, char *mode);
         _DCB   dcb1;
         _DCB   masterDCB;
         _COMSTAT comstat1;
         _COMMTIMEOUTS timeout1;
         _COMMTIMEOUTS masterTimeOut;

         unsigned long ErrorMask;
         int  myAddr;
         bool myRSFlow;

   protected:
         HANDLE handle;

   public :
	  WinRS(int addr = 1, int bps = 9600, ifLine::delim delim = ifLine::crlf, char *mode = "8N1", bool rsFlow = false) throw(portDuplex);
	  // mode means  "(bit size(7 or 8), parity(E or O or N), stop (1 or 2), rsFlow = (0 or 1)

	  ifLine::stat talk(char *message);
      ifLine::stat putN(char *mess, int len);
      ifLine::stat putc1(char c);
	  ifLine::stat listen(char *message, unsigned int &len);
	  ifLine::stat setDelim(ifLine::delim delim)
          {
            myDelim = delim;
	    return ifLine::normalEnd;
	  }
	  unsigned int loc();
	  char         getc1();
	  
	 ~WinRS();

};

#endif
