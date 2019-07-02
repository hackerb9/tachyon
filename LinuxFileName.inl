class FILENAME			// To avoid malloc and memory leaks
{
private:
  char *m_name[4];
  void createThreeNames (const char *filename);
  const char *createName (const char *folder, const char *file);
public:
  char* m_fName;
  FILE * Open (const char *name, const char *flags);
  FILE *Create (const char *name, const char *flags);
  i32 Rename (const char *oldname, const char *newname);
  i32 Unlink (const char *name);
  FILENAME (void);
  ~FILENAME (void);
};

FILENAME::FILENAME (void)
{
  m_name[0] = NULL;
  m_name[1] = NULL;
  m_name[2] = NULL;
  m_name[3] = NULL;
  m_fName   = NULL;
}

FILENAME::~FILENAME (void)
{
  int i;
  for (i = 0; i < 4; i++)
  {
    if (m_name[i] != NULL)
    {
      UI_free (m_name[i]);
    };
    m_name[i] = NULL;
  };
  if (m_fName != NULL) UI_free(m_fName);
}

const char * FILENAME::createName (const char *folder, const char *file)
{
  char *result;
  if (file == NULL)
  {
    return NULL;
  };
  if (strlen (file) == 0)
  {
    return NULL;
  };
  if (folder == NULL || *folder == '\0')
  {
    result = (char *) UI_malloc (strlen (file) + 1, MALLOC033);
    if (result != NULL)
    {
      strcpy (result, file);
    };
  }
  else
  {
    result = (char *) UI_malloc (strlen (folder) + strlen (file) + 2,
                                 MALLOC034);
    if (result != NULL)
    {
      strcpy (result, folder);
    };
    /* If file is used, then folder better be a real folder and not another filename. */
    if(file) 
    {
      strcat (result,"/"); strcat (result, file);
    }
  };
  return result;
}


void FILENAME::createThreeNames (const char *filename)
{
  if (m_name[0] != NULL)
  {
    UI_free (m_name[0]);
    m_name[0] = NULL;
  };
  if (m_name[1] != NULL)
  {
    UI_free (m_name[1]);
    m_name[1] = NULL;
  };
  if (m_name[2] != NULL)
  {
    UI_free (m_name[2]);
    m_name[2] = NULL;
  };
  if (m_name[3] != NULL)
  {
    UI_free (m_name[3]);
    m_name[3] = NULL;
  };
  // Linux-specific check for absolute pathways.
  if (filename && '/'==*(strspn(filename," \t")+filename) ) 
  {
    m_name[0] = (char*)createName ("", filename);
    return;
  };
  // Default folders for relative pathways.
  //if (folderSavedGame != NULL)
  //{
  //  m_name[0] = (char*)createName (folderSavedGame, filename);
  //};
  if (folderParentName != NULL)
  {
    m_name[1] = (char*)createName (folderParentName, filename);
  };
  if (folderName != NULL)
  {
    m_name[2] = (char*)createName (folderName, filename);
  };
  m_name[3] = (char*)createName (root, filename);
}

FILE *FILENAME::Open(const char *name, const char *flags)
{
  FILE *result = NULL;
  int i;
  createThreeNames (name);
  for (i = 0; i < 4; i++)
  {
    if (m_name[i] == NULL)
    {
      continue;
    };
    result = UI_fopen (m_name[i], flags);
    //		g_warning ("Ick, ick, ick: %s\n", m_name[i]);
    //printf("open(%s)\n",m_name[i]);
    if (result != NULL) 
    {
      if (TimerTraceActive)
      {
        fprintf (GETFILE (TraceFile), "Opened %s\n",
					 m_name[i]);
      };
      m_fName = (char*)UI_malloc(strlen(m_name[i])+1,MALLOC106	);
      strcpy(m_fName, m_name[i]);
      return result;
    };
  };
  return NULL;
}


FILE *FILENAME::Create (const char *name, const char *flags)
{
  int i;
  createThreeNames (name);
  for (i = 0; i < 4; i++)
  {
    if (m_name[i] == NULL)
    {
      continue;
    };
    return UI_fopen (m_name[i], flags);
  };
  return NULL;
}

i32 FILENAME::Rename (const char *oldname, const char *newname)
{
  FILENAME newfile;
  int i;
  createThreeNames (oldname);
  newfile.createThreeNames (newname);
  for (i = 0; i < 4; i++)
  {
    if (m_name[i] == NULL)
    {
      continue;
    };
    return rename (m_name[i], newfile.m_name[i]);
  };
  return -1;
}

i32 FILENAME::Unlink (const char *name)
{
  int i;
  createThreeNames (name);
  for (i = 0; i < 4; i++)
  {
    if (m_name[i] == NULL)
    {
      continue;
    };
    return UI_DeleteFile (m_name[i]);
  };
  return 0;
}

#include <sys/stat.h>
char *GETFILENAME(i32 f);

ui64 MODIFIEDTIME(i32 file)
{
  const  char* fn = GETFILENAME(file);
  struct stat  info;

  if ((fn != NULL) && (stat(fn, &info)==0)) 
  {
    return info.st_mtime;
  }
  return 0;
}
