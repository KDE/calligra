#ifndef __kchart_main_h__
#define __kchart_main_h__

class PartShell_impl;
class KChartApp;
class KChartDocument_impl;

#include <op_app.h>
#include <part_shell_impl.h>
#include <parts.h>

#include "kchart_doc.h"
#include "kchart_shell.h"

/**
 * The framework.
 */
class KChartApp : public OPApplication
{
  Q_OBJECT
public:
  KChartApp( int argc, char** argv );
  ~KChartApp();
  
  /**
   * Startup function.
   */
  virtual void start();
};

#endif
