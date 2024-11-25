#include "ocursearch.h"
#include <QFileIconProvider>
#include <QThread>



OcurSearch::OcurSearch(const QString &fileName, QStringList items, QObject *parent)
    : QObject(parent), fileName(fileName), items(items){}

void OcurSearch::find()
{
    ocurSearch(fileName, items);
    emit finished();
}

void OcurSearch::ocurSearch(const QString &fileName, QStringList items)
{
    int totalsize = items.size();
    int size = 0;
    int numOfOcur = 0;

    int Num = 1;

    QFileIconProvider iconProvider; // Для получения иконок файлов
    for (const QString &item : items) {
        if (item.contains(fileName, Qt::CaseInsensitive)) { // Поиск по имени
            QFileInfo fileInfo(item);
            if (fileInfo.fileName().contains(fileName, Qt::CaseInsensitive)) {
                QStandardItem *standardItem = new QStandardItem(iconProvider.icon(fileInfo), fileInfo.fileName()); // Устанавливаем иконку

                QStandardItem *num = new QStandardItem(QString::number(Num));
                num->setFlags(num->flags() & ~Qt::ItemIsEditable);

                QStandardItem* size;
                if (fileInfo.isFile()) {
                    size = new QStandardItem(QString::number(double(fileInfo.size() / 1024)) + " KB");
                }
                else size = new QStandardItem("");
                size->setFlags(size->flags() & ~Qt::ItemIsEditable);


                QList<QStandardItem*> rowItems;
                rowItems << num << standardItem << size;

                emit updateModel(rowItems);

                QString count = "Found: " + QString::number(++numOfOcur);
                emit setCount(count);
                ++Num;
            }
        }
        emit setProgressBar((size + 1) * 100 / totalsize);
    }
    emit setProgressBar(100);
}

