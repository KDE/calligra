#ifndef __img_main_h__
#define __img_main_h__

class PartShell_impl;
class ImageApp;
class ImageDocument_impl;

#include <op_app.h>
#include <part_shell_impl.h>
#include <parts.h>

#include "img_doc.h"
#include "img_shell.h"

/**
 * The framework.
 */
class ImageApp : public OPApplication
{
  Q_OBJECT
public:
  ImageApp( int argc, char** argv );
  ~ImageApp();
  
  /**
   * Startup function.
   */
  virtual void start();
};

#endif
