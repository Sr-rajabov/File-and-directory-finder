#include "recursivesearch.h"
#include <QDir>
#include <QFileInfo>
#include <QFileIconProvider>
#include <QThread>

recursiveSearch::recursiveSearch(const QString& dirPath, QObject *parent)
    : QObject{parent}, m_fileName(dirPath) {}

void recursiveSearch::search() {
    recCall(m_fileName);
    emit finished();
}

void recursiveSearch::recCall(const QString& fileName) {
    QDir dir(fileName);
    QFileInfoList fileInfoList = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries);

    for (const QFileInfo &fileInfo : fileInfoList) {
        emit updateDir(fileInfo.filePath()); // Добавляем путь к файлу или папке
        if (fileInfo.isDir()) {
            recCall(fileInfo.filePath()); // Рекурсивный обход
        }
    }
}
