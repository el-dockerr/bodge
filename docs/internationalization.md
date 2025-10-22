# Internationalization (i18n) Support

## Overview

Bodge now includes full internationalization support with automatic language detection. The system automatically detects your operating system language and displays all messages in the appropriate language.

## Supported Languages

- **English** (Default)
- **简体中文 (Simplified Chinese)**

## How It Works

### Automatic Language Detection

The build system automatically detects your system language when it starts:

**Windows:**
- Uses `GetSystemDefaultLCID()` to detect the system locale
- Automatically switches to Chinese if system language is set to Chinese

**Linux/macOS/Unix:**
- Checks the `LANG` environment variable
- Recognizes: `zh_CN`, `zh_HANS`, or any language string containing "chinese"
- Falls back to English for all other locales

### Manual Language Selection

You can override the automatic detection by modifying the language at runtime (future feature) or by setting your system language.

## Examples

### English Output
```
Bodge - The Idiotic Build System

Usage: bodge [command] [target/sequence] [options]

Commands:
  build [target]     - Build specific target (default: all targets)
  fetch              - Fetch git dependencies only
  ...
```

### Chinese Output (中文输出)
```
Bodge - 简易构建系统

用法：bodge [命令] [目标/序列] [选项]

命令：
  build [目标]       - 构建指定目标（默认：所有目标）
  fetch              - 仅获取git依赖项
  ...
```

## Testing Language Detection

### On Windows

1. **To test Chinese:**
   ```powershell
   # Change Windows display language to Chinese
   # Settings > Time & Language > Language > Windows display language
   # Select "中文(简体，中国)" or similar
   # Restart application
   ```

2. **To test English:**
   ```powershell
   # Set display language back to English
   # Restart application
   ```

### On Linux/macOS

1. **To test Chinese:**
   ```bash
   export LANG=zh_CN.UTF-8
   ./bodge help
   ```

2. **To test English:**
   ```bash
   export LANG=en_US.UTF-8
   ./bodge help
   ```

## Translation Coverage

All user-facing strings are translated, including:

✅ **Application Information**
- Title, version, author information

✅ **Commands and Help Text**
- All command descriptions
- Option descriptions
- Usage information

✅ **Build Messages**
- Build started/completed/failed
- Target status messages
- Progress indicators

✅ **Dependency Management**
- Git clone/pull messages
- Dependency fetch status

✅ **Error Messages**
- Configuration errors
- Command errors
- File operation errors

✅ **Warning Messages**
- Invalid arguments
- Configuration warnings

✅ **Daemon Mode**
- File watcher messages
- Rebuild notifications

## Architecture

### String Management System

```
src/Strings.h          - String identifiers and API
src/Strings.cpp        - Language detection and translations
```

### Key Components

1. **StringID Enum:** Unique identifier for each translatable string
2. **Language Enum:** Supported languages
3. **Strings Class:** Manages translations and language detection
4. **STR() Macro:** Convenient string retrieval

### Usage in Code

```cpp
#include "Strings.h"

// Initialize (called once in main.cpp)
Strings::initialize();

// Get a translated string
std::cout << STR(APP_TITLE) << std::endl;

// Get translated string with variable
std::cout << STR(MSG_TARGET_PLATFORM) << platform.to_string() << std::endl;
```

## Adding New Strings

To add a new translatable string:

1. **Add to StringID enum in `Strings.h`:**
   ```cpp
   enum class StringID {
       // ... existing entries
       MY_NEW_STRING,
   };
   ```

2. **Add translations in `Strings.cpp`:**
   ```cpp
   void Strings::load_translations() {
       auto& en = translations_[Language::ENGLISH];
       auto& zh = translations_[Language::CHINESE_SIMPLIFIED];
       
       // ... existing translations
       
       en[StringID::MY_NEW_STRING] = "My English text";
       zh[StringID::MY_NEW_STRING] = "我的中文文本";
   }
   ```

3. **Use in code:**
   ```cpp
   std::cout << STR(MY_NEW_STRING) << std::endl;
   ```

## Adding New Languages

To add support for a new language:

1. **Add language to enum in `Strings.h`:**
   ```cpp
   enum class Language {
       ENGLISH,
       CHINESE_SIMPLIFIED,
       FRENCH,  // New language
   };
   ```

2. **Update detection in `Strings.cpp`:**
   ```cpp
   Language Strings::detect_system_language() {
       // Add detection logic for new language
       if (lang_str.find("fr_") != std::string::npos) {
           return Language::FRENCH;
       }
   }
   ```

3. **Add translations in `load_translations()`:**
   ```cpp
   auto& fr = translations_[Language::FRENCH];
   fr[StringID::APP_TITLE] = "Bodge - Le Système de Construction Idiote";
   // ... add all translations
   ```

## Performance

- **Initialization:** One-time cost at startup (~1-2ms)
- **String Lookup:** O(log n) map lookup (negligible)
- **Memory:** ~50KB for both English and Chinese translations
- **Runtime Overhead:** Minimal, no impact on build performance

## Best Practices

1. **Always use STR() macro** for user-facing strings
2. **Never hardcode** user messages in source files
3. **Keep translations consistent** in tone and style
4. **Test both languages** when adding new features
5. **Use descriptive StringID names** for maintainability

## Technical Details

### Language Detection Priority

1. Windows: System default LCID
2. Unix-like: `LANG` environment variable
3. Fallback: English (always available)

### Thread Safety

The Strings class is thread-safe after initialization:
- `initialize()` should be called once at startup
- `get()` operations are read-only and safe for concurrent access
- No locking required after initialization

### Encoding

All strings are stored in UTF-8 encoding:
- Chinese characters: UTF-8 encoded
- Console output: Automatically configured for UTF-8 on Windows
- Cross-platform compatibility guaranteed

## Future Enhancements

Potential future improvements:

- [ ] Runtime language switching
- [ ] Configuration file language override
- [ ] Additional languages (Japanese, Korean, German, French, Spanish)
- [ ] Plural forms support
- [ ] Date/time localization
- [ ] Number formatting localization
- [ ] External translation files (JSON/XML)

## Troubleshooting

### Chinese characters not displaying correctly

**Windows:**
```powershell
# Run the Unicode fix script
.\scripts\fix-console-unicode.bat
```

**Linux/macOS:**
```bash
# Ensure UTF-8 locale is set
export LANG=zh_CN.UTF-8
export LC_ALL=zh_CN.UTF-8
```

### Language not detected automatically

Check your system language settings:
- Windows: Settings > Time & Language > Language
- Linux: `echo $LANG` and `locale` commands
- macOS: System Preferences > Language & Region

### Fallback to English

If the system language is not Chinese, the system will automatically use English. This is expected behavior and ensures the application always displays readable text.

---

## Example Session

### English System
```bash
$ ./bodge help
Bodge - The Idiotic Build System

Usage: bodge [command] [target/sequence] [options]

Commands:
  build [target]     - Build specific target (default: all targets)
  fetch              - Fetch git dependencies only
  sequence [name]    - Execute specific sequence
  watch              - Watch mode: automatically rebuild on file changes
  ...
```

### Chinese System (中文系统)
```bash
$ ./bodge help
Bodge - 简易构建系统

用法：bodge [命令] [目标/序列] [选项]

命令：
  build [目标]       - 构建指定目标（默认：所有目标）
  fetch              - 仅获取git依赖项
  sequence [名称]    - 执行指定序列
  watch              - 监视模式：文件更改时自动重新构建
  ...
```

---

**Version:** 1.0.1-rc1  
**Last Updated:** October 22, 2025

