#ifndef __img_shell_h__
#define __img_shell_h__

class ImageShell_impl;

#include "img.h"
#include "img_doc.h"

#include <default_shell_impl.h>
#include <document_impl.h>

class ImageShell_impl : public DefaultShell_impl
{
  Q_OBJECT
public:
  // C++
  ImageShell_impl();
  ~ImageShell_impl();
  
  virtual void setDocument( ImageDocument_impl *_doc );
  
  virtual bool openDocument( const char *_filename );
  virtual bool saveDocument( const char *_file, const char *_format );

  virtual void cleanUp();
  
protected:
  Document_ref m_rDoc;
};

#endif
