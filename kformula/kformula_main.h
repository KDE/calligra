#ifndef __kformula_main_h__
#define __kformula_main_h__

#include <koApplication.h>

#include "kformula_shell.h"

class KoMainWindow;

class KFormulaApp : public KoApplication
{
  Q_OBJECT
public:
  KFormulaApp( int &argc, char** argv );
  ~KFormulaApp();

  KoMainWindow* createNewShell() { return new KFormulaShell; }
};

#endif
