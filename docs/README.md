# Bodge Documentation

This directory contains the source files for the Bodge documentation website, hosted on GitHub Pages.

## Files

- `index.md` - Homepage
- `getting-started.md` - Installation and first steps
- `configuration.md` - Complete configuration reference
- `examples.md` - Real-world usage examples
- `contributing.md` - Contribution guidelines
- `_config.yml` - Jekyll configuration for GitHub Pages

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

## Deployment

The site is automatically deployed to GitHub Pages when changes are pushed to the main branch.

GitHub Pages URL: https://el-dockerr.github.io/bodge