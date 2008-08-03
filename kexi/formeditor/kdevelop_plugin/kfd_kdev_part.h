/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>

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

#ifndef KFORMDESIGNER_KDEVELOP_PART_H
#define KFORMDESIGNER_KDEVELOP_PART_H

#include <qwidget.h>
#include <qpixmap.h>
//Added by qt3to4:
#include <Q3ValueList>
#include <QCloseEvent>

#include <kinterfacedesigner/designer.h>
#include <kparts/factory.h>

#include "form.h"

class KAboutData;
class KComponentData;
class QWorkspace;
class QCloseEvent;

using KFormDesigner::Form;
using namespace KInterfaceDesigner;

class KFORMEDITOR_EXPORT KFDFactory : public KParts::Factory
{
    Q_OBJECT

public:
    KFDFactory();
    virtual ~KFDFactory();

    virtual KParts::Part* createPartObject(QWidget *parentWidget = 0, const char *widgetName = 0, QObject *parent = 0, const char *name = 0,
                                           const char *classname = "KParts::Part", const QStringList &args = QStringList());

    static const KComponentData &componentData();
    static KAboutData *aboutData();

private:
    static KComponentData *m_componentData;
};

class KFORMEDITOR_EXPORT KFormDesignerKDevPart : public Designer
{
    Q_OBJECT

public:
    KFormDesignerKDevPart(QWidget *parent, const char *name, bool readOnly = true, const QStringList &args = QStringList());
    virtual ~KFormDesignerKDevPart();

    virtual DesignerType designerType() {
        return QtDesigner;
    }
    virtual void openProject(const QString &) {}

//  KFormDesigner::FormManager*   manager()  { return m_manager; }
    void      setUniqueFormMode(bool enable)  {
        m_uniqueFormMode = enable;
    }

    bool   closeForm(Form *form);
    bool   closeForms();

    virtual bool closeUrl();

    static KFormDesigner::WidgetLibrary* formsLibrary();

public slots:
    /*! Creates a new blank Form. The new Form is shown and become the active Form. */
    void createBlankForm();
    /*! Loads a Form from a UI file. A "Open File" dialog is shown to select the file. The loaded Form is shown and becomes
       the active Form. */
    void open();
    void slotPreviewForm();
    void saveAs();
    void slotCreateFormSlot(KFormDesigner::Form *form, const QString &widget, const QString &signal);

protected slots:
    void slotWidgetSelected(KFormDesigner::Form *form, bool multiple);
    void slotFormWidgetSelected(KFormDesigner::Form *form);
    void slotNoFormSelected();
    void slotFormModified(KFormDesigner::Form *form, bool isDirty);
    void setUndoEnabled(bool enabled, const QString &text);
    void setRedoEnabled(bool enabled, const QString &text);

protected:
    virtual bool openFile();
    virtual bool saveFile();
    void disableWidgetActions();
    void enableFormActions();
    void setupActions();

private:
    static KFormDesigner::WidgetLibrary* static_formsLibrary;
//  KFormDesigner::FormManager  *m_manager;
    QWorkspace  *m_workspace;
    int  m_count;
    bool   m_uniqueFormMode;
    bool   m_openingFile;
    bool   m_inShell;
};

//! Helper: this widget is used to create form's surface
class KFORMEDITOR_EXPORT FormWidgetBase : public QWidget, public KFormDesigner::FormWidget
{
    Q_OBJECT

public:
    FormWidgetBase(KFormDesignerKDevPart *part, QWidget *parent = 0, const char *name = 0, int WFlags = Qt::WDestructiveClose)
            : QWidget(parent, name, Qt::WFlags), m_part(part) {}
    ~FormWidgetBase() {
        ;
    }

    void drawRect(const QRect& r, int type);
    void drawRects(const QList<QRect> &list, int type);
    void initRect();
    void clearRect();
    void highlightWidgets(QWidget *from, QWidget *to);//, const QPoint &p);

protected:
    void closeEvent(QCloseEvent *ev);

private:
    QPixmap buffer; //!< stores grabbed entire form's area for redraw
    QRect prev_rect; //!< previously selected rectangle
    KFormDesignerKDevPart  *m_part;
};

#endif

