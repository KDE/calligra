#ifndef __kdiagramm_main_h__
#define __kdiagramm_main_h__

#include <koApplication.h>

#include "kdiagramm_doc.h"
#include "kdiagramm_shell.h"

class KDiagrammShell;

class KDiagrammApp : public KoApplication
{
  Q_OBJECT
public:
  KDiagrammApp( int &argc, char** argv );
  ~KDiagrammApp();
  
  virtual void start();
  
protected:
  KDiagrammShell *m_pShell;
};

#endif
