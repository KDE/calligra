/* This file is part of the KDE project
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
#include <qlayout.h>
#include <qlistbox.h>
#include <qcombobox.h>
#include <qobject.h>
#include <qregexp.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qtextbrowser.h>
#include <qwidget.h>
#include <qvbox.h>

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
#include <kurl.h>

class Thesaurus : public KDataTool
{
    Q_OBJECT

public:
    Thesaurus(QObject* parent, const char* name, const QStringList &);
    ~Thesaurus();
    virtual bool run( const QString& command, void* data, 
        const QString& datatype, const QString& mimetype);

protected slots:
    void slotFindTerm();
    void slotFindTerm(const QString &term);
    void thesaurusExited(KProcess *proc);
    void receivedStdout(KProcess *proc, char *result, int len);
    void receivedStderr(KProcess *proc, char *result, int len);

protected:
    enum Mode {grep, other};
    void findTerm(const QString &term);
    QString formatLine(QString l);

    KProcess *m_thesaurusproc;
    QString m_procresult_stdout;
    QString m_procresult_stderr;
    QVBox *m_layout;
    KHistoryCombo *m_edit;
    QTextBrowser *m_resultbox;
    QComboBox *m_combobox;
    KDialogBase *m_dialog;
    Mode m_mode;
};

#endif
