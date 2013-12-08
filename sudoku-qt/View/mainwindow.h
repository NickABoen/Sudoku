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

        void makeMove(int *moveArray);
        void setMove(int *moveArray, bool isCurrent);
        void clearBoard();

        QAction *undoAction;
        QAction *redoAction;
        QAction *clearAction;

    signals:
        void onLoadProgressPressed();
        void onSaveProgressPressed();
        void onLoadPuzzlePressed();
        void onSavePuzzlePressed();
        void onMakeMove(int *moveArray);
        void onUndoPressed();
        void onRedoPressed();
        void onGenerateBoardPressed();
        void onClearPressed();

    private slots:
        void fieldChanged(QString text);

    private:
        int * createMoveArray(QString text, QString fieldname);
        void createMenu();

        Ui::MainWindow *ui;
        QLineEdit *fields[9][9];
    };
}

#endif // MAINWINDOW_H
