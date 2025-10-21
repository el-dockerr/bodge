# Windows Terminal Setup for Unicode Display

If you're experiencing issues with the ASCII art or emojis not displaying correctly in PowerShell or Windows CMD, here are solutions:

## The Fix (Automatic)

Bodge now automatically sets UTF-8 encoding when it starts. The latest version should work out of the box!

## If You Still Have Issues

### Option 1: Use Windows Terminal (Recommended)
The new Windows Terminal has excellent Unicode support:

1. Install from Microsoft Store: [Windows Terminal](https://www.microsoft.com/store/productId/9N0DX20HK701)
2. Open Windows Terminal and run Bodge - everything should work perfectly!

### Option 2: Configure PowerShell
Add this to your PowerShell profile to always use UTF-8:

```powershell
# Find your profile location
$PROFILE

# Edit the profile (create if it doesn't exist)
notepad $PROFILE

# Add this line to the file:
[Console]::OutputEncoding = [System.Text.Encoding]::UTF8
```

### Option 3: Set CMD Code Page
For Command Prompt, run this before using Bodge:

```cmd
chcp 65001
```

To make it permanent, add it to a batch file that you run before using Bodge.

### Option 4: Use a Better Font
Some fonts don't support all Unicode characters. Try these fonts:
- **Cascadia Code** (recommended, comes with Windows Terminal)
- **Consolas** (built into Windows)
- **DejaVu Sans Mono**
- **JetBrains Mono**

To change font in CMD:
1. Right-click the title bar
2. Select "Properties"
3. Go to "Font" tab
4. Choose a Unicode-compatible font

## Testing Unicode Support

Run this command to test if Unicode is working:
```bash
bodge version
```

You should see:
- Box drawing characters: ╔ ║ ╚ ╗ ═
- Emojis: 🔨 📦 ⚙️ 🎯
- Special symbols: ✓ ✗ ℹ ⚠

## What Bodge Does Automatically

On startup, Bodge automatically:
1. Sets Windows console output to UTF-8 (code page 65001)
2. Enables ANSI color support for Windows 10+
3. Detects if output is a terminal (TTY)
4. Disables colors if output is redirected to a file

## Fallback Mode

If you're using a very old terminal or redirecting output, Bodge will:
- Still show all information clearly
- Disable ANSI colors when not in a terminal
- Maintain functionality without visual enhancements

## Common Issues

### Issue: Question marks or boxes instead of emojis
**Solution**: Use a font that supports emojis (like Cascadia Code or Segoe UI Emoji)

### Issue: Colors don't work
**Solution**: Make sure you're on Windows 10 or later. Windows 7/8 has limited color support.

### Issue: Box drawing characters are broken
**Solution**: 
1. Try `chcp 65001` before running Bodge
2. Ensure your terminal font supports Unicode
3. Use Windows Terminal instead of legacy CMD

### Issue: Everything looks weird in PowerShell ISE
**Solution**: PowerShell ISE doesn't support ANSI colors well. Use regular PowerShell or Windows Terminal instead.

## Environment Detection

Bodge automatically detects your environment:
- ✅ **Windows Terminal** - Full support
- ✅ **PowerShell 7+** - Full support
- ✅ **PowerShell 5** - Full support (with UTF-8 fix)
- ✅ **CMD.exe** - Full support (with UTF-8 fix)
- ⚠️ **PowerShell ISE** - Limited color support
- ⚠️ **Git Bash/MSYS2** - Should work but may need font adjustment
- ✅ **ConEmu/Cmder** - Full support

## Advanced: Permanent UTF-8 Setup

### For PowerShell (All Users)
Create/edit: `C:\Windows\System32\WindowsPowerShell\v1.0\profile.ps1`

```powershell
[Console]::OutputEncoding = [System.Text.Encoding]::UTF8
$PSDefaultParameterValues['Out-File:Encoding'] = 'utf8'
```

### For CMD (Registry Method)
1. Open Registry Editor (regedit)
2. Navigate to: `HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Command Processor`
3. Create new String Value: `Autorun`
4. Set value to: `chcp 65001 >nul`

**Warning**: This affects all CMD instances. Test first!

## Still Not Working?

If you're still having issues:

1. **Check your Windows version**: Run `winver` to see your version
2. **Update Windows**: Some fixes require Windows 10 build 1903 or later
3. **Try a different terminal**: Windows Terminal is the best option
4. **Check antivirus**: Some security software interferes with console settings
5. **Run as Administrator**: Some console settings require admin rights

## Report Issues

If none of these solutions work, please report an issue with:
- Your Windows version (`winver`)
- Your terminal application
- A screenshot of the problem
- Output of: `[Console]::OutputEncoding` in PowerShell

---

*The visual enhancements should work on any modern Windows system!* 🎨

