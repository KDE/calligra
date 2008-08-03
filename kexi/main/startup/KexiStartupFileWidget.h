/* This file is part of the KDE project
   Copyright (C) 2003-2007 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef _KexiStartupFileWidget_h_
#define _KexiStartupFileWidget_h_

#include <QSet>
#include <kfilewidget.h>
#include <kexi_export.h>

class QEvent;

//! @short Widget for opening/saving files supported by Kexi
/*! For simplicity, initially the widget has hidden the preview pane. */
class KEXIMAIN_EXPORT KexiStartupFileWidget : public KFileWidget
{
    Q_OBJECT

public:
    /*! Dialog mode:
    - Opening opens existing database (or shortcut)
    - SavingFileBasedDB saves file-based database file
    - SavingServerBasedDB saves server-based (shortcut) file
    - CustomOpening can be used for opening other files, like CSV
    */
    enum ModeFlag {
        Opening = 1,
        SavingFileBasedDB = 2,
        SavingServerBasedDB = 4,
        Custom = 256
    };
    Q_DECLARE_FLAGS(Mode, ModeFlag)

    KexiStartupFileWidget(
        const KUrl &startDirOrVariable, Mode mode, QWidget *parent);

    virtual ~KexiStartupFileWidget();

    /*! Helper. Displays "The file %1 already exists. Do you want to overwrite it?" yes/no message box.
    \a parent is used as a parent of the KMessageBox.
    \return true if \a filePath file does not exists or user has agreed on overwriting,
    false in user do not want to overwrite. */
    static bool askForOverwriting(const QString& filePath, QWidget *parent = 0);

    void setMode(Mode mode);

    QSet<QString> additionalFilters() const;

    //! Sets additional filters list, e.g. "text/x-csv"
    void setAdditionalFilters(const QSet<QString>& mimeTypes);

    QSet<QString> excludedFilters() const;

    //! Excludes filters list
    void setExcludedFilters(const QSet<QString>& mimeTypes);

    // KUrl currentURL();
    virtual QString highlightedFile() const;

    //#ifndef Q_WS_WIN
    // KUrlComboBox *locationWidget() const;
    //#endif
    //! just sets locationWidget()->setCurrentText(fn)
    //! (and something similar on win32)
    void setLocationText(const QString& fn);

    //! Sets default extension which will be added after accepting
    //! if user didn't provided one. This method is usable when there is
    //! more than one filter so there is no rule what extension should be selected
    //! (by default first one is selected).
    void setDefaultExtension(const QString& ext);

    /*! \return true if the current URL meets requies constraints
    (i.e. exists or doesn't exist);
    shows appropriate message box if needed. */
    bool checkSelectedFile();
    // bool checkURL();

    /*! If true, user will be asked to accept overwriting existing file.
    This is true by default. */
    void setConfirmOverwrites(bool set);

    //virtual bool eventFilter ( QObject * watched, QEvent * e );

public slots:
    virtual void showEvent(QShowEvent * event);
    virtual void focusInEvent(QFocusEvent *);

    // Typing a file that doesn't exist closes the file dialog, we have to
    // handle this case better here.
    virtual void accept();

signals:
    //entered file name is accepted
//alread avail. in KFileWidget  void accepted();
    void fileHighlighted();
    void rejected();

protected slots:
    virtual void reject();
    void slotExistingFileHighlighted(const QString& fileName);

private:
    void updateFilters();

    class Private;
    Private * const d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KexiStartupFileWidget::Mode)

#endif
