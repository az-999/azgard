#!/usr/bin/env bash
set -euo pipefail

# Debian обычно использует /run/nut для PID/lock, но в контейнере
# это не всегда создается автоматически.
mkdir -p /run/nut /var/run/nut
chown -R root:nut /run/nut /var/run/nut || true
chmod 775 /run/nut /var/run/nut || true

echo "[NUT] Starting UPS driver(s)..."
upsdrvctl -u root start

echo "[NUT] Starting upsd..."
/usr/sbin/upsd -u root

echo "[NUT] Starting upsmon in foreground..."
exec /usr/sbin/upsmon -F
