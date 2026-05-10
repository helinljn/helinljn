#!/usr/bin/env bash

set -euo pipefail

# Redis 7.0.15 manager for Ubuntu 24.04.
# It starts Redis with this project's config/redis.conf and avoids matching
# unrelated redis-server processes on the same host.

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CONFIG_FILE="${REDIS_CONFIG_FILE:-$SCRIPT_DIR/redis.conf}"
CLI_BIN="${REDIS_CLI_BIN:-redis-cli}"
SERVER_BIN="${REDIS_SERVER_BIN:-redis-server}"
REDIS_USER="${REDIS_USER:-redis}"
HOST="${REDIS_HOST:-127.0.0.1}"
PORT="${REDIS_PORT:-6379}"
PID_FILE="${REDIS_PID_FILE:-/run/redis/redis-server.pid}"
LOG_FILE="${REDIS_LOG_FILE:-/var/log/redis/redis-server.log}"
LOG_DIR="${REDIS_LOG_DIR:-$(dirname "$LOG_FILE")}"
DATA_DIR="${REDIS_DATA_DIR:-/var/lib/redis}"
RUN_DIR="$(dirname "$PID_FILE")"

sudo_cmd() {
    if [ "$(id -u)" -eq 0 ]; then
        "$@"
    else
        sudo "$@"
    fi
}

run_as_redis_user() {
    if [ "$(id -un)" = "$REDIS_USER" ]; then
        "$@"
    else
        sudo -u "$REDIS_USER" "$@"
    fi
}

redis_cli() {
    "$CLI_BIN" -h "$HOST" -p "$PORT" "$@"
}

prepare_dirs() {
    sudo_cmd install -d -m 0755 -o "$REDIS_USER" -g "$REDIS_USER" "$RUN_DIR" "$LOG_DIR" "$DATA_DIR"
}

prepare_log_file() {
    sudo_cmd touch "$LOG_FILE"
    sudo_cmd chown "$REDIS_USER:$REDIS_USER" "$LOG_FILE"
    sudo_cmd chmod u+rw "$LOG_FILE"
}

prepare_data_files() {
    sudo_cmd chown -R "$REDIS_USER:$REDIS_USER" "$DATA_DIR"
    sudo_cmd chmod -R u+rwX "$DATA_DIR"
}

is_redis_running() {
    redis_cli PING >/dev/null 2>&1
}

get_redis_pid() {
    if [ -f "$PID_FILE" ]; then
        cat "$PID_FILE"
        return 0
    fi

    pgrep -f "$SERVER_BIN .*$(basename "$CONFIG_FILE")" 2>/dev/null || true
}

start_redis() {
    echo "Starting Redis with $CONFIG_FILE..."

    if is_redis_running; then
        echo "Redis is already running (PID: $(get_redis_pid))"
        return 0
    fi

    prepare_dirs
    prepare_log_file
    prepare_data_files
    run_as_redis_user "$SERVER_BIN" "$CONFIG_FILE"

    for _ in {1..20}; do
        if is_redis_running; then
            echo "Redis started successfully (PID: $(get_redis_pid))"
            echo "Redis is listening on $HOST:$PORT"
            return 0
        fi
        sleep 0.5
    done

    echo "Failed to start Redis"
    return 1
}

stop_redis() {
    echo "Stopping Redis..."

    if ! is_redis_running; then
        echo "Redis is not running"
        return 0
    fi

    redis_cli SHUTDOWN SAVE >/dev/null 2>&1 || true

    for _ in {1..20}; do
        if ! is_redis_running; then
            echo "Redis stopped successfully"
            return 0
        fi
        sleep 0.5
    done

    local pid
    pid="$(get_redis_pid)"
    if [ -n "$pid" ]; then
        echo "Graceful shutdown timed out; sending TERM to PID $pid"
        sudo_cmd kill -TERM "$pid" 2>/dev/null || true
    fi

    for _ in {1..10}; do
        if ! is_redis_running; then
            echo "Redis stopped successfully"
            return 0
        fi
        sleep 0.5
    done

    echo "Failed to stop Redis"
    return 1
}

restart_redis() {
    stop_redis
    start_redis
}

redis_config_file() {
    local config_file
    config_file="$(redis_cli INFO server 2>/dev/null | awk -F: '/^config_file:/ {gsub(/\r/, "", $2); print $2}')"
    echo "$config_file"
}

check_redis_config() {
    if ! is_redis_running; then
        echo "Redis is not running"
        return 1
    fi

    local current_config
    current_config="$(redis_config_file)"

    if [ -z "$current_config" ]; then
        echo "Unable to determine active Redis config file"
        return 1
    fi

    echo "Active config file: $current_config"
    if [ "$current_config" = "$CONFIG_FILE" ]; then
        echo "Config file matches expected file"
        return 0
    fi

    echo "Config file does not match expected file"
    return 1
}

status_redis() {
    if ! is_redis_running; then
        echo "Redis is not running"
        return 1
    fi

    echo "Redis is running (PID: $(get_redis_pid))"
    echo "Connection: OK"
    echo "Address: $HOST:$PORT"
    echo "Config file: $(redis_config_file)"
    echo "Memory used: $(redis_cli INFO memory | awk -F: '/^used_memory_human:/ {gsub(/\r/, "", $2); print $2}')"
    echo "Keys in DB 0: $(redis_cli DBSIZE)"
    echo "Maxmemory policy: $(redis_cli CONFIG GET maxmemory-policy | awk 'NR==2')"
}

show_usage() {
    cat <<EOF
Redis Manager Script
Usage: $0 {start|stop|restart|status|check-config|help}

Environment overrides:
  REDIS_CONFIG_FILE  default: $CONFIG_FILE
  REDIS_LOG_FILE     default: $LOG_FILE
  REDIS_USER         default: $REDIS_USER
  REDIS_HOST         default: $HOST
  REDIS_PORT         default: $PORT
EOF
}

case "${1:-help}" in
    start)
        start_redis
        ;;
    stop)
        stop_redis
        ;;
    restart)
        restart_redis
        ;;
    status)
        status_redis
        ;;
    check-config)
        check_redis_config
        ;;
    help|--help|-h)
        show_usage
        ;;
    *)
        echo "Unknown command: $1"
        show_usage
        exit 1
        ;;
esac
