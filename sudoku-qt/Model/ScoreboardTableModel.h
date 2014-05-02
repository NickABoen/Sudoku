#ifndef SCOREBOARDTABLEMODEL_H
#define SCOREBOARDTABLEMODEL_H

#include <QAbstractTableModel>

namespace Model
{
    const int COLS = 2;
    const int ROWS = 2;

    class ScoreboardTableModel : public QAbstractTableModel
    {
        Q_OBJECT
    public:
        ScoreboardTableModel(QObject * parent, QList<QPair<QString, int> > *scoreList);
        ~ScoreboardTableModel();

        int rowCount(const QModelIndex &parent = QModelIndex()) const;
        int columnCount(const QModelIndex &parent = QModelIndex()) const;
        QVariant data (const QModelIndex &index, int role = Qt::DisplayRole)const;
        Qt::ItemFlags flags(const QModelIndex & index) const;
        QVariant headerData(int section, Qt::Orientation orientation, int role) const;
        void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);
        void addScore(QString PlayerName, int score);
        QString debug();

    private:
        QString m_gridData[ROWS][COLS]; // holds text entered int QTableView
        QList<QPair<QString,int>> *scoreboardList;

        //Structs
            struct CompareScores
            {
                template<typename T1, typename T2>
                bool operator()(const QPair<T1,T2> & a, const QPair<T1, T2> & b) const
                {
                    return a.second > b.second;
                }
            };
    signals:
        void editCompleted(const QString &);
    };
}

#endif // SCOREBOARDTABLEMODEL_H
