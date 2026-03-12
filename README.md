# Pipes

Reusable GitHub workflows for CI/CD

<img width="1920" height="1080" src="https://github.com/user-attachments/assets/19ed7541-aaa3-47c4-b78b-8c2032304276" />

## Workflows

### CI (`ci.yaml`)

Runs format, types, lint, and test checks. All steps are enabled by default and can be individually disabled

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

Builds and deploys a static site to GitHub Pages

> The caller workflow must grant the required permissions:
> ```txt
> permissions:
>     contents: read
>     pages: write
>     id-token: write
> ```

```yaml
jobs:
    pipes:
        uses: dragunovartem99/pipes/.github/workflows/deploy.yaml@main
        with:
            build-command: build  # default
            dist-folder: ./dist   # default
            node-version: lts/*   # default
```
