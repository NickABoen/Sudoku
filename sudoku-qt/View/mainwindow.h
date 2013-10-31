#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QString>

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
        void createLayout();
        void createMenu();

        QLineEdit *fields[9][9];

        QPushButton* undoButton;
        QPushButton* redoButton;
        QPushButton* loadPuzzleButton;
        QPushButton* savePuzzleButton;
        QPushButton* loadProgressButton;
        QPushButton* saveProgressButton;

    };
}

#endif // MAINWINDOW_H
