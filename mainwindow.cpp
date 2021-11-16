#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <vector>

/* Overall 'Big grid' New Feature Note: Most functions check if the big_grid flag is set to true or false. If this flag is true, a 20x40 matrix
 * will be used instead of a 10x20. Function operations are the same for both cells_matrix and big_cells_matrix, but have adjusted
 * bounds/indices as well as adjusted population formulas to fit each matrix size.
 */
/*
    Return nothing(constructor)
    @param QWidget
    @return Sets up the cell grid, population bar, and labels
*/
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    turn_=0;

    // Build starting cell field
    BuildCellGrid_ = new QGraphicsScene;
    QGraphicsView * grid_view = ui->MainPlot;

    grid_view->setScene(BuildCellGrid_);
    grid_view->setSceneRect(0,0,grid_view->frameSize().width(),grid_view->frameSize().height());

    cellHeight_ = grid_view->frameSize().height();
    cellWidth_ = grid_view->frameSize().width();

    ui->label_5->setText(QString("Speed: ")+QString::number(speed_, 'f', 6));

    srand(time(0));
    // initial cell grid is 10x20
    for(int i = 0; i < 10; i++) {
        for(int j = 0; j < 20; j++) {
            Cell * item = new Cell(j, i, cellWidth_/20, cellHeight_/10);
            cells_matrix[i][j] = item;
            BuildCellGrid_->addItem(item);
            connect(item, &Cell::increase, this, &MainWindow::recieve_inc);
            connect(item, &Cell::decrease, this, &MainWindow::recieve_dec);

            if(item->getColor()== QColor(255,  166,  201)) {
                countPop(1);
            }
        }
    }
    ui->label->setText(QString("Turn: ")+QString::number(turn_));
    ui->label_2->setText(QString("Population: ")+QString::number(currPopulation)+ QString(" (") + QString::number(( currPopulation*100 )/200 ) + QString("%)" ));
    // Build population graph

    BuildPopGraph_ = new QGraphicsScene;
    QGraphicsView * graph_view = ui->MainPlot_2;
    graph_view->setScene(BuildPopGraph_);
    graph_view->setSceneRect(0,0,graph_view->frameSize().width(),graph_view->frameSize().height());
    double percent = (double(currPopulation) /200.0);
    y_bar=graph_view->frameSize().height()-2;
    h_bar = graph_view->frameSize().height() ;
    popBar* first_bar = new popBar(0, y_bar, int(percent * h_bar));
    bars_.push_back(first_bar);
    BuildPopGraph_->addItem(first_bar);

    timer=new QTimer();

    connect(timer, SIGNAL(timeout()), this, SLOT(on_startButton_clicked()));

}

// Deconstructor
MainWindow::~MainWindow()
{
    delete ui;
}

/*
    Increases the turn_ variable and
    updates the turn label
    @param nothing
    @return nothing
*/
void MainWindow::countTurn()
{
    turn_++;
    ui->label->setText(QString("Turn: ")+QString::number(turn_));
}


/*
    Check if a cell is alive or dead based off the color of the cell, if alive/pink/yellow return true,
    if dead/white return false
    @param nothing
    @return bool
*/
bool MainWindow::isCellAlive(int i, int j){
    bool flag = false;
    if (big_flag == false) {
        if(cells_matrix[i][j]->getColor() != QColor(255,  255,  255)){
            flag = true;
        }
    }
    else if (big_flag == true) {
        if(big_cells_matrix[i][j]->getColor() != QColor(255,  255,  255)){
            flag = true;
        }
    }
    return flag;
}


/*
    Checks all surrounding cells for live cells by calling the isCellAlive func. Adds the boolean values
    returned by isCellAlive to get the total number of live neighbors.
    @param nothing
    @return bool
*/
int MainWindow::countCellNeighbors(int j, int i){

    int count = 0;
    if (big_flag == false) {
        if (i == 0){
            if (j == 0) {
                // top left corner
                count += isCellAlive(0,1)+isCellAlive(1,0)+isCellAlive(1,1)+isCellAlive(0,19)+isCellAlive(1,19)+isCellAlive(9,0)+isCellAlive(9,1)+isCellAlive(9,19);
            }
            else if (j == 19) {
                // top right corner
                count += isCellAlive(0,18)+isCellAlive(1,18)+isCellAlive(1,19)+isCellAlive(0,0)+isCellAlive(1,0)+isCellAlive(9,18)+isCellAlive(9,19)+isCellAlive(9,0);
            }
            else {
                // top row
                count += isCellAlive(9,j-1)+isCellAlive(9,j)+isCellAlive(9,j+1)+isCellAlive(i,j-1)+isCellAlive(i,j+1)+isCellAlive(i+1,j-1)+isCellAlive(i+1,j)+isCellAlive(i+1,j+1);
            }
        }
        else if (i == 9) {
            if (j == 0) {
                // bottom left corner
                count += isCellAlive(8,19)+isCellAlive(8,0)+isCellAlive(8,1)+isCellAlive(9,19)+isCellAlive(9,1)+isCellAlive(0,19)+isCellAlive(0,0)+isCellAlive(0,1);
            }
            else if (j == 19) {
                // bottom right corner
                count += isCellAlive(8,18)+isCellAlive(8,19)+isCellAlive(8,0)+isCellAlive(9,18)+isCellAlive(9,0)+isCellAlive(0,18)+isCellAlive(0,19)+isCellAlive(0,0);
            }
            else {
                // bottom row
                count += isCellAlive(0,j-1)+isCellAlive(0,j)+isCellAlive(0,j+1)+isCellAlive(i,j-1)+isCellAlive(i,j+1)+isCellAlive(i-1,j-1)+isCellAlive(i-1,j)+isCellAlive(i-1,j+1);
            }
        }
        else if (j == 0) {
            // left column
            count += isCellAlive(i-1,19)+isCellAlive(i,19)+isCellAlive(i+1,19)+isCellAlive(i-1,j)+isCellAlive(i+1,j)+isCellAlive(i-1,j+1)+isCellAlive(i,j+1)+isCellAlive(i+1,j+1);
        }
        else if (j == 19) {
            // right column
            count += isCellAlive(i-1,0)+isCellAlive(i,0)+isCellAlive(i+1,0)+isCellAlive(i-1,j)+isCellAlive(i+1,j)+isCellAlive(i-1,j-1)+isCellAlive(i,j-1)+isCellAlive(i+1,j-1);
        }
        else {
            // non edge bordering cell
            count += isCellAlive(i-1,j-1)+isCellAlive(i-1,j)+isCellAlive(i-1,j+1)+isCellAlive(i,j-1)+isCellAlive(i,j+1)+isCellAlive(i+1,j-1)+isCellAlive(i+1,j)+isCellAlive(i+1,j+1);
        }
    }
    else if (big_flag == true) {
        if (i == 0){
            if (j == 0) {
                // top left corner
                count += isCellAlive(0,1)+isCellAlive(1,0)+isCellAlive(1,1)+isCellAlive(0,39)+isCellAlive(1,39)+isCellAlive(19,0)+isCellAlive(19,1)+isCellAlive(19,39);
            }
            else if (j == 39) {
                // top right corner
                count += isCellAlive(0,38)+isCellAlive(1,38)+isCellAlive(1,39)+isCellAlive(0,0)+isCellAlive(1,0)+isCellAlive(19,38)+isCellAlive(19,39)+isCellAlive(19,0);
            }
            else {
                // top row
                count += isCellAlive(19,j-1)+isCellAlive(19,j)+isCellAlive(19,j+1)+isCellAlive(i,j-1)+isCellAlive(i,j+1)+isCellAlive(i+1,j-1)+isCellAlive(i+1,j)+isCellAlive(i+1,j+1);
            }
        }
        else if (i == 19) {
            if (j == 0) {
                // bottom left corner
                count += isCellAlive(18,39)+isCellAlive(18,0)+isCellAlive(18,1)+isCellAlive(19,39)+isCellAlive(19,1)+isCellAlive(0,39)+isCellAlive(0,0)+isCellAlive(0,1);
            }
            else if (j == 39) {
                // bottom right corner
                count += isCellAlive(18,38)+isCellAlive(18,39)+isCellAlive(18,0)+isCellAlive(19,38)+isCellAlive(19,0)+isCellAlive(0,38)+isCellAlive(0,39)+isCellAlive(0,0);
            }
            else {
                // bottom row
                count += isCellAlive(0,j-1)+isCellAlive(0,j)+isCellAlive(0,j+1)+isCellAlive(i,j-1)+isCellAlive(i,j+1)+isCellAlive(i-1,j-1)+isCellAlive(i-1,j)+isCellAlive(i-1,j+1);
            }
        }
        else if (j == 0) {
            // left column
            count += isCellAlive(i-1,39)+isCellAlive(i,39)+isCellAlive(i+1,39)+isCellAlive(i-1,j)+isCellAlive(i+1,j)+isCellAlive(i-1,j+1)+isCellAlive(i,j+1)+isCellAlive(i+1,j+1);
        }
        else if (j == 39) {
            // right column
            count += isCellAlive(i-1,0)+isCellAlive(i,0)+isCellAlive(i+1,0)+isCellAlive(i-1,j)+isCellAlive(i+1,j)+isCellAlive(i-1,j-1)+isCellAlive(i,j-1)+isCellAlive(i+1,j-1);
        }
        else {
            // non edge bordering cell
            count += isCellAlive(i-1,j-1)+isCellAlive(i-1,j)+isCellAlive(i-1,j+1)+isCellAlive(i,j-1)+isCellAlive(i,j+1)+isCellAlive(i+1,j-1)+isCellAlive(i+1,j)+isCellAlive(i+1,j+1);
        }
    }
    return count;
}


/*
    Calls countCellNeighbors to get the count of live neighbors a cell has then uses
    Conway's 4 rules to determine if the cell state for the next turn. The population bar graph is also
    updated so that it will continuouesly move to the right.
    @param nothing
    @return nothing
*/
void MainWindow::calculateCellState(){
    // determine the state of the cell based off 4 rules in conways game of life

    int neighbors_count = 0;
    if (big_flag == false) {
        QColor new_colors[10][20]; // Matrix of new color states
        for(int i = 0; i < 10; i++) {
            for(int j = 0; j < 20; j++) {
                neighbors_count = countCellNeighbors(j,i);
                // check if cell currently alive
                if (cells_matrix[i][j]->getColor() == QColor(255,  166,  201) ||cells_matrix[i][j]->getColor() == QColor(252, 239, 145)) {
                    // check if cell dies (underpopulation)
                    if(neighbors_count < 2) {
                        new_colors[i][j] = QColor(255, 255, 255); // kill cell
                        currPopulation--;
                    }
                    // check if cell dies (overpopulation)
                    else if (neighbors_count > 3) {
                        new_colors[i][j] = QColor(255, 255, 255); // kill cell
                        currPopulation--;
                    }
                    else {
                        // cell has 2 or 3 neighbors and is stable
                        new_colors[i][j] = QColor(255,  166,  201);
                    }
                }
                // check if cell currently dead
                else if (cells_matrix[i][j]->getColor() == QColor(255,  255,  255)) {
                    // check if cell becomes live (reproduction)
                    if (neighbors_count == 3){
                        // NEW FEATURE: new live cells made active by reproduction are yellow
                        new_colors[i][j] = QColor(252, 239, 145); // cell becomes alive (yellow - new feature)
                        currPopulation++;
                    }
                    else {
                        new_colors[i][j] = QColor(255,  255,  255); // cell remains dead
                    }
                }
            }
        }
        // update cells_matrix to new cell states
        for(int i = 0; i < 10; i++) {
            for(int j = 0; j < 20; j++) {
                cells_matrix[i][j]->setColor(new_colors[i][j]);
            }
        }

    }

    else if (big_flag == true) {
        QColor new_colors[20][40]; // Matrix of new color states
        for(int i = 0; i < 20; i++) {
            for(int j = 0; j < 40; j++) {
                neighbors_count = countCellNeighbors(j,i);
                // check if cell currently alive
                if (big_cells_matrix[i][j]->getColor() == QColor(255,  166,  201) ||big_cells_matrix[i][j]->getColor() == QColor(252, 239, 145)) {
                    // check if cell dies (underpopulation)
                    if(neighbors_count < 2) {
                        new_colors[i][j] = QColor(255, 255, 255); // kill cell
                        currPopulation--;
                    }
                    // check if cell dies (overpopulation)
                    else if (neighbors_count > 3) {
                        new_colors[i][j] = QColor(255, 255, 255); // kill cell
                        currPopulation--;
                    }
                    else {
                        // cell has 2 or 3 neighbors and is stable
                        new_colors[i][j] = QColor(255,  166,  201);
                    }
                }
                // check if cell currently dead
                else if (big_cells_matrix[i][j]->getColor() == QColor(255,  255,  255)) {
                    // check if cell becomes live (reproduction)
                    if (neighbors_count == 3){
                        new_colors[i][j] = QColor(252, 239, 145); // cell becomes alive (yellow - new feature)
                        currPopulation++;
                    }
                    else {
                        new_colors[i][j] = QColor(255,  255,  255); // cell remains dead
                    }
                }
            }
        }
        // update big_cells_matrix to new cell states
        for(int i = 0; i < 20; i++) {
            for(int j = 0; j < 40; j++) {
                big_cells_matrix[i][j]->setColor(new_colors[i][j]);
            }
        }
    }

    BuildCellGrid_->update();
    if (currPopulation < 0){
        currPopulation = 0;
    }

    countPop(0);
    double percent;
    if (big_flag == true) {
        percent = (double(currPopulation) /800.0);
    }
    else {
        percent = (double(currPopulation) /200.0);
    }


    // set each bar back -1 bar
    if (bars_.size() > 20) {  // 600/30 = 20
        int prev = 0;
        for (popBar* bar : bars_) {
            bar->setX(-1 * bar->getWidth()); // set each bar back -1 bar
            prev = bar->getX();
        }
        popBar* bar = new popBar(prev+30 , y_bar, int(percent * h_bar));
        bars_.push_back(bar);
        BuildPopGraph_->addItem(bar);

    }
    //add the bars to the right
    else {
       popBar* bar = new popBar((turn_*30)+30, y_bar, int(percent * h_bar));
       bars_.push_back(bar);
       BuildPopGraph_->addItem(bar);
    }

    BuildPopGraph_->update();
    countTurn();

}

/*
    Increases or decreases the population and sets the population label accordingly
    @param population
    @return nothing
*/
void MainWindow::countPop(int new_pop) {
    currPopulation += new_pop;
    if (big_flag == true) {
        ui->label_2->setText(QString("Population: ")+QString::number(currPopulation)+QString(" (")+QString::number((currPopulation*100)/800)+QString("%)"));
    }
    else {
        ui->label_2->setText(QString("Population: ")+QString::number(currPopulation)+QString(" (")+QString::number((currPopulation*100)/200)+QString("%)"));
    }
}

/*
    Resets the whole program to the starting state.
    @param nothing
    @return nothing
*/
void MainWindow::on_resetButton_clicked()
{
    BuildCellGrid_->clear();
    BuildPopGraph_->clear();
    BuildPopGraph_->update();
    ui->checkBox->setChecked(false); // uncheck big grid
    turn_=0;
    currPopulation=0;
    srand(time(0));
    for(int i = 0; i < 10; i++) {
        for(int j = 0; j < 20; j++) {
            Cell * item = new Cell(j, i, cellWidth_/20, cellHeight_/10);
            cells_matrix[i][j] = item;
            BuildCellGrid_->addItem(item);
            connect(item, &Cell::increase, this, &MainWindow::recieve_inc);
            connect(item, &Cell::decrease, this, &MainWindow::recieve_dec);

            if(item->getColor()== QColor(255,  166,  201))
            {
                countPop(1);
            }
        }
    }
    bars_.clear();
    double percent = (double(currPopulation) /200.0);
    popBar* first_bar = new popBar(0, 98, int(percent * 100));
    bars_.push_back(first_bar);
    BuildPopGraph_->addItem(first_bar);

}

/*
    Starts the timer. The population bar graph and cell grid are
    populated from the calculateCellState function.
    @param nothing
    @return nothing
*/
void MainWindow::on_startButton_clicked()
{
    calculateCellState();
    timer->start(speed_*1000.0);
}

/*
    Pauses the timer when the pause button is clicked.
    @param nothing
    @return nothing
*/
void MainWindow::on_pauseButton_clicked()
{
    timer->stop();
}

/*
    Adjusts the label of the slider
    and grabs the value/speed from the slider
    @param value
    @return nothing
*/
void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    speed_=1-(value/100.0);
    ui->label_5->setText(QString("Speed: ")+QString::number(speed_, 'f', 6));

}

/*
    Steps through the program one step at a time.
    Calls the calculateCellState function to populate the population graph and
    the cell grid.
    @param nothing
    @return nothing
*/
void MainWindow::on_stepButton_clicked()
{
    calculateCellState();
}

/*
    Increases the population
    @param nothing
    @return nothing
*/
void MainWindow::recieve_inc(){
    countPop(1);
}

/*
    Decreases the population
    @param nothing
    @return nothing
*/
void MainWindow::recieve_dec(){
    countPop(-1);
}


void MainWindow::on_checkBox_stateChanged(int arg1)
{
    if (arg1){
        qDebug() << "big check!";
        big_flag = true;
        // reset grid to big grid
        BuildCellGrid_->clear();
        BuildPopGraph_->clear();
        BuildPopGraph_->update();
        turn_=0;
        currPopulation=0;
        srand(time(0));
        for(int i = 0; i < 20; i++) {
            for(int j = 0; j < 40; j++) {
                Cell * item = new Cell(j, i, cellWidth_/40, cellHeight_/20);
                big_cells_matrix[i][j] = item;
                BuildCellGrid_->addItem(item);
                connect(item, &Cell::increase, this, &MainWindow::recieve_inc);
                connect(item, &Cell::decrease, this, &MainWindow::recieve_dec);

                if(item->getColor()== QColor(255,  166,  201)) {
                    countPop(1);
                }
            }
        }
        bars_.clear();
        double percent = (double(currPopulation) /800.0);
        popBar* first_bar = new popBar(0, 98, int(percent * 100));
        bars_.push_back(first_bar);
        BuildPopGraph_->addItem(first_bar);
    }
    if (!arg1){
        big_flag = false;
        on_resetButton_clicked();
    }
}

