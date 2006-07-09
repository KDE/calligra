/* This file is part of the KDE project
   Copyright (C) 2003 Peter Simonsson <psn@linux.se>

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
   Boston, MA 02110-1301, USA.
*/

#include "kiviooptionsdialog.h"
#include "kivio_view.h"
#include "kivio_doc.h"
#include "kivio_page.h"
#include "kivioglobal.h"
#include "kivio_grid_data.h"
#include "kivio_canvas.h"
#include "kivio_settings.h"
#include "kivio_config.h"

#include <klocale.h>
#include <KoApplication.h>
#include <kiconloader.h>
#include <kpushbutton.h>
#include <KoPageLayoutDia.h>
#include <kurlrequester.h>
#include <kcolorbutton.h>
#include <KoUnitWidgets.h>
#include <kglobal.h>
#include <kdebug.h>
#include <kfontdialog.h>

#include <qlabel.h>
#include <qbuttongroup.h>
#include <qgroupbox.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qtabwidget.h>

// GuidesListViewItem::GuidesListViewItem(QListView* parent, KivioGuideLineData *gd, KoUnit::Unit u)
// : KListViewItem(parent), m_data(gd)
// {
//   setPixmap(0, BarIcon(m_data->orientation() == Qt::Vertical ?
//     "guides_vertical":"guides_horizontal"));
//   QString s = KGlobal::_locale->formatNumber(KoUnit::toUserValue(m_data->position(), u), 2);
//   s += " " + KoUnit::unitName(u);
//   setText(1, s);
// }
// 
// GuidesListViewItem::~GuidesListViewItem()
// {
//   delete m_data;
// }
// 
// void GuidesListViewItem::setUnit(KoUnit::Unit u)
// {
//   QString s = KGlobal::_locale->formatNumber(KoUnit::toUserValue(m_data->position(), u), 2);
//   s += " " + KoUnit::unitName(u);
//   setText(1, s);
// }
// 
// void GuidesListViewItem::setPosition(double p, KoUnit::Unit u)
// {
//   m_data->setPosition(KoUnit::fromUserValue(p, u));
//   QString s = KGlobal::_locale->formatNumber(p, 2);
//   s += " " + KoUnit::unitName(u);
//   setText(1, s);
// }
// 
// void GuidesListViewItem::setOrientation(Qt::Orientation o)
// {
//   m_data->setOrientation(o);
//   setPixmap(0, BarIcon(m_data->orientation() == Qt::Vertical ?
//     "guides_vertical":"guides_horizontal"));
// }

/*****************************************************************************/

KivioOptionsDialog::KivioOptionsDialog(KivioView* parent, const char* name)
  : KDialogBase(IconList, i18n("Settings"), Ok|Cancel|Apply|Default, Ok, parent, name)
{
  initPage();
  initGrid();
  unitChanged(parent->doc()->unit());
}

void KivioOptionsDialog::initPage()
{
  QFrame* page = addPage(i18n("Page"), i18n("Page Settings"),
    kapp->iconLoader()->loadIcon("empty", KIcon::Toolbar, 32));
  m_pageIndex = pageIndex(page);

  KivioView* view = static_cast<KivioView*>(parent());
  KoUnit::Unit unit = KoUnit::unit(Kivio::Config::unit());
  m_layout = Kivio::Config::defaultPageLayout();
  m_font = Kivio::Config::font();

  QLabel* unitLbl = new QLabel(i18n("Default &units:"), page);
  m_unitCombo = new QComboBox(page);
  m_unitCombo->insertStringList(KoUnit::listOfUnitName());
  m_unitCombo->setCurrentItem(unit);
  unitLbl->setBuddy(m_unitCombo);
  QLabel* layoutLbl = new QLabel(i18n("Default layout:"), page);
  m_layoutTxtLbl = new QLabel(page);
  m_layoutTxtLbl->setFrameStyle(QFrame::LineEditPanel | QFrame::Sunken);
  m_layoutTxtLbl->setSizePolicy(QSizePolicy(
    QSizePolicy::Minimum, QSizePolicy::Fixed));
  setLayoutText(m_layout);
  KPushButton* layoutBtn = new KPushButton(i18n("Change..."), page);
  layoutBtn->setSizePolicy(QSizePolicy(
    QSizePolicy::Fixed, QSizePolicy::Fixed));
  QLabel* fontLbl = new QLabel(i18n("Default font:"), page);
  m_fontTxtLbl = new QLabel(page);
  m_fontTxtLbl->setFrameStyle(QFrame::LineEditPanel | QFrame::Sunken);
  m_fontTxtLbl->setSizePolicy(QSizePolicy(
    QSizePolicy::Minimum, QSizePolicy::Fixed));
  setFontText(m_font);
  KPushButton* fontBtn = new KPushButton(i18n("Change..."), page);
  layoutBtn->setSizePolicy(QSizePolicy(
    QSizePolicy::Fixed, QSizePolicy::Fixed));
  m_marginsChBox = new QCheckBox(i18n("Show page &margins"), page);
  m_marginsChBox->setChecked(view->isShowPageMargins());
  m_rulersChBox = new QCheckBox(i18n("Show page &rulers"), page);
  m_rulersChBox->setChecked(view->isShowRulers());

  QGridLayout* gl = new QGridLayout(page);
  gl->setSpacing(KDialog::spacingHint());
  gl->addWidget(unitLbl, 0, 0);
  gl->addMultiCellWidget(m_unitCombo, 0, 0, 1, 2);
  gl->addWidget(layoutLbl, 1, 0);
  gl->addWidget(m_layoutTxtLbl, 1, 1);
  gl->addWidget(layoutBtn, 1, 2);
  gl->addWidget(fontLbl, 2, 0);
  gl->addWidget(m_fontTxtLbl, 2, 1);
  gl->addWidget(fontBtn, 2, 2);
  gl->addMultiCellWidget(m_marginsChBox, 3, 3, 0, 2);
  gl->addMultiCellWidget(m_rulersChBox, 4, 4, 0, 2);
  gl->addMultiCell(new QSpacerItem(0, 0), 5, 5, 0, 2);

  connect(layoutBtn, SIGNAL(clicked()), SLOT(pageLayoutDlg()));
  connect(fontBtn, SIGNAL(clicked()), SLOT(fontDlg()));
  connect(m_unitCombo, SIGNAL(activated(int)), SLOT(unitChanged(int)));
}

void KivioOptionsDialog::initGrid()
{
  QFrame* page = addPage(i18n("Grid"), i18n("Grid Settings"), BarIcon( "grid", KIcon::SizeMedium ));
  m_gridIndex = pageIndex(page);

  KoUnit::Unit unit = static_cast<KivioView*>(parent())->doc()->unit();
  KivioGridData d = static_cast<KivioView*>(parent())->doc()->grid();
  double pgw = m_layout.ptWidth;
  double pgh = m_layout.ptHeight;
  double fw = Kivio::Config::gridXSpacing();
  double fh = Kivio::Config::gridYSpacing();
  double sw = Kivio::Config::gridXSnap();
  double sh = Kivio::Config::gridYSnap();

  m_gridChBox = new QCheckBox(i18n("Show &grid"), page);
  m_gridChBox->setChecked(Kivio::Config::showGrid());
  m_snapChBox = new QCheckBox(i18n("Snap to g&rid"), page);
  m_snapChBox->setChecked(Kivio::Config::snapGrid());
  QLabel* gridColorLbl = new QLabel(i18n("Grid &color:"), page);
  m_gridColorBtn = new KColorButton(Kivio::Config::gridColor(), page);
  gridColorLbl->setBuddy(m_gridColorBtn);
  QGroupBox* spacingGrp = new QGroupBox(2, Qt::Horizontal, i18n("Spacing"), page);
  QLabel* spaceHorizLbl = new QLabel(i18n("&Horizontal:"), spacingGrp);
  m_spaceHorizUSpin = new KoUnitDoubleSpinBox(spacingGrp, 0.0, pgw, 0.1,
    fw, unit);
  spaceHorizLbl->setBuddy(m_spaceHorizUSpin);
  QLabel* spaceVertLbl = new QLabel(i18n("&Vertical:"), spacingGrp);
  m_spaceVertUSpin = new KoUnitDoubleSpinBox(spacingGrp, 0.0, pgh, 0.1,
    fh, unit);
  spaceVertLbl->setBuddy(m_spaceVertUSpin);
  QGroupBox* snapGrp = new QGroupBox(2, Qt::Horizontal, i18n("Snap Distance"), page);
  QLabel* snapHorizLbl = new QLabel(i18n("H&orizontal:"), snapGrp);
  m_snapHorizUSpin = new KoUnitDoubleSpinBox(snapGrp, 0.0, fw, 0.1,
    sw, unit);
  snapHorizLbl->setBuddy(m_snapHorizUSpin);
  QLabel* snapVertLbl = new QLabel(i18n("V&ertical:"), snapGrp);
  m_snapVertUSpin = new KoUnitDoubleSpinBox(snapGrp, 0.0, fh, 0.1,
    sh, unit);
  snapVertLbl->setBuddy(m_snapVertUSpin);

  QGridLayout* gl = new QGridLayout(page);
  gl->setSpacing(KDialog::spacingHint());
  gl->addMultiCellWidget(m_gridChBox, 0, 0, 0, 2);
  gl->addMultiCellWidget(m_snapChBox, 1, 1, 0, 2);
  gl->addWidget(gridColorLbl, 2, 0);
  gl->addWidget(m_gridColorBtn, 2, 1);
  gl->addItem(new QSpacerItem(0, 0), 2, 2);
  gl->addMultiCellWidget(spacingGrp, 3, 3, 0, 2);
  gl->addMultiCellWidget(snapGrp, 4, 4, 0, 2);
  gl->addMultiCell(new QSpacerItem(0, 0), 5, 5, 0, 2);

  connect(m_spaceHorizUSpin, SIGNAL(valueChanged(double)), SLOT(setMaxHorizSnap(double)));
  connect(m_spaceVertUSpin, SIGNAL(valueChanged(double)), SLOT(setMaxVertSnap(double)));
}

void KivioOptionsDialog::applyPage()
{
  KivioView* view = static_cast<KivioView*>(parent());
  view->doc()->setUnit(static_cast<KoUnit::Unit>(m_unitCombo->currentItem()));
  Kivio::Config::setUnit(KoUnit::unitName(view->doc()->unit()));
  Kivio::Config::setDefaultPageLayout(m_layout);
  Kivio::Config::setFont(m_font);
  view->doc()->setDefaultFont(m_font);
  view->togglePageMargins(m_marginsChBox->isChecked());
  view->toggleShowRulers(m_rulersChBox->isChecked());
}

void KivioOptionsDialog::applyGrid()
{
  KivioGridData d;
  Kivio::Config::setGridXSpacing(m_spaceHorizUSpin->value());
  Kivio::Config::setGridYSpacing(m_spaceVertUSpin->value());
  Kivio::Config::setGridXSnap(m_snapHorizUSpin->value());
  Kivio::Config::setGridYSnap(m_snapVertUSpin->value());
  Kivio::Config::setShowGrid(m_gridChBox->isChecked());
  Kivio::Config::setSnapGrid(m_snapChBox->isChecked());
  Kivio::Config::setGridColor(m_gridColorBtn->color());
  KivioView* view = static_cast<KivioView*>(parent());
  view->doc()->updateView(0);
}

void KivioOptionsDialog::defaultPage()
{
  m_layout = Kivio::Config::defaultPageLayout();
  m_font = Kivio::Config::font();
  m_unitCombo->setCurrentItem(KoUnit::unit(Kivio::Config::unit()));
  unitChanged(m_unitCombo->currentItem());
  setLayoutText(m_layout);
  setFontText(m_font);
  m_marginsChBox->setChecked(true);
  m_rulersChBox->setChecked(true);
}

void KivioOptionsDialog::defaultGrid()
{
  m_spaceHorizUSpin->changeValue(Kivio::Config::gridXSpacing());
  m_spaceVertUSpin->changeValue(Kivio::Config::gridYSpacing());
  m_snapHorizUSpin->changeValue(Kivio::Config::gridXSnap());
  m_snapVertUSpin->changeValue(Kivio::Config::gridYSnap());
  m_gridChBox->setChecked(Kivio::Config::showGrid());
  m_snapChBox->setChecked(Kivio::Config::snapGrid());
  m_gridColorBtn->setColor(Kivio::Config::gridColor());
}

void KivioOptionsDialog::setLayoutText(const KoPageLayout& l)
{
  KoUnit::Unit unit = static_cast<KoUnit::Unit>(m_unitCombo->currentItem());
  QString txt = i18n("Format: %1, Width: %2 %4, Height: %3 %5").arg(
    KoPageFormat::formatString(l.format)).arg(KoUnit::toUserValue(l.ptWidth, unit))
    .arg(KoUnit::toUserValue(l.ptHeight, unit)).arg(KoUnit::unitName(unit)).arg(
    KoUnit::unitName(unit));
  m_layoutTxtLbl->setText(txt);
}

void KivioOptionsDialog::pageLayoutDlg()
{
  KoHeadFoot headfoot;
  int tabs = FORMAT_AND_BORDERS | DISABLE_UNIT;
  KoUnit::Unit unit = static_cast<KoUnit::Unit>(m_unitCombo->currentItem());

  if(KoPageLayoutDia::pageLayout(m_layout, headfoot, tabs, unit))
  {
    setLayoutText(m_layout);
  }
}

void KivioOptionsDialog::unitChanged(int u)
{
  KoUnit::Unit unit = static_cast<KoUnit::Unit>(u);
  setLayoutText(m_layout);
  m_snapHorizUSpin->setUnit(unit);
  m_snapVertUSpin->setUnit(unit);
  m_spaceHorizUSpin->setUnit(unit);
  m_spaceVertUSpin->setUnit(unit);
}

void KivioOptionsDialog::slotOk()
{
  slotApply();
  accept();
}

void KivioOptionsDialog::slotApply()
{
  applyPage();
  applyGrid();
  Kivio::Config::self()->writeConfig();
}

void KivioOptionsDialog::slotDefault()
{
  bool defaults = Kivio::Config::self()->useDefaults(true);
  defaultPage();
  defaultGrid();
  Kivio::Config::self()->useDefaults(defaults);
}

void KivioOptionsDialog::setMaxHorizSnap(double v)
{
  KoUnit::Unit unit = static_cast<KoUnit::Unit>(m_unitCombo->currentItem());
  m_snapHorizUSpin->setMaxValue(KoUnit::fromUserValue(v, unit));
}

void KivioOptionsDialog::setMaxVertSnap(double v)
{
  KoUnit::Unit unit = static_cast<KoUnit::Unit>(m_unitCombo->currentItem());
  m_snapVertUSpin->setMaxValue(KoUnit::fromUserValue(v, unit));
}

void KivioOptionsDialog::setFontText(const QFont& f)
{
  QString txt = f.family() + " " + QString::number(f.pointSize());
  m_fontTxtLbl->setText(txt);
}

void KivioOptionsDialog::fontDlg()
{
  if(KFontDialog::getFont(m_font, false, this) == QDialog::Accepted) {
    setFontText(m_font);
  }
}

#include "kiviooptionsdialog.moc"
