#ifndef __kformula_shell_h__
#define __kformula_shell_h__

class KFormulaShell_impl;

#include "kformula.h"
#include "kformula_doc.h"

#include <default_shell_impl.h>
#include <document_impl.h>

class KFormulaShell : public DefaultShell_impl
{
  Q_OBJECT
public:
  // C++
  KFormulaShell();
  ~KFormulaShell();
  
  // IDL
  void fileNew();

  // C++
  virtual void setDocument( KFormulaDocument *_doc );
  
  virtual bool openDocument( const char *_filename );
  virtual bool saveDocument( const char *_file, const char *_format );

  virtual void cleanUp();
  
protected:
  Document_ref m_rDoc;
};

#endif
