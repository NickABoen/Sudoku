#ifndef FOCUSLINEEDIT_H
#define FOCUSLINEEDIT_H

#include <Qwidget>
#include <QLineEdit>
#include <QDebug>
#include <QKeyEvent>

class FocusLineEdit : public QLineEdit
{
  Q_OBJECT

public:
  FocusLineEdit(QWidget *parent = 0);
  ~FocusLineEdit();

signals:
  void focused(bool hasFocus);
  void moveRight();
  void moveLeft();

protected:
  virtual void focusInEvent(QFocusEvent *e);
  virtual void focusOutEvent(QFocusEvent *e);
  void keyPressEvent(QKeyEvent *e);
};

#endif
