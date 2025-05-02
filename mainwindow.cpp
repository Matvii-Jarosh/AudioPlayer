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
#include "mainwindow.h"
#include "playback.h"

#include <QVBoxLayout>
#include <QMenuBar>
#include <QFileDialog>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QMessageBox>
#include <QUrl>
#include <QFileInfo>
#include <QTime>
#include <QStyle>
#include <aboutwindow.h>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Audio Player");
    setFixedSize(550, 400);
    setAcceptDrops(true);

    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    player->setAudioOutput(audioOutput);
    audioOutput->setVolume(0.5f);

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);

    artworkDisplay = new ArtImage(player);
    playback = new Playback();

    mainLayout->addWidget(artworkDisplay, 0, Qt::AlignHCenter);
    mainLayout->addWidget(playback);

    setCentralWidget(centralWidget);
    setupMenu();
}

void MainWindow::setupMenu() {
    QMenu* fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction("&Open", this, &MainWindow::openFile)->setShortcut(QKeySequence::Open);
    fileMenu->addSeparator();
    fileMenu->addAction("&Exit", this, &QMainWindow::close)->setShortcut(QKeySequence::Quit);

    QMenu* controlMenu = menuBar()->addMenu("&Control");
    controlMenu->addAction("&Play Pause", this, &MainWindow::handlePlayPause);
    controlMenu->addAction("&Prev", this, &MainWindow::handlePrev);
    controlMenu->addAction("&Next", this, &MainWindow::handleNext);

    QMenu* helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction("&About Audio Player", this, &MainWindow::showAbout);
}

void MainWindow::handlePlayPause() {
    playback->playPause();
}

void MainWindow::handlePrev() {
    playback->prev();
}

void MainWindow::handleNext() {
    playback->next();
}

void MainWindow::showAbout() {
    AboutWindow* ab = new AboutWindow();
    ab->show();
}

void MainWindow::openFile() {
    QStringList filters = {
        "Audio Files (*.mp3 *.wav *.ogg *.flac *.aac)",
        "MP3 (*.mp3)",
        "WAV (*.wav)",
        "OGG (*.ogg)",
        "FLAC (*.flac)",
        "All Files (*)"
    };

    QString filePath = QFileDialog::getOpenFileName(
        this,
        "Open Audio File",
        QDir::homePath(),
        filters.join(";;")
        );

    if (!filePath.isEmpty()) {
        loadAudioFile(filePath);
    }
}

void MainWindow::loadAudioFile(const QString &filePath) {
    if (player->isAvailable())
        player->stop();

    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists()) {
        QMessageBox::warning(this, "Error", "File does not exist");
        return;
    }

    setWindowTitle("Audio Player - " + fileInfo.fileName());
    player->setSource(QUrl::fromLocalFile(filePath));
    playback->setAudio(player, audioOutput);

    connect(player, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::LoadedMedia) {
            player->setPosition(500);
            player->play();
            disconnect(player, &QMediaPlayer::mediaStatusChanged, this, nullptr);
        }
    });
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *event) {
    const QMimeData *mimeData = event->mimeData();

    if (mimeData->hasUrls()) {
        QList<QUrl> urlList = mimeData->urls();
        QString filePath = urlList.first().toLocalFile();

        if (QFileInfo(filePath).suffix().toLower() == "mp3" ||
            QFileInfo(filePath).suffix().toLower() == "wav" ||
            QFileInfo(filePath).suffix().toLower() == "ogg" ||
            QFileInfo(filePath).suffix().toLower() == "flac") {
            loadAudioFile(filePath);
        }
    }
}

MainWindow::~MainWindow() {
    delete player;
    delete audioOutput;
}
