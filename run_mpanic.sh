#!/usr/bin/env bash
set -euo pipefail

MINISHELL=./minishell
VALGRIND_CMD="valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --track-fds=yes --suppressions=minishell.supp --log-file=log.txt"
OUTFILE="fd_tester_output.txt"

# leak satırı yakalama paterni
LEAK_PAT='LEAK SUMMARY|definitely lost|indirectly lost|possibly lost|still reachable|Invalid (read|write)|Mismatched free'

# Başlangıç
: > "$OUTFILE"
# Kümülatif log için staging dosyası (valgrind her seferinde log.txt'yi yazar, biz biriktiririz)
: > log_accum.txt
# İlk görünüm: boş kümülatif log, en başta da log.txt'yi temizleyelim
: > log.txt

echo -e "\e[1;32m🔥 FD TESTER STARTED 🔥\e[0m"
{
  echo "==== FD TEST RESULTS ===="
  echo "MINISHELL: $MINISHELL"
  echo "VALGRIND : $VALGRIND_CMD"
} >> "$OUTFILE"

case_id=0

# ANSI/CR temizle (ekranda silinme/üstüne yazma olmasın)
strip_ansi_cr() {
  sed -r 's/\x1B\[[0-9;]*[A-Za-z]//g' | tr '\r' '\n'
}

# Leak satırlarını komut etiketiyle işaretleyip ekle
annotate_and_append_leaks() {
  local cmd="$1"
  local cmd_oneline
  # Çok satırlı komutu tek satıra indir, boşlukları sadeleştir
  cmd_oneline="$(echo "$cmd" | tr '\n' ' ' | sed -E 's/[[:space:]]+/ /g')"

  # log.txt içeriğini al
  local tmp="log_case.tmp"
  cp log.txt "$tmp" 2>/dev/null || :  

  {
    echo "===== CASE #$case_id ====="
    echo "CMD: $cmd_oneline"
    echo "--- RAW VALGRIND LOG BEGIN ---"
    cat "$tmp"
    echo "--- RAW VALGRIND LOG END ---"
    echo "--- LEAK TAGGED LINES ---"
    # awk ile yalnızca leak ile ilgili satırların başına prefix koy
    awk -v pat="$LEAK_PAT" -v tag="$cmd_oneline" '
      BEGIN{
        IGNORECASE=1
      }
      {
        line=$0
        if (line ~ pat) {
          print "[LEAK - " tag "] " line
        }
      }
    ' "$tmp"
    echo
  } >> log_accum.txt

  # Kümülatifi asıl log.txt olarak güncelle (tek dosya istedin)
  cp log_accum.txt log.txt
  rm -f "$tmp"
}

run_case() {
  local CMD="$1"
  case_id=$((case_id+1))

  echo -e "\n\e[1;36m[CASE #$case_id]: $CMD\e[0m"
  {
    echo
    echo "==== TEST CASE #$case_id ===="
    echo "$CMD"
  } >> "$OUTFILE"

  # Çalıştırmadan önce log.txt'yi sıfırla (valgrind bu dosyayı yeniden yazar)
  : > log.txt

  # minishell'i valgrind ile ÇALIŞTIR (tam senin istediğin komut)
  # stdout/stderr'i OUTFILE'a ekle; CR/ANSI temizliği uygula
  printf "%s\n" "$CMD" \
    | env TERM=dumb $VALGRIND_CMD "$MINISHELL" \
    > >(strip_ansi_cr >>"$OUTFILE") \
    2> >(strip_ansi_cr >>"$OUTFILE")

  # valgrind logunu işle ve kümülatife ekle
  annotate_and_append_leaks "$CMD"

  # Basit görünür uyarı
  if grep -qiE "$LEAK_PAT" log.txt; then
    echo -e "\e[1;31m[LEAK DETECTED]\e[0m"
  else
    echo -e "\e[1;32m[NO LEAK LINE MATCHED]\e[0m"
  fi
}

########## TESTLER ##########

### ✔ Basit redir testleri
run_case "echo test > test_out.txt"
run_case "cat < test_out.txt"
run_case "echo test >> test_out.txt"
run_case "cat < test_out.txt | grep test"

### ✔ Pipe zinciri
run_case "ls | cat | cat | cat | grep .c || true"
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
run_case 'cat <'
run_case 'echo >'
run_case '>'
run_case '|'
run_case '<<'

### ✔ Subtle dup testi
run_case "echo test > test.txt && cat < test.txt | cat | cat"
run_case "ls | grep .sh > file.txt"

### ✔ Kısa heredoc zinciri
run_case "cat << EOF | cat | cat
line
EOF"

### ✔ Uydurma binary
run_case "./fake_binary"
run_case "/bin/ls | ./nonexistent || true"

### ✔ fd leak olabilecek: exec hata durumları
run_case "/bin/ls | /bin/fake || true"
run_case "echo ok | /bin/notexist | cat || true"

### ✔ execve fail ama pipe açılmış olur
run_case "ls | ./404 | cat || true"

echo -e "\n\e[1;32m🎯 TÜM TESTLER TAMAMLANDI — ÇIKTI: $OUTFILE | LEAK LOG: log.txt\e[0m"
