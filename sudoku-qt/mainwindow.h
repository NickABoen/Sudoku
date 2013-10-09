#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void fieldChanged(QString text);

private:
    Ui::MainWindow *ui;

    void createLayout();

    QLineEdit *fields[10][10];
};

#endif // MAINWINDOW_H
