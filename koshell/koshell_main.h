#ifndef __koshell_main_h__
#define __koshell_main_h__

#include <koApplication.h>

#include "koshell_shell.h"

class KoShellShell;

class KoShellApp : public KoApplication
{
  Q_OBJECT
public:
  KoShellApp( int &argc, char** argv );
  ~KoShellApp();
  
  virtual void start();
  
protected:
  KoShellWindow *m_pShell;
};

#endif
