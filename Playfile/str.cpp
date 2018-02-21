/**********************************************************************\
 Library     :  Utils / Common
 Filename    :  str.cpp
 Purpose     :  Class for manipulating with text strings.
 Author      :  (c) RedSoft, Sergey Krasnitsky
 Created     :  1.02.2005
 Note		 :  21.2.2018 it's short version
\**********************************************************************/

#include "def.h"
#include "str.h"


//static
char STRB::hex2char (char h1)
{
    if (isdigit(h1))
        return h1 - '0';
    else if (h1 >= 'A' && h1 <= 'F')
        return h1 - 'A' + '\xA';
    else if (h1 >= 'a' && h1 <= 'f')
        return h1 - 'a' + '\xA';
    return 0;
}


//static
bool STRB::isblank (cchar* str)
{
    if (str) {
        for ( ; *str; str++)
            if (!STRB::isspace(*str))
                return false;
    }
    return true;
}


//static
int STRB::str2bytes (cchar* str, uint8 outbytes[], int outsize)
{
    memset (outbytes,0,outsize);
    
    //ASSERT_0 (outsize < STR2BYTES_MAX)

    if (str)
    {
        uint8 pvaltmp [STR2BYTES_MAX];
        int   n, len = 0;

        for (n=0; n<outsize; ) {
            if (!str[0])
                break;
            ((uint8*)pvaltmp)[n++] = (uint8) STRB::hex2char (str[0],str[1]);
            if (!str[1]) {
                ((uint8*)pvaltmp)[n-1] >>= 4;
                len++;
                break;
            }
            if (!str[2]) {
                len+=2;
                break;
            }
            if (str[2] == ':') {
                str += 3;  // skip 2 hex chars and a delimiter
                len += 3;
            }
            else {
                str += 2;  // skip 2 hex chars only
                len += 2;
            }
        }
        memcpy ((uint8*)outbytes+outsize-n, pvaltmp, n);
        return len;
    }
    return 0;
}


//static
int STRB::bytes2str (uint8 bytes[], int nbytes, char* strout, bool usecolon)
{
    int len = 0;
	if (nbytes > 0) {
		cchar* sformt = (usecolon) ? "%02X:":"%02X";
		STRB sout(strout);
		for (int i=0; ; i++) {
			if (i==nbytes-1) {
				len += sout.PrnSetEnd("%02X", bytes[i]);
				break;
			}
			len += sout.PrnSetEnd (sformt, bytes[i]);
		}
	}
    return len;
}


//static
uint32 STRB::str2ip (cchar* str)
{
	unsigned u1, u2, u3, u4;
	if (sscanf(str, "%u.%u.%u.%u", &u1, &u2, &u3, &u4) != 4)
        return 0;
    return IpAddr(u1,u2,u3,u4);
}


//static
bool STRB::str2ip (cchar* str, uint32* x)
{
	unsigned u1, u2, u3, u4;
	if (sscanf(str, "%u.%u.%u.%u", &u1, &u2, &u3, &u4) != 4)
        return false;
    *x = IpAddr(u1,u2,u3,u4);
	return true;
}


//static 
uint32 STRB::str2ipmask (cchar* str, int* nbits)
{
    char*	 separ;
    STR<32>  s;

    s = str;
    if ( (separ = s.ScanChar('/')) ) {
        *separ = '\0';
        *nbits = ::atoi(separ+1);
        if (*nbits > 32)
            *nbits = -1; // error
    }
    else {
        // Default mask
        *nbits = 32;
    }

    return str2ip ((char*)s);
}


//static
int STRB::ip2str (uint32 ipaddr, char* strout)
{
    return sprintf (strout,"%d.%d.%d.%d", IpByte0(ipaddr), IpByte1(ipaddr), IpByte2(ipaddr), IpByte3(ipaddr));
}


//static
int STRB::ipport2str (uint32 ipaddr, uint16 port, char* strout)
{
    return sprintf (strout,"%d.%d.%d.%d:%d", IpByte0(ipaddr), IpByte1(ipaddr), IpByte2(ipaddr), IpByte3(ipaddr), port);
}


//static
int STRB::decimal2str (int64 x, uint32 n, char* strout)
{
	char s[UINT64_MAX_LEN];
	int len = lltoa(x,s);
	int len_till_point = len - n;
	if (len_till_point <= 0) {
			strout[0] = '0';
			strout[1] = '.';
			memset (strout + 2, '0', -len_till_point);
			int lennew = 2 - len_till_point;
			memcpy (strout + lennew, s, len + 1);
			return lennew + len;
	}
	memcpy (strout, s, len_till_point);
	memcpy (strout + len_till_point + 1, s + len_till_point, n + 1);
	strout[len_till_point] = '.';
	return len + 1;
}


//static
uint32 STRB::str2ipport (cchar* str, uint16* port)
{
    char*	 separ;
    STR<32>  s;

    s = str;
    if ( (separ = s.ScanChar(':')) ) {
        *separ = '\0';
        *port = (uint16) ::atoi(separ+1);
    }
    else {
        *port = 0;
    }

    return str2ip ((char*)s);
}



//static 
bool STRB::strequ_rn (cchar* str1, cchar* str2)
{
	if (!str1 || !str2)
		return false;
	int i;
	for ( i = 0; str1[i]==str2[i] && str1[i] && str1[i]!='\r' && str1[i]!='\n' &&
									 str2[i] && str2[i]!='\r' && str2[i]!='\n';     i++ );
	return (str1[i] == str2[i]); 
}


//static 
bool STRB::wildcmp (cchar* str, cchar *wild, bool case_sens)
{
	// We want to support some CSV formats when end of string is '\0' and ','
	#if 0
	#define WILDCMP_ENDOF_STR_CONDITION	(*str && *str!=',')
	#else
	#define WILDCMP_ENDOF_STR_CONDITION	(*str)
	#endif

	cchar *cp = 0, *mp = 0;

	while (WILDCMP_ENDOF_STR_CONDITION  &&  *wild != '*') {
		if (*wild!=*str && *wild!='?')
			return false;
		wild++;
		str++;
	}

	while (WILDCMP_ENDOF_STR_CONDITION) {
		if (*wild == '*') {
			if (!*++wild)
				return true;
			mp = wild;
			cp = str+1;
		}
		else if ((case_sens ? *wild==*str : tolower(*wild)==tolower(*str)) || *wild=='?') {
			wild++;
			str++;
		}
		else {
			wild = mp;
			str = cp++;
		}
	}

	while (*wild == '*') wild++;
	return !*wild;
}


//static
int STRB::strcicmp (cchar* str1, cchar* str2)
{
	for (;; str1++, str2++) {
		int d = tolower(*str1) - tolower(*str2);
		if (d != 0 || !*str1)
			return d;
	}
}

//static
char* STRB::stristr (char* str1, cchar* str2)
{
	char*   p1 = str1;
	cchar*  p2 = str2;
	char*   r = (*p2 == 0) ? str1 : 0;

	while (*p1 != 0 && *p2 != 0) {
		if (tolower(*p1) == tolower(*p2)) {
			if (r == 0)
				r = p1;
			p2++;
		}
		else {
			p2 = str2;
			if (tolower(*p1) == tolower(*p2)) {
				r = p1;
				p2++;
			}
			else 
				r = 0;
		}
		p1++;
	}

	return (*p2 == 0) ? r : 0;
}


//static
void STRB::replace (char* str, char from, char to)
{
	while (*str) {
		if (*str == from)
			*str = to;
		str++;
	}
}


STRB& STRB::Print (cchar* sformat, ...)
{
	va_list  argptr;
	va_start (argptr, sformat);
	Len = vsprintf (Str, sformat, argptr);
	va_end (argptr);
	return (*this);
}


int STRB::AddSprintf (cchar* sformat, ...)
{
	va_list  argptr;
	va_start (argptr, sformat);
	int ret = vsprintf (Str+Len, sformat, argptr);
	va_end (argptr);
	if (ret > 0)
		Len += ret;
	return ret;
}


int STRB::PrnSetEnd (cchar* sformat, ...)
{
	va_list  argptr;
	va_start (argptr, sformat);
	int ret = vsprintf (Str, sformat, argptr);
	va_end (argptr);
	Len = ret;
	SetToEnd();
	return ret;
}


int STRB::CopySetEnd (cchar* s)
{
    int ret = Copy(s);
    SetToEnd();
    return ret;
}


int STRB::CopySetEnd (cchar* s, int size)
{
    Copy(s,size);
    SetToEnd();
    return size;
}


bool STRB::CompEx (cchar* s, int* n)
{
    int i;
    bool res = true;

    for (i = 0; s[i]; i++) {
        if (!Str[i] || (Str[i]!=s[i])) {
            res = false;
            break;
        }
    }

    if (n)
        *n = i;

    return res;
}


int STRB::Copy (cchar* s)
{
    Len = 0;
    if (s) {
        for (; s[Len]; Len++) {
            if (IsLenExceeded())
                break;
            Str[Len] = s[Len];
        }
    }
    Str[Len] = '\0';
    return Len;
}


int STRB::Add (cchar* s)
{
    int i = 0;
    if (s) {
        for (; s[i]; Len++, i++) {
            if (IsLenExceeded())
                break;
            Str[Len] = s[i];
        }
        Str[Len] = '\0';
    }
    return i;
}


int STRB::Add (cchar* s, char terminator, char terminator2)
{
    int i = 0;
    if (s) {
        for (; s[i] && s[i]!=terminator && s[i]!=terminator2; Len++, i++) {
            if (IsLenExceeded())
                break;
            Str[Len] = s[i];
        }
        Str[Len] = '\0';
    }
    return i;
}


int STRB::Add (int count, char ch)
{
    if (IsLenExceeded(count))
        return 0;
    char *send1 = Str + Len;	// or End()
	char *send2 = send1 + count;
    for (; send1 < send2; send1++) *send1 = ch;
    Str[Len += count] = '\0';
	return count;
}


int STRB::AddEx (cchar* s, int size)
{
    int j = Add(s,',');
    int i = size - j;
    if (i > 0 && Add(i,' '))
		return size;
    return j;
}


int STRB::SeekInt (cchar* format)
{
	SkipChars('0');
	if (!isdigit(*Spos))
		return 0;
    int x;
    if (sscanf(Spos, format, &x) != 1) {
		SeekError = true;
        return 0;
	}
    STR<16> s;
    s = x;
    Spos += s.Strlen();	// increment Spos by number of digits in x
    return x;
}


int64 STRB::SeekInt64 (cchar* format)
{
	SkipChars('0');
	if (!isdigit(*Spos))
		return 0;
    int64 x;
	if (sscanf(Spos, format, &x) != 1) {
		SeekError = true;
        return 0;
	}
    
    STR<24> s;
    s = x;
    Spos += s.Strlen();	// increment Spos by number of digits in x
    return x;
}


int STRB::SeekIp ()
{
	unsigned u1, u2, u3, u4;
	if (sscanf(Spos, "%u.%u.%u.%u", &u1, &u2, &u3, &u4) != 4) {
		SeekError = true;
        return 0;
    }
    int x = u1<<24 | u2<<16 | u3<<8 | u4;
    // now calculate number of digits in *x
    STR<64> s;
    s =  u1;
    s += u2;
    s += u3;
    s += u4;
    Spos += s.Strlen() + 3;
    return x;
}


char* STRB::ScanStrSkip (cchar* s)
{
    char* s1 = ScanStr(s);
    if (!s1)
        return s1;
    int len = (int)strlen(s);
    return Spos += len;
}


char* STRB::ScanStrNextSkip (cchar* s)
{
    char* s1 = ScanStrNext(s);
    if (!s1)
        return s1;
    int len = (int)strlen(s);
    return Spos += len;
}


char* STRB::ScanStrSkip (STRB& s)
{
    char* s1 = ScanStr(s);
    if (!s1)
        return s1;
    return Spos += s.Length();
}


char* STRB::Strlwr()
{
    int currLen = Strlen();
    for (int i=0; i<currLen; i++)
        Str[i] = tolower(Str[i]);
    return Str;
}


char* STRB::ToLower()
{
    for (unsigned i=0; i<Len; i++)
        Str[i] = STRB::tolower(Str[i]);
    return Str;
}

