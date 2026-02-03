# Oasis - 绿洲饮水助手 🌵💧

[![C++](https://img.shields.io/badge/C++-11-blue.svg)](https://en.cppreference.com/w/cpp/11)
[![Qt](https://img.shields.io/badge/Qt-5.12%2B-green.svg)](https://www.qt.io/)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

**Oasis** 是一款基于 Qt 开发的桌面级饮水提醒工具。它不仅能帮助你养成健康的补水习惯，还融入了有趣的“灵魂”和一套温馨的植物养成系统。

---

## ✨ 核心特性

### 🎭 丰富的提醒灵魂 (Four Souls)
不只是死板的弹窗，Oasis 提供四种截然不同的文案风格，让提醒变得更有趣：
- **标准清新** (Standard Style): 温柔莫兰迪色，默默守护你的健康。
- **亮剑·李云龙** (Li Yunlong Style): “他娘的意大利炮呢？” 硬核提醒，不喝水小心被特种兵突击。
- **带薪摸鱼** (Moyu Style): 打工人专属，“喝水是为了名正言顺地离开工位两分钟”，带薪喝水 yyds。
- **魔童哪吒** (Nezha Style): “我命由我不由天，喝水由爷不由命！” 叛逆又热血。

### 🌿 植物养成系统
你的每一次饮水记录都会转化为绿洲能量。
- 观察你的“生命之树”从萌芽到参天。
- 详细的数据统计，记录你每一天的水分摄入。

### 🛠️ 灵活的功能配置
- **双模式切换**: 支持“固定时刻提醒”和“循环间隔提醒”。
- **免打扰模式 (DND)**: 支持全局暂停提醒以及自定义夜间/午休免打扰时段。
- **极简 UI**: 丝滑的淡入淡出动画，不打扰你的工作流。
- **系统托盘**: 隐藏在后台，安静守护。

---

## 🚀 快速上手

### 依赖环境
- CMake (3.10+)
- Qt 5.12+ (建议使用 Qt 5.12 或更高版本)
- 支持 C++11 的编译器 (GCC, Clang, MSVC)

### 编译安装
```bash
# 克隆仓库
git clone https://github.com/your-username/Oasis.git
cd Oasis

# 创建并进入构建目录
mkdir build && cd build

# 配置项目
cmake ..

# 编译
make -j$(nproc)

# 运行
./Oasis
```

---

## 📂 项目结构

```text
Oasis/
├── src/
│   ├── core/           # 核心逻辑 (引擎、配置管理、养成系统)
│   │   ├── reminder_engine.cpp     # 提醒驱动器
│   │   ├── settings_manager.cpp    # 配置持久化
│   │   ├── plant_system.cpp        # 植物养成算法
│   │   └── warming_copy.hpp        # 灵魂文案库
│   └── ui/             # 界面实现 (Qt Widgets)
│       ├── popup_widget.cpp        # 动画弹窗
│       ├── settings_widget.cpp     # 设置中心
│       └── stats_widget.cpp        # 统计面板
├── resources/          # 静态资源 (QSS 样式表、图标)
├── CMakeLists.txt      # CMake 构建配置
└── README.md           # 你现在看到的
```

---

## 🎨 样式预览
*(您可以根据实际项目运行截图替换此处链接)*
- [主界面预览](assets/main_preview.png)
- [统计分析预览](assets/stats_view.png)

---

## 📄 开源协议
本项目采用 [MIT License](LICENSE) 许可协议。

---

**再忙也要喝水，愿每一滴水都能灌溉出你心中的绿洲。保持补水！💦**
