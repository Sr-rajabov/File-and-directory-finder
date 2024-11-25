#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QFileIconProvider>
#include <recursivesearch.h>
#include <ocursearch.h>
#include <QThread>
#include <QMutexLocker>
#include <QMessageBox>
#include <QInputDialog>
#include <QHeaderView>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    model = new QStandardItemModel(this);
    model->setHorizontalHeaderLabels(QStringList() << "№" << "Name" << "Size");

    ui->treeView->setModel(model);
    ui->dirName->setText(QDir::homePath());
    recursiveDirectorySearch(dir);
    ui->clear->hide();
    ui->searchField->setFocus();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::recursiveDirectorySearch(const QString& fileName) {
    QMutexLocker locker(&mtx);
    ui->indexBar->setValue(0);
    QThread* thread = new QThread();
    auto worker = new recursiveSearch(fileName);
    worker->moveToThread(thread);

    connect(thread, &QThread::started, worker, &recursiveSearch::search);
    connect(worker, &recursiveSearch::updateDir, this, [this](const QString& dir){
        items << dir;
    });
    connect(worker, &recursiveSearch::finished, thread, &QThread::quit);
    connect(worker, &recursiveSearch::finished, worker, &recursiveSearch::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    thread->start();
    ui->indexBar->setValue(100);
}

void MainWindow::on_openFile_clicked()
{
    dir = QFileDialog::getExistingDirectory(this, "Select directory", QDir::homePath(),
                                            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    ui->dirName->setText(dir);
    if (!dir.isEmpty()) {
        items.clear();
        recursiveDirectorySearch(dir);
        model->clear();
        on_searchField_textChanged();
        ui->searchField->setFocus();
    }
}

void MainWindow::on_searchField_textChanged()
{
    QMutexLocker locker(&mtx);
    QString text = ui->searchField->toPlainText();

    ui->searchBar->setValue(0);
    if (text.isEmpty()){
        model->clear();
        ui->clear->hide();
        ui->statusbar->clearMessage();
    }
    else {
        ui->clear->show();
        model->clear();

        QThread* thread = new QThread();

        auto worker = new OcurSearch(text, items);

        worker->moveToThread(thread);

        connect(thread, &QThread::started, worker, &OcurSearch::find);
        connect(worker, &OcurSearch::updateModel, this, [this](QList<QStandardItem*> value){
            model->appendRow(value);
        });

        connect(worker, &OcurSearch::setCount, this, [this](const QString& value){
            ui->statusbar->showMessage(value);
        });
        connect(worker, &OcurSearch::setProgressBar, this, [this](const int value) {
            ui->searchBar->setValue(value);
        });
        connect(worker, &OcurSearch::setProgressBar, this, [this](const int value) {
            ui->searchBar->setValue(value);
        });
        connect(worker, &OcurSearch::finished, thread, &QThread::quit);
        connect(worker, &OcurSearch::finished, worker, &OcurSearch::deleteLater);
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);

        thread->start();
    }
    model->setHorizontalHeaderLabels(QStringList() << "№" << "Name" << "Size");

    ui->treeView->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->treeView->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents); // Столбец "Имя" растягивается на всё доступное место
    ui->treeView->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
}

void MainWindow::on_clear_clicked()
{
    ui->searchField->clear();
    model->clear();
    ui->statusbar->clearMessage();
    ui->searchBar->setValue(0);
    model->setHorizontalHeaderLabels(QStringList() << "№" << "Name" << "Size");
    ui->searchField->setFocus();

}

void MainWindow::on_delete_2_clicked()
{
    QModelIndex index = ui->treeView->currentIndex();

    QModelIndex secondCol = model->index(index.row(), 1);

    if (!secondCol.isValid()) return;

    QString fileName = model->itemFromIndex(secondCol)->text();
    QString filePath = "";
    int i = 0;
    for (; i < items.size(); ++i) {
        if (items.at(i).contains(fileName, Qt::CaseInsensitive)) {
            filePath = items.at(i);
            break;
        }
    }
    if (!QFileInfo::exists(filePath)) {
        QMessageBox::warning(this, "delete", "File doesn't exist");
        model->removeRow(secondCol.row(), secondCol.parent());
        items.remove(i);
        return;
    }

    if (QMessageBox::question(this, "delete", "Sure want to delete it?") == QMessageBox::Yes) {
        if (QFile::remove(filePath) || QDir(filePath).removeRecursively()){
            model->removeRow(secondCol.row(), secondCol.parent());
            items.remove(i);
        }
        else {
            QMessageBox::warning(this, "Error", "This file could not be deleted.");
        }
    }
}

void MainWindow::on_rename_clicked()
{
    QModelIndex index = ui->treeView->currentIndex();

    QModelIndex secondCol = model->index(index.row(), 1);

    if (!secondCol.isValid()) return;

    ui->treeView->selectionModel()->select(secondCol, QItemSelectionModel::Select);
    QString fileName = model->itemFromIndex(secondCol)->text();

    QString oldFilePath = "";
    int i = 0;
    for (; i < items.size(); ++i) {
        if (items.at(i).contains(fileName, Qt::CaseInsensitive)) {
            oldFilePath = items.at(i);
            break;
        }
    }

    bool ok;
    QString newName = QInputDialog::getText(this, "Rename", "Enter a new name:",
        QLineEdit::Normal, model->itemFromIndex(secondCol)->text(), &ok);


    if (ok && !newName.isEmpty()) {

        QFileInfo fileinfo(oldFilePath);
        QString newFilePath = fileinfo.absolutePath() + '/' + newName;

        QDir dir(fileinfo.absolutePath());
        QStringList files = dir.entryList(QDir::Files | QDir::NoDotAndDotDot);
        for (const QString &file : files) {
            if (file.compare(newName, Qt::CaseInsensitive) == 0) {
                QMessageBox::warning(this, "Error", "A file with that name already exists");
                return;
            }
        }

        if (QFile::rename(oldFilePath, newFilePath)) {
            model->itemFromIndex(secondCol)->setText(newName);
            items.removeAll(oldFilePath);
            items.append(newFilePath);
        } else {
            QMessageBox::warning(this, "Error", "The file could not be renamed");
        }
    }
}

void MainWindow::on_updateButton_clicked()
{
    items.clear();
    ui->statusbar->clearMessage();
    recursiveDirectorySearch(dir);
    on_searchField_textChanged();
    ui->searchField->setFocus();

}

