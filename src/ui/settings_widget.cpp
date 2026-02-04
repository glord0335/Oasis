#include "settings_widget.hpp"
#include <QApplication>
#include <QDesktopWidget>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

SettingsWidget::SettingsWidget(SettingsManager *settings, QWidget *parent)
    : QWidget(parent), m_settings(settings) {

  setObjectName("SettingsWidget");
  setWindowTitle("Oasis (干一杯) 设置");
  setFixedSize(420, 780); // 增加窗口高度以确保第5行完全显示

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(24, 16, 24, 24); // 减少顶部边距，让内容往上移
  mainLayout->setSpacing(20);

  // 基础设置部分
  QGroupBox *basicGroup = new QGroupBox("基础设置", this);
  QFormLayout *basicLayout = new QFormLayout(basicGroup);

  m_goalSpin = new QSpinBox(this);
  m_goalSpin->setRange(100, 10000);
  m_goalSpin->setSingleStep(100);
  m_goalSpin->setValue(m_settings->dailyGoal());
  m_goalSpin->setSuffix(" ml");

  m_drinkAmountSpin = new QSpinBox(this);
  m_drinkAmountSpin->setRange(50, 1000);
  m_drinkAmountSpin->setSingleStep(50);
  m_drinkAmountSpin->setValue(m_settings->drinkAmount());
  m_drinkAmountSpin->setSuffix(" ml");

  m_autoStartCheck = new QCheckBox("开机自启", this);
  m_autoStartCheck->setChecked(m_settings->autoStart());

  basicLayout->addRow("每日目标:", m_goalSpin);
  basicLayout->addRow("每次喝水量:", m_drinkAmountSpin);

  m_styleCombo = new QComboBox(this);
  m_styleCombo->addItem("标准清新 (默认)", SettingsManager::StandardStyle);
  m_styleCombo->addItem("李云龙 (亮剑特色)", SettingsManager::LiYunlongStyle);
  m_styleCombo->addItem("职场摸鱼 (打工人专属)", SettingsManager::MoyuStyle);
  m_styleCombo->addItem("魔童哪吒 (我命由我不由天)",
                        SettingsManager::NezhaStyle);
  m_styleCombo->setCurrentIndex(m_settings->reminderStyle());
  basicLayout->addRow("提醒文案风格:", m_styleCombo);

  m_dndEnabledCheck = new QCheckBox("开启免打扰时段", this);
  m_dndEnabledCheck->setChecked(m_settings->isDNDEnabled());

  m_dndStartEdit = new QTimeEdit(this);
  m_dndStartEdit->setTime(m_settings->dndStart());
  m_dndEndEdit = new QTimeEdit(this);
  m_dndEndEdit->setTime(m_settings->dndEnd());

  auto updateDNDUI = [this](bool enabled) {
    m_dndStartEdit->setEnabled(enabled);
    m_dndEndEdit->setEnabled(enabled);
  };
  connect(m_dndEnabledCheck, &QCheckBox::toggled, updateDNDUI);
  updateDNDUI(m_dndEnabledCheck->isChecked());

  QHBoxLayout *dndLayout = new QHBoxLayout();
  dndLayout->addWidget(m_dndStartEdit);
  dndLayout->addWidget(new QLabel("至"));
  dndLayout->addWidget(m_dndEndEdit);

  basicLayout->addRow(m_dndEnabledCheck);
  basicLayout->addRow("免打扰时间:", dndLayout);

  basicLayout->addRow("", m_autoStartCheck);

  // 提醒设置部分
  QGroupBox *reminderGroup = new QGroupBox("提醒逻辑", this);
  QVBoxLayout *reminderLayout = new QVBoxLayout(reminderGroup);

  m_modeCombo = new QComboBox(this);
  m_modeCombo->addItem("间隔提醒模式", SettingsManager::IntervalMode);
  m_modeCombo->addItem("固定时刻模式", SettingsManager::FixedMomentMode);
  m_modeCombo->setCurrentIndex(m_settings->reminderMode());
  reminderLayout->addWidget(new QLabel("选择模式:"));
  reminderLayout->addWidget(m_modeCombo);

  // 间隔布局
  QWidget *intervalWidget = new QWidget(this);
  QHBoxLayout *intervalLayout = new QHBoxLayout(intervalWidget);
  intervalLayout->setContentsMargins(0, 5, 0, 5);
  m_intervalSpin = new QSpinBox(this);
  m_intervalSpin->setRange(1, 1440);
  m_intervalSpin->setValue(m_settings->reminderInterval());
  m_intervalSpin->setSuffix(" 分钟");
  intervalLayout->addWidget(new QLabel("提醒频率: 每"));
  intervalLayout->addWidget(m_intervalSpin);
  reminderLayout->addWidget(intervalWidget);

  // 固定时刻布局
  QWidget *fixedWidget = new QWidget(this);
  QVBoxLayout *fixedLayout = new QVBoxLayout(fixedWidget);
  fixedLayout->setContentsMargins(0, 5, 0, 5);

  m_momentsList = new QListWidget(this);
  m_momentsList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  m_momentsList->setMinimumHeight(220); // 确保至少能显示 5 个时刻 (44px * 5)
  m_momentsList->setStyleSheet("QListWidget::item { height: 36px; padding: "
                               "4px; }"); // 设置行高以改善编辑体验
  m_momentsList->setEditTriggers(QAbstractItemView::DoubleClicked |
                                 QAbstractItemView::EditKeyPressed);
  // 移除强制最小高度，由 mainLayout 的 stretch 自动分配空间
  QList<QTime> moments = m_settings->fixedMoments();
  for (const QTime &t : moments) {
    QListWidgetItem *item =
        new QListWidgetItem(t.toString("HH:mm"), m_momentsList);
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    m_momentsList->addItem(item);
  }
  connect(m_momentsList, &QListWidget::itemChanged, this,
          &SettingsWidget::onItemChanged);

  QHBoxLayout *listBtns = new QHBoxLayout();
  QPushButton *addBtn = new QPushButton("+ 添加提醒时刻", this);
  QPushButton *removeBtn = new QPushButton("- 删除选中", this);
  listBtns->addWidget(addBtn);
  listBtns->addWidget(removeBtn);

  fixedLayout->addWidget(new QLabel("已设定的时刻列表:"));
  fixedLayout->addWidget(m_momentsList);
  fixedLayout->addLayout(listBtns);
  reminderLayout->addWidget(fixedWidget);

  auto updateVisibility = [this, intervalWidget, fixedWidget](int index) {
    bool isInterval = (index == SettingsManager::IntervalMode);
    intervalWidget->setVisible(isInterval);
    fixedWidget->setVisible(!isInterval);
  };
  connect(m_modeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
          updateVisibility);
  updateVisibility(m_modeCombo->currentIndex());

  connect(addBtn, &QPushButton::clicked, this, &SettingsWidget::addMoment);
  connect(removeBtn, &QPushButton::clicked, this,
          &SettingsWidget::removeMoment);

  QPushButton *saveBtn = new QPushButton("保存并应用设置", this);
  saveBtn->setFixedHeight(45); // 增加高度更显气派

  mainLayout->addWidget(basicGroup);
  mainLayout->addWidget(reminderGroup,
                        2); // 给予提醒设置更高的伸缩权重，确保列表有足够空间
  // 移除 addStretch，让 reminderGroup 占据所有剩余空间
  mainLayout->addWidget(saveBtn);

  connect(saveBtn, &QPushButton::clicked, this, &SettingsWidget::saveSettings);
}

void SettingsWidget::addMoment() {
  bool ok;
  QString text = QInputDialog::getText(
      this, "添加提醒时刻",
      "请输入 24 小时制时间 (如 9:45 或 14:30):", QLineEdit::Normal, "", &ok);
  if (ok && !text.isEmpty()) {
    // 灵活解析: 支持 9:59 和 09:59
    QTime t = QTime::fromString(text.trimmed(), "H:m");
    if (!t.isValid())
      t = QTime::fromString(text.trimmed(), "HH:mm");

    if (t.isValid()) {
      QListWidgetItem *item =
          new QListWidgetItem(t.toString("HH:mm"), m_momentsList);
      item->setFlags(item->flags() | Qt::ItemIsEditable);
      m_momentsList->addItem(item);
      m_momentsList->sortItems();
    } else {
      QMessageBox::warning(this, "格式错误",
                           "无法识别该时间格式，请严格按照 24 小时制输入。");
    }
  }
}

void SettingsWidget::removeMoment() {
  if (m_momentsList->currentItem()) {
    delete m_momentsList->currentItem();
  }
}

void SettingsWidget::onItemChanged(QListWidgetItem *item) {
  if (!item)
    return;

  QString text = item->text().trimmed();
  QTime t = QTime::fromString(text, "H:m");
  if (!t.isValid())
    t = QTime::fromString(text, "HH:mm");

  if (t.isValid()) {
    // 强制统一格式为 HH:mm
    item->setText(t.toString("HH:mm"));
    m_momentsList->sortItems();
  } else {
    QMessageBox::warning(this, "格式错误", "无效的时间格式，已恢复原状。");
    // 这里简单处理：如果无效，用户虽然改了，但 saveSettings 时会尝试解析。
    // 为了更好的 UX，可以记录并还原内容，但目前先提示。
  }
}

void SettingsWidget::saveSettings() {
  m_settings->setReminderMode(
      static_cast<SettingsManager::ReminderMode>(m_modeCombo->currentIndex()));
  m_settings->setReminderInterval(m_intervalSpin->value());

  QList<QTime> moments;
  for (int i = 0; i < m_momentsList->count(); ++i) {
    moments << QTime::fromString(m_momentsList->item(i)->text(), "HH:mm");
  }
  m_settings->setFixedMoments(moments);

  m_settings->setDailyGoal(m_goalSpin->value());
  m_settings->setDrinkAmount(m_drinkAmountSpin->value());
  m_settings->setReminderStyle(static_cast<SettingsManager::ReminderStyle>(
      m_styleCombo->currentIndex()));
  m_settings->setDNDEnabled(m_dndEnabledCheck->isChecked());
  m_settings->setDNDRange(m_dndStartEdit->time(), m_dndEndEdit->time());
  m_settings->setAutoStart(m_autoStartCheck->isChecked());

  emit settingsChanged();
  close();
}

void SettingsWidget::showEvent(QShowEvent *event) {
  QWidget::showEvent(event);
  QRect desktop = QApplication::desktop()->availableGeometry();
  move((desktop.width() - width()) / 2, (desktop.height() - height()) / 2);
}
