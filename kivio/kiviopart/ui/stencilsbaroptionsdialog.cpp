#include "stencilsbaroptionsdialog.h"
#include "kivio_view.h"
#include "kivio_doc.h"
#include "kivio_icon_view.h"
#include "kivio_config.h"

#include <qlabel.h>
#include <qlineedit.h>
#include <qradiobutton.h>
#include <qdir.h>
#include <qfileinfo.h>

#include <kcolorbtn.h>
#include <kfiledialog.h>
#include <kimageio.h>
#include <kmessagebox.h>
#include <kdebug.h>

Preview::Preview(QWidget *parent)
: QScrollView(parent)
{
  viewport()->setBackgroundMode(PaletteBase);
}

void Preview::showPreview(const KURL& url)
{
  if (url.isLocalFile()) {
    QString path = url.path();
    pixmap = QPixmap(path);
  kdDebug() << path <<endl;
	} else
  	pixmap = QPixmap();

	resizeContents(pixmap.size().width(), pixmap.size().height());
	viewport()->repaint();

  kdDebug() << pixmap.size().width() <<endl;
}

void Preview::drawContents(QPainter* p, int, int, int, int)
{
  p->drawPixmap(0, 0, pixmap);
}
/*******************************************************************************************************/
StencilsBarOptionsDialog::StencilsBarOptionsDialog(KivioView* view, QWidget* parent, const char* name)
: StencilsBarOptionsDialogBase(parent, name)
{
  m_pView = view;
  KivioIconViewVisual v = m_pView->doc()->config()->stencilsBarVisual();

  preview->setText("");
  color->setColor(v.color);
  pixmapName->setText(v.pixmapFileName);

  usePixmap->setChecked(v.usePixmap);
  useColor->setChecked(!v.usePixmap);

  updateEnabledState();
}

StencilsBarOptionsDialog::~StencilsBarOptionsDialog()
{
}

void StencilsBarOptionsDialog::selectSource()
{
  QRadioButton* r = (QRadioButton*)sender();
  if (r == usePixmap) {
    useColor->setChecked(!usePixmap->isChecked());
  } else {
    usePixmap->setChecked(!useColor->isChecked());
  }
  updateEnabledState();
}

void StencilsBarOptionsDialog::updateEnabledState()
{
  color->setEnabled(useColor->isChecked());
  pixmapName->setEnabled(usePixmap->isChecked());
  openButton->setEnabled(usePixmap->isChecked());
}

void StencilsBarOptionsDialog::slotOpenButton()
{
  static QString dir = QDir::homeDirPath();

  KFileDialog fd(dir, KImageIO::pattern(KImageIO::Reading), 0, 0, true);
  fd.setCaption(i18n("Select Background Pixmap"));
  //fd.setPreviewMode( FALSE, TRUE );
  fd.setPreviewWidget(new Preview(&fd));
  //fd.setViewMode( QFileDialog::ListView | QFileDialog::PreviewContents );
  KURL url;
  if ( fd.exec() == QDialog::Accepted )
    url = fd.selectedURL();

  if(url.isEmpty())
    return;

  if(!url.isLocalFile()) {
    KMessageBox::sorry( 0L, i18n( "Only local files supported yet." ) );
    return;
  }

  QString file = url.path();
  pixmapName->setText(file);
  dir = QFileInfo(file).dirPath();
}

void StencilsBarOptionsDialog::updatePreview()
{
  preview->setPixmap(QPixmap(pixmapName->text()));
}

void StencilsBarOptionsDialog::apply(QWidget* page)
{
  if (page != this)
    return;

  KivioIconViewVisual v;
  v.color =  color->color();
  v.pixmapFileName = pixmapName->text();
  v.usePixmap = usePixmap->isChecked();
  m_pView->doc()->config()->setGlobalStencilsBarVisual(v);
}

#include "stencilsbaroptionsdialog.moc"
