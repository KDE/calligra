#ifndef __kspread_main_h__
#define __kspread_main_h__

class PartShell_impl;
class KSpreadApp;
class KSpreadDocument_impl;

#include <op_app.h>
#include <part_shell_impl.h>
#include <parts.h>

#include "kspread_doc.h"
#include "kspread_shell.h"

class KSpreadApp : public OPApplication
{
  Q_OBJECT
public:
  KSpreadApp( int argc, char** argv );
  ~KSpreadApp();
  
  virtual void start();
  
protected:
  KSpreadShell_impl *m_pShell;
};

#endif
