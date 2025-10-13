# GitHub Pages Setup Instructions

## 1. Enable GitHub Pages

1. Go to your repository on GitHub: https://github.com/el-dockerr/bodge
2. Click on **Settings** tab
3. Scroll down to **Pages** section in the left sidebar
4. Under **Source**, select:
   - **Source**: Deploy from a branch
   - **Branch**: main
   - **Folder**: /docs
5. Click **Save**

## 2. Wait for Deployment

- GitHub will automatically build and deploy your site
- The first deployment may take 5-10 minutes
- Your site will be available at: **https://el-dockerr.github.io/bodge**

## 3. Verify the Site

Once deployed, your site will include:

- **Homepage**: Overview and features
- **Getting Started**: Installation and first steps  
- **Configuration**: Complete reference for .bodge files
- **Examples**: Real-world usage examples
- **Contributing**: Guidelines for contributors

## 4. Custom Domain (Optional)

If you want to use a custom domain:

1. In the Pages settings, add your custom domain
2. Create a CNAME file in the docs/ directory with your domain
3. Configure DNS with your domain provider

## 5. Automatic Updates

The site will automatically update when you push changes to the `docs/` folder in the main branch.

## Troubleshooting

If the site doesn't build:

1. Check the **Actions** tab for build errors
2. Ensure all Markdown files have proper frontmatter
3. Check Jekyll build logs for specific errors

Your site should be live at: https://el-dockerr.github.io/bodge