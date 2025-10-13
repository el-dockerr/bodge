# 🔧 GitHub Pages Setup Guide

## Quick Fix: Enable GitHub Pages

### Option 1: Enable GitHub Actions Deployment (Recommended)

1. **Go to Repository Settings**
   - Navigate to https://github.com/el-dockerr/bodge
   - Click the **Settings** tab

2. **Configure Pages**
   - In the left sidebar, click **Pages**
   - Under **Source**, select **GitHub Actions**
   - No need to specify a branch or folder

3. **Trigger Deployment**
   - Go to the **Actions** tab
   - Click on **Deploy Documentation to GitHub Pages**
   - Click **Run workflow** → **Run workflow**

### Option 2: Branch Deployment (Alternative)

1. **Go to Repository Settings**
   - Navigate to https://github.com/el-dockerr/bodge
   - Click the **Settings** tab

2. **Configure Pages**
   - In the left sidebar, click **Pages**
   - Under **Source**, select **Deploy from a branch**
   - Choose **main** branch
   - Choose **/ (root)** folder (not /docs)
   - Click **Save**

## 🚀 Your Site Will Be Available At:
**https://el-dockerr.github.io/bodge**

## 🛠️ Troubleshooting

### If you get "Not Found" errors:

1. **Check Repository Visibility**
   - Ensure your repository is **public** (required for free GitHub Pages)
   - Or you have GitHub Pro/Team for private repo pages

2. **Verify Settings**
   - Go to Settings → Pages
   - Ensure Pages is enabled
   - Check the source is set correctly

3. **Wait for Build**
   - First deployment can take 5-10 minutes
   - Check Actions tab for build status

### If builds fail:

1. **Check Actions Tab**
   - Look for error messages in workflow runs
   - Common issues: missing files, syntax errors

2. **Verify File Structure**
   ```
   docs/
   ├── index.md
   ├── _config.yml
   ├── Gemfile
   └── _layouts/
   ```

3. **Test Locally** (Optional)
   ```bash
   cd docs
   bundle install
   bundle exec jekyll serve
   ```

## 🎯 What You Get

Once setup is complete, your documentation site will include:

- **Professional Homepage** with features overview
- **Getting Started Guide** with installation steps
- **Configuration Reference** for .bodge files
- **Real-World Examples** for different project types
- **Contribution Guidelines** for developers
- **Automatic Updates** when you push changes

## 📞 Need Help?

If you're still having issues:

1. Check the **Actions** tab for detailed error messages
2. Ensure your repository is public
3. Verify all files were pushed to the main branch
4. Try the manual workflow trigger in Actions tab

The site should be live within 10 minutes of enabling Pages! 🎉