#!/bin/bash

# Redis Manager Script for WSL Ubuntu 24.04
# This script provides start, stop, and flush operations for Redis server

# Function to check if Redis is running
is_redis_running() {
    if pgrep -f "redis-server" > /dev/null; then
        return 0
    else
        return 1
    fi
}

# Function to get Redis PID
get_redis_pid() {
    pgrep -f "redis-server" 2>/dev/null
}

# Function to start Redis
start_redis() {
    echo "Starting Redis server..."

    if is_redis_running; then
        echo "Redis is already running (PID: $(get_redis_pid))"
        return 0
    fi

    # Start Redis
    sudo redis-server --daemonize yes

    sleep 2

    if is_redis_running; then
        echo "✓ Redis started successfully (PID: $(get_redis_pid))"
        echo "Redis is listening on port 6379"
    else
        echo "✗ Failed to start Redis"
        return 1
    fi
}

# Function to stop Redis
stop_redis() {
    echo "Stopping Redis server..."

    if ! is_redis_running; then
        echo "Redis is not running"
        return 0
    fi

    local pid=$(get_redis_pid)

    # Try graceful shutdown first
    sudo kill -TERM "$pid" 2>/dev/null

    # Wait for process to stop
    local count=0
    while is_redis_running && [ $count -lt 10 ]; do
        sleep 1
        count=$((count + 1))
    done

    # Force kill if still running
    if is_redis_running; then
        echo "Force stopping Redis..."
        sudo kill -KILL "$pid" 2>/dev/null
        sleep 1
    fi

    if ! is_redis_running; then
        echo "✓ Redis stopped successfully"
    else
        echo "✗ Failed to stop Redis"
        return 1
    fi
}

# Function to flush Redis data
flush_redis() {
    echo "Flushing Redis data..."

    if ! is_redis_running; then
        echo "Redis is not running. Starting Redis first..."
        if ! start_redis; then
            echo "✗ Failed to start Redis for flushing"
            return 1
        fi
    fi

    # Flush all databases
    redis-cli FLUSHALL

    if [ $? -eq 0 ]; then
        echo "✓ Redis data flushed successfully"
    else
        echo "✗ Failed to flush Redis data"
        return 1
    fi
}

# Function to show Redis status
status_redis() {
    if is_redis_running; then
        local pid=$(get_redis_pid)
        echo "✓ Redis is running (PID: $pid)"

        # Try to get connection info
        if redis-cli PING > /dev/null 2>&1; then
            echo "  - Connection: OK"
            echo "  - Port: 6379"

            # Get memory info
            local memory=$(redis-cli info memory | grep "used_memory_human" | cut -d: -f2)
            echo "  - Memory used: $memory"

            # Get keys count
            local keys=$(redis-cli dbsize)
            echo "  - Keys in DB: $keys"
        else
            echo "  - Connection: Failed"
        fi
    else
        echo "✗ Redis is not running"
    fi
}

# Function to restart Redis
restart_redis() {
    echo "Restarting Redis server..."

    if is_redis_running; then
        stop_redis
    fi

    start_redis
}

# Function to show usage
show_usage() {
    echo "Redis Manager Script"
    echo "Usage: $0 {start|stop|restart|status|flush|help}"
    echo ""
    echo "Commands:"
    echo "  start   - Start Redis server"
    echo "  stop    - Stop Redis server"
    echo "  restart - Restart Redis server"
    echo "  status  - Show Redis status"
    echo "  flush   - Flush all Redis data"
    echo "  help    - Show this help message"
    echo ""
}

# Main script logic
case "$1" in
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
    flush)
        flush_redis
        ;;
    help|--help|-h)
        show_usage
        ;;
    *)
        echo "Error: Unknown command '$1'"
        echo ""
        show_usage
        exit 1
        ;;
esac

exit 0