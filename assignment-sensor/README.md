## 🛠️ Assignment: Systemd Service que registra un sensor simulado

Este proyecto implementa un servicio en systemd que arranca en multi-user.target y ejecuta un programa en C que simula la lectura de un sensor.
El servicio registra periódicamente un valor junto con la marca de tiempo ISO-8601 en un archivo de log bajo /tmp.
Si /tmp no está disponible, utiliza /var/tmp como ruta de respaldo.

### 📌 Características

- Programa escrito en C, compilado con make.
- Registro en archivo: TIMESTAMP | VALUE.
- Intervalo configurable mediante flag (--interval <segundos>, por defecto: 5 segundos).
- Ruta de log configurable (--logfile <ruta>).
- Simulación de sensor usando /dev/urandom (puede cambiarse por otro).
- Manejo de SIGTERM para apagado limpio (flush y cierre del log).
- Arranca automáticamente con systemd en multi-user.target.

## 📂 Estructura del proyecto

```markdown
.
├─ 📂 _src/_ # 🧑‍💻 Código fuente principal en C.
│ └─ 📄 _assignment_sensor.c_ # 🎯 Archivo principal de la aplicación (el sensor).
|
├─ 📂 _systemd/_ # ⚙️ Archivos de configuración de servicio para Linux.
│ └─ 📄 _assignment-sensor.service_ # 🚀 Unidad de servicio systemd para la ejecución automática del programa.
|
├─ 📂 _ai/_ # 🤖 Evidencia de uso de Inteligencia Artificial en el desarrollo.
│ ├─ 📄 _prompt-log.md_ # 💬 Registro de los _prompts_ (instrucciones) dados a la IA.
│ ├─ 📄 _reflection.md_ # 🧠 Reflexión sobre el proceso de desarrollo asistido con IA.
│ └─ 📄 _provenance.json_ # 📜 Información de procedencia de la IA (modelos, fechas de uso, etc.).
|
├─ 📂 _tests/_ # 🧪 Contiene documentación de pruebas realizadas.
│ └─ 📄 _manual-tests.md_ # ✅ Procedimientos y resultados de pruebas manuales.
|
├─ 📄 _Makefile_ # 🛠️ Script para compilar el código e instalar/desinstalar el binario.
|
└─ 📄 _README.md_ # 📘 Documento principal con introducción, instrucciones de instalación y uso.
```

### ⚙️ Requisitos

- Distribución Linux con systemd (ej. Debian, Ubuntu, Kali, Arch).
- Compilador de C (gcc).
- Acceso con privilegios sudo para instalación del servicio.

## 🚀 Instrucciones de uso

1. Clonar y compilar

```bash
git clone https://github.com/IsaMedina07/Embedded-Linux-Systems-Programming.git
cd assignment-sensor
make
```

El binario se generará en build/assignment-sensor.

2. Instalar binario y servicio

```bash
sudo make install
sudo cp systemd/assignment-sensor.service /etc/systemd/system/
sudo systemctl daemon-reload
sudo systemctl enable --now assignment-sensor.service
```

3. Verificar estado del servicio

```bash
systemctl status assignment-sensor.service
```

4. Ver logs generados

Por defecto, los logs se guardan en:

```bash
tail -f /tmp/assignment_sensor.log
```

Si /tmp no es escribible, se utiliza la ruta de respaldo:

```bash
/var/tmp/assignment_sensor.log
```

5. Probar configuración manual

- Intervalo personalizado:

```bash
/usr/local/bin/assignment-sensor --interval 2
```

- Logfile personalizado:

```bash
/usr/local/bin/assignment-sensor --logfile /var/log/misensor.log
```

6. Detener el servicio

```bash
sudo systemctl stop assignment-sensor.service
```

El programa recibe SIGTERM, cierra el archivo y termina correctamente.

7. Desinstalar

```bash
sudo systemctl disable --now assignment-sensor.service
sudo rm /usr/local/bin/assignment-sensor
sudo rm /etc/systemd/system/assignment-sensor.service
sudo systemctl daemon-reload
```

## 🧪 Pruebas

En tests/manual-tests.md se documenta cada caso. Ejemplos:

1. Happy Path

Arrancar el servicio y confirmar que escribe logs:

```bash
tail -n 5 /tmp/assignment_sensor.log
```

2. Fallback Path
   Montar /tmp como no escribible y verificar que el log se crea en /var/tmp.

3. SIGTERM Handling

```bash
sudo systemctl stop assignment-sensor.service
```

Confirmar que el archivo de log queda cerrado sin líneas incompletas.

4. Error Path
   Ejecutar con un dispositivo inexistente:

```bash
/usr/local/bin/assignment-sensor --device /dev/fake0
```

El programa debe fallar y salir con código ≠ 0.

5. Restart Test (opcional)
   Matar el proceso manualmente:

```bash
sudo kill -9 <PID>
```

Verificar que systemd lo reinicia (si está configurado Restart=on-failure).

## 📖 Diseño

- Se eligió /dev/urandom como sensor simulado porque provee un flujo seguro de valores pseudoaleatorios, sin bloquear.
- Formato de tiempo: ISO-8601 en UTC, compatible con herramientas estándar.
- Fallback en /var/tmp para asegurar persistencia incluso si /tmp no está disponible.
- Se usa systemd para garantizar inicio automático, gestión de fallos y limpieza.

## ✨ Evidencia de IA

Este proyecto fue desarrollado con apoyo de herramientas de IA:

- Prompt Log (ai/prompt-log.md): histórico de interacciones.
- Reflexión (ai/reflection.md): análisis crítico del proceso asistido.
- Provenance (ai/provenance.json): metadatos de modelos y fechas.
