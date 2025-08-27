#!/bin/bash

MINISHELL_PATH=./minishell
VALGRIND="valgrind --leak-check=full --track-fds=yes --show-leak-kinds=all --suppressions=readline.supp"
TEST_FILE="fd_leak_results.txt"

BUILTINS=("env" "unset FOO" "pwd" "export FOO=123" "exit" "echo selam" "cd .")

rm -f "$TEST_FILE"
touch "$TEST_FILE"

echo "=== FD Leak and Memory Leak Test ===" >> "$TEST_FILE"

for builtin in "${BUILTINS[@]}"; do
    echo ">>> Testing: $builtin" | tee -a "$TEST_FILE"

    # Komutu heredoc içerikleriyle birlikte besle
    $VALGRIND $MINISHELL_PATH 2>> "$TEST_FILE" <<EOF
cat << t1 << t2 > samet.txt | $builtin | cat << t3 > samet2.txt
hello from t1
t1
hello from t2
t2
hello from t3
t3
EOF

    echo -e "\n=============================\n" >> "$TEST_FILE"
done

echo "🧪 Test complete. Results saved to $TEST_FILE"