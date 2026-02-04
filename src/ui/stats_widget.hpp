#ifndef STATS_WIDGET_HPP
#define STATS_WIDGET_HPP

#include "../core/plant_system.hpp"
#include "../core/settings_manager.hpp"
#include "components/circular_progress.hpp"
#include <QLabel>
#include <QListWidget>
#include <QWidget>

class QPushButton;

class StatsWidget : public QWidget {
  Q_OBJECT
public:
  explicit StatsWidget(PlantSystem *plantSystem, SettingsManager *settings,
                       QWidget *parent = nullptr);

public slots:
  void refresh();

protected:
  void paintEvent(QPaintEvent *event) override;
  void focusOutEvent(QFocusEvent *event) override;
  void showEvent(QShowEvent *event) override;

private:
  PlantSystem *m_plantSystem;
  SettingsManager *m_settings;
  CircularProgressBar *m_progressBar;
  QLabel *m_percentLabel;
  QLabel *m_amountLabel;
  QLabel *m_statusLabel;
  QPushButton *m_harvestButton; // 收成按钮
  QLabel *m_harvestLabel;       // 收成勋章
  QLabel *m_growthLabel;
  QListWidget *m_recordList; // 饮水记录列表
};

#endif // STATS_WIDGET_HPP
