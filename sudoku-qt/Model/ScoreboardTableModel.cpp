
#include "ScoreboardTableModel.h"
#include <Qfont>
#include <QDebug>

namespace Model
{
    //TODO: Implement Sort method and Sort by score

    ScoreboardTableModel::ScoreboardTableModel(QObject *parent, QList<QPair<QString,int>> *scoreList)
        :QAbstractTableModel(parent)
    {
        qDebug() << "ScoreboardTableModel Constructor";
        scoreboardList = (scoreList != NULL)? scoreList: new QList<QPair<QString,int>>();
    }

    ScoreboardTableModel::~ScoreboardTableModel()
    {
        qDebug() << "Disposing of current Scoreboard";
        if(scoreboardList != NULL)
        {
            /*
            qDebug() << "ScoreboardList is not Null";
            qDebug() << this->debug();
            for(int i = 0; i < scoreboardList->count(); i++)
            {
                qDebug() << "\tDeleting element of scoreboard";
                delete &(scoreboardList->at(i));
            }
            qDebug() << "\tDeleting scoreboardList";
            delete scoreboardList;
            */
            //scoreboardList->clear();
        }
        qDebug() << "\tSetting scoreboard to Null";
        scoreboardList = NULL;
        qDebug() << "Finished Disposing";
    }

    int ScoreboardTableModel::rowCount(const QModelIndex & /*parent*/)const
    {
        if(scoreboardList != NULL)
            return scoreboardList->size();
        else
            return 0;
    }

    int ScoreboardTableModel::columnCount(const QModelIndex & /*parent*/)const
    {
        return 2;
    }

    QVariant ScoreboardTableModel::headerData(int section, Qt::Orientation orientation, int role) const
    {
        if(role == Qt::DisplayRole)
        {
            if(orientation == Qt::Horizontal)
            {
                switch(section)
                {
                    case 0:
                        return QString("Player");
                    case 1:
                        return QString("Score");
                }
            }
        }

        return QVariant();
    }

    QVariant ScoreboardTableModel::data(const QModelIndex & index, int role) const
    {
        int row = index.row();
        int col = index.column();
        QString cellText = "";

        switch(role)
        {
            case Qt::FontRole:
            {

            }

            case Qt::DisplayRole:
            {
                if(scoreboardList != NULL)
                {
                    if(col == 0)
                        cellText = (scoreboardList->at(row).first);
                    else if(col > 0) cellText = QString("%1").arg(scoreboardList->at(row).second);

                    return cellText;
                }
                else
                    return QString("%1").arg("");
            }

            case Qt::BackgroundRole:
            {

            }

            case Qt::TextAlignmentRole:
            {

            }

            case Qt::CheckStateRole:
            {

            }
        }
        return QVariant();
    }

    Qt::ItemFlags ScoreboardTableModel::flags(const QModelIndex & /*index*/) const
    {
        return Qt::ItemIsEnabled;
    }

    void ScoreboardTableModel::sort(int column, Qt::SortOrder order)
    {
        qDebug() << "Sorting ScoreboardList...";
        if(scoreboardList!=NULL)
        {
            qSort(scoreboardList->begin(), scoreboardList->end(), CompareScores());
        }
    }

    QString ScoreboardTableModel::debug()
    {
        QString returnString = "ScoreboardTableModel:\n";
        if(scoreboardList != NULL)
        {
            returnString.append("\tScoreboardList has ").append(scoreboardList->count()).append(" elements\n");

            for(int i = 0; i < scoreboardList->length(); i++)
            {
                returnString.append("\t").append(scoreboardList->at(i).first.toUtf8().constData()).append(
                        "\t").append(scoreboardList->at(i).second).append("\n");
            }
        }
        else
        {
            returnString.append("\tScoreboardList = NULL\n");
        }

        return returnString;
    }

    void ScoreboardTableModel::addScore(QString PlayerName, int score)
    {
        qDebug()<<"ScoreboardTableModel: Adding " << score << " points to " << PlayerName.toUtf8().constData() << "'s score";

        if(scoreboardList != NULL)
        {
            qDebug()<<"Before Inserting Row: " << this->debug().toUtf8().constData();
            beginInsertRows(QModelIndex(), scoreboardList->size(), scoreboardList->size());
            scoreboardList->append(*(new QPair<QString,int>(PlayerName, score)));
            endInsertRows();
            qDebug()<<"After Inserting Row: " << this->debug().toUtf8().constData();
        }
    }
}
