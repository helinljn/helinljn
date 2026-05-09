#!/usr/bin/env bash

set -euo pipefail

# MySQL 8.0.45 manager for Ubuntu 24.04.
# It manages the system service and assumes the project config file is
# installed separately under the system MySQL configuration directory.

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CONFIG_FILE="${MYSQL_CONFIG_FILE:-$SCRIPT_DIR/my.cnf}"
INSTALLED_CONFIG_FILE="${MYSQL_INSTALLED_CONFIG_FILE:-/etc/mysql/conf.d/gmtool.cnf}"
SERVICE_BIN="${MYSQL_SERVICE_BIN:-service}"
SERVICE_NAME="${MYSQL_SERVICE_NAME:-mysql}"

sudo_cmd() {
    if [ "$(id -u)" -eq 0 ]; then
        "$@"
    else
        sudo "$@"
    fi
}

service_exists() {
    "$SERVICE_BIN" --status-all 2>/dev/null \
        | awk '{print $4}' \
        | grep -qx "$1"
}

detect_service_name() {
    if service_exists "$SERVICE_NAME"; then
        echo "$SERVICE_NAME"
        return 0
    fi

    echo "MySQL service not found: $SERVICE_NAME" >&2
    echo "Set MYSQL_SERVICE_NAME explicitly if your service has a different name." >&2
    exit 1
}

run_service() {
    local action="$1"
    shift

    sudo_cmd "$SERVICE_BIN" "$MYSQL_SERVICE_NAME" "$action" "$@"
}

show_config_paths() {
    echo "Source config: $CONFIG_FILE"
    echo "Installed config: $INSTALLED_CONFIG_FILE"
}

is_mysql_running() {
    "$SERVICE_BIN" "$MYSQL_SERVICE_NAME" status >/dev/null 2>&1
}

wait_for_state() {
    local expected_state="$1"
    local attempts="${2:-20}"

    for _ in $(seq 1 "$attempts"); do
        if is_mysql_running; then
            if [ "$expected_state" = "active" ]; then
                return 0
            fi
        else
            if [ "$expected_state" = "inactive" ]; then
                return 0
            fi
        fi
        sleep 0.5
    done

    return 1
}

start_mysql() {
    echo "Starting MySQL service: $MYSQL_SERVICE_NAME"
    show_config_paths

    if is_mysql_running; then
        echo "MySQL is already running"
        return 0
    fi

    run_service start

    if wait_for_state active; then
        echo "MySQL started successfully"
        return 0
    fi

    echo "Failed to start MySQL"
    return 1
}

stop_mysql() {
    echo "Stopping MySQL service: $MYSQL_SERVICE_NAME"

    if ! is_mysql_running; then
        echo "MySQL is not running"
        return 0
    fi

    run_service stop

    if wait_for_state inactive; then
        echo "MySQL stopped successfully"
        return 0
    fi

    echo "Failed to stop MySQL"
    return 1
}

restart_mysql() {
    echo "Restarting MySQL service: $MYSQL_SERVICE_NAME"
    run_service restart

    if wait_for_state active; then
        echo "MySQL restarted successfully"
        return 0
    fi

    echo "Failed to restart MySQL"
    return 1
}

status_mysql() {
    if is_mysql_running; then
        echo "MySQL is running"
        echo "Service: $MYSQL_SERVICE_NAME"
        show_config_paths
        run_service status | sed -n '1,12p'
        return 0
    fi

    echo "MySQL is not running"
    echo "Service: $MYSQL_SERVICE_NAME"
    show_config_paths
    return 1
}

check_config() {
    local source_file="$CONFIG_FILE"
    local installed_file="$INSTALLED_CONFIG_FILE"

    show_config_paths

    if [ ! -f "$source_file" ]; then
        echo "Source config file not found"
        return 1
    fi

    if [ ! -f "$installed_file" ]; then
        echo "Installed config file not found"
        return 1
    fi

    if cmp -s "$source_file" "$installed_file"; then
        echo "Config file matches"
        return 0
    fi

    echo "Config file differs from source"
    return 1
}

show_usage() {
    cat <<EOF
MySQL Manager Script
Usage: $0 {start|stop|restart|status|check-config|help}

Environment overrides:
  MYSQL_CONFIG_FILE    default: $CONFIG_FILE
  MYSQL_INSTALLED_CONFIG_FILE default: $INSTALLED_CONFIG_FILE
  MYSQL_SERVICE_NAME   default: mysql
  MYSQL_SERVICE_BIN    default: $SERVICE_BIN
EOF
}

MYSQL_SERVICE_NAME="$(detect_service_name)"

case "${1:-help}" in
    start)
        start_mysql
        ;;
    stop)
        stop_mysql
        ;;
    restart)
        restart_mysql
        ;;
    status)
        status_mysql
        ;;
    check-config)
        check_config
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
