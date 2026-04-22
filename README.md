# 🐚 Minishell

> Ecole 42 — Minishell Projesi  
> Basit ama işlevsel bir Unix kabuğu (shell) uygulaması.

---

## 📋 İçindekiler

- [Proje Hakkında](#-proje-hakkında)
- [Özellikler](#-özellikler)
- [Nasıl Derlenir ve Çalıştırılır?](#-nasıl-derlenir-ve-çalıştırılır)
- [Kullanım Örnekleri](#-kullanım-örnekleri)
- [Kullanılan Yapılar](#-kullanılan-yapılar)
- [Temel Kavramlar](#-temel-kavramlar)
  - [Tokenizer (Sözcük Ayrıştırıcı)](#tokenizer-sözcük-ayrıştırıcı)
  - [Parser (Ayrıştırıcı)](#parser-ayrıştırıcı)
  - [Yönlendirmeler (Redirections)](#yönlendirmeler-redirections)
  - [Heredoc](#heredoc)
  - [Pipe (Boru Hattı)](#pipe-boru-hattı)
  - [Fork / Child / Parent](#fork--child--parent)
  - [Execute (Komut Çalıştırma)](#execute-komut-çalıştırma)
  - [Sinyaller](#sinyaller)
- [Dahili Komutlar (Builtins)](#-dahili-komutlar-builtins)
- [Proje Klasör Yapısı](#-proje-klasör-yapısı)

---

## 🎯 Proje Hakkında

**Minishell**, Ecole 42'nin sistem programlama müfredatındaki önemli projelerinden biridir. Bu projede Bash gibi gerçek bir Unix kabuğunun (shell) minyatür versiyonu C dilinde sıfırdan yazılır.

Proje; giriş okuma, tokenize etme, yönlendirme (redirection), pipe, heredoc, ortam değişkeni genişletme ve process yönetimi gibi temel Unix/Linux sistem programlama kavramlarının uygulamalı olarak öğrenilmesini sağlar.

---

## ✨ Özellikler

| Özellik | Açıklama |
|---|---|
| Komut satırı okuma | `readline` ile geçmiş (history) desteği |
| Pipe (`\|`) | Komutları birbirine zincirleme |
| Yönlendirme | `<`, `>`, `>>` ve `<<` (heredoc) |
| Ortam değişkenleri | `$VAR`, `$?` genişletme desteği |
| Tırnak işleme | Tek (`'`) ve çift (`"`) tırnak desteği |
| Sinyal yönetimi | `Ctrl+C`, `Ctrl+D`, `Ctrl+\` doğru işlenir |
| Dahili komutlar | `echo`, `cd`, `pwd`, `export`, `unset`, `env`, `exit` |
| Bellek yönetimi | Özel allocator ile bellek sızıntısı önlenir |

---

## 🚀 Nasıl Derlenir ve Çalıştırılır?

### Gereksinimler

- **İşletim Sistemi:** Linux veya macOS
- **Derleyici:** `cc` (GCC veya Clang)
- **Kütüphane:** `readline` (geliştirici paketleri gerekli)

#### Ubuntu / Debian için readline kurulumu:
```bash
sudo apt-get install libreadline-dev
```

#### macOS için readline kurulumu:
```bash
brew install readline
```

### Derleme

Proje dizinine gidin ve aşağıdaki komutu çalıştırın:

```bash
# Projeyi derle
make

# Nesne dosyalarını temizle
make clean

# Her şeyi temizle (nesne + binary)
make fclean

# Temizleyip yeniden derle
make re
```

Başarılı derlemenin ardından `minishell` adlı çalıştırılabilir dosya oluşacaktır.

### Çalıştırma

```bash
./minishell
```

Shell başladığında `minishell$` prompt'u görüntülenir:

```
minishell$ _
```

---

## 💡 Kullanım Örnekleri

```bash
# Dosya listeleme
minishell$ ls -la

# Ortam değişkeni genişletme
minishell$ echo $HOME
/home/kullanici

# Son komutun çıkış kodu
minishell$ echo $?
0

# Pipe zinciri
minishell$ ls | grep ".c" | wc -l

# Giriş yönlendirmesi
minishell$ wc -l < dosya.txt

# Çıkış yönlendirmesi (üzerine yazar)
minishell$ echo "merhaba" > cikti.txt

# Çıkış yönlendirmesi (sona ekler)
minishell$ echo "dünya" >> cikti.txt

# Heredoc
minishell$ cat << EOF
> Birinci satır
> İkinci satır
> EOF
Birinci satır
İkinci satır

# export / unset
minishell$ export DEGISKEN=deger
minishell$ echo $DEGISKEN
deger
minishell$ unset DEGISKEN

# Çift tırnak — değişken genişletilir
minishell$ echo "Ev dizini: $HOME"

# Tek tırnak — değişken genişletilmez
minishell$ echo 'Değer: $HOME'
Değer: $HOME

# Shell'den çıkış
minishell$ exit
```

---

## 🏗️ Kullanılan Yapılar

Projenin tüm veri modeli `include/minishell.h` dosyasında tanımlanmıştır.

### `t_redir_type` — Yönlendirme Türleri (enum)

```c
typedef enum e_redir_type
{
    REDIR_IN,      // < giriş yönlendirmesi
    REDIR_OUT,     // > çıkış yönlendirmesi (üzerine yazar)
    REDIR_APPEND,  // >> çıkış yönlendirmesi (sona ekler)
    HEREDOC        // << satır satır giriş
} t_redir_type;
```

### `t_redir` — Yönlendirme Düğümü

```c
typedef struct s_redir
{
    t_redir_type  type;               // Yönlendirme türü
    char          *filename;          // Hedef dosya adı
    char          *delimiter;         // Heredoc sınır kelimesi (ham)
    char          *cleaned_delimiter; // Heredoc sınır kelimesi (tırnak temizlenmiş)
    char          *content;           // Heredoc'tan okunan içerik
    int           quoted_flag;        // Sınır kelime tırnak içinde mi?
    struct s_redir *next;             // Sonraki yönlendirme (bağlı liste)
} t_redir;
```

### `t_cmd` — Komut Düğümü

```c
typedef struct s_cmd
{
    char         **argv;   // Komut ve argümanlar (NULL-sonlandırmalı dizi)
    t_redir      *redirs;  // Bu komuta ait yönlendirme listesi
    struct s_cmd *next;    // Sonraki komut (pipe ile bağlı)
} t_cmd;
```

### `t_shell` — Kabuk Durumu

```c
typedef struct s_shell
{
    t_cmd  *cmds;      // Komut listesinin başı
    int    last_exit;  // Son çalıştırılan komutun çıkış kodu ($?)
    char   **env;      // Ortam değişkenleri dizisi
} t_shell;
```

### `t_proc_ctx` — İşlem Bağlamı

```c
typedef struct s_proc_ctx
{
    const char *input;     // Ham giriş dizisi
    int        *i;         // Mevcut konum
    int        end;        // Bitiş konumu
    char       *processed; // İşlenmiş çıktı tamponu
    int        *proc_len;  // Çıktının mevcut uzunluğu
    t_shell    *shell;     // Kabuk durumuna referans
} t_proc_ctx;
```

### `t_mem_block` — Bellek Yönetimi

```c
typedef struct s_mem_block
{
    void            *ptr;  // Ayrılan bellek bloğu
    struct s_mem_block *next; // Sonraki blok (bağlı liste)
} t_mem_block;
```

---

## 📚 Temel Kavramlar

### Tokenizer (Sözcük Ayrıştırıcı)

**İlgili dosyalar:** `src/tokenizer.c`, `src/tokenizer_utils.c`, `src/token_processing.c`, `src/token_heredoc.c`

Tokenizer, kullanıcıdan alınan ham giriş dizisini anlamlı parçalara (token) böler.

```
"ls -la | grep .c > out.txt"
     ↓ Tokenizer
["ls", "-la", "|", "grep", ".c", ">", "out.txt"]
```

**Nasıl çalışır:**
1. Giriş dizisi karakter karakter okunur.
2. Boşluklar, tırnak işaretleri ve özel karakterler (`|`, `<`, `>`) tespit edilir.
3. Tırnaklar arasındaki içerik bütün olarak işlenir.
4. Her token bir diziye yerleştirilir.

---

### Parser (Ayrıştırıcı)

**İlgili dosyalar:** `src/parse_command.c`, `src/parse_heredoc.c`, `src/pipe_parser.c`, `src/cmd_loop.c`

Parser, tokenizer'dan gelen token dizisini `t_cmd` bağlı listesine dönüştürür.

```
Token dizisi → [cmd1 | cmd2 | cmd3] (bağlı liste)
                  ↓         ↓
               redirs     redirs
```

**Aşamalar:**
1. `|` karakterleri bulunarak pipe sayısı hesaplanır.
2. Her pipe segmenti için bir `t_cmd` düğümü oluşturulur.
3. Her komutun yönlendirmeleri (`<`, `>`, `>>`, `<<`) `t_redir` listesine eklenir.
4. Ortam değişkenleri (`$VAR`) bu aşamada genişletilir.

---

### Yönlendirmeler (Redirections)

**İlgili dosyalar:** `src/create_redir.c`, `src/create_redir_process.c`, `src/apply_redir_child.c`, `src/apply_redir_builtin.c`, `src/setup_built_redir.c`

#### `<` — Giriş Yönlendirmesi (REDIR_IN)

Komutun standart girdisini (`stdin`) bir dosyadan okur.

```bash
cat < dosya.txt
# dosya.txt'nin içeriği cat komutuna stdin olarak verilir
```

**Çalışma mekanizması:**
```c
// apply_redir_child.c içinde
void apply_in_redir_child(t_redir *r, int *input_redirected)
{
    int fd = open(r->filename, O_RDONLY);
    dup2(fd, STDIN_FILENO);  // stdin'i dosyaya yönlendir
    close(fd);
    *input_redirected = 1;
}
```

#### `>` — Çıkış Yönlendirmesi (REDIR_OUT)

Komutun standart çıktısını (`stdout`) bir dosyaya yazar. Dosya mevcutsa sıfırdan başlar.

```bash
echo "merhaba" > dosya.txt
```

**Çalışma mekanizması:**
```c
// O_WRONLY | O_CREAT | O_TRUNC bayraklarıyla dosya açılır
// stdout -> dosya fd'si olarak dup2() ile yönlendirilir
```

#### `>>` — Ekleme Yönlendirmesi (REDIR_APPEND)

Çıktıyı dosyanın sonuna ekler, mevcut içeriği silmez.

```bash
echo "yeni satır" >> dosya.txt
```

**Çalışma mekanizması:**
```c
// O_WRONLY | O_CREAT | O_APPEND bayraklarıyla dosya açılır
```

---

### Heredoc

**İlgili dosyalar:** `src/heredoc.c`, `src/heredoc_child.c`, `src/heredoc_expand.c`, `src/heredoc_quotes.c`, `src/parse_heredoc.c`, `src/expand_heredoc_size.c`, `src/token_heredoc.c`

Heredoc (`<<`), birden fazla satırlı girdi sağlamak için kullanılır. Belirtilen sınır kelimesi (delimiter) görülene kadar satır satır okunur.

```bash
cat << EOF
Merhaba
Dünya
EOF
```

**Çalışma adımları:**

```
1. << operatörü tespit edilip delimiter okunur (örn. EOF)
2. Fork ile yeni bir child process oluşturulur
3. Child process, pipe'ın yazma ucuna satır satır yazar
4. Parent process, pipe'ın okuma ucundan heredoc içeriğini okur
5. İçerik t_redir->content alanında saklanır
6. Komut çalıştırılırken stdin bu içerikle beslenir
```

**Tırnaklı delimiter:** Delimiter tırnak içindeyse (`'EOF'` veya `"EOF"`), ortam değişkeni genişletmesi yapılmaz:

```bash
cat << 'EOF'
$HOME genişletilmez
EOF
```

**Sinyal yönetimi:** Heredoc okuma sırasında `Ctrl+C` basıldığında child process sonlandırılır ve işlem iptal edilir.

---

### Pipe (Boru Hattı)

**İlgili dosyalar:** `src/exec_pipeline.c`, `src/pipe_parser.c`, `src/pipe_check.c`

Pipe (`|`) operatörü, bir komutun çıktısını (`stdout`) bir sonraki komutun girdisine (`stdin`) bağlar.

```bash
ls -la | grep ".c" | wc -l
```

**Çalışma akışı:**

```
[ls -la] → pipe1 → [grep ".c"] → pipe2 → [wc -l]
  stdout      fd[1]  stdin  stdout    fd[1]  stdin
              fd[0]                   fd[0]
```

**Uygulama detayı:**
```c
// Her ardışık komut çifti için bir pipe oluşturulur
pipe(pipefd);          // pipefd[0] = okuma, pipefd[1] = yazma
fork();                // Her komut için child process
dup2(pipefd[1], STDOUT_FILENO); // child: stdout -> pipe yazma ucu
dup2(pipefd[0], STDIN_FILENO);  // sonraki child: stdin <- pipe okuma ucu
```

---

### Fork / Child / Parent

**İlgili dosyalar:** `src/child_process.c`, `src/exec_pipeline.c`, `src/heredoc.c`

Unix'te bir programın kendini kopyalamasının tek yolu `fork()` sistem çağrısıdır.

```
                  fork()
                /        \
        Parent Process   Child Process
        (PID > 0)        (PID == 0)
        waitpid()        execve() / builtin
```

**Parent process:**
- `fork()` çağrısı yapar ve child'ın PID değerini alır.
- Pipe'ın kendi tarafındaki fazla dosya tanımlayıcılarını kapatır.
- `waitpid()` ile child'ın bitmesini bekler.
- Child'ın çıkış kodunu `$?` için saklar.

**Child process:**
- `fork()` sonrası PID = 0 döner.
- Yönlendirmeleri (`dup2`) uygular.
- Dahili komutsa `exec_builtin()`, harici komutsa `execve()` çağırır.
- `execve()` başarısız olursa `exit(127)` ile çıkar.

**Bellek temizliği:**  
Child process, `execve()` öncesinde shell'in kendi bellek alanını temizler (`cleanup_shell_for_child`), böylece bellek sızıntısı önlenir.

---

### Execute (Komut Çalıştırma)

**İlgili dosyalar:** `src/executor.c`, `src/execute_command.c`, `src/command_executors.c`, `src/command_handlers.c`, `src/exec_builtin.c`

```
execute_command_main()
        │
        ├── Tek komut mu?
        │       ├── Dahili komut → execute_single_builtin()  (fork yok!)
        │       └── Harici komut → execute_single_external() (fork + execve)
        │
        └── Pipe var mı? → exec_pipeline()
                                │
                                └── Her komut için fork()
                                        ├── Child: handle_child()
                                        └── Parent: handle_parent()
```

**PATH arama:** Harici komutlar için `$PATH` ortam değişkeni `:` ile split edilerek komut aranır:

```c
// parse_path.c ve execute_command.c
// PATH=/usr/bin:/bin:/usr/local/bin
// "ls" → /usr/bin/ls, /bin/ls ... sırayla denenir
```

---

### Sinyaller

**İlgili dosyalar:** `src/signals.c`, `src/signal_utils.c`

| Sinyal | Normal mod | Heredoc modu | Komut çalışırken |
|---|---|---|---|
| `Ctrl+C` (SIGINT) | Yeni prompt göster | Okumayı iptal et | Komutları durdur |
| `Ctrl+D` (EOF) | Shell'den çık | — | — |
| `Ctrl+\` (SIGQUIT) | Yoksay | Yoksay | Core dump (varsayılan) |

Global değişken `g_signal_number` (`sig_atomic_t` türünde), sinyal numarasını thread-safe şekilde depolar ve çıkış koduna (`$?`) yansıtılır.

---

## 🔧 Dahili Komutlar (Builtins)

**İlgili dosyalar:** `src/builtin.c`, `src/builtin_utils.c`, `src/exec_builtin.c`, `src/ft_export.c`, `src/ft_export_two.c`, `src/ft_unset.c`, `src/sort_env.c`, `src/env.c`, `src/env_utils.c`

| Komut | Açıklama | Dosya |
|---|---|---|
| `echo [-n]` | Metni ekrana yazdırır | `src/builtin.c` |
| `cd [dizin]` | Çalışma dizinini değiştirir | `src/builtin.c` |
| `pwd` | Geçerli dizin yolunu yazdırır | `src/builtin.c` |
| `export [VAR=val]` | Ortam değişkeni tanımlar/günceller | `src/ft_export.c` |
| `unset [VAR]` | Ortam değişkenini siler | `src/ft_unset.c` |
| `env` | Tüm ortam değişkenlerini listeler | `src/env.c` |
| `exit [n]` | Shell'den çıkar | `src/builtin.c` |

> **Not:** Dahili komutlar pipe olmayan tek komut olarak çalıştırıldığında **fork yapılmaz**; doğrudan mevcut process içinde çalıştırılır. Bu sayede `cd`, `export` gibi komutların shell durumunu değiştirmesi mümkün olur.

---

## 📁 Proje Klasör Yapısı

```
Minishell-main/
│
├── Makefile                        ← Derleme kuralları (all, clean, fclean, re)
│
├── include/
│   └── minishell.h                 ← Tüm yapılar, enum'lar ve fonksiyon prototipleri
│
├── LIBFT/                          ← Projeye özel yardımcı C kütüphanesi
│   ├── ft_atoi.c                   ← String'i integer'a çevirir
│   ├── ft_isalnum.c                ← Alfanümerik karakter kontrolü
│   ├── ft_isalpha.c                ← Alfabetik karakter kontrolü
│   ├── ft_itoa.c                   ← Integer'ı string'e çevirir
│   ├── ft_memset.c                 ← Belleği belirli bir değerle doldurur
│   ├── ft_putstr_fd.c              ← Belirli fd'ye string yazar
│   ├── ft_split.c                  ← String'i ayırıcıya göre böler
│   ├── ft_strcat.c                 ← String birleştirme
│   ├── ft_strchr.c                 ← String içinde karakter arama
│   ├── ft_strcmp.c                 ← String karşılaştırma
│   ├── ft_strcpy.c                 ← String kopyalama
│   ├── ft_strdup.c                 ← String kopyasını heap'te oluşturur
│   ├── ft_strjoin.c                ← İki string'i birleştirip yeni string döner
│   ├── ft_strlen.c                 ← String uzunluğunu hesaplar
│   ├── ft_strncmp.c                ← N karakter kadar string karşılaştırma
│   └── ft_strncpy.c                ← N karakter kadar string kopyalama
│
└── src/                            ← Projenin tüm kaynak dosyaları
    │
    │   ── Giriş Noktası ──
    ├── main.c                      ← Shell başlatma, ana döngü, giriş okuma
    ├── main_utils.c                ← Ana döngü yardımcı fonksiyonları
    ├── main_input_check.c          ← Ham giriş doğrulama işlemleri
    ├── input_check_utils.c         ← Giriş doğrulama yardımcıları (tırnak, pipe)
    │
    │   ── Tokenizer ──
    ├── tokenizer.c                 ← Ham girişi token'lara böler
    ├── tokenizer_utils.c           ← Token bölme yardımcı fonksiyonları
    ├── token_processing.c          ← Token içerik işleme (tırnak, genişletme)
    ├── token_heredoc.c             ← Heredoc token'larının işlenmesi
    │
    │   ── Parser ──
    ├── parse_command.c             ← Token listesini t_cmd bağlı listesine dönüştürür
    ├── parse_heredoc.c             ← Heredoc direktiflerini parse eder
    ├── parse_path.c                ← PATH değişkenini parse eder, komut arar
    ├── pipe_parser.c               ← Pipe operatörüne göre komut segmentlerini ayırır
    ├── pipe_check.c                ← Pipe sözdizimi doğrulama
    ├── cmd_loop.c                  ← Komut listesi oluşturma döngüsü
    │
    │   ── Token Sayma / Boyut Hesaplama ──
    ├── count_token_check.c         ← Token sayma (özel durumlar)
    ├── count_token_utils.c         ← Token sayma yardımcıları
    ├── calculate_required.c        ← Genişletilmiş string için bellek hesaplama
    │
    │   ── Ortam Değişkeni Genişletme ──
    ├── expand_env_content.c        ← $VAR genişletme içeriği oluşturur
    ├── expand_heredoc_size.c       ← Heredoc içindeki genişletme boyutu
    ├── heredoc_expand.c            ← Heredoc satırlarında değişken genişletme
    ├── process_double_quote_utils.c← Çift tırnak içi genişletme işlemleri
    ├── process_expansion_outside_quotes.c ← Tırnak dışı genişletme
    │
    │   ── Yönlendirme (Redirection) ──
    ├── create_redir.c              ← t_redir düğümü oluşturur, listeye ekler
    ├── create_redir_process.c      ← Yönlendirme token'larını işler
    ├── apply_redir_child.c         ← Child process'te dup2 ile yönlendirme uygular
    ├── apply_redir_builtin.c       ← Dahili komutlarda yönlendirme uygular
    ├── setup_built_redir.c         ← Dahili komutlar için yönlendirme hazırlığı
    │
    │   ── Heredoc ──
    ├── heredoc.c                   ← Heredoc ana akışı (fork, pipe, okuma)
    ├── heredoc_child.c             ← Child process heredoc yazma döngüsü
    ├── heredoc_quotes.c            ← Delimiter'daki tırnak tespiti ve temizleme
    │
    │   ── Executor (Çalıştırıcı) ──
    ├── executor.c                  ← Child için yönlendirmeleri uygular
    ├── execute_command.c           ← execve ile harici komut çalıştırır
    ├── exec_pipeline.c             ← Pipe hattını fork/waitpid ile yürütür
    ├── exec_builtin.c              ← Dahili komutu seçer ve çalıştırır
    ├── command_executors.c         ← Tek komut: harici veya dahili ayrımı
    ├── command_handlers.c          ← Komut çalıştırma üst düzey yönetimi
    │
    │   ── Dahili Komutlar (Builtins) ──
    ├── builtin.c                   ← echo, cd, pwd, exit implementasyonları
    ├── builtin_utils.c             ← Dahili komut yardımcıları
    ├── ft_export.c                 ← export komutu implementasyonu
    ├── ft_export_two.c             ← export yardımcı fonksiyonları
    ├── ft_unset.c                  ← unset komutu implementasyonu
    ├── sort_env.c                  ← export çıktısı için env sıralama
    │
    │   ── Ortam Değişkenleri ──
    ├── env.c                       ← env komutu, cd için env yönetimi
    ├── env_utils.c                 ← get/set/update ortam değişken işlemleri
    │
    │   ── Bellek Yönetimi ──
    ├── memory.c                    ← ft_malloc, ft_free, ft_mem_cleanup (özel allocator)
    ├── cleanup.c                   ← Shell ve komut yapıları serbest bırakma
    ├── cleanup_utils.c             ← Temizlik yardımcı fonksiyonları
    │
    │   ── Sinyaller ──
    ├── signals.c                   ← Sinyal kurulumu (normal, heredoc, komut modları)
    ├── signal_utils.c              ← Sinyal işleyici fonksiyonlar
    │
    │   ── Okuma / Yardımcılar ──
    ├── read_content.c              ← Heredoc için pipe'tan içerik okuma
    └── child_process.c             ← create_child_process() (fork sarmalayıcı)
```

---

## 👤 Yazar

| Kullanıcı | Okul |
|---|---|
| yayiker | 42 |

---

*Bu proje Ecole 42 müfredatı kapsamında eğitim amaçlı geliştirilmiştir.*
