#include <QSyntaxHighlighter>
//header file for FunctionNameSpellingHighlighter
class FunctionNameSpellingHighlighter : public QSyntaxHighlighter
 {
Q_OBJECT
public:
     FunctionNameSpellingHighlighter(QTextDocument *parent = 0);

 protected:
     void highlightBlock(const QString &text);//function mandatory to be implemented
}


