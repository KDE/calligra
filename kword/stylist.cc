/******************************************************************/ 
/* KWord - (c) by Reginald Stadlbauer and Torben Weis 1997-1998   */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer, Torben Weis                       */
/* E-Mail: reggie@kde.org, weis@kde.org                           */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Stylist                                                */
/******************************************************************/

#include "kword_doc.h"
#include "paraglayout.h"
#include "format.h"
#include "font.h"

#include "stylist.h"
#include "stylist.moc"

/******************************************************************/
/* Class: KWStyleManager                                          */
/******************************************************************/

/*================================================================*/
KWStyleManager::KWStyleManager(QWidget *_parent,KWordDocument *_doc)
  : QTabDialog(_parent,"",true)
{
  doc = _doc;
  editor = 0L;

  setupTab1();

  setCancelButton(i18n("Cancel"));
  setOkButton(i18n("OK"));
}

/*================================================================*/
void KWStyleManager::setupTab1()
{
  tab1 = new QWidget(this);

  grid1 = new QGridLayout(tab1,1,2,15,7);

  lStyleList = new QListBox(tab1);
  for (unsigned int i = 0;i < doc->paragLayoutList.count();i++)
    lStyleList->insertItem(doc->paragLayoutList.at(i)->getName());
  connect(lStyleList,SIGNAL(selected(int)),this,SLOT(editStyle(int)));
  lStyleList->setCurrentItem(0);
  grid1->addWidget(lStyleList,0,0);

  bButtonBox = new KButtonBox(tab1,KButtonBox::VERTICAL);
  bAdd = bButtonBox->addButton(i18n("&Add.."),false);
  bDelete = bButtonBox->addButton(i18n("&Delete"),false);
  bButtonBox->addStretch();
  bEdit = bButtonBox->addButton(i18n("&Edit..."),false);
  connect(bEdit,SIGNAL(clicked()),this,SLOT(editStyle()));
  bCopy = bButtonBox->addButton(i18n("&Copy..."),false); 
  bButtonBox->addStretch();
  bUp = bButtonBox->addButton(i18n("Up"),false); 
  bDown = bButtonBox->addButton(i18n("D&own"),false); 
  bButtonBox->layout();
  grid1->addWidget(bButtonBox,0,1);

  grid1->addColSpacing(0,lStyleList->width());
  grid1->addColSpacing(1,bButtonBox->width());
  grid1->setColStretch(0,1);

  grid1->addRowSpacing(0,lStyleList->height());
  grid1->addRowSpacing(0,bButtonBox->height());
  grid1->setRowStretch(0,1);

  grid1->activate();

  addTab(tab1,i18n("Style Manager"));
}

/*================================================================*/
void KWStyleManager::editStyle()
{
  if (editor)
    {
      disconnect(editor,SIGNAL(updateStyles()),this,SLOT(updateStyles()));
      delete editor;
      editor = 0L;
    }

  editor = new KWStyleEditor(0L,doc->paragLayoutList.at(lStyleList->currentItem()),doc);
  editor->setCaption(i18n("KWord - Stylist"));
  connect(editor,SIGNAL(updateStyles()),this,SLOT(updateStyles()));
  editor->show();
}

/*================================================================*/
void KWStyleManager::updateStyles()
{
}

/******************************************************************/
/* Class: KWStylePreview                                          */
/******************************************************************/

/*================================================================*/
void KWStylePreview::drawContents(QPainter *painter)
{
  QRect r = contentsRect();
  QFontMetrics fm(font());

  painter->fillRect(r.x() + fm.width('W'),r.y() + fm.height(),r.width() - 2 * fm.width('W'),r.height() - 2 * fm.height(),white);
  painter->setClipRect(r.x() + fm.width('W'),r.y() + fm.height(),r.width() - 2 * fm.width('W'),r.height() - 2 * fm.height());

  QFont f(style->getFormat().getUserFont()->getFontName(),style->getFormat().getPTFontSize());
  f.setBold(style->getFormat().getWeight() == 75 ? true : false);
  f.setItalic(static_cast<bool>(style->getFormat().getItalic()));
  f.setUnderline(static_cast<bool>(style->getFormat().getUnderline()));

  QColor c(style->getFormat().getColor());

  painter->setPen(QPen(c));
  painter->setFont(f);

  fm = QFontMetrics(f);
  int y = height() / 2 - fm.height() / 2;

  painter->drawText(20 + style->getPTFirstLineLeftIndent() + style->getPTLeftIndent(),
		    y,fm.width(i18n("KWord, KOffice's Wordprocessor")),fm.height(),0,i18n("KWord, KOffice's Wordprocessor"));
}

/******************************************************************/
/* Class: KWStyleEditor                                           */
/******************************************************************/

/*================================================================*/
KWStyleEditor::KWStyleEditor(QWidget *_parent,KWParagLayout *_style,KWordDocument *_doc)
  : QTabDialog(_parent,"",true)
{
  paragDia = 0;
  style = _style;
  doc = _doc;
  setupTab1();

  setCancelButton(i18n("Cancel"));
  setOkButton(i18n("OK"));
}

/*================================================================*/
void KWStyleEditor::setupTab1()
{
  tab1 = new QWidget(this);

  grid1 = new QGridLayout(tab1,3,1,15,7);

  nwid = new QWidget(tab1);
  grid2 = new QGridLayout(nwid,1,2,15,7);

  lName = new QLabel(i18n("Name:"),nwid);
  lName->resize(lName->sizeHint());
  grid2->addWidget(lName,0,0);

  eName = new QLineEdit(nwid);
  eName->resize(eName->sizeHint());
  eName->setText(style->getName());
  grid2->addWidget(eName,0,1);

  if (style->getName() == QString(i18n("Standard")))
    eName->setEnabled(false);

  grid2->addRowSpacing(0,lName->height());
  grid2->addRowSpacing(0,eName->height());

  grid2->addColSpacing(0,lName->width());
  grid2->addColSpacing(1,eName->width());
  grid2->setColStretch(1,1);
  
  grid2->activate();

  grid1->addWidget(nwid,0,0);

  preview = new KWStylePreview(i18n("Preview"),tab1,style);
  grid1->addWidget(preview,1,0);

  bButtonBox = new KButtonBox(tab1);
  bFont = bButtonBox->addButton(i18n("&Font..."),true);
  connect(bFont,SIGNAL(clicked()),this,SLOT(changeFont()));
  bButtonBox->addStretch();
  bColor = bButtonBox->addButton(i18n("&Color..."),true);
  connect(bColor,SIGNAL(clicked()),this,SLOT(changeColor()));
  bButtonBox->addStretch();
  bSpacing = bButtonBox->addButton(i18n("&Spacing and Indents..."),true);
  connect(bSpacing,SIGNAL(clicked()),this,SLOT(changeSpacing()));
  bButtonBox->addStretch();
  bAlign = bButtonBox->addButton(i18n("&Alignment..."),true);
  connect(bAlign,SIGNAL(clicked()),this,SLOT(changeAlign()));
  bButtonBox->addStretch();
  bBorders = bButtonBox->addButton(i18n("&Borders..."),true);
  connect(bBorders,SIGNAL(clicked()),this,SLOT(changeBorders()));
  bButtonBox->addStretch();
  bNumbering = bButtonBox->addButton(i18n("&Numbering..."),true);
  connect(bNumbering,SIGNAL(clicked()),this,SLOT(changeNumbering()));
  bButtonBox->layout();
  grid1->addWidget(bButtonBox,2,0);

  grid1->addColSpacing(0,nwid->width());
  grid1->addColSpacing(0,preview->width());
  grid1->addColSpacing(0,bButtonBox->width());
  grid1->setColStretch(0,1);

  grid1->addRowSpacing(0,nwid->height());
  grid1->addRowSpacing(1,100);
  grid1->addRowSpacing(2,bButtonBox->height());
  grid1->setRowStretch(1,1);

  grid1->activate();

  addTab(tab1,i18n("Style Editor"));
}

/*================================================================*/
void KWStyleEditor::changeFont()
{
  QFont f(style->getFormat().getUserFont()->getFontName(),style->getFormat().getPTFontSize());
  f.setBold(style->getFormat().getWeight() == 75 ? true : false);
  f.setItalic(static_cast<bool>(style->getFormat().getItalic()));
  f.setUnderline(static_cast<bool>(style->getFormat().getUnderline()));

  if (KFontDialog::getFont(f))
    {
      style->getFormat().setUserFont(new KWUserFont(doc,f.family()));
      style->getFormat().setPTFontSize(f.pointSize());
      style->getFormat().setWeight(f.bold() ? 75 : 50);
      style->getFormat().setItalic(static_cast<int>(f.italic()));
      style->getFormat().setUnderline(static_cast<int>(f.underline()));
      preview->repaint(true);
    }
}

/*================================================================*/
void KWStyleEditor::changeColor()
{
  QColor c(style->getFormat().getColor());
  if (KColorDialog::getColor(c))
    {
      style->getFormat().setColor(c);
      preview->repaint(true);
    }
}

/*================================================================*/
void KWStyleEditor::changeSpacing()
{
  if (paragDia)
    {
      disconnect(paragDia,SIGNAL(applyButtonPressed()),this,SLOT(paragDiaOk()));
      paragDia->close();
      delete paragDia;
      paragDia = 0;
    }
  paragDia = new KWParagDia(0,"",KWParagDia::PD_SPACING);
  paragDia->setCaption(i18n("KWord - Paragraph Spacing"));
  connect(paragDia,SIGNAL(applyButtonPressed()),this,SLOT(paragDiaOk()));
  paragDia->setSpaceBeforeParag(style->getMMParagHeadOffset());
  paragDia->setSpaceAfterParag(style->getMMParagFootOffset());
  paragDia->setLineSpacing(style->getPTLineSpacing());
  paragDia->setLeftIndent(style->getMMLeftIndent());
  paragDia->setFirstLineIndent(style->getMMFirstLineLeftIndent());
  paragDia->show();
}

/*================================================================*/
void KWStyleEditor::changeAlign()
{
  if (paragDia)
    {
      disconnect(paragDia,SIGNAL(applyButtonPressed()),this,SLOT(paragDiaOk()));
      paragDia->close();
      delete paragDia;
      paragDia = 0;
    }
  paragDia = new KWParagDia(0,"",KWParagDia::PD_FLOW);
  paragDia->setCaption(i18n("KWord - Paragraph Flow (Alignment)"));
  connect(paragDia,SIGNAL(applyButtonPressed()),this,SLOT(paragDiaOk()));
  paragDia->setFlow(style->getFlow());
  paragDia->show();
}

/*================================================================*/
void KWStyleEditor::changeBorders()
{
}

/*================================================================*/
void KWStyleEditor::changeNumbering()
{
}

/*================================================================*/
void KWStyleEditor::paragDiaOk()
{
  switch (paragDia->getFlags())
    {
    case KWParagDia::PD_SPACING:
      {
	style->setMMParagHeadOffset(static_cast<int>(paragDia->getSpaceBeforeParag()));
	style->setMMParagFootOffset(static_cast<int>(paragDia->getSpaceAfterParag()));
	style->setPTLineSpacing(static_cast<int>(paragDia->getLineSpacing()));
	style->setMMLeftIndent(static_cast<int>(paragDia->getLeftIndent()));
	style->setMMFirstLineLeftIndent(static_cast<int>(paragDia->getFirstLineIndent()));
      } break;
    case KWParagDia::PD_FLOW:
      style->setFlow(paragDia->getFlow());
      break;
    default: break;
    }

  preview->repaint(true);
}
