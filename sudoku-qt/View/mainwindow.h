#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QString>
#include <QAction>
#include <QLabel>
#include <QInputDialog>

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
        bool isFieldEnabled(int i, int j);
        bool isNotesEnabled();
        void setNotesEnabled(bool enabled);

        QAction *undoAction;
        QAction *redoAction;
        QAction *clearAction;

        QAction *enableNotes;
        QAction *disableNotes;

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

    private slots:
        void fieldChanged(QString text);
        void fieldFocused();

    private:
        int * createMoveArray(QString text, QString fieldname);
        void createMenu();

        bool notesEnabled;
        Ui::MainWindow *ui;
        QLineEdit *fields[9][9];
        QLabel *labels[9][9];
    };
}

#endif // MAINWINDOW_H
