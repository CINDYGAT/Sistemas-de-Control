Archivo labee.ino 

** Práctica 2 **

Botón externo LOW: modo analógico - analógico. Potenciómetro conectado a un pin analógico A0 el cual controla el PWM de un motor los valores que se pueden obtener están entre 0 y 1023. Motor conectado a un puente H. Puente H habilitado únicamente para un motor. 

• Rango 1 (ejemplo, 0 a 300): el motor está fijo, no tiene movimiento.  Velocidad 0

•	Rango 2 (ejemplo, 301 a 700): el motor está “viendo” al frente y gira a la derecha hasta 90°

•	Rango 3 (ejemplo, 701 a 1023): el motor está “viendo” al frente y gira hacia la izquierda, hasta -90°

Botón externo en HIGH: 

La velocidad del ventilador encargado de controlar los niveles de velocidad, hecho de forma digital.  Es decir, se contarán con 3 botones, para los cuales 1 es de encendido/apagado, y los otros 2 deberán configurar las siguientes velocidades:

Botón 1: 0 a 25% velocidad al presionar una vez, 50% velocidad al presionar 2 veces, 75% velocidad al presionar 3 veces y 100% de velocidad al presionar 4 veces.

Botón 2: de 100% a 75% al presionar 1 vez, 50% al presionar 2 veces, 25% al presionar 3 veces, ventilador OFF al presionar 4 veces

Botón 3: apagado/encendido.

Control de Motor DC con LCD y PWM

Este proyecto implementa el control de un motor DC mediante un puente H y un Arduino, utilizando una pantalla LCD para mostrar el estado del sistema. El motor puede controlarse tanto en velocidad (PWM) como en sentido de giro, y el sistema responde a entradas de botones para ajustar los parámetros.

🔹 Funcionalidad del código

Control de velocidad con PWM:

El duty cycle aumenta progresivamente cada vez que se presiona el botón asignado.

Control de dirección del motor:

Giro hacia adelante (FWD).

Giro en reversa (REV).

Apagado (OFF).

Interfaz en LCD 16x2:

Muestra en tiempo real:

El modo de funcionamiento.

El porcentaje del duty cycle aplicado.

Gestión de entradas digitales:
Se utilizan botones físicos para cambiar el duty cycle y el sentido de giro.

Optimización de impresión en LCD:
El código evita caracteres residuales en pantalla, limpiando solo las líneas necesarias.

🔹 Hardware utilizado

Arduino UNO/Nano.

Puente H (ej. L298N, L293D, o equivalente con transistores).

Motor DC de 12V nominal (recomendado para esta implementación).

Pantalla LCD 16x2 con interfaz I2C.

Botones pulsadores.

Resistencias y fuente de alimentación de 12V.

🔹 Procedimiento de uso

Conexión del hardware

Conecta el motor DC a la salida del puente H.

Alimenta el puente H con 12V (o el voltaje nominal del motor).

Conecta los pines de control del puente H a los pines D6 y D10 del Arduino.

Conecta los botones a los pines digitales definidos en el código (uno para aumentar, otro para disminuir el duty cycle).

Conecta la pantalla LCD 16x2 con I2C a los pines A4 (SDA) y A5 (SCL) del Arduino.

Asegúrate de compartir la tierra (GND) entre la fuente del motor, el puente H y el Arduino.

Carga del programa

Abre el código en el Arduino IDE.

Selecciona la placa correcta (Arduino UNO/Nano) y el puerto COM.

Carga el programa en el Arduino.

Uso del sistema

Al encender el sistema, la LCD mostrará el modo de operación y el ciclo de trabajo inicial (0%).

Presiona el botón de incremento para subir el duty cycle en pasos definidos (0%, 25%, 50%, 75%, 100%).

Presiona el botón de decremento para reducir el duty cycle en los mismos pasos.

El motor responderá en tiempo real a los cambios, aumentando o disminuyendo la velocidad según el ciclo de trabajo aplicado.

La LCD actualizará la información mostrando el modo actual y el duty cycle aplicado.

🔹 Notas importantes

El motor debe coincidir con el voltaje de alimentación del puente H para evitar saturación o comportamientos erráticos (ejemplo: no usar motor de 5V con 12V).

Se recomienda evitar el uso de lcd.clear() en cada ciclo para prevenir parpadeos, utilizando en su lugar funciones de limpieza de línea o sobreescritura con espacios.

Los pines de PWM utilizados en este proyecto son D6 y D10, para no interferir con el bus I2C de la LCD.
