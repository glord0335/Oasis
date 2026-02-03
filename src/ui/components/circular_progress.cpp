#include "circular_progress.hpp"
#include <QColor>
#include <QPainter>
#include <QPen>

CircularProgressBar::CircularProgressBar(QWidget *parent)
    : QWidget(parent), m_value(0), m_min(0), m_max(100) {
  setFixedSize(120, 120);
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

void CircularProgressBar::setText(const QString &text) {
  m_text = text;
  update();
}

void CircularProgressBar::paintEvent(QPaintEvent *event) {
  Q_UNUSED(event);
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  int side = qMin(width(), height());
  int thickness = 8;
  QRectF rect(thickness / 2.0, thickness / 2.0, side - thickness,
              side - thickness);

  // 背景圆环
  QPen bgPen(QColor(255, 255, 255, 100)); // 略微增加不透明度
  bgPen.setWidth(thickness);
  bgPen.setCapStyle(Qt::RoundCap);
  painter.setPen(bgPen);
  painter.drawArc(rect, 0, 360 * 16);

  // 进度圆环 (改用深橄榄绿以增强在鼠尾草绿背景下的对比度)
  QPen progressPen;
  progressPen.setWidth(thickness);
  progressPen.setCapStyle(Qt::RoundCap);
  progressPen.setColor(QColor("#5A6B58"));
  painter.setPen(progressPen);

  double spanAngle = -((double)(m_value - m_min) / (m_max - m_min)) * 360 * 16;
  painter.drawArc(rect, 90 * 16, spanAngle);

  // 文字 (改用白色以适配彩色背景)
  painter.setPen(QColor("#FFFFFF"));
  QFont font = painter.font();
  font.setPointSize(11);
  font.setBold(true);
  painter.setFont(font);

  int percent =
      (m_max - m_min) > 0 ? (m_value - m_min) * 100 / (m_max - m_min) : 0;
  QString percentage = QString::number(percent) + "%";
  painter.drawText(rect.adjusted(0, -5, 0, -5), Qt::AlignCenter, percentage);

  if (!m_text.isEmpty()) {
    font.setPointSize(8);
    font.setBold(false);
    painter.setFont(font);
    painter.drawText(rect.adjusted(0, 15, 0, 15), Qt::AlignCenter, m_text);
  }
}
