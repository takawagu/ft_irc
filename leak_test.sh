#!/bin/bash

PORT=6667
PASS=pass
SERVER=./ircserv
WAIT=0.3

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

run_test() {
	local name="$1"
	local cmds="$2"
	echo -e "${YELLOW}[TEST]${NC} $name"
	printf "%b" "$cmds" | timeout 2 nc -C 127.0.0.1 $PORT > /dev/null 2>&1
	sleep $WAIT
}

# サーバー起動
$SERVER $PORT $PASS &
SERVER_PID=$!
sleep 0.5

if ! kill -0 $SERVER_PID 2>/dev/null; then
	echo -e "${RED}[ERROR]${NC} Server failed to start"
	exit 1
fi
echo -e "${GREEN}[INFO]${NC} Server started (PID: $SERVER_PID)"

# テストケース実行
run_test "登録前に切断" \
	"\r\n"

run_test "PASSのみ送って切断" \
	"PASS $PASS\r\n"

run_test "NICK まで送って切断" \
	"PASS $PASS\r\nNICK test\r\n"

run_test "正常登録後に切断" \
	"PASS $PASS\r\nNICK test\r\nUSER test 0 * :Test User\r\n"

run_test "チャンネル参加後に切断" \
	"PASS $PASS\r\nNICK test\r\nUSER test 0 * :Test User\r\nJOIN #ch1\r\nJOIN #ch2\r\nJOIN #ch3\r\n"

run_test "不完全なコマンドで切断" \
	"PASS $PASS\r\nNICK test\r\nUSER test 0 * :Test User\r\nJOI\r\n"

echo -e "${YELLOW}[TEST]${NC} 複数クライアント同時接続・切断"
printf "PASS $PASS\r\nNICK user1\r\nUSER user1 0 * :User1\r\n" | timeout 2 nc -C 127.0.0.1 $PORT > /dev/null 2>&1 &
PID1=$!
printf "PASS $PASS\r\nNICK user2\r\nUSER user2 0 * :User2\r\n" | timeout 2 nc -C 127.0.0.1 $PORT > /dev/null 2>&1 &
PID2=$!
printf "PASS $PASS\r\nNICK user3\r\nUSER user3 0 * :User3\r\n" | timeout 2 nc -C 127.0.0.1 $PORT > /dev/null 2>&1 &
PID3=$!
wait $PID1 $PID2 $PID3
sleep $WAIT

echo -e "${YELLOW}[TEST]${NC} アクティブなクライアントがいる状態でサーバー停止前"
printf "PASS $PASS\r\nNICK test\r\nUSER test 0 * :Test User\r\nJOIN #ch1\r\n" | timeout 5 nc -C 127.0.0.1 $PORT > /dev/null 2>&1 &
LAST_NC_PID=$!
sleep $WAIT

# leaks チェック（サーバー起動中）
echo ""
echo -e "${GREEN}[INFO]${NC} Running leaks check on running server..."
leaks $SERVER_PID 2>&1 | tail -20

# サーバー停止
kill -INT $SERVER_PID
wait $SERVER_PID 2>/dev/null
wait $LAST_NC_PID 2>/dev/null

echo ""
echo -e "${GREEN}[INFO]${NC} All tests completed."
