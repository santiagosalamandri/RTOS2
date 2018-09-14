/*R1)	Utilizar protocolo												OK
 *R1.1)	Leer con interrupcion por rx uart								OK
 *R1.2)	Ignorar si no empieza son STX									Revisar
 *R1.3) Ignorar si no termina con ETX									Revisar
 *R1.4) Paquete ok se copia a mem dinamica								OK
 *R1.5) Puntero a paquete valido con OP 0 en la cola "queMayusculizar"	NO
 *R1.6) Convertir a mayus los paquetes de la cola "queMayusculizar"		NO
 *R1.7) Puntero a paquete valido con OP 1 en la cola "queMinusculizar"	NO
 *R1.8) Convertir a minus los paquetes de la cola "queMinusculizar"		NO
 *R1.9) Solo cambiar los alfabeticos a-z A-Z							OK
 *
 *R2) Devolver los paquetes procesados por el mismo canal				OK
 *R2.1) Puntero a paquete mayusculizado en la cola "queMayusculizados"  NO
 *R2.2) Paquetes recibidos en "queMayusculizados" a UART				NO
 *R2.3) Puntero a paquete minusculizado en la cola "queMinusculizados"  NO
 *R2.4) Paquetes recibidos en "queMminusculizados" a UART				NO
 *R2.5) Enviar paquetes con interrupcion tx vacio						NO
 *R2.6) Al terminar de enviar, liberar memoria dinamica asignada
 *
 *R3) Usar algoritmo de asignacion de memoria dinamica determinista
 *R4) Usar algoritmo que no fragemente la memoria del sistema
 *R5) Luego de cada process reportar minimo de Stack
 *R6) Al iniciar cada process reportar Heap
 *
 */
