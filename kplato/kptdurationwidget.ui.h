//
// ui.h extension file, included from the uic-generated form implementation.
//
// If you wish to add, delete or rename functions or slots use
// Qt Designer which will update this file, preserving your code. Create an
// init() function in place of a constructor, and a destroy() function in
// place of a destructor.
//

/**
 * This structure describes one of the fields shown.
 */
struct FieldDescriptor
{
    // Which field is to my left, and what conversion factor separates us?
    QLineEdit *left;
    unsigned leftScale;
    
    // Which field am I, and how am I formatted?
    QLineEdit *current;
    const char *format;
    
    // Which field is to my right, and what conversion factor separates us?
    QLineEdit *right;
    unsigned rightScale;
    
    // If I am hidden, who else hides with me?
    QLabel *separator;
};

#define setField(f, l, ls, c, fmt, r, rs, s) \
do \
{ \
    m_fields[f].left = l; \
    m_fields[f].leftScale = ls; \
    m_fields[f].current = c; \
    m_fields[f].format = fmt; \
    m_fields[f].right = r; \
    m_fields[f].rightScale = rs; \
    m_fields[f].separator = s; \
} while (0)
    
void KPTDurationWidget::init()
{
    // Use the user's decimal point!
    m_decimalPoint = tr2i18n(".");
    
    // Any field can be entered as an integer or a floating point value. Whatever
    // is entered is treated as follows:
    //
    //    - any fractional part is moved right one field
    //
    //    - any overflow from the integer part is carried left one field
    //
    // and the process repeated until the rightmost and leftmost fields are reached.
    QRegExp re(QString("\\d{1,10}|\\d{1,5}\\") + m_decimalPoint + 
        QString("\\d{0,5}|\\d{0,5}\\") + m_decimalPoint + 
        QString("\\d{1,5}"));
    m_validator = new QRegExpValidator(re, this);
    m_ddd->setValidator(m_validator);
    m_hh->setValidator(m_validator);
    m_mm->setValidator(m_validator);
    m_ss->setValidator(m_validator);
    m_ms->setValidator(m_validator);
    
    m_fields = new FieldDescriptor[5];    
    setField(0, NULL, 0, m_ddd, "%u", m_hh, 24, m_hhSpace);
    setField(1, m_ddd, 24, m_hh, "%02u", m_mm, 60, m_mmColon);
    setField(2, m_hh, 60, m_mm, "%02u", m_ss, 60, NULL);
    setField(3, m_mm, 60, m_ss, "%02u", m_ms, 1000, m_ssColon);
    setField(4, m_ss, 1000, m_ms, "%03u", NULL, 0, m_dot);
}

void KPTDurationWidget::destroy()
{
    delete m_fields;
    delete m_validator;
}

void KPTDurationWidget::setValue(const KPTDuration &newDuration)
{
    int i;
    unsigned v[5];
    QString tmp;
    
    newDuration.get(&v[0], &v[1], &v[2], &v[3], &v[4]);
    for (i = 0; i < 5; i++)
    {
        tmp.sprintf(m_fields[i].format, v[i]);
        m_fields[i].current->setText(tmp);
    }
    emit valueChanged();
}

KPTDuration KPTDurationWidget::value() const
{
    int i;
    unsigned v[5];
    
    for (i = 0; i < 5; i++)
    {
        v[i] = m_fields[i].current->text().toUInt();

        // Ignore hidden field contributions.
        if (m_fields[i].current->isHidden())
            v[i] = 0;
    }
    KPTDuration tmp(v[0], v[1], v[2], v[3], v[4]);
    return tmp;
}

void KPTDurationWidget::dddLostFocus()
{
    handleLostFocus(0);
    emit valueChanged();
}

void KPTDurationWidget::hhLostFocus( )
{
    handleLostFocus(1);
    emit valueChanged();
}

void KPTDurationWidget::mmLostFocus()
{
    handleLostFocus(2);
    emit valueChanged();
}

void KPTDurationWidget::ssLostFocus()
{
    handleLostFocus(3);
    emit valueChanged();
}

void KPTDurationWidget::msLostFocus()
{
    handleLostFocus(4);
    emit valueChanged();
}

void KPTDurationWidget::handleLostFocus(
    int field)
{
    // Get our own info, and that of our left and right neighbours.
    QLineEdit *left = m_fields[field].left;
    unsigned leftScale = m_fields[field].leftScale; 
    const char *leftFormat = left ? m_fields[field - 1].format : NULL; 
    QLineEdit *current = m_fields[field].current; 
    const char *currentFormat = m_fields[field].format;
    QLineEdit *right = m_fields[field].right;
    unsigned rightScale = m_fields[field].rightScale; 
    const char *rightFormat = right ? m_fields[field + 1].format : NULL;
    
    // Get the text and start processing...
    QString newValue(current->text());
    unsigned currentValue;
    QString tmp;
    int point = newValue.find(m_decimalPoint);
    
    if (point != -1)
    {
        if (right)
        {
            // Propagate fraction: we just truncate any new fractional part.
            tmp.sprintf(rightFormat, (unsigned)(rightScale * newValue.mid(point).toDouble()));
            right->setText(tmp);
        }
        
        // Truncate the fractional value.
        newValue = newValue.left(point);
    }
    currentValue = newValue.toUInt();

    if (left)
    {
        if (currentValue >= leftScale)
        {
            // Carry overflow, recurse as required.
            tmp.sprintf(leftFormat, currentValue / leftScale);
            left->setText(tmp);
            handleLostFocus(field - 1);

            // Get remainder.
            currentValue = currentValue % leftScale;
        }
    }
    tmp.sprintf(currentFormat, currentValue);
    current->setText(tmp);
}

// Set which fields are visible.
void KPTDurationWidget::setVisibleFields( int fieldMask )
{
    int i;
    for (i = 0; i < 5; i++)
    {
        bool visible = ((fieldMask >> i) & 1) == 1;
        
        // Set the visibility of the fields, and of any associated separator.
        if (visible)
        {
            m_fields[i].current->show();
            if (m_fields[i].separator)
            {
                m_fields[i].separator->show();
            }
        }
        else
        {
            m_fields[i].current->hide();
            if (m_fields[i].separator)
            {
                m_fields[i].separator->hide();
            }
        }
    }
}

// Retreive the visible fields.
int KPTDurationWidget::visibleFields()
{
    int i;
    int fieldMask = 0;
    for (i = 0; i < 5; i++)
    {
        if (m_fields[i].current->isHidden())
        {   
            fieldMask |= (1 << i);
        }
    }
    return fieldMask;
}

void KPTDurationWidget::setFieldLeftscale(int f, unsigned ls)
{
    m_fields[f].leftScale = ls;
}

void KPTDurationWidget::setFieldRightscale(int f, unsigned rs)
{
    m_fields[f].rightScale = rs;
}
