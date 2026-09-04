RFID Pin	     Arduino UNO
SDA (SS)	      D10
SCK	            D13
MOSI          	D11
MISO          	D12
RST           	D9
GND           	GND
3.3V	          3.3V ⚠️ ONLY
❌ NEVER use 5V on RFID
✔ Keep wires short (10–15 cm max)

LCD Pin	       Arduino UNO
VCC	            5V
GND	            GND
SDA           	A4
SCL           	A5

Relay Pin    	Arduino UNO
IN	            D7
VCC           	5V
GND           	GND
HIGH = Unlock
LOW = Lock

IR Pin	      Arduino UNO
OUT	            D6
VCC           	5V
GND           	GND
LOW = Object detected → LOCK door
HIGH = No detection → keep unlocked state

Add Button	      Arduino UNO
One side	          D2
Other side        	GND

Remove Button	    Arduino UNO
One side	          D3
Other side	        GND
Press = LOW
Release = HIGH

Buzzer Pin	      Arduino UNO
+	                  D8
-                 	GND

⚡ POWER SUMMARY
Module	          Voltage
RFID	              3.3V ONLY
LCD                	5V
Relay	              5V
IR Sensor         	5V
Buzzer            	5V

👉 ALL components MUST share:

GND connected together
