/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtOpenCL module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qclimageformat.h"

QT_BEGIN_NAMESPACE

/*!
    \class QCLImageFormat
    \brief The QCLImageFormat class represents the format of a QCLImage.
    \since 4.7
    \ingroup opencl
*/

/*!
    \enum QCLImageFormat::ChannelOrder
    This enum defines the order of R, G, B, and A channels in an image.

    \value Order_R Single red channel.
    \value Order_A Single alpha channel.
    \value Order_RG Red then green channel.
    \value Order_RA Red then alpha channel.
    \value Order_RGB Red, green, and blue channels.
    \value Order_RGBA Red, green, blue, then alpha channels.
    \value Order_BGRA Blue, green, red, then alpha channels.
    \value Order_ARGB Alpha, red, green, then blue channels.
    \value Order_Intensity Single intensity channel.
    \value Order_Luminence Single luminence channel.
*/

/*!
    \enum QCLImageFormat::ChannelType
    This enum defines the representation type for an image channel.

    \value Type_Normalized_Int8 Signed 8-bit integer, normalized to
           the range -1.0 to 1.0.
    \value Type_Normalized_Int16 Signed 8-bit integer, normalized to
           the range -1.0 to 1.0.
    \value Type_Normalized_UInt8 Unsigned 8-bit integer, normalized
           to the range 0.0 to 1.0.
    \value Type_Normalized_UInt16 Unsigned 16-bit integer, normalized
           to the range 0.0 to 1.0.
    \value Type_Normalized_565 RGB565 representation.
    \value Type_Normalized_555 RGB555 representation.
    \value Type_Normalized_101010 RGB-10-10-10 representation with
           10 bits of precision for each channel.
    \value Type_Unnormalized_Int8 Unnormalized signed 8-bit integer.
    \value Type_Unnormalized_Int16 Unnormalized signed 16-bit integer.
    \value Type_Unnormalized_Int32 Unnormalized signed 32-bit integer.
    \value Type_Unnormalized_UInt8 Unnormalized unsigned 8-bit integer.
    \value Type_Unnormalized_UInt16 Unnormalized unsigned 16-bit integer.
    \value Type_Unnormalized_UInt32 Unnormalized unsigned 32-bit integer.
    \value Type_Half_Float Half-precision floating-point value.
    \value Type_Float Single-precision floating-point value.
*/

/*!
    \fn QCLImageFormat::QCLImageFormat()

    Constructs a null OpenCL image format descriptor.

    \sa isNull()
*/

/*!
    Constructs an OpenCL image format descriptor from \a order and \a type.

    This constructor will infer the closest matching QImage format
    to return from toQImageFormat(), which may be QImage::Format_Invalid
    if \a order and \a type do not correspond to a QImage format.
*/
QCLImageFormat::QCLImageFormat
    (QCLImageFormat::ChannelOrder order, QCLImageFormat::ChannelType type)
{
    m_format.image_channel_order = order;
    m_format.image_channel_data_type = type;
    m_qformat = QImage::Format_Invalid;
    if (order == Order_RGB) {
        if (type == Type_Normalized_565)
            m_qformat = QImage::Format_RGB16;
        else if (type == Type_Normalized_555)
            m_qformat = QImage::Format_RGB555;
        else if (type == Type_Normalized_UInt8)
            m_qformat = QImage::Format_RGB888;
    } else if (order == Order_BGRA && type == Type_Normalized_UInt8) {
        if (QSysInfo::ByteOrder == QSysInfo::LittleEndian)
            m_qformat = QImage::Format_ARGB32;
    } else if (order == Order_ARGB && type == Type_Normalized_UInt8) {
        if (QSysInfo::ByteOrder != QSysInfo::LittleEndian)
            m_qformat = QImage::Format_ARGB32;
    } else if (order == Order_A) {
        // We assume that Indexed8 images are alpha maps for font glyphs.
        m_qformat = QImage::Format_Indexed8;
    }
}


/*!
    Constructs an OpenCL image format descriptor that is equivalent to
    the specified QImage \a format.  If the \a format does not have
    an OpenCL equivalent, the descriptor will be set to null.

    \sa isNull(), toQImageFormat()
*/
QCLImageFormat::QCLImageFormat(QImage::Format format)
{
    m_qformat = format;

    switch (format) {
    case QImage::Format_Indexed8:
        // We assume that Indexed8 images are alpha maps for font glyphs.
        m_format.image_channel_order = Order_A;
        m_format.image_channel_data_type = Type_Normalized_UInt8;
        break;

    case QImage::Format_RGB32:
    case QImage::Format_ARGB32:
    case QImage::Format_ARGB32_Premultiplied:
        if (QSysInfo::ByteOrder == QSysInfo::LittleEndian) {
            m_format.image_channel_order = Order_BGRA;
            m_format.image_channel_data_type = Type_Normalized_UInt8;
        } else {
            m_format.image_channel_order = Order_ARGB;
            m_format.image_channel_data_type = Type_Normalized_UInt8;
        }
        break;

    case QImage::Format_RGB16:
        m_format.image_channel_order = Order_RGB;
        m_format.image_channel_data_type = Type_Normalized_565;
        break;

    case QImage::Format_RGB555:
        m_format.image_channel_order = Order_RGB;
        m_format.image_channel_data_type = Type_Normalized_555;
        break;

    case QImage::Format_RGB888:
        m_format.image_channel_order = Order_RGB;
        m_format.image_channel_data_type = Type_Normalized_UInt8;
        break;

    default:
        // Everything else is null.
        m_format.image_channel_order = 0;
        m_format.image_channel_data_type = 0;
        m_qformat = QImage::Format_Invalid;
        break;
    }
}

/*!
    \fn bool QCLImageFormat::isNull() const

    Returns true if this OpenCL image format descriptor is null.
*/

/*!
    \fn QCLImageFormat::ChannelOrder QCLImageFormat::channelOrder() const

    Returns the order of channels in this OpenCL image format.

    \sa channelType()
*/

/*!
    \fn QCLImageFormat::ChannelType QCLImageFormat::channelType() const

    Returns the representation type for channels in this OpenCL image format.

    \sa channelOrder()
*/

/*!
    \fn bool QCLImageFormat::operator==(const QCLImageFormat &other)

    Returns true if this image format is the same as \a other;
    false otherwise.

    \sa operator!=()
*/

/*!
    \fn bool QCLImageFormat::operator!=(const QCLImageFormat &other)

    Returns true if this image format is not the same as \a other;
    false otherwise.

    \sa operator==()
*/

/*!
    \fn QImage::Format QCLImageFormat::toQImageFormat() const

    Returns the nearest QImage format to this OpenCL image format;
    QImage::Format_Invalid if there is no corresponding QImage format.
*/

QT_END_NAMESPACE
