# DriverFran

### INTRODUCCIÓN
Este driver para Linux crea tres dispositivos de caracteres en /dev, cada uno encargado de realizar una comprobación de un Teorema matemático diferente basado en datos proporcionados por el usuario. Cuando un programa escribe en uno de estos dispositivos, el driver procesa la entrada, verifica una propiedad matemática y registra los resultados en los logs del sistema.

El primer dispositivo (DriverFran0) comprueba si tres números cumplen el Teorema de Pitágoras. El segundo (DriverFran1) determina si un número es primo utilizando el Teorema de Euclides. El tercero (DriverFran2) verifica si un número es divisible por otro, también basado en el Teorema de Euclides.

El driver maneja la apertura, escritura y cierre de los dispositivos, asegurándose de que cada operación se realice en el dispositivo correcto. Además, implementa la lógica necesaria para interpretar los datos recibidos y validar que el formato sea correcto antes de procesarlos.

Al ser cargado en el kernel, el driver registra sus dispositivos y los hace accesibles en /dev, y cuando se descarga, limpia todos los recursos asignados.

### FUNCIONAMIENTO
Para ver el desempeño del driver hay que seguir estos pasos:
- Abrimos una terminal y nos dirigimos a la carpeta donde tenemos el driver descargado.
- Ejecutamos el Makefile, compilando el código fuente del módulo del kernel (DriverFran.ko):
```
make
```
- Listamos los archivos del directorio para verificar que los archivos compilados y el módulo del kernel están presentes:
```
ls
```
Debería salir este resultado: <br>
![imagen](https://github.com/user-attachments/assets/1841daaf-7a5c-4e7f-b802-0479df4b49aa)
- Insertamos el módulo del kernel en el sistema (esto se carga en el kernel de Linux):
```
sudo insmod DriverFran.ko
```
- Cambiamos los permisos del dispositivo permitiendo lectura y escritura para todos los usuarios:
```
sudo chmod 666 /dev/DriverFran*
```
- Ejecutamos los drivers (estos son algunos ejemplos que cumplen los teoremas):
```
echo "3 4 5" > /dev/DriverFran0
```
```
echo "3" > /dev/DriverFran1
```
```
echo "10 5" > /dev/DriverFran2
```
- Mostramos los últimos mensajes del buffer del kernel para ver los mensajes de depuración del driver:
```
sudo dmesg | tail -n 50
```
Debería salir este resultado: <br>
![imagen](https://github.com/user-attachments/assets/c938969f-d26c-421d-bb2d-ba61da003825)
- Eliminamos el módulo DriverFran del kernel:
```
sudo rmmod DriverFran
```
- Borramos los archivos generados por el ```make``` para limpiar la compilación:
```sh
make clean
```
- Volvemos a listar nuevamente los archivos para confirmar que la limpieza se realizó correctamente:
```sh
ls
```
Debería salir este resultado: <br>
![imagen](https://github.com/user-attachments/assets/1a70e6d3-e885-40a8-9d32-63228e0d3d2a) <br><br>

Para terminar, estas serían todas las casuísticas posibles en cada driver:
- Que cumpla el Teorema.
- Que no cumpla el Teorema.
- Que se introduzcan más o menos números de los que se necesitan para comprobar el Teorema (DriverFran0 necesita 3, DriverFran1 necesita 1 y DriverFran2 necesita 2). <br>
![imagen](https://github.com/user-attachments/assets/6b7f1b5a-1593-4cbf-9b2d-705d20a50849)
