/*
   $Id$
   This file is part of the KDE project
   Copyright (C) 2001,2002 Daniel Naber <daniel.naber@t-online.de>
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
 Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 ***************************************************************************/

#ifndef __main_h__
#define __main_h__

#include <QApplication>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QListWidget>
#include <qobject.h>
#include <QRegExp>
#include <QString>
#include <QStringList>
#include <q3tabdialog.h>
#include <QTabWidget>
#include <QTextEdit>
#include <QToolTip>
#include <QWidget>
//Added by qt3to4:
#include <QVBoxLayout>
#include <QFrame>

#include <kapplication.h>
#include <kcombobox.h>
#include <kconfig.h>
#include <kcursor.h>
#include <kdatatool.h>
#include <kdebug.h>
#include <kdialogbase.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <klineedit.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kprocess.h>
#include <kpushbutton.h>
#include <krun.h>
#include <kstandarddirs.h>
#include <kurl.h>

class QToolButton;

class Thesaurus : public KDataTool
{
    Q_OBJECT

public:
    Thesaurus(QObject* parent, const char* name, const QStringList &);
    ~Thesaurus();
    virtual bool run(const QString& command, void* data,
        const QString& datatype, const QString& mimetype);

protected slots:
    void slotChangeLanguage();

    void slotEditReturnPressed();
    void slotFindTerm();
    void slotFindTerm(const QString &term, bool add_to_history = true);
    void slotFindTermFromList(QListWidgetItem *item);

    void slotUpdateNavButtons();
    void slotGotoHistory(int index);

    void slotSetReplaceTerm(QListWidgetItem *item);
    void slotSetReplaceTerm(const QString &term);

    void slotBack();
    void slotForward();

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

/** 
 * Sort a list case insensitively.
 * Be careful: @p list is modified
 * TODO: use ksortablevaluelist?
 */
    QStringList sortQStringList(QStringList list);
    void setCaption();

    QString m_no_match;

    int m_history_pos;

    bool m_replacement;        // does this dialog offer a replace button etc.?

    KConfig *m_config;

    KProcess *m_thesproc;
    QString m_thesproc_stdout;
    QString m_thesproc_stderr;

    KProcess *m_wnproc;
    QString m_wnproc_stdout;
    QString m_wnproc_stderr;

    Mode m_mode;

    QFrame *m_page;
    QVBoxLayout *m_top_layout;

    KDialogBase *m_dialog;
    QTabWidget *m_tab;
    QWidget *vbox;
    QWidget *vbox2;

    QToolButton *m_back;
    QToolButton *m_forward;
    KPushButton *m_lang;

    KPushButton *m_search;
    KHistoryCombo *m_edit;
    QLabel *m_edit_label;

    QLabel *m_replace_label;
    KLineEdit *m_replace;

    QString m_data_file;

    // WordNet:
    QTextEdit *m_resultbox;
    QComboBox *m_combobox;

    // Thesaurus:
    QGroupBox *grpbox_syn;
    QListWidget *m_thes_syn;
    QGroupBox *grpbox_hyper;
    QListWidget *m_thes_hyper;
    QGroupBox *grpbox_hypo;
    QListWidget *m_thes_hypo;
};

#endif
