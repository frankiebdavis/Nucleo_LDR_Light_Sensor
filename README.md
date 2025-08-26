# STM32 LDR Light Sensor with LED Control

This project uses an **LDR (Light Dependent Resistor)** to measure ambient light with the STM32F303RE Nucleo board and automatically toggles an LED based on brightness. The ADC continuously samples the light level, UART prints readings to a terminal, and hysteresis logic prevents flickering at threshold values.

---

## Problem

How can an STM32 microcontroller automatically **control an LED based on room brightness**, while avoiding rapid flicker when the light level hovers near a threshold?

---

## Method

- **Analog Input (PA0):** LDR + resistor form a voltage divider; ADC1 reads the light intensity.  
- **Digital Output (PA10):** Drives an LED through a current-limiting resistor.  
- **Threshold Logic with Hysteresis:**  
  - LED turns **ON** if `lux < 600`  
  - LED turns **OFF** if `lux > 1000`  
  - Between 600–1000, LED **holds previous state** (prevents flicker).  
- **UART2 Transmission:** Prints live light readings and LED state every 250 ms.  

Key code excerpt:  
```c
HAL_ADC_Start(&hadc1);  
lux = HAL_ADC_GetValue(&hadc1);  
if (!led_on && lux < 600) { LED ON }  
else if (led_on && lux > 1000) { LED OFF }  
sprintf(msg, "Light: %hu | LED: %s\r\n", lux, led_on ? "ON" : "OFF");  
```

---

## Result

Sample UART output as lighting conditions change:  

```c
Light: 512 | LED: ON
Light: 580 | LED: ON
Light: 1203 | LED: OFF
Light: 1187 | LED: OFF
```


- Covering the LDR simulates darkness → LED turns **ON**.  
- Bright light or flashlight → LED turns **OFF**.  
- Hysteresis ensures the LED doesn’t flicker around the threshold.  

Watch the demo: 

[![Watch the tutorial](https://img.youtube.com/vi/2mi29EBLYoI/hqdefault.jpg)](https://youtube.com/shorts/2mi29EBLYoI?feature=share)

---

## Hardware

- STM32F303RE Nucleo board  
- Breadboard  
- LDR sensor  
- 10kΩ resistor (pull-down for LDR divider)  
- LED  
- 220Ω resistor (LED current limit)  
- Jumper wires  

### Circuit

```
           3.3V
            │
           [LDR]
            │──── PA0 (ADC input)
            │
           └─────┬──── GND
                [10kΩ]
                 │
                GND

   PA10 ─── [220Ω] ───►|─── GND
                      LED
```

---

## Testing

1. Upload the program using **STM32CubeIDE**.  
2. Open PuTTY (or any serial terminal).  
   - Baud rate: **38400**  
   - COM port: check Device Manager.  
3. Observe live UART output.  
4. Cover the LDR → LED turns **ON**.  
5. Shine light → LED turns **OFF**.  

---

## Takeaways

- Implemented **ADC sampling** with STM32 HAL.  
- Applied **hysteresis logic** to eliminate flicker.  
- Used **UART debugging** for live feedback.  
- Combined **analog sensing + GPIO control** for a practical embedded application.  

---
