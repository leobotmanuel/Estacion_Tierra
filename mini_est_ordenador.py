import serial
import time

filename = 'datos_cansat.csv'
with open(filename, 'a') as file_object:
    file_object.write('clave,temperaturaBME,presionBME,humedadBME,altitudBME,DUV,CO2,GasesVolatiles,AcelerometroX,'
                      'AcelerometroY,AcelerometroZ,GiroscopioX,GiroscopioY,GiroscopioZ,MagnetometroX,MagnetometroY,'
                      'MagnetometroZ,presAlt,tempAlt,altAlt,bateria,IR,LatitudGPS,LongitudGPS,velocidadGPS,'
                      'altitudGPS\n')
    file_object.close()
while True:
    arduino = serial.Serial('/dev/ttyACM0', baudrate=115200, timeout=1.0)
    line = arduino.readline()
    try:
        lectura = str(line, 'UTF-8')
        with open(filename, 'a') as file_object:
            file_object.write(lectura)
            print(lectura)
            file_object.close()
    except:
        print("ERROR: ", line)

