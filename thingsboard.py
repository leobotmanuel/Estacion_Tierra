import os
import time
import random
import sys
import paho.mqtt.client as mqtt
import json
import serial

arduino = serial.Serial('/dev/ttyACM0', baudrate=115200, timeout=1.0)

THINGSBOARD_HOST = 'localhost'
ACCESS_TOKEN = 'ASD'

INTERVAL = 1

lat =0
lon =0

sensor_data = {}

next_reading = time.time()

client = mqtt.Client()

# Ponemos el token de acceso
client.username_pw_set(ACCESS_TOKEN)

# Conectamos a Thingsboard
client.connect(THINGSBOARD_HOST, 1883, 60)
client.loop_start()

while True:
    linea = ""
    prov = ""
    datosthingsboard = [None] * 26
    contador = 0

    linea = str(arduino.readline())

    if linea is not None:

        for h in range(len(linea)):
            if (linea[h] != ','):
                prov = prov + linea[h]

            elif (linea[h] == ','):
                datosthingsboard[contador] = prov
                prov = ""
                contador += 1
            temp = datosthingsboard[1]
            pres = datosthingsboard[2]
            hum = datosthingsboard[3]
            duv = datosthingsboard[5]
            co2 = datosthingsboard[6]
            gas = datosthingsboard[7]
            velo = datosthingsboard[24]
            alt = datosthingsboard[25]
            if (datosthingsboard[22] is not None):
                latgrad = float(datosthingsboard[22]) // 100
                latmin = float(datosthingsboard[22]) % 100
                lat = latgrad + (latmin/60)




            if (datosthingsboard[23] is not None):
                longrad = float(datosthingsboard[23]) // 100
                lonmin = float(datosthingsboard[23]) % 100
                lon = -(longrad + (lonmin/60))

            sensor_data['temperaturaBME'] = temp
            sensor_data['presionBME'] = pres
            sensor_data['humedadBME'] = hum
            sensor_data['DUV'] = duv
            sensor_data['CO2'] = co2
            sensor_data['GasesVolatiles'] = gas
            sensor_data['velocidadGPS'] = velo
            sensor_data['altitudGPS'] = alt
            sensor_data['LatitudGPS'] = str(lat)
            sensor_data['LongitudGPS'] = str(lon)

            # Mandamos los datos a Thingsboard
            client.publish('v1/devices/me/telemetry', json.dumps(sensor_data), 1)

        next_reading += INTERVAL
        sleep_time = next_reading - time.time()
        if sleep_time > 0:
            time.sleep(sleep_time)
