#ifndef __kspread_shell_h__
#define __kspread_shell_h__

class KSpreadShell_impl;

#include <default_shell_impl.h>
#include <document_impl.h>

#include "kspread_doc.h"

class KSpreadShell_impl : public DefaultShell_impl
{
  Q_OBJECT
public:
  // C++
  KSpreadShell_impl();
  ~KSpreadShell_impl();

  // IDL
  virtual void fileNew();
  
  // C++
  virtual void setDocument( KSpreadDoc *_doc );
  
  virtual bool openDocument( const char *_filename );
  virtual bool saveDocument( const char *_file, const char *_format );
  virtual bool printDlg();
  
  virtual void cleanUp();
  
protected:
  Document_ref m_rDoc;
  OPParts::View_var m_vView;
};

#endif
