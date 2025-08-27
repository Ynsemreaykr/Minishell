#!/bin/bash

MINISHELL=./minishell
VALGRIND="valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes --suppressions=readline.supp"
OUTFILE="fd_tester_output.txt"

echo -e "\e[1;32m🔥 FD TESTER STARTED 🔥\e[0m"
echo "==== FD TEST RESULTS ====" > "$OUTFILE"

function run_case() {
	echo -e "\n\e[1;36m[CASE]: $1\e[0m"
	echo -e "\n==== TEST CASE ====\n$1" >> "$OUTFILE"
	echo "$1" | $VALGRIND $MINISHELL >> "$OUTFILE" 2>&1
}

### ✔ Basit redir testleri
run_case "echo test > test_out.txt"
run_case "cat < test_out.txt"
run_case "echo test >> test_out.txt"
run_case "cat < test_out.txt | grep test"

### ✔ Pipe zinciri
run_case "ls | cat | cat | cat | grep .c"
run_case "echo hello | tr a-z A-Z | tr A-Z a-z | cat"

### ✔ File not found redir
run_case "cat < nonexistent_file.txt"
run_case "echo test > /dev/null && cat < notexist | cat > yep.txt"

### ✔ Pipe + heredoc
run_case "cat << EOF | grep test
test line
EOF"

### ✔ Boş komut
run_case ""

### ✔ Çoklu heredoc
run_case "cat << A | cat << B
bir
A
iki
B"

### ✔ Çoklu pipe + redir + heredoc
run_case "cat << E | grep test | cat > temp.txt
test line
E"

### ✔ Yanlış komutlar
run_case "cat <"
run_case "echo >"
run_case ">"
run_case "|"
run_case "<<"

### ✔ Subtle dup testi
run_case "echo test > test.txt && cat < test.txt | cat | cat"
run_case "ls | grep .sh > file.txt"

### ✔ Kısa heredoc zinciri
run_case "cat << EOF | cat | cat
line
EOF"

### ✔ Uydurma binary
run_case "./fake_binary"
run_case "/bin/ls | ./nonexistent"

### ✔ fd leak olabilecek: exec hata durumları
run_case "/bin/ls | /bin/fake"
run_case "echo ok | /bin/notexist | cat"

### ✔ execve fail ama pipe açılmış olur
run_case "ls | ./404 | cat"

echo -e "\n\e[1;32m🎯 TÜM TESTLER TAMAMLANDI — ÇIKTI: fd_tester_output.txt\e[0m"