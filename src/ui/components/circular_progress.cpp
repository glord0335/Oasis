#include "circular_progress.hpp"
#include <QColor>
#include <QPainter>
#include <QPen>

CircularProgressBar::CircularProgressBar(QWidget *parent)
    : QWidget(parent), m_value(0), m_min(0), m_max(100) {
  setFixedSize(140, 140);
}

void CircularProgressBar::setValue(int value) {
  if (m_value == value)
    return;
  m_value = qBound(m_min, value, m_max);
  update();
}

void CircularProgressBar::setRange(int min, int max) {
  m_min = min;
  m_max = max;
  update();
}

// 已移除 setText

void CircularProgressBar::setIconText(const QString &icon) {
  m_iconText = icon;
  update();
}

void CircularProgressBar::paintEvent(QPaintEvent *event) {
  Q_UNUSED(event);
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  int w = width();
  int h = height();
  int side = qMin(w, h);
  int thickness = 8;

  // 圆环居中展示
  int circleSize = side - thickness;
  QRectF circleRect(thickness / 2.0, thickness / 2.0, circleSize, circleSize);

  // 背景圆环
  QPen bgPen(QColor(255, 255, 255, 100));
  bgPen.setWidth(thickness);
  bgPen.setCapStyle(Qt::RoundCap);
  painter.setPen(bgPen);
  painter.drawArc(circleRect, 0, 360 * 16);

  // 进度圆环
  QPen progressPen;
  progressPen.setWidth(thickness);
  progressPen.setCapStyle(Qt::RoundCap);
  progressPen.setColor(QColor("#5A6B58"));
  painter.setPen(progressPen);

  double spanAngle = -((double)(m_value - m_min) / (m_max - m_min)) * 360 * 16;
  painter.drawArc(circleRect, 90 * 16, spanAngle);

  // 绘制圆心图标 (48px 大图标)
  if (!m_iconText.isEmpty()) {
    QFont iconFont = painter.font();
    iconFont.setPointSize(48);
    painter.setFont(iconFont);
    painter.setPen(Qt::white);
    painter.drawText(circleRect, Qt::AlignCenter, m_iconText);
  }
}
