#include <QSyntaxHighlighter>

 class FunctionNameSpellingHighlighter : public QSyntaxHighlighter //used for highlighting the letters that were wrongly typed in a function name
 {
     

     
    void FunctionNameSpellingHighlighter::highlightBlock(const QString &text) //reimplementing QSyntaxHighlighter's highLightBlock() method
 {
     QTextCharFormat changedFunctionNameCharacters;
     changedFunctionNameCharacters.setFontWeight(QFont::Bold);// the changed letters will be shown in bold and dark blue colour
    changedFunctionNameCharacters.setForeground(Qt::darkBlue);
     QString pattern = "\\bMy[A-Za-z]+\\b";

     QRegExp expression(pattern);
     int index = text.indexOf(expression);
     while (index >= 0) {
         int length = expression.matchedLength();
         setFormat(index, length, changedFunctionNameCharacters);
         index = text.indexOf(expression, index + length);
     }
 }