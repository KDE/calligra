#ifndef __kspread_main_h__
#define __kspread_main_h__

#include <koApplication.h>

#include "kspread_doc.h"
#include "kspread_shell.h"

class KSpreadShell;

class KSpreadApp : public KoApplication
{
  Q_OBJECT
public:
  KSpreadApp( int &argc, char** argv );
  ~KSpreadApp();
  
  virtual void start();
  
protected:
  KSpreadShell *m_pShell;
};

#endif
