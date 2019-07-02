#include "stdafx.h"
#include <stdio.h>
#include "UI.h"
//#include "Objects.h"
#include "Dispatch.h"
#include "CSB.h"


typedef char *pCH;
typedef pCH *ppCH;


class XTABL
{
  int m_numXlate[32];  // indexed on first character & 0x1f
  ppCH m_english[32]; 
  ppCH m_language[32]; 
public:
  Clear(void);
  XTABL(void);
  ~XTABL(void);
  void AddTranslation(const char *s1, int len1, const char *s2, int len2);
  const char *Translate(const char *text);
  int EscapeCopy(char *dest, const char *src, int len);
};

XTABL::XTABL(void)
{
  int i;
  for (i=0; i<32; i++)
  {
    m_numXlate[i] = 0;
    m_english[i] = NULL;
    m_language[i] = NULL;
  };
}

XTABL::~XTABL(void)
{
  Clear();
}

XTABL::Clear(void)
{
  int i;
  for (i=0; i<32; i++)
  {
    int j;
    for (j=0; j<m_numXlate[i]; j++)
    {
      if (m_english[i][j] != NULL) free(m_english[i][j]);
      m_english[i][j] = NULL;
      if (m_language[i][j] != NULL) free(m_language[i][j]);
      m_language[i][j] = 0;
    };
    if (m_english[i]  != NULL) free(m_english[i]);
    m_english[i] = NULL;
    if (m_language[i] != NULL) free(m_language[i]);
    m_language[i] = NULL;
    m_numXlate[i] = 0;
  };
}

int XTABL::EscapeCopy(char *dest, const char *src, int len)
{
  // Change \n to 0x0A
  int i, j;
  for (i=0, j=0; j<len; i++, j++)
  {
    dest[j] = src[i];
    if (src[i] == '\\')
    {
      if (i < len-1)
      {
        if (src[i+1] == 'n')
        {
          dest[j] = 0x0A;
          i++;
          len--;
        };
      };
    };
  };
  return len;
}

void XTABL::AddTranslation(const char *s1, int len1, const char *s2, int len2)
{
  char *S1, *S2;
  S1 = (char *)malloc(len1);
  if (S1 == NULL) return;
  S2 = (char *)malloc(len2);
  if (S2 == NULL) {free(S1); return;};
  len1 = EscapeCopy(S1, s1, len1);
  len2 = EscapeCopy(S2, s2, len2);
  int idx = S1[0];
  if (len1 > 1) idx = (idx + S1[1]);
  idx &= 0x1f;

  void *newmem;
  newmem = realloc(m_english[idx], (m_numXlate[idx]+1)*sizeof(m_english[idx][0]));
  if (newmem == NULL) 
  {
    free(S1); free(S2); return;
  };
  m_english[idx] = (ppCH)newmem;
  m_english[idx][m_numXlate[idx]] = NULL;
  newmem = realloc(m_language[idx], (m_numXlate[idx]+1)*sizeof(m_language[idx][0]));
  if (newmem == NULL)
  {
    free(S1); free(S2); return;
  };
  m_language[idx] = (ppCH)newmem;
  m_language[idx][m_numXlate[idx]] = NULL;
  m_numXlate[idx]++;
  m_english [idx][m_numXlate[idx]-1] = S1;
  m_language[idx][m_numXlate[idx]-1] = S2;
}

XTABL xlate;


struct AutoFree
{
  char *buf;
  int size;
  AutoFree(void){size=200000; buf = (char *)malloc(size);};
  ~AutoFree(void){free(buf);};
};

void ReadTranslationFile(void)
{
  AutoFree buf;
  int handle, len, len1, len2, lineNum, numNonblank;
  int dquote[4];
  xlate.Clear();
  handle = OPEN("Translation.txt", "r");
  if (handle < 0) return;
  lineNum = 0;
  while (GETS(buf.buf, buf.size-1, (i16)handle) != NULL)
  {
    int i = 0, j;
    lineNum++;
    len = (i32)strlen(buf.buf);
    if (len == 0) continue;
    if (  (buf.buf[0] == '/') && (buf.buf[1] == '/') ) continue;
    dquote[3] = -1;
    numNonblank = 0;
    for (j=0; j<4; j++) // Find 4 double-quotes
    {
      for (; i<len; i++)
      {
        if (  (buf.buf[i] != ' ') && (buf.buf[i] != '\n') )numNonblank++;
        if (buf.buf[i] == '"')
        {
          dquote[j] = i++;
          break;
        };
      };
    };
    if (numNonblank == 0) continue;
    if (dquote[3] < 0)
    {
      char line[100];
      sprintf(line,"Translation Error\nLine Number %d", lineNum);
      UI_MessageBox(line,"Warning",MESSAGE_OK);
    };
    buf.buf[dquote[1]] = 0;
    buf.buf[dquote[3]] = 0;
    len1 = dquote[1]-dquote[0]; // includes trailing nil;
    len2 = dquote[3]-dquote[2]; 
    xlate.AddTranslation(buf.buf+dquote[0]+1, len1, buf.buf+dquote[2]+1, len2);
  };
  CLOSE(handle);
}

const char *XTABL::Translate(const char *text)
{
  int idx, i;
  idx = text[0];
  if (text[0] != 0) idx += text[1];
  idx &= 0x1f;
  for (i=0; i<m_numXlate[idx]; i++)
  {
    if (strcmp(m_english[idx][i], text) == 0) return m_language[idx][i];
  };
  return text;
}


const char *TranslateLanguage(const char *text)
{
  return xlate.Translate(text);
}

void TranslateWallLanguage(unsigned char *text)
{
  // character encoding -- A=0, B=1,... space=26;  period=27; end of line = 128; end of text=129
  int i;
  const char *xText;
  for (i=0; text[i] != 129; i++)
  {
    if      (text[i] == 128) text[i] = '\n';
    else if (text[i] == 26)  text[i] = ' ';
    else if (text[i] == 27)  text[i] = '.';
    else                     text[i] = (char)('A' + text[i]);
  };
  text[i] = 0;
  xText = xlate.Translate((char *)text);
  for(i=0; xText[i] != 0; i++)
  {
    if ( (xText[i] >= 'A')  && (xText[i] <= 'Z') ) text[i] = (char)(xText[i] - 'A');
    else if (xText[i] == '\n') text[i] = 128;
    else if (xText[i] == '.')  text[i] = 27;
    else text[i] = 26;
  };
  text[i] = 129;
}

void CleanupTranslations(void)
{
  xlate.Clear();
}