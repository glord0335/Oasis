#include <QAction>
#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QIcon>
#include <QMenu>
#include <QMessageBox>
#include <QProcess>
#include <QSystemTrayIcon>
#include <QTimer>

#ifndef Q_OS_WIN
#include <QDesktopWidget>
#endif

#include "core/plant_system.hpp"
#include "core/reminder_engine.hpp"
#include "core/settings_manager.hpp"
#include "ui/popup_widget.hpp"
#include "ui/settings_widget.hpp"
#include "ui/stats_widget.hpp"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  app.setApplicationName("Oasis");
  app.setOrganizationName("Agil");

  // 注入莫兰迪风格全局样式
  app.setStyleSheet(R"(
    * {
        font-family: 'Segoe UI', 'Microsoft YaHei', sans-serif;
        color: #4A4A4A;
    }
    QWidget {
        background-color: transparent;
    }
    QMainWindow, QDialog, QWidget#centralWidget, QWidget#SettingsWidget {
        background-color: #B9C4C9;
    }
    QGroupBox {
        border: none;
        background-color: rgba(255, 255, 255, 0.7);
        border-radius: 16px;
        margin-top: 30px;
        padding-top: 15px;
        font-weight: bold;
    }
    QGroupBox::title {
        subcontrol-origin: margin;
        subcontrol-position: top center;
        padding: 0 10px;
        color: #5A6B58; /* 加深颜色以提高可读性 */
    }
    QPushButton {
        background-color: #A7B9A4;
        color: white;
        border: none;
        border-radius: 12px;
        padding: 8px 16px;
        font-weight: bold;
    }
    QPushButton:hover {
        background-color: #96A893;
    }
    QPushButton:pressed {
        background-color: #859782;
    }
    /* 弹窗及统计面板专用按钮样式 (适配绿背景) */
    QPushButton#confirm_popup, QPushButton#confirm_stats {
        background-color: #F5F5F5;
        color: #A7B9A4;
    }
    QPushButton#confirm_popup:hover, QPushButton#confirm_stats:hover {
        background-color: #FFFFFF;
    }
    QPushButton#delay_popup {
        background-color: rgba(255, 255, 255, 0.3);
        color: white;
    }
    QPushButton#delay_popup:hover {
        background-color: rgba(255, 255, 255, 0.4);
    }
    QPushButton#delay {
        background-color: #DCC7B1;
    }
    QPushButton#delay:hover {
        background-color: #CBB6A0;
    }
    QLineEdit, QSpinBox, QTimeEdit, QComboBox {
        background-color: #FFFFFF;
        border: 1px solid #E0E0E0;
        border-radius: 8px;
        padding: 4px 10px;
    }
    QComboBox QAbstractItemView {
        background-color: #FFFFFF;
        border: 1px solid #E0E0E0;
        selection-background-color: #F0F4EF;
        selection-color: #A7B9A4;
        outline: none;
    }
    QListWidget {
        background-color: #FFFFFF;
        border: 1px solid #E0E0E0;
        border-radius: 12px;
        padding: 5px;
    }
    QListWidget::item {
        padding: 8px;
        border-bottom: 1px solid #F0F0F0;
    }
    QListWidget::item:selected {
        background-color: #F0F4EF;
        color: #A7B9A4;
    }
    QCheckBox {
        spacing: 8px;
    }
    QCheckBox::indicator {
        width: 18px;
        height: 18px;
        border-radius: 4px;
        border: 2px solid #A7B9A4;
    }
    QCheckBox::indicator:checked {
        background-color: #A7B9A4;
    }
  )");

  // 加载样式表 (如果有)
  QFile styleFile(":/styles/main.qss");
  if (styleFile.open(QFile::ReadOnly)) {
    app.setStyleSheet(app.styleSheet() + styleFile.readAll());
  }

  QApplication::setQuitOnLastWindowClosed(false);

  // 初始化核心逻辑
  SettingsManager *settings = new SettingsManager(&app);
  ReminderEngine *engine = new ReminderEngine(&app);
  PlantSystem *plantSystem = new PlantSystem(&app);

  // 初始化 UI 组件
  PopupWidget *popup = new PopupWidget();
  StatsWidget *statsWidget = new StatsWidget(plantSystem, settings);
  SettingsWidget *settingsWidget = new SettingsWidget(settings);

  engine->setMode(
      static_cast<ReminderEngine::ReminderMode>(settings->reminderMode()));
  engine->setInterval(settings->reminderInterval());
  engine->setFixedMoments(settings->fixedMoments());
  engine->setDNDRange(settings->dndStart(), settings->dndEnd());
  engine->setDNDEnabled(settings->isDNDEnabled());
  engine->setDND(settings->isPaused());
  engine->start();

  // 基础系统托盘初始化
  QSystemTrayIcon *trayIcon = new QSystemTrayIcon(&app);
  trayIcon->setIcon(QIcon(":/icon.png"));
  trayIcon->setToolTip("Oasis (干一杯) - 智能补水助手");

  QMenu *trayMenu = new QMenu();
  QAction *testPopupAction =
      new QAction("测试弹窗", trayMenu); // Keep this action as it's used later
  QAction *quickDrinkAction = new QAction(
      QString("快捷补水 (+%1ml)").arg(settings->drinkAmount()), trayMenu);
  QAction *pauseAction =
      new QAction(settings->isPaused() ? "恢复提醒" : "暂停提醒", trayMenu);
  QAction *settingsAction = new QAction("个人设置", trayMenu);
  QAction *statsAction = new QAction("进度报告", trayMenu);
  QAction *exitAction = new QAction("完全退出", trayMenu);

  trayMenu->addAction(quickDrinkAction);
  trayMenu->addSeparator();
  trayMenu->addAction(testPopupAction);
  trayMenu->addAction(pauseAction);
  trayMenu->addSeparator();
  trayMenu->addAction(settingsAction);
  trayMenu->addAction(statsAction);
  QAction *restartAction = new QAction("重启 Oasis", trayMenu);
  QAction *quitAction = new QAction("退出 Oasis", trayMenu);

  trayMenu->addAction(restartAction);
  trayMenu->addAction(quitAction);

  QObject::connect(restartAction, &QAction::triggered, [=, &app]() {
    qDebug() << "Restarting application...";
    QProcess::startDetached(app.applicationFilePath(), app.arguments());
    app.quit();
  });
  QObject::connect(quitAction, &QAction::triggered, &app, &QApplication::quit);

  trayIcon->setContextMenu(trayMenu);
  trayIcon->show();

  // 信号槽连接
  auto updateTooltip = [=]() {
    int current = plantSystem->todayWaterIntake();
    int goal = settings->dailyGoal();
    trayIcon->setToolTip(QString("Oasis (干一杯) - 今日进度: %1/%2 ml (%3%)")
                             .arg(current)
                             .arg(goal)
                             .arg(current * 100 / (goal ? goal : 1)));
  };
  updateTooltip();

  QObject::connect(engine, &ReminderEngine::reminderTriggered, [=]() {
    popup->setDrinkAmount(settings->drinkAmount());
    popup->setReminderStyle(settings->reminderStyle());
    popup->showAnimated();
  });
  QObject::connect(popup, &PopupWidget::drinkConfirmed, [=](int ml) {
    plantSystem->recordDrink(ml);
    updateTooltip();
  });
  QObject::connect(quickDrinkAction, &QAction::triggered, [=]() {
    plantSystem->recordDrink(settings->drinkAmount());
    updateTooltip();
  });
  QObject::connect(pauseAction, &QAction::triggered, [=]() {
    bool newState = !settings->isPaused();
    settings->setPaused(newState);
    engine->setDND(newState);
    pauseAction->setText(newState ? "恢复提醒" : "暂停提醒");
  });
  QObject::connect(testPopupAction, &QAction::triggered, [=]() {
    popup->setDrinkAmount(settings->drinkAmount());
    popup->setReminderStyle(settings->reminderStyle());
    popup->showAnimated();
  });
  QObject::connect(statsAction, &QAction::triggered, statsWidget,
                   &StatsWidget::show);
  QObject::connect(settingsAction, &QAction::triggered, settingsWidget,
                   &SettingsWidget::show);
  QObject::connect(settingsWidget, &SettingsWidget::settingsChanged, [=]() {
    engine->setMode(
        static_cast<ReminderEngine::ReminderMode>(settings->reminderMode()));
    engine->setInterval(settings->reminderInterval());
    engine->setFixedMoments(settings->fixedMoments());
    engine->setDNDRange(settings->dndStart(), settings->dndEnd());
    engine->setDNDEnabled(settings->isDNDEnabled());
    quickDrinkAction->setText(
        QString("快捷补水 (+%1ml)").arg(settings->drinkAmount()));
    statsWidget->refresh();
    updateTooltip();
    qDebug() << "Settings applied to engine and stats";
  });
  QObject::connect(exitAction, &QAction::triggered, &app,
                   &QCoreApplication::quit);

  qDebug() << "Oasis started...";

  return app.exec();
}
