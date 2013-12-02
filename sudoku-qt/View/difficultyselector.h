#ifndef DIFFICULTYSELECTOR_H
#define DIFFICULTYSELECTOR_H

#include <QDialog>

namespace Ui {
class DifficultySelector;
}

class DifficultySelector : public QDialog
{
    Q_OBJECT

public:
    int DS;
    explicit DifficultySelector(QWidget *parent = 0);
    ~DifficultySelector();

private slots:

    void EasyClicked(){

        DS = 1;
        close();
    }

    void MediumClicked(){

        DS = 2;
        close();
    }
    void HardClicked(){

        DS = 3;
        close();
    }

signals:
    void on_EasyButton_clicked();
    void on_MediumButton_clicked();
    void on_HardButton_clicked();
private:
    Ui::DifficultySelector *ui;
};

#endif // DIFFICULTYSELECTOR_H
