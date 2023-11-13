import pyautogui
import serial
import argparse
import time
import logging
from ctypes import cast, POINTER
from comtypes import CLSCTX_ALL
from pycaw.pycaw import AudioUtilities, IAudioEndpointVolume

class MyControllerMap:
    def __init__(self):
        self.button = {
            0x1: 'q',
            0x2: '2',
            0x3: 'w',
            0x4: '3',
            0x5: 'e',
            0x6: 'r',
            0x7: '5',
            0x8: 't',
            0x9: '6',
            0xa: 'y',
            0xb: '7',
            0xc: 'u',
            } # Fast forward (10 seg) pro Youtube
        # self.button = ['q','2','w']

class SerialControllerInterface:
    # Protocolo
    # byte 1 -> Botão 1 (estado - Apertado 1 ou não 0)
    # byte 2 -> EOP - End of Packet -> valor reservado 'X'

    def __init__(self, port, baudrate):
        self.ser = serial.Serial(port, baudrate=baudrate)
        self.mapping = MyControllerMap()
        self.incoming = '0'
        # self.update_c = 0
        pyautogui.PAUSE = 0  ## remove delay
    
    def update(self):
        ## Sync protocol
        while self.incoming != b'X':
            self.incoming = self.ser.read()
            logging.debug("Received INCOMING: {}".format(self.incoming)) #if (self.update_c%20==0) else print(end='')

        data = self.ser.read()
        logging.debug("Received DATA: {}".format(data)) #if (self.update_c%20==0) else print(end='')

        if data == 0x1:
            logging.info(f"KEYUP {self.button[data]}") #if (self.update_c%20==0) else print(end='')
            pyautogui.keyUp(self.button[data])
        else:
            logging.info(f"KEYDOWN {self.button[data]}") #if (self.update_c%20==0) else print(end='')
            pyautogui.keyDown(self.button[data])

        # fader
        valor = self.ser.read()
        valor = int.from_bytes(valor)
        logging.info(f"Analogico: {valor}")
        devices = AudioUtilities.GetSpeakers()
        interface = devices.Activate(IAudioEndpointVolume._iid_, CLSCTX_ALL, None)
        volume = cast(interface, POINTER(IAudioEndpointVolume))
        vol =  max(0, min(100, abs(int(((valor-80)/3950)*100))))
        logging.info(f"NOVO VOLUME:  {vol}")
        volume.SetMasterVolumeLevelScalar(vol/100, None)
        print() #if (self.update_c%20==0) else print(end='')
        # self.update_c += 1


class DummyControllerInterface:
    def __init__(self):
        self.mapping = MyControllerMap()

    def update(self):
        pyautogui.keyDown(self.mapping.button['A'])
        time.sleep(0.1)
        pyautogui.keyUp(self.mapping.button['A'])
        logging.info("[Dummy] Pressed A button")
        time.sleep(1)


if __name__ == '__main__':
    interfaces = ['dummy', 'serial']
    argparse = argparse.ArgumentParser()
    argparse.add_argument('serial_port', type=str)
    argparse.add_argument('-b', '--baudrate', type=int, default=9600)
    argparse.add_argument('-c', '--controller_interface', type=str, default='serial', choices=interfaces)
    argparse.add_argument('-d', '--debug', default=False, action='store_true')
    args = argparse.parse_args()
    if args.debug:
        logging.basicConfig(level=logging.DEBUG)

    print("Connection to {} using {} interface ({})".format(args.serial_port, args.controller_interface, args.baudrate))
    if args.controller_interface == 'dummy':
        controller = DummyControllerInterface()
    else:
        controller = SerialControllerInterface(port=args.serial_port, baudrate=args.baudrate)
    fist = 1
    while True:
        if fist:
            controller.ser.write(b'0')
        controller.update()
