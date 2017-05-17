
import matplotlib.pyplot as plt

ser = serial.Serial('COM5',9600)
print(ser.name)

ser.write('r')

raw = []
MAF = []
IIR = []
FIR = []

for i in range(99):
	serial_line = ser.readline()
	data_read = serial_line.split(",")
	print(i)
	print(float(data_read[1]), flat(data_read[2]), float(data_read[3]), float(data_read[4]))
	raw.append(float(data_read[1]))
	MAF.append(float(data_read[2]))
	IIR.append(float(data_read[3]))
	FIR.append(float(data_read[4]))

plt.plot(raw,label='raw')
plt.plot(MAF,label='MAF')
plt.plot(IIR,label='IIR')
plt.plot(FIR,label='FIR')
plt.legend()
plt.show()
	