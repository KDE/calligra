/* This file is part of the KDE project
   Copyright (C) 2003-2011 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KEXINEWPROJECTASSISTANT_H
#define KEXINEWPROJECTASSISTANT_H

#include "kexidbconnectionset.h"
#include "ui_KexiOpenExistingFile.h"
#include "ui_KexiServerDBNamePage.h"
#include "ui_KexiProjectStorageTypeSelectionPage.h"
#include <kexidb/connectiondata.h>
#include <kexiutils/kmessagewidget.h>

#include <QLabel>

class Q3ListViewItem;

class KexiOpenExistingFile : public QWidget, public Ui::KexiOpenExistingFile
{
public:
    KexiOpenExistingFile(QWidget* parent);
};

class KexiServerDBNamePage : public QWidget, public Ui::KexiServerDBNamePage
{
public:
    KexiServerDBNamePage(QWidget* parent);
};

class KexiConnSelectorWidget;
class KexiProjectSelectorWidget;
class KCategorizedView;
class KPushButton;

class KexiTitleLabel : public QLabel
{
public:
    explicit KexiTitleLabel(QWidget * parent = 0, Qt::WindowFlags f = 0);
    explicit KexiTitleLabel(const QString & text, QWidget * parent = 0, Qt::WindowFlags f = 0);
    ~KexiTitleLabel();
protected:
    void changeEvent(QEvent* event);
private:
    void updateFont();
    void init();
    
    class Private;
    Private * const d;
};

class KexiAssistantPage : public QWidget
{
    Q_OBJECT
public:
    explicit KexiAssistantPage(const QString& title, const QString& description,
                               QWidget* parent = 0);
    virtual ~KexiAssistantPage();
    void setContents(QWidget* widget);
    void setContents(QLayout* layout);
public slots:
    void setDescription(const QString& text);
    void setBackButtonVisible(bool set);
    void setNextButtonVisible(bool set);
signals:    
    void back(KexiAssistantPage* page);
    void next(KexiAssistantPage* page);
protected:
    QLabel* backButton();
    QLabel* nextButton();

private slots:    
    void slotLinkActivated(const QString& link);
private:
    class Private;
    Private * const d;
};

class KexiTemplateSelectionPage : public KexiAssistantPage
{
    Q_OBJECT
public:
    explicit KexiTemplateSelectionPage(QWidget* parent = 0);
    
protected slots:
    void slotItemClicked(const QModelIndex& index);
private:
    KCategorizedView* m_templatesList;
public:
    QString selectedTemplate;
    QString selectedCategory;
};

class KexiProjectStorageTypeSelectionPage : public KexiAssistantPage, public Ui::KexiProjectStorageTypeSelectionPage
{
    Q_OBJECT
public:
    explicit KexiProjectStorageTypeSelectionPage(QWidget* parent = 0);
    virtual ~KexiProjectStorageTypeSelectionPage();
    
    bool fileTypeSelected() const { return m_fileTypeSelected; }
private slots:
    void buttonClicked();

private:
    bool m_fileTypeSelected;
};

class KexiDBTitlePage;
class KexiStartupFileHandler;
class KMessageWidget;
class KexiContextMessage;
class KexiContextMessageWidget;

class KexiProjectTitleSelectionPage : public KexiAssistantPage
{
    Q_OBJECT
public:
    explicit KexiProjectTitleSelectionPage(QWidget* parent = 0);
    virtual ~KexiProjectTitleSelectionPage();

    bool isAcceptable();

    KexiDBTitlePage* contents;
    KexiStartupFileHandler *fileHandler;
    KexiContextMessageWidget *messageWidget;
private slots:    
    void titleTextChanged(const QString & text);
    //void urlSelected(const KUrl& url);
    void askForOverwriting(const KexiContextMessage& message);
private:
    void updateUrl();
    //QString m_recentDirClass;
};

class QProgressBar;

class KexiProjectCreationPage : public KexiAssistantPage
{
    Q_OBJECT
public:
    explicit KexiProjectCreationPage(QWidget* parent = 0);
    virtual ~KexiProjectCreationPage();
    
    QProgressBar* progressBar;
};

#include <QPropertyAnimation>
#include <QPointer>
#include <QStackedLayout>

//! A tool for animated switching between widgets in a given stacked layout.
/*! Animation is performed if the graphic effects level is set at least
 at "simple" level, i.e. when
 @code
 KGlobalSettings::self()->graphicEffectsLevel() & KGlobalSettings::SimpleAnimationEffects
 @endcode
 is true. Otherwise */
class KexiAnimatedLayout : public QStackedLayout
{
    Q_OBJECT
public:
    explicit KexiAnimatedLayout(QWidget* parent = 0);

    ~KexiAnimatedLayout();

public slots:
    //! Sets the current widget to be the specified widget.
    /*! Animation is performed while switching the widgets
        (assuming animations are enabled in KGlobalSettings (see the explanation
        for @ref KexiAnimatedStackedLayout).
        The new current widget must already be contained in this stacked layout. 
        Because of the animation, changing current widget is asynchronous, i.e.
        after this methods returns, current widget is not changed. 
        Connect to signal QStackedLayout::currentChanged(int index) to be notified
        about actual change of the current widget when animation finishes.
        @note this method is not virtual, so when calling it, make sure
              the pointer is KexiAnimatedStackedLayout, not parent class QStackedLayout.
        @see setCurrentIndex() currentWidget() */
    void setCurrentWidget(QWidget* widget);

    //! Sets the current widget to be the specified index.
    /*! Animation is performed as for setCurrentWidget(). */
    void setCurrentIndex(int index);
    
private:
    class Private;
    Private* const d;
};

class KexiAnimatedLayout::Private : public QWidget
{
    Q_OBJECT
public:
    explicit Private(KexiAnimatedLayout* qq);
    void animateTo(QWidget* destination);
protected:
    void paintEvent(QPaintEvent* event);
protected slots:
    void animationFinished();
private:
    QPointer<KexiAnimatedLayout> q;
    QPixmap buffer;
    QPropertyAnimation animation;
    QPointer<QWidget> destinationWidget;
};

class KexiNewProjectAssistant : public QWidget
{
    Q_OBJECT
public:
    explicit KexiNewProjectAssistant(QWidget* parent = 0);
    ~KexiNewProjectAssistant();
public slots:
    virtual void previousPageRequested(KexiAssistantPage* sender);
    virtual void nextPageRequested(KexiAssistantPage* sender);
protected:
    void setCurrentPage(KexiAssistantPage* page);
private:
    class Private;
    Private* const d;
};

// MOC_SKIP_BEGIN
#ifdef OLD_KexiNewProjectWizard

class KEXIMAIN_EXPORT KexiNewProjectAssistant : public K3Wizard
{
    Q_OBJECT
public:
    KexiNewProjectAssistant(KexiDBConnectionSet& conn_set, QWidget *parent = 0);
    virtual ~KexiNewProjectAssistant();

    /*! \return name for a new project's database if server-based project
     type was selected. Returns file name if file-based project was selected. */
    QString projectDBName() const;

    /*! \return name for a new project. Used for both file- and serever- based projects. */
    QString projectCaption() const;

    /*! \return data of selected connection for new project,
     if server-based project type was selected.
     Returns NULL if no selection has been made or file-based project
     has been selected. */
    KexiDB::ConnectionData* projectConnectionData() const;

    /*! Reimplemented for internal reasons */
    virtual void show();

    /*! If true, user will be asked to accept overwriting existing project.
     This is true by default. */
    void setConfirmOverwrites(bool set);

protected slots:
    void slotLvTypesSelected(Q3ListViewItem *);
    void slotLvTypesExecuted(Q3ListViewItem *);
    void slotServerDBCaptionTxtChanged(const QString &capt);
    void slotServerDBNameTxtChanged(const QString &n);

    virtual void done(int r);
    virtual void next();
    virtual void accept();

protected:
    virtual void showPage(QWidget *page);

    KexiNewPrjTypeSelector *m_prjtype_sel;
    KexiDBTitlePage *m_db_title;
    KexiServerDBNamePage *m_server_db_name;
    KexiProjectSelectorWidget* m_project_selector;

    KexiConnSelectorWidget *m_conn_sel;
    QWidget *m_conn_sel_widget;

    class Private;
    Private * const d;
};
#endif
// MOC_SKIP_END

#endif
