/**
 * KFormula - Formula Editor
 *
 * This code is free. Do with it what you want.
 *
 * (c) KDE Project
 * Andrea Rizzi
 * Bilibao@ouverture.it
 */

#include "part.h"
#include <kpart/kpart.h>


extern "C" void initkformula(void)
{
}

extern "C" void* newkformula( void *s, void *w )
{
    KPartShell *shell = (KPartShell*)s;
    QWidget *win = (QWidget*)w;
    
    KPart *part = new FormulaEditor( shell, win );

    return (void*)part;
}


