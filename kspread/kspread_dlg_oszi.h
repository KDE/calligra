#ifndef KSPREAD_DLG_OSZI
#define KSPREAD_DLG_OSZI

#include <qdialog.h>

class KSpreadView;

class AIPlot;

class QLineEdit;
class QGroupBox;
class QPushButton;
class QSlider;

class KSpreadOsziDlg : public QDialog
{
    Q_OBJECT
public:
    KSpreadOsziDlg( KSpreadView*, const char* name = 0 );
    
private slots:
    void slotCalc();
    
private:
    KSpreadView* m_view;
    QLineEdit* m_from1;
    QLineEdit* m_to1;
    QLineEdit* m_cell1;
    QLineEdit* m_from2;
    QLineEdit* m_to2;
    QLineEdit* m_cell2;
    QLineEdit* m_resultCell1;
    QLineEdit* m_resultCell2;

    QGroupBox* m_data1;
    QGroupBox* m_data2;
    QGroupBox* m_result1;
    QGroupBox* m_result2;
    QGroupBox* m_time;
    
    QSlider* m_slider;
    QPushButton* m_start;
    QPushButton* m_stop;
    
    AIPlot* m_plot;
};

#endif
