#ifndef PLANT_SYSTEM_HPP
#define PLANT_SYSTEM_HPP

#include <QDateTime>
#include <QObject>

class PlantSystem : public QObject {
  Q_OBJECT
public:
  enum PlantStatus { Seedling, Small, Medium, Large, Flowering, Wilting };

  // 饮水记录结构
  struct DrinkRecord {
    QDateTime timestamp;
    int amount; // ml
  };

  explicit PlantSystem(QObject *parent = nullptr);

  void recordDrink(int ml);
  void updateState(); // 每小时或每天调用一次，更新枯萎逻辑

  int growthValue() const;
  PlantStatus status() const;
  int todayWaterIntake() const;
  int harvestCount() const;
  void harvest();                               // 收成逻辑
  QList<DrinkRecord> todayDrinkRecords() const; // 获取今日饮水记录

signals:
  void plantUpdated();

private:
  int m_growthValue;
  int m_todayWaterIntake;
  int m_harvestCount; // 收成次数
  QDateTime m_lastDrinkTime;
  PlantStatus m_status;
  QList<DrinkRecord> m_drinkRecords; // 今日饮水记录

  void calculateStatus();
  void writeToLog(int ml); // 写入日志文件
  void loadTodayRecords(); // 从日志文件加载今日记录
  void saveGrowthData();   // 持久化成长数据
  void loadGrowthData();   // 加载持久化成长数据
};

#endif // PLANT_SYSTEM_HPP
