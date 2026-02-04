#ifndef CIRCULAR_PROGRESS_HPP
#define CIRCULAR_PROGRESS_HPP

#include <QPropertyAnimation>
#include <QWidget>

class CircularProgressBar : public QWidget {
  Q_OBJECT
  Q_PROPERTY(int value READ value WRITE setValue)

public:
  CircularProgressBar(QWidget *parent = nullptr);

  void setValue(int value);
  int value() const { return m_value; }

  void setRange(int min, int max);
  void setIconText(const QString &icon);

protected:
  void paintEvent(QPaintEvent *event) override;

private:
  int m_value;
  int m_min;
  int m_max;
  QString m_iconText;
};

#endif // CIRCULAR_PROGRESS_HPP
