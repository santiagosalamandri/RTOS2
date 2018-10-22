# RTOS2_CESE2018
## Descripción:
- Repositorio con trabajos practicos de RTOS 2.
- Link [Condiciones de aprobación]( https://docs.google.com/spreadsheets/d/1JzukJYtKw0y7-BZhlFbDcP16snVtVe4iEaZriVZ2zgk/edit?usp=sharing).

## Aclaraciones:

- Modificación de SAPI_UART.c: Es necesaria cambiar la libreria de la uart de la SAPI por la de este [LINK](https://github.com/santiagosalamandri/SAPI_UART).
- Cada carpeta de cada ejercicio cuenta con los requerimientos del anterior incluídos.
- Estructura del proyecto:
  - ejercicio1: Carpeta con el código para TP1
  - ejercicio2: Carpeta con el código para TP1 y TP2.
  - ejercicio3: Carpeta con el código para TP1, TP2 y TP3.
  - enviar_texto.py: Script de prueba. (Comando de prueba: ```python3 enviar_texto.py texto /dev/ttyUSB1 0```).
  - texto:  Texto de prueba, para comprobar funcionalidad del programa.
  - README.md: Este documento.

## Consideraciones de diseño
- Se modifico el script "enviar_texto.py" (y se agrego al proyecto), para que acepte todos los comandos de operación:

  - 0.Convertir los datos recibidos a mayúsculas.
  - 1.Convertir los datos recibidos a minúsculas.
  - 2.Reportar stack disponible.
  - 3.Reportar heap disponible.
  - 4.Mensajes de estado de la aplicación(No se utiliza explicitamente).
  - 5.Medir performance del sistema.
  - 6.Eventos de botones oprimidos/liberados.
- Cada comando devuelve a parte de la respuesta, el stack.
- El comando 6 devuelve el tiempo de pulsación de cada tecla **entre pedidos**, es decir:     
  - Si no presiono ninguna tecla devolverá, T1:0mS...T4:0mS.
  - Si se presiono la tecla 1 durante 1 segundo, devolverá, T1:1000mS,T2:0mS...T4:0mS.
  - En cada pedido, se resetean los tiempos.

# Documentación

## Estados

- ```ESPERANDO_STX```: Espera caracter STX, guarda el mismo en estructura, actualiza tiempos y pasa a ESPERANDO_OP

- ```ESPERANDO_OP```: Espera operacion y procede según corresponda, guardando la misma en estructura, actualizando tiempo y pasando a

- ```ESPERANDO_TAM```: Calcula el tamaño necesario y si es posible, actualiza la estructura, los tiempos y pasa a ESPERANDO_DATOS. Si no se puede reservar la memoria se pasa a - ESPERANDO_STX, descartando todo lo previo.

- ```ESPERANDO_DATOS```: Se guardan los datos en el pool pedido anteriormente y se pasa a ESPERANDO_ETX

- ```ESPERANDO_ETX```: Se prende LED3, se actualiza el pool y los tiempos, se llama a realizarOperacion() y pasa a ESPERANDO_STX

- ```MAYUSCULA```: Se prende LED1, se calcula el tamaño total y se encola en queMayusculizar

- ```MINUSCULA```: Se prende LED2, se calcula el tamaño total y se encola en queMinusculizar

- ```STACK```: Se calcula el stack y se encola en queTransmision

- HEAP: Se calcula el heap y se encola en queTransmision

- ```PERFORMANCE```: Se prende LED3, se calcula el tamaño total y se encola en queMedirPerformance

# TP1
## Colas :
- queMayusculizar.
- queMayusculizados.
- queMinusculizar.
- queMinusculizados.
- queEnvioUART.

## Funciones:
```
pools_init(): Inicializacion de pools de memoria segun la capacidad requerida
```
```
queues_init(): Inicializacion de colas a utilizar
```
```
pool_free(): Liberar la memoria del pool indicado
```
```
heapReport():  Reporta inicialmente la cantidad de heap disponible
```
```
uart_task_create(): Creacion de la tarea de manejo de UART
```
```
mayusculizador_task_create(): Creacion de la tarea de mayusculizado
```
```
minusculizador_task_create(): Creacionm de la tarea de minusculizado
```
```
minusculizador_task(): Desencola de queMinusculizar, llama a minusculizar(), encola en queEnvioUart, reporta stack.
```
```
minusculizar(): Si los caracteres se encuentran entre A y Z, sumar la diferencia ASCII para pasarlos a entre a y z.
```
```
mayusculizador_task(): Desencola de queMayusculizar, llama a mayusculizar(), encola en queEnvioUart, reporta stack.
```
```
mayusculizar(): Si los caracteres se encuentran entre a y z, resta la diferencia ASCII para pasarlos a entre A y Z.
```
```
stackreport(): Calculo del stack disponible luego del procesamiento e impresión de resultados.
```
```
procesarByteRecibido():
Estados de la maquina:
            ESPERANDO_STX(inicial y pordefecto)
            ESPERANDO_OP.
            ESPERANDO_TAM.
            ESPERANDO_DATOS.
            ESPERANDO_ETX.
```
```
getPool():
Decide entre:
            BLOQUE_POOL_CHICO.
            BLOQUE_POOL_MEDIANO.
            BLOQUE_POOL_GRANDE.
```
```
realizarOperacion():
Estados de la maquina:
            MAYUSCULA.
            MINUSCULA.
            STACK.
            HEAP.
```

# TP2
## Colas :
- queMayusculizar.
- queMinusculizar.
- queTransmision.
- queMedirPerformance (Simplificacion en cantidad).

## Funciones:
```
performance_task_create(): Creacion de tarea de medicion de performance
```
```
header_init(): Inicializacion de los valores de la estructura previa a enviarse. Incluye los tiempos.
```
```
sizeReport(): Calculo e impresion de la memoria pedida vs la memoria necesaria para la estructura.
```
```
timeReport(): Calculo e impresion de los resultados temporales
```
```
performance_task(): Desencola de queMedirPerformance, mide el tiempo_de_fin, llama a performance(), encola en queTransmision, reporta stack, tamaño y tiempos.
```
```
performance(): Idem mayusculizar().
```

Se modifican:

```
realizarOperacion(): Añadido el estado PERFORMANCE y la medicion de tiempos
```
```
mayusculizador_task(): Añadida la llamada a timeReport()
```
```
minusculizador_task(): Añadida la llamada a timeReport()
```

# TP3

Se agregan:
- Framework para manejo eventos.
- Libreria de manejos de timers (TimerService.h y .c).
- Libreria de manejos de leds (leds.h y .c).
- Libreria de manejos de pulsadores (pulsadores.h y .c).
- Libreria de manejos de tiempo de encendido de leds (tiempoLeds.h y .c).
- Tarea de manejador de eventos.


## Colas:
- queEventosBaja.

### Módulos registrados:
- ModuloDriverPulsadores.
- ModuloBroadcast.
- ModuloDriverLeds.
- ModuloDriverTiempoLeds.

### Aclaraciones:
- Se utiliza el modulo Broadcast para reenviar la notificacion de tecla presionada a los módulos DriverLed y TiempoLeds.
- Se agrega un nuevo comando en ```realizarOperacion()``` , para que detecte el nuevo comando y pida los datos de tiempo de pulsacion a traves de ```getTiempoPulsador()```.
