#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "focusLineEdit.h"
#include <QPushButton>
#include <QString>
#include <QAction>
#include <QLabel>
#include <QInputDialog>
#include <QElapsedTimer>

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
        void clearMove(int, int);
        void setMove(int *moveArray, bool isCurrent);
        void clearBoard();
        void clearLabels();
        bool isFieldEnabled(int i, int j);
        bool isNotesEnabled();
        void setNotesEnabled(bool enabled);
        void changeColor(int x, int y);
        void resetColor(int x, int y);

        QAction *undoAction;
        QAction *redoAction;
        QAction *clearAction;
        QAction *clue;
        QAction *enableNotes;
        QAction *disableNotes;
        QLabel *TIMER;
        int focusedField[2];
        bool notesEnabled;

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
        void onHintPressed();
        void onEnableNotesPressed();
        void onCluePressed();

    private slots:
        void fieldChanged(QString text);
        void fieldFocused();
        void focusChanged(bool b);
        void shortcutAddNote();
        void moveUp();
        void moveDown();
        void moveRight();
        void moveLeft();

    private:
        int * createMoveArray(QString text, QString fieldname);
        void createMenu();

        Ui::MainWindow *ui;
        FocusLineEdit *fields[9][9];
        QLabel *labels[9][9];
    };
}

#endif // MAINWINDOW_H
