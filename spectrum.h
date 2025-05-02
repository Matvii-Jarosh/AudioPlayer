#ifndef SPECTRUM_H
#define SPECTRUM_H

#include <QWidget>
#include <QVector>

class Spectrum : public QWidget
{
    Q_OBJECT
public:
    explicit Spectrum(QWidget *parent = nullptr);
    void processAudioData(const QVector<float>& samples);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QVector<float> m_spectrumData;
    void calculateSpectrum(const QVector<float>& samples);
};

#endif // SPECTRUM_H
