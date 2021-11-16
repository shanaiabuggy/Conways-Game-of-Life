#include "game.h"
#include "mainwindow.h"
#include <mainwindow.cpp>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QtWidgets>
#include <QTimer>

Cell::Cell(int x, int y, int width, int height){
    QColor color(255, 0, 0);
    // When game begins, 50 percent chance cell is alive
    if(rand() % 100 < 50) {
        color.setRgb(255,  166,  201);
    }
    else{
        color.setRgb(255, 255, 255);
    }
    color_ = color;
    x_ = x * width;
    y_ = y * height;
    width_ = width;
    height_ = height;
}

QRectF Cell::boundingRect() const
{
    return QRectF(x_, y_, width_, width_);
}

QPainterPath Cell::shape() const
{
    QPainterPath path;
    path.addRect(x_, y_, width_, width_);
    return path;
}

void Cell::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget)
    QBrush b = painter->brush();
    painter->setBrush(QBrush(color_));
    painter->drawRect(QRect(this->x_, this->y_, this->width_, this->width_));
    painter->setBrush(b);
}

popBar::popBar(const int x, const int y, int h) {

    x_ = x;
    y_ = y;
    height_ = -h; // adjusted height
    color_ = "white";

}

QRectF popBar::boundingRect() const
{
    return QRectF(x_, y_, width_, height_);
}

QPainterPath popBar::shape() const
{
    QPainterPath path;
    path.addRect(x_, y_, width_, height_);
    return path;
}


void popBar::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{

    Q_UNUSED(widget);

    QBrush b = painter->brush();
    painter->setBrush(QBrush(color_.darker(option->state & QStyle::State_Sunken ? 120 : 100)));
    painter->drawRect(this->x_, this->y_, this->width_, this->height_);
    painter->setBrush(b);
}

void Cell::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
         if(this->getColor() == QColor(255, 255, 255)){
                qDebug() << "Ressurect Cell";
                this->setColor(QColor(255,  166,  201));
                emit increase();
            }
     }
    else if (event->button() == Qt::RightButton) {
        if(this->getColor() == QColor(255,  166,  201) || this->getColor() == QColor(252, 239, 145)){
            qDebug() << "Kill cell";
            this->setColor(QColor(255,255,255));
            emit decrease();
        }
    }
    update();
}
