#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QString>
#include <QAction>

namespace Ui {
class MainWindow;
}
namespace View {

    class MainWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();
        void setDefaultMove(int *moveArray);
        void makeMove(int *moveArray);
        void clearBoard();

        QAction *undoAction;
        QAction *redoAction;

    private slots:
        void fieldChanged(QString text);

    signals:
        void onLoadProgressPressed();
        void onSaveProgressPressed();
        void onLoadPuzzlePressed();
        void onSavePuzzlePressed();
        void onMakeMove(int *moveArray);
        void onUndoPressed();
        void onRedoPressed();

    private:
        Ui::MainWindow *ui;

        int * createMoveArray(QString text, QString fieldname);
        void createMenu();

        QLineEdit *fields[9][9];




    };
}

#endif // MAINWINDOW_H
