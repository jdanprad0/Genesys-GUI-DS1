#ifndef TRIGGERANIMATION_H
#define TRIGGERANIMATION_H

#include <QObject>
#include <QTimer>
#include <QElapsedTimer>
#include <QList>

#include "AnimationTransition.h"

class TriggerAnimation : public QObject {
    Q_OBJECT

public:
    // Construtor
    TriggerAnimation(unsigned int _timeExecution, QObject *parent = nullptr);
    // Destrutor
    ~TriggerAnimation();

    // Getters
    QList<AnimationTransition*>* getAnimations() const;
    bool getTimerStopped() const;
    double *getTimeExecution() const;

    // Setters
    void setTimeExecution(unsigned int timeExecution);

    // Inicia o relógio
    void clockInit();
    // Pega o momento atual do relógio
    qint64 getTime() const;
    // Inicia a animação daquele tempo exato
    void run();

public slots:
    // Atualiza o timer
    void updateTimer();

private:
    QElapsedTimer _clock;
    QTimer _timer;
    QList<AnimationTransition*> *_animations;
    bool _timerStopped;
    double _timeExecution;
};

#endif // TRIGGERANIMATION_H
