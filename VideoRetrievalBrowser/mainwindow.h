#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <QColorDialog>
#include "clickablelabel.h"


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
    void on_ClickableLabel_clicked();

    void on_hsvToleranceSlider_valueChanged(int value);

    void on_adjacentFramesButton_clicked();

    void on_sendSelectedFrameButton_clicked();

    void on_CNNsearchButton_clicked();

    void on_colorSearchButton_clicked();

    void on_edgeSearchButton_clicked();

    void on_keypointSearchButton_clicked();

private:
    Ui::MainWindow *ui;
    QGridLayout *gridLayout;
    QColorDialog *colorPicker;
    ClickableLabel *lastSelectedImage = nullptr;

    int gridLayoutCounter_row;
    int gridLayoutCounter_col;
    int gridLayoutMaxCols;
    QString shotpath;

    /**
     * @brief Puts an image it on the gridview to be displayed and manages gridLayout's index overflows
     * @param img
     * @todo override QWidget::resizeEvent() to get correct gridLayoutMaxCols after resize.
     */
    void displayImage(QString img);

    /**
     * @brief Deletes all images that are currently displayed on the gridLayout
     * @note Does nothing if there are no images in gridLayout
     */
    void deleteDisplayedImages();

    /**
     * @brief Creates,sets up and configures color picker on its place UI window
     */
    void setupColorPicker();
};

#endif // MAINWINDOW_H
