#ifndef PIXMAPFILTERWIDGET_H
#define PIXMAPFILTERWIDGET_H

#include <QWidget>
#include <QtGui/private/qpixmapfilter_p.h>
#include <QEventLoop>

class PixmapFilterWidget : public QWidget
{
    Q_OBJECT
public:
    PixmapFilterWidget(QWidget *parent = 0);

    void setFilter(QPixmapFilter *f) { filter = f; }
    void setEventLoop(QEventLoop *loop) { eventLoop = loop; }

    void setPixmapOffset(int value) { pixmapOffset = value * 9; }

    void setUseOpenVG(bool value) { useOpenVG = value; }

protected:
    void paintEvent(QPaintEvent *);

private:
    QPixmap pixmaps[9 * 17];
    QPixmapFilter *filter;
    QEventLoop *eventLoop;
    int pixmapOffset;
    bool useOpenVG;
#ifdef OPENVG_RAW_TEST
    VGImage tempImage[17];
#endif
};

#endif // PIXMAPFILTERWIDGET_H
