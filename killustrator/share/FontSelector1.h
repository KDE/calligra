// See FontSelector.h

#ifndef FONTSELECTOR_1_H
#define FONTSELECTOR_1_H

class FontSelector : public QWidget {
    Q_OBJECT

public:
    FontSelector (QWidget *parent, const char *name = 0L,
		  const char* example = "Text",
		  const QStrList* fontlist = 0L );

    void setFont (const QFont &font);
    QFont font () { return selFont; }

signals:
	/*
	 * connect to this to monitor the font as it as selected if you are
	 * not running modal.
	 */
	void fontSelected (const QFont &font);

private slots:
      void 	family_chosen_slot (const char *);
      void      size_chosen_slot   (const char *);
      void      weight_chosen_slot (const char *);
      void      style_chosen_slot  (const char *);
      void      display_example    (const QFont &font);
      void      charset_chosen_slot(int index);
      void      setColors();

private:

    bool loadKDEInstalledFonts();
    void fill_family_combo();
    void setCombos();
   
    QGroupBox	 *box1;
    QGroupBox	 *box2;
    
    // pointer to an optinally supplied list of fonts to 
    // inserted into the fontdialog font-family combo-box
    QStrList     *fontlist; 

    QLabel	 *family_label;
    QLabel	 *size_label;
    QLabel       *weight_label;
    QLabel       *style_label;
    QLabel	 *charset_label;

    QLabel	 *actual_family_label;
    QLabel	 *actual_size_label;
    QLabel       *actual_weight_label;
    QLabel       *actual_style_label;
    QLabel	 *actual_charset_label;


    QLabel	 *actual_family_label_data;
    QLabel	 *actual_size_label_data;
    QLabel       *actual_weight_label_data;
    QLabel       *actual_style_label_data;
    QLabel	 *actual_charset_label_data;
    QComboBox    *family_combo;
    QComboBox    *size_combo;
    QComboBox    *weight_combo;
    QComboBox    *style_combo;
    QComboBox	 *charset_combo;    

    QLabel       *example_label;
    QFont         selFont;
};
#endif
