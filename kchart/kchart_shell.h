#ifndef __kchart_shell_h__
#define __kchart_shell_h__

class KChartShell_impl;

#include "kchart.h"
#include "kchart_doc.h"

#include <default_shell_impl.h>
#include <document_impl.h>

class KChartShell : public DefaultShell_impl
{
  Q_OBJECT
public:
  // C++
  KChartShell();
  ~KChartShell();
  
  // IDL
  void fileNew();

  // C++
  virtual void setDocument( KChartDocument *_doc );
  
  virtual bool openDocument( const char *_filename );
  virtual bool saveDocument( const char *_file, const char *_format );

  virtual void cleanUp();
  
protected:
  Document_ref m_rDoc;
};

#endif
