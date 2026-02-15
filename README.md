# 基于 STM32F103C8T6 的简易麦轮底盘

## 项目简介

本项目是基于 STM32F103C8T6 开发的简易麦轮底盘，包含一个遥控器（一个摇杆 + 两个按键）和一个麦轮底盘。  
支持通过遥控器发送指令，实现底盘的简易全向移动。底盘功能采用模块化设计，基于 FreeRTOS 架构，具备一定的扩展性。  
适用于快速原型验证、教学演示或作为比赛项目的初期方案 ~~（甚至可用于高中比赛糊弄评委）~~。


## 开源协议

本项目采用 **MIT 开源协议**, 详见 [LICENSE](LICENSE)


## 前期准备

### 软件工具

| 工具 | 说明 |
|------|------|
| [STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html) | 用于生成初始化代码 |
| [Visual Studio Code](https://code.visualstudio.com/) | 代码编辑与开发环境 |
| [STM32 ST-LINK Utility](https://www.st.com/en/development-tools/stsw-link004.html) | 烧录与调试工具（也可用于擦除芯片） |
| [Keil uVision5](https://www.keil.com/) | 备选 IDE，若 VS Code 工具链配置失败可选用（参考其他开源项目时可能用到） |

### 硬件清单

| 组件 | 数量 |
|------|------|
| STM32F103C8T6 最小系统板 | 2 |
| 0.96 英寸 OLED 显示屏 | 2 |
| L298N 电机驱动板 | 2 |
| 12V 锂电池 | 1 |
| 稳压模块（如 LM2596） | 1 |
| JGB37-520 直流减速电机（带编码器） | 4 |
| 80mm 麦克纳姆轮 | 4 |
| 双层亚克力底盘 | 1 |
| 杜邦线（公对公、公对母、母对母） | 若干 |
| PH2.0 转杜邦公头线（≥200mm，用于电机编码器） | 4 |
| NRF24L01 无线收发模块 | 2 |
| 面包板 | 2 |
| 摇杆传感器模块 | 1 |
| 轻触式按键 | 2 |
| 跳线（可选，用于整洁布线） | 若干 |

### 辅助工具

| 工具 | 说明 |
|------|------|
| ST-Link 调试器 | 至少一个，两个更方便 |
| 万用表 | 必备，用于硬件调试 |
| 示波器 | 可选，用于高级调试 |


## 快速开始

### 1. 编程环境配置

#### VS Code 开发环境搭建

推荐使用 VS Code 搭配以下工具链：
- 参考视频教程：
  - [【爽！手把手教你用VSCode开发STM32】](https://www.bilibili.com/video/BV1QfbpzGENy/)
  - [【STM32终极开发环境搭建教程01】](https://www.bilibili.com/video/BV1svm1YYEQR/)

  （二选一即可，若配置失败可改用 Keil 或 STM32CubeIDE）

#### ST-Link 驱动安装

请自行搜索安装 ST-Link 驱动程序，确保调试器能被电脑识别。

#### STM32CubeMX 安装与支持包

- 安装 [STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html)
- 在 CubeMX 中安装 STM32F1 系列的支持包（可通过软件内置的包管理器安装）。

### 2. 硬件组装

参考图片进行底盘与遥控器的组装：

![组装1](https://github.com/user-attachments/assets/ba0c9c49-dbb0-4b61-b4da-e8c16b99860b)  
![组装2](https://github.com/user-attachments/assets/974ea66c-13ab-4c2b-b29c-b23582388f5c)  
![组装3](https://github.com/user-attachments/assets/30362503-de1a-464a-b0e5-65a28898e501)

### 3. 接线图
#### 底盘接线

<img width="1539" height="862" alt="437707046a38b6abfb77fe9d9f123a47" src="https://github.com/user-attachments/assets/b2acbcf3-dfb3-498f-ac3c-75d262b902cb" />


#### 遥控器接线

<img width="1538" height="857" alt="613b60adf4f6da3203e0257d750fc0e4" src="https://github.com/user-attachments/assets/13e12665-fdcb-4f5d-bc08-df00e11a20fe" />



## 常见问题

### 编译相关问题
**Q：如何向项目添加新的源文件？**  
A：将源文件拖入项目目录后，在 `CMakeLists.txt` 中找到 `target_sources`，将文件路径添加进去。头文件路径则添加到 `target_include_directories` 中。（具体操作可参考 [Keysking 的视频教程](https://www.bilibili.com/video/BV1QfbpzGENy/)） <!-- 待确认网址是否正确 -->


**Q：为什么修改文件名或移动文件后无法烧录？**  
A：CMake 编译时缓存了旧路径。删除 `project/build` 文件夹，然后重新编译即可。

### 无线通信问题
**Q：为什么直接发送摇杆的电位值数据会不可靠？**  
A：摇杆电位器输出范围为 0~4095，而 NRF24L01 一次只能发送 32 个字节（`uint8_t`，每个字节范围 0~255）。因此需要将 12 位数据拆分为两个字节发送，接收端再重组。

**Q：我想改用其他无线模块（如 RF-Nano），或通信失败如何排查？**  
A：请检查 NRF24L01 的初始化函数，确保收发双方的 `TX_PACKET`、`RX_PACKET` 结构体定义以及通信地址完全一致。更多细节可参考：[【模块教程】NRF24L01 无线通信模块](https://www.bilibili.com/video/BV16bN1zZES6/)

### OLED 显示屏问题
**Q：OLED 正确连接但无显示，代码未修改，可能是什么原因？**  
A：可能与 OLED 模块的版本有关。旧版 OLED 可能需要焊接一个电阻才能工作，建议直接购买新版模块（注意避开某些低价平台 ~~(PDD)~~，商家可能不标明版本）。


## 更新日志
| 日期   | 说明 |
|--------|------|
| 2.15   | 基础版本（包含基础注释） |
| 2.15   | 添加部分文字说明 |
| 2.15   | 更新图片，调整 README 格式 |
| 2.15   | 更新接线图 |
| 2.15   | 加入nrf模块资料 |


## 联系方式

如有问题或建议，欢迎提交 [GitHub Issues](https://github.com/poor-eriday/simple-chassis/issues)


## 致谢

感谢以下资源和项目的启发与参考：
- keysking [B站](https://space.bilibili.com/6100925) [官方网站](https://baud-dance.com/)
- 江协科技 [B站](https://space.bilibili.com/383400717) [官方网站](http://jiangxiekeji.com)

以及我们的 [Contributors](https://github.com/poor-eriday/simple-chassis/graphs/contributors) 和社区成员，感谢你们的支持与贡献！

<a href="https://github.com/poor-eriday/simple-chassis/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=poor-eriday/simple-chassis" />
</a>
<!-- Made with [contrib.rocks](https://contrib.rocks). -->
