#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>

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
    void on_debugButton_clicked();

private:
    Ui::MainWindow *ui;
    QGridLayout *gridLayout;
    int gridLayoutCounter_row;
    int gridLayoutCounter_col;
    int gridLayoutMaxRows;
    int gridLayoutMaxCols;

};

#endif // MAINWINDOW_H
