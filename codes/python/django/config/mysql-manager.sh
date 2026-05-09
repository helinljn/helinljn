#!/usr/bin/env bash

set -euo pipefail

# MySQL 8.0.45 manager for Ubuntu 24.04.
# It manages the system service and assumes the project config file is
# installed separately under the system MySQL configuration directory.

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CONFIG_FILE="${MYSQL_CONFIG_FILE:-$SCRIPT_DIR/my.cnf}"
INSTALLED_CONFIG_FILE="${MYSQL_INSTALLED_CONFIG_FILE:-/etc/mysql/conf.d/gmtool.cnf}"
SYSTEMCTL_BIN="${MYSQL_SYSTEMCTL_BIN:-systemctl}"
SERVICE_NAME="${MYSQL_SERVICE_NAME:-mysql}"

sudo_cmd() {
    if [ "$(id -u)" -eq 0 ]; then
        "$@"
    else
        sudo "$@"
    fi
}

service_exists() {
    "$SYSTEMCTL_BIN" list-unit-files --type=service --no-legend --no-pager 2>/dev/null \
        | awk '{print $1}' \
        | grep -qx "${1}.service"
}

detect_service_name() {
    if service_exists "$SERVICE_NAME"; then
        echo "$SERVICE_NAME"
        return 0
    fi

    echo "MySQL service not found: ${SERVICE_NAME}.service" >&2
    echo "Set MYSQL_SERVICE_NAME explicitly if your service has a different name." >&2
    exit 1
}

run_systemctl() {
    sudo_cmd "$SYSTEMCTL_BIN" "$@"
}

is_mysql_running() {
    "$SYSTEMCTL_BIN" is-active --quiet "$MYSQL_SERVICE_UNIT"
}

wait_for_state() {
    local expected_state="$1"
    local attempts="${2:-20}"

    for _ in $(seq 1 "$attempts"); do
        if "$SYSTEMCTL_BIN" is-active --quiet "$MYSQL_SERVICE_UNIT"; then
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
    echo "Starting MySQL service: $MYSQL_SERVICE_UNIT"
    echo "Config file: $CONFIG_FILE"

    if is_mysql_running; then
        echo "MySQL is already running"
        return 0
    fi

    run_systemctl start "$MYSQL_SERVICE_UNIT"

    if wait_for_state active; then
        echo "MySQL started successfully"
        return 0
    fi

    echo "Failed to start MySQL"
    return 1
}

stop_mysql() {
    echo "Stopping MySQL service: $MYSQL_SERVICE_UNIT"

    if ! is_mysql_running; then
        echo "MySQL is not running"
        return 0
    fi

    run_systemctl stop "$MYSQL_SERVICE_UNIT"

    if wait_for_state inactive; then
        echo "MySQL stopped successfully"
        return 0
    fi

    echo "Failed to stop MySQL"
    return 1
}

restart_mysql() {
    echo "Restarting MySQL service: $MYSQL_SERVICE_UNIT"
    run_systemctl restart "$MYSQL_SERVICE_UNIT"

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
        echo "Service: $MYSQL_SERVICE_UNIT"
        echo "Config file: $CONFIG_FILE"
        run_systemctl status --no-pager --full "$MYSQL_SERVICE_UNIT" | sed -n '1,12p'
        return 0
    fi

    echo "MySQL is not running"
    echo "Service: $MYSQL_SERVICE_UNIT"
    return 1
}

check_config() {
    local source_file="$CONFIG_FILE"
    local installed_file="$INSTALLED_CONFIG_FILE"

    echo "Source config: $source_file"
    echo "Installed config: $installed_file"

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
  MYSQL_SYSTEMCTL_BIN  default: $SYSTEMCTL_BIN
EOF
}

MYSQL_SERVICE_UNIT="$(detect_service_name)"

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
