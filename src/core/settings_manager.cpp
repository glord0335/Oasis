#include "settings_manager.hpp"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QTextStream>
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

  // 实现 Linux 下的自启动逻辑 (在 ~/.config/autostart/ 创建 .desktop 文件)
  QString autostartPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/autostart";
  QDir dir(autostartPath);
  if (!dir.exists()) {
    dir.mkpath(".");
  }

  QString desktopFilePath = autostartPath + "/oasis.desktop";
  if (enable) {
    QFile file(desktopFilePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
      QTextStream out(&file);
      out << "[Desktop Entry]\n";
      out << "Type=Application\n";
      out << "Name=Oasis\n";
      out << "Comment=智能补水助手\n";
      out << "Exec=" << QCoreApplication::applicationFilePath() << "\n";
      out << "Icon=" << QCoreApplication::applicationFilePath() << "\n"; // 暂时使用程序路径作为图标
      out << "Terminal=false\n";
      out << "Categories=Utility;\n";
      out << "X-GNOME-Autostart-enabled=true\n";
      file.close();
      qDebug() << "Autostart enabled: created" << desktopFilePath;
    } else {
      qWarning() << "Failed to create autostart file:" << desktopFilePath;
    }
  } else {
    if (QFile::exists(desktopFilePath)) {
      if (QFile::remove(desktopFilePath)) {
        qDebug() << "Autostart disabled: removed" << desktopFilePath;
      } else {
        qWarning() << "Failed to remove autostart file:" << desktopFilePath;
      }
    }
  }
}

bool SettingsManager::autoStart() const {
  return m_settings.value("auto_start", false).toBool();
}
