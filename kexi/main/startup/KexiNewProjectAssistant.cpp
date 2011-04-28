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

#include "KexiNewProjectAssistant.h"

#include "ui_KexiOpenExistingFile.h"
#include "ui_KexiServerDBNamePage.h"
#include "KexiConnSelector.h"
#include "KexiDBTitlePage.h"
#include "KexiProjectSelector.h"
#include "KexiStartupFileWidget.h"
#include "kexi.h"
#include "KexiTemplatesModel.h"
#include "KexiStartupFileHandler.h"

#include <kexiguimsghandler.h>
#include <kexidb/utils.h>
#include <kexiutils/identifier.h>
#include <kexiutils/utils.h>

#include <kapplication.h>
#include <kiconloader.h>
#include <kmimetype.h>
#include <klocale.h>
#include <kdebug.h>
#include <kconfig.h>
#include <k3listview.h>
#include <kurlcombobox.h>
#include <kmessagebox.h>
#include <klineedit.h>
#include <kurlcombobox.h>
#include <KCategorizedView>
#include <KTitleWidget>
#include <KCategoryDrawer>
#include <KPushButton>
#include <KAcceleratorManager>
#include <KFileDialog>

#include <qpushbutton.h>
#include <qlayout.h>
#include <qcheckbox.h>
#include <QStackedLayout>
#include <QPaintEvent>
#include <QPainter>
#include <QPropertyAnimation>
#include <QDesktopWidget>
#include <QProgressBar>
 
// added because of lack of krecentdirs.h
namespace KRecentDirs
{
    KDE_IMPORT void add(const QString &fileClass, const QString &directory);
};


KexiOpenExistingFile::KexiOpenExistingFile(QWidget* parent)
        : QWidget(parent)
{
    setupUi(this);
}

KexiServerDBNamePage::KexiServerDBNamePage(QWidget* parent)
        : QWidget(parent)
{
    setupUi(this);
}

// ----

#if KDE_IS_VERSION(4,5,0)
class KexiTemplatesCategoryDrawer : public KCategoryDrawerV3
#else
class KexiTemplatesCategoryDrawer : public KCategoryDrawerV2
#endif
{
public:
    KexiTemplatesCategoryDrawer() {}
protected:
#if KDE_IS_VERSION(4,5,0)
    void mouseButtonPressed(const QModelIndex&, const QRect&, QMouseEvent *event) {
        event->accept();
    }
    void mouseButtonReleased(const QModelIndex&, const QRect&, QMouseEvent *event) {
        event->accept();
    }
#endif
};

class KexiTemplatesSelectionModel : public QItemSelectionModel
{
public:
    KexiTemplatesSelectionModel(QAbstractItemModel* model)
        : QItemSelectionModel(model)
    {
    }

    //! Reimplemented to disable full category selections.
    //! Shouldn't be needed in KDElibs >= 4.5,
    //! where KexiTemplatesCategoryDrawer::mouseButtonPressed() works.
    void select(const QItemSelection& selection,
                QItemSelectionModel::SelectionFlags command)
    {
        // kDebug() << selection.indexes().count() << command;
        if ((command & QItemSelectionModel::Select) && 
            !(command & QItemSelectionModel::Clear) &&
            (selection.indexes().count() > 1 || !this->selection().indexes().isEmpty()))
        {
            return;
        }
        QItemSelectionModel::select(selection, command);
    }
    void select(const QModelIndex& index,
                QItemSelectionModel::SelectionFlags command)
    {
        QItemSelectionModel::select(index, command);
    }
};

// ----

class KexiTitleLabel::Private {
public:
    Private() : updateFontEnabled(true) {}
    bool updateFontEnabled;
};

KexiTitleLabel::KexiTitleLabel(QWidget * parent, Qt::WindowFlags f)
 : QLabel(parent, f)
 , d(new Private)
{
    init();
}

KexiTitleLabel::KexiTitleLabel(const QString & text, QWidget * parent, Qt::WindowFlags f)
 : QLabel(text, parent, f)
 , d(new Private)
{
    init();
}

void KexiTitleLabel::init()
{
    setWordWrap(true);
    updateFont();
}

KexiTitleLabel::~KexiTitleLabel()
{
    delete d;
}

void KexiTitleLabel::updateFont()
{
    if (!d->updateFontEnabled)
        return;
    KexiUtils::BoolBlocker guard(d->updateFontEnabled, false);
    
    qreal factor;
    QRect geo = QApplication::desktop()->availableGeometry(this);
    QFont f = font();
    if (geo.width() > 600 && geo.height() > 600) {
        factor = 2.0;
    }
    else {
        factor = 1.2;
        f.setBold(true);
    }
    kDebug() << f.pointSize() << f.pixelSize();
    if (f.pointSize() == -1) {
        f.setPixelSize(qreal(f.pixelSize()) * factor);
    }
    else {
        f.setPointSize(f.pointSizeF() * factor);
    }
    setFont(f);
}

void KexiTitleLabel::changeEvent(QEvent* event)
{
    QLabel::changeEvent(event);
    if (event->type() == QEvent::FontChange) {
        updateFont();
    }
}

// ----

class KexiAssistantPage::Private {
public:    
    Private(KexiAssistantPage* q_) : q(q_), backButton(0), nextButton(0)
    {
    }
    void setButtonVisible(QLabel** button, bool back, bool set,
                          int x, int y);
    KexiAssistantPage * const q;
    QGridLayout* mainLyr;
    QLabel* descriptionLabel;
    QLabel* backButton;
    QLabel* nextButton;
};

void KexiAssistantPage::Private::setButtonVisible(QLabel** button,
                                                  bool back, /* or next */
                                                  bool set, int x, int y)
{
    if (set) {
        if (*button) {
            (*button)->show();
        }
        else {
            QString text;
            if (back) {
                text = QString::fromUtf8("‹ <a href=\"KexiAssistantPage:back\">%1</a>")
                    .arg(KStandardGuiItem::back().text().replace('&', ""));
            }
            else {
                text = QString::fromUtf8("<a href=\"KexiAssistantPage:next\">%1</a> ›")
                    .arg(i18nc("Button text: Next page in assistant (wizard)", "Next"));
            }
            *button = new QLabel(text);
            (*button)->setFocusPolicy(Qt::StrongFocus);
            int space = (*button)->fontMetrics().height() / 2;
            if (back) {
                (*button)->setContentsMargins(0, 0, space, 0);
            }
            else {
                (*button)->setContentsMargins(space, 0, 0, 0);
            }
            KAcceleratorManager::setNoAccel(*button);
            mainLyr->addWidget(*button, x, y, Qt::AlignTop);
            connect(*button, SIGNAL(linkActivated(QString)),
                    q, SLOT(slotLinkActivated(QString)));
        }
    }
    else {
        if (*button)
            (*button)->hide();
    }
}

KexiAssistantPage::KexiAssistantPage(const QString& title, const QString& description, QWidget* parent)
 : QWidget(parent)
 , d(new Private(this))
{
/*0         [titleLabel]
  1  [back] [descriptionLabel] [next]
  2         [contents]                 */
    d->mainLyr = new QGridLayout(this);
    d->mainLyr->setContentsMargins(0, 0, 0, 0);
    d->mainLyr->setColumnStretch(1, 1);
    d->mainLyr->setRowStretch(2, 1);
    KexiTitleLabel* titleLabel = new KexiTitleLabel(title);
    d->mainLyr->addWidget(titleLabel, 0, 1, Qt::AlignTop);
    d->descriptionLabel = new QLabel(description);
    int space = d->descriptionLabel->fontMetrics().height();
    d->descriptionLabel->setContentsMargins(0, 0, 0, space);
    d->descriptionLabel->setWordWrap(true);
    d->mainLyr->addWidget(d->descriptionLabel, 1, 1, Qt::AlignTop);
    /*m_backButton = new KPushButton(KStandardGuiItem::back());
    m_backButton->setFlat(true);
    m_mainLyr->addWidget(m_backButton, 1, 0);*/
    /*m_nextButton = new KPushButton(KStandardGuiItem::forward());
    m_nextButton->setFlat(true);
    m_nextButton->setContentsMargins(space, 0, 0, 0);
    m_mainLyr->addWidget(m_nextButton, 1, 2);*/
    //KAcceleratorManager::manage(this);
}

KexiAssistantPage::~KexiAssistantPage()
{
    delete d;
}

void KexiAssistantPage::setDescription(const QString& text)
{
    d->descriptionLabel->setText(text);
}

void KexiAssistantPage::setBackButtonVisible(bool set)
{
    d->setButtonVisible(&d->backButton, true/*back*/, set, 1, 0);
}

void KexiAssistantPage::setNextButtonVisible(bool set)
{
    d->setButtonVisible(&d->nextButton, false/*next*/, set, 1, 2);
}

void KexiAssistantPage::setContents(QWidget* widget)
{
    widget->setContentsMargins(0, 0, 0, 0);
    d->mainLyr->addWidget(widget, 2, 1);
}

void KexiAssistantPage::setContents(QLayout* layout)
{
    layout->setContentsMargins(0, 0, 0, 0);
    d->mainLyr->addLayout(layout, 2, 1);
}

void KexiAssistantPage::slotLinkActivated(const QString& link)
{
    if (link == QLatin1String("KexiAssistantPage:back")) {
        emit back(this);
    }
    else if (link == QLatin1String("KexiAssistantPage:next")) {
        emit next(this);
    }
}

QLabel* KexiAssistantPage::backButton()
{
    if (!d->backButton) {
        setBackButtonVisible(true);
        d->backButton->hide();
    }
    return d->backButton;
}

QLabel* KexiAssistantPage::nextButton()
{
    if (!d->nextButton) {
        setNextButtonVisible(true);
        d->nextButton->hide();
    }
    return d->nextButton;
}

// ----

KexiTemplateSelectionPage::KexiTemplateSelectionPage(QWidget* parent)
 : KexiAssistantPage(i18n("New Project"),
                  i18n("Kexi will create a new database project. Select blank database or template."),
                  parent)
{
    m_templatesList = new KCategorizedView;
    m_templatesList->setWordWrap(true);
    m_templatesList->setFrameShape(QFrame::NoFrame);
    m_templatesList->setContentsMargins(0, 0, 0, 0);
    int margin = style()->pixelMetric(QStyle::PM_MenuPanelWidth, 0, 0)
        + KDialog::marginHint();
    //m_templatesList->setCategorySpacing(5 + margin);
    //not needed in grid:
    m_templatesList->setSpacing(margin);
    m_templatesList->setSelectionMode(QAbstractItemView::SingleSelection);
    m_templatesList->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    m_templatesList->setMouseTracking(true);
    connect(m_templatesList, SIGNAL(clicked(QModelIndex)), this, SLOT(slotItemClicked(QModelIndex)));

    KexiTemplatesCategoryDrawer* templatesCategoryDrawer = new KexiTemplatesCategoryDrawer;
    m_templatesList->setCategoryDrawer(templatesCategoryDrawer);
    m_templatesList->setViewMode(QListView::IconMode);
    KexiTemplatesProxyModel* proxyModel = new KexiTemplatesProxyModel(m_templatesList);

    KexiTemplateCategoryInfoList templateCategories;
    KexiTemplateCategoryInfo templateCategory;
    templateCategory.name = "blank";
    templateCategory.caption = i18n("Blank Projects");
    
    KexiTemplateInfo info;
    info.name = "blank";
    info.caption = i18n("Blank database");
    info.description = i18n("Database project without any objects");
    info.icon = KIcon(KexiDB::defaultFileBasedDriverIcon()); //"x-office-document");
    templateCategory.addTemplate(info);
    templateCategories.append(templateCategory);
    
    templateCategory = KexiTemplateCategoryInfo();
    templateCategory.name = "office";
    templateCategory.caption = i18n("Office Templates");
    //templateCategory.enabled = false;
    
    info = KexiTemplateInfo();
    info.name = "contacts";
    info.caption = i18n("Contacts");
    info.description = i18n("Database for collecting and managing contacts");
    info.icon = KIcon("view-pim-contacts");
    //info.enabled = false;
    templateCategory.addTemplate(info);
    
    info = KexiTemplateInfo();
    info.name = "movie";
    info.caption = i18n("Movie catalog");
    info.description = i18n("Database for collecting movies");
    info.icon = KIcon("video-x-genenric");
    //info.enabled = false;
    templateCategory.addTemplate(info);
    templateCategories.append(templateCategory);
    
//     QStringList names;
//     names << "A" << "B" << "C";
    KexiTemplatesModel* model = new KexiTemplatesModel(templateCategories);
    proxyModel->setSourceModel(model);
    m_templatesList->setModel(proxyModel);
    m_templatesList->setSelectionModel(new KexiTemplatesSelectionModel(proxyModel));

    kDebug() << "templatesCategoryDrawer:" << m_templatesList->categoryDrawer() << (KCategoryDrawer*)templatesCategoryDrawer;

    setContents(m_templatesList);
}

void KexiTemplateSelectionPage::slotItemClicked(const QModelIndex& index)
{
    if (!index.isValid())
        return;
    selectedTemplate = index.data(KexiTemplatesModel::NameRole).toString();
    selectedCategory = index.data(KexiTemplatesModel::CategoryRole).toString();
    m_templatesList->clearSelection();

    //! @todo support templates
    if (selectedTemplate == "blank" && selectedCategory == "blank") {
        emit next(this);
        return;
    }
    KEXI_UNFINISHED(i18n("Templates"));
}

// ----

KexiProjectStorageTypeSelectionPage::KexiProjectStorageTypeSelectionPage(QWidget* parent)
 : KexiAssistantPage(i18n("Storage Method"),
                  i18n("Select a storage method which will be used to store the new project."),
                  parent)
 , m_fileTypeSelected(true)
{
    setBackButtonVisible(true);
    QWidget* contents = new QWidget;
    setupUi(contents);
    int dsize = KIconLoader::global()->currentSize(KIconLoader::Desktop);
    btn_file->setIcon(KIcon(KexiDB::defaultFileBasedDriverIcon()));
    btn_file->setIconSize(QSize(dsize, dsize));
    connect(btn_file, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    btn_server->setIcon(KIcon(KEXI_ICON_DATABASE_SERVER));
    btn_server->setIconSize(QSize(dsize, dsize));
    connect(btn_server, SIGNAL(clicked()), this, SLOT(buttonClicked()));

    setContents(contents);
}

KexiProjectStorageTypeSelectionPage::~KexiProjectStorageTypeSelectionPage()
{
}

void KexiProjectStorageTypeSelectionPage::buttonClicked()
{
    m_fileTypeSelected = sender() == btn_file;
    emit next(this);
}

// ----

KexiProjectTitleSelectionPage::KexiProjectTitleSelectionPage(QWidget* parent)
 : KexiAssistantPage(i18n("Project Title & Filename"),
                  i18n("Enter title for the new project. "
                       "Filename will created automatically based on the title. "
                       "You can change the filename too."),
//                  i18n("Enter title for the new project."),
                  parent)
{
    setBackButtonVisible(true);
    setNextButtonVisible(true);
    contents = new KexiDBTitlePage(QString());
    contents->formLayout->setSpacing(KDialog::spacingHint());
    contents->le_title->setText(i18n("New database"));
    contents->le_title->selectAll();
    connect(contents->le_title, SIGNAL(textChanged(QString)),
            this, SLOT(titleTextChanged(QString)));
    //GLUE_WIDGET(nextButton(), contents->nextButtonPlaceholder);
    fileHandler = new KexiStartupFileHandler(
        KUrl("kfiledialog:///OpenExistingOrCreateNewProject"),
        KexiStartupFileHandler::SavingFileBasedDB,
        contents->file_requester);
    connect(fileHandler, SIGNAL(askForOverwriting(KexiContextMessage)),
            this, SLOT(askForOverwriting(KexiContextMessage)));

    //KUrl url = KFileDialog::getStartUrl(
    //    KUrl("kfiledialog:///OpenExistingOrCreateNewProject"), m_recentDirClass);
    //contents->file_requester->fileDialog()->setOperationMode(KFileDialog::Saving);
    contents->file_requester->fileDialog()->setCaption(i18n("Save New Project As"));
    //contents->file_requester->setUrl(url);
    //KMimeType::Ptr mime = KMimeType::mimeType(KexiDB::defaultFileBasedDriverMimeType());
    //QString filter;
    /*if (mime) {
        filter = KexiUtils::fileDialogFilterString(mime);
        contents->file_requester->setFilter(filter);
    }*/
    //connect(contents->file_requester, SIGNAL(urlSelected(KUrl)),
    //        this, SLOT(urlSelected(KUrl)));
    updateUrl();

    setContents(contents);
}

KexiProjectTitleSelectionPage::~KexiProjectTitleSelectionPage()
{
    delete fileHandler;
}

void KexiProjectTitleSelectionPage::askForOverwriting(const KexiContextMessage& message)
{
    kDebug() << message.text();
    delete messageWidget;
    messageWidget = new KexiContextMessageWidget(this,
                                                 contents->formLayout,
                                                 contents->file_requester, message);
    messageWidget->setNextFocusWidget(contents->le_title);
}

void KexiProjectTitleSelectionPage::titleTextChanged(const QString & text)
{
    Q_UNUSED(text);
    updateUrl();
//    nextButton()->setEnabled(!text.trimmed().isEmpty());
}

void KexiProjectTitleSelectionPage::updateUrl()
{
    KUrl url = contents->file_requester->url();
    QString fn = KexiUtils::string2FileName(contents->le_title->text());
    if (!fn.isEmpty() && !fn.endsWith(".kexi"))
        fn += ".kexi";
    url.setFileName(fn);
    contents->file_requester->setUrl(url);
}

/*void KexiProjectTitleSelectionPage::urlSelected(const KUrl& url)
{
    KUrl dirUrl = url;
    dirUrl.setFileName(QString());
    if (dirUrl.isValid() && dirUrl.isLocalFile()) {
        KRecentDirs::add(m_recentDirClass, dirUrl.url());
    }
}*/

bool KexiProjectTitleSelectionPage::isAcceptable()
{
    delete messageWidget;
    if (contents->le_title->text().trimmed().isEmpty()) {
        messageWidget = new KexiContextMessageWidget(contents->formLayout,
                                                     contents->le_title,
                                                     i18n("Enter project title."));
        contents->le_title->setText(QString());
        return false;
    }
    KUrl url = contents->file_requester->url();
    if (!url.isValid() || !url.isLocalFile() || url.fileName().isEmpty()) {
        messageWidget = new KexiContextMessageWidget(contents->formLayout,
            contents->file_requester,
            i18n("Enter valid project filename. The file should be located on this computer."));
        return false;
    }
    if (!fileHandler->checkSelectedUrl()) {
        return false;
    }
    //urlSelected(url); // to save recent dir
    return true;
}

// ----

KexiProjectCreationPage::KexiProjectCreationPage(QWidget* parent)
 : KexiAssistantPage(i18n("Creating Project"),
                  i18n("Please wait while the project is created."),
                  parent)
{
     QHBoxLayout *lyr = new QHBoxLayout;
     progressBar = new QProgressBar;
     lyr->addWidget(progressBar);
//! @todo add cancel
     QVBoxLayout *vlyr = new QVBoxLayout;
     vlyr->addLayout(lyr);
     vlyr->addStretch(1);
     setContents(vlyr);
}

KexiProjectCreationPage::~KexiProjectCreationPage()
{
}

// ----

KexiAnimatedLayout::Private::Private(KexiAnimatedLayout * qq)
 : QWidget(), q(qq), animation(this, "pos")
{
    hide();
    setAttribute(Qt::WA_OpaquePaintEvent, true);
    animation.setEasingCurve(QEasingCurve::InOutQuart);
    animation.setDuration(500);
    connect(&animation, SIGNAL(finished()), this, SLOT(animationFinished()));
}

void KexiAnimatedLayout::Private::animateTo(QWidget* destination)
{
    QWidget *from = q->currentWidget();
    Q_ASSERT(destination && from);
    destinationWidget = destination;
    if (from == destinationWidget)
        return;
    if (!(KGlobalSettings::self()->graphicEffectsLevel()
          & KGlobalSettings::SimpleAnimationEffects))
    {
        // animations not allowed: switch to destination widget immediately
        animationFinished();
        return;
    }
    bool toRight = q->currentIndex() < q->indexOf(destinationWidget);
    hide();
    setParent(from);
    QSize s(from->parentWidget()->size());
    QPoint startPos(toRight ? QPoint(0, 0) : QPoint(s.width(), 0));
    QPoint endPos(toRight ? QPoint(s.width(), 0) : QPoint(0, 0));
    animation.setStartValue(-startPos);
    animation.setEndValue(-endPos);
    buffer = QPixmap(s.width() * 2, s.height());
    buffer.fill(Qt::white);
    from->render(&buffer, startPos);
    //kDebug() << s << from->geometry() << destinationWidget->geometry();
    destinationWidget->resize(from->size()); // needed because destination could
                                               // have been never displayed
    destinationWidget->render(&buffer, endPos);
    resize(buffer.size());
    move(animation.startValue().toPoint().x(), animation.startValue().toPoint().y());
    show();
    animation.start();
}

void KexiAnimatedLayout::Private::paintEvent(QPaintEvent* event)
{
    if (buffer.isNull())
        return;
    QPainter p(this);
    p.drawPixmap(event->rect(), buffer, event->rect());
}

void KexiAnimatedLayout::Private::animationFinished()
{
    if (destinationWidget) {
        static_cast<QStackedLayout*>(q)->setCurrentWidget(destinationWidget);
    }
    hide();
    destinationWidget = 0;
    buffer = QPixmap();
}

KexiAnimatedLayout::KexiAnimatedLayout(QWidget* parent)
 : QStackedLayout(parent)
 , d(new Private(this))
{
}

KexiAnimatedLayout::~KexiAnimatedLayout()
{
    delete d;
}

void KexiAnimatedLayout::setCurrentWidget(QWidget* widget)
{
    if (indexOf(widget) < 0) {
        return;
    }
    if (!currentWidget()) {
        QStackedLayout::setCurrentWidget(widget);
        return;
    }
    d->animateTo(widget);
}

void KexiAnimatedLayout::setCurrentIndex(int index)
{
    QWidget *w = widget(index);
    if (!w)
        return;
    
    setCurrentWidget(w);
}

// ----

class KexiNewProjectAssistant::Private
{
public:
    Private(KexiNewProjectAssistant *qq)
        : q(qq)
    {
    }
    
    ~Private()
    {
        //if (layoutSwitch)
        //    delete static_cast<KexiStackedLayoutSwitch*>(layoutSwitch);
    }
    
    KexiTemplateSelectionPage* templateSelectionPage() {
        return page<KexiTemplateSelectionPage>(&m_templateSelectionPage);
    }
    KexiProjectStorageTypeSelectionPage* projectStorageTypeSelectionPage() {
        return page<KexiProjectStorageTypeSelectionPage>(&m_projectStorageTypeSelectionPage);
    }
    KexiProjectTitleSelectionPage* titleSelectionPage() {
        return page<KexiProjectTitleSelectionPage>(&m_titleSelectionPage);
    }
    KexiProjectCreationPage* projectCreationPage() {
        return page<KexiProjectCreationPage>(&m_projectCreationPage);
    }
    
    KexiAnimatedLayout *lyr;
    //QPointer<KexiStackedLayoutSwitch> layoutSwitch;
private:
    void addPage(KexiAssistantPage* page) {
        lyr->addWidget(page);
        connect(page, SIGNAL(back(KexiAssistantPage*)), q, SLOT(previousPageRequested(KexiAssistantPage*)));
        connect(page, SIGNAL(next(KexiAssistantPage*)), q, SLOT(nextPageRequested(KexiAssistantPage*)));
    }

    template <class C>
    C* page(QPointer<C>* p) {
        if (!*p) {
            *p = new C;
            addPage(*p);
        }
        return *p;
    }

    KexiNewProjectAssistant* q;
    QPointer<KexiTemplateSelectionPage> m_templateSelectionPage;
    QPointer<KexiProjectStorageTypeSelectionPage> m_projectStorageTypeSelectionPage;
    QPointer<KexiProjectTitleSelectionPage> m_titleSelectionPage;
    QPointer<KexiProjectCreationPage> m_projectCreationPage;
};

// ----

KexiNewProjectAssistant::KexiNewProjectAssistant(QWidget* parent)
 : QWidget(parent)
 , d(new Private(this))
{
    QVBoxLayout *mainLyr = new QVBoxLayout(this);
    d->lyr = new KexiAnimatedLayout;
    mainLyr->addLayout(d->lyr);
    int margin = style()->pixelMetric(QStyle::PM_MenuPanelWidth, 0, 0)
        + KDialog::marginHint();
    mainLyr->setContentsMargins(margin, margin, margin, margin);

    setCurrentPage(d->templateSelectionPage());
    //d->layoutSwitch = new KexiStackedLayoutSwitch(d->lyr);
}

KexiNewProjectAssistant::~KexiNewProjectAssistant()
{
    delete d;
}
       
void KexiNewProjectAssistant::previousPageRequested(KexiAssistantPage* sender)
{
    if (sender == d->projectStorageTypeSelectionPage()) {
        d->lyr->setCurrentWidget(d->templateSelectionPage());
    }
    else if (sender == d->titleSelectionPage()) {
        d->lyr->setCurrentWidget(d->projectStorageTypeSelectionPage());
    }
}

void KexiNewProjectAssistant::nextPageRequested(KexiAssistantPage* sender)
{
    if (sender == d->templateSelectionPage()) {
        d->lyr->setCurrentWidget(d->projectStorageTypeSelectionPage());
#if 1
        d->titleSelectionPage()->contents->le_title->setFocus();
        d->lyr->setCurrentWidget(d->titleSelectionPage());
#endif
/*        d->slideWidget->setParent(d->lyr->currentWidget());
        d->slideWidget->move(100, 0);
        d->slideWidget->setup(page, d->projectStorageTypeSelectionPage());
        d->slideWidget->show();*/
        //setCurrentPage(d->projectStorageTypeSelectionPage());
    }
    else if (sender == d->projectStorageTypeSelectionPage()) {
        d->titleSelectionPage()->contents->le_title->setFocus();
        d->lyr->setCurrentWidget(d->titleSelectionPage());
    }
    else if (sender == d->titleSelectionPage()) {
        if (!d->titleSelectionPage()->isAcceptable()) {
            //d->titleSelectionPage()->messageWidget->fadeIn();
            return;
        }
        d->lyr->setCurrentWidget(d->projectCreationPage());
    }
}
    
void KexiNewProjectAssistant::setCurrentPage(KexiAssistantPage* page)
{
    d->lyr->setCurrentWidget(page);
}

#ifdef OLD_KexiNewProjectWizard

//! @internal
class KexiNewProjectAssistant::Private
{
public:
    Private() {
        le_dbname_txtchanged_disable = false;
        le_dbname_autofill = true;
//  conndata_to_show = 0;
//  project_set_to_show = 0;
    }
    ~Private() {
//  delete conndata_to_show;
//  delete project_set_to_show;
        delete msgHandler;
    }
// K3ListView *lv_types;
    K3ListViewItem *lvi_file, *lvi_server;
    QString chk_file_txt, chk_server_txt; //!< helper

    QString server_db_name_dblist_lbl_txt; //!< helper

    //for displaying db list of the selected conn.
    QPointer<KexiDB::ConnectionData> conndata_to_show;
    KexiProjectSet *project_set_to_show;

    KexiGUIMessageHandler* msgHandler;

    bool le_dbname_txtchanged_disable;
    bool le_dbname_autofill;
};

KexiNewProjectAssistant::KexiNewProjectAssistant(KexiDBConnectionSet& conn_set,
        QWidget *parent)
        : K3Wizard(parent)
        , d(new Private())
{
    d->msgHandler = new KexiGUIMessageHandler(this);
    setWindowIcon(KIcon("document-new"));
    setWindowTitle(i18n("Creating New Project"));
    finishButton()->setText(i18n("Create"));

    //page: type selector
    m_prjtype_sel = new KexiNewPrjTypeSelector(this);
    m_prjtype_sel->setObjectName("KexiNewPrjTypeSelector");
// lv_types = new K3ListView(m_prjtype_sel, "types listview");
// m_prjtype_sel->lv_types->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum, 0, 2));
    m_prjtype_sel->lv_types->setShadeSortColumn(false);
    m_prjtype_sel->lv_types->header()->hide();
    m_prjtype_sel->lv_types->setSorting(-1);
    m_prjtype_sel->lv_types->setAlternateBackground(QColor()); //disable altering
    m_prjtype_sel->lv_types->setItemMargin(KDialog::marginHint());
    QString none;
    d->lvi_file = new K3ListViewItem(
        m_prjtype_sel->lv_types, i18n("New Project Stored in File"));
    d->lvi_file->setPixmap(0, DesktopIcon(KexiDB::defaultFileBasedDriverIcon()));
    d->lvi_file->setMultiLinesEnabled(true);
    d->lvi_server = new K3ListViewItem(m_prjtype_sel->lv_types, d->lvi_file,
                                       i18n("New Project Stored on Database Server"));
    d->lvi_server->setPixmap(0, DesktopIcon(KEXI_ICON_DATABASE_SERVER));
    d->lvi_server->setMultiLinesEnabled(true);
// m_prjtype_sel->lv_types->resize(d->m_prjtype_sel->lv_types->width(), d->lvi_file->height()*3);
    m_prjtype_sel->lv_types->setFocus();
// QString txt_dns = i18n("Don't show me this question again.");
// d->chk_file_txt = m_prjtype_sel->chk_always->text() +"\n"+txt_dns;
// d->chk_server_txt = i18n("Always &use database server for creating new projects.")
//  +"\n"+txt_dns;

    connect(m_prjtype_sel->lv_types, SIGNAL(executed(Q3ListViewItem*)),
            this, SLOT(slotLvTypesExecuted(Q3ListViewItem*)));
    connect(m_prjtype_sel->lv_types, SIGNAL(returnPressed(Q3ListViewItem*)),
            this, SLOT(slotLvTypesExecuted(Q3ListViewItem*)));
    connect(m_prjtype_sel->lv_types, SIGNAL(selectionChanged(Q3ListViewItem*)),
            this, SLOT(slotLvTypesSelected(Q3ListViewItem*)));

// static_cast<QVBoxLayout*>(m_prjtype_sel->layout())->insertWidget(1,d->m_prjtype_sel->lv_types);
// static_cast<QVBoxLayout*>(m_prjtype_sel->layout())->insertStretch(3,1);
// updateGeometry();

    addPage(m_prjtype_sel, i18n("Select Storage Method"));
// d->m_prjtype_sel->lv_types->setMinimumHeight(qMax(d->lvi_file->height(),d->lvi_server->height())+25);

    //page: db title
    m_db_title = new KexiDBTitlePage(QString(), this);
    m_db_title->setObjectName("KexiDBTitlePage");
    addPage(m_db_title, i18n("Select Project's Caption"));

    //page: connection selector
    m_conn_sel_widget = new QWidget(this);
    QVBoxLayout* conn_sel_lyr = new QVBoxLayout(m_conn_sel_widget);
    QLabel *conn_sel_label = new QLabel(i18n("Enter a new Kexi project's file name:"));
    conn_sel_label->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    conn_sel_label->setWordWrap(true);
    conn_sel_lyr->addWidget(conn_sel_label);
    conn_sel_lyr->addSpacing(KDialog::spacingHint());

    m_conn_sel = new KexiConnSelectorWidget(conn_set,
                                            "kfiledialog:///OpenExistingOrCreateNewProject",
                                            KAbstractFileWidget::Saving, m_conn_sel_widget);
    conn_sel_lyr->addWidget(m_conn_sel);

    //"Select database server connection"
// m_conn_sel->m_file->btn_advanced->hide();
// m_conn_sel->m_file->label->hide();
//TODO m_conn_sel->m_file->lbl->setText( i18n("Enter a new Kexi project's file name:") );
    m_conn_sel->hideHelpers();

    m_conn_sel->m_remote->label->setText(
        i18n("Select database server's connection you wish to use to create a new Kexi project. "
             "<p>Here you may also add, edit or remove connections from the list."));
// m_conn_sel->m_remote->label_back->hide();
// m_conn_sel->m_remote->btn_back->hide();

    m_conn_sel->showSimpleConn();
    //anyway, db files will be _saved_
    m_conn_sel->fileWidget->setMode(KexiStartupFileWidget::SavingFileBasedDB);
#ifdef __GNUC__
#warning TODO KFileWidget connect(m_conn_sel->m_fileDlg,SIGNAL(accepted()),this,SLOT(accept()));
#else
#pragma WARNING( TODO KFileWidget connect(m_conn_sel->m_fileDlg,SIGNAL(accepted()),this,SLOT(accept())); )
#endif
    m_conn_sel->showAdvancedConn();
    connect(m_conn_sel, SIGNAL(connectionItemExecuted(ConnectionDataLVItem*)),
            this, SLOT(next()));

    addPage(m_conn_sel_widget, i18n("Select Project's Location"));

    //page: server db name
    m_server_db_name = new KexiServerDBNamePage(this);
    m_server_db_name->setObjectName("KexiServerDBNamePage");
    d->server_db_name_dblist_lbl_txt
        = I18N_NOOP("Existing project databases on <b>%1</b> database server:");
    connect(m_server_db_name->le_caption, SIGNAL(textChanged(const QString&)),
            this, SLOT(slotServerDBCaptionTxtChanged(const QString&)));
    connect(m_server_db_name->le_dbname, SIGNAL(textChanged(const QString&)),
            this, SLOT(slotServerDBNameTxtChanged(const QString&)));
    connect(m_server_db_name->le_caption, SIGNAL(returnPressed()),
            this, SLOT(accept()));
    connect(m_server_db_name->le_dbname, SIGNAL(returnPressed()),
            this, SLOT(accept()));
    m_server_db_name->le_caption->setText(i18n("New database"));
    m_server_db_name->le_dbname->setValidator(new KexiUtils::IdentifierValidator(this));
    m_project_selector = new KexiProjectSelectorWidget(
        m_server_db_name->frm_dblist, 0, false, false);
    GLUE_WIDGET(m_project_selector, m_server_db_name->frm_dblist);
    m_project_selector->setFocusPolicy(Qt::NoFocus);
    m_project_selector->setSelectable(false);

    addPage(m_server_db_name, i18n("Select Project's Caption & Database Name"));

    setFinishEnabled(m_prjtype_sel, false);
    setFinishEnabled(m_db_title, false);
    setFinishEnabled(m_server_db_name, true);

    //finish:
    updateGeometry();
    m_prjtype_sel->lv_types->setSelected(d->lvi_file, true);
}

KexiNewProjectAssistant::~KexiNewProjectAssistant()
{
    delete d;
}

void KexiNewProjectAssistant::show()
{
    KDialog::centerOnScreen(this);
    K3Wizard::show();
}

void KexiNewProjectAssistant::slotLvTypesExecuted(Q3ListViewItem *)
{
    next();
}

void KexiNewProjectAssistant::slotLvTypesSelected(Q3ListViewItem *item)
{
    /* if (item==d->lvi_file) {
        m_prjtype_sel->chk_always->setText(d->chk_file_txt);
      }
      else if (item==d->lvi_server) {
        m_prjtype_sel->chk_always->setText(d->chk_server_txt);
      }*/
    setAppropriate(m_db_title, item == d->lvi_file);
    setAppropriate(m_server_db_name, item == d->lvi_server);
}

void KexiNewProjectAssistant::showPage(QWidget *page)
{
    if (page == m_prjtype_sel) {//p 1
        m_prjtype_sel->lv_types->setFocus();
        m_prjtype_sel->lv_types->setCurrentItem(m_prjtype_sel->lv_types->currentItem());
    } else if (page == m_db_title) {//p 2
        if (m_db_title->le_caption->text().trimmed().isEmpty())
            m_db_title->le_caption->setText(i18n("New database"));
        m_db_title->le_caption->selectAll();
        m_db_title->le_caption->setFocus();
    } else if (page == m_conn_sel_widget) {//p 3
        if (m_prjtype_sel->lv_types->currentItem() == d->lvi_file) {
            m_conn_sel->showSimpleConn();
            QString fn = KexiUtils::string2FileName(m_db_title->le_caption->text());
            if (!fn.endsWith(".kexi"))
                fn += ".kexi";
//   m_conn_sel->fileWidget->setLocationText( fn );
            m_conn_sel->fileWidget->setSelection(fn);
            setFinishEnabled(m_conn_sel_widget, true);
            m_conn_sel->setFocus();
        } else {
            m_conn_sel->showAdvancedConn();
            setFinishEnabled(m_conn_sel_widget, false);
            m_conn_sel->setFocus();
            m_server_db_name->le_caption->selectAll();
        }
    } else if (page == m_server_db_name) {
        if (m_conn_sel->selectedConnectionData()
                && (static_cast<KexiDB::ConnectionData*>(d->conndata_to_show) != m_conn_sel->selectedConnectionData())) {
            m_project_selector->setProjectSet(0);
//   delete d->project_set_to_show;
            d->conndata_to_show = 0;
            d->project_set_to_show = new KexiProjectSet(*m_conn_sel->selectedConnectionData(), d->msgHandler);
            if (d->project_set_to_show->error()) {
                delete d->project_set_to_show;
                d->project_set_to_show = 0;
                return;
            }
            d->conndata_to_show = m_conn_sel->selectedConnectionData();
            //-refresh projects list
            m_project_selector->setProjectSet(d->project_set_to_show);
        }
    }
    K3Wizard::showPage(page);
}

void KexiNewProjectAssistant::next()
{
    //let's check if move to next page is allowed:
    if (currentPage() == m_db_title) { //pg 2
        if (m_db_title->le_caption->text().trimmed().isEmpty()) {
            KMessageBox::information(this, i18n("Enter project caption."));
            m_db_title->le_caption->setText(QString());
            m_db_title->le_caption->setFocus();
            return;
        }
    } else if (currentPage() == m_conn_sel_widget) {//p 3
        if (m_prjtype_sel->lv_types->currentItem() == d->lvi_file) {
            //test for db file selection
        } else {
            //test for db conn selection
            if (!m_conn_sel->selectedConnectionData()) {
                KMessageBox::information(this, i18n("Select server connection for a new project."));
                return;
            }
            m_project_selector->label()->setText(
                d->server_db_name_dblist_lbl_txt
                .arg(m_conn_sel->selectedConnectionData()->serverInfoString(false)));
            m_server_db_name->le_caption->setFocus();

        }
    }
    K3Wizard::next();
}

void KexiNewProjectAssistant::accept()
{
    if (m_prjtype_sel->lv_types->currentItem() == d->lvi_file) {//FILE:
        //check if new db file name is ok
        kDebug() << "********** sender() " << sender()->metaObject()->className();
        if (sender() == finishButton()) { /*(only if signal does not come from filedialog)*/
            kDebug() << "********** sender()==finishButton() ********";
            if (!m_conn_sel->fileWidget->checkSelectedFile()) {
                return;
            }
        }
    } else {//SERVER:
        //check if we have enough of data
        if (m_server_db_name->le_caption->text().trimmed().isEmpty()) {
            KMessageBox::information(this, i18n("Enter project caption."));
            m_server_db_name->le_caption->setText("");
            m_server_db_name->le_caption->setFocus();
            return;
        }
        QString dbname = m_server_db_name->le_dbname->text().trimmed();
        if (dbname.isEmpty()) {
            KMessageBox::information(this, i18n("Enter project's database name."));
            m_server_db_name->le_dbname->setText(QString());
            m_server_db_name->le_dbname->setFocus();
            return;
        }
        //check for duplicated dbname
        if (m_conn_sel->confirmOverwrites() && m_project_selector->projectSet() && m_project_selector->projectSet()
                ->findProject(m_server_db_name->le_dbname->text())) {
            if (KMessageBox::Continue != KMessageBox::warningContinueCancel(this,
                    "<qt>" + i18n("<b>A project with database name \"%1\" already exists</b>"
                                  "<p>Do you want to delete it and create a new one?",
                                  m_server_db_name->le_dbname->text()), QString(),
                    KStandardGuiItem::del(), KStandardGuiItem::cancel(),
                    QString(), KMessageBox::Notify | KMessageBox::Dangerous)) {
                m_server_db_name->le_dbname->setFocus();
                return;
            }
        }
    }

    K3Wizard::accept();
}

void KexiNewProjectAssistant::done(int r)
{
    /* //save state (always, no matter if dialog is accepted or not)
      KGlobal::config()->setGroup("Startup");
      if (!m_prjtype_sel->chk_always->isChecked())
        KGlobal::config()->deleteEntry("DefaultStorageForNewProjects");
      else if (m_prjtype_sel->lv_types->currentItem()==d->lvi_file)
        KGlobal::config()->writeEntry("DefaultStorageForNewProjects","File");
      else
        KGlobal::config()->writeEntry("DefaultStorageForNewProjects","Server");*/

    KGlobal::config()->sync();
    K3Wizard::done(r);
}

QString KexiNewProjectAssistant::projectDBName() const
{
    if (m_prjtype_sel->lv_types->currentItem() == d->lvi_server)
        return m_server_db_name->le_dbname->text();
    return m_conn_sel->selectedFileName();
}

QString KexiNewProjectAssistant::projectCaption() const
{
    if (m_prjtype_sel->lv_types->currentItem() == d->lvi_server) {
        return m_server_db_name->le_caption->text();
    }
    return m_db_title->le_caption->text();
}

KexiDB::ConnectionData* KexiNewProjectAssistant::projectConnectionData() const
{
    if (m_prjtype_sel->lv_types->currentItem() == d->lvi_file)
        return 0;
    return m_conn_sel->selectedConnectionData();
}

void KexiNewProjectAssistant::slotServerDBCaptionTxtChanged(const QString &capt)
{
    if (m_server_db_name->le_dbname->text().isEmpty())
        d->le_dbname_autofill = true;
    if (d->le_dbname_autofill) {
        d->le_dbname_txtchanged_disable = true;
        QString captionAsId = KexiUtils::string2Identifier(capt);
        m_server_db_name->le_dbname->setText(captionAsId);
        d->le_dbname_txtchanged_disable = false;
    }
}

void KexiNewProjectAssistant::slotServerDBNameTxtChanged(const QString &)
{
    if (d->le_dbname_txtchanged_disable)
        return;
    d->le_dbname_autofill = false;
}

/*! If true, user will be asked to accept overwriting existing file.
 This is true by default. */
void KexiNewProjectAssistant::setConfirmOverwrites(bool set)
{
    m_conn_sel->setConfirmOverwrites(set);
}

#endif //OLD_KexiNewProjectWizard

#include "KexiNewProjectAssistant.moc"
