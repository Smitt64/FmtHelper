#include "codeeditor.h"
#include <QRectF>
#include <QInputDialog>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QScrollBar>
#include <QMenu>

CodeEditor::CodeEditor(QWidget *parent) : QTextEdit(parent)
{
    lineNumberArea = new LineNumberArea(this);

    connect(this->document(), SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    //connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

    connect(verticalScrollBar(), SIGNAL(sliderMoved(int)), this, SLOT(onScroll(int)));

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();

    context = this->createStandardContextMenu();
    addWordToDb = context->addAction(tr("Add word to dictionary"));
    setContextMenuPolicy(Qt::CustomContextMenu);

    setFontFamily("Courier New");
    setTabStopWidth(QFontMetrics(font()).width(' ') * 4);

    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));
    connect(addWordToDb, SIGNAL(toggled(bool)), this, SLOT(addWordAction(bool)));
}

void CodeEditor::showContextMenu(const QPoint&)
{
    context->exec(QCursor::pos());
}

void CodeEditor::addWordAction(bool)
{
    QString str = textCursor().selectedText();

    if (str.isEmpty())
        return;

    bool ok = true;
    QString s = QInputDialog::getText(this, tr("Add selected word"), tr("Add selected word:"), QLineEdit::Normal, str, &ok);

    if (ok)
    {
        QSqlDatabase db = QSqlDatabase::database("wordbd");
        QSqlQuery q;
        q.prepare("insert into words(en) values(?)");
        q.bindValue(0, s);
        q.exec();
    }
}

int CodeEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, document()->blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 9 + fontMetrics().width(QLatin1Char('9')) * digits;

    return space;
}

void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void CodeEditor::resizeEvent(QResizeEvent *e)
{
    QTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::onScroll(int Value)
{
    lineNumberArea->update();
}

void CodeEditor::wheelEvent(QWheelEvent *event)
{
    QTextEdit::wheelEvent(event);
    lineNumberArea->update();
}

void CodeEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    lineNumberArea->update();
    /*if (!isReadOnly()) {*/
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(Qt::yellow).lighter(160);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    /*}*/

    setExtraSelections(extraSelections);
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    QRect rc = event->rect();
    rc.setWidth(lineNumberAreaWidth());
    painter.fillRect(event->rect(), Qt::lightGray);


    QTextBlock block = document()->firstBlock();
    int blockNumber = block.blockNumber();
    int top = (int) document()->documentLayout()->blockBoundingRect(block).translated(QPoint(0, -verticalScrollBar()->value())).top();
    int bottom = top + (int) document()->documentLayout()->blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom())
    {
        if (block.isVisible() && bottom >= event->rect().top())
        {
            QFont f = painter.font();
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            f.setBold(false);

            if (block == this->textCursor().block())
            {
                painter.setPen(QColor(Qt::red).darker());
                f.setBold(true);
            }

            painter.setFont(f);
            painter.drawText(0, top, rc.width(), fontMetrics().height(),
                             Qt::AlignCenter, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) document()->documentLayout()->blockBoundingRect(block).height();
        ++blockNumber;
    }
}
