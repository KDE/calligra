/******************************************************************/
/* KWord - (c) by Reginald Stadlbauer and Torben Weis 1997-1998   */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer, Torben Weis                       */
/* E-Mail: reggie@kde.org, weis@kde.org                           */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Shell (header)                                         */
/******************************************************************/

#ifndef KWORD_SHELL
#define KWORD_SHELL

#include <koMainWindow.h>

class QAction;

/******************************************************************/
/* Class: KWordShell                                              */
/******************************************************************/

class KWordShell : public KoMainWindow
{
    Q_OBJECT
public:
    KWordShell( QWidget* parent = 0, const char* name = 0 );
    ~KWordShell();

    QString nativeFormatMimeType() const { return "application/x-kword"; }
    QString nativeFormatPattern() const { return "*.kwd"; }
    QString nativeFormatName() const { return "KWord"; }

    virtual void setRootPart( Part* );

public slots:
    void slotFilePrint();
    
protected:
    virtual QString configFile() const;
    virtual KoDocument* createDoc();
};

#endif

