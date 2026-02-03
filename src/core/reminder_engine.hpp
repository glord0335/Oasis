#ifndef REMINDER_ENGINE_HPP
#define REMINDER_ENGINE_HPP

#include <QDateTime>
#include <QList>
#include <QObject>
#include <QTime>
#include <QTimer>

class ReminderEngine : public QObject {
  Q_OBJECT
public:
  enum ReminderMode { IntervalMode, FixedMomentMode };

  explicit ReminderEngine(QObject *parent = nullptr);

  void setMode(ReminderMode mode);
  void setInterval(int minutes);
  void setFixedMoments(const QList<QTime> &moments);

  void start();
  void stop();

  bool isDNDActive() const; // Do Not Disturb
  void setDND(bool active);
  void setDNDRange(const QTime &start, const QTime &end);
  void setDNDEnabled(bool enabled);

signals:
  void reminderTriggered();

private slots:
  void onTimerTimeout();
  void checkFixedMoments();

private:
  ReminderMode m_mode;
  int m_intervalMinutes;
  QList<QTime> m_fixedMoments;

  QTimer *m_timer;
  QTimer *m_momentChecker;

  bool m_isDND;
  bool m_dndEnabled;
  QTime m_dndStart;
  QTime m_dndEnd;

  QDateTime m_lastTriggerTime;
};

#endif // REMINDER_ENGINE_HPP
