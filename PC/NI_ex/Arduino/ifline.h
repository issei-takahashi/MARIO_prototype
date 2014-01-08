// ifLine .. line 単位のコミニュケーションを行う通信回線を定義する
// address, type, delimter を基本的な定義とする

#if !defined(_BCL_IfLineHeadder)
#define      _BCL_IfLineHeadder

class ifLineError  // ifLine 配下の例外
{
};

class portDuplex : public ifLineError // 同一のポートを２重に開いたときに投げられる例外
{
    private:
       int portNo;

    public:
       portDuplex() {portNo = -1; }
       portDuplex(int port) : portNo(port) {}
       int getPortNo() { return portNo; }
};

class ifLine
{
	public:
	  enum type   {gpib, rsline, scsi};
	  enum stat   {error = 255, cannotUse = 254,
	               normalEnd = 0, 
	               timeOver = 1, illegalCommand, messageFull};
	  enum delim  {unknown = 0, crlf = 3, cr = 1, lf = 2, eoi = 16};

	protected:
	  int     hWait;  // ハードウエアウエイトのための out 回数
	  int     myAddr;              // 多回線をサポートする
	  type    myType;              // 回線タイプを定義する
	  delim   myDelim;             // デリミタ定義

      bool    ifaceValid;          // 自分自身の取得に成功したとき ture isValid() と void *() が参照する

	public:
	  enum spMode {fast, middle, slow};

	  void setSpeed(spMode mod)
	  {  hWait = mod; }

      virtual ~ifLine()
      {  }
      
      
	  ifLine(int addr, type setIfType, delim sdelim)
	  {  myAddr  = addr;
		 myType  = setIfType;
		 myDelim = sdelim;
		 setSpeed(slow);
         ifaceValid = false;
	  }

	  type ifType()      { return myType; }

	  virtual stat       setDelim(delim sdelim)        = 0;
	  virtual stat       talk(char *message)           = 0;
	  virtual stat       listen(char *message, unsigned int &maxChar) = 0;
	  virtual unsigned long getStat(int mode)          {return 0; }
	  virtual double     getIfVer()    {return 0; }
	  virtual char      *getIfName()   {return "ifLine presents: B&C Lab.";}

      bool isValid() { return ifaceValid; }
      operator void *() {return (ifaceValid) ? this : 0; }

};

#endif



