#pragma once

char transducerTypes[15][25] = { "Voltage +/-10V", "Current 4...20mA", "Current +/-20mA",
"Temperature Sensor PT100", "Potentiometer", "Full bridge DC 5mV/V", "Full bridge DC 25mV/V",
"Full bridge CF 5mV/V", "Full bridge CF 25mV/V", "Half bridge DC 5mV/V", "Half bridge DC 25mV/V",
"Half bridge CF 5mV/V", "Half bridge CF 25mV/V", "Full bridge DC 100mV/V", "Full bridge DC 800mV/V" };

char signalNames[32][25] = { "", "", "Field Value",	"Gross Value", "Net Value", "Min Value", "Max Value", "Peak Value",
"Captured Value 1", "Captured Value 2", "ClipX Bus 1",	"ClipX Bus 2", "ClipX Bus 3", "ClipX Bus 4", "ClipX Bus 5",
"ClipX Bus 6", "None1", "None2", "None3", "None4", "None5",	"Calculated Value 1", "Calculated Value 2",
"Calculated Value 3", "Calculated Value 4",	"Calculated Value 5", "Calculated Value 6",	"Ethernet Value 1",
"Ethernet Value 2",	"Fieldbus 1", "Fieldbus 2",	"Analog Output" };
