/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2004-2005 Jarosław Staniek <staniek@kde.org>
   Copyright (C) 2005 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2005 Sebastian Sauer <mail@dipe.org>

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

#include "kexiscriptdesignview.h"
#include "kexiscripteditor.h"

#include <kross/core/manager.h>
#include <kross/core/action.h>
#include <kross/core/action.h>
#include <kross/core/interpreter.h>

#include <QLayout>
#include <QSplitter>
#include <QTimer>
#include <QDateTime>
#include <QDomDocument>
#include <q3stylesheet.h>
#include <ktextbrowser.h>
#include <kfiledialog.h>
#include <kmenu.h>
#include <kactionmenu.h>
#include <kdebug.h>

#include <KexiMainWindowIface.h>
//#include <kexidialogbase.h>
#include <kexidb/connection.h>
#include <QTextDocument>

/// @internal
class KexiScriptDesignViewPrivate
{
public:

    QSplitter* splitter;

    /**
     * The \a Kross::Action instance which provides
     * us access to the scripting framework Kross.
     */
    Kross::Action* scriptaction;

    /// The \a KexiScriptEditor to edit the scripting code.
    KexiScriptEditor* editor;

    /// The \a KoProperty::Set used in the propertyeditor.
    KoProperty::Set* properties;

    /// Boolean flag to avoid infinite recursion.
    bool updatesProperties;

    /// Used to display statusmessages.
    KTextBrowser* statusbrowser;
    
    /** The type of script
     *  executable = regular script that can be executed by the user
     *  module = a script which doesn't contain a 'main', only
     *           functions that can be used by other scripts
     *  object = a script which contains code to be loaded into another
                 object such as a report or form
     */
    QString scriptType;
};

KexiScriptDesignView::KexiScriptDesignView(
    QWidget *parent, Kross::Action* scriptaction)
        : KexiView(parent)
        , d(new KexiScriptDesignViewPrivate())
{
    setObjectName("KexiScriptDesignView");
    d->scriptaction = scriptaction;
    d->updatesProperties = false;

    d->splitter = new QSplitter(this);
    d->splitter->setOrientation(Qt::Vertical);

    d->editor = new KexiScriptEditor(d->splitter);
    d->splitter->addWidget(d->editor);
    d->editor->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    d->splitter->setStretchFactor(d->splitter->indexOf(d->editor), 3);
    d->splitter->setFocusProxy(d->editor);
    //addChildView(d->editor);
    setViewWidget(d->splitter);

    d->statusbrowser = new KTextBrowser(d->splitter);
    d->splitter->addWidget(d->statusbrowser);
    d->splitter->setStretchFactor(d->splitter->indexOf(d->statusbrowser), 1);
    d->statusbrowser->setObjectName("ScriptStatusBrowser");
    d->statusbrowser->setReadOnly(true);
#if 0
    d->statusbrowser->setTextFormat(Q3TextBrowser::RichText);
#endif
    //d->browser->setWordWrap(QTextEdit::WidgetWidth);
    d->statusbrowser->installEventFilter(this);

    /*
    plugSharedAction( "data_execute", this, SLOT(execute()) );
    if(KexiEditor::isAdvancedEditor()) // the configeditor is only in advanced mode avaiable.
        plugSharedAction( "script_config_editor", d->editor, SLOT(slotConfigureEditor()) );
    */

    // setup local actions
    QList<QAction*> viewActions;

    KActionMenu* filemenu = new KActionMenu(KIcon("system-file-manager"), i18n("File"), this);
    filemenu->setObjectName("script_file_menu");
    filemenu->setToolTip(i18n("File actions"));
    filemenu->setWhatsThis(i18n("File actions"));
    QAction *a = new QAction(KIcon("document-new"), i18n("New"), this);
    a->setShortcut(Qt::CTRL + Qt::Key_N);
    connect(a, SIGNAL(triggered(bool)), this, SLOT(slotFileNew()));
    filemenu->addAction(a);
    a = new QAction(KIcon("document-open"), i18n("Open..."), this);
    a->setShortcut(Qt::CTRL + Qt::Key_O);
    connect(a, SIGNAL(triggered(bool)), this, SLOT(slotFileOpen()));
    filemenu->addAction(a);
    a = new QAction(KIcon("document-save-as"), i18n("Save As..."), this);
    a->setShortcut(Qt::CTRL + Qt::Key_S);
    connect(a, SIGNAL(triggered(bool)), this, SLOT(slotFileSave()));
    filemenu->addAction(a);
    viewActions << filemenu;

    KActionMenu* menu = new KActionMenu(KIcon("document-properties"), i18n("Edit"), this);
    menu->setObjectName("script_edit_menu");
    menu->setToolTip(i18n("Edit actions"));
    menu->setWhatsThis(i18n("Edit actions"));
    foreach(QAction *a, d->editor->defaultContextMenu()->actions()) {
        menu->addAction(a);
    }
    if (KexiEditor::isAdvancedEditor()) { // the configeditor is only in advanced mode avaiable.
        menu->addSeparator();
        QAction* a = new KAction(KIcon("configure"), i18n("Configure Editor..."), this);
        a->setObjectName("script_config_editor");
        a->setToolTip(i18n("Configure the scripting editor"));
        a->setWhatsThis(i18n("Configure the scripting editor"));
        connect(a, SIGNAL(triggered()), d->editor, SLOT(slotConfigureEditor()));
        menu->addAction(a);
    }
    viewActions << menu;
    {
        QAction* a = new KAction(KIcon("system-run"), i18n("Execute"), this);
        a->setObjectName("script_execute");
        a->setToolTip(i18n("Execute the scripting code"));
        a->setWhatsThis(i18n("Execute the scripting code"));
        connect(a, SIGNAL(triggered()), this, SLOT(execute()));
        viewActions << a;
    }
    setViewActions(viewActions);

    loadData();

    d->properties = new KoProperty::Set(this, "KexiScripting");
    connect(d->properties, SIGNAL(propertyChanged(KoProperty::Set&, KoProperty::Property&)),
            this, SLOT(slotPropertyChanged(KoProperty::Set&, KoProperty::Property&)));

    // To schedule the initialize fixes a crasher in Kate.
    QTimer::singleShot(50, this, SLOT(initialize()));
}

KexiScriptDesignView::~KexiScriptDesignView()
{
    delete d->properties;
    delete d;
}

Kross::Action* KexiScriptDesignView::scriptAction() const
{
    return d->scriptaction;
}

void KexiScriptDesignView::initialize()
{
    setDirty(false);
    updateProperties();
    d->editor->initialize(d->scriptaction);
    connect(d->editor, SIGNAL(textChanged()), this, SLOT(setDirty()));
    d->splitter->setSizes( QList<int>() << height() * 2 / 3 << height() * 1 / 3 );
}

void KexiScriptDesignView::slotFileNew()
{
    d->editor->setText(QString());
}

void KexiScriptDesignView::slotFileOpen()
{
    QStringList filters;
    foreach(QString interpreter, Kross::Manager::self().interpreters()) {
        filters << Kross::Manager::self().interpreterInfo(interpreter)->mimeTypes();
    }
    const QString file = KFileDialog::getOpenFileName(KUrl("kfiledialog:///kexiscriptingdesigner"), filters.join(" "));
    if (file.isEmpty())
        return;
    QFile f(file);
    if (! f.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    d->editor->setText(f.readAll());
    f.close();
}

void KexiScriptDesignView::slotFileSave()
{
    QStringList filters;
    foreach(QString interpreter, Kross::Manager::self().interpreters()) {
        filters << Kross::Manager::self().interpreterInfo(interpreter)->mimeTypes();
    }
    const QString file = KFileDialog::getSaveFileName(KUrl("kfiledialog:///kexiscriptingdesigner"), filters.join(" "));
    if (file.isEmpty())
        return;
    QFile f(file);
    if (! f.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    f.write(d->editor->text().toUtf8());
    f.close();
}

void KexiScriptDesignView::updateProperties()
{
    if (d->updatesProperties)
        return;
    d->updatesProperties = true;

    Kross::Manager* manager = &Kross::Manager::self();

    QString interpretername = d->scriptaction->interpreter();
    Kross::InterpreterInfo* info = interpretername.isEmpty() ? 0 : manager->interpreterInfo(interpretername);

    if (!info) {
        // if interpreter isn't defined or invalid, try to fallback.
        foreach (const QString& interpretername, 
            QStringList() << "python" << "ruby" << "qtscript" << "javascript" << "java")
        {
            info = manager->interpreterInfo(interpretername);
            if (info) {
                d->scriptaction->setInterpreter(interpretername);
                break;
            }
        }
    }

    if (!info) {
        d->updatesProperties = false;
        return;
    }

    d->properties->clear();

    QStringList types;
    types << "executable" << "module" << "object";
    KoProperty::Property::ListData* typelist = new KoProperty::Property::ListData(types, types);
    KoProperty::Property* t = new KoProperty::Property(
        "type", // name
        typelist, // ListData
        (d->scriptType.isEmpty() ? "executable" : d->scriptType), // value
        i18n("Script Type"), // caption
        i18n("The type of script"), // description
        KoProperty::List // type
    );
    d->properties->addProperty(t);

    QStringList interpreters = manager->interpreters();

    kDebug() << interpreters;

    KoProperty::Property::ListData* proplist = new KoProperty::Property::ListData(interpreters, interpreters);
    KoProperty::Property* prop = new KoProperty::Property(
        "language", // name
        proplist, // ListData
        d->scriptaction->interpreter(), // value
        i18n("Interpreter"), // caption
        i18n("The used scripting interpreter."), // description
        KoProperty::List // type
    );
    d->properties->addProperty(prop);

    Kross::InterpreterInfo::Option::Map options = info->options();
    Kross::InterpreterInfo::Option::Map::ConstIterator it, end(options.constEnd());
    for (it = options.constBegin(); it != end; ++it) {
        Kross::InterpreterInfo::Option* option = it.value();
        KoProperty::Property* prop = new KoProperty::Property(
            it.key().toLatin1(), // name
            d->scriptaction->option(it.key(), option->value), // value
            it.key(), // caption
            option->comment, // description
            KoProperty::Auto // type
        );
        d->properties->addProperty(prop);
    }

    //propertySetSwitched();
    propertySetReloaded(true);
    d->updatesProperties = false;
}

KoProperty::Set* KexiScriptDesignView::propertySet()
{
    return d->properties;
}

void KexiScriptDesignView::slotPropertyChanged(KoProperty::Set& /*set*/, KoProperty::Property& property)
{
    kDebug();

    if (property.isNull())
        return;

    if (property.name() == "language") {
        QString language = property.value().toString();
        kDebug() << "language:" << language;
        d->scriptaction->setInterpreter(language);
        // We assume Kross and the HighlightingInterface are using same
        // names for the support languages...
        d->editor->setHighlightMode(language);
        updateProperties();
    }
    else if (property.name() == "type") {
        d->scriptType = property.value().toString();
    }
    else {
        bool ok = d->scriptaction->setOption(property.name(), property.value());
        if (! ok) {
            kWarning() << "unknown property:" << property.name();
            return;
        }
    }

    setDirty(true);
}

void KexiScriptDesignView::execute()
{
    d->statusbrowser->clear();
    QTime time;
    time.start();
    d->statusbrowser->append(i18n("Execution of the script \"%1\" started.", d->scriptaction->name()));

    d->scriptaction->trigger();
    if (d->scriptaction->hadError()) {
        QString errormessage = d->scriptaction->errorMessage();
        d->statusbrowser->append(QString("<b>%2</b><br>").arg(Qt::escape(errormessage)));

        QString tracedetails = d->scriptaction->errorTrace();
        d->statusbrowser->append(Qt::escape(tracedetails));

        long lineno = d->scriptaction->errorLineNo();
        if (lineno >= 0)
            d->editor->setLineNo(lineno);
    }
    else {
        // xgettext: no-c-format
        d->statusbrowser->append(i18n("Successfully executed. Time elapsed: %1ms", time.elapsed()));
    }
}

bool KexiScriptDesignView::loadData()
{
    QString data;
    if (! loadDataBlock(data)) {
        kDebug() << "no DataBlock";
        return false;
    }

    QString errMsg;
    int errLine;
    int errCol;

    QDomDocument domdoc;
    bool parsed = domdoc.setContent(data, false, &errMsg, &errLine, &errCol);

    if (! parsed) {
        kDebug() << "XML parsing error line: " << errLine << " col: " << errCol << " message: " << errMsg;
        return false;
    }

    QDomElement scriptelem = domdoc.namedItem("script").toElement();
    if (scriptelem.isNull()) {
        kDebug() << "script domelement is null";
        return false;
    }

    d->scriptType = scriptelem.attribute("scripttype");
    if (d->scriptType.isEmpty()) {
        d->scriptType = "executable";
    }

    QString interpretername = scriptelem.attribute("language");
    Kross::Manager* manager = &Kross::Manager::self();
    Kross::InterpreterInfo* info = interpretername.isEmpty() ? 0 : manager->interpreterInfo(interpretername);
    if (info) {
        d->scriptaction->setInterpreter(interpretername);

        Kross::InterpreterInfo::Option::Map options = info->options();
        Kross::InterpreterInfo::Option::Map::ConstIterator it, end = options.constEnd();
        for (it = options.constBegin(); it != end; ++it) {
            QString value = scriptelem.attribute(it.key());
            if (! value.isNull()) {
                QVariant v(value);
                if (v.convert(it.value()->value.type()))    // preserve the QVariant's type
                    d->scriptaction->setOption(it.key(), v);
            }
        }
    }

    d->scriptaction->setCode(scriptelem.text().toUtf8());

    return true;
}

KexiDB::SchemaData* KexiScriptDesignView::storeNewData(const KexiDB::SchemaData& sdata, bool &cancel)
{
    KexiDB::SchemaData *s = KexiView::storeNewData(sdata, cancel);
    kDebug() << "new id:" << s->id();

    if (!s || cancel) {
        delete s;
        return 0;
    }

    if (! storeData()) {
        kWarning() << "Failed to store the data.";
        //failure: remove object's schema data to avoid garbage
        KexiDB::Connection *conn = KexiMainWindowIface::global()->project()->dbConnection();
        conn->removeObject(s->id());
        delete s;
        return 0;
    }

    return s;
}

tristate KexiScriptDesignView::storeData(bool /*dontAsk*/)
{
    kDebug(); //<< window()->partItem()->name() << " [" << window()->id() << "]";

    QDomDocument domdoc("script");
    QDomElement scriptelem = domdoc.createElement("script");
    domdoc.appendChild(scriptelem);

    QString language = d->scriptaction->interpreter();
    scriptelem.setAttribute("language", language);
    //TODO move different types to their own part??
    scriptelem.setAttribute("scripttype", d->scriptType);

    Kross::InterpreterInfo* info = Kross::Manager::self().interpreterInfo(language);
    if (info) {
        Kross::InterpreterInfo::Option::Map defoptions = info->options();
        QMap<QString, QVariant> options = d->scriptaction->options();
        QMap<QString, QVariant>::ConstIterator it, end(options.constEnd());
        for (it = options.constBegin(); it != end; ++it)
            if (defoptions.contains(it.key()))  // only remember options which the InterpreterInfo knows about...
                scriptelem.setAttribute(it.key(), it.value().toString());
    }

    QDomText scriptcode = domdoc.createTextNode(d->scriptaction->code());
    scriptelem.appendChild(scriptcode);

    return storeDataBlock(domdoc.toString());
}

#include "kexiscriptdesignview.moc"

