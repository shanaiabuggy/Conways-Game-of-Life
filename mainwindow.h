#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "game.h"
#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void countTurn();
    void countPop(int x);
    int countCellNeighbors(int i, int j);
    void calculateCellState();
    bool isCellAlive(int i, int j);

    int currPopulation;

public slots:
    void recieve_inc();

    void recieve_dec();

private slots:

    void on_resetButton_clicked();

    void on_startButton_clicked();

    void on_pauseButton_clicked();

    void on_horizontalSlider_valueChanged(int value);

    void on_stepButton_clicked();


    void on_checkBox_stateChanged(int arg1);

private:
    Ui::MainWindow *ui;
    QGraphicsScene *BuildCellGrid_;
    QGraphicsScene *BuildPopGraph_;
    int cellWidth_;
    int cellHeight_;
    int barWidth_;
    int barHeight_;
    int turn_ = 0;
    int y_bar;
    double h_bar;
    double speed_ = 1.0;
    QTimer *timer;
    Cell * cells_matrix[10][20];
    Cell * big_cells_matrix[20][40];
    bool big_flag;
    std::vector <popBar*> bars_;
};

#endif // MAINWINDOW_H
