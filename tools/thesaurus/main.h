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

#include <qlayout.h>
#include <qstring.h>
#include <qlistbox.h>
#include <qcombobox.h>
#include <qobject.h>
#include <qregexp.h>
#include <qwidget.h>
#include <qvbox.h>

#include <kcombobox.h>
#include <kdatatool.h>
#include <kdebug.h>
#include <kdialogbase.h>
#include <klibloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kprocess.h>

class Thesaurus : public KDataTool
{
    Q_OBJECT

public:
    //Thesaurus(QObject* parent = 0, const char* name = 0, const QStringList* = 0);
    Thesaurus(QObject* parent = 0, const char* name = 0);
    ~Thesaurus();
    virtual bool run( const QString& command, void* data, const QString& datatype, const QString& mimetype);

protected slots:
    bool slotFindTerm();
    bool slotFindTerm(int index);
    bool slotFindTerm(const QString &term);
    void thesaurusExited(KProcess *proc);
    void receivedStdout(KProcess *proc, char *result, int len);
    void receivedStderr(KProcess *proc, char *result, int len);
protected:
    bool findTerm(const QString &term);
    KProcess *thesaurusproc;
    QString procresult_stdout;
    QString procresult_stderr;
    QString indent;
    QVBox *layout;
    KHistoryCombo *edit;
    QListBox *listbox;
    QComboBox *combobox;
    KDialogBase *dialog;
    void* data_access;
};

class ThesaurusFactory : public KLibFactory
{
    Q_OBJECT

public:
    ThesaurusFactory( QObject* parent = 0, const char* name = 0 );
    ~ThesaurusFactory();
    virtual QObject* createObject( QObject* parent = 0, const char* name = 0,
        const char* classname = "QObject", const QStringList &args = QStringList() );
};

#endif
