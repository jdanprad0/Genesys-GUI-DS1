#include "TriggerAnimation.h"

TriggerAnimation::TriggerAnimation(unsigned int timeExecution, QObject *parent) : QObject(parent) {
    connect(&_timer, &QTimer::timeout, this, &TriggerAnimation::run);

    // Inicializa a lista de animações como vazia
    _animations = new QList<AnimationTransition*>();

    // Define tempo de duração de uma animação
    _timeExecution = timeExecution;
}

TriggerAnimation::~TriggerAnimation() {
    delete _animations;
}

// Getters
QList<AnimationTransition*>* TriggerAnimation::getAnimations() const {
    return _animations;
}

double *TriggerAnimation::getTimeExecution() const {
    return const_cast<double*>(&_timeExecution);
}

// Setters
void TriggerAnimation::setTimeExecution(unsigned int timeExecution) {
    _timeExecution = timeExecution;
}

void TriggerAnimation::clockInit() {
    _clock.restart();
    updateTimer();
}

qint64 TriggerAnimation::getTime() const {
    return _clock.elapsed();
}

void TriggerAnimation::run() {
    if (!_animations->isEmpty()) {
        _animations->first()->startAnimation();
        _animations->removeFirst();
        updateTimer();
    }
}

void TriggerAnimation::updateTimer() {
    if (!_animations->isEmpty()) {
        qint64 currentTimeMs = this->getTime();
        qint64 nextAnimationTimeStartMs = static_cast<qint64>(_animations->first()->getTimeStart());

        int timeUntilNextAnimationMs = static_cast<int>(nextAnimationTimeStartMs - currentTimeMs);
        if (timeUntilNextAnimationMs <= 0) {
            // Se o tempo atual já passou além do tempo de início da próxima animação, execute imediatamente
            run();
        } else {
            _timer.start(timeUntilNextAnimationMs);
        }
    }
}
