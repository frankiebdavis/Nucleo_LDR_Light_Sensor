# 🌒 STM32 LDR Light Sensor with LED Control

This project reads ambient light using an analog LDR (Light Dependent Resistor) sensor and controls an external LED based on light intensity. The system uses ADC1 on an STM32F303RE Nucleo board to continuously sample the light level and transmits the value over UART. When it's dark, the LED turns on. When it's bright, the LED turns off, with a built-in hysteresis band to prevent flickering.

## 🔧 Hardware

- STM32F303RE Nucleo board  
- Breadboard  
- LDR sensor  
- 10kΩ pull-down resistor 
- LED 
- 220Ω resistor (LED current limit)  
- Jumper wires

### Circuit:

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

- **LDR + 10kΩ** forms a voltage divider; middle node goes to **PA0 (ADC input)**.
- **PA10** drives the LED through a current-limiting resistor.

- ### 🎥 Video Tutorial Reference

[![Watch the tutorial](https://img.youtube.com/vi/2mi29EBLYoI/hqdefault.jpg)](https://youtube.com/shorts/2mi29EBLYoI?feature=share)

## 📟 Behavior

The ADC samples ambient light intensity from the LDR circuit. UART prints the light level every 250 ms. The LED turns on in darkness and off in bright light using a hysteresis band:

| Condition                  | Action           |
|---------------------------|------------------|
| Light < 600               | LED turns ON     |
| Light > 1000              | LED turns OFF    |
| 600 ≤ Light ≤ 1000        | LED holds state  |

This avoids flickering when the light level hovers near a threshold.

### Sample Serial Output:
```
Light: 512 | LED: ON
Light: 580 | LED: ON
Light: 1203 | LED: OFF
Light: 982 | LED: OFF
```

## 🧠 Code Highlights

```c
// Read light intensity from ADC1 (PA0)
HAL_ADC_Start(&hadc1);
HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
lux = HAL_ADC_GetValue(&hadc1);

// LED logic with hysteresis
if (!led_on && lux < 600)
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);  // Turn LED ON
    led_on = 1;
}
else if (led_on && lux > 1000)
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET); // Turn LED OFF
    led_on = 0;
}

// Transmit status via UART2
sprintf(msg, "Light: %hu | LED: %s\r\n", lux, led_on ? "ON" : "OFF");
HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
```

## ✅ Testing

1. Upload the code via STM32CubeIDE.
2. Open PuTTY or any serial terminal.
   - **Baud rate**: `38400`
   - **COM port**: Auto-detected (check Device Manager)
3. Observe live light readings.
4. Cover the LDR to simulate darkness → LED turns ON.
5. Shine a flashlight or room light → LED turns OFF.

## 🧠 Skills Demonstrated

- STM32 ADC configuration
- UART communicationt
- Analog circuit design 
- GPIO control

---
