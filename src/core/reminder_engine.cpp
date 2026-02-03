#include "reminder_engine.hpp"
#include <QDateTime>
#include <QDebug>

ReminderEngine::ReminderEngine(QObject *parent)
    : QObject(parent), m_mode(IntervalMode), m_intervalMinutes(60),
      m_isDND(false), m_dndEnabled(true), m_dndStart(23, 0), m_dndEnd(8, 0) {

  m_timer = new QTimer(this);
  connect(m_timer, &QTimer::timeout, this, &ReminderEngine::onTimerTimeout);

  m_momentChecker = new QTimer(this);
  m_momentChecker->setInterval(1000); // 每秒检查一次，实现精准提醒
  connect(m_momentChecker, &QTimer::timeout, this,
          &ReminderEngine::checkFixedMoments);
}

void ReminderEngine::setMode(ReminderMode mode) {
  m_mode = mode;
  if (m_timer->isActive() || m_momentChecker->isActive()) {
    stop();
    start();
  }
}

void ReminderEngine::setInterval(int minutes) {
  m_intervalMinutes = minutes;
  if (m_mode == IntervalMode && m_timer->isActive()) {
    m_timer->start(m_intervalMinutes * 60000);
  }
}

void ReminderEngine::setFixedMoments(const QList<QTime> &moments) {
  m_fixedMoments = moments;
}

void ReminderEngine::start() {
  if (m_mode == IntervalMode) {
    m_timer->start(m_intervalMinutes * 60000);
  } else {
    m_momentChecker->start();
  }
  qDebug() << "Reminder Engine started in"
           << (m_mode == IntervalMode ? "Interval" : "Fixed") << "mode";
}

void ReminderEngine::stop() {
  m_timer->stop();
  m_momentChecker->stop();
  qDebug() << "Reminder Engine stopped";
}

void ReminderEngine::setDND(bool active) { m_isDND = active; }

void ReminderEngine::setDNDRange(const QTime &start, const QTime &end) {
  m_dndStart = start;
  m_dndEnd = end;
}

void ReminderEngine::setDNDEnabled(bool enabled) { m_dndEnabled = enabled; }

bool ReminderEngine::isDNDActive() const {
  if (m_isDND)
    return true;

  if (!m_dndEnabled)
    return false;

  QTime now = QTime::currentTime();
  if (m_dndStart <= m_dndEnd) {
    // 同一天内，如 23:00 - 23:59 (虽然通常跨天)
    return now >= m_dndStart && now <= m_dndEnd;
  } else {
    // 跨天情况，如 23:00 - 08:00
    return now >= m_dndStart || now <= m_dndEnd;
  }
}

void ReminderEngine::onTimerTimeout() {
  if (!isDNDActive()) {
    m_lastTriggerTime = QDateTime::currentDateTime();
    emit reminderTriggered();
  } else {
    qDebug() << "Reminder skipped due to DND";
  }
}

void ReminderEngine::checkFixedMoments() {
  if (isDNDActive())
    return;

  QDateTime now = QDateTime::currentDateTime();
  // 如果 60 秒内已经触发过，跳过
  if (m_lastTriggerTime.isValid() && m_lastTriggerTime.secsTo(now) < 60)
    return;

  QTime currentTime = now.time();
  for (const QTime &moment : m_fixedMoments) {
    if (currentTime.hour() == moment.hour() &&
        currentTime.minute() == moment.minute()) {
      m_lastTriggerTime = now;
      emit reminderTriggered();
      break;
    }
  }
}
