#ifndef __kformula_main_h__
#define __kformula_main_h__

#include <koApplication.h>

#include "kformula_doc.h"
#include "kformula_shell.h"

class KFormulaShell;

class KFormulaApp : public KoApplication
{
  Q_OBJECT
public:
  KFormulaApp( int &argc, char** argv );
  ~KFormulaApp();
  
  virtual void start();
  
protected:
  KFormulaShell *m_pShell;
};

#endif
