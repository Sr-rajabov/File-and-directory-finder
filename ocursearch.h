#ifndef OCURSEARCH_H
#define OCURSEARCH_H

#include <QObject>
#include <QStandardItem>
#include <QStringList>
#include <QList>

class OcurSearch : public QObject
{
    Q_OBJECT
public:
    explicit OcurSearch(const QString& fileName, QStringList items, QObject *parent = nullptr);

public slots:
    void find();

signals:
    void finished();
    void setProgressBar(int);
    void setCount(QString);
    void clear(bool);
    void updateModel(QList<QStandardItem*>);

private:
    QString fileName;
    QStringList items;

    void ocurSearch(const QString& fileName, QStringList items);
};

#endif // OCURSEARCH_H
