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
#include "playback.h"
#include <QHBoxLayout>
#include <QStyle>
#include <QTime>
#include <QMessageBox>
#include <QAudioBufferOutput>
#include <QAudioBuffer>
#include <QFile>

Playback::Playback(QWidget *parent) : QWidget(parent) {
    setFocusPolicy(Qt::StrongFocus);
    player = nullptr;
    audioOutput = nullptr;
    setupUI();
}

void Playback::setupUI() {
    prevButton = new QPushButton;
    prevButton->setIcon(style()->standardIcon(QStyle::SP_MediaSeekBackward));

    startStopButton = new QPushButton;
    startStopButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));

    nextButton = new QPushButton;
    nextButton->setIcon(style()->standardIcon(QStyle::SP_MediaSeekForward));

    playbackBar = new QSlider(Qt::Horizontal);
    playbackBar->setRange(0, 100);

    volumeBar = new QSlider(Qt::Horizontal);
    volumeBar->setRange(0, 100);
    volumeBar->setValue(50);
    volumeBar->setFixedWidth(100);

    time = new QLabel("00:00 / 00:00");

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(prevButton);
    layout->addWidget(startStopButton);
    layout->addWidget(nextButton);
    layout->addWidget(playbackBar);
    layout->addWidget(time);
    layout->addWidget(volumeBar);

    setLayout(layout);

    QFile styleFile(":/styles.qss");
    if (styleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        setStyleSheet(styleFile.readAll());
        styleFile.close();
    }
}

void Playback::setAudio(QMediaPlayer* med, QAudioOutput* out) {
    player = med;
    audioOutput = out;
    startStopButton->setEnabled(true);
    playbackBar->setEnabled(true);
    setupAudioConnections();
}

void Playback::setupAudioConnections() {
    if (!player) return;
    connect(player, &QMediaPlayer::positionChanged, this, &Playback::updatePlaybackPosition);
    connect(player, &QMediaPlayer::durationChanged, this, &Playback::durationChanged);
    connect(player, &QMediaPlayer::playbackStateChanged, this, &Playback::handlePlaybackState);
    connect(player, &QMediaPlayer::errorOccurred, player, [this](){QMessageBox::critical(this, "Error", player->errorString());});
    connect(startStopButton, &QPushButton::clicked, this, &Playback::playPause);
    connect(prevButton, &QPushButton::clicked, this, &Playback::prev);
    connect(nextButton, &QPushButton::clicked, this, &Playback::next);
    connect(playbackBar, &QSlider::sliderMoved, this, [this](int position) {player->setPosition(position);});
    connect(volumeBar, &QSlider::valueChanged, this, [this](int volume) {audioOutput->setVolume(volume / 100.0);});

}

void Playback::keyPressEvent(QKeyEvent *event) {
    if (!player) return;
    auto key = event->key();
    if (key == Qt::Key_Left)
        prev();
    else if (key == Qt::Key_Right)
        next();
    else if (key == Qt::Key_Space || key == Qt::Key_P)
        playPause();
}

void Playback::playPause() {
    if (!player) return;
    if (player->playbackState() == QMediaPlayer::PlayingState) {
        player->pause();
    } else {
        player->play();
    }
}

void Playback::prev() {
    if (!player) return;
    qint64 newPosition = player->position() - 5000;

    if (newPosition < 0) {
        newPosition = 0;
    }

    player->setPosition(newPosition);
}

void Playback::next() {
    if (!player) return;
    qint64 newPosition = player->position() + 5000;
    qint64 duration = player->duration();

    if (newPosition > duration) {
        newPosition = duration-100;
    }

    player->setPosition(newPosition);
}

void Playback::updatePlaybackPosition(qint64 position) {
    if (!player) return;
    if (!playbackBar->isSliderDown()) {
        playbackBar->setValue(position);
    }

    QTime currentTime(0, 0);
    currentTime = currentTime.addMSecs(position);
    QTime totalTime(0, 0);
    totalTime = totalTime.addMSecs(player->duration());

    time->setText(
        currentTime.toString("mm:ss") + " / " +
        totalTime.toString("mm:ss")
    );
}

void Playback::durationChanged(qint64 duration) {
    if (!player) return;
    playbackBar->setRange(0, duration);
}

void Playback::handlePlaybackState(QMediaPlayer::PlaybackState state) {
    if (!player) return;
    switch(state) {
        case QMediaPlayer::PlayingState:
            startStopButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
            break;
        case QMediaPlayer::PausedState:
            startStopButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
            break;
        case QMediaPlayer::StoppedState:
            startStopButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
            playbackBar->setValue(0);
            break;
    }
}
