Cosas que tenemos:
* 2 sensores de presión
* 2 celdas de carga
* 2 sensores de temperatura 
* 1 módulo de radio (Transmisor/Receptor)
* 1 módulo SD
* 1 un canal pirotecnico 
* 2 leds programables 
* 1 buzzer 

----Cosas que queremos hacer-----
* 2 sensores de presión                    [Leer hasta dos presiones]
* 2 celdas de carga                        [Leer hasta dos empujes]
* 2 sensores de temperatura                [Leer hasta dos temperaturas]
* 1 módulo de radio (Transmisor/Receptor)  [Enviar y/o recibir señales]
* 1 módulo SD                              [Guardar archivos de datos]
* 1 un canal pirotecnico                   [Detonar una carga pirotécnica]
* 2 leds programables                      [Indicar acciones y/o estados]
* 1 buzzer                                 [Indicar acciones y/o estados]
-----Estados---------
Estado 0: Inicializar módulos y variables
>> Incluir bibliotecas        (Q2HX711, LORA, SD.h, SPI.h)
>> Definir pines:             (Loadcell1, Loadcell2, pressure1,pressure2,temp1,temp2,SPI,LORA,LEDS,BUZZER)
>> Crear objetos:             (Loadcell1, Loadcell2)
>> Definir constantes: Calibración (Loadcell1, Loadcell2)
>> Variables long  ADC:       (Loadcell1, Loadcell2, pressure1,pressure2,temp1,temp2)
>> Variables float Unidades:  (Loadcell1, Loadcell2, pressure1,pressure2,temp1,temp2)
>> Variables string: Datos csv, nombre de archivo
>> Pasa a Estado 1

Estado 1 (setup): 
>> Inicializar comunicación serial 
>> Inicializar módulos: (LORA, SD, CP,LEDS,BUZZER)
>> Si no se inicializan marcar error
>> Crear archivo y encabezados en el archivo nombre de archivo
>> Inicialización correcta: 3 blinks(Sonido buzzer)
>> Pasa a Estado 2
 
Estado 2 (verificación "viva"):

>> 30 veces repetir (datos ambientales )
      --Lectura y guardado en variables ADC:       (Loadcell1, Loadcell2, pressure1,pressure2,temp1,temp2)
      --Lectura y guardado en variables Unidades:  (Loadcell1, Loadcell2, pressure1,pressure2,temp1,temp2)
      --Construcción datos csv con las variables ADC y Unidades
      --Enviar datos csv por LORA
      -- delay(1000)ms
>> Pasar a modo receptor

Estado 3 (Esperando señal de activación)
>> Hasta que no reciba la señal de activación
     --Encender led1
     --delay(200)ms
>> Pasar a modo transmisor 
>> Pasa a Estado 4
Estado 4 (Prueba estática)
>> Declaración e inicialización int variable contador = 0
>> Mientras (true)
      -- Si contador es = 75 entonces ---> (activa carga pirotécnica y pitido buzzer)
      -- Lectura y guardado en variables ADC:       (Loadcell1, Loadcell2, pressure1,pressure2,temp1,temp2)
      -- Lectura y guardado en variables Unidades:  (Loadcell1, Loadcell2, pressure1,pressure2,temp1,temp2)
      -- Construcción datos csv con las variables ADC y Unidades
      -- Enviar datos csv por LORA
      -- Guardar datos csv en la SD
      -- Blink LED2
      -- Contador+1
      -- delay(200)ms
>> Pasa a estado 5

Estado 5 (Desconectar)
>> Apagado total 



 
