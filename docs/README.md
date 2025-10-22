# Bodge Documentation

This directory contains the source files for the Bodge documentation website, hosted on GitHub Pages.

## 🎨 Modern Design Features

The documentation features a modern, user-friendly design with:
- **Sidebar navigation** with icons for easy browsing
- **Dark mode toggle** (🌙/☀️) with automatic OS preference detection
- **Responsive design** optimized for mobile, tablet, and desktop
- **Card-based layouts** for better visual hierarchy
- **Smooth animations** and transitions
- **Clean, modern typography** for excellent readability

## Files

- `index.md` - Homepage
- `getting-started.md` - Installation and first steps
- `configuration.md` - Complete configuration reference
- `examples.md` - Real-world usage examples
- `contributing.md` - Contribution guidelines
- `_config.yml` - Jekyll configuration for GitHub Pages
- `_layouts/default.html` - Custom layout with sidebar navigation
- `assets/css/style.scss` - Custom styles with dark mode support

## Local Development

To run the documentation locally:

1. Install Jekyll:
   ```bash
   gem install bundler jekyll
   ```

2. Create a Gemfile in this directory:
   ```ruby
   source "https://rubygems.org"
   gem "github-pages", group: :jekyll_plugins
   ```

3. Install dependencies:
   ```bash
   bundle install
   ```

4. Run the local server:
   ```bash
   bundle exec jekyll serve
   ```

5. Open http://localhost:4000/bodge in your browser

## Adding New Pages

When creating new documentation pages:

1. **Create a new `.md` file** in the `docs/` directory
2. **Add YAML front matter:**
   ```yaml
   ---
   layout: default
   title: Your Page Title
   ---
   ```
3. **Write your content** in Markdown
4. **Update the sidebar navigation** in `_layouts/default.html` if needed

### Using Feature Cards

For modern card-based layouts:

```html
<div class="feature-grid">
  <div class="feature-card">
    <h3>🚀 Feature Title</h3>
    <p>Feature description goes here.</p>
    <a href="link" class="btn">Learn More →</a>
  </div>
</div>
```

## Theme Customization

The theme uses CSS custom properties for easy customization. Edit `assets/css/style.scss` to modify colors, spacing, typography, and dark mode appearance.

## Deployment

The site is automatically deployed to GitHub Pages when changes are pushed to the main branch.

GitHub Pages URL: https://el-dockerr.github.io/bodge