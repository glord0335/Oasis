#ifndef SETTINGS_MANAGER_HPP
#define SETTINGS_MANAGER_HPP

#include <QList>
#include <QObject>
#include <QSettings>
#include <QTime>

class SettingsManager : public QObject {
  Q_OBJECT
public:
  enum ReminderMode { IntervalMode = 0, FixedMomentMode = 1 };
  enum ReminderStyle {
    StandardStyle = 0,
    LiYunlongStyle = 1,
    MoyuStyle = 2,
    NezhaStyle = 3
  };

  explicit SettingsManager(QObject *parent = nullptr);

  void setReminderMode(ReminderMode mode);
  ReminderMode reminderMode() const;

  void setReminderStyle(ReminderStyle style);
  ReminderStyle reminderStyle() const;

  void setReminderInterval(int minutes);
  int reminderInterval() const;

  void setFixedMoments(const QList<QTime> &moments);
  QList<QTime> fixedMoments() const;

  void setDailyGoal(int ml);
  int dailyGoal() const;

  void setDrinkAmount(int ml);
  int drinkAmount() const;

  void setDNDRange(const QTime &start, const QTime &end);
  QTime dndStart() const;
  QTime dndEnd() const;
  void setDNDEnabled(bool enabled);
  bool isDNDEnabled() const;

  void setPaused(bool paused);
  bool isPaused() const;

  void setAutoStart(bool enable);
  bool autoStart() const;

private:
  QSettings m_settings;
};

#endif // SETTINGS_MANAGER_HPP
