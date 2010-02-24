#ifndef CLWIDGET_H
#define CLWIDGET_H

#include <QWidget>
#include <QEventLoop>
#include "qclcontext.h"

class CLWidget : public QWidget
{
public:
    CLWidget(QWidget *parent = 0);

    bool contextCreated();
    void setEventLoop(QEventLoop *loop) { eventLoop = loop; }
    void setup(int radius);
    void startBlur(int radius);

protected:
    void paintEvent(QPaintEvent *);

    void populateSourceImages();
    void adjustSourceImageSizes(QSize newSize);

private:
    QEventLoop *eventLoop;

    QCLContext context;
    QCLProgram program;
    QCLKernel horizontalGaussianKernel;
    QCLKernel verticalGaussianKernel;

    QImage srcImages[9];
    QImage dstImages[9 * 15];
    QCLImage2D srcImageBuffers[9];
    QCLImage2D tmpImageBuffers[9];
    QCLImage2D dstImageBuffers[9];

    QVector<float> offsets;
    QVector<float> weights;
    QCLVector<float> weightsBuffer;
    QCLVector<float> offsetsBuffer;

    QSize largestSourceImageSize;

    // old variables
//    QImage dstImage;
//    QCLImage2D srcImageBuffer;
//    QCLImage2D dstImageBuffer;
//    QCLImage2D tmpImageBuffer;
};

#endif // CLWIDGET_H
