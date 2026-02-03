#include "popup_widget.hpp"
#include "../core/warming_copy.hpp"
#include <QApplication>
#include <QDebug>
#include <QDesktopWidget>
#include <QEasingCurve>
#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>
#include <QPainterPath>
#include <QVBoxLayout>

PopupWidget::PopupWidget(QWidget *parent)
    : QWidget(parent), m_opacity(0.0), m_drinkAmount(250), m_reminderStyle(0) {

  // 设置基础窗口属性
  setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
  setAttribute(Qt::WA_TranslucentBackground);
  setAttribute(Qt::WA_ShowWithoutActivating);

  setFixedSize(320, 180);

  m_autoHideTimer = new QTimer(this);
  m_autoHideTimer->setSingleShot(true);
  m_autoHideTimer->setInterval(30000); // 30 秒
  connect(m_autoHideTimer, &QTimer::timeout, this, &PopupWidget::hideAnimated);

  setupUi();
  setupAnimations();
}

PopupWidget::~PopupWidget() {}

void PopupWidget::setupUi() {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(24, 24, 24, 24);
  mainLayout->setSpacing(12);

  m_titleLabel = new QLabel("[干一杯]~(￣▽￣)~*", this);
  m_titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: "
                              "#FFFFFF; letter-spacing: 1px;");

  m_contentLabel = new QLabel("喝一小杯水，让心情也跟着透亮起来。", this);
  m_contentLabel->setWordWrap(true);
  m_contentLabel->setStyleSheet(
      "font-size: 14px; color: #F5F5F5; line-height: 20px;");

  QHBoxLayout *btnLayout = new QHBoxLayout();
  m_confirmBtn = new QPushButton("好哒", this);
  m_delayBtn = new QPushButton("等会儿", this);

  m_confirmBtn->setCursor(Qt::PointingHandCursor);
  m_delayBtn->setCursor(Qt::PointingHandCursor);

  m_confirmBtn->setObjectName("confirm_popup");
  m_delayBtn->setObjectName("delay_popup");

  btnLayout->addStretch();
  btnLayout->addWidget(m_delayBtn);
  btnLayout->addWidget(m_confirmBtn);

  mainLayout->addWidget(m_titleLabel);
  mainLayout->addWidget(m_contentLabel);
  mainLayout->addStretch();
  mainLayout->addLayout(btnLayout);

  connect(m_confirmBtn, &QPushButton::clicked, this, [this]() {
    emit drinkConfirmed(m_drinkAmount);
    hideAnimated();
  });
  connect(m_delayBtn, &QPushButton::clicked, this, &PopupWidget::hideAnimated);

  // 阴影已改为在 paintEvent 中手动绘制,以完美贴合圆角
}

void PopupWidget::setupAnimations() {
  m_fadeAnimation = new QPropertyAnimation(this, "opacity");
  m_fadeAnimation->setDuration(200); // 缩短动画时间以加快显示
  m_fadeAnimation->setEasingCurve(QEasingCurve::OutBack); // 果冻回弹效果
  connect(m_fadeAnimation, &QPropertyAnimation::finished, this, [this]() {
    if (m_opacity == 0.0) {
      hide();
    }
  });
}

void PopupWidget::setOpacity(qreal opacity) {
  m_opacity = opacity;
  setWindowOpacity(opacity);
}

qreal PopupWidget::opacity() const { return m_opacity; }

void PopupWidget::setDrinkAmount(int ml) {
  m_drinkAmount = ml;
  if (m_confirmBtn) {
    m_confirmBtn->setText(m_reminderStyle == 1 ? "执行命令" : "好哒");
  }
}

void PopupWidget::setReminderStyle(int style) {
  m_reminderStyle = style;
  if (style == 1) {
    if (m_titleLabel)
      m_titleLabel->setText("独立团团部公告箱");
    if (m_delayBtn)
      m_delayBtn->setText("待会儿再说");
  } else if (style == 2) {
    if (m_titleLabel)
      m_titleLabel->setText("【摸鱼办】紧急通知");
    if (m_delayBtn)
      m_delayBtn->setText("再卷一会儿");
  } else if (style == 3) {
    if (m_titleLabel)
      m_titleLabel->setText("【陈塘关第一混世魔王】");
    if (m_delayBtn)
      m_delayBtn->setText("爷就不喝");
  } else {
    if (m_titleLabel)
      m_titleLabel->setText("[干一杯]~(￣▽￣)~*");
    if (m_delayBtn)
      m_delayBtn->setText("等会儿");
  }
}

void PopupWidget::showAnimated() {
  m_contentLabel->setText(WarmingCopy::getRandomCopy(m_reminderStyle));

  if (m_reminderStyle == 1) {
    m_confirmBtn->setText("执行命令");
  } else if (m_reminderStyle == 2) {
    m_confirmBtn->setText("带薪喝水");
  } else if (m_reminderStyle == 3) {
    m_confirmBtn->setText("去他爷的，喝！");
  } else {
    m_confirmBtn->setText("好哒");
  }
  // 定位到屏幕中央以达到强制提醒的目的
  QRect desktop = QApplication::desktop()->availableGeometry();
  int x = (desktop.width() - width()) / 2;
  int y = (desktop.height() - height()) / 2;
  move(x, y);

  m_fadeAnimation->stop();
  show();
  m_fadeAnimation->setStartValue(m_opacity);
  m_fadeAnimation->setEndValue(1.0);
  m_fadeAnimation->start();
  m_autoHideTimer->start();
}

void PopupWidget::hideAnimated() {
  m_autoHideTimer->stop();
  m_fadeAnimation->stop();
  m_fadeAnimation->setStartValue(m_opacity);
  m_fadeAnimation->setEndValue(0.0);
  m_fadeAnimation->start();
}

void PopupWidget::paintEvent(QPaintEvent *event) {
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
  gradient.setColorAt(0, QColor(40, 60, 40, 80));   // 中心较深
  gradient.setColorAt(0.7, QColor(40, 60, 40, 40)); // 中间过渡
  gradient.setColorAt(1, QColor(40, 60, 40, 0));    // 边缘透明

  painter.setBrush(gradient);
  painter.setPen(Qt::NoPen);
  painter.drawPath(shadowOnlyPath);

  // 绘制主体圆角矩形
  painter.setBrush(QColor(167, 185, 164, 250));       // #A7B9A4 莫兰迪豆沙绿
  painter.setPen(QPen(QColor(255, 255, 255, 60), 1)); // 极淡白边框
  painter.drawRoundedRect(contentRect, radius, radius);
}

void PopupWidget::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    m_dragPosition = event->globalPos() - frameGeometry().topLeft();
    event->accept();
  }
}

void PopupWidget::mouseMoveEvent(QMouseEvent *event) {
  if (event->buttons() & Qt::LeftButton) {
    move(event->globalPos() - m_dragPosition);
    event->accept();
  }
}
