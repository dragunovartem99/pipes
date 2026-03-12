# Pipes

Reusable GitHub workflows for CI/CD

<img src="https://github.com/user-attachments/assets/741f0a1f-2791-4933-9bcc-996d4cf3d25f" width="320" />

## Workflows

### CI (`ci.yaml`)

Runs format, types, lint, and test checks. All steps are enabled by default and can be individually disabled.

```yaml
jobs:
    pipes:
        uses: dragunovartem99/pipes/.github/workflows/ci.yaml@main
        with:
            run-format: true  # default
            run-types: true   # default
            run-lint: true    # default
            run-test: true    # default
```

### Deploy (`deploy.yaml`)

Builds and deploys a static site to GitHub Pages.

```yaml
jobs:
    pipes:
        uses: dragunovartem99/pipes/.github/workflows/deploy.yaml@main
        with:
            build-command: build  # default
            dist-folder: ./dist   # default
            node-version: lts/*   # default
```
