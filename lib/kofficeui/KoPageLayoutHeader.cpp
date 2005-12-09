#include <KoPageLayoutHeader.h>
#include <koUnitWidgets.h>
#include <koUnit.h>

#include <qlayout.h>
#include <qcheckbox.h>

KoPageLayoutHeader::KoPageLayoutHeader(QWidget *parent, KoUnit::Unit unit, const KoKWHeaderFooter &kwhf)
    : KoPageLayoutHeaderBase(parent) {
    m_headerFooters = kwhf;
    QHBoxLayout *lay = new QHBoxLayout(headerSpacingPane);
    m_headerSpacing = new KoUnitDoubleSpinBox( headerSpacingPane, 0.0, 999.0, 0.5, kwhf.ptHeaderBodySpacing, unit );
    lay->addWidget(m_headerSpacing);

    lay = new QHBoxLayout(footerSpacingPane);
    m_footerSpacing = new KoUnitDoubleSpinBox( footerSpacingPane, 0.0, 999.0, 0.5, kwhf.ptFooterBodySpacing, unit );
    lay->addWidget(m_footerSpacing);

    lay = new QHBoxLayout(footnotePane);
    m_footnoteSpacing = new KoUnitDoubleSpinBox( footnotePane, 0.0, 999.0, 0.5, kwhf.ptFootNoteBodySpacing, unit );
    lay->addWidget(m_footnoteSpacing);

    if ( kwhf.header == HF_FIRST_DIFF || kwhf.header == HF_FIRST_EO_DIFF )
        rhFirst->setChecked( true );
    if ( kwhf.header == HF_EO_DIFF || kwhf.header == HF_FIRST_EO_DIFF )
        rhEvenOdd->setChecked( true );
    if ( kwhf.footer == HF_FIRST_DIFF || kwhf.footer == HF_FIRST_EO_DIFF )
        rfFirst->setChecked( true );
    if ( kwhf.footer == HF_EO_DIFF || kwhf.footer == HF_FIRST_EO_DIFF )
        rfEvenOdd->setChecked( true );
}

const KoKWHeaderFooter& KoPageLayoutHeader::headerFooter() {
    if ( rhFirst->isChecked() && rhEvenOdd->isChecked() )
        m_headerFooters.header = HF_FIRST_EO_DIFF;
    else if ( rhFirst->isChecked() )
        m_headerFooters.header = HF_FIRST_DIFF;
    else if ( rhEvenOdd->isChecked() )
        m_headerFooters.header = HF_EO_DIFF;
    else
        m_headerFooters.header = HF_SAME;

    m_headerFooters.ptHeaderBodySpacing = m_headerSpacing->value();
    m_headerFooters.ptFooterBodySpacing = m_footerSpacing->value();
    m_headerFooters.ptFootNoteBodySpacing = m_footnoteSpacing->value();
    if ( rfFirst->isChecked() && rfEvenOdd->isChecked() )
        m_headerFooters.footer = HF_FIRST_EO_DIFF;
    else if ( rfFirst->isChecked() )
        m_headerFooters.footer = HF_FIRST_DIFF;
    else if ( rfEvenOdd->isChecked() )
        m_headerFooters.footer = HF_EO_DIFF;
    else
        m_headerFooters.footer = HF_SAME;
    return m_headerFooters;
}
