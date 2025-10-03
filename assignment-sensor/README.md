# Assignment: Systemd Service que registra un sensor simulado

Este proyecto implementa un **servicio en systemd** que arranca en `multi-user.target` y ejecuta un programa en **C** que simula la lectura de un sensor.  
El servicio registra periódicamente un valor junto con la **marca de tiempo ISO-8601** en un archivo de log bajo `/tmp`.  
Si `/tmp` no está disponible, utiliza `/var/tmp` como ruta de respaldo.

---

## 📌 Características

- Programa escrito en **C**, compilado con `make`.
- Registro en archivo: `TIMESTAMP | VALUE`.
- Intervalo configurable (por defecto: 5 segundos).
- Manejo de **SIGTERM** para apagado limpio.
- Arranca automáticamente con **systemd** en `multi-user.target`.

---

## 📂 Estructura del proyecto

| Archivo/Directorio               | Descripción                                                                          |
| :------------------------------- | :----------------------------------------------------------------------------------- |
| ├─ **src/**                      | Código fuente principal en C.                                                        |
| │ └─ `main.c`                    | Archivo principal de la aplicación.                                                  |
| ├─ **systemd/**                  | Archivos de configuración de servicio para Linux.                                    |
| │ └─ `assignment-sensor.service` | Unidad de servicio systemd para la aplicación (sensor).                              |
| ├─ **ai/**                       | Evidencia y registro del uso de Inteligencia Artificial (IA).                        |
| │ ├─ `prompt-log.md`             | Registro de los prompts (instrucciones) dados a la IA.                               |
| │ ├─ `reflection.md`             | Reflexión sobre el uso de la IA y su impacto en el proyecto.                         |
| │ └─ `provenance.json`           | Metadatos o procedencia del contenido generado o asistido por IA.                    |
| ├─ **tests/**                    | Contiene procedimientos y resultados de pruebas.                                     |
| │ └─ `manual-tests.md`           | Documentación de pruebas manuales realizadas.                                        |
| ├─ `Makefile`                    | Archivo de utilidad para compilar el código y automatizar tareas.                    |
| └─ `README.md`                   | Documento principal con la introducción del proyecto, configuración e instrucciones. |

---

## ⚙️ Requisitos

- Linux con `systemd` (ej. Debian, Ubuntu, Kali, etc.).
- Compilador de C (`gcc`).
- Acceso `sudo` para instalar el servicio.

---

## 🚀 Instrucciones de uso

### 1. Clonar y compilar

```bash
git clone <URL_DE_TU_REPO>
cd assignment-sensor
make
```

### 2. Instalar binario y servicio

```bash
sudo cp assignment-sensor /usr/local/bin/
sudo cp systemd/assignment-sensor.service /etc/systemd/system/
sudo systemctl daemon-reload
sudo systemctl enable --now assignment-sensor.service
```

### 3. Verificar estado

```bash
systemctl status assignment-sensor
```

### 4. Ver logs

```bash
tail -f /tmp/assignment_sensor.log
```

Si /tmp no es escribible, los logs se guardarán en:

```bash
/var/tmp/assignment_sensor.log
```

### 5. Detener el servicio

```bash
sudo systemctl stop assignment-sensor
```

El programa recibe SIGTERM, cierra el archivo y termina correctamente.

### 6. Desinstalar

```bash
sudo systemctl disable --now assignment-sensor.service
sudo rm /usr/local/bin/assignment-sensor
sudo rm /etc/systemd/system/assignment-sensor.service
sudo systemctl daemon-reload
```
