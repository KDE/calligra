#ifndef __kformula_main_h__
#define __kformula_main_h__

class PartShell_impl;
class KFormulaApp;

#include <op_app.h>
#include <part_shell_impl.h>
#include <parts.h>

#include "kformula_doc.h"
#include "kformula_shell.h"

/**
 * The framework.
 */
class KFormulaApp : public OPApplication
{
    Q_OBJECT
public:
    KFormulaApp( int argc, char** argv );
    ~KFormulaApp();
    
    /**
     * Startup function.
     */
    virtual void start();
};

#endif
