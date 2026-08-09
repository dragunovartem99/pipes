# Pipes

<img width="1920" height="1080" src="https://github.com/user-attachments/assets/19ed7541-aaa3-47c4-b78b-8c2032304276" />

## Workflows

### CI (`ci.yaml`)

Runs the given npm scripts as parallel jobs, one per check

```yaml
jobs:
    pipes:
        uses: dragunovartem99/pipes/.github/workflows/ci.yaml@main
        with:
            checks: '["format:check", "types:check", "lint:check", "test"]'
            node-version: "24"  # optional, defaults to the runner's preinstalled Node
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
            build-command: build
            dist-folder: ./dist
        secrets:
            build-env: '{"API_URL": "${{ secrets.API_URL }}"}'  # optional, exposed as env vars to the build step
```

### Release (`release.yaml`)

Opens a release PR while changesets are pending, then publishes to npm and creates the git tag and
GitHub release once that PR is merged

> The caller workflow must grant the required permissions:
> ```txt
> permissions:
>     contents: write
>     pull-requests: write
>     id-token: write
> ```

```yaml
on:
    push:
        branches: ["main"]

jobs:
    pipes:
        uses: dragunovartem99/pipes/.github/workflows/release.yaml@main
        with:
            node-version: "24"  # optional, defaults to the runner's preinstalled Node
        secrets:
            npm-token: ${{ secrets.NPM_TOKEN }}  # optional, omit when the package uses trusted publishing
```

Authenticate one of two ways:

- **Trusted publishing (OIDC)** — omit `npm-token` and set the package's trusted publisher on npmjs.com
  to this repo and workflow. Nothing to store or rotate; the workflow updates npm to a version that
  supports it
- **Token** — add a granular automation token as the `NPM_TOKEN` secret and pass it as above
