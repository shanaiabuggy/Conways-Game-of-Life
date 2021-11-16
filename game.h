#ifndef GAME_H
#define GAME_H

#include <QColor>
#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QtWidgets>
#include <QObject>

class Cell : public QObject, public QGraphicsItem {
    Q_OBJECT

public:
    Cell(int x, int y, int width, int height); //constructor
    int getX() const { return x_; }  // inline member function
    int getY() const { return y_; }
    int getWidth() { return width_; }
    int getHeight() { return height_; }
    QColor getColor() { return color_; }
    void setColor(QColor c){
        color_ = c;
    }

    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) override;

signals:
    void increase();
    void decrease();
    void neighbors(int x,int y);

private:
  int x_;
  int y_;
  int width_;
  int height_;
  QColor color_; // used to identify is cell alive (pink/yellow) or dead (white)

protected:
  void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

};

class popBar : public QObject, public QGraphicsItem
{
    Q_OBJECT

public:
    popBar(int x, int y, int h);
    static int getWidth() { return width_; }
    int getX() { return x_; }
    void setX(int offset) { x_ = x_ + offset; }

    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) override;

private:
    int x_;
    int y_;
    QColor color_;
    static const int width_ = 30; // 600 / 20
    int height_;

};


#endif // GAME_H
