/*
Copyright (C) 1996 Bernd Johannes Wuebben  <wuebben@kde.org>
Copyright (c) 1999 Preston Brown <pbrown@kde.org>
Copyright (c) 1999 Mario Weilguni <mweilguni@kde.org>

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

#include "kfontchooser.h"
#include "fonthelpers_p.h"

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGuiApplication>
#include <QLabel>
#include <QLayout>
#include <QLocale>
#include <QSplitter>
#include <QScrollBar>
#include <QFontDatabase>
#include <QGroupBox>
#include <QListWidget>
#include <QTextEdit>

#include <cmath>

// When message extraction needs to be avoided.
#define TR_NOX tr

static int minimumListWidth( const QListWidget *list )
{
    int w=0;
    for( int i=0; i<list->count(); i++ )
    {
        int itemWidth = list->visualItemRect(list->item(i)).width();
        // ...and add a space on both sides for not too tight look.
        itemWidth += list->fontMetrics().width(QLatin1Char(' ')) * 2;
        w = qMax(w,itemWidth);
    }
    if( w == 0 ) { w = 40; }
    w += list->frameWidth() * 2;
    w += list->verticalScrollBar()->sizeHint().width();
    return w;
}

static int minimumListHeight( const QListWidget *list, int numVisibleEntry )
{
    int w = list->count() > 0 ? list->visualItemRect(list->item(0)).height() :
            list->fontMetrics().lineSpacing();

    if( w < 0 ) { w = 10; }
    if( numVisibleEntry <= 0 ) { numVisibleEntry = 4; }
    return ( w * numVisibleEntry + 2 * list->frameWidth() );
}

static QString formatFontSize(qreal size)
{
    return QLocale::system().toString(size, 'f', (size == floor(size)) ? 0 : 1);
}

class KFontChooser::Private
{
public:
    Private( KFontChooser* qq )
        : q( qq )
    {
        m_palette.setColor(QPalette::Active, QPalette::Text, Qt::black);
        m_palette.setColor(QPalette::Active, QPalette::Base, Qt::white);
        signalsAllowed = true;
        selectedSize = -1;
        customSizeRow = -1;
    }

    // pointer to an optinally supplied list of fonts to
    // inserted into the fontdialog font-family combo-box
//    QStringList  fontList;

    void setFamilyBoxItems(const QStringList &fonts);
    void fillFamilyListBox(bool onlyFixedFonts = false);
    int nearestSizeRow(qreal val, bool customize);
    qreal fillSizeList(const QList<qreal> &sizes = QList<qreal>());
    qreal setupSizeListBox(const QString& family, const QString& style);

    void setupDisplay();
    QString styleIdentifier (const QFont &font);

    void _k_toggled_checkbox();
    void _k_family_chosen_slot(const QString&);
    void _k_size_chosen_slot(const QString&);
    void _k_style_chosen_slot(const QString&);
    void _k_displaySample(const QFont &font);
    void _k_size_value_slot(double);

    KFontChooser *q;

    QPalette m_palette;
    bool signalsAllowed:1;

    bool usingFixed:1;

    QDoubleSpinBox *sizeOfFont;

    QTextEdit   *sampleEdit;

    QLabel       *familyLabel;
    QLabel       *styleLabel;
    QCheckBox    *familyCheckbox;
    QCheckBox    *styleCheckbox;
    QCheckBox    *sizeCheckbox;
    QLabel       *sizeLabel;
    QListWidget     *familyListBox;
    QListWidget     *styleListBox;
    QListWidget     *sizeListBox;
    QCheckBox    *sizeIsRelativeCheckBox;

    QFont        selFont;

    QString      selectedStyle;
    qreal        selectedSize;

    int          customSizeRow;
    QString      standardSizeAtCustom;

    // Mappings of translated to Qt originated family and style strings.
    QHash<QString, QString> qtFamilies;
    QHash<QString, QString> qtStyles;
    // Mapping of translated style strings to internal style identifiers.
    QHash<QString, QString> styleIDs;

};


KFontChooser::KFontChooser( QWidget *parent,
                            const DisplayFlags& flags,
                            const QStringList &fontList,
                            int visibleListSize,
                            Qt::CheckState *sizeIsRelativeState )
    : QWidget(parent),
      d( new KFontChooser::Private( this ) )
{
    d->usingFixed = flags & FixedFontsOnly;
    setWhatsThis(tr("Here you can choose the font to be used.", "@info:whatsthis"));

    // The top layout is divided vertically into a splitter with font
    // attribute widgets and preview on the top, and XLFD data at the bottom.
    QVBoxLayout *topLayout = new QVBoxLayout( this );
    topLayout->setMargin( 0 );
    const int spacingHint = style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing);
    int checkBoxGap = spacingHint / 2;

    // The splitter contains font attribute widgets in the top part,
    // and the font preview in the bottom part.
    // The splitter is there to allow the user to resize the font preview.
    QSplitter *splitter = new QSplitter(Qt::Vertical, this);
    splitter->setChildrenCollapsible(false);
    topLayout->addWidget(splitter);

    // Build the grid of font attribute widgets for the upper splitter part.
    //
    QWidget *page;
    QGridLayout *gridLayout;
    int row = 0;
    if( flags & DisplayFrame )
    {
        page = new QGroupBox( tr("Requested Font"), this );
        splitter->addWidget(page);
        gridLayout = new QGridLayout( page );
        row = 1;
    }
    else
    {
        page = new QWidget( this );
        splitter->addWidget(page);
        gridLayout = new QGridLayout( page );
        gridLayout->setMargin( 0 );
    }

    //
    // first, create the labels across the top
    //
    QHBoxLayout *familyLayout = new QHBoxLayout();
    familyLayout->addSpacing( checkBoxGap );
    if ( flags & ShowDifferences ) {
        d->familyCheckbox = new QCheckBox(tr("Font", "@option:check"), page);
        connect(d->familyCheckbox, SIGNAL(toggled(bool)),
                this, SLOT(_k_toggled_checkbox()));
        familyLayout->addWidget(d->familyCheckbox, 0, Qt::AlignLeft);
        d->familyCheckbox->setWhatsThis(tr("Enable this checkbox to change the font family settings.", "@info:whatsthis"));
        d->familyCheckbox->setToolTip(tr("Change font family?", "@info:tooltip") );
        d->familyLabel = 0;
    } else {
        d->familyCheckbox = 0;
        d->familyLabel = new QLabel( tr("Font:", "@label"), page );
        familyLayout->addWidget(d->familyLabel, 1, Qt::AlignLeft);
    }
    gridLayout->addLayout(familyLayout, row, 0 );

    QHBoxLayout *styleLayout = new QHBoxLayout();
    if ( flags & ShowDifferences ) {
        d->styleCheckbox = new QCheckBox(tr("Font style", "@option:check"), page);
        connect(d->styleCheckbox, SIGNAL(toggled(bool)),
                this, SLOT(_k_toggled_checkbox()));
        styleLayout->addWidget(d->styleCheckbox, 0, Qt::AlignLeft);
        d->styleCheckbox->setWhatsThis(tr("Enable this checkbox to change the font style settings.", "@info:whatsthis"));
        d->styleCheckbox->setToolTip(tr("Change font style?", "@info:tooltip"));
        d->styleLabel = 0;
    } else {
        d->styleCheckbox = 0;
        d->styleLabel = new QLabel(tr("Font style:"), page );
        styleLayout->addWidget(d->styleLabel, 1, Qt::AlignLeft);
    }
    styleLayout->addSpacing( checkBoxGap );
    gridLayout->addLayout(styleLayout, row, 1 );

    QHBoxLayout *sizeLayout = new QHBoxLayout();
    if ( flags & ShowDifferences ) {
        d->sizeCheckbox = new QCheckBox(tr("Size", "@option:check"),page);
        connect(d->sizeCheckbox, SIGNAL(toggled(bool)),
                this, SLOT(_k_toggled_checkbox()));
        sizeLayout->addWidget(d->sizeCheckbox, 0, Qt::AlignLeft);
        d->sizeCheckbox->setWhatsThis(tr("Enable this checkbox to change the font size settings.", "@info:whatsthis"));
        d->sizeCheckbox->setToolTip(tr("Change font size?", "@info:tooltip"));
        d->sizeLabel = 0;
    } else {
        d->sizeCheckbox = 0;
        d->sizeLabel = new QLabel(tr("Size:", "@label:listbox Font size"), page );
        sizeLayout->addWidget(d->sizeLabel, 1, Qt::AlignLeft);
    }
    sizeLayout->addSpacing( checkBoxGap );
    sizeLayout->addSpacing( checkBoxGap ); // prevent label from eating border
    gridLayout->addLayout(sizeLayout, row, 2 );

    row ++;

    //
    // now create the actual boxes that hold the info
    //
    d->familyListBox = new QListWidget( page );
    d->familyListBox->setEnabled( flags ^ ShowDifferences );
    gridLayout->addWidget( d->familyListBox, row, 0 );
    QString fontFamilyWhatsThisText (
        tr("Here you can choose the font family to be used.", "@info:whatsthis"));
    d->familyListBox->setWhatsThis(fontFamilyWhatsThisText );

    if ( flags & ShowDifferences ) {
        d->familyCheckbox->setWhatsThis(fontFamilyWhatsThisText );
    } else {
        d->familyLabel->setWhatsThis(fontFamilyWhatsThisText );
    }

    connect(d->familyListBox, SIGNAL(currentTextChanged(QString)),
            this, SLOT(_k_family_chosen_slot(QString)));
    if ( !fontList.isEmpty() ) {
        d->setFamilyBoxItems(fontList);
    }
    else
    {
        d->fillFamilyListBox( flags & FixedFontsOnly );
    }

    d->familyListBox->setMinimumWidth( minimumListWidth( d->familyListBox ) );
    d->familyListBox->setMinimumHeight(
        minimumListHeight( d->familyListBox, visibleListSize  ) );

    d->styleListBox = new QListWidget( page );
    d->styleListBox->setEnabled( flags ^ ShowDifferences );
    gridLayout->addWidget(d->styleListBox, row, 1);
    d->styleListBox->setWhatsThis(tr("Here you can choose the font style to be used.", "@info:whatsthis"));
    if ( flags & ShowDifferences ) {
        ((QWidget *)d->styleCheckbox)->setWhatsThis(fontFamilyWhatsThisText );
    } else {
        ((QWidget *)d->styleLabel)->setWhatsThis( fontFamilyWhatsThisText );
    }
    // Populate usual styles, to determine minimum list width;
    // will be replaced later with correct styles.
    d->styleListBox->addItem(TR_NOX("Normal", "QFontDatabase"));
    d->styleListBox->addItem(tr("Italic", "@item font"));
    d->styleListBox->addItem(tr("Oblique", "@item font"));
    d->styleListBox->addItem(tr("Bold", "@item font"));
    d->styleListBox->addItem(tr("Bold Italic", "@item font"));
    d->styleListBox->setMinimumWidth( minimumListWidth( d->styleListBox ) );
    d->styleListBox->setMinimumHeight(
        minimumListHeight( d->styleListBox, visibleListSize  ) );

    connect(d->styleListBox, SIGNAL(currentTextChanged(QString)),
            this, SLOT(_k_style_chosen_slot(QString)));


    d->sizeListBox = new QListWidget( page );
    d->sizeOfFont = new QDoubleSpinBox(page);
    d->sizeOfFont->setMinimum(4);
    d->sizeOfFont->setMaximum(999);
    d->sizeOfFont->setDecimals(1);
    d->sizeOfFont->setSingleStep(1);

    d->sizeListBox->setEnabled( flags ^ ShowDifferences );
    d->sizeOfFont->setEnabled( flags ^ ShowDifferences );
    if( sizeIsRelativeState ) {
        QString sizeIsRelativeCBText =
            tr("Relative", "@item font size");
        QString sizeIsRelativeCBToolTipText =
            tr("Font size<br /><i>fixed</i> or <i>relative</i><br />to environment");
        QString sizeIsRelativeCBWhatsThisText =
            tr("Here you can switch between fixed font size and font size "
               "to be calculated dynamically and adjusted to changing "
               "environment (e.g. widget dimensions, paper size)." );
        d->sizeIsRelativeCheckBox = new QCheckBox( sizeIsRelativeCBText,
                                                page );
        d->sizeIsRelativeCheckBox->setTristate( flags & ShowDifferences );
        QGridLayout *sizeLayout2 = new QGridLayout();
        sizeLayout2->setSpacing( spacingHint/2 );
        gridLayout->addLayout(sizeLayout2, row, 2);
        sizeLayout2->setColumnStretch( 1, 1 ); // to prevent text from eating the right border
        sizeLayout2->addWidget( d->sizeOfFont, 0, 0, 1, 2);
        sizeLayout2->addWidget(d->sizeListBox, 1,0, 1,2);
        sizeLayout2->addWidget(d->sizeIsRelativeCheckBox, 2, 0, Qt::AlignLeft);
        d->sizeIsRelativeCheckBox->setWhatsThis(sizeIsRelativeCBWhatsThisText );
        d->sizeIsRelativeCheckBox->setToolTip( sizeIsRelativeCBToolTipText );
    }
    else {
        d->sizeIsRelativeCheckBox = 0L;
        QGridLayout *sizeLayout2 = new QGridLayout();
        sizeLayout2->setSpacing( spacingHint/2 );
        gridLayout->addLayout(sizeLayout2, row, 2);
        sizeLayout2->addWidget( d->sizeOfFont, 0, 0);
        sizeLayout2->addWidget(d->sizeListBox, 1,0);
    }
    QString fontSizeWhatsThisText =
        tr("Here you can choose the font size to be used." );
    d->sizeListBox->setWhatsThis(fontSizeWhatsThisText );

    if ( flags & ShowDifferences ) {
        ((QWidget *)d->sizeCheckbox)->setWhatsThis(fontSizeWhatsThisText );
    } else {
        ((QWidget *)d->sizeLabel)->setWhatsThis( fontSizeWhatsThisText );
    }

    // Populate with usual sizes, to determine minimum list width;
    // will be replaced later with correct sizes.
    d->fillSizeList();
    d->sizeListBox->setMinimumWidth( minimumListWidth(d->sizeListBox) +
                                  d->sizeListBox->fontMetrics().maxWidth() );
    d->sizeListBox->setMinimumHeight(
        minimumListHeight( d->sizeListBox, visibleListSize  ) );

    connect( d->sizeOfFont, SIGNAL(valueChanged(double)),
             this, SLOT(_k_size_value_slot(double)));

    connect( d->sizeListBox, SIGNAL(currentTextChanged(QString)),
             this, SLOT(_k_size_chosen_slot(QString)) );

    row ++;
    //
    // Completed the font attribute grid.

    // Add the font preview into the lower part of the splitter.
    //
    d->sampleEdit = new QTextEdit(page);
    d->sampleEdit->setAcceptRichText(false);
    QFont tmpFont( font().family(), 64, QFont::Black );
    d->sampleEdit->setFont(tmpFont);
    d->sampleEdit->setMinimumHeight( d->sampleEdit->fontMetrics().lineSpacing() );
    // tr: A classical test phrase, with all letters of the English alphabet.
    // Replace it with a sample text in your language, such that it is
    // representative of language's writing system.
    // If you wish, you can input several lines of text separated by \n.
    setSampleText(tr("The Quick Brown Fox Jumps Over The Lazy Dog"));
    d->sampleEdit->setTextCursor(QTextCursor(d->sampleEdit->document()));
    QString sampleEditWhatsThisText =
        tr("This sample text illustrates the current settings. "
             "You may edit it to test special characters." );
    d->sampleEdit->setWhatsThis(sampleEditWhatsThisText );

    connect(this, SIGNAL(fontSelected(QFont)),
            this, SLOT(_k_displaySample(QFont)));

    splitter->addWidget(d->sampleEdit);
    //
    // Finished setting up the splitter.

    //
    // Finished setting up the chooser layout.

    // lets initialize the display if possible
    if (d->usingFixed) {
        setFont( QFontDatabase::systemFont(QFontDatabase::FixedFont), d->usingFixed );
    } else {
        setFont( QGuiApplication::font(), d->usingFixed );
    }

    // check or uncheck or gray out the "relative" checkbox
    if( sizeIsRelativeState && d->sizeIsRelativeCheckBox )
        setSizeIsRelative( *sizeIsRelativeState );

    // Set focus to the size list as this is the most commonly changed property
    d->sizeListBox->setFocus();
}

KFontChooser::~KFontChooser()
{
    delete d;
}

void KFontChooser::setColor( const QColor & col )
{
    d->m_palette.setColor( QPalette::Active, QPalette::Text, col );
    QPalette pal = d->sampleEdit->palette();
    pal.setColor( QPalette::Active, QPalette::Text, col );
    d->sampleEdit->setPalette( pal );
    QTextCursor cursor = d->sampleEdit->textCursor();
    d->sampleEdit->selectAll();
    d->sampleEdit->setTextColor( col );
    d->sampleEdit->setTextCursor( cursor );
}

QColor KFontChooser::color() const
{
    return d->m_palette.color( QPalette::Active, QPalette::Text );
}

void KFontChooser::setBackgroundColor( const QColor & col )
{
    d->m_palette.setColor( QPalette::Active, QPalette::Base, col );
    QPalette pal = d->sampleEdit->palette();
    pal.setColor( QPalette::Active, QPalette::Base, col );
    d->sampleEdit->setPalette( pal );
}

QColor KFontChooser::backgroundColor() const
{
    return d->m_palette.color( QPalette::Active, QPalette::Base );
}

void KFontChooser::setSizeIsRelative( Qt::CheckState relative )
{
    // check or uncheck or gray out the "relative" checkbox
    if( d->sizeIsRelativeCheckBox ) {
        if( Qt::PartiallyChecked == relative )
            d->sizeIsRelativeCheckBox->setCheckState(Qt::PartiallyChecked);
        else
            d->sizeIsRelativeCheckBox->setCheckState(  (Qt::Checked == relative )  ? Qt::Checked : Qt::Unchecked);
    }
}

Qt::CheckState KFontChooser::sizeIsRelative() const
{
    return d->sizeIsRelativeCheckBox
        ? d->sizeIsRelativeCheckBox->checkState()
        : Qt::PartiallyChecked;
}

QString KFontChooser::sampleText() const
{
    return d->sampleEdit->toPlainText();
}

void KFontChooser::setSampleText( const QString &text )
{
    d->sampleEdit->setPlainText(text);
}

void KFontChooser::setSampleBoxVisible( bool visible )
{
    d->sampleEdit->setVisible( visible );
}

QSize KFontChooser::sizeHint( void ) const
{
    return minimumSizeHint();
}


void KFontChooser::enableColumn( int column, bool state )
{
    if( column & FamilyList )
    {
        d->familyListBox->setEnabled(state);
    }
    if( column & StyleList )
    {
        d->styleListBox->setEnabled(state);
    }
    if( column & SizeList )
    {
        d->sizeListBox->setEnabled(state);
        d->sizeOfFont->setEnabled(state);
    }
}


void KFontChooser::setFont( const QFont& aFont, bool onlyFixed )
{
    d->selFont = aFont;
    d->selectedSize=aFont.pointSizeF();
    if (d->selectedSize == -1)
        d->selectedSize = QFontInfo(aFont).pointSizeF();

    if( onlyFixed != d->usingFixed)
    {
        d->usingFixed = onlyFixed;
        d->fillFamilyListBox(d->usingFixed);
    }
    d->setupDisplay();
}


KFontChooser::FontDiffFlags KFontChooser::fontDiffFlags() const
{
    FontDiffFlags diffFlags = NoFontDiffFlags;

    if ( d->familyCheckbox && d->familyCheckbox->isChecked() ) {
        diffFlags |= FontDiffFamily;
    }

    if ( d->styleCheckbox && d->styleCheckbox->isChecked() ) {
        diffFlags |= FontDiffStyle;
    }

    if ( d->sizeCheckbox && d->sizeCheckbox->isChecked() ) {
        diffFlags |= FontDiffSize;
    }

    return diffFlags;
}

QFont KFontChooser::font() const
{
    return d->selFont;
}

void KFontChooser::Private::_k_toggled_checkbox()
{
    familyListBox->setEnabled( familyCheckbox->isChecked() );
    styleListBox->setEnabled( styleCheckbox->isChecked() );
    sizeListBox->setEnabled( sizeCheckbox->isChecked() );
    sizeOfFont->setEnabled( sizeCheckbox->isChecked() );
}

void KFontChooser::Private::_k_family_chosen_slot(const QString& family)
{
    if ( !signalsAllowed ) {
        return;
    }
    signalsAllowed = false;

    QString currentFamily;
    if (family.isEmpty()) {
        Q_ASSERT( familyListBox->currentItem() );
        if (familyListBox->currentItem()) {
          currentFamily = qtFamilies[familyListBox->currentItem()->text()];
        }
    }
    else {
        currentFamily = qtFamilies[family];
    }

    // Get the list of styles available in this family.
    QFontDatabase dbase;
    QStringList styles = dbase.styles(currentFamily);
    if (styles.isEmpty()) {
        // Avoid extraction, it is in kdeqt.po
        styles.append(TR_NOX("Normal", "QFontDatabase"));
    }

    // Filter style strings and add to the listbox.
    QString pureFamily;
    splitFontString(family, &pureFamily);
    QStringList filteredStyles;
    qtStyles.clear();
    styleIDs.clear();
    Q_FOREACH (const QString &style, styles) {
        // Sometimes the font database will report an invalid style,
        // that falls back back to another when set.
        // Remove such styles, by checking set/get round-trip.
        QFont testFont = dbase.font(currentFamily, style, 10);
        if (dbase.styleString(testFont) != style) {
            styles.removeAll(style);
            continue;
        }

        QString fstyle = tr("%1", "@item Font style").arg(style);
        if (!filteredStyles.contains(fstyle)) {
            filteredStyles.append(fstyle);
            qtStyles.insert(fstyle, style);
            styleIDs.insert(fstyle, styleIdentifier(testFont));
        }
    }
    styleListBox->clear();
    styleListBox->addItems(filteredStyles);

    // Try to set the current style in the listbox to that previous.
    int listPos = filteredStyles.indexOf(selectedStyle.isEmpty() ?  TR_NOX("Normal", "QFontDatabase") : selectedStyle);
    if (listPos < 0) {
        // Make extra effort to have Italic selected when Oblique was chosen,
        // and vice versa, as that is what the user would probably want.
        QString styleIt = tr("Italic", "@item font");
        QString styleOb = tr("Oblique", "@item font");
        for (int i = 0; i < 2; ++i) {
            int pos = selectedStyle.indexOf(styleIt);
            if (pos >= 0) {
                QString style = selectedStyle;
                style.replace(pos, styleIt.length(), styleOb);
                listPos = filteredStyles.indexOf(style);
                if (listPos >= 0) break;
            }
            qSwap(styleIt, styleOb);
        }
    }
    styleListBox->setCurrentRow(listPos >= 0 ? listPos : 0);
    QString currentStyle = qtStyles[styleListBox->currentItem()->text()];

    // Recompute the size listbox for this family/style.
    qreal currentSize = setupSizeListBox(currentFamily, currentStyle);
    sizeOfFont->setValue(currentSize);

    selFont = dbase.font(currentFamily, currentStyle, int(currentSize));
    if (dbase.isSmoothlyScalable(currentFamily, currentStyle) && selFont.pointSize() == floor(currentSize)) {
        selFont.setPointSizeF(currentSize);
    }
    emit q->fontSelected(selFont);

    signalsAllowed = true;
}

void KFontChooser::Private::_k_style_chosen_slot(const QString& style)
{
    if ( !signalsAllowed ) {
        return;
    }
    signalsAllowed = false;

    QFontDatabase dbase;
    QString currentFamily = qtFamilies[familyListBox->currentItem()->text()];
    QString currentStyle;
    if (style.isEmpty()) {
        currentStyle = qtStyles[styleListBox->currentItem()->text()];
    } else {
        currentStyle = qtStyles[style];
    }

    // Recompute the size listbox for this family/style.
    qreal currentSize = setupSizeListBox(currentFamily, currentStyle);
    sizeOfFont->setValue(currentSize);

    selFont = dbase.font(currentFamily, currentStyle, int(currentSize));
    if (dbase.isSmoothlyScalable(currentFamily, currentStyle) && selFont.pointSize() == floor(currentSize)) {
        selFont.setPointSizeF(currentSize);
    }
    emit q->fontSelected(selFont);

    if (!style.isEmpty()) {
        selectedStyle = currentStyle;
    }

    signalsAllowed = true;
}

void KFontChooser::Private::_k_size_chosen_slot(const QString& size)
{
    if ( !signalsAllowed ) {
        return;
    }

    signalsAllowed = false;

    qreal currentSize;
    if (size.isEmpty()) {
        currentSize = QLocale::system().toDouble(sizeListBox->currentItem()->text());
    } else {
        currentSize = QLocale::system().toDouble(size);
    }

    // Reset the customized size slot in the list if not needed.
    if (customSizeRow >= 0 && selFont.pointSizeF() != currentSize) {
        sizeListBox->item(customSizeRow)->setText(standardSizeAtCustom);
        customSizeRow = -1;
    }

    sizeOfFont->setValue(currentSize);
    selFont.setPointSizeF(currentSize);
    emit q->fontSelected(selFont);

    if (!size.isEmpty()) {
        selectedSize = currentSize;
    }

    signalsAllowed = true;
}

void KFontChooser::Private::_k_size_value_slot(double dval)
{
    if ( !signalsAllowed ) {
        return;
    }
    signalsAllowed = false;

    // We compare with qreal, so convert for platforms where qreal != double.
    qreal val = qreal(dval);

    QFontDatabase dbase;
    QString family = qtFamilies[familyListBox->currentItem()->text()];
    QString style = qtStyles[styleListBox->currentItem()->text()];

    // Reset current size slot in list if it was customized.
    if (customSizeRow >= 0 && sizeListBox->currentRow() == customSizeRow) {
        sizeListBox->item(customSizeRow)->setText(standardSizeAtCustom);
        customSizeRow = -1;
    }

    bool canCustomize = true;

    // For Qt-bad-sizes workaround: skip this block unconditionally
    if (!dbase.isSmoothlyScalable(family, style)) {
        // Bitmap font, allow only discrete sizes.
        // Determine the nearest in the direction of change.
        canCustomize = false;
        int nrows = sizeListBox->count();
        int row = sizeListBox->currentRow();
        int nrow;
        if (val - selFont.pointSizeF() > 0) {
            for (nrow = row + 1; nrow < nrows; ++nrow)
                if (QLocale::system().toDouble(sizeListBox->item(nrow)->text()) >= val)
                    break;
        }
        else {
            for (nrow = row - 1; nrow >= 0; --nrow)
                if (QLocale::system().toDouble(sizeListBox->item(nrow)->text()) <= val)
                    break;
        }
        // Make sure the new row is not out of bounds.
        nrow = nrow < 0 ? 0 : nrow >= nrows ? nrows - 1 : nrow;
        // Get the size from the new row and set the spinbox to that size.
        val = QLocale::system().toDouble(sizeListBox->item(nrow)->text());
        sizeOfFont->setValue(val);
    }

    // Set the current size in the size listbox.
    int row = nearestSizeRow(val, canCustomize);
    sizeListBox->setCurrentRow(row);

    selectedSize = val;
    selFont.setPointSizeF(val);
    emit q->fontSelected( selFont );

    signalsAllowed = true;
}

void KFontChooser::Private::_k_displaySample( const QFont& font )
{
    sampleEdit->setFont(font);
    //sampleEdit->setCursorPosition(0);

    //QFontInfo a = QFontInfo(font);
    //qDebug() << "font: " << a.family () << ", " << a.pointSize ();
    //qDebug() << "      (" << font.toString() << ")\n";
}

int KFontChooser::Private::nearestSizeRow (qreal val, bool customize)
{
    qreal diff = 1000;
    int row = 0;
    for (int r = 0; r < sizeListBox->count(); ++r) {
        qreal cval = QLocale::system().toDouble(sizeListBox->item(r)->text());
        if (qAbs(cval - val) < diff) {
            diff = qAbs(cval - val);
            row = r;
        }
    }
    // For Qt-bad-sizes workaround: ignore value of customize, use true
    if (customize && diff > 0) {
        customSizeRow = row;
        standardSizeAtCustom = sizeListBox->item(row)->text();
        sizeListBox->item(row)->setText(formatFontSize(val));
    }
    return row;
}

qreal KFontChooser::Private::fillSizeList (const QList<qreal> &sizes_)
{
    if ( !sizeListBox ) {
        return 0; //assertion.
    }

    QList<qreal> sizes = sizes_;
    bool canCustomize = false;
    if (sizes.count() == 0) {
        static const int c[] = {
            4,  5,  6,  7,
            8,  9,  10, 11,
            12, 13, 14, 15,
            16, 17, 18, 19,
            20, 22, 24, 26,
            28, 32, 48, 64,
            72, 80, 96, 128,
            0
        };
        for (int i = 0; c[i]; ++i) {
            sizes.append(c[i]);
        }
        // Since sizes were not supplied, this is a vector font,
        // and size slot customization is allowed.
        canCustomize = true;
    }

    // Insert sizes into the listbox.
    sizeListBox->clear();
    qSort(sizes);
    Q_FOREACH (qreal size, sizes) {
        sizeListBox->addItem(formatFontSize(size));
    }

    // Return the nearest to selected size.
    // If the font is vector, the nearest size is always same as selected,
    // thus size slot customization is allowed.
    // If the font is bitmap, the nearest size need not be same as selected,
    // thus size slot customization is not allowed.
    customSizeRow = -1;
    int row = nearestSizeRow(selectedSize, canCustomize);
    return QLocale::system().toDouble(sizeListBox->item(row)->text());
}

qreal KFontChooser::Private::setupSizeListBox (const QString& family, const QString& style)
{
    QFontDatabase dbase;
    QList<qreal> sizes;
    if (dbase.isSmoothlyScalable(family, style)) {
        // A vector font.
        //>sampleEdit->setPaletteBackgroundPixmap( VectorPixmap ); // TODO
    }
    else {
        // A bitmap font.
        //sampleEdit->setPaletteBackgroundPixmap( BitmapPixmap ); // TODO
        QList<int> smoothSizes = dbase.smoothSizes(family, style);
        Q_FOREACH (int size, smoothSizes) {
            sizes.append(size);
        }
    }

    // Fill the listbox (uses default list of sizes if the given is empty).
    // Collect the best fitting size to selected size, to use if not smooth.
    qreal bestFitSize = fillSizeList(sizes);

    // Set the best fit size as current in the listbox if available.
    const QList<QListWidgetItem*> selectedSizeList =
        sizeListBox->findItems( formatFontSize(bestFitSize),
                                Qt::MatchExactly );
    if ( !selectedSizeList.isEmpty() ) {
        sizeListBox->setCurrentItem(selectedSizeList.first());
    }
    //TODO - KDE4 : sizeListBox->scrollTo(sizeListBox->currentItem());

    return bestFitSize;
}

void KFontChooser::Private::setupDisplay()
{
    QFontDatabase dbase;
    QString family = selFont.family().toLower();
    QString styleID = styleIdentifier(selFont);
    qreal size = selFont.pointSizeF();
    if (size == -1)
        size = QFontInfo( selFont ).pointSizeF();

    int numEntries, i;

    // Direct family match.
    numEntries = familyListBox->count();
    for (i = 0; i < numEntries; i++) {
        if (family == qtFamilies[familyListBox->item(i)->text()].toLower()) {
            familyListBox->setCurrentRow(i);
            break;
        }
    }

    // 1st family fallback.
    if ( i == numEntries )
    {
        if (family.contains(QLatin1Char('[')))
        {
            family = family.left(family.indexOf(QLatin1Char('['))).trimmed();
            for (i = 0; i < numEntries; i++) {
                if (family == qtFamilies[familyListBox->item(i)->text()].toLower()) {
                    familyListBox->setCurrentRow(i);
                    break;
                }
            }
        }
    }

    // 2nd family fallback.
    if ( i == numEntries )
    {
        QString fallback = family + QStringLiteral(" [");
        for (i = 0; i < numEntries; i++) {
            if (qtFamilies[familyListBox->item(i)->text()].toLower().startsWith(fallback)) {
                familyListBox->setCurrentRow(i);
                break;
            }
        }
    }

    // 3rd family fallback.
    if ( i == numEntries )
    {
        for (i = 0; i < numEntries; i++) {
            if (qtFamilies[familyListBox->item(i)->text()].toLower().startsWith(family)) {
                familyListBox->setCurrentRow(i);
                break;
            }
        }
    }

    // Family fallback in case nothing matched. Otherwise, diff doesn't work
    if ( i == numEntries ) {
        familyListBox->setCurrentRow( 0 );
    }

    // By setting the current item in the family box, the available
    // styles and sizes for that family have been collected.
    // Try now to set the current items in the style and size boxes.

    // Set current style in the listbox.
    numEntries = styleListBox->count();
    for (i = 0; i < numEntries; i++) {
        if (styleID == styleIDs[styleListBox->item(i)->text()]) {
            styleListBox->setCurrentRow(i);
            break;
        }
    }
    if (i == numEntries) {
        // Style not found, fallback.
        styleListBox->setCurrentRow(0);
    }

    // Set current size in the listbox.
    // If smoothly scalable, allow customizing one of the standard size slots,
    // otherwise just select the nearest available size.
    QString currentFamily = qtFamilies[familyListBox->currentItem()->text()];
    QString currentStyle = qtStyles[styleListBox->currentItem()->text()];
    bool canCustomize = dbase.isSmoothlyScalable(currentFamily, currentStyle);
    sizeListBox->setCurrentRow(nearestSizeRow(size, canCustomize));

    // Set current size in the spinbox.
    sizeOfFont->setValue(QLocale::system().toDouble(sizeListBox->currentItem()->text()));
}


void KFontChooser::getFontList( QStringList &list, uint fontListCriteria)
{
    QFontDatabase dbase;
    QStringList lstSys(dbase.families());

    // if we have criteria; then check fonts before adding
    if (fontListCriteria)
    {
        QStringList lstFonts;
        for (QStringList::const_iterator it = lstSys.constBegin(); it != lstSys.constEnd(); ++it)
        {
            if ((fontListCriteria & FixedWidthFonts) > 0 && !dbase.isFixedPitch(*it)) continue;
            if (((fontListCriteria & (SmoothScalableFonts | ScalableFonts)) == ScalableFonts) &&
                !dbase.isBitmapScalable(*it)) continue;
            if ((fontListCriteria & SmoothScalableFonts) > 0 && !dbase.isSmoothlyScalable(*it)) continue;
            lstFonts.append(*it);
        }

        if((fontListCriteria & FixedWidthFonts) > 0) {
            // Fallback.. if there are no fixed fonts found, it's probably a
            // bug in the font server or Qt.  In this case, just use 'fixed'
            if (lstFonts.count() == 0)
                lstFonts.append(QStringLiteral("fixed"));
        }

        lstSys = lstFonts;
    }

    lstSys.sort();

    list = lstSys;
}

void KFontChooser::Private::setFamilyBoxItems(const QStringList &fonts)
{
    signalsAllowed = false;

    QStringList trfonts = translateFontNameList(fonts, &qtFamilies);
    familyListBox->clear();
    familyListBox->addItems(trfonts);

    signalsAllowed = true;
}

void KFontChooser::Private::fillFamilyListBox(bool onlyFixedFonts)
{
    QStringList fontList;
    getFontList(fontList, onlyFixedFonts?FixedWidthFonts:0);
    setFamilyBoxItems(fontList);
}

// Human-readable style identifiers returned by QFontDatabase::styleString()
// do not always survive round trip of QFont serialization/deserialization,
// causing wrong style in the style box to be highlighted when
// the chooser dialog is opened. This will cause the style to be changed
// when the dialog is closed and the user did not touch the style box.
// Hence, construct custom style identifiers sufficient for the purpose.
QString KFontChooser::Private::styleIdentifier(const QFont &font)
{
    const QChar comma(QLatin1Char(','));
    return   QString::number(font.weight()) + comma
           + QString::number((int)font.style()) + comma
           + QString::number(font.stretch());
}

#include "moc_kfontchooser.cpp"
