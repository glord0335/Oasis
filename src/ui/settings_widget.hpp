#ifndef SETTINGS_WIDGET_HPP
#define SETTINGS_WIDGET_HPP

#include "../core/settings_manager.hpp"
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QWidget>

#include <QListWidget>
#include <QPushButton>
#include <QTimeEdit>

class SettingsWidget : public QWidget {
  Q_OBJECT
public:
  explicit SettingsWidget(SettingsManager *settings, QWidget *parent = nullptr);

signals:
  void settingsChanged();

private slots:
  void saveSettings();
  void addMoment();
  void removeMoment();
  void onItemChanged(QListWidgetItem *item);

private:
  SettingsManager *m_settings;
  QComboBox *m_modeCombo;
  QSpinBox *m_intervalSpin;
  QListWidget *m_momentsList;
  QCheckBox *m_dndEnabledCheck;
  QTimeEdit *m_dndStartEdit;
  QTimeEdit *m_dndEndEdit;
  QSpinBox *m_goalSpin;
  QSpinBox *m_drinkAmountSpin;
  QComboBox *m_styleCombo;
  QCheckBox *m_autoStartCheck;
};

#endif // SETTINGS_WIDGET_HPP
