# Embedded Session HW (solución ejemplo)

## Estructura

- `sensor/` - biblioteca de sensor (sensor.h/.c)
- `actuators/` - interfaz polimórfica y backends (LED, buzzer)
- `controller/ctl.c` - controlador cerrado (muestra la lógica)
- `Makefile` - objetivos `ctl64`, `ctl32`, `clean`
- `sensor_feed.csv` - ejemplo de entrada

## Compilación

Requisitos: `gcc` (para 64-bit). Para `ctl32` necesitas toolchain 32-bit (`gcc-multilib` en muchas distros).

Compilar 64-bit (por defecto):

```bash
make ctl64
```

## Para probarlo

```bash
Compilar:
   - 32-bit: make ctl32
   - 64-bit: make ctl64
Verificar binario: file ctl
Ejecutar: ./ctl
```
