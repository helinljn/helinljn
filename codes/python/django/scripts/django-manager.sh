#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
MANAGE_PY="${DJANGO_MANAGE_PY:-$PROJECT_ROOT/manage.py}"
PYTHON_BIN="${DJANGO_PYTHON_BIN:-python}"
BIND_HOST="${DJANGO_BIND_HOST:-0.0.0.0}"
BIND_PORT="${DJANGO_BIND_PORT:-8000}"
LOG_DIR="${DJANGO_LOG_DIR:-$PROJECT_ROOT/logs}"
LOG_FILE="${DJANGO_LOG_FILE:-$LOG_DIR/django-server.log}"
PID_FILE="${DJANGO_PID_FILE:-$LOG_DIR/django-server.pid}"

if [ ! -f "$MANAGE_PY" ]; then
    echo "manage.py not found: $MANAGE_PY" >&2
    exit 1
fi

prepare_runtime() {
    mkdir -p "$LOG_DIR"
}

get_running_pid() {
    local pid=""

    if [ -f "$PID_FILE" ]; then
        pid="$(tr -d '[:space:]' < "$PID_FILE")"
        if [ -n "$pid" ] && kill -0 "$pid" 2>/dev/null; then
            echo "$pid"
            return 0
        fi
    fi

    pid="$(pgrep -f -- "manage.py runserver ${BIND_HOST}:${BIND_PORT}" | head -n 1 || true)"
    if [ -n "$pid" ] && kill -0 "$pid" 2>/dev/null; then
        echo "$pid"
        return 0
    fi

    return 1
}

is_running() {
    get_running_pid >/dev/null 2>&1
}

start_django() {
    local address="${BIND_HOST}:${BIND_PORT}"

    if is_running; then
        echo "Django is already running (PID: $(get_running_pid))"
        echo "Address: $address"
        return 0
    fi

    prepare_runtime
    rm -f "$PID_FILE"

    echo "Starting Django at $address"
    echo "Log file: $LOG_FILE"

    (
        cd "$PROJECT_ROOT"
        nohup "$PYTHON_BIN" "$MANAGE_PY" runserver "$address" --noreload >>"$LOG_FILE" 2>&1 &
        echo $! > "$PID_FILE"
    )

    for _ in $(seq 1 20); do
        if is_running; then
            echo "Django started successfully (PID: $(get_running_pid))"
            return 0
        fi
        sleep 0.5
    done

    echo "Failed to start Django" >&2
    if [ -f "$LOG_FILE" ]; then
        tail -n 20 "$LOG_FILE" >&2 || true
    fi
    rm -f "$PID_FILE"
    return 1
}

stop_django() {
    local pid

    if ! is_running; then
        echo "Django is not running"
        rm -f "$PID_FILE"
        return 0
    fi

    pid="$(get_running_pid)"
    echo "Stopping Django (PID: $pid)"

    kill "$pid" 2>/dev/null || true

    for _ in $(seq 1 20); do
        if ! is_running; then
            rm -f "$PID_FILE"
            echo "Django stopped successfully"
            return 0
        fi
        sleep 0.5
    done

    echo "Graceful stop timed out; sending KILL to PID $pid" >&2
    kill -KILL "$pid" 2>/dev/null || true

    for _ in $(seq 1 10); do
        if ! is_running; then
            rm -f "$PID_FILE"
            echo "Django stopped successfully"
            return 0
        fi
        sleep 0.5
    done

    echo "Failed to stop Django" >&2
    return 1
}

restart_django() {
    stop_django
    start_django
}

status_django() {
    local address="${BIND_HOST}:${BIND_PORT}"

    if is_running; then
        echo "Django is running (PID: $(get_running_pid))"
        echo "Address: $address"
        echo "Log file: $LOG_FILE"
        return 0
    fi

    echo "Django is not running"
    echo "Address: $address"
    echo "Log file: $LOG_FILE"
    return 1
}

show_usage() {
    cat <<EOF
Django Manager Script
Usage: $0 {start|stop|restart|status|help}

Environment overrides:
  DJANGO_MANAGE_PY   default: $MANAGE_PY
  DJANGO_PYTHON_BIN  default: $PYTHON_BIN
  DJANGO_BIND_HOST   default: $BIND_HOST
  DJANGO_BIND_PORT   default: $BIND_PORT
  DJANGO_LOG_DIR     default: $LOG_DIR
  DJANGO_LOG_FILE    default: $LOG_FILE
  DJANGO_PID_FILE    default: $PID_FILE
EOF
}

case "${1:-help}" in
    start)
        start_django
        ;;
    stop)
        stop_django
        ;;
    restart)
        restart_django
        ;;
    status)
        status_django
        ;;
    help|--help|-h)
        show_usage
        ;;
    *)
        echo "Unknown command: $1" >&2
        show_usage
        exit 1
        ;;
esac
