import serial
import time
import os

# 設定 UART 連接埠 (QEMU 創建的 pseudo-terminal)
uart_port = "/dev/pts/1"  # 這個數字要根據你的 QEMU 輸出調整！
baud_rate = 115200

# 開啟 Serial 連接
ser = serial.Serial(uart_port, baud_rate, timeout=1)

def read_line(ser):
    received_string = ""
    while True:
        c = ser.read().decode()
        if c=="\r":
            continue
        if c=="\n":
            break
        received_string += c
    return received_string

size = os.stat("../kernel/kernel8.img").st_size 
size_bytes = size.to_bytes(4,"little")
ser.write(size_bytes)

received_size = read_line(ser)
print(received_size)

with open("../kernel/kernel8.img","rb") as f:
    ser.write(f.read())

received_content = read_line(ser)
print(received_content)

