set(srcs "main.c"
         "../BSP/spi/my_spi.c"
         "../BSP/spi/Epaper_func.c"
         "../BSP/spi/Fonts/font12.c"
         "../BSP/spi/Fonts/font16.c"
         "../BSP/spi/Fonts/font20.c"
         "../BSP/spi/GUI/GUI_Paint.c"
         "../BSP/spi/img/ImageData.c"
         
   )

set(dirs "."
         "../BSP/spi"
         "../BSP/spi/Fonts"
         "../BSP/spi/GUI"
         "../BSP/spi/img"

   )

idf_component_register( SRCS ${srcs}
                        INCLUDE_DIRS ${dirs}
                      )
