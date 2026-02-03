#ifndef POPUP_WIDGET_HPP
#define POPUP_WIDGET_HPP

#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

class PopupWidget : public QWidget {
  Q_OBJECT
  Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)

public:
  explicit PopupWidget(QWidget *parent = nullptr);
  ~PopupWidget();

  void showAnimated();
  void hideAnimated();
  void setDrinkAmount(int ml);
  void setReminderStyle(int style);

  void setOpacity(qreal opacity);
  qreal opacity() const;

signals:
  void drinkConfirmed(int ml);

protected:
  void paintEvent(QPaintEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;

private:
  void setupUi();
  void setupAnimations();

  QPoint m_dragPosition;
  qreal m_opacity;
  QPropertyAnimation *m_fadeAnimation;
  QTimer *m_autoHideTimer;
  int m_drinkAmount;
  int m_reminderStyle;

  // UI Elements
  QLabel *m_titleLabel;
  QLabel *m_contentLabel;
  QPushButton *m_confirmBtn;
  QPushButton *m_delayBtn;
};

#endif // POPUP_WIDGET_HPP
