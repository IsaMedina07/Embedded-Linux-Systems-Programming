# Manual tests for assignment-sensor

## Requisitos previos

- Binary instalado en `/usr/local/bin/assignment-sensor`
- Unit file copiado a `/etc/systemd/system/assignment-sensor.service`
- systemd activo (en WSL: ver README para habilitarlo)

---

## 1) Happy Path

1. `sudo systemctl daemon-reload`
2. `sudo systemctl enable --now assignment-sensor.service`
3. Esperar ~10 segundos (intervalo por defecto = 5s).
4. `tail -n 10 /tmp/assignment_sensor.log` → deberías ver líneas:
   2025-10-02T22:10:01.123Z | 0xDEADBEEF

5. `sudo systemctl status assignment-sensor.service`

**Resultado esperado:** líneas con ISO8601 + valor hex, servicio `active (running)`.

---

## 2) Fallback Path (/tmp no escribible)

1. Remontar /tmp como no escribible (ejemplo):
   - `sudo mount --bind /var/empty /tmp` (prepara /var/empty previamente) _o_ ejecutar container/chroot donde /tmp no sea escribible.
2. Reiniciar el servicio: `sudo systemctl restart assignment-sensor.service`
3. Verificar `/var/tmp/assignment_sensor.log` (fallback).

**Resultado esperado:** el servicio detecta fallo al abrir `/tmp/...`, crea/escribe en `/var/tmp/assignment_sensor.log` con mismas líneas. En stderr del unit (journal) verás la advertencia de fallback.

---

## 3) SIGTERM Handling

1. `sudo systemctl stop assignment-sensor.service`
2. `sudo journalctl -u assignment-sensor.service -n 50 --no-pager`

**Resultado esperado:** Mensaje `INFO: received stop signal, shutting down.` en journal y no lines parciales en el log.

---

## 4) Failure Path (device inexistente)

1. Edita unit temporalmente: `ExecStart=/usr/local/bin/assignment-sensor --device /dev/fake0`
2. `sudo systemctl daemon-reload`
3. `sudo systemctl start assignment-sensor.service`
4. `sudo systemctl status assignment-sensor.service` y `sudo journalctl -u assignment-sensor.service -n 50`

**Resultado esperado:** service failed to start; exit code distinto de 0; journal muestra `FATAL: cannot open device`.

---

## 5) Restart test (optional)

1. Kill process: `sudo pkill -f assignment-sensor`
2. `sudo systemctl status assignment-sensor.service` → debe reiniciarse por `Restart=on-failure`.
