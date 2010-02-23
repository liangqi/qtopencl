#include "graphicseffectview.h"

GraphicsEffectView::GraphicsEffectView(QWidget *parent)
    : QGraphicsView(parent), eventLoop(0)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setViewportUpdateMode(FullViewportUpdate);
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform
                   | QPainter::TextAntialiasing);

    scene = new QGraphicsScene(this);
    setScene(scene);

    pixmaps[0] = new QGraphicsPixmapItem
        (QPixmap(QLatin1String(":/images/accessories-calculator.png")));
    pixmaps[1] = new QGraphicsPixmapItem
        (QPixmap(QLatin1String(":/images/accessories-text-editor.png")));
    pixmaps[2] = new QGraphicsPixmapItem
        (QPixmap(QLatin1String(":/images/help-browser.png")));
    pixmaps[3] = new QGraphicsPixmapItem
        (QPixmap(QLatin1String(":/images/internet-group-chat.png")));
    pixmaps[4] = new QGraphicsPixmapItem
        (QPixmap(QLatin1String(":/images/internet-mail.png")));
    pixmaps[5] = new QGraphicsPixmapItem
        (QPixmap(QLatin1String(":/images/internet-web-browser.png")));
    pixmaps[6] = new QGraphicsPixmapItem
        (QPixmap(QLatin1String(":/images/office-calendar.png")));
    pixmaps[7] = new QGraphicsPixmapItem
        (QPixmap(QLatin1String(":/images/system-users.png")));
    pixmaps[8] = new QGraphicsPixmapItem
        (QPixmap(QLatin1String(":/images/qt-logo.png")));

    for (int y = 0; y < 3; ++y) {
        for (int x = 0; x < 3; ++x) {
            int index = y * 3 + x;
            pixmaps[index]->setPos(x * 100 + 50, y * 100 + 50);
            scene->addItem(pixmaps[index]);
        }
    }
}

GraphicsEffectView::~GraphicsEffectView()
{
}

void GraphicsEffectView::paintEvent(QPaintEvent *event)
{
    QGraphicsView::paintEvent(event);
    if (eventLoop) {
        eventLoop->quit();
        eventLoop = 0;
    }
}
