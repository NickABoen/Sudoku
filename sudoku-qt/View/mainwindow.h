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

    private slots:
        void fieldChanged(QString text);

    signals:
        void onLoadProgressPressed();
        void onSaveProgressPressed();
        void onLoadPuzzlePressed();
        void onSavePuzzlePressed();
        void onMakeMove(QString text);
        void onUndo();
        void onRedo();

    private:
        Ui::MainWindow *ui;

        void createLayout();

        QLineEdit *fields[10][10];

        QPushButton* undoButton;
        QPushButton* redoButton;
        QPushButton* loadPuzzleButton;
        QPushButton* savePuzzleButton;
        QPushButton* loadProgressButton;
        QPushButton* saveProgressButton;

    };
}

#endif // MAINWINDOW_H
