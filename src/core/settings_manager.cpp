#include "settings_manager.hpp"
#include <QDebug>

SettingsManager::SettingsManager(QObject *parent)
    : QObject(parent), m_settings("Agil", "Oasis") {
  qDebug() << "Settings loaded from:" << m_settings.fileName();
}

void SettingsManager::setReminderMode(ReminderMode mode) {
  m_settings.setValue("reminder_mode", static_cast<int>(mode));
}

SettingsManager::ReminderMode SettingsManager::reminderMode() const {
  return static_cast<ReminderMode>(
      m_settings.value("reminder_mode", IntervalMode).toInt());
}

void SettingsManager::setReminderStyle(ReminderStyle style) {
  m_settings.setValue("reminder_style", static_cast<int>(style));
  m_settings.sync();
}

SettingsManager::ReminderStyle SettingsManager::reminderStyle() const {
  return static_cast<ReminderStyle>(
      m_settings.value("reminder_style", StandardStyle).toInt());
}

void SettingsManager::setReminderInterval(int minutes) {
  m_settings.setValue("reminder_interval", minutes);
}

int SettingsManager::reminderInterval() const {
  return m_settings.value("reminder_interval", 45).toInt();
}

void SettingsManager::setFixedMoments(const QList<QTime> &moments) {
  QStringList list;
  for (const QTime &t : moments) {
    list << t.toString("HH:mm");
  }
  m_settings.setValue("fixed_moments", list);
}

QList<QTime> SettingsManager::fixedMoments() const {
  QStringList list = m_settings.value("fixed_moments").toStringList();
  QList<QTime> moments;
  for (const QString &s : list) {
    QTime t = QTime::fromString(s, "HH:mm");
    if (!t.isValid()) {
      t = QTime::fromString(s, "H:mm");
    }
    if (t.isValid()) {
      moments << t;
    }
  }
  if (moments.isEmpty()) {
    moments << QTime(10, 0) << QTime(14, 0) << QTime(16, 0);
  }
  return moments;
}

void SettingsManager::setDailyGoal(int ml) {
  m_settings.setValue("daily_goal", ml);
  m_settings.sync();
}

int SettingsManager::dailyGoal() const {
  return m_settings.value("daily_goal", 2000).toInt();
}

void SettingsManager::setDrinkAmount(int ml) {
  m_settings.setValue("drink_amount", ml);
  m_settings.sync();
}

int SettingsManager::drinkAmount() const {
  return m_settings.value("drink_amount", 250).toInt();
}

void SettingsManager::setDNDRange(const QTime &start, const QTime &end) {
  m_settings.setValue("dnd_start", start.toString("HH:mm"));
  m_settings.setValue("dnd_end", end.toString("HH:mm"));
}

QTime SettingsManager::dndStart() const {
  return QTime::fromString(m_settings.value("dnd_start", "23:00").toString(),
                           "HH:mm");
}

QTime SettingsManager::dndEnd() const {
  return QTime::fromString(m_settings.value("dnd_end", "08:00").toString(),
                           "HH:mm");
}

void SettingsManager::setDNDEnabled(bool enabled) {
  m_settings.setValue("dnd_enabled", enabled);
}

bool SettingsManager::isDNDEnabled() const {
  return m_settings.value("dnd_enabled", false).toBool(); // 默认关闭免打扰
}

void SettingsManager::setPaused(bool paused) {
  m_settings.setValue("is_paused", paused);
}

bool SettingsManager::isPaused() const {
  return m_settings.value("is_paused", false).toBool();
}

void SettingsManager::setAutoStart(bool enable) {
  m_settings.setValue("auto_start", enable);
  // TODO: 实现 Linux 下的自启动逻辑 (如在 ~/.config/autostart/ 创建 .desktop
  // 文件)
}

bool SettingsManager::autoStart() const {
  return m_settings.value("auto_start", false).toBool();
}
