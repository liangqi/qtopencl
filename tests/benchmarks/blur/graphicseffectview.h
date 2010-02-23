#ifndef GRAPHICSEFFECTVIEW_H
#define GRAPHICSEFFECTVIEW_H

#include <QGraphicsView>
#include <QEventLoop>
#include <QGraphicsItem>

class GraphicsEffectView : public QGraphicsView
{
    Q_OBJECT
public:
    GraphicsEffectView(QWidget *parent = 0);
    ~GraphicsEffectView();

    void setEventLoop(QEventLoop *loop) { eventLoop = loop; }
    void updateScene() { scene->update(); }

    QGraphicsEffect *itemEffect(int item)
    {
        return pixmaps[item]->graphicsEffect();
    }

    void setItemEffect(int item, QGraphicsEffect *effect)
    {
        pixmaps[item]->setGraphicsEffect(effect);
    }

protected:
    void paintEvent(QPaintEvent *event);

private:
    QEventLoop *eventLoop;
    QGraphicsScene *scene;
    QGraphicsPixmapItem *pixmaps[9];
};

#endif // GRAPHICSEFFECTVIEW_H
