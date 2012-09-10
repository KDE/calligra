/* This file is part of the KDE project
   Copyright (C) 2009, 2011 Dag Andersen <danders@get2net.dk>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/
#include "aboutpage.h"

#include "kptproject.h"

#include <KoIcon.h>

#include <QTextCodec>
#include <QApplication>
#include <QDir>
#include <QString>

#include <kaboutdata.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <KUrl>
#include <KHTMLPart>

KPlatoAboutPage::KPlatoAboutPage()
    : m_project( 0 )
{
}

KPlatoAboutPage::~KPlatoAboutPage()
{
}

QString KPlatoAboutPage::main()
{
    KIconLoader *iconloader = KIconLoader::global();
    QString res = loadFile( KStandardDirs::locate( "data", "plan/about/main.html" ));
    if ( res.isEmpty() ) {
        return res;
    }
    const char *const continue_icon_id =
        (QApplication::isRightToLeft() ? koIconNameCStr("go-previous") : koIconNameCStr("go-next"));
    const QString continue_icon_path = iconloader->iconPath(continue_icon_id, KIconLoader::Small);

    QString icon_path = "<img width='16' height='16' src=\"" + continue_icon_path + "\">";

    res = res.arg( KStandardDirs::locate( "data", "kdeui/about/kde_infopage.css" ) );
    if ( qApp->layoutDirection() == Qt::RightToLeft )
    res = res.arg( "@import \"%1\";" ).arg( KStandardDirs::locate( "data", "kdeui/about/kde_infopage_rtl.css" ) );
    else
    res = res.arg( "" );

    res = res.arg( i18n( "Plan" ) )
    .arg( i18nc("KDE 4 tag line, see http://kde.org/img/kde40.png", "Be free.") )
    .arg( i18n("Plan is a Project Planning and Management application.") )
    .arg( i18n("Welcome to Plan.") )
    .arg( i18n("These introductory pages should give you an idea of how to use Plan and what you can use it for.") )
    .arg( icon_path ).arg( i18n( "A short introduction." ) )
    .arg( icon_path ).arg( i18n( "Tips on how to manipulate and inspect data." ) )
    .arg( icon_path ).arg( i18n( "A small tutorial to get you started." ) )
    .arg( i18n(
        "<em>Note:</em> To view these pages when you are in other parts of Plan, choose the menu option <em>Help ->  Introduction to Plan</em>."
        ) )
    ;

    return res;
}

QString KPlatoAboutPage::intro()
{
    KIconLoader *iconloader = KIconLoader::global();
    QString res = loadFile( KStandardDirs::locate( "data", "plan/about/intro.html" ));
    if ( res.isEmpty() ) {
        return res;
    }
    const char *const continue_icon_id =
        (QApplication::isRightToLeft() ? koIconNameCStr("go-previous") : koIconNameCStr("go-next"));
    const QString continue_icon_path = iconloader->iconPath(continue_icon_id, KIconLoader::Small);

    res = res.arg( KStandardDirs::locate( "data", "kdeui/about/kde_infopage.css" ) );
    if ( qApp->layoutDirection() == Qt::RightToLeft )
	res = res.arg( "@import \"%1\";" ).arg( KStandardDirs::locate( "data", "kdeui/about/kde_infopage_rtl.css" ) );
    else
        res = res.arg( "" );

    res = res.arg( i18n("Plan is a Project Planning and Management application.") )
    .arg( i18n( 
        "Plan is intended for managing moderately large projects with multiple resources. To enable you to model your project adequately, Plan offers different types of task dependencies and timing constraints. Usually you will define your tasks, estimate the effort needed to perform each task, allocate resources and then schedule the project according to the dependency network and resource availability."
        "<p>You can find more information in the <a href=\"help:plan\">documentation</a> "
        "or online at <a href=\"http://www.calligra.org/plan\">http://www.calligra.org/plan</a></p>"
        ) )
    .arg( "<img width='16' height='16' src=\"%1\">" ).arg( continue_icon_path )
    .arg( i18n( "Next: Tips" ) )
    ;

    return res;
}

QString KPlatoAboutPage::tips()
{
    
    QString res = loadFile( KStandardDirs::locate( "data", "plan/about/tips.html" ));
    if ( res.isEmpty() ) {
        return res;
    }

    KIconLoader *iconloader = KIconLoader::global();
    QString viewmag_icon_path =
        iconloader->iconPath(koIconNameCStr("zoom-in"), KIconLoader::Small);
    QString history_icon_path =
        iconloader->iconPath(koIconNameCStr("view-history"), KIconLoader::Small);
    QString openterm_icon_path =
        iconloader->iconPath(koIconNameCStr("utilities-terminal"), KIconLoader::Small);
    QString locationbar_erase_rtl_icon_path =
        iconloader->iconPath(koIconNameCStr("edit-clear-locationbar-rtl"), KIconLoader::Small);
    QString locationbar_erase_icon_path =
        iconloader->iconPath(koIconNameCStr("edit-clear-locationbar-ltr"), KIconLoader::Small);
    QString window_fullscreen_icon_path =
        iconloader->iconPath(koIconNameCStr("view-fullscreen"), KIconLoader::Small);
    QString view_left_right_icon_path =
        iconloader->iconPath(koIconNameCStr("view-split-left-right"), KIconLoader::Small);
    const char *const continue_icon_id =
        (QApplication::isRightToLeft() ? koIconNameCStr("go-previous") : koIconNameCStr("go-next"));
    const QString continue_icon_path = iconloader->iconPath(continue_icon_id, KIconLoader::Small);

    res = res.arg( KStandardDirs::locate( "data", "kdeui/about/kde_infopage.css" ) );
    if ( qApp->layoutDirection() == Qt::RightToLeft )
        res = res.arg( "@import \"%1\";" ).arg( KStandardDirs::locate( "data", "kdeui/about/kde_infopage_rtl.css" ) );
    else
        res = res.arg( "" );

    res = res.arg( i18n("Editing tips.") )
    .arg( i18n( 
        "<br/><b>To</b> edit project data, different views and editors can be selected in the View Selector docker."
        "<br/><b>The</b> views are generally used to inspect data after the project has been scheduled. No data will appear in the views if the project has not been scheduled. Scheduling is done in the Schedules editor."
        "<br/><b>You</b> can edit attributes in the various editors by selecting the item you want to edit (doubleclick or press F2), or open a dialog using the context menu."
        "</ul>"
        ) )
    .arg( "<img width='16' height='16' src=\"%1\">" ).arg( continue_icon_path )
    .arg( i18n( "Next: Create a simple project" ) )
    ;

    return res;
}

QString KPlatoAboutPage::tutorial( const QString &header, const QString &text, const QString &nextpage, const QString &nexttext )
{
    QString res = loadFile( KStandardDirs::locate( "data", "plan/about/tutorial.html" ));
    if ( res.isEmpty() ) {
        return res;
    }

    const char *const nextIconName = QApplication::isRightToLeft() ? koIconNameCStr("go-previous") : koIconNameCStr("go-next");
    const QString next_icon_path = KIconLoader::global()->iconPath(nextIconName, KIconLoader::Small );

    res = res.arg( KStandardDirs::locate( "data", "kdeui/about/kde_infopage.css" ) );
    if ( qApp->layoutDirection() == Qt::RightToLeft )
        res = res.arg( "@import \"%1\";" ).arg( KStandardDirs::locate( "data", "kdeui/about/kde_infopage_rtl.css" ) );
    else
        res = res.arg( "" );

    res = res.arg( header )
    .arg( text )
    .arg( "about:plan/" + nextpage )
    .arg( "<img width='16' height='16' src=\"%1\">" ).arg( next_icon_path )
    .arg( nexttext )
    ;

    return res;
}

QString KPlatoAboutPage::tutorial1()
{
    return tutorial(
        i18n("Create the simplest project ever."),
        i18nc( "1=datetime",
            "Select the task editor <em>Editors->Tasks</em>:"
            "<ul>"
            "<li>Create a task by selecting <em>Add Task</em> in the toolbar.</li>"
            "<li>Set <em>Type</em> to <em>Duration</em>.</li>"
            "<li>Set <em>Estimate</em> to <em>8 hours</em>.</li>"
            "<li>Set <em>Constraint</em> to <em>As Soon As Possible</em>.</li>"
            "</ul>"

            "Select the schedules editor <em>Editors->Schedules</em>:"
            "<ul>"
            "<li>Create a schedule by selecting <em>Add Schedule</em> in the toolbar.</li>"
            "<li>Calculate the schedule by selecting <em>Calculate</em> in the toolbar.</li>"
            "</ul>"
            "The task should now have been scheduled to start %1 with a duration of 8 hours. You can check this by selecting the gantt chart <em>Views->Gantt</em>."
        , KGlobal::locale()->formatDateTime( m_project->startTime(), KLocale::FancyLongDate ) ),
        "tutorial2",
        i18n( "Next: Resource allocation" )
    );
}

QString KPlatoAboutPage::tutorial2()
{
    DateTime dt = m_project->constraintStartTime();
    if ( m_project->defaultCalendar() ) {
        dt = m_project->defaultCalendar()->firstAvailableAfter( dt, m_project->constraintEndTime() );
    }
    return tutorial(
        i18n("Allocate a resource to the task."),
        i18nc( "1=datetime",
            "Select the task editor <em>Editors->Tasks</em>:"
            "<ul>"
            "<li>Enter a name (e.g. 'John') in the <em>Allocation</em> column."
            " (Plan will automatically create a resource with name 'John' under resource group 'Resources'.</li>"
            "<li>Set <em>Type</em> to <em>Effort</em>.</li>"
            "</ul>"
            "Now you need to schedule the project again with the new allocation:"
            "<br/>Select the schedules editor <em>Editors->Schedules</em> and calculate the schedule by selecting <em>Calculate</em> in the toolbar."
            "<p>The task should be scheduled to start %1 with a duration of 8 hours. You can check this by selecting the gantt chart <em>Views->Gantt</em>.<p>"
        , KGlobal::locale()->formatDateTime( dt, KLocale::FancyLongDate ) ),
        "main",
        i18n( "Next: Introduction" )
    );
}

void KPlatoAboutPage::generatePage( KHTMLPart &part, const KUrl &url)
{
    QString html;
    if (url.url() == "about:plan/main")
        html = main();
    else if (url.url() == "about:plan/intro")
        html = intro();
    else if (url.url() == "about:plan/tips")
        html = tips();
    else if (url.url() == "about:plan/tutorial")
        html = tutorial1();
    else if (url.url() == "about:plan/tutorial2")
        html = tutorial2();
    else
        html = main();

    part.begin();
    part.write( html );
    part.end();
}

QString KPlatoAboutPage::loadFile( const QString& file )
{
    QString res;
    if ( file.isEmpty() )
    return res;

    QFile f( file );

    if ( !f.open( QIODevice::ReadOnly ) )
    return res;

    QTextStream t( &f );

    res = t.readAll();

    // otherwise all embedded objects are referenced as about:/...
    QString basehref = QLatin1String("<BASE HREF=\"file:") +
               file.left( file.lastIndexOf( '/' )) +
               QLatin1String("/\">\n");
    res.replace("<head>", "<head>\n\t" + basehref, Qt::CaseInsensitive);
    return res;
}

