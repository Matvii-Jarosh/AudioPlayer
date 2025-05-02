/*
 * Copyright (c) 2025 Matvii Jarosh
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
*/
#include "artimage.h"
#include <QPainter>
#include <QPaintEvent>
#include <QMediaMetaData>
#include <QDebug>

ArtImage::ArtImage(QMediaPlayer* player, QWidget *parent) : QWidget(parent), m_player(player) {
    setFixedSize(300, 300);

    connect(player, &QMediaPlayer::metaDataChanged, this, &ArtImage::updateArtwork);
}

void ArtImage::updateArtwork() {
    QVariant coverData = m_player->metaData().value(QMediaMetaData::ThumbnailImage);

    if (coverData.isValid()) {
        m_artwork = coverData.value<QImage>();
        update();
    } else {
        m_artwork = QImage(":/images/default_cover.png");
        if (m_artwork.isNull()) {
            //qDebug() << "Default cover image not found!";
        }
        update();
    }
}

void ArtImage::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);

    if (!m_artwork.isNull()) {
        QRect targetRect = m_artwork.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation)
                               .rect();
        targetRect.moveCenter(rect().center());
        painter.drawImage(targetRect, m_artwork);
    } else {
        painter.fillRect(rect(), Qt::darkGray);
        painter.setPen(Qt::white);
        painter.drawText(rect(), Qt::AlignCenter, "No artwork available");
    }
}
