/*
   $Id$
   This file is part of the KDE project
   Copyright (C) 2001 Daniel Naber <daniel.naber@t-online.de>
*/
/***************************************************************************
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 ***************************************************************************/

#ifndef __main_h__
#define __main_h__

#include <qapplication.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlistbox.h>
#include <qobject.h>
#include <qregexp.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qtabdialog.h>
#include <qtabwidget.h>
#include <qtextbrowser.h>
#include <qwidget.h>
#include <qvbox.h>

#include <kapplication.h>
#include <kcombobox.h>
#include <kcursor.h>
#include <kdatatool.h>
#include <kdebug.h>
#include <kdialogbase.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kprocess.h>
#include <krun.h>
#include <kstddirs.h>
#include <kurl.h>

class Thesaurus : public KDataTool
{
    Q_OBJECT

public:
    Thesaurus(QObject* parent, const char* name, const QStringList &);
    ~Thesaurus();
    virtual bool run(const QString& command, void* data, 
        const QString& datatype, const QString& mimetype);

protected slots:
    void slotFindTerm();
    void slotFindTerm(const QString &term);

    void thesExited(KProcess *proc);
    void receivedThesStdout(KProcess *proc, char *result, int len);
    void receivedThesStderr(KProcess *proc, char *result, int len);

    void wnExited(KProcess *proc);
    void receivedWnStdout(KProcess *proc, char *result, int len);
    void receivedWnStderr(KProcess *proc, char *result, int len);

protected:
    enum Mode {grep, other};
    void findTerm(const QString &term);
    void findTermThesaurus(const QString &term);
    void findTermWordnet(const QString &term);
    QString formatLine(QString l);
    QStringList sortQStringList(QStringList list);

    KProcess *m_thesproc;
    QString m_thesproc_stdout;
    QString m_thesproc_stderr;

    KProcess *m_wnproc;
    QString m_wnproc_stdout;
    QString m_wnproc_stderr;

    Mode m_mode;
    
    KDialogBase *m_dialog;
    QTabWidget *m_tab;
    QVBox *vbox;
    QVBox *vbox2;
    
    KHistoryCombo *m_edit;
    QLabel *m_edit_label;

    // WordNet:
    QTextBrowser *m_resultbox;
    QComboBox *m_combobox;
    
    // Thesaurus:
    QVBox *vbox_syn;
    QListBox *m_thes_syn;
    QVBox *vbox_hyper;
    QListBox *m_thes_hyper;
    QVBox *vbox_hypo;
    QListBox *m_thes_hypo;
};

#endif
