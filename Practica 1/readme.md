# Practica Blur


## Instalación OpenCV

### Instalar CMake
De todas las versiones que probé esta fue la primera que funcionó sin errores.
Hay que tener en cuenta que es necesario instalar la GUI, para después en modo gráfico agregar y eliminar las dependencias que se van a utilizar en el proyecto, como OpenMP y CUDA.

[http://embedonix.com/articles/linux/installing-cmake-3-5-2-on-ubuntu/](http://embedonix.com/articles/linux/installing-cmake-3-5-2-on-ubuntu/)

### Ejecutar Script de instalación de OpenCV
Luego se descarga y ejecuta el siguiente script (dandole primero permisos de sudo chmod +x scInst.sh ), desde el lugar donde quiere que queden los archivos de instalacion de OpenCV
[https://github.com/dacaitac/Paralela/blob/master/scInst.sh](https://github.com/dacaitac/Paralela/blob/master/scInst.sh)
que clona la librería y alista los paquetes que se necesitan en CMake

Y ya para la instalación se sigue este otro tutorial
[https://towardsdatascience.com/how-to-install-opencv-and-extra-modules-from-source-using-cmake-and-then-set-it-up-in-your-pycharm-7e6ae25dbac5](https://towardsdatascience.com/how-to-install-opencv-and-extra-modules-from-source-using-cmake-and-then-set-it-up-in-your-pycharm-7e6ae25dbac5)

Cuando se esté haciendo la instalación es importante seleccionar WITH_OPENMP y deseleccionar todas las que dicen DNN

## No olvidar crear el archivo opencv.pc  en la ruta

    /usr/local/lib/pkgconfig/opencv.pc

con el siguiente codigo (INCOMPLETO)

    prefix=/usr/local
    exec_prefix=${prefix}
    includedir_old=${prefix}/include
    includedir_new=${prefix}/include/opencv
    libdir=${exec_prefix}/lib
    
    Name: OpenCV
    Description: Opencv
    Version: 3.4.4
    Cflags: -I${includedir_old} -I${includedir_new}
    Libs: -L${libdir} -lfoo


#### Hay que asegurarse de  que pkg-config quede instalado, porque se necesita para compilar
[https://people.freedesktop.org/~dbn/pkg-config-guide.html](https://people.freedesktop.org/~dbn/pkg-config-guide.html)

## Compilar

Por ahora estoy utilizando este comando para compilar

    g++ DisplayImage.cpp -o app -I/usr/include/opencv -L/usr/lib/x86_64-linux-gnu -lopencv_shape -lopencv_stitching -lopencv_superres -lopencv_videostab -lopencv_aruco -lopencv_bgsegm -lopencv_bioinspired -lopencv_ccalib -lopencv_datasets -lopencv_dpm -lopencv_face -lopencv_freetype -lopencv_fuzzy -lopencv_hdf -lopencv_line_descriptor -lopencv_optflow -lopencv_video -lopencv_plot -lopencv_reg -lopencv_saliency -lopencv_stereo -lopencv_structured_light -lopencv_phase_unwrapping -lopencv_rgbd -lopencv_viz -lopencv_surface_matching -lopencv_text -lopencv_ximgproc -lopencv_calib3d -lopencv_features2d -lopencv_flann -lopencv_xobjdetect -lopencv_objdetect -lopencv_ml -lopencv_xphoto -lopencv_highgui -lopencv_videoio -lopencv_imgcodecs -lopencv_photo -lopencv_imgproc -lopencv_core

Pero se puede hacer más corto cuando se configura el archivo opencv.pc

