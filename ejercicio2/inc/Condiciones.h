/*R1)	Utilizar protocolo												OK
 *R1.1)	Leer con interrupcion por rx uart								OK
 *R1.2)	Ignorar si no empieza son STX									OK
 *R1.3) Ignorar si no termina con ETX									OK
 *R1.4) Paquete ok se copia a mem dinamica								OK
 *R1.5) Puntero a paquete valido con OP 0 en la cola "queMayusculizar"	OK
 *R1.6) Convertir a mayus los paquetes de la cola "queMayusculizar"		OK
 *R1.7) Puntero a paquete valido con OP 1 en la cola "queMinusculizar"	OK
 *R1.8) Convertir a minus los paquetes de la cola "queMinusculizar"		OK
 *R1.9) Solo cambiar los alfabeticos a-z A-Z							OK
 *
 *R2) Devolver los paquetes procesados por el mismo canal				OK
 *R2.1) Puntero a paquete mayusculizado en la cola "queMayusculizados"  OK
 *R2.2) Paquetes recibidos en "queMayusculizados" a UART				OK
 *R2.3) Puntero a paquete minusculizado en la cola "queMinusculizados"  OK
 *R2.4) Paquetes recibidos en "queMminusculizados" a UART				OK
 *R2.5) Enviar paquetes con interrupcion tx vacio						Revisar
 *R2.6) Al terminar de enviar, liberar memoria dinamica asignada		Revisar
 *
 *R3) Usar algoritmo de asignacion de memoria dinamica determinista		OK
 *R4) Usar algoritmo que no fragemente la memoria del sistema			Revisar
 *R5) Luego de cada process reportar minimo de Stack					Revisar
 *R6) Al iniciar cada process reportar Heap								Revisar
 *
 */
