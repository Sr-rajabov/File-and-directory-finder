#ifndef RECURSIVESEARCH_H
#define RECURSIVESEARCH_H

#include <QObject>
#include <QMutex>
#include <QStringList>

class recursiveSearch : public QObject
{
    Q_OBJECT
public:
    explicit recursiveSearch(const QString& dirPath, QObject *parent = nullptr);

signals:
    void finished();
    void updateDir(QString);


public slots:
    void search();

private:
    QString m_fileName;

    void recCall(const QString& fileName);
};

#endif // RECURSIVESEARCH_H
