#!/bin/bash

PORT=6668
PASS=pass
SERVER=./ircserv

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

PASS_COUNT=0
FAIL_COUNT=0

ok()   { echo -e "${GREEN}[PASS]${NC} $1"; PASS_COUNT=$((PASS_COUNT + 1)); }
fail() { echo -e "${RED}[FAIL]${NC} $1"; FAIL_COUNT=$((FAIL_COUNT + 1)); }
info() { echo -e "${YELLOW}[INFO]${NC} $1"; }

start_server() {
	$SERVER $PORT $PASS > /tmp/irc_server.log 2>&1 &
	SERVER_PID=$!
	sleep 0.3
	if ! kill -0 $SERVER_PID 2>/dev/null; then
		echo -e "${RED}[ERROR]${NC} Server failed to start"
		exit 1
	fi
	info "Server started (PID: $SERVER_PID)"
}

stop_server() {
	kill -INT $SERVER_PID 2>/dev/null
	wait $SERVER_PID 2>/dev/null
}

# printf 後に sleep して stdin を保持し、サーバーの応答を受け取る
irc() {
	local cmds="$1"
	local hold="${2:-1}"
	local outfile="$3"
	{ printf "%b" "$cmds"; sleep "$hold"; } | timeout $((hold + 2)) nc 127.0.0.1 $PORT > "$outfile" 2>/dev/null
}

echo "=== errno fix tests ==="
echo ""

# -------------------------------------------------------
# Test 1: recv path — サーバーが正しくデータを受信・処理できるか
# -------------------------------------------------------
info "Test 1: recv path - 正常登録フロー (001 Welcome)"
start_server
irc "PASS $PASS\r\nNICK alice\r\nUSER alice 0 * :Alice\r\n" 1 /tmp/irc_out.txt
if grep -q "001" /tmp/irc_out.txt; then
	ok "recv: Welcome (001) を受信"
else
	fail "recv: 001 が返らなかった"
	cat /tmp/irc_out.txt
fi
stop_server

# -------------------------------------------------------
# Test 2: send path — サーバーが正しくデータを送信できるか
# -------------------------------------------------------
info "Test 2: send path - PRIVMSG が別クライアントに届く"
start_server

irc "PASS $PASS\r\nNICK bob\r\nUSER bob 0 * :Bob\r\nJOIN #test\r\n" 2 /tmp/irc_bob.txt &
BOB_PID=$!
sleep 0.5

irc "PASS $PASS\r\nNICK carol\r\nUSER carol 0 * :Carol\r\nJOIN #test\r\nPRIVMSG #test :hello\r\n" 1 /tmp/irc_carol.txt
sleep 0.5

wait $BOB_PID 2>/dev/null
if grep -q "hello" /tmp/irc_bob.txt; then
	ok "send: bob が carol の PRIVMSG を受信"
else
	fail "send: PRIVMSG が届かなかった"
fi
stop_server

# -------------------------------------------------------
# Test 3: accept path — 複数クライアントの同時接続
# -------------------------------------------------------
info "Test 3: accept path - 3クライアント同時接続"
start_server

irc "PASS $PASS\r\nNICK u1\r\nUSER u1 0 * :U1\r\n" 1 /tmp/irc_u1.txt &
P1=$!
irc "PASS $PASS\r\nNICK u2\r\nUSER u2 0 * :U2\r\n" 1 /tmp/irc_u2.txt &
P2=$!
irc "PASS $PASS\r\nNICK u3\r\nUSER u3 0 * :U3\r\n" 1 /tmp/irc_u3.txt &
P3=$!
wait $P1 $P2 $P3

ALL_OK=true
for f in /tmp/irc_u1.txt /tmp/irc_u2.txt /tmp/irc_u3.txt; do
	if ! grep -q "001" "$f"; then
		ALL_OK=false
	fi
done

if $ALL_OK; then
	ok "accept: 3クライアント全員が 001 を受信"
else
	fail "accept: 一部クライアントの接続に失敗"
fi
stop_server

# -------------------------------------------------------
# Test 4: poll path — SIGINT でのグレースフルシャットダウン
# -------------------------------------------------------
info "Test 4: poll path - SIGINT グレースフルシャットダウン"
start_server

irc "PASS $PASS\r\nNICK stay\r\nUSER stay 0 * :Stay\r\nJOIN #ch\r\n" 5 /tmp/irc_stay.txt &
NC_PID=$!
sleep 0.5

kill -INT $SERVER_PID
wait $SERVER_PID 2>/dev/null
EXIT_CODE=$?

if [ $EXIT_CODE -eq 0 ]; then
	ok "poll: SIGINT 後に exit code 0 で正常終了"
else
	fail "poll: 終了コードが $EXIT_CODE (期待値 0)"
fi
wait $NC_PID 2>/dev/null

# -------------------------------------------------------
# Test 5: 再起動後も正常動作するか
# -------------------------------------------------------
info "Test 5: サーバー再起動後の動作"
start_server
irc "PASS $PASS\r\nNICK dave\r\nUSER dave 0 * :Dave\r\n" 1 /tmp/irc_dave.txt
if grep -q "001" /tmp/irc_dave.txt; then
	ok "再起動後も正常に接続・応答"
else
	fail "再起動後の接続に失敗"
fi
stop_server

# -------------------------------------------------------
echo ""
echo "=== Results: ${PASS_COUNT} passed, ${FAIL_COUNT} failed ==="
[ $FAIL_COUNT -eq 0 ] && exit 0 || exit 1
