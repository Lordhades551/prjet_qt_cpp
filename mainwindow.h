#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QSqlTableModel>
#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include "boat.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void addBoat();
    void removeBoat();
    void updateDisplay();
    void refreshBoats();
    void modifyBoat();
    void searchBoatByOwner();
    void searchBoatByModel();
    void sortBoatsByModel();
    // void exportPDF();

private:
    Ui::MainWindow *ui;
    QSqlDatabase db;
    QSqlTableModel *model;
    QList<Boat> boats;
};

#endif // MAINWINDOW_H
