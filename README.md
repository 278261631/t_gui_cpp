# T-GUI Framework

一个类似napari的C++ GUI框架，基于Qt构建，支持插件系统和图层管理。

## 特性

- **图层管理系统**: 类似napari的图层架构，支持多种数据类型
- **插件系统**: 动态加载插件，支持UI和数据处理插件
- **现代UI**: 基于Qt6的现代用户界面
- **可扩展架构**: 模块化设计，易于扩展和定制
- **事件系统**: 组件间通信的事件驱动架构
- **配置管理**: 灵活的配置系统，支持JSON和QSettings

## 项目结构

```
t_gui_cpp/
├── CMakeLists.txt              # 主CMake配置文件
├── build.bat                   # Windows构建脚本
├── src/                        # 源代码目录
│   ├── main.cpp               # 程序入口
│   ├── core/                  # 核心组件
│   │   ├── Application.h/cpp  # 主应用程序类
│   │   ├── MainWindow.h/cpp   # 主窗口
│   │   ├── LayerManager.h     # 图层管理器
│   │   └── EventSystem.h      # 事件系统
│   ├── plugins/               # 插件系统
│   │   ├── PluginManager.h    # 插件管理器
│   │   ├── PluginInterface.h  # 插件接口
│   │   └── BasePlugin.h/cpp   # 基础插件类
│   ├── ui/                    # UI组件
│   │   ├── LayerWidget.h      # 图层控制面板
│   │   ├── ViewerWidget.h     # 主视图组件
│   │   └── ToolBar.h          # 工具栏
│   └── utils/                 # 工具类
│       ├── Logger.h           # 日志系统
│       └── Config.h           # 配置管理
├── plugins/                   # 插件目录
│   └── example_plugin/        # 示例插件
└── include/                   # 公共头文件
```

## 系统要求

- **操作系统**: Windows 10/11, Linux, macOS
- **编译器**: MSVC 2019+, GCC 9+, Clang 10+
- **Qt版本**: Qt 6.2+
- **CMake**: 3.16+

## 构建说明

### Windows

1. 安装Qt 6.2或更高版本
2. 安装CMake 3.16或更高版本
3. 确保CMake路径正确设置在`build.bat`中
4. 运行构建脚本：
   ```cmd
   build.bat
   ```

### Linux/macOS

```bash
mkdir build
cd build
cmake ..
make -j$(nproc)
```

## 使用说明

### 启动应用程序

构建完成后，可执行文件位于：
- Windows: `build/bin/Release/t_gui_cpp.exe`
- Linux/macOS: `build/bin/t_gui_cpp`

### 基本功能

1. **图层管理**: 右侧面板显示图层列表，支持添加、删除、重排序
2. **视图控制**: 中央视图支持缩放、平移、旋转
3. **工具栏**: 提供常用工具和视图控制
4. **插件**: 通过插件菜单管理和配置插件

## 插件开发

### 创建插件

1. 继承`PluginInterface`或其子接口
2. 实现必要的方法
3. 创建插件元数据JSON文件
4. 编译为动态库

### 示例插件

参考`plugins/example_plugin/`目录中的示例插件实现。

## 架构设计

### 核心组件

- **Application**: 应用程序主类，管理生命周期
- **MainWindow**: 主窗口，提供UI框架
- **LayerManager**: 图层管理，支持多种图层类型
- **PluginManager**: 插件管理，动态加载和卸载
- **EventSystem**: 事件系统，组件间通信

### 设计模式

- **单例模式**: Application, Logger等核心服务
- **观察者模式**: 事件系统和信号槽
- **策略模式**: 插件接口和实现
- **MVC模式**: 图层数据和视图分离

## 贡献指南

1. Fork项目
2. 创建特性分支
3. 提交更改
4. 创建Pull Request

## 许可证

MIT License - 详见LICENSE文件

## 联系方式

- 项目主页: https://github.com/your-username/t_gui_cpp
- 问题反馈: https://github.com/your-username/t_gui_cpp/issues
