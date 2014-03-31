#include "focusLineEdit.h"
#include <Qwidget>
#include <QLineEdit>

FocusLineEdit::FocusLineEdit(QWidget *parent)
 : QLineEdit(parent)
{}

FocusLineEdit::~FocusLineEdit()
{}

void FocusLineEdit::focusInEvent(QFocusEvent *e)
{
  QLineEdit::focusInEvent(e);
  emit(focused(true));
}

void FocusLineEdit::focusOutEvent(QFocusEvent *e)
{
  QLineEdit::focusOutEvent(e);
  emit(focused(false));
}

void FocusLineEdit::keyPressEvent(QKeyEvent *e)
{
    switch(e->key())
    {
    case Qt::Key_Left:
        emit(moveLeft());
        break;
    case Qt::Key_Right:
        emit(moveRight());
        break;
    default:
        QLineEdit::keyPressEvent(e);
    }
}
