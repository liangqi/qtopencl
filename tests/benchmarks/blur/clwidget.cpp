#include "clwidget.h"
#include <QtCore/qmath.h>
#include <QPainter>
#include <QTest>

static const qreal Q_2PI = qreal(6.28318530717958647693); // 2*pi

static inline qreal gaussian(qreal dx, qreal sigma)
{
    return exp(-dx * dx / (2 * sigma * sigma)) / (Q_2PI * sigma * sigma);
}


CLWidget::CLWidget(QWidget *parent)
    : QWidget(parent), eventLoop(0)
{
    if (!context.create())
        qFatal("Could not create OpenCL context");
}

bool CLWidget::contextCreated()
{
    return context.isCreated();
}

static inline int roundUp(int value, int to)
{
    int remainder = value % to;
    if (remainder)
        return value - remainder + to;
    else
        return value;
}

void CLWidget::populateSourceImages()
{
    srcImages[0] = QImage(QLatin1String(":/images/accessories-calculator.png"));
    srcImages[1] = QImage(QLatin1String(":/images/accessories-text-editor.png"));
    srcImages[2] = QImage(QLatin1String(":/images/help-browser.png"));
    srcImages[3] = QImage(QLatin1String(":/images/internet-group-chat.png"));
    srcImages[4] = QImage(QLatin1String(":/images/internet-mail.png"));
    srcImages[5] = QImage(QLatin1String(":/images/internet-web-browser.png"));
    srcImages[6] = QImage(QLatin1String(":/images/office-calendar.png"));
    srcImages[7] = QImage(QLatin1String(":/images/system-users.png"));
    srcImages[8] = QImage(QLatin1String(":/images/qt-logo.png"));

    largestSourceImageSize = QSize();
    for(int i = 0; i < 9; ++i)
    {
        largestSourceImageSize = largestSourceImageSize.expandedTo(srcImages[i].size());
    }
}

void CLWidget::adjustSourceImageSizes(QSize newSize)
{
    for (int index = 0; index < 9; ++index) {
        // Adjust for the best work size on the kernel.

        if(srcImages[index].size() == newSize)
            continue;
        QImage resizedSource(newSize, QImage::Format_ARGB32);
        resizedSource.fill(0);
        QPainter p(&resizedSource);
        QPoint topLeft = QPoint(
                (resizedSource.width() - srcImages[index].width()) / 2,
                (resizedSource.height() - srcImages[index].height()) / 2);
        p.drawImage( topLeft , srcImages[index] );
        p.end();
        srcImages[index] = resizedSource;
    }
}

void CLWidget::setup( int maxRadius ) {
    program = context.buildProgramFromSourceFile(QLatin1String(":/blur.cl"));
    horizontalGaussianKernel = program.createKernel("hgaussian");
    verticalGaussianKernel = program.createKernel("vgaussian");

    populateSourceImages();

    // Adjust for the best work size on the kernel.
    QCLWorkSize bestSize = horizontalGaussianKernel.bestLocalWorkSizeImage2D();


    QSize adjustedSize = largestSourceImageSize + QSize((maxRadius + 1) * 2, (maxRadius + 1) * 2);
    adjustedSize = QSize(roundUp(adjustedSize.width(), bestSize.width()),
                         roundUp(adjustedSize.height(), bestSize.height()));


    adjustSourceImageSizes(adjustedSize);

    for (int index = 0; index < 9; ++index) {
        // Adjust for the best work size on the kernel.

        srcImageBuffers[index] = context.createImage2DCopy(srcImages[index], QCL::ReadOnly);
        tmpImageBuffers[index] = context.createImage2DDevice(QImage::Format_ARGB32, adjustedSize, QCL::ReadWrite);

        dstImages[index] = QImage(adjustedSize, QImage::Format_ARGB32);
        dstImageBuffers[index] = context.createImage2DDevice(dstImages[index].format(), dstImages[index].size(), QCL::WriteOnly);
    }

    horizontalGaussianKernel.setGlobalWorkSize(adjustedSize);
    verticalGaussianKernel.setGlobalWorkSize(adjustedSize);
    horizontalGaussianKernel.setLocalWorkSize(bestSize);
    verticalGaussianKernel.setLocalWorkSize(bestSize);

    weightsBuffer = context.createVector<float>(100);
    offsetsBuffer = context.createVector<float>(100);
};

void CLWidget::startBlur(int radius)
{
    // build weights and offset vectors
    // (Would this be faster on the gpu?)
    QVector<qreal> components;
    qreal sigma = radius / 1.65;
    qreal sum = 0;
    offsets.clear();
    weights.clear();
    for (int i = -radius; i <= radius; ++i) {
        qreal value = gaussian(i, sigma);
        components.append(value);
        sum += value;
    }
    for (int i = 0; i < components.size(); ++i)
        components[i] /= sum;
    for (int i = 0; i < components.size() - 1; i += 2) {
        qreal weight = components[i] + components[i + 1];
        qreal offset = i - radius + components[i + 1] / weight;
        offsets.append(offset);
        weights.append(weight);
    }
    // odd size ?
    if (components.size() & 1) {
        offsets.append(radius);
        weights.append(components[components.size() - 1]);
    }

    // Upload the weights and offsets into OpenCL.
    offsetsBuffer.write(offsets);
    weightsBuffer.write(weights);

    // Execute the horizontal and vertical gaussian kernels.
    for(int i = 0; i < 9; i++)
    {
        horizontalGaussianKernel(srcImageBuffers[i], tmpImageBuffers[i], weightsBuffer, offsetsBuffer, weights.size());
        verticalGaussianKernel(tmpImageBuffers[i], dstImageBuffers[i], weightsBuffer, offsetsBuffer, weights.size());
    }
}

void CLWidget::paintEvent(QPaintEvent *)
{
    if (!eventLoop)
        return;

    QPainter painter;
    painter.begin(this);
    for (int y = 0; y < 3; ++y) {
        for (int x = 0; x < 3; ++x) {
            int index = y * 3 + x;
            QPointF dest(x * 100 + (100 - dstImages[index].width()) / 2,
                         y * 100 + (100 - dstImages[index].height()) / 2);

            // Extra copy here could potentially be eliminated
            dstImageBuffers[index].read(&(dstImages[index]));
            painter.drawImage(dest, dstImages[index]);
        }
    }

    painter.end();
    eventLoop->quit();
    eventLoop = 0;
};
