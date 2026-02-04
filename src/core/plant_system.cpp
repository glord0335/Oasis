#include "plant_system.hpp"
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QSettings>
#include <QTextCodec>
#include <QTextStream>

PlantSystem::PlantSystem(QObject *parent)
    : QObject(parent), m_growthValue(0), m_todayWaterIntake(0),
      m_harvestCount(0), m_status(Seedling) {
  loadGrowthData(); // 加载持久化成长数据
  m_lastDrinkTime = QDateTime::currentDateTime();
  loadTodayRecords(); // 加载今日历史记录
}

void PlantSystem::recordDrink(int ml) {
  m_todayWaterIntake += ml;
  m_growthValue += 10; // 这里的数值可以更复杂一点
  m_lastDrinkTime = QDateTime::currentDateTime();

  // 添加饮水记录
  DrinkRecord record;
  record.timestamp = m_lastDrinkTime;
  record.amount = ml;
  m_drinkRecords.append(record);

  // 写入日志文件
  writeToLog(ml);
  // 持久化保存
  saveGrowthData();

  updateState();
}

void PlantSystem::updateState() {
  // 检查枯萎逻辑 (24小时不喝水开始枯萎)
  qint64 hoursSinceLastDrink =
      m_lastDrinkTime.secsTo(QDateTime::currentDateTime()) / 3600;

  if (hoursSinceLastDrink > 24) {
    m_status = Wilting;
  } else {
    calculateStatus();
  }

  emit plantUpdated();
}

void PlantSystem::calculateStatus() {
  if (m_growthValue < 50)
    m_status = Seedling;
  else if (m_growthValue < 150)
    m_status = Small;
  else if (m_growthValue < 300)
    m_status = Medium;
  else if (m_growthValue < 500)
    m_status = Large;
  else
    m_status = Flowering;
}

void PlantSystem::writeToLog(int ml) {
  // 创建 logs 目录（如果不存在）
  QDir logsDir("logs");
  if (!logsDir.exists()) {
    logsDir.mkpath(".");
  }

  // 生成日志文件名（按日期）
  QString dateStr = QDateTime::currentDateTime().toString("yyyy-MM-dd");
  QString logFileName = QString("logs/%1.log").arg(dateStr);

  // 以追加模式打开文件
  QFile logFile(logFileName);
  if (logFile.open(QIODevice::Append | QIODevice::Text)) {
    // 格式化日志内容
    QString timeStr = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString statusStr;
    switch (m_status) {
    case Seedling:
      statusStr = "萌芽期";
      break;
    case Small:
      statusStr = "小苗期";
      break;
    case Medium:
      statusStr = "成长期";
      break;
    case Large:
      statusStr = "繁茂期";
      break;
    case Flowering:
      statusStr = "开花期";
      break;
    case Wilting:
      statusStr = "缺水枯萎";
      break;
    }

    // 构建日志行：时间 | 饮水量 | 今日总量 | 成长值 | 状态
    QString logLine =
        QString("%1 | %2ml | 今日总量: %3ml | 成长值: %4 | 状态: %5\n")
            .arg(timeStr)
            .arg(ml)
            .arg(m_todayWaterIntake)
            .arg(m_growthValue)
            .arg(statusStr);

    // 使用 toUtf8() 转换为 UTF-8 字节流写入
    logFile.write(logLine.toUtf8());
    logFile.close();
  } else {
    qWarning() << "无法打开日志文件:" << logFileName;
  }
}

void PlantSystem::loadTodayRecords() {
  // 生成今日日志文件名
  QString dateStr = QDateTime::currentDateTime().toString("yyyy-MM-dd");
  QString logFileName = QString("logs/%1.log").arg(dateStr);

  QFile logFile(logFileName);
  if (!logFile.exists()) {
    qDebug() << "今日日志文件不存在，从零开始";
    return;
  }

  if (logFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QTextStream in(&logFile);
    in.setCodec("UTF-8");

    int recordCount = 0;
    while (!in.atEnd()) {
      QString line = in.readLine();
      // 解析日志行格式: "14:30:25 | 250ml | 今日总量: 500ml | 成长值: 20 |
      // 状态: 萌芽期"
      QStringList parts = line.split(" | ");
      if (parts.size() >= 4) {
        // 提取时间
        QString timeStr = parts[0].trimmed();
        QTime time = QTime::fromString(timeStr, "hh:mm:ss");

        // 提取饮水量
        QString amountStr = parts[1].trimmed();
        amountStr.remove("ml");
        int amount = amountStr.toInt();

        // 提取今日总量
        QString totalStr = parts[2].trimmed();
        totalStr.remove("今日总量: ").remove("ml");
        int total = totalStr.toInt();

        // 提取成长值
        QString growthStr = parts[3].trimmed();
        growthStr.remove("成长值: ");
        int growth = growthStr.toInt();

        // 创建记录
        DrinkRecord record;
        record.timestamp = QDateTime(QDate::currentDate(), time);
        record.amount = amount;
        m_drinkRecords.append(record);

        // 更新统计数据（使用最后一条记录的值）
        m_todayWaterIntake = total;
        // 注意：不在这里加载 m_growthValue，已由 loadGrowthData 处理
        m_lastDrinkTime = record.timestamp;

        recordCount++;
      }
    }

    logFile.close();

    // 更新状态
    calculateStatus();

    qDebug() << "已加载" << recordCount
             << "条今日饮水记录，总量:" << m_todayWaterIntake
             << "ml，成长值:" << m_growthValue;
  } else {
    qWarning() << "无法打开日志文件:" << logFileName;
  }
}

int PlantSystem::growthValue() const { return m_growthValue; }
PlantSystem::PlantStatus PlantSystem::status() const { return m_status; }
int PlantSystem::todayWaterIntake() const { return m_todayWaterIntake; }

QList<PlantSystem::DrinkRecord> PlantSystem::todayDrinkRecords() const {
  return m_drinkRecords;
}

int PlantSystem::harvestCount() const { return m_harvestCount; }

void PlantSystem::harvest() {
  if (m_status == Flowering || m_growthValue >= 500) {
    m_harvestCount++;
    m_growthValue = 0; // 重置成长周期
    saveGrowthData();
    calculateStatus();
    emit plantUpdated();
  }
}

void PlantSystem::saveGrowthData() {
  QSettings settings("Agil", "OasisGrowth");
  settings.setValue("total_growth", m_growthValue);
  settings.setValue("harvest_count", m_harvestCount);
}

void PlantSystem::loadGrowthData() {
  QSettings settings("Agil", "OasisGrowth");
  m_growthValue = settings.value("total_growth", 0).toInt();
  m_harvestCount = settings.value("harvest_count", 0).toInt();
}
