/* This file is part of the KDE project
   Copyright (C) 2003-2004 Jaroslaw Staniek <js@iidea.pl>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef _KexiStartupFileDialogBase_win_h_
#define _KexiStartupFileDialogBase_win_h_

#include <q3filedialog.h>
#include <qpushbutton.h>

class KexiStartupFileDialogBasePrivate;

/*! Wrapper for win32-like file dialog. QFileDialog is used for this.
 Temporary moved from QKW KFileDialog implementation.
 TODO: move to KDElibs/win32 KFileDialog wrapper
*/
class KexiStartupFileDialogBase : public Q3FileDialog
{
public:
    KexiStartupFileDialogBase(const QString & dirName, const QString & filter = QString(),
                              QWidget * parent = 0, const char * name = 0, bool modal = false);
    ~KexiStartupFileDialogBase();

    QPushButton * okButton() const {
        return m_okBtn;
    }

    void clearFilter();
    void setFilter(const QString& filter);
    void setOperationMode(KFileDialog::OperationMode mode);
    void setMode(KFile::Mode m);
    void setMode(unsigned int m);
    QString currentFilter() const;
    void setMimeFilter(const QStringList& mimeTypes, const QString& defaultType = QString());

    KFile::Mode mode() const;

protected:
    void init(const QString& startDir, const QString& filter, QWidget* widget);
    void updateAutoSelectExtension() {};

    //! Helper added because QFileDialog on win32 doesn't support ":" prefixes
    //! for recent dir's storage.
    QString realStartDir(const QString& startDir);

    void saveLastVisitedPath(const QString& path);

    QPushButton* m_okBtn;
    QLineEdit* m_lineEdit;
    QString m_lastVisitedPathsVariable; //!< Used by win32; @see realStartDir()
    KexiStartupFileDialogBasePrivate* d;
};

#endif
