#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <QSyntaxHighlighter>

namespace Ui {
class Highlighter;
}

class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    explicit Highlighter(QTextDocument *parent = 0);
    ~Highlighter();
};

#endif // TEXTEDIT_H
