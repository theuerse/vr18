#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
    void on_searchButton_clicked();

    void on_colorHistButton_clicked();

    void on_edgeHistButton_clicked();

    void on_keypointsButton_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
