#!/usr/bin/env bash
set -euo pipefail

mkdir -p /var/run/nut
chown -R root:nut /var/run/nut || true

echo "[NUT] Starting UPS driver(s)..."
upsdrvctl -u root start

echo "[NUT] Starting upsd..."
/usr/sbin/upsd -u root

echo "[NUT] Starting upsmon in foreground..."
exec /usr/sbin/upsmon -F
