/**********************************************************************\
 Library     :  Utils / Common
 Filename    :  str.h
 Purpose     :  Class for manipulating with ASCII text strings.
 Author      :  (c) RedSoft, Sergey Krasnitsky
 Created     :  1.02.2005
\**********************************************************************/

#ifndef _STR_H
#define _STR_H

#include "autostr.h"


#define STRB_USEC_THRESHOLD_10YEARS		315360000000000ll		// Threshold meaning to show or not the date part, to use in STRB::datetime2str


/*
 ***************************************************************************
 Base string class. It's the wrapper for all standard string manipulation 
 functions. 
 To store last 0 there is always one last char place kept by means of setting 
 MaxLen = passed maxLen-1. In the case maxLen = 0, the MaxLen turns to be the 
 max integer, meaning no actual length check will be done.
 User is responsible to keep last 0 in his strings.

 Class STRB defines 3 operators:
	1. =  copy operator
	2. += concatenation operator
	3. +  multiple concatenation operator 
	4. ==, >, < comparison operators

  - Concatenation execution is much faster than ::strncat
  - Len member access is much faster than that ::strlen
  - Operator char* returns the pointers to string
  - Operator int converts string to integer

  Time converting functions.
  All functions manipulating with time are converting the internal time 
  format (that is the Epoch time in microseconds - the time since 1.1.1970)
  to/from string representation.
 ***************************************************************************
*/
class STRB
{
  public:
	enum { 
		UINT64_MAX_LEN = 21,	// Max string size including last '\0' to represent 64b value
		STR2BYTES_MAX  = 32,	// Max supported byte array size for str2bytes() function
	};

	enum TimeResolution
	{
		TimeResolution_Minutes,
		TimeResolution_Seconds,
		TimeResolution_Millis,
		TimeResolution_Micros
	};

  public:
	static bool isalpha (char c)			{ return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c=='_'); }
	static bool isdigit (char c)			{ return (c >= '0' && c <= '9'); }
	static bool isdigit2 (char c)			{ return (c >= '0' && c <= '9') || (c=='.'); }
	static bool isspace (char c)			{ return (c == ' ' || c == '\t'); }
	static bool isspace2 (char c)			{ return (c <= ' '); /*cover all control chars and spaces*/}
	static bool isalnum (char c)			{ return isalpha(c) || isdigit(c); }
	static bool isxdigit (char c)			{ return (isdigit(c) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f')); }
	static char tolower (char c)			{ return (c >= 'A' && c <= 'Z') ? c-'A'+'a' : c; }
	static char hex2char (char h1);
	static char hex2char (char h1, char h2);
	static bool isblank  (cchar* str);
	static bool isempty  (cchar* str)		{ return (!str || !str[0]); }

	// Common itoa function fit for all integer types
	// Note: to call the standard C atoi() func use ::atoi (duplicated as STRB::atoi32)
	static int64 atoi   (cchar* str, cchar* sformat = "%lli");
	static int   atoi32 (cchar* str)		{ return ::atoi(str); }

	// Note: for atof use standard C ::atof()

	// Common itoa function fit for all integer types + supports error returning 
	static bool  atoi	(cchar* str, int64* x,  cchar* sformat = "%lld");
	static bool  atoi	(cchar* str, uint64* x, cchar* sformat = "%llu")	{ return atoi(str,(int64*)x,sformat); }

	static int   itoa	(int    val, char* str, cchar* sformat = "%d")		{ return sprintf(str,sformat,val); }
	static int   ultoa	(uint32 val, char* str, cchar* sformat = "%u")		{ return sprintf(str,sformat,val); }
	static int   lltoa	(int64  val, char* str, cchar* sformat = "%lld")	{ return sprintf(str,sformat,val); }
	static int   ulltoa (uint64 val, char* str, cchar* sformat = "%llu")	{ return sprintf(str,sformat,val); }
	static int   ftoa	(double val, char* str, cchar* sformat = "%lf")		{ return sprintf(str,sformat,val); }

	static char* int2str   (int val)										{ char* strout = autostrGet(); itoa  (val,strout); return strout; }
	static char* int2str   (int64 val)										{ char* strout = autostrGet(); lltoa (val,strout); return strout; }
	static char* int2str   (uint64 val)										{ char* strout = autostrGet(); ulltoa(val,strout); return strout; }
	static char* uint2str  (unsigned val)									{ char* strout = autostrGet(); ultoa (val,strout); return strout; }
	static char* float2str (double val)										{ char* strout = autostrGet(); ftoa	 (val,strout); return strout; }
	static char* int2str   (int val, cchar* sformat)						{ char* strout = autostrGet(); itoa  (val,strout, sformat); return strout; }


	// Convert hex byte sequence like "11:22:AA:BB:CC" or "1122AABBCC" to byte array
	// return number of read characters from input str; the output bytes are right aligned
	static int str2bytes (cchar* str, uint8 outbytes[], int outsize);

	// Convert a byte array to a string
	static int   bytes2str (uint8 bytes[], int nbytes, char* strout, bool usecolon = true);
	static char* bytes2str (uint8 bytes[], int nbytes) 						{ char* strout = autostrGet();  bytes2str(bytes,nbytes,strout);  return strout; }

	// Convert an IP-address string to uint32
	static uint32 str2ip (cchar* str);

	// Convert an IP-address string to uint32 (returns true if succeeded)
	static bool	  str2ip (cchar* str, uint32* x);

	// Convert an IP/X string to uint32 IP (returned) + *nbits set to X
	static uint32 str2ipmask (cchar* str, int* nbits);

	// Convert an uint32 IP-address to a string. Returns number of bytes in strout
	static int	  ip2str (uint32 ipaddr, char* strout);
	static char*  ip2str (uint32 ipaddr)									{ char* strout = autostrGet(); ip2str(ipaddr,strout); return strout; }

	// Same as ip2str, but added port number to produce the ip:port string
	static int    ipport2str (uint32 ipaddr, uint16 port, char* strout);
	static char*  ipport2str (uint32 ipaddr, uint16 port);					// Same as above ipport2str() but take strout from autostrGet

	// Convert an IP:PORT pair string to uint32 IP (returned) + uint16 *port. If :PORT is not present the *port == 0
	static uint32 str2ipport (cchar* str, uint16* port);

	// Convert an uint64 to a hex string. Returns number of bytes in strout
	static int    hex2str (uint64 x, char* strout)							{ return sprintf(strout,"%llX",x);  }
	static char*  hex2str (uint64 x)										{ char* strout = autostrGet();  hex2str(x,strout);  return strout; }	// Same as above hex2str() but take strout from autostrGet

	static int    hex0xstr (uint64 x, char* strout)							{ return sprintf(strout,"0x%llX",x);  }
	static char*  hex0xstr (uint64 x)										{ char* strout = autostrGet();  hex0xstr(x,strout);  return strout; }	// Same as above hex2str() but take strout from autostrGet

	static int    hex0xstr (uint32 x, char* strout)							{ return sprintf(strout,"0x%08X",x);  }									// Same as above hex2str() but works with uint32 arg
	static char*  hex0xstr (uint32 x)										{ char* strout = autostrGet();  hex0xstr(x,strout);  return strout; }	// Same as above hex2str() but take strout from autostrGet

	// Convert an int64 integer with n decimal places to string. Returns number of bytes in strout
	static int    decimal2str (int64 x, uint32 n, char* strout);
	static char*  decimal2str (int64 x, uint32 n)							{ char* strout = autostrGet();  decimal2str(x,n,strout);  return strout; }	// Same as above hex2str() but take strout from autostrGet

	// Convert an UTC Epoch time in microseconds to very common format "yy-mm-dd hh:mm:ss.microseconds"
	static int    time2str (uint64 timeusec, char* outstr);
	static char*  time2str (uint64 timeusec)								{ char* strout = autostrGet();  time2str(timeusec,strout);  return strout; }

	// Same as time2str() but convert the timeusec to Local zone
	// Note: these methods are moved to the TIMER class
	//static int    time2str_loc (uint64 timeusec, char* outstr);
	//static char*  time2str_loc (uint64 timeusec);

	// Experimental function. It's like time2str, but 1) detects small time value and don't print date if it's not needed 1) rounds/formats the output string according to requested resolution
	static int   datetime2str (int64 timeusec, char* strout, TimeResolution format = TimeResolution_Micros);
	static char* datetime2str (int64 timeusec, TimeResolution format = TimeResolution_Micros)	{ char* strout = autostrGet();  datetime2str(timeusec,strout,format);  return strout; }

	// Heuristic convert a date/time string to the Epoch time in microseconds (tries different input forms)
	static uint64 str2time (cchar* str);

	// Convert a time string (hh:mm:ss[.xxxxxx]) to the Epoch time. If the dot is present, digit amount after the dot may be 1..6. The func return value is always in microseconds.
	static uint64 str2time_timeonly (cchar* str);

	// Convert a date string (yy-mm-dd) to the Epoch time in microseconds
	static uint64 str2time_dateonly (cchar* str);

	// Convert a time in minutes to a string [hh]:mm - hh may be large
	static int   minutes2str (int64 timemin, char* strout);
	static char* minutes2str (int64 timemin)								{ char* strout = autostrGet();  minutes2str(timemin,strout);  return strout; }

	// Convert a time in seconds to a string [hh]:mm:ss - hh may be large
	static int   seconds2str (int64 timesec, char* strout);
	static char* seconds2str (int64 timesec)								{ char* strout = autostrGet();  seconds2str(timesec,strout);  return strout; }

	// Convert a time in seconds to a string hh:mm:ss - hh lies in 0..23
	static int   seconds2str_day (int64 timesec, char* strout);
	static char* seconds2str_day (int64 timesec)							{ char* strout = autostrGet();  seconds2str_day(timesec,strout);  return strout; }

	// Convert a time in milliseconds to a string ss.xxx, where ss - seconds, xxx - milliseconds
	static int	 millis2str (int64 timemsec, char* strout);
	static char* millis2str (int64 timemsec)								{ char* strout = autostrGet();  millis2str(timemsec,strout);  return strout; }

	// Convert a time in milliseconds to a string Nd hh:mm:ss.xxx
	static int	 millis2str_detailed (int64 timeusec, char* strout);
	static char* millis2str_detailed (int64 timeusec) { char* strout = autostrGet ();  millis2str_detailed (timeusec, strout);  return strout; }

	// Convert a time in microseconds to a string ss.xxxxxx, where ss - seconds, xxxxxx - microseconds
	static int	 micros2str (int64 timeusec, char* strout);
	static char* micros2str (int64 timeusec)								{ char* strout = autostrGet();  micros2str(timeusec,strout);  return strout; }

	// Convert a time in microseconds to a string Nd hh:mm:ss.xxxxxx
	static int	 micros2str_detailed (int64 timeusec, char* strout);
	static char* micros2str_detailed (int64 timeusec)						{ char* strout = autostrGet();  micros2str_detailed(timeusec,strout);  return strout; }

	// Fast and simple pattern matching function. Matches a string against a wildcard such as "*.*" or "bl?h.*". It's good for file globbing, etc
	static bool wildcmp (cchar* str, cchar *wild, bool case_sens = true);

	// Replace all entries of 1 char in a string
	static void replace (char* str, char from, char to);

	// Same as ::strcmp, but compare in case-insesitive mode
	static int  strcicmp (cchar* str1, cchar* str2);

	// Same as ::strstr, but searching in case-insesitive mode
	static char* stristr (char* str1, cchar* str2);

	// Standard strcmp() wrapper when needed to compare 2 strings for equality only. Returns true if 2 strings are equal
	static bool strequ (cchar* str1, cchar* str2)			{ return strcmp(str1,str2) == 0; }

	// Standard strcmp() wrapper when needed to compare 2 strings for equality only. Returns true if 2 strings are equal
	static bool strequ (cchar* str1, cchar* str2, int len)	{ return memcmp(str1,str2,len) == 0; }

	// Same as strequ(), but works with strings which may be terminated by 0 and \r or \n
	static bool strequ_rn (cchar* str1, cchar* str2);

  public:
	STRB ()	{}	// this empty constructor is important, otherwise STRD new overloaded operators doesn't work
	STRB (char*  buf, int maxlen = 0, int len = 0)		{ Construct(buf,maxlen,len); }
	STRB (cchar* buf, int maxlen = 0, int len = 0)		{ Construct((char*)buf,maxlen,len); }
	STRB (int maxlen /*= 0*/)							: Str(0), Spos(0), MaxLen(maxlen-1), Len(0), SeekError(false) {}
	STRB (bool autoalloc)								{ Construct(autostrGet(),AUTOSTR_MAXLEN); }

	void  Construct (char* buf, int maxlen = 0, int len = 0)   { Str = Spos = buf; MaxLen = maxlen-1; Len = len; SeekError = false; }
	void  Set       (char* buf)							{ Str = Spos = buf;  }
	void  Set       (cchar* buf)						{ Str = Spos = (char*) buf;  }
	void  Set       (int len)							{ Str = Spos = &Str[len];  }
	void  SetLen    (int len)							{ Len = len; }
	void  IncLen    (int len)							{ Len += len; }
	void  SetChar   (int i, char ch)					{ Str[i] = ch; }
	void  SetToSeek ()									{ Str = Spos; Len=0; }
	void  SetToEnd  ()									{ Str = Spos = &Str[Len]; Len=0; }
	char* GetBuffer  ()									{ return Str; }
	int   GetMaxLen ()									{ return MaxLen; }
	void  Truncate  (int len = 0)						{ Str[Len=len] = '\0'; }
	char* StripQuotes ()								{ if (Str[0]=='"' &&  Str[Len-1]=='"') { Str[--Len]='\0'; --Len; --MaxLen; ++Str; } return Str; }
	bool  IsBlank  ()									{ return isblank(Str); }
	bool  IsEmpty  ()									{ return isempty(Str); }

  public:
	operator void* ()    const							{ return Str; }
	operator char* ()    const							{ return Str; }
	operator cchar* ()   const							{ return (cchar*)Str; }
	operator uint8* ()   const							{ return (uint8*)Str; }
	operator int ()      const							{ return ::atoi(Str); }

	char& operator[] (int i)							{ return Str[i]; }

	bool   Valid()										{ return (Str != 0); }

	void   Clear()										{ Str[Len=0] = '\0'; }

	int    Copy (cchar* s);
	int    Copy (cchar* s, int size);
	int    Copy (cchar* s, char terminator, char terminator2 = '\0') { Len = 0; return Add(s,terminator,terminator2); };

	int    Add  (int count, char ch = ' ');
	int    Add  (cchar* s);
	int    Add  (cchar* s, int size);
	int    Add  (cchar* s, char terminator, char terminator2 = '\0');

	void   JustifyLeft   (unsigned newlen, char fillchar = ' ', char truncate = false);
	void   JustifyRight  (unsigned newlen, char fillchar = ' ', char truncate = false);
	void   JustifyCenter (unsigned newlen, char fillchar = ' ', char truncate = false)		{ Add((newlen-Len)/2,fillchar); JustifyRight(newlen,fillchar,truncate); }

	int    CopyEx(cchar* s, int size = 0) { Len = 0; return AddEx(s,size); }
	int    AddEx (cchar* s, int size = 0);												// add string s to the STRB object, and if strlen(s) < size then to supplement with ' ', added string may be terminated by '\0' or ','

	bool   Comp (cchar* s)         const		{ return s ? (Strcmp(s) == 0) : false; }
	bool   Comp (const STRB& c)    const		{ return Comp((char*)c); }

	bool   CompEx (cchar* s, int * n = 0);
#ifndef __APPLE__
	// Strange incorrect warning on MAC
	bool   CompEx (const STRB& c, int * n = 0) const { return CompEx ((cchar*)c, n); }
#endif

	bool   operator == (cchar* s)		const	{ return Comp(s);  }
	bool   operator != (cchar* s)		const	{ return !Comp(s); }

	bool   operator == (const STRB& c)  const	{ return Comp((char*)c);  }
	bool   operator != (const STRB& c)  const	{ return !Comp((char*)c); }

	bool   operator <  (const STRB& c)  const	{ return Strcmp((char*)c) < 0;  }
	bool   operator >  (const STRB& c)  const	{ return Strcmp((char*)c) > 0;  }

	STRB&  operator =  (cchar* s)				{ Copy(s); return *this; }
	STRB&  operator += (cchar* s)				{ Add(s);  return *this; }
	STRB&  operator +  (cchar* s)				{ return operator += (s); }
		   
	STRB&  operator =  (const STRB& c)			{ return operator =  ((char*)c); }
	STRB&  operator += (const STRB& c)			{ return operator += ((char*)c); }
	STRB&  operator +  (const STRB& c)			{ return operator += (c); }
		   
	STRB&  operator =  (char ch);
	STRB&  operator += (char ch);
	STRB&  operator +  (char ch)				{ return operator += (ch);  }
		   
	STRB&  operator =  (int i);
	STRB&  operator += (int i);
	STRB&  operator +  (int i)					{ return operator += (i); }
		   
	STRB&  operator =  (unsigned i);
	STRB&  operator += (unsigned i);
	STRB&  operator +  (unsigned i)				{ return operator += (i); }
		   
	STRB&  operator =  (uint64 i);
	STRB&  operator += (uint64 i);
	STRB&  operator +  (uint64 i)				{ return operator += (i); }
		   
	STRB&  operator =  (int64 i);
	STRB&  operator += (int64 i);
	STRB&  operator +  (int64 i)				{ return operator += (i); }

	STRB&  operator =  (double i);
	STRB&  operator += (double i);
	STRB&  operator +  (double i)				{ return operator += (i); }

	STRB&  operator =  (const wchar_t* s);

	// Cut off i last chars
	STRB& operator -= (int i);

	int   Length()    const						{ return Len; }
	char* End ()      const						{ return &Str[Len]; }
	char  Lastchr()   const						{ return  Str[Len-1]; }


	//********************************************************************
	// Init search position
	void  SeekStart		()						{ Spos = Str; }
	void  SeekStart		(int abspos)			{ Spos = &Str[abspos]; }

	//********************************************************************
	// Standard string functions wrappers
	int   Strlen		()						{ return (Len = (unsigned) strlen(Str)); }
	STRB& Strcpy		(cchar* s)				{ strcpy(Str,s);  return *this; }
	STRB& Strcat		(cchar* s)				{ strcat(Str,s);  return *this; }
	char* Strstr		(cchar* s)				{ return strstr(Str,s);  }
	int   Strcmp		(cchar* s)        const { return strcmp(Str,s); }
	int   Strncmp		(cchar* s, int n) const { return strncmp(Str,s,n); }
	char* Strchr		(char ch)				{ return strchr(Str,ch); }
	char* Strrchr		(char ch)				{ return strrchr(Str,ch); }
	int   Vsprintf		(cchar* sformat, va_list arglist);
	int	  AddSprintf	(cchar* sformat, ...);		// do vsprintf, update Len, and concatenate the new string to the current, return number of added chars
	STRB& Print			(cchar* sformat, ...);		// Format, update Len and return reference to myself
	char* Strlwr		();
	void  Replace		(char from, char to)	{ replace(Str,from,to); }
	char* Stristr		(char* str)				{ return stristr(Str,str); }

	//********************************************************************
	// Get seek/scan positions
	char* GetSeekPos	() const				{ return Spos; }
	char  GetSeekChar	() const				{ return *Spos; }
	char  GetSeekChar	(int offset) const		{ return *(Spos+offset); }
	int   GetSeekOffset	() const				{ return (int) (Spos - Str); }
	void  SetSeekPos	(char *spos)			{ Spos = spos; }
	void  SetSeekPosToStart()					{ Spos = Str; }

	//********************************************************************
	// Skip methods skip some characters from the current position 
	char* Skip        (int numch = 1)			{ return Spos += numch; }
	char* SkipChars   (char ch)					{ while(*Spos==ch) ++Spos; return Spos;}
	char* SkipSpaces  ()						{ return SkipChars(' '); }

	//********************************************************************
	// Seek methods look for characters from the current position. Returns Spos that points ON NEXT CHAR
	char  SeekChar    ()						{ return (*Spos++); }
	char* SeekChar    (char ch)					{ return (*Spos==ch) ? ++Spos : 0; }
	char* SeekChar    (char ch1, char ch2)		{ return (*Spos==ch1 || *Spos==ch2) ? ++Spos : 0; }
	char* SeekStr     (cchar* s)				{ int l; return (memcmp(Spos,s,l=(int)strlen(s))==0)    ? Spos+=l : 0; }
	char* SeekStr     (STRB& s)					{ int l; return (memcmp(Spos,(void*)s,l=s.Length())==0) ? Spos+=l : 0; }
	char* SeekStr     (cchar* s, int slen)		{ return (memcmp(Spos,s,slen)==0) ? Spos+=slen : 0; }
	int   SeekInt     (cchar* format = "%d");
	int64 SeekInt64   (cchar* format = "%lld");
	int   SeekIp	  ();

	//********************************************************************
	// Scan methods scan forward for characters from the current position. Returns Spos that points ON SCANNING CHAR (except ScanCharSet0)
	char* ScanChar    (char ch)					{ char* s1 = strchr(Spos,ch);    return (s1) ? Spos=s1 : 0; }
	char* ScanCharSet0(char ch)					{ char* s1 = strchr(Spos,ch);    return (s1) ? *s1='\0',Spos=s1+1 : 0; }
	char* ScanCharSet0(char ch1, char ch2)		{ while (*Spos) { if (*Spos==ch1 || *Spos==ch2)  return *Spos='\0',Spos++;  Spos++;} return 0; }
	char* ScanCharr   (char ch)					{ char* s1 = strrchr(Spos,ch);   return (s1) ? Spos=s1 : 0; }
	char* ScanCharNext(char ch)					{ char* s1 = strchr(Spos+1,ch);  return (s1) ? Spos=s1 : 0; }
	char* ScanCharrNext(char ch)				{ char* s1 = strrchr(Spos+1,ch); return (s1) ? Spos=s1 : 0; }
	char* ScanStr     (cchar* s)				{ char* s1 = strstr(Spos,s);     return (s1) ? Spos=s1 : 0; }
	char* ScanStrNext (cchar* s)				{ char* s1 = strstr(Spos+1,s);   return (s1) ? Spos=s1 : 0; }
	char* ScanStrNextSkip (cchar* s);
	char* ScanStrSkip (cchar* s);
	char* ScanStrSkip (STRB& s);

	//********************************************************************
	// Standard Basic-style string functions
	int	  Index	(cchar* str, int pos = 0) 					{ Spos = Str + pos; char*s = ScanStr(str); if (!s) return -1; return int(s-Str); }
	int	  IndexNoCaseSensitivity (cchar* str, int pos = 0);
	char* Mid	(int pos, int n = -1);

	//********************************************************************
	// Other useful methods
	void  PrnTime (unsigned seconds);
	int   GetTime ();
	int   PrnSetEnd   (cchar* sformat, ...);
	int   CopySetEnd  (cchar* s);
	int   CopySetEnd  (cchar* s, int size);
	void  TruncateBgn (unsigned len);
	void  TruncateEnd (unsigned len);
	char* ToLower ();
	void  TrimEndSpaces ();						// Trim end-spaces and decrease Len
	char* TrimStartEndSpaces ();				// Call TrimEndSpaces() and then simply find 1st non-space char and return the pointer

	//********************************************************************
	// Streaming operators simulating byte-stream. Note: Len <= MaxLen is not checked, very optimal
	void operator << (char  ch)					{ Str[Len++] = ch; }
	void operator << (uint8 ch)					{ Str[Len++] = ch; }
	void operator << (int16  i)					{ *((int16*) (Str+Len)) = i; Len+=2; }
	void operator << (uint16 i)					{ *((uint16*)(Str+Len)) = i; Len+=2; }
	void operator << (int32  i)					{ *((int32*) (Str+Len)) = i; Len+=4; }
	void operator << (uint32 i)					{ *((uint32*)(Str+Len)) = i; Len+=4; }
	void operator << (int64  i)					{ *((int64*) (Str+Len)) = i; Len+=8; }
	void operator << (uint64 i)					{ *((uint64*)(Str+Len)) = i; Len+=8; }
	void operator << (STRB&  src)				{ memcpy(Str+Len, src.Str, src.Len);  Len+=src.Len; }

  public:
	bool  IsSizeExceeded (int size)				{ return (unsigned(size) >= MaxLen); }
	bool  IsLenExceeded ()						{ return (Len >= MaxLen);			 }
	bool  IsLenExceeded (int addlen)			{ return ((Len+addlen)>= MaxLen);	 }
	
  protected:
	char*		Str;			// Basic String pointer set in Construct. It should not be changed during the object lifetime (use Spos for this). The Str string should be always 0-terminated. For this purpose the preallocated buffer always contains 1 byte spare not accounted in MaxLen.
	char*		Spos;			// Pointer to a current position inside [Str..Str+Len]. Used by Seek/Scan/other methods
	unsigned	MaxLen;			// Maximum possible string length, it cannot be 0. In may be -1 casted to unsigned to choose max number in order accept any lengths
	unsigned	Len;			// Current Str length; when working with standard operators it is always updated. Strlen method forces its recalculation by means of ::strlen() call

  public:
	bool		SeekError;
};


/*
 **************************************************************************
 STRRAW class behaves exactly like STRB except the string length is 
 calculated in the constructor.
 **************************************************************************
*/
class STRRAW : public STRB
{
  public:
	STRRAW (char* buf): STRB(buf)  { Strlen(); MaxLen = Len; }
};


/*
 **************************************************************************
 STR object allocated from autostr buffer.
 **************************************************************************
*/
class AUTOSTR : public STRB
{
  public:
	AUTOSTR () : STRB(true)  							{ Str[0] = '\0'; }
	AUTOSTR (const AUTOSTR& initstring) : STRB(true)  	{ STRB::operator = (initstring); }
	AUTOSTR (const STRB&	initstring) : STRB(true)  	{ STRB::operator = (initstring); }
	AUTOSTR (cchar*   		initstring) : STRB(true)  	{ STRB::operator = (initstring); }
};


/*
 **************************************************************************
 String template derived from the string base class. Contains the string
 buffer of user-defined size.
 **************************************************************************
*/
template <unsigned MaxSize> class STR : public STRB
{
  public:
	enum { SIZE = MaxSize };

	STR ()				: STRB(StrBuf,MaxSize)		{ StrBuf[MaxSize] = '\0'; Clear(); }
	STR (char last_char): STRB(StrBuf,MaxSize-1)	{ StrBuf[MaxSize] = '\0'; StrBuf[MaxSize-1] = last_char; Clear(); }		// e.g. last_char = '\n'
	STR (cchar* sinit)	: STRB(StrBuf,MaxSize)		{ StrBuf[MaxSize] = '\0'; STRB::operator= (sinit); }
	STR (STR&  sinit)	: STRB(StrBuf,MaxSize)		{ StrBuf[MaxSize] = '\0'; STRB::operator= (sinit); }

	void Construct () { STRB::Construct(StrBuf,MaxSize); }
	void SetToBegin() { Set(StrBuf); }

  public:
	using STRB::operator=;

  protected:
	char StrBuf [SIZE+1];   // Buffer for one string
};



/*
 **************************************************************************
 Dynamically allocated string. This is tricky class when actual allocation 
 of the class and adjacent string buffer are made at once. 
 The objects of this class MUST be allocated by means of new operator only.
 **************************************************************************
*/

class STRD : public STRB
{
  public:
	STRD ()	{}

	void SetToBegin() { Set((char*)(this+1)); }

  public:
	using STRB::operator=;

  public:
	enum DoNotCopy { donotcopy };

	void* operator new (size_t size, int size2)
	{
		LOW_ASSERT (size2 >= 0);
		char* ret = (char*) malloc (size + size2 + 1);
		char* sbuf = ret + size;
		((STRB*)ret)->Construct (sbuf, size2+1);
		return ret;
	}

	void* operator new (size_t size, cchar* sinit)
	{
		int size2 = (int) strlen(sinit);
		char* ret = (char*) malloc (size + size2 + 1);
		char* sbuf = ret + size;
		memcpy (sbuf, sinit, size2 + 1);
		((STRB*)ret)->Construct (sbuf, size2+1, size2);
		return ret;
	}

	void* operator new (size_t size, cchar* sinit, DoNotCopy)
	{
		int size2 = (int) strlen(sinit);
		char* ret = (char*) malloc (size);
		((STRB*)ret)->Construct ((char*)sinit, size2+1, size2);
		return ret;
	}

	void* operator new (size_t size, cchar* sinit, int size3)
	{
		LOW_ASSERT (size3 >= 0);
		int size2 = (int) strlen(sinit);
		LOW_ASSERT (size3 >= size2);
		char* ret = (char*) malloc (size + size3 + 1);
		char* sbuf = ret + size;
		memcpy (sbuf, sinit, size2 + 1);
		((STRB*)ret)->Construct (sbuf, size3+1, size2);
		return ret;
	}

	void  operator delete (void* p)								{ free(p); }
	void  operator delete (void* p, int size2)					{ free(p); }
	void  operator delete (void* p, cchar* sinit)				{ free(p); }
	void  operator delete (void* p, cchar* sinit, DoNotCopy)	{ free(p); }
	void  operator delete (void* p, cchar* sinit, int size3)	{ free(p); }

  protected:
	// disable the standard new
	void* operator new (size_t nSize);
};



/*
 **************************************************************************
 Predefined type for use as one string. 
 The STRING::SIZE will get the line buffer size. 
 **************************************************************************
*/
typedef  STR<AUTOSTR_MAXLEN>  STRING;




/**********************************************************************\
						Inline implementations
\**********************************************************************/

//static
inline char STRB::hex2char (char h1, char h2)
{
	return ((hex2char(h1) << 4) | hex2char(h2));
}


//static
inline int64 STRB::atoi (cchar* str, cchar* sformat)
{
	int64 x;
	if (sscanf(str, sformat, &x) != 1)
		return 0;
	return x;
}


//static
inline bool STRB::atoi (cchar* str, int64* x, cchar* sformat)
{
	return (sscanf(str, sformat, x) == 1);
}


// static
inline int STRB::minutes2str (int64 timemin, char* strout)
{
	int addlen = 0;
	if (timemin < 0) {
		timemin = -timemin;
		*strout++ = '-';
		addlen = 1;
	}

	int hours	= int(timemin / 60);
	int minutes = int(timemin % 60);
	return addlen + sprintf(strout, "%d:%02d", hours, minutes);
}


// static
inline int STRB::seconds2str (int64 timesec, char* strout)
{
	int addlen = 0;
	if (timesec < 0) {
		timesec = -timesec;
		*strout++ = '-';
		addlen = 1;
	}

	int x = int(timesec / 60);
	int hours	= x / 60;
	int minutes = x % 60;
	int seconds = int(timesec % 60);

	return addlen + sprintf(strout, "%d:%02d:%02d", hours, minutes, seconds);
}


// static
inline int STRB::millis2str (int64 timemsec, char* strout)
{
	if (timemsec < 0) {
		timemsec = -timemsec;
		return sprintf(strout, "-%d.%03d", int(timemsec/1000), int(timemsec%1000));
	}
	return sprintf(strout, "%d.%03d", int(timemsec/1000), int(timemsec%1000));
}


// static
inline int STRB::micros2str (int64 timeusec, char* strout)
{
	if (timeusec < 0) {
		timeusec = -timeusec;
		return sprintf(strout, "-%d.%06d", int(timeusec/1000000), int(timeusec%1000000));
	}
	return sprintf(strout, "%d.%06d", int(timeusec/1000000), int(timeusec%1000000));
}


inline int STRB::Copy (cchar* s, int size)
{
	if (IsSizeExceeded(size))
		return 0;
	memcpy (Str, s, size);
	Str [Len = size] = '\0';
	return size;
}


inline int STRB::Add (cchar* s, int size)
{
	if (IsLenExceeded(size))
		return 0;
	memcpy (Str+Len, s, size);
	Str [Len+=size] = '\0';
	return size;
}

inline STRB& STRB::operator = (cchar ch)
{
	Str[0] = ch;
	Str[1] = '\0';
	Len = 1;
	return *this;
}


inline STRB& STRB::operator += (cchar ch)
{
	Str[Len] = ch;
	Str[++Len] = '\0';
	return *this;
}


inline STRB& STRB::operator = (int i)
{
	char s[20];
	itoa (i, s);
	operator=(s);
	return *this;
}


inline STRB& STRB::operator += (int i)
{
	char s[20];
	itoa (i, s);
	operator+=(s);
	return *this;
}

inline STRB& STRB::operator = (unsigned i)
{
	char s[20];
	ultoa (i, s);
	operator=(s);
	return *this;
}


inline STRB& STRB::operator += (unsigned i)
{
	char s[20];
	ultoa (i, s);
	operator+=(s);
	return *this;
}


inline STRB& STRB::operator = (uint64 i)
{
	char s[30];
	ulltoa(i, s);
	operator=(s);
	return *this;
}


inline STRB& STRB::operator += (uint64 i)
{
	char s[30];
	ulltoa(i, s);
	operator+=(s);
	return *this;
}


inline STRB& STRB::operator = (int64 i)
{
	char s[30];
	lltoa(i, s);
	operator=(s);
	return *this;
}


inline STRB& STRB::operator += (int64 i)
{
	char s[30];
	lltoa(i, s);
	operator+=(s);
	return *this;
}


inline STRB& STRB::operator = (double i)
{
	char s[30];
	ftoa(i, s);
	operator=(s);
	return *this;
}


inline STRB& STRB::operator += (double i)
{
	char s[30];
	ftoa(i, s);
	operator+=(s);
	return *this;
}


inline STRB& STRB::operator -= (int i)
{
	if (Len >= unsigned(i))
		Str[Len-=i] = '\0';
	return *this;
}


inline void STRB::TruncateEnd (unsigned len)
{
	if (len < Len)
		Truncate (len);
}


inline void STRB::TruncateBgn (unsigned len)
{
	Str += len;
	Len -= len;
}


inline int STRB::Vsprintf (cchar* sformat, va_list arglist)
{
	int ret = vsprintf (Str, sformat, arglist);
	return (Len = ret);
}



#endif // _STR_H
