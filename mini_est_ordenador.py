import serial
import time

filename = 'datos_cansat.csv'
with open(filename, 'a') as file_object:
    file_object.write('cabecera1, cabecera2, cabecera3\n')
    file_object.close()
while True:
    arduino = serial.Serial('/dev/ttyACM0', baudrate=9600, timeout=1.0)
    line = arduino.readline()
    lectura = str(line, 'UTF-8')
    with open(filename, 'a') as file_object:
        file_object.write(lectura)
        print(lectura)
        file_object.close()
