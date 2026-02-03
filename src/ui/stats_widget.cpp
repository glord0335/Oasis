#include "stats_widget.hpp"
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QPainterPath>
#include <QVBoxLayout>

StatsWidget::StatsWidget(PlantSystem *plantSystem, SettingsManager *settings,
                         QWidget *parent)
    : QWidget(parent), m_plantSystem(plantSystem), m_settings(settings) {
  setWindowFlags(Qt::Popup | Qt::FramelessWindowHint |
                 Qt::NoDropShadowWindowHint);
  setAttribute(Qt::WA_TranslucentBackground);
  setFixedSize(280, 480); // 增加尺寸以容纳饮水记录列表

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setContentsMargins(24, 24, 24, 24);
  layout->setSpacing(15);

  QLabel *title = new QLabel("今日统计", this);
  title->setStyleSheet("font-size: 16px; font-weight: bold; color: #FFFFFF;");
  title->setAlignment(Qt::AlignCenter);

  m_progressBar = new CircularProgressBar(this);
  m_progressBar->setRange(0, m_settings->dailyGoal());

  m_growthLabel = new QLabel(this);
  m_growthLabel->setStyleSheet("font-size: 12px; color: #F5F5F5;");
  m_growthLabel->setAlignment(Qt::AlignCenter);

  m_statusLabel = new QLabel(this);
  m_statusLabel->setStyleSheet(
      "font-size: 12px; color: #FFFFFF; font-weight: bold;");
  m_statusLabel->setAlignment(Qt::AlignCenter);

  // 饮水记录列表
  QLabel *recordTitle = new QLabel("今日饮水记录", this);
  recordTitle->setStyleSheet(
      "font-size: 13px; font-weight: bold; color: #FFFFFF;");
  recordTitle->setAlignment(Qt::AlignCenter);

  m_recordList = new QListWidget(this);
  m_recordList->setStyleSheet(
      "QListWidget { "
      "  background-color: rgba(255, 255, 255, 0.9); "
      "  border: none; "
      "  border-radius: 8px; "
      "  padding: 4px; "
      "  font-size: 11px; "
      "  color: #5A6B58; "
      "} "
      "QListWidget::item { "
      "  padding: 4px; "
      "  border-bottom: 1px solid rgba(167, 185, 164, 0.2); "
      "}");
  m_recordList->setMaximumHeight(120);

  layout->addWidget(title);
  layout->addWidget(m_progressBar, 0, Qt::AlignCenter);
  layout->addWidget(m_growthLabel);
  layout->addWidget(m_statusLabel);
  layout->addSpacing(10);
  layout->addWidget(recordTitle);
  layout->addWidget(m_recordList);
  layout->addStretch();

  refresh();

  connect(m_plantSystem, &PlantSystem::plantUpdated, this,
          &StatsWidget::refresh);

  // 阴影已改为在 paintEvent 中手动绘制,以完美贴合圆角
}

void StatsWidget::refresh() {
  int intake = m_plantSystem->todayWaterIntake();
  int goal = m_settings->dailyGoal();
  m_progressBar->setRange(0, goal);
  m_progressBar->setValue(intake);
  m_progressBar->setText(QString::number(intake) + " / " +
                         QString::number(goal) + " ml");

  m_growthLabel->setText("盆栽成长值: " +
                         QString::number(m_plantSystem->growthValue()));

  // 更新饮水记录列表
  m_recordList->clear();
  QList<PlantSystem::DrinkRecord> records = m_plantSystem->todayDrinkRecords();
  if (records.isEmpty()) {
    m_recordList->addItem("暂无记录");
  } else {
    // 倒序显示（最新的在上面）
    for (int i = records.size() - 1; i >= 0; --i) {
      const auto &record = records[i];
      QString timeStr = record.timestamp.toString("hh:mm");
      QString text = QString("%1  %2 ml").arg(timeStr).arg(record.amount);
      m_recordList->addItem(text);
    }
  }

  QString statusText;
  switch (m_plantSystem->status()) {
  case PlantSystem::Seedling:
    statusText = "状态: 萌芽期 🌱";
    break;
  case PlantSystem::Small:
    statusText = "状态: 小苗期 🌿";
    break;
  case PlantSystem::Medium:
    statusText = "状态: 成长期 🌳";
    break;
  case PlantSystem::Large:
    statusText = "状态: 繁茂期 🌲";
    break;
  case PlantSystem::Flowering:
    statusText = "状态: 开花期 🌸";
    break;
  case PlantSystem::Wilting:
    statusText = "状态: 缺水枯萎 🍂";
    break;
  }
  m_statusLabel->setText(statusText);
}

void StatsWidget::paintEvent(QPaintEvent *event) {
  Q_UNUSED(event);
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  // 手动绘制圆润的阴影边界 (使用渐变填充营造立体感)
  const int radius = 8;
  const int shadowSize = 12; // 增加阴影范围,使立体感更明显

  // 主体内容区域 (向内缩进,为阴影留出空间)
  QRectF contentRect =
      rect().adjusted(shadowSize, shadowSize, -shadowSize, -shadowSize);

  // 绘制柔和的径向渐变阴影
  QPainterPath shadowPath;
  shadowPath.addRoundedRect(rect(), radius + shadowSize / 2,
                            radius + shadowSize / 2);

  QPainterPath contentPath;
  contentPath.addRoundedRect(contentRect, radius, radius);

  // 阴影区域 = 外部路径 - 内容路径
  QPainterPath shadowOnlyPath = shadowPath.subtracted(contentPath);

  // 创建径向渐变 (从内容边缘向外渐变)
  QRadialGradient gradient(contentRect.center(), shadowSize * 1.5);
  gradient.setColorAt(0, QColor(80, 60, 40, 80));   // 中心较深
  gradient.setColorAt(0.7, QColor(80, 60, 40, 40)); // 中间过渡
  gradient.setColorAt(1, QColor(80, 60, 40, 0));    // 边缘透明

  painter.setBrush(gradient);
  painter.setPen(Qt::NoPen);
  painter.drawPath(shadowOnlyPath);

  // 绘制主体圆角矩形
  painter.setBrush(QColor(179, 193, 161, 250));       // #B3C1A1 莫兰迪鼠尾草绿
  painter.setPen(QPen(QColor(255, 255, 255, 60), 1)); // 极淡白边框
  painter.drawRoundedRect(contentRect, radius, radius);
}

void StatsWidget::focusOutEvent(QFocusEvent *event) {
  Q_UNUSED(event);
  hide();
}

void StatsWidget::showEvent(QShowEvent *event) {
  Q_UNUSED(event);
  activateWindow();
  raise();
  setFocus();
}
