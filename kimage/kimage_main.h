#ifndef __kimage_main_h__
#define __kimage_main_h__

#include <koApplication.h>

#include "kimage_doc.h"
#include "kimage_shell.h"

class KImageShell;

class KImageApp : public KoApplication
{
  Q_OBJECT
public:
  KImageApp( int &argc, char** argv );
  ~KImageApp();
  
  virtual void start();
  
protected:
  KImageShell *m_pShell;
};

#endif
