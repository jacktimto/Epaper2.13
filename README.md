0.管脚定义
#define MOSI  23
#define SCK   19
#define CS    22
#define DC    21
#define RST   18
#define BUSY   5

1. 基于esp32 硬件spi, 2.13寸墨水屏
2. 在BSP/my_spi.c是底层gpio和spi的初始化,要移植的话改里面的参数
3. 在BSP/Epaper_func.c 是基础的描点操作
4. 在BSP/Fonts 是字库
5. 在BSP/GUI 是画线的操作
6. 在BSP/img 是一些图形
