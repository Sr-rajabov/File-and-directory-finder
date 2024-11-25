#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QScopedPointer>
#include <QMutex>
#include <QDir>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_openFile_clicked();

    void on_searchField_textChanged();

    void on_clear_clicked();

    void on_delete_2_clicked();

    void on_rename_clicked();

    void on_updateButton_clicked();

private:

    void recursiveDirectorySearch(const QString& fileName);

    Ui::MainWindow *ui;
    QString dir = QDir::homePath();
    QStandardItemModel* model;
    QStringList items;
    QMutex mtx;
};
#endif // MAINWINDOW_H
